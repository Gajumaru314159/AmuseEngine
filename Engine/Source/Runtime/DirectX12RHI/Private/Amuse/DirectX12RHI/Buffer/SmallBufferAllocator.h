//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/DirectX12RHI/pch.h>
#include <Amuse/Core/Core.h>
#include <Amuse/Core/Allocator/Utility/TLSFMapper.h>
#include <Amuse/Core/Utility/Noncopyable.h>
#include <Amuse/RHI/Types/BufferDesc.h>
#include <d3d12.h>
#include <wrl/client.h>

namespace Amuse {
	using Microsoft::WRL::ComPtr;

	// 前方宣言
	class SmallBufferAllocator;


	//! @brief バッファ確保情報
	//! @ingroup AmuseDirectX12RHI
	struct BufferAllocation {
		ComPtr<ID3D12Resource> resource;			//!< DirectX12リソース
		size_t offset = 0;							//!< ビュー開始オフセット
		size_t size = 0;							//!< ビューサイズ
		const TLSFBlock* block = nullptr;			//!< TLSFブロック（解放用）
		bool isSubAllocation = false;				//!< サブアロケーションか
		SmallBufferAllocator* allocator = nullptr;	//!< アロケータ（解放用）
	};

	//! @brief Small Buffer Allocator
	//!
	//! @details D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT以下の
	//!          小さなバッファを効率的に管理するアロケーター
	//! @ingroup AmuseDirectX12RHI
	class SmallBufferAllocator : private Noncopyable {
	public:

		//! @brief コンストラクタ
		//! @param device DirectX12デバイス
		//! @param heapType ヒープタイプ
		//! @param chunkSize チャンクサイズ（デフォルト64KB）
		SmallBufferAllocator(ID3D12Device& device, D3D12_HEAP_TYPE heapType, size_t chunkSize = 65536);

		//! @brief デストラクタ
		~SmallBufferAllocator();

		//! @brief バッファをアロケート
		//! @param size バッファサイズ
		//! @param alignment アライメント要求
		//! @return 確保情報
		BufferAllocation allocate(size_t size, size_t alignment);

		//! @brief バッファを解放
		//! @param allocation 確保情報
		void free(const BufferAllocation& allocation);

		//! @brief バッファ記述からアライメント要求を取得
		static size_t GetAlignmentFromUsage(const BufferDesc& desc);

	private:

		//! @brief バッファチャンク
		//! @ingroup AmuseDirectX12RHI
		struct BufferChunk {
			ComPtr<ID3D12Resource> resource;  //!< 64KBリソース
			TLSFMapper mapper;               //!< メモリマッパー
			D3D12_HEAP_TYPE heapType;        //!< ヒープタイプ

			BufferChunk(ID3D12Device& device, D3D12_HEAP_TYPE type, size_t size);
		};

		//! @brief 新しいチャンクを作成
		UPtr<BufferChunk> createChunk();

		//! @brief 指定サイズを確保できるチャンクを検索
		BufferChunk* findAvailableChunk(size_t alignedSize);

	private:
		ID3D12Device& m_device;                   //!< DirectX12デバイス
		D3D12_HEAP_TYPE m_heapType;               //!< ヒープタイプ
		size_t m_chunkSize;                      //!< チャンクサイズ
		Vector<UPtr<BufferChunk>> m_chunks; //!< チャンクリスト
		mutable SpinLock m_spinLock;        //!< スピンロック
	};


}
