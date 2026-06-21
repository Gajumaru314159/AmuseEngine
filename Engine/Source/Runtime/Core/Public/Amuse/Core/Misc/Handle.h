//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/CorePrivate.h>

namespace Amuse::Core {

#define AMUSE_DEFINE_HANDLE_WITH_BASE(type,base) \
    struct Handle_##type {};\
    using type = Amuse::Core::Handle<base,Handle_##type>

#define AMUSE_DEFINE_HANDLE(type) AMUSE_DEFINE_HANDLE_WITH_BASE(type,Amuse::Core::s32)


    template<class T = s32, class TNamespace = void>
    class Handle {
    public:
        using IndexType = T;
        static_assert(std::is_integral_v<T>, "Integral type required for Handle<T>");

        static constexpr T NullIndex = T(-1);

        constexpr Handle() = default;
        constexpr explicit Handle(T index) : m_index{index} {}

        constexpr bool operator == (const Handle& rhs) const;
        constexpr bool operator != (const Handle& rhs) const;
        constexpr bool operator < (const Handle& rhs) const;
        constexpr bool operator > (const Handle& rhs) const;
        constexpr bool operator <= (const Handle& rhs) const;

        constexpr operator bool () const;

        void reset();

        constexpr T index() const;

        constexpr bool empty() const;

    private:
        T m_index = NullIndex;
    };

    template <typename T, typename TNamespace>
    constexpr bool Handle<T, TNamespace>::operator==(const Handle& rhs) const {
        return m_index == rhs.m_index;
    }

    template <typename T, typename TNamespace>
    constexpr bool Handle<T, TNamespace>::operator!=(const Handle& rhs) const {
        return m_index != rhs.m_index;
    }

    template <typename T, typename TNamespace>
    constexpr bool Handle<T, TNamespace>::operator<(const Handle& rhs) const {
        return m_index < rhs.m_index;
    }

    template <typename T, typename TNamespace>
    constexpr bool Handle<T, TNamespace>::operator<=(const Handle& rhs) const {
        return m_index <= rhs.m_index;
    }

    template <typename T, typename TNamespace>
    constexpr bool Handle<T, TNamespace>::operator>(const Handle& rhs) const {
        return m_index > rhs.m_index;
    }

    template <class T, class TNamespace>
    constexpr Handle<T, TNamespace>::operator bool() const     {
        return m_index != NullIndex;
    }

    template <typename T, typename TNamespace>
    void Handle<T, TNamespace>::reset() {
        m_index = NullIndex;
    }

    template <typename T, typename TNamespace>
    constexpr T Handle<T, TNamespace>::index() const {
        return m_index;
    }

    template <typename T, typename TNamespace>
    constexpr bool Handle<T, TNamespace>::empty() const {
        return m_index == NullIndex;
    }

}

//! @cond
template<class T>
struct std::hash<Amuse::Core::Handle<T>>
{
    constexpr size_t operator()(const Amuse::Core::Handle<T>& value)const noexcept {
        size_t result = 0;
        Amuse::Core::Hash::Combine(result,value.index());
        return result;
    }

};
//! @endcond