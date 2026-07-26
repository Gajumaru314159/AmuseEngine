//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>
#include <Amuse/RHI/GraphicObject.h>
#include <Amuse/RHI/Types/BufferDesc.h>
#include <Amuse/RHI/Types/BindlessHandle.h>

namespace Amuse {
	struct DrawParam;
	struct DrawIndexedParam;
	struct DispatchParam;

	//! @brief      バッファ
	//! @ingroup AmuseRHI
	class Buffer :public GraphicObject {
	public:
		//! @brief バッファコピー関数型
		using CopyFunc = Func<void(void*)>;
	public:

		//! @brief  空のバッファを作成
		static Ref<Buffer> Create(const BufferDesc& desc);

		//! @brief  初期データを指定してバッファを作成
		static Ref<Buffer> Create(const BufferDesc& desc, BlobView blob);

		//! @brief  ベースのバッファを指定して異なるビューを持つバッファを作成
		static Ref<Buffer> Create(const BufferViewDesc& desc);

		//! @brief  頂点バッファを作成
		template<typename TVertex>
		static Ref<Buffer> CreateVertex(StringView name, u64 count, BufferFlags flags = BufferFlag::Vertex) {
			auto desc = BufferDesc::Vertex<TVertex>(count, flags);
			desc.name = name;
			return Create(desc);
		}

		//! @brief  インデックスバッファを作成
		template<typename TIndex>
		static Ref<Buffer> CreateIndex(StringView name, u64 count, BufferFlags flags = BufferFlag::Index) {
			auto desc = BufferDesc::Index<TIndex>(count, flags);
			desc.name = name;
			return Create(desc);
		}

		//! @brief  定数バッファを作成
		//! @details サイズが256の倍数になるように調整されます。
		static Ref<Buffer> CreateConstant(
			StringView name,
			u64 size,
			BufferState initialState = BufferState::Constant,
			BufferFlags flags = BufferFlag::Constant);

		//! @brief  ByteAddressバッファを作成
		//! @details サイズが16の倍数になるように調整されます。
		static Ref<Buffer> CreateByteAddress(
			StringView name,
			u64 size,
			BufferState initialState = BufferState::ShaderResource,
			BufferFlags flags = BufferFlag::ShaderResource);

		//! @brief  Structuredバッファを作成
		template<typename T>
		static Ref<Buffer> CreateStructured(
			StringView name,
			u64 count,
			BufferState initialState = BufferState::ShaderResource,
			BufferFlags flags = BufferFlag::ShaderResource | BufferFlag::UnorderedAccess)
		{
			auto desc = BufferDesc::Structured<T>(count, initialState, flags);
			desc.name = name;
			return Create(desc);
		}

	public:

		//! @brief		定義を取得
		virtual const BufferDesc& getDesc()const = 0;

		//! @brief      BindlessHandleを取得
		virtual BindlessHandle getHandle()const = 0;



		//! @brief      バッファを更新
		//! @note		deprecated
		//! 
		//! @details    バッファの更新は描画スレッドの直前にまとめて行われます。
		virtual void update(size_t size, const void* pData, size_t offset = 0) = 0;

		//! @brief      バッファを更新
		//! 
		//! @details    バッファの更新は描画スレッドの直前にまとめて行われます。
		virtual void update(const CopyFunc& func) = 0;

		//! @brief      バッファを更新(直接更新)
		//! @note		deprecated
		//! @details    CPUアクセス可能なバッファをmap / unmapで即時更新します。
		virtual void updateDirect(size_t size, const void* pData, size_t offset = 0) = 0;

		//! @brief      バッファを更新(直接更新)
		//! @details    CPUアクセス可能なバッファをmap / unmapで即時更新します。
		virtual void updateDirect(const CopyFunc& func) = 0;

		//! @brief      バッファを更新(直接更新)
		template<class T>
		void updateDirect(const T& value, size_t offset) {
			updateDirect(sizeof(T), &value, offset);
		}

		//! @brief      CPUからバッファをマップ
		//! @details    CPUアクセス可能なバッファのみ有効です。GPUローカルの通常バッファでは nullptr を返します。
		virtual void* map(CpuAccess access) = 0;

		//! @brief      CPUからのバッファマップを解除
		virtual void unmap() = 0;



		//! @brief      drawIndirect() 用の引数を書き込む
		virtual void writeDrawParam(const DrawParam& param, u64 offset = 0);

		//! @brief      drawIndirect() 用の引数を一括で書き込む
		virtual void writeDrawParams(Span<const DrawParam> params, u64 offset = 0) = 0;

		//! @brief      drawIndexedIndirect() 用の引数を書き込む
		virtual void writeDrawIndexedParam(const DrawIndexedParam& param, u64 offset = 0);

		//! @brief      drawIndexedIndirect() 用の引数を一括で書き込む
		virtual void writeDrawIndexedParams(Span<const DrawIndexedParam> params, u64 offset = 0) = 0;

		//! @brief      dispatchIndirect() 用の引数を書き込む
		virtual void writeDispatchParam(const DispatchParam& param, u64 offset = 0);

		//! @brief      dispatchIndirect() 用の引数を一括で書き込む
		virtual void writeDispatchParams(Span<const DispatchParam> params, u64 offset = 0) = 0;

	};

}
