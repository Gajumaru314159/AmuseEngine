//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include "Amuse/Core/Memory/Allocator.h"
#include <mimalloc.h>

namespace Amuse::Core {

    //! @brief mimallocベースのアロケーター
    class MimallocAllocator : public Allocator {
    public:
        //! @brief メモリを確保する
        void* alloc(size_t size, size_t alignment) override {
            return mi_malloc_aligned(size, alignment);
        }

        //! @brief メモリを解放する
        void free(void* ptr) override {
            mi_free(ptr);
        }
    };

}