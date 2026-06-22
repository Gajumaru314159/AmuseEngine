//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/DirectX12RHI/Buffer/DirectX12Buffer.h>
#include <Amuse/DirectX12RHI/DirectX12Device.h>
#include <Amuse/DirectX12RHI/Utility/Utility.h>
#include <Amuse/DirectX12RHI/Utility/TypeConverter.h>
#include <Amuse/DirectX12RHI/Buffer/SmallBufferAllocator.h>
#include <Amuse/RHI/Types/CommandParam.h>

namespace Amuse::RHI {
    using namespace Amuse::Core;

	//! @brief  BufferFlags を D3D12_RESOURCE_FLAGS に変換
	static D3D12_RESOURCE_FLAGS Convert(BufferFlags value) {
		D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE;
		if (!value.has(BufferFlag::ShaderResource)) flags |= D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;
		if (value.has(BufferFlag::UnorderedAccess))flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

		// D3D12では使用しない
		// if (value.has(BufferFlag::CopySource));
		// if (value.has(BufferFlag::CopyDest));
		// if (value.has(BufferFlag::Vertex));
		// if (value.has(BufferFlag::Index));
		// if (value.has(BufferFlag::Constant));
		// if (value.has(BufferFlag::IndirectArgument));

		return flags;
	}

	//! @brief  CPUアクセス専用バッファか
	static bool IsCpuAccessBuffer(const BufferDesc& desc) {
		return
			desc.flags == BufferFlag::CopySource ||
			desc.flags == BufferFlag::CopyDest;
	}

	//! @brief TypedBufferとして扱うか
	static bool IsTypedBuffer(BufferViewType type) {
		return type == BufferViewType::Buffer || type == BufferViewType::RWBuffer;
	}

	//! @brief StructuredBufferとして扱うか
	static bool IsStructuredBuffer(BufferViewType type) {
		return type == BufferViewType::StructuredBuffer || type == BufferViewType::RWStructuredBuffer;
	}

	//! @brief RawBufferとして扱うか
	static bool IsRawBuffer(BufferViewType type) {
		return type == BufferViewType::ByteAddressBuffer || type == BufferViewType::RWByteAddressBuffer;
	}

	//! @brief TextureFormatのバイトサイズを取得
	static u32 GetTextureFormatByteSize(TextureFormat format) {
		switch (format) {
		case TextureFormat::SDR:
		case TextureFormat::RGBA8:
		case TextureFormat::RGBA8_SRGB:
			return 4;
		case TextureFormat::HDR:
		case TextureFormat::RGBA16:
			return 8;
		case TextureFormat::RGBA32:
			return 16;
		case TextureFormat::RGB32:
			return 12;
		case TextureFormat::RGB8:
			return 3;
		case TextureFormat::RG32:
			return 8;
		case TextureFormat::RG16:
			return 4;
		case TextureFormat::RG8:
			return 2;
		case TextureFormat::R32:
			return 4;
		case TextureFormat::R16:
			return 2;
		case TextureFormat::R8:
			return 1;
		default:
			return 0;
		}
	}

	//! @brief 間接引数をバッファに書き込む
	template<class TArgument, class TParam, class TWriter>
	void WriteIndirectArguments(DirectX12Buffer& buffer, Span<const TParam> params, u64 offset, u32 stride, TWriter&& writer) {
		if (params.empty() || stride == 0) return;

		Vector<u8> bytes(static_cast<size_t>(stride) * params.size());
		for (size_t i = 0; i < params.size(); ++i) {
			TArgument argument{};
			writer(argument, params[i]);
			std::memcpy(bytes.data() + static_cast<size_t>(stride) * i, &argument, std::min<size_t>(sizeof(argument), stride));
		}

		if (auto mapped = buffer.map(CpuAccess::Write)) {
			std::memcpy(static_cast<u8*>(mapped) + offset, bytes.data(), bytes.size());
			buffer.unmap();
		} else {
			buffer.update(bytes.size(), bytes.data(), offset);
		}
	}


	//! @brief  コンストラクタ
	DirectX12Buffer::DirectX12Buffer(DirectX12Device& device, const BufferDesc& desc)
		: m_device(device)
		, m_desc(desc)
	{
		if (!m_desc.isValid()) return;

		// SmallBufferAllocatorを使用できるかチェック
		auto canUseSmallAllocator =
			!IsCpuAccessBuffer(desc) &&
			(desc.flags & BufferFlag::UnorderedAccess) &&
			(1 < BitOp::GetBitCount(static_cast<u32>(desc.flags))) &&
			(desc.size > 65536);

		// SmallBufferAllocatorを試す
		if (canUseSmallAllocator) {
			auto& allocator = device.getSmallBufferAllocator(desc.flags.get_enum());
			size_t alignment = SmallBufferAllocator::GetAlignmentFromUsage(desc);
			auto allocation = allocator.allocate(desc.size, alignment);

			if (allocation.resource) {
				m_resource = allocation.resource;
				m_allocation = allocation;
			}
		}

		// SmallBufferAllocatorを使用しない、またはサブアロケーションに失敗した場合は個別リソース生成
		if (!m_resource) {

			HRESULT result;

			D3D12_HEAP_TYPE heapType = D3D12_HEAP_TYPE_DEFAULT;
			if (desc.flags == BufferFlag::CopySource) heapType = D3D12_HEAP_TYPE_UPLOAD;
			if (desc.flags == BufferFlag::CopyDest) heapType = D3D12_HEAP_TYPE_READBACK;

			D3D12_HEAP_PROPERTIES heapprop = CD3DX12_HEAP_PROPERTIES(heapType);

			D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON;
			if (heapType == D3D12_HEAP_TYPE_UPLOAD) state = D3D12_RESOURCE_STATE_GENERIC_READ;
			if (heapType == D3D12_HEAP_TYPE_READBACK) state = D3D12_RESOURCE_STATE_COPY_DEST;

			UINT64 alignment = 0; // TODO PlacedBuffer対応時に設定する
			D3D12_RESOURCE_FLAGS flags = Convert(m_desc.flags);
			D3D12_RESOURCE_DESC resdesc = CD3DX12_RESOURCE_DESC::Buffer(m_desc.size,flags, alignment);

			// TODO ReadBack対応

			ComPtr<ID3D12Resource> buffer;
			result = device.getNative()->CreateCommittedResource(&heapprop,D3D12_HEAP_FLAG_NONE,&resdesc,state,nullptr,IID_PPV_ARGS(buffer.GetAddressOf()));

			if (FAILED(result))
			{
				Utility::OutputFatalLog(result, "ID3D12Device::CreateCommittedResource()");
				return;
			}

			m_resource = buffer;
			Utility::SetName(m_resource.Get(), m_desc.name);
		}

		// BufferViewの生成
		if (desc.flags.has(BufferFlag::Constant)) {
			device.allocateHandle(DescriptorHeapType::CBV_SRV_UAV, m_handle, 1);
			m_viewDesc.type = BufferViewType::ConstantBuffer;
			createCBV(m_handle.getCpuHandle());
		} else if (desc.flags.has(BufferFlag::ShaderResource)) {
			device.allocateHandle(DescriptorHeapType::CBV_SRV_UAV, m_handle, 1);
			m_viewDesc.type = 0 < desc.stride ? BufferViewType::StructuredBuffer : BufferViewType::ByteAddressBuffer;
			if (!createSRV(m_handle.getCpuHandle())) {
				m_resource = nullptr;
				return;
			}
		} else if (desc.flags.has(BufferFlag::UnorderedAccess)) {
			device.allocateHandle(DescriptorHeapType::CBV_SRV_UAV, m_handle, 1);
			m_viewDesc.type = 0 < desc.stride ? BufferViewType::RWStructuredBuffer : BufferViewType::RWByteAddressBuffer;
			if (!createUAV(m_handle.getCpuHandle())) {
				m_resource = nullptr;
				return;
			}
		} else {
			m_viewDesc.type = BufferViewType::None;
		}

		manage();
	}


	//! @brief  デストラクタ
	DirectX12Buffer::~DirectX12Buffer() {
		// サブアロケーションの場合は解放処理を行う
		if (m_allocation.allocator) {
			m_allocation.allocator->free(m_allocation);
			m_allocation.allocator = nullptr;
		}
	}


	//! @brief  コンストラクタ
	DirectX12Buffer::DirectX12Buffer(DirectX12Device& device, const BufferDesc& desc, const Blob& blob)
		: DirectX12Buffer(device,desc)
	{
		update(blob.size(), blob.data(), 0);
	}

	//! @brief  コンストラクタ
	DirectX12Buffer::DirectX12Buffer(DirectX12Device& device, const BufferViewDesc& desc)
		: m_device(device)
		, m_viewDesc(desc)
		, m_allocation(nullptr)
	{
		auto base = desc.base.cast<DirectX12Buffer>();

		if (base == nullptr) {
			LOG_ERROR("ベースバッファが指定されていません");
			return;
		}

		m_desc = base->m_desc;

		if (!m_desc.isValid()) return;

		m_resource = base->m_resource;
		m_allocation = base->m_allocation;
		m_allocation.allocator = nullptr; // ベースバッファに解放を任せる

		device.allocateHandle(DescriptorHeapType::CBV_SRV_UAV, m_handle, 1);

		auto flags = m_desc.flags;

		bool hasView = false;

		switch (desc.type) {
		case BufferViewType::ConstantBuffer:
			if (flags.has(BufferFlag::Constant)) {
				createCBV(m_handle.getCpuHandle());
				hasView = true;
			}
			break;
		case BufferViewType::Buffer:
			if (flags.has(BufferFlag::ShaderResource)) {
				if (desc.format == TextureFormat::Unknown) {
					LOG_ERROR("BufferViewType::Bufferにはformatの指定が必要です");
					return;
				}
				hasView = createSRV(m_handle.getCpuHandle());
			}
			break;
		case BufferViewType::RWBuffer:
			if (flags.has(BufferFlag::UnorderedAccess)) {
				if (desc.format == TextureFormat::Unknown) {
					LOG_ERROR("BufferViewType::RWBufferにはformatの指定が必要です");
					return;
				}
				hasView = createUAV(m_handle.getCpuHandle());
			}
			break;
		case BufferViewType::StructuredBuffer:
		case BufferViewType::ByteAddressBuffer:
			if (flags.has(BufferFlag::ShaderResource)) {
				hasView = createSRV(m_handle.getCpuHandle());
			}
			break;
		case BufferViewType::RWStructuredBuffer:
		case BufferViewType::RWByteAddressBuffer:
			if (flags.has(BufferFlag::UnorderedAccess)) {
				hasView = createUAV(m_handle.getCpuHandle());
			}
			break;
		default:
			AMUSE_ABORT("未実装のBufferViewType");
			break;
		}

		if(!hasView) {
			// Viewの生成に失敗しているのでInvalid状態にする
			m_resource = nullptr;
		}

		manage();
	}


	//! @brief  妥当な状態か
	bool DirectX12Buffer::isValid()const {
		return m_resource;
	}


	//! @brief      名前を取得
	const String& DirectX12Buffer::getName()const {
		return m_desc.name;
	}


	//! @brief  定義を取得
	const BufferDesc& DirectX12Buffer::getDesc()const {
		return m_desc;
	}


	//! @brief      バッファを更新
	//!
	//! @details    バッファの更新は描画スレッドの直前にまとめて行われます。
	void DirectX12Buffer::update(size_t size, const void* pData, size_t offset) {
		if (pData == nullptr) return;
		m_device.getBufferUploader().add(BlobView(pData, size), m_resource, offset + m_allocation.offset, TypeConverter::Convert(m_desc.state));
	}


	//! @brief      バッファを更新
	//!
	//! @details    バッファの更新は描画スレッドの直前にまとめて行われます。
	void DirectX12Buffer::update(const CopyFunc& func) {
		if (!func) return;
		m_device.getBufferUploader().add(func, m_desc.size, m_resource, m_allocation.offset, TypeConverter::Convert(m_desc.state));
	}


	//! @brief      バッファを更新(直接更新)
	//!
	//! @details    CPUアクセス可能なバッファをmap / unmapで即時更新します。
	void DirectX12Buffer::updateDirect(size_t size, const void* data, size_t offset) {
		if (data == nullptr) return;
		auto mapped = map(CpuAccess::Write);
		if (!mapped) {
			LOG_ERROR("CPUアクセスできないBufferをupdateDirectで更新しようとしました [name={}]", m_desc.name);
			return;
		}
		std::memcpy(static_cast<u8*>(mapped) + offset, data, size);
		unmap();
	}


	//! @brief      バッファを更新(直接更新)
	//!
	//! @details    CPUアクセス可能なバッファをmap / unmapで即時更新します。
	void DirectX12Buffer::updateDirect(const CopyFunc& func){
		if (!func) return;
		auto mapped = map(CpuAccess::Write);
		if (!mapped) {
			LOG_ERROR("CPUアクセスできないBufferをupdateDirectで更新しようとしました [name={}]", m_desc.name);
			return;
		}
		func(mapped);
		unmap();
	}

	//! @brief      CPUからバッファをマップ
	void* DirectX12Buffer::map(CpuAccess access) {
		if (!m_resource || m_allocation.allocator) return nullptr;

		m_mapAccess = access;
		D3D12_RANGE readRange{};
		if (access == CpuAccess::Read || access == CpuAccess::ReadWrite) {
			readRange.Begin = 0;
			readRange.End = static_cast<SIZE_T>(m_desc.size);
		}

		void* mapped = nullptr;
		if (FAILED(m_resource->Map(0, &readRange, &mapped))) {
			return nullptr;
		}
		return static_cast<std::byte*>(mapped) + m_allocation.offset;
	}

	//! @brief      CPUからのバッファマップを解除
	void DirectX12Buffer::unmap() {
		if (!m_resource || m_allocation.allocator) return;

		D3D12_RANGE writeRange{};
		if (m_mapAccess == CpuAccess::Write || m_mapAccess == CpuAccess::ReadWrite) {
			writeRange.Begin = static_cast<SIZE_T>(m_allocation.offset);
			writeRange.End = static_cast<SIZE_T>(m_allocation.offset + m_desc.size);
		}
		m_resource->Unmap(0, &writeRange);
	}

	//! @brief drawIndirect() 用の引数を一括で書き込む
	void DirectX12Buffer::writeDrawParams(Span<const DrawParam> params, u64 offset) {
		WriteIndirectArguments<D3D12_DRAW_ARGUMENTS>(*this, params, offset, m_device.getDrawIndirectArgumentSize(), [](D3D12_DRAW_ARGUMENTS& argument, const DrawParam& param) {
			argument.VertexCountPerInstance = param.vertexCount;
			argument.InstanceCount = param.instanceCount;
			argument.StartVertexLocation = param.startVertex;
			argument.StartInstanceLocation = param.startInstance;
		});
	}

	//! @brief drawIndexedIndirect() 用の引数を一括で書き込む
	void DirectX12Buffer::writeDrawIndexedParams(Span<const DrawIndexedParam> params, u64 offset) {
		WriteIndirectArguments<D3D12_DRAW_INDEXED_ARGUMENTS>(*this, params, offset, m_device.getDrawIndexedIndirectArgumentSize(), [](D3D12_DRAW_INDEXED_ARGUMENTS& argument, const DrawIndexedParam& param) {
			argument.IndexCountPerInstance = param.indexCount;
			argument.InstanceCount = param.instanceCount;
			argument.StartIndexLocation = param.startIndex;
			argument.BaseVertexLocation = static_cast<INT>(param.startVertex);
			argument.StartInstanceLocation = param.startInstance;
		});
	}

	//! @brief dispatchIndirect() 用の引数を一括で書き込む
	void DirectX12Buffer::writeDispatchParams(Span<const DispatchParam> params, u64 offset) {
		WriteIndirectArguments<D3D12_DISPATCH_ARGUMENTS>(*this, params, offset, m_device.getDispatchIndirectArgumentSize(), [](D3D12_DISPATCH_ARGUMENTS& argument, const DispatchParam& param) {
			argument.ThreadGroupCountX = param.groupX;
			argument.ThreadGroupCountY = param.groupY;
			argument.ThreadGroupCountZ = param.groupZ;
		});
	}


	//! @brief      CBVを生成
	void DirectX12Buffer::createCBV(D3D12_CPU_DESCRIPTOR_HANDLE handle)const {

		AMUSE_ASSERT(m_desc.flags & BufferFlag::Constant, "定数バッファを生成するには、BufferFlag::Constantフラグが必要です。 [name={}]", m_desc.name);

		D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {};
		desc.BufferLocation = m_resource->GetGPUVirtualAddress() + m_allocation.offset;
		desc.SizeInBytes = static_cast<UINT>(m_desc.size);

		m_device.getNative()->CreateConstantBufferView(&desc, handle);

	}


	//! @brief      SRVを生成
	bool DirectX12Buffer::createSRV(D3D12_CPU_DESCRIPTOR_HANDLE handle)const {

		AMUSE_ASSERT(m_desc.flags & BufferFlag::ShaderResource, "SRVを生成するには、BufferFlag::ShaderResourceフラグが必要です。 [name={}]", m_desc.name);

		const bool isTypedBuffer = IsTypedBuffer(m_viewDesc.type);
		const bool isStructuredBuffer = IsStructuredBuffer(m_viewDesc.type) || (!IsRawBuffer(m_viewDesc.type) && 0 < m_desc.stride);
		const u32 typedElementSize = isTypedBuffer ? GetTextureFormatByteSize(m_viewDesc.format) : 0;
		if (isTypedBuffer && typedElementSize == 0) {
			LOG_ERROR("SRVの生成に失敗しました。TypedBufferには有効なformatの指定が必要です。[name={}, format={}]", m_desc.name, enum_cast(m_viewDesc.format));
			return false;
		}
		if (isStructuredBuffer && m_desc.stride == 0) {
			LOG_ERROR("SRVの生成に失敗しました。StructuredBufferにはstrideの指定が必要です。[name={}]", m_desc.name);
			return false;
		}

		// D3D12のバッファSRVは、TypedBuffer / StructuredBuffer / ByteAddressBuffer で
		// Format、StructureByteStride、RAWフラグの組み合わせが排他的に変わる。
		// TypedBufferはformatの要素数、StructuredBufferはstrideの要素数、
		// ByteAddressBufferはR32_TYPELESSを4byte単位のRAW viewとして設定する。
		D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format = isTypedBuffer ? TypeConverter::Convert(m_viewDesc.format, false) : (isStructuredBuffer ? DXGI_FORMAT_UNKNOWN : DXGI_FORMAT_R32_TYPELESS);
		desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		desc.Buffer.FirstElement = static_cast<UINT64>(m_allocation.offset / (isTypedBuffer ? typedElementSize : (isStructuredBuffer ? m_desc.stride : 4)));
		desc.Buffer.NumElements = gsl::narrow<UINT>(isTypedBuffer ? m_desc.size / typedElementSize : (isStructuredBuffer ? m_desc.size/m_desc.stride : m_desc.size/4));
		desc.Buffer.StructureByteStride = isStructuredBuffer ? m_desc.stride : 0;
		desc.Buffer.Flags = isTypedBuffer || isStructuredBuffer ? D3D12_BUFFER_SRV_FLAG_NONE : D3D12_BUFFER_SRV_FLAG_RAW;

		m_device.getNative()->CreateShaderResourceView(m_resource.Get(), &desc, handle);
		return true;

	}


	//! @brief      UAVを生成
	bool DirectX12Buffer::createUAV(D3D12_CPU_DESCRIPTOR_HANDLE handle)const {

		AMUSE_ASSERT(m_desc.flags & BufferFlag::UnorderedAccess, "UAVを生成するには、BufferFlag::UnorderedAccessフラグが必要です。 [name={}]", m_desc.name);

		const bool isTypedBuffer = IsTypedBuffer(m_viewDesc.type);
		const bool isStructuredBuffer = IsStructuredBuffer(m_viewDesc.type) || (!IsRawBuffer(m_viewDesc.type) && 0 < m_desc.stride);
		const u32 typedElementSize = isTypedBuffer ? GetTextureFormatByteSize(m_viewDesc.format) : 0;
		if (isTypedBuffer && typedElementSize == 0) {
			LOG_ERROR("UAVの生成に失敗しました。TypedBufferには有効なformatの指定が必要です。[name={}, format={}]", m_desc.name, enum_cast(m_viewDesc.format));
			return false;
		}
		if (isStructuredBuffer && m_desc.stride == 0) {
			LOG_ERROR("UAVの生成に失敗しました。StructuredBufferにはstrideの指定が必要です。[name={}]", m_desc.name);
			return false;
		}

		// D3D12のバッファUAVは、TypedBuffer / StructuredBuffer / ByteAddressBuffer で
		// Format、StructureByteStride、RAWフラグの組み合わせが排他的に変わる。
		// TypedBufferはformatの要素数、StructuredBufferはstrideの要素数、
		// ByteAddressBufferはR32_TYPELESSを4byte単位のRAW viewとして設定する。
		// 現状はカウンタ付きStructuredBufferを扱わないため、CounterOffsetInBytesは0に固定する。
		D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
		desc.Format = isTypedBuffer ? TypeConverter::Convert(m_viewDesc.format, false) : (isStructuredBuffer ? DXGI_FORMAT_UNKNOWN : DXGI_FORMAT_R32_TYPELESS);
		desc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		desc.Buffer.FirstElement = static_cast<UINT64>(m_allocation.offset / (isTypedBuffer ? typedElementSize : (isStructuredBuffer ? m_desc.stride : 4)));
		desc.Buffer.StructureByteStride = isStructuredBuffer ? m_desc.stride : 0;
		desc.Buffer.NumElements = gsl::narrow<UINT>(isTypedBuffer ? m_desc.size / typedElementSize : (isStructuredBuffer ? m_desc.size / m_desc.stride : m_desc.size / 4));
		desc.Buffer.CounterOffsetInBytes = 0;
		desc.Buffer.Flags = isTypedBuffer || isStructuredBuffer ? D3D12_BUFFER_UAV_FLAG_NONE : D3D12_BUFFER_UAV_FLAG_RAW;

		// TODO pCounterResource の調査
		m_device.getNative()->CreateUnorderedAccessView(m_resource.Get(),nullptr, &desc, handle);
		return true;

	}

}// Amuse::RHI
