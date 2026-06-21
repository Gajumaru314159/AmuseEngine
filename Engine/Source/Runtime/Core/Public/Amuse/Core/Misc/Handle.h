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
    //! @brief ハンドル値を表す。
    class Handle {
    public:
        //! @brief IndexType 型のエイリアス。
        using IndexType = T;
        static_assert(std::is_integral_v<T>, "Integral type required for Handle<T>");

        static constexpr T NullIndex = T(-1); //!< 無効インデックス

        constexpr Handle() = default;
        //! @brief Handle を初期化する。
        constexpr explicit Handle(T index) : m_index{index} {}

        //! @brief 同じインデックスを指すか比較する。
        constexpr bool operator == (const Handle& rhs) const;
        //! @brief 異なるインデックスを指すか比較する。
        constexpr bool operator != (const Handle& rhs) const;
        //! @brief インデックス値の小なり比較を行う。
        constexpr bool operator < (const Handle& rhs) const;
        //! @brief インデックス値の大なり比較を行う。
        constexpr bool operator > (const Handle& rhs) const;
        //! @brief インデックス値の小なり等価比較を行う。
        constexpr bool operator <= (const Handle& rhs) const;

        //! @brief 有効なハンドルかどうかを bool として返す。
        constexpr operator bool () const;

        //! @brief 状態を初期化する。
        void reset();

        //! @brief ハンドルが保持するインデックス値を取得する。
        constexpr T index() const;

        //! @brief 空かどうかを判定する。
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
