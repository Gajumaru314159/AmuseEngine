//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include "DirectX12CommandList.h"
#include <Amuse/RHI/Buffer.h>
#include <Amuse/RHI/RenderTexture.h>
#include <Amuse/RHI/Constants.h>
#include <Amuse/RHI/Types/CommandParam.h>
#include <Amuse/DirectX12RHI/DirectX12Device.h>
#include <Amuse/DirectX12RHI/SwapChain/DirectX12SwapChain.h>
#include <Amuse/DirectX12RHI/Texture/DirectX12Texture.h>
#include <Amuse/DirectX12RHI/RootSignature/DirectX12RootSignature.h>
#include <Amuse/DirectX12RHI/PipelineState/DirectX12PipelineState.h>
#include <Amuse/DirectX12RHI/PipelineState/DirectX12ComputePipelineState.h>
#include <Amuse/DirectX12RHI/Descriptor/DirectX12DescriptorTable.h>
#include <Amuse/DirectX12RHI/Buffer/DirectX12Buffer.h>
#include <Amuse/DirectX12RHI/Query/DirectX12TimerQuery.h>
#include <Amuse/DirectX12RHI/Utility/Utility.h>
#include <Amuse/DirectX12RHI/Utility/TypeConverter.h>

#include <pix3.h>

//===============================================================
// # CommandListのマルチスレッド対応について
// * https://docs.microsoft.com/ja-jp/windows/win32/direct3d12/recording-command-lists-and-bundles#recording-command-lists
// * https://shobomaru.wordpress.com/2015/04/20/d3d12-command/
// * https://shobomaru.wordpress.com/2015/07/12/d3d12-fence/
//===============================================================


namespace Amuse {
	namespace {

		//! @brief  サブリソース範囲が全体指定か
		bool IsAllSubresources(const TextureSubresourceRange& range) {
			return
				range.baseMipLevel == 0 &&
				range.baseArrayLayer == 0 &&
				range.mipLevelCount == TextureSubresourceRange::All &&
				range.arrayLayerCount == TextureSubresourceRange::All;
		}

		//! @brief  D3D12サブリソース番号を計算
		u32 CalcSubresource(u32 mipSlice, u32 arraySlice, u32 mipLevels) {
			return mipSlice + arraySlice * mipLevels;
		}

		//! @brief  TextureSubresource から D3D12 サブリソース番号を計算
		u32 CalcSubresource(const TextureSubresource& subresource, const TextureDesc& desc) {
			return CalcSubresource(subresource.mipLevel, GetTextureSubresourceArrayLayer(desc, subresource), std::max<s32>(desc.mipLevels, 1));
		}

		u32 GetTextureFormatByteSize(TextureFormat format) {
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

	}

	//! @brief  コンストラクタ
	DirectX12CommandList::DirectX12CommandList(class DirectX12Device& device, const CommandListDesc& desc)
		: m_device(device)
		, m_desc(desc)
	{
		HRESULT result;
		auto type = TypeConverter::Convert(desc.type);

		// アロケータ生成
		result = device.getNative()->CreateCommandAllocator(type, IID_PPV_ARGS(m_cmdAllocator.ReleaseAndGetAddressOf()));
		if (FAILED(result)) {
			Utility::OutputFatalLog(result, "ID3D12Device::CreateCommandAllocator()");
			return;
		}

		// コマンドリスト生成
		UINT nodeMask = 0;
		result = device.getNative()->CreateCommandList(nodeMask, type, m_cmdAllocator.Get(), nullptr, IID_PPV_ARGS(m_cmdList.ReleaseAndGetAddressOf()));
		if (FAILED(result)) {
			Utility::OutputFatalLog(result, "ID3D12Device::CreateCommandList()");
			return;
		}

		// 初期状態で記録モードなのでクローズ
		m_cmdList->Close();

		Utility::SetName(m_cmdAllocator.Get(), m_desc.name);
		Utility::SetName(m_cmdList.Get(), m_desc.name);

		createCommandSignatures();

		manage();
	}


	//! @brief  妥当な状態か
	bool DirectX12CommandList::isValid()const {
		return m_cmdList;
	}


	//! @brief      名前を取得
	const String& DirectX12CommandList::getName()const {
		return m_desc.name;
	}


	//! @brief  描画開始
	void DirectX12CommandList::begin() {
		HRESULT result;

		// コマンドアロケータをリセット
		result = m_cmdAllocator->Reset();
		if (FAILED(result)) {
			String message = "ID3D12CommandAllocator::Reset()\n";
			message += Utility::GetDebugLayerLastString(m_device.getNative().Get());
			Utility::OutputFatalLog(result, message);
		}

		// コマンドリストをリセット
		result = m_cmdList->Reset(m_cmdAllocator.Get(), nullptr);
		if (FAILED(result)) {
			String message = "ID3D12CommandList::Reset()\n";
			message += Utility::GetDebugLayerLastString(m_device.getNative().Get());
			Utility::OutputFatalLog(result, message);
		}

		Utility::SetName(m_cmdAllocator.Get(), getName());
		Utility::SetName(m_cmdList.Get(), getName());

		m_temporaryResources.clear();
		m_temporaryBuffers.clear();

		// デスクリプタヒープを設定
		m_device.setDescriptorHeaps(*this);

		m_signature = nullptr;
		m_computePipelineBound = false;
	}

	//! @brief  描画終了
	void DirectX12CommandList::end() {
		m_cmdList->Close();
	}

	//! @brief  コマンドをシステムキューに追加
	//! // TODO Singletonに依存しているので廃止
	//! // DirectX12CommandList::writeTexture() が一時アップロードバッファ生成に Buffer::Create() を使用し、
	//! // Buffer::Create() が Amuse::Device::Get() (Device Singleton) 経由でリソースを生成している。
	//! @brief 自動バリアを有効化
	void DirectX12CommandList::setEnableAutomaticBarriers(bool enable) {
		m_enableAutomaticBarriers = enable;
	}

	//! @brief テクスチャ遷移バリアを追加
	void DirectX12CommandList::addTextureBarrier(const TextureBarrier& barrier) {
		if (barrier.after == TextureState::Unknown) return;
		if (auto texture = barrier.texture.cast<DirectX12Texture>()) {
			const auto after = TypeConverter::Convert(barrier.after);
			if (barrier.before == TextureState::Unknown) {
				if (IsAllSubresources(barrier.range)) {
					m_cache.addTexture(*texture, after);
				}
				else {
					const auto& desc = texture->desc();
					const u32 mipLevels = std::max<s32>(desc.mipLevels, 1);
					const u32 arrayCount = std::max<s32>(desc.arrayNum, 1);
					const u32 mipCount = barrier.range.mipLevelCount == TextureSubresourceRange::All ? mipLevels - barrier.range.baseMipLevel : barrier.range.mipLevelCount;
					const u32 layerCount = barrier.range.arrayLayerCount == TextureSubresourceRange::All ? arrayCount - barrier.range.baseArrayLayer : barrier.range.arrayLayerCount;
					for (u32 layer = 0; layer < layerCount; ++layer) {
						for (u32 mip = 0; mip < mipCount; ++mip) {
							m_cache.addTexture(*texture, after, CalcSubresource(barrier.range.baseMipLevel + mip, barrier.range.baseArrayLayer + layer, mipLevels));
						}
					}
				}
			}
			else {
				const auto before = TypeConverter::Convert(barrier.before);
				if (IsAllSubresources(barrier.range)) {
					m_cache.addTexture(texture->getResource(), before, after);
				}
				else {
					const auto& desc = texture->desc();
					const u32 mipLevels = std::max<s32>(desc.mipLevels, 1);
					const u32 arrayCount = std::max<s32>(desc.arrayNum, 1);
					const u32 mipCount = barrier.range.mipLevelCount == TextureSubresourceRange::All ? mipLevels - barrier.range.baseMipLevel : barrier.range.mipLevelCount;
					const u32 layerCount = barrier.range.arrayLayerCount == TextureSubresourceRange::All ? arrayCount - barrier.range.baseArrayLayer : barrier.range.arrayLayerCount;
					for (u32 layer = 0; layer < layerCount; ++layer) {
						for (u32 mip = 0; mip < mipCount; ++mip) {
							m_cache.addTexture(texture->getResource(), before, after, CalcSubresource(barrier.range.baseMipLevel + mip, barrier.range.baseArrayLayer + layer, mipLevels));
						}
					}
				}
			}
		}
	}

	//! @brief バッファ遷移バリアを追加
	void DirectX12CommandList::addBufferBarrier(const BufferBarrier& barrier) {
		if (barrier.after == BufferState::Unknown) return;
		if (auto buffer = barrier.buffer.cast<DirectX12Buffer>()) {
			const auto before = TypeConverter::Convert(barrier.before == BufferState::Unknown ? buffer->getDesc().state : barrier.before);
			const auto after = TypeConverter::Convert(barrier.after);
			m_cache.addBuffer(buffer->getNative(), before, after);
		}
	}

	//! @brief テクスチャUAVバリアを追加
	void DirectX12CommandList::addUAVBarrier(const Ref<Texture>& texture) {
		if (auto native = texture.cast<DirectX12Texture>()) {
			m_cache.addUAV(native->getResource());
		}
	}

	//! @brief バッファUAVバリアを追加
	void DirectX12CommandList::addUAVBarrier(const Ref<Buffer>& buffer) {
		if (auto native = buffer.cast<DirectX12Buffer>()) {
			m_cache.addUAV(native->getNative());
		}
	}

	//! @brief 追加済みバリアを記録
	void DirectX12CommandList::commitBarriers() {
		m_cache.recordCommand(*m_cmdList.Get());
	}

	//! @brief バッファをコピー
	void DirectX12CommandList::copyBuffer(const BufferCopyRegion& region) {
		auto source = region.source.cast<DirectX12Buffer>();
		auto target = region.target.cast<DirectX12Buffer>();
		if (!source || !target) return;

		const auto size = region.size == 0 ? std::min(source->getDesc().size - region.sourceOffset, target->getDesc().size - region.targetOffset) : region.size;
		m_cmdList->CopyBufferRegion(
			target->getNative(), region.targetOffset + target->getViewOffset(),
			source->getNative(), region.sourceOffset + source->getViewOffset(),
			size);
	}

	//! @brief テクスチャをコピー
	void DirectX12CommandList::copyTexture(const TextureCopyRegion& region) {
		auto source = region.source.cast<DirectX12Texture>();
		auto target = region.target.cast<DirectX12Texture>();
		if (!source || !target) return;

		D3D12_TEXTURE_COPY_LOCATION src{};
		src.pResource = source->getResource();
		src.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		src.SubresourceIndex = CalcSubresource(region.sourceSubresource, source->desc());

		D3D12_TEXTURE_COPY_LOCATION dst{};
		dst.pResource = target->getResource();
		dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		dst.SubresourceIndex = CalcSubresource(region.targetSubresource, target->desc());

		D3D12_BOX box{};
		box.left = region.sourceOrigin.x;
		box.top = region.sourceOrigin.y;
		box.front = region.sourceOrigin.z;
		box.right = box.left + region.size.width;
		box.bottom = box.top + region.size.height;
		box.back = box.front + std::max<s32>(region.size.depth, 1);

		m_cmdList->CopyTextureRegion(&dst, region.targetOrigin.x, region.targetOrigin.y, region.targetOrigin.z, &src, &box);
	}

	//! @brief バッファからテクスチャへコピー
	void DirectX12CommandList::copyBufferToTexture(const BufferTextureCopyRegion& region) {
		auto buffer = region.buffer.cast<DirectX12Buffer>();
		auto texture = region.texture.cast<DirectX12Texture>();
		if (!buffer || !texture) return;

		D3D12_TEXTURE_COPY_LOCATION src{};
		src.pResource = buffer->getNative();
		src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		src.PlacedFootprint.Offset = region.bufferOffset + buffer->getViewOffset();
		src.PlacedFootprint.Footprint.Format = TypeConverter::Convert(texture->desc().format);
		src.PlacedFootprint.Footprint.Width = region.size.width;
		src.PlacedFootprint.Footprint.Height = region.size.height;
		src.PlacedFootprint.Footprint.Depth = std::max<s32>(region.size.depth, 1);
		src.PlacedFootprint.Footprint.RowPitch = region.rowPitch;

		D3D12_TEXTURE_COPY_LOCATION dst{};
		dst.pResource = texture->getResource();
		dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		dst.SubresourceIndex = CalcSubresource(region.textureSubresource, texture->desc());

		m_cmdList->CopyTextureRegion(&dst, region.textureOrigin.x, region.textureOrigin.y, region.textureOrigin.z, &src, nullptr);
	}

	//! @brief テクスチャからバッファへコピー
	void DirectX12CommandList::copyTextureToBuffer(const BufferTextureCopyRegion& region) {
		auto buffer = region.buffer.cast<DirectX12Buffer>();
		auto texture = region.texture.cast<DirectX12Texture>();
		if (!buffer || !texture) return;

		D3D12_TEXTURE_COPY_LOCATION src{};
		src.pResource = texture->getResource();
		src.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		src.SubresourceIndex = CalcSubresource(region.textureSubresource, texture->desc());

		D3D12_TEXTURE_COPY_LOCATION dst{};
		dst.pResource = buffer->getNative();
		dst.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		dst.PlacedFootprint.Offset = region.bufferOffset + buffer->getViewOffset();
		dst.PlacedFootprint.Footprint.Format = TypeConverter::Convert(texture->desc().format);
		dst.PlacedFootprint.Footprint.Width = region.size.width;
		dst.PlacedFootprint.Footprint.Height = region.size.height;
		dst.PlacedFootprint.Footprint.Depth = std::max<s32>(region.size.depth, 1);
		dst.PlacedFootprint.Footprint.RowPitch = region.rowPitch;

		m_cmdList->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);
	}

	//! @brief テクスチャへデータを書き込む
	void DirectX12CommandList::writeTexture(const Ref<Texture>& texture, const TextureSubresource& subresource, BlobView data, u32 rowPitch, u32 depthPitch) {
		if (!texture || data.empty() || rowPitch == 0) return;

		const auto& desc = texture->desc();
		const u32 bytesPerPixel = GetTextureFormatByteSize(desc.format);
		if (bytesPerPixel == 0) return;

		const auto width = subresource.region ? subresource.region->width() : desc.size.width;
		const auto height = subresource.region ? subresource.region->height() : desc.size.height;
		const auto depth = std::max<s32>(desc.type == TextureType::Texture3D ? desc.size.depth : 1, 1);
		const u32 uploadRowPitch = align_up(rowPitch, static_cast<size_t>(D3D12_TEXTURE_DATA_PITCH_ALIGNMENT));
		const u32 uploadDepthPitch = uploadRowPitch * height;
		const u64 uploadSize = static_cast<u64>(depthPitch == 0 ? uploadDepthPitch * depth : uploadDepthPitch * depth);

		Vector<u8> uploadData(uploadSize);
		const auto sourceDepthPitch = depthPitch == 0 ? rowPitch * height : depthPitch;
		for (s32 z = 0; z < depth; ++z) {
			for (s32 y = 0; y < height; ++y) {
				const auto* src = data.data() + static_cast<size_t>(z) * sourceDepthPitch + static_cast<size_t>(y) * rowPitch;
				auto* dst = uploadData.data() + static_cast<size_t>(z) * uploadDepthPitch + static_cast<size_t>(y) * uploadRowPitch;
				std::memcpy(dst, src, std::min<size_t>(rowPitch, static_cast<size_t>(width) * bytesPerPixel));
			}
		}

		auto upload = Buffer::Create(BufferDesc::ByteAddress(
			uploadSize,
			BufferState::CopySource,
			BufferFlag::CopySource));
		if (!upload) return;
		if (auto mapped = upload->map(CpuAccess::Write)) {
			std::memcpy(mapped, uploadData.data(), uploadData.size());
			upload->unmap();
		} else {
			upload->updateDirect(uploadData.size(), uploadData.data(), 0);
		}

		BufferTextureCopyRegion region;
		region.buffer = upload;
		region.texture = texture;
		region.rowPitch = uploadRowPitch;
		region.depthPitch = uploadDepthPitch;
		region.textureSubresource = subresource;
		region.textureOrigin = subresource.region ? Point{ subresource.region->left, subresource.region->top, 0 } : Point{};
		region.size = { width, height, depth };
		copyBufferToTexture(region);

		m_temporaryBuffers.emplace_back(upload);
	}

	//! @brief カラーテクスチャをクリア
	void DirectX12CommandList::clearColorTexture(const Ref<Texture>& texture, const TextureSubresourceRange& range, const Color& color) {
		if (auto renderTexture = texture.cast<DirectX12Texture>()) {
			FLOAT clearColor[4] = { color.r, color.g, color.b, color.a };
			if (!renderTexture->getRTV()) return;

			if (IsAllSubresources(range)) {
				m_cmdList->ClearRenderTargetView(renderTexture->getRTV().getCpuHandle(), clearColor, 0, nullptr);
			} else {
				DescriptorHandle handle;
				m_device.allocateHandle(DescriptorHeapType::RTV, handle, 1);
				renderTexture->createRTV(handle.getCpuHandle(), range);
				m_cmdList->ClearRenderTargetView(handle.getCpuHandle(), clearColor, 0, nullptr);
			}
		}
	}

	//! @brief 深度ステンシルテクスチャをクリア
	void DirectX12CommandList::clearDepthStencilTexture(const Ref<Texture>& texture, const TextureSubresourceRange& range, bool clearDepth, f32 depth, bool clearStencil, u8 stencil) {
		if (auto renderTexture = texture.cast<DirectX12Texture>()) {
			if (!renderTexture->getDSV()) return;
			D3D12_CLEAR_FLAGS flags{};
			if (clearDepth && TextureFormatUtility::HasDepth(renderTexture->desc().format)) flags |= D3D12_CLEAR_FLAG_DEPTH;
			if (clearStencil && TextureFormatUtility::HasStencil(renderTexture->desc().format)) flags |= D3D12_CLEAR_FLAG_STENCIL;
			if (flags == D3D12_CLEAR_FLAGS{}) return;

			if (IsAllSubresources(range)) {
				m_cmdList->ClearDepthStencilView(renderTexture->getDSV().getCpuHandle(), flags, depth, stencil, 0, nullptr);
			} else {
				DescriptorHandle handle;
				m_device.allocateHandle(DescriptorHeapType::DSV, handle, 1);
				renderTexture->createDSV(handle.getCpuHandle(), range);
				m_cmdList->ClearDepthStencilView(handle.getCpuHandle(), flags, depth, stencil, 0, nullptr);
			}
		}
	}

	//! @brief バッファを32bit整数値でクリア
	void DirectX12CommandList::clearBufferUInt(const Ref<Buffer>& buffer, u32 value) {
		auto native = buffer.cast<DirectX12Buffer>();
		if (!native) return;

		const auto clearSize = align_down(native->getDesc().size, static_cast<u64>(sizeof(u32)));
		if (clearSize == 0) return;

		auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(clearSize);
		ComPtr<ID3D12Resource> upload;
		auto result = m_device.getNative()->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(upload.GetAddressOf()));
		if (FAILED(result)) {
			Utility::OutputErrorLog(result, "ID3D12Device::CreateCommittedResource()");
			return;
		}

		void* mapped = nullptr;
		result = upload->Map(0, nullptr, &mapped);
		if (FAILED(result) || mapped == nullptr) {
			Utility::OutputErrorLog(result, "ID3D12Resource::Map()");
			return;
		}
		std::fill_n(static_cast<u32*>(mapped), clearSize / sizeof(u32), value);
		upload->Unmap(0, nullptr);

		auto toCopyDest = CD3DX12_RESOURCE_BARRIER::Transition(
			native->getNative(),
			D3D12_RESOURCE_STATE_COMMON,
			D3D12_RESOURCE_STATE_COPY_DEST);
		m_cmdList->ResourceBarrier(1, &toCopyDest);
		m_cmdList->CopyBufferRegion(native->getNative(), native->getViewOffset(), upload.Get(), 0, clearSize);
		auto toInitialState = CD3DX12_RESOURCE_BARRIER::Transition(
			native->getNative(),
			D3D12_RESOURCE_STATE_COPY_DEST,
			TypeConverter::Convert(native->getDesc().state));
		m_cmdList->ResourceBarrier(1, &toInitialState);

		m_temporaryResources.emplace_back(upload);
	}

	//! @brief テクスチャをResolve
	void DirectX12CommandList::resolveTexture(const Ref<Texture>& source, const Ref<Texture>& target, const TextureSubresourceRange& sourceRange, const TextureSubresourceRange& targetRange) {
		auto src = source.cast<DirectX12Texture>();
		auto dst = target.cast<DirectX12Texture>();
		if (!src || !dst) return;
		const u32 srcSubresource = CalcSubresource(sourceRange.baseMipLevel, sourceRange.baseArrayLayer, std::max<s32>(src->desc().mipLevels, 1));
		const u32 dstSubresource = CalcSubresource(targetRange.baseMipLevel, targetRange.baseArrayLayer, std::max<s32>(dst->desc().mipLevels, 1));
		m_cmdList->ResolveSubresource(dst->getResource(), dstSubresource, src->getResource(), srcSubresource, TypeConverter::Convert(dst->desc().format));
	}

	//! @brief      RenderPass開始
	void DirectX12CommandList::beginRenderPass(const BeginPassParam& param) {

		clearRenderTargets();

		m_currentRenderPass = param;

		s32 width = 0;
		s32 height = 0;

		FixedVector<D3D12_RENDER_PASS_RENDER_TARGET_DESC, RENDER_TARGET_MAX> colors;
		D3D12_RENDER_PASS_DEPTH_STENCIL_DESC  depth;
		D3D12_RENDER_PASS_DEPTH_STENCIL_DESC* pDepth = nullptr;

		RenderTargetArray colorTextures;
		Ref<RenderTexture> depthTexture;

		if (m_enableAutomaticBarriers) {
			m_cache.clear();
		}

		for (auto [i, color] : Indexed(param.colors)) {
			auto& targetDesc = colors.emplace_back();
			if (auto texture = color.texture.cast<DirectX12Texture>()) {
				targetDesc.cpuDescriptor = texture->getRTV().getCpuHandle();
				targetDesc.BeginningAccess.Type = TypeConverter::Convert(color.beforeAccess);
				targetDesc.EndingAccess.Type = TypeConverter::Convert(color.afterAccess);

				width = texture->width();
				height = texture->height();

				if (m_enableAutomaticBarriers) {
					m_cache.addTexture(*texture, D3D12_RESOURCE_STATE_RENDER_TARGET);
				}
			} else {
				targetDesc.BeginningAccess.Type = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_NO_ACCESS;
				targetDesc.EndingAccess.Type = D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_NO_ACCESS;
			}
		}
		{
			auto& targetDesc = depth;
			if (auto texture = param.depth.texture.cast<DirectX12Texture>()) {
				targetDesc.cpuDescriptor = texture->getDSV().getCpuHandle();
				targetDesc.DepthBeginningAccess.Type = TypeConverter::Convert(param.depth.beforeAccess);
				targetDesc.DepthEndingAccess.Type = TypeConverter::Convert(param.depth.afterAccess);

				width = texture->width();
				height = texture->height();

				pDepth = &depth;

				// TODO D3D12_RESOURCE_STATE_DEPTH_READが必要か確認する
				if (m_enableAutomaticBarriers) {
					m_cache.addTexture(*texture, D3D12_RESOURCE_STATE_DEPTH_WRITE);
				}
			} else {
				targetDesc.DepthBeginningAccess.Type = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_NO_ACCESS;
				targetDesc.DepthEndingAccess.Type = D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_NO_ACCESS;
			}
			if (auto texture = param.stencil.texture.cast<DirectX12Texture>()) {
				targetDesc.cpuDescriptor = texture->getRTV().getCpuHandle();
				targetDesc.StencilBeginningAccess.Type = TypeConverter::Convert(param.stencil.beforeAccess);
				targetDesc.StencilEndingAccess.Type = TypeConverter::Convert(param.stencil.afterAccess);

				width = texture->width();
				height = texture->height();

				pDepth = &depth;

				if (m_enableAutomaticBarriers) {
					m_cache.addTexture(*texture, D3D12_RESOURCE_STATE_DEPTH_WRITE);
				}
			} else {
				targetDesc.StencilBeginningAccess.Type = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_NO_ACCESS;
				targetDesc.StencilEndingAccess.Type = D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_NO_ACCESS;
			}

			if (param.depth.texture && param.stencil.texture && param.depth.texture != param.stencil.texture) {
				AMUSE_ABORT("DepthとStencilが異なるテクスチャを指しています");
			}
		}

		D3D12_RENDER_PASS_FLAGS flags = D3D12_RENDER_PASS_FLAG_NONE;
		if (param.flags & RenderPassFlag::AllowUAVWrite) flags |= D3D12_RENDER_PASS_FLAG_ALLOW_UAV_WRITES;
		if (param.flags & RenderPassFlag::SuspendingPass) flags |= D3D12_RENDER_PASS_FLAG_SUSPENDING_PASS;
		if (param.flags & RenderPassFlag::ResumingPass) flags |= D3D12_RENDER_PASS_FLAG_RESUMING_PASS;


		// リソースバリア
		if (m_enableAutomaticBarriers) {
			m_cache.recordCommand(*m_cmdList.Get());
		}

		m_cmdList->BeginRenderPass(colors.size(), colors.data(), pDepth, flags);

		// 初期設定としてViewportとScissorRectを設定
		Viewport viewport(0, 0, width, height);
		IntRect scissor(0,0,width,height);

		setViewport(&viewport,1);
		setScissorRect(&scissor,1);

	}

	//! @brief RenderPass終了
	void DirectX12CommandList::endRenderPass() {

		m_cmdList->EndRenderPass();

		if (m_enableAutomaticBarriers) {
			m_cache.clear();
		}

		if (m_enableAutomaticBarriers) {
			for (auto [i, color] : Indexed(m_colorTextures)) {
				if (auto texture = color.cast<DirectX12Texture>()) {
					m_cache.addTexture(*texture, D3D12_RESOURCE_STATE_COMMON);
				}
			}
			if (auto texture = m_depthTexture.cast<DirectX12Texture>()) {
				m_cache.addTexture(*texture, D3D12_RESOURCE_STATE_COMMON);
			}
		}

		// リソースバリア
		if (m_enableAutomaticBarriers) {
			m_cache.recordCommand(*m_cmdList.Get());
		}

		clearRenderTargets();

	}


	//! @brief      スワップチェーンにテクスチャを適用
	void DirectX12CommandList::applySwapChain(const Ref<SwapChain>& swapChain, const Ref<RenderTexture>& texture)
	{
		if (auto pSwapChain = swapChain.cast<DirectX12SwapChain>()) {
			pSwapChain->recordApplySwapChain(*this, texture);
		}
	}


	//! @brief  シザー矩形を設定
	void DirectX12CommandList::setScissorRect(const IntRect* pRect, s32 num) {

		AMUSE_ASSERT(num <= SCISSOR_RECT_MAX, "シザー矩形の最大数を超えました。[value={0},max={1}]", num, SCISSOR_RECT_MAX);
		D3D12_RECT rects[SCISSOR_RECT_MAX];
		for (s32 i = 0; i < num; ++i) {
			rects[i].left = pRect[i].left;
			rects[i].top = pRect[i].top;
			rects[i].right = pRect[i].right;
			rects[i].bottom = pRect[i].bottom;
		}
		m_cmdList->RSSetScissorRects(num, rects);

	}


	//! @brief  ビューポートを設定
	void DirectX12CommandList::setViewport(const Viewport* pViewport, s32 num) {

		AMUSE_ASSERT(num <= VIEWPORT_MAX, "ビューポートの最大数を超えました。[value={0},max={1}]", num, VIEWPORT_MAX);
		D3D12_VIEWPORT viewports[VIEWPORT_MAX];
		for (s32 i = 0; i < num; ++i) {
			viewports[i].TopLeftX = pViewport[i].left;
			viewports[i].TopLeftY = pViewport[i].top;
			viewports[i].Width = pViewport[i].width();
			viewports[i].Height = pViewport[i].height();
			viewports[i].MinDepth = pViewport[i].nearZ;
			viewports[i].MaxDepth = pViewport[i].farZ;
		}
		m_cmdList->RSSetViewports(num, viewports);
	}


	//! @brief      頂点バッファを設定
	void DirectX12CommandList::setVertexBuffers(Span<Ref<Buffer>> buffers, s32 first) {
		Array<D3D12_VERTEX_BUFFER_VIEW, VERTEX_BUFFER_MAX> views;
		if (views.size() <= buffers.size()) {
			LOG_ERROR("頂点バッファは{}以下である必要があります。[size={}]",views.size(),buffers.size());
			return;
		}

		size_t size = 0;
		for (auto& buffer : buffers) {
			if (buffer == nullptr) {
				LOG_ERROR("頂点バッファがnullです。");
				return;
			}
			
			if (auto pBuffer = buffer.cast<DirectX12Buffer>()) {
				auto& view = views[size];
				view.BufferLocation = pBuffer->getNative()->GetGPUVirtualAddress() + pBuffer->getViewOffset();
				view.SizeInBytes = static_cast<UINT>(pBuffer->getDesc().size);
				view.StrideInBytes = pBuffer->getDesc().stride;
			} else {
				LOG_ERROR("空の頂点バッファが含まれています");
			}
			size++;
		}

		m_cmdList->IASetVertexBuffers(first, static_cast<UINT>(size), views.data());
	}


	//! @brief      インデックスバッファを設定
	void DirectX12CommandList::setIndexBuffer(const Ref<Buffer>& buffer) {
		if (auto pBuffer = buffer.cast<DirectX12Buffer>()) {
			D3D12_INDEX_BUFFER_VIEW view;
			view.BufferLocation = pBuffer->getNative()->GetGPUVirtualAddress() + pBuffer->getViewOffset();
			view.SizeInBytes = static_cast<UINT>(pBuffer->getDesc().size);
			view.Format = pBuffer->getDesc().stride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
			m_cmdList->IASetIndexBuffer(&view);
		} else {
			LOG_ERROR("空のインデックスバッファは指定できません");
		}
	}


	//! @brief      パイプラインステートを設定
	void DirectX12CommandList::setPipelineState(const Ref<PipelineState>& pipeline) {
		if (auto p = pipeline.cast<DirectX12PipelineState>()) {

			if (auto signature = p->getRootSignature()) {

				// Bindless時は特にルートシグネチャが共通しているので変更時のみ記録する
				if (m_signature != signature) {
					m_cmdList->SetGraphicsRootSignature(signature->getNative());
					m_signature = signature;
				}

				m_cmdList->SetPipelineState(p->getNative());

				// TODO Geometryシェーダでのプリミティブ設定対応
				m_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				m_computePipelineBound = false;

			}

		}
	}

	//! @brief      コンピュートパイプラインステートを設定
	void DirectX12CommandList::setComputePipelineState(const Ref<ComputePipelineState>& pipeline) {
		if (auto p = pipeline.cast<DirectX12ComputePipelineState>()) {
			if (auto signature = p->getRootSignature()) {
				if (m_signature != signature) {
					m_cmdList->SetComputeRootSignature(signature->getNative());
					m_signature = signature;
				}
				m_cmdList->SetPipelineState(p->getNative());
				m_computePipelineBound = true;
			}
		}
	}


	//! @brief      描画
	void DirectX12CommandList::draw(const DrawParam& param) {
		m_cmdList->DrawInstanced(param.vertexCount, param.instanceCount, param.startVertex,param.startInstance);
	}


	//! @brief      インデックス描画
	void DirectX12CommandList::drawIndexed(const DrawIndexedParam& param) {
		m_cmdList->DrawIndexedInstanced(param.indexCount,param.instanceCount, param.startIndex, gsl::narrow_cast<INT>(param.startVertex),param.startInstance);
	}

	//! @brief      コンピュートディスパッチ
	void DirectX12CommandList::dispatch(const DispatchParam& param) {
		m_cmdList->Dispatch(param.groupX, param.groupY, param.groupZ);
	}

	//! @brief      Indirect描画
	void DirectX12CommandList::drawIndirect(const IndirectCommandParam& param) {
		if (auto buffer = param.argumentBuffer.cast<DirectX12Buffer>()) {
			m_cmdList->ExecuteIndirect(m_drawCommandSignature.Get(), param.commandCount, buffer->getNative(), param.argumentOffset + buffer->getViewOffset(), nullptr, 0);
		}
	}

	//! @brief      Indirectインデックス描画
	void DirectX12CommandList::drawIndexedIndirect(const IndirectCommandParam& param) {
		if (auto buffer = param.argumentBuffer.cast<DirectX12Buffer>()) {
			m_cmdList->ExecuteIndirect(m_drawIndexedCommandSignature.Get(), param.commandCount, buffer->getNative(), param.argumentOffset + buffer->getViewOffset(), nullptr, 0);
		}
	}

	//! @brief      Indirectコンピュートディスパッチ
	void DirectX12CommandList::dispatchIndirect(const IndirectCommandParam& param) {
		if (auto buffer = param.argumentBuffer.cast<DirectX12Buffer>()) {
			m_cmdList->ExecuteIndirect(m_dispatchCommandSignature.Get(), param.commandCount, buffer->getNative(), param.argumentOffset + buffer->getViewOffset(), nullptr, 0);
		}
	}


	//! @brief      デスクリプタテーブルを設定
	void DirectX12CommandList::setDescriptorTables(const SetDescriptorTableParam* params, s32 num) {
		AMUSE_ASSERT(m_signature!=nullptr,"先にPipelineStateを設定してください");
		for (s32 i = 0; i < num; ++i) {
			auto& param = params[i];

			if (!is_in_range(param.slot, m_signature->getDesc().layouts)) {
				LOG_ERROR("スロットが範囲外です");
				continue;
			}

			if (auto pTable = param.table.cast<DirectX12DescriptorTable>()) {
				if (m_computePipelineBound) {
					pTable->recordCompute(*m_cmdList.Get(), *m_signature, param.slot);
				} else {
					pTable->record(*m_cmdList.Get(), *m_signature, param.slot);
				}
			}
		}
	}


	//! @brief      ルート定数を設定
	void DirectX12CommandList::setRootConstant(const SetRootConstantsParam& param) {
		if (param.blob.size() % 4) {
			LOG_WARNING("ルート定数のサイズが4の倍数ではありません。");
		}
		if (param.blob.size()) {
			// DirectX12ではルート定数はスロット0に固定
			constexpr s32 slot = 0;
			if (m_computePipelineBound) {
				m_cmdList->SetComputeRoot32BitConstants(slot, param.blob.size() / sizeof(s32), param.blob.data(), param.offset / sizeof(s32));
			} else {
				m_cmdList->SetGraphicsRoot32BitConstants(slot, param.blob.size() / sizeof(s32), param.blob.data(), param.offset / sizeof(s32));
			}
		}
	}


	//! @brief  GPUマーカーをプッシュ
	void DirectX12CommandList::pushMarker(StringView name) {
		StringEncoder::Encode(name, m_markerNameCache);
		::PIXBeginEvent(m_cmdList.Get(), PIX_COLOR_DEFAULT, m_markerNameCache.data());
	}


	//! @brief  GPUマーカーをポップ
	void DirectX12CommandList::popMarker() {
		::PIXEndEvent(m_cmdList.Get());
	}

	void DirectX12CommandList::setEventQuery(const Ref<EventQuery>& query) {
		if (query) {
			m_eventQueries.push_back(query);
		}
	}

	//! @brief      GPUタイマークエリを開始
	void DirectX12CommandList::beginTimerQuery(const Ref<TimerQuery>& query) {
		if (auto timer = query.cast<DirectX12TimerQuery>()) {
			timer->recordBegin(*m_cmdList.Get());
		}
	}

	//! @brief      GPUタイマークエリを終了
	void DirectX12CommandList::endTimerQuery(const Ref<TimerQuery>& query) {
		if (auto timer = query.cast<DirectX12TimerQuery>()) {
			timer->recordEnd(*m_cmdList.Get());
		}
	}
	

	//! @brief  デスクリプタハンドルのキャッシュをクリア
	void DirectX12CommandList::clearRenderTargets() {
		m_colorTextures.clear();
		m_depthTexture = nullptr;
	}

	//! @brief      Indirectコマンドシグネチャを生成
	void DirectX12CommandList::createCommandSignatures() {
		{
			D3D12_INDIRECT_ARGUMENT_DESC argument{};
			argument.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW;
			D3D12_COMMAND_SIGNATURE_DESC desc{};
			desc.ByteStride = m_device.getDrawIndirectArgumentSize();
			desc.NumArgumentDescs = 1;
			desc.pArgumentDescs = &argument;
			m_device.getNative()->CreateCommandSignature(&desc, nullptr, IID_PPV_ARGS(m_drawCommandSignature.ReleaseAndGetAddressOf()));
		}
		{
			D3D12_INDIRECT_ARGUMENT_DESC argument{};
			argument.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED;
			D3D12_COMMAND_SIGNATURE_DESC desc{};
			desc.ByteStride = m_device.getDrawIndexedIndirectArgumentSize();
			desc.NumArgumentDescs = 1;
			desc.pArgumentDescs = &argument;
			m_device.getNative()->CreateCommandSignature(&desc, nullptr, IID_PPV_ARGS(m_drawIndexedCommandSignature.ReleaseAndGetAddressOf()));
		}
		{
			D3D12_INDIRECT_ARGUMENT_DESC argument{};
			argument.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DISPATCH;
			D3D12_COMMAND_SIGNATURE_DESC desc{};
			desc.ByteStride = m_device.getDispatchIndirectArgumentSize();
			desc.NumArgumentDescs = 1;
			desc.pArgumentDescs = &argument;
			m_device.getNative()->CreateCommandSignature(&desc, nullptr, IID_PPV_ARGS(m_dispatchCommandSignature.ReleaseAndGetAddressOf()));
		}
	}

}
