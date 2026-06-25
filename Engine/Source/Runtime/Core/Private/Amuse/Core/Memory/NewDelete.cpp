//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include "Amuse/Core/Memory/NewDelete.h"
#include "Amuse/Core/Memory/Memory.h"

#pragma warning(push)
#pragma warning(disable : 28251)

void operator delete  (void* p, [[maybe_unused]]std::align_val_t al) noexcept {
    Amuse::Free(p);
}
void operator delete[](void* p, [[maybe_unused]]std::align_val_t al) noexcept {
    Amuse::Free(p);
}
void operator delete  (void* p, [[maybe_unused]]std::size_t n, [[maybe_unused]]std::align_val_t al) noexcept {
    Amuse::Free(p);
}
void operator delete[](void* p, [[maybe_unused]]std::size_t n, [[maybe_unused]]std::align_val_t al) noexcept {
    Amuse::Free(p);
}
void operator delete  (void* p, [[maybe_unused]]std::align_val_t al, [[maybe_unused]]const std::nothrow_t&) noexcept {
    Amuse::Free(p);
}
void operator delete[](void* p, [[maybe_unused]]std::align_val_t al, [[maybe_unused]]const std::nothrow_t&) noexcept {
    Amuse::Free(p);
}

void* operator new( std::size_t n, std::align_val_t al)   noexcept(false) {
    return Amuse::Alloc(n, static_cast<size_t>(al));
}
void* operator new[]( std::size_t n, std::align_val_t al) noexcept(false) {
    return Amuse::Alloc(n, static_cast<size_t>(al));
}
void* operator new  (std::size_t n, std::align_val_t al, const std::nothrow_t&) noexcept {
    return Amuse::Alloc(n, static_cast<size_t>(al));
}
void* operator new[](std::size_t n, std::align_val_t al, const std::nothrow_t&) noexcept {
    return Amuse::Alloc(n, static_cast<size_t>(al));
}

#pragma warning(pop)