//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Memory/Allocator.h>
#include "MemoryUtility.h"
#include <atomic>
#include <cstddef>
#include <cassert>

namespace Amuse::Core {

    //! @brief カスタムメモリ確保関数
    //! @param size 確保するサイズ
    //! @param alignment アライメント
    //! @return 確保されたメモリのポインタ
    void* Alloc(size_t size, size_t alignment = alignof(std::max_align_t));

    //! @brief カスタムメモリ解放関数
    //! @param ptr 解放するメモリのポインタ
    void Free(void* ptr);

    //! @brief 現在のメモリ使用量を取得
    //! @return メモリ使用量（バイト）
    size_t GetMemoryUsage();


#define AMUSE_MEMORY_SCOPE(category) Amuse::Core::Internal::MemoryCategoryScope __memoryScope(category)



    namespace Internal {

		//! @brief メモリシステムの初期化
        //! @details メモリのプロファイリングを開始します。
        void InitMemory();

        //! @brief メモリシステムの終了
        void FinalizeMemory();

        //! @brief      メモリカテゴリのスコープを管理するクラス
		//! @details    スレッドごとに現在のメモリカテゴリを保持し、スコープ内で変更を行います。
        class MemoryCategoryScope {
        private:
            const char* m_store;
        public:
            //! @brief MemoryCategoryScope を初期化する。
            MemoryCategoryScope(const char* category) {
				m_store = GetCurrentCategory();
                std::swap(GetCurrentCategory(),category);
            }
            ~MemoryCategoryScope() {
                std::swap(GetCurrentCategory(), m_store);
            }
            //! @brief 現在のスレッドに設定されているメモリカテゴリを取得する。
            static const char*& GetCurrentCategory() {
                thread_local const char* category = "Unknown";
                return category;
			}
        };

    }

}
