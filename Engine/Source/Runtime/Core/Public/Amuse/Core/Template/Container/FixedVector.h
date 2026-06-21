#pragma once
#include <array>
#include <stdexcept>
#include <iterator>
#include <algorithm>
#include <initializer_list>
#include <utility>
#include <Amuse/Core/Template/Utility/TypeTraits.h>

namespace Amuse::Core {

    //! @brief  Fixed配列
    template<typename T, std::size_t N>
    class FixedVector {
    public:
        //! @brief 要素参照型。
        using reference = T&;
        //! @brief 読み取り専用の要素参照型。
        using const_reference = const T&;
        //! @brief 要素を走査するイテレータ型。
        using iterator = typename std::array<T, N>::iterator;
        //! @brief 読み取り専用で要素を走査するイテレータ型。
        using const_iterator = typename std::array<T, N>::const_iterator;
        //! @brief 要素数を表す型。
        using size_type = std::size_t;
        //! @brief イテレータ差分を表す型。
        using difference_type = std::ptrdiff_t;
        //! @brief 格納する値の型。
        using value_type = T;
        //! @brief 要素ポインタ型。
        using pointer = value_type*;
        //! @brief 読み取り専用の要素ポインタ型。
        using const_pointer = const value_type*;
        //! @brief 逆順に要素を走査するイテレータ型。
        using reverse_iterator = typename std::array<T, N>::reverse_iterator;
        //! @brief 読み取り専用で逆順に要素を走査するイテレータ型。
        using const_reverse_iterator = typename std::array<T, N>::const_reverse_iterator;
    public:

        //===============================================================
        // コンストラクタ / デストラクタ
        //===============================================================
        constexpr FixedVector() noexcept : m_size(0) {}

        //! @brief 指定した要素数で空き固定容量内の要素列を構築する。
        constexpr FixedVector(size_type size)
            : m_size(size) 
        {
            if (size > N) throw std::overflow_error("FixedVector: overflow");
        }

        //! @brief 指定した要素数分を同じ値で初期化して構築する。
        constexpr FixedVector(size_type size, const T& value)
            : m_size(size) 
        {
            if (size > N) throw std::overflow_error("FixedVector: overflow");
            for (s32 i = 0; i < m_size; ++i) {
                m_data[i] = value;
            }
        }

        //! @brief 初期化リストの要素で構築する。
        constexpr FixedVector(std::initializer_list<T> init)
            : m_size(init.size())
        {
            if (init.size() > N) throw std::overflow_error("FixedVector: overflow");
            for (s32 i = 0; i < m_size; ++i) {
				m_data[i] = *(init.begin() + i);
            }
        }

        template<class InputIt,std::enable_if_t<is_iterator<InputIt>::value>>
        //! @brief イテレータ範囲 [first, last) の要素で構築する。
        constexpr FixedVector(InputIt first, InputIt last) {
            size_type count = std::distance(first, last);
            if (count > N) throw std::overflow_error("FixedVector: overflow");
			for (auto itr = first; first != last; ++itr) {
				m_data[m_size++] = *itr;
			}
        }


        //===============================================================
        // イテレータ
        //===============================================================

        //! @brief 先頭要素を指すイテレータを返す。
        constexpr iterator begin() noexcept {
            return m_data.begin();
        }

        //! @brief 先頭要素を指す読み取り専用イテレータを返す。
        constexpr const_iterator begin() const noexcept {
            return m_data.begin();
        }

        //! @brief 先頭要素を指す読み取り専用イテレータを返す。
        constexpr const_iterator cbegin() const noexcept {
            return m_data.cbegin();
        }

        //! @brief 末尾要素の次を指すイテレータを返す。
        constexpr iterator end() noexcept {
            return m_data.begin() + m_size;
        }

        //! @brief 末尾要素の次を指す読み取り専用イテレータを返す。
        constexpr const_iterator end() const noexcept {
            return m_data.begin() + m_size;
        }

        //! @brief 末尾要素の次を指す読み取り専用イテレータを返す。
        constexpr const_iterator cend() const noexcept {
            return m_data.cbegin() + m_size;
        }

        //! @brief 逆順の先頭要素を指すイテレータを返す。
        constexpr reverse_iterator rbegin() noexcept {
            return m_data.rbegin();
        }

        //! @brief 逆順の先頭要素を指す読み取り専用イテレータを返す。
        constexpr const_reverse_iterator rbegin() const noexcept {
            return m_data.rbegin();
        }

        //! @brief 逆順の先頭要素を指す読み取り専用イテレータを返す。
        constexpr const_reverse_iterator crbegin() const noexcept {
            return m_data.crbegin();
        }

        //! @brief 逆順の末尾要素の次を指すイテレータを返す。
        constexpr reverse_iterator rend() noexcept {
            return m_data.rbegin() + m_size;
        }

        //! @brief 逆順の末尾要素の次を指す読み取り専用イテレータを返す。
        constexpr const_reverse_iterator rend() const noexcept {
            return m_data.rbegin() + m_size;
        }

        //! @brief 逆順の末尾要素の次を指す読み取り専用イテレータを返す。
        constexpr const_reverse_iterator crend() const noexcept {
            return m_data.crbegin() + m_size;
        }


        //===============================================================
        // 領域
        //===============================================================

        //! @brief 現在格納している要素数を返す。
        constexpr size_type size() const noexcept {
            return m_size;
        }

        //! @brief 格納可能な最大要素数を返す。
        static constexpr size_type max_size() noexcept {
            return N;
        }

        //! @brief 要素数を変更する。
        constexpr void resize(size_type count) {
            if (count > N) throw std::overflow_error("FixedVector: overflow");
            m_size = count;
        }

        //! @brief 要素数を変更する。
        constexpr void resize(size_type count, const value_type& value) {
            if (count > N) throw std::overflow_error("FixedVector: overflow");
            if (count > m_size) {
                for (s32 i = 0; i < count; ++i) {
					m_data[i] = value;
                }
            }
            m_size = count;
        }

        //! @brief 格納可能な固定容量を返す。
        static constexpr size_type capacity() noexcept {
            return N;
        }

        //! @brief 空かどうかを判定する。
        constexpr bool empty() const noexcept {
            return m_size == 0;
        }


        //===============================================================
        // 要素アクセス
        //===============================================================

        //! @brief 指定した要素へアクセスする。
        constexpr reference operator[](size_type pos) {
            return m_data[pos];
        }

        //! @brief 指定した要素へアクセスする。
        constexpr const_reference operator[](size_type pos) const {
            return m_data[pos];
        }

        //! @brief 指定位置の要素へアクセスする。
        constexpr reference at(size_type pos) {
            if (pos >= m_size) throw std::out_of_range("FixedVector: out of range");
            return m_data[pos];
        }

        //! @brief 指定位置の要素へアクセスする。
        constexpr const_reference at(size_type pos) const {
            if (pos >= m_size) throw std::out_of_range("FixedVector: out of range");
            return m_data[pos];
        }

        //! @brief 先頭要素への連続領域ポインタを返す。
        constexpr pointer data() {
            return m_data.data();
        }
        
        //! @brief 先頭要素への読み取り専用連続領域ポインタを返す。
        constexpr const_pointer data() const {
            return m_data.data();
        }

        //! @brief 先頭要素へアクセスする。
        constexpr reference front() {
            return m_data.front();
        }

        //! @brief 先頭要素へアクセスする。
        constexpr const_reference front() const {
            return m_data.front();
        }

        //! @brief 末尾要素へアクセスする。
        constexpr reference back() {
            return m_data[m_size - 1];
        }

        //! @brief 末尾要素へアクセスする。
        constexpr const_reference back() const {
            return m_data[m_size - 1];
        }


        //===============================================================
        // コンテナの変更
        //===============================================================

        //! @brief 先頭から n 個の要素に同じ値を代入する。
        constexpr void assign(size_type n, const T& t) {
            for (s32 i = 0; i < n; ++i) {
				m_data[i] = t;
            }
        }

        template<class InputIt, std::enable_if_t<is_iterator<InputIt>::value>>
        //! @brief イテレータ範囲 [first, last) の値を代入する。
        constexpr void assign(InputIt first, InputIt last) {
            size_type count = std::distance(first, last);
            if (count > N) throw std::overflow_error("FixedVector: overflow");
            s32 i = 0;
            for (auto itr = first; first != last; ++itr) {
                m_data[i++] = *itr;
            }
            m_size = count;
        }

        //! @brief 末尾に要素をコピー追加する。
        constexpr void push_back(const T& value) {
            if (m_size >= N) throw std::overflow_error("FixedVector: overflow");
            m_data[m_size++] = value;
        }

        //! @brief 末尾に要素をムーブ追加する。
        constexpr void push_back(T&& value) {
            if (m_size >= N) throw std::overflow_error("FixedVector: overflow");
            m_data[m_size++] = std::move(value);
        }

        template<class... Args>
        //! @brief 末尾に要素を直接構築する。
        constexpr reference emplace_back(Args&&... args) {
            if (m_size >= N) throw std::overflow_error("FixedVector: overflow");
            m_data[m_size] = T(std::forward<Args>(args)...);
            return m_data[m_size++];
        }

        //! @brief 末尾要素を取り除く。
        constexpr void pop_back() {
            if (m_size > 0) --m_size;
        }

        //! @brief 指定位置の前に要素をコピー挿入する。
        constexpr iterator insert(const_iterator pos, const T& value) {
            return insert_impl(pos, value);
        }

        //! @brief 指定位置の前に要素をムーブ挿入する。
        constexpr iterator insert(const_iterator pos, T&& value) {
            return insert_impl(pos, std::move(value));
        }

        //! @brief 指定位置の前に同じ値を count 個挿入する。
        constexpr iterator insert(const_iterator pos, size_type count, const T& value) {
            if (m_size + count > N) throw std::overflow_error("FixedVector: overflow");
            auto index = pos - cbegin();
            if (count > 0) {
                std::move_backward(begin() + index, end(), end() + count);
                std::fill(begin() + index, begin() + index + count, value);
                m_size += count;
            }
            return begin() + index;
        }

        template<class InputIt, std::enable_if_t<is_iterator<InputIt>::value>>
        //! @brief 指定位置の前にイテレータ範囲 [first, last) を挿入する。
        constexpr iterator insert(const_iterator pos, InputIt first, InputIt last) {
            size_type count = std::distance(first, last);
            if (m_size + count > N) throw std::overflow_error("FixedVector: overflow");
            auto index = pos - cbegin();
            if (count > 0) {
                std::move_backward(begin() + index, end(), end() + count);
                std::copy(first, last, begin() + index);
                m_size += count;
            }
            return begin() + index;
        }

        //! @brief 指定位置の前に初期化リストの要素を挿入する。
        constexpr iterator insert(const_iterator pos, std::initializer_list<T> ilist) {
            return insert(pos, ilist.begin(), ilist.end());
        }

        template<class... Args>
        //! @brief 指定位置の前に要素を直接構築して挿入する。
        constexpr iterator emplace(const_iterator pos, Args&&... args) {
            if (m_size >= N) throw std::overflow_error("FixedVector: overflow");
            auto index = pos - cbegin();
            if (index < m_size) {
                std::move_backward(begin() + index, end(), end() + 1);
            }
            m_data[index] = T(std::forward<Args>(args)...);
            ++m_size;
            return begin() + index;
        }

        //! @brief 指定位置の要素を取り除き、次の要素を指すイテレータを返す。
        constexpr iterator erase(const_iterator pos) {
            auto index = pos - cbegin();
            if (index < m_size) {
                std::move(begin() + index + 1, end(), begin() + index);
                --m_size;
            }
            return begin() + index;
        }

        //! @brief 範囲 [first, last) の要素を取り除き、次の要素を指すイテレータを返す。
        constexpr iterator erase(const_iterator first, const_iterator last) {
            auto start_index = first - cbegin();
            auto end_index = last - cbegin();
            if (start_index < end_index && end_index <= m_size) {
                std::move(begin() + end_index, end(), begin() + start_index);
                m_size -= (end_index - start_index);
            }
            return begin() + start_index;
        }

        //! @brief 内容を入れ替える。
        constexpr void swap(FixedVector& other) noexcept {
            if (this != &other) {
                std::swap_ranges(m_data.begin(), m_data.begin() + std::max(m_size, other.m_size), other.m_data.begin());
                std::swap(m_size, other.m_size);
            }
        }

        //! @brief 全要素を取り除き、要素数を 0 にする。
        constexpr void clear() noexcept {
            m_size = 0;
        }


        //===============================================================
        // 比較演算子
        //===============================================================

        //! @brief 等値比較を行う。
        constexpr bool operator==(const FixedVector& other) const {
            return m_size == other.m_size && std::equal(begin(), end(), other.begin());
        }

        //! @brief 非等値比較を行う。
        constexpr bool operator!=(const FixedVector& other) const {
            return !(*this == other);
        }

        //! @brief 小なり比較を行う。
        constexpr bool operator<(const FixedVector& other) const {
            return std::lexicographical_compare(begin(), end(), other.begin(), other.end());
        }

        //! @brief 以下比較を行う。
        constexpr bool operator<=(const FixedVector& other) const {
            return !(other < *this);
        }

        //! @brief 大なり比較を行う。
        constexpr bool operator>(const FixedVector& other) const {
            return other < *this;
        }

        //! @brief 以上比較を行う。
        constexpr bool operator>=(const FixedVector& other) const {
            return !(*this < other);
        }


    private:

        template<typename U>
        constexpr iterator insert_impl(const_iterator pos, U&& value) {
            if (m_size >= N) throw std::overflow_error("FixedVector: overflow");
            auto index = pos - cbegin();
            if (index < m_size) {
                std::move_backward(begin() + index, end(), end() + 1);
            }
            m_data[index] = std::forward<U>(value);
            ++m_size;
            return begin() + index;
        }

    private:
        std::array<T, N> m_data;
        size_type m_size = 0;
    };


}
