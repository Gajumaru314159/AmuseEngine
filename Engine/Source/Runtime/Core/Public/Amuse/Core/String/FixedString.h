//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <string>
#include <Amuse/Core/CoreTypes.h>
#include <Amuse/Core/String/StringView.h>

namespace Amuse::Core {

	//! @brief		固定長文字列基底
	template<class TChar, size_t N>
	class FixedStringBase {
		static_assert(0<N);
    public:
        //! @brief 格納する値の型。
        using value_type = TChar;
        //! @brief 要素ポインタ型。
        using pointer = TChar*;
        //! @brief 要素参照型。
        using reference = TChar&;
        //! @brief イテレータ差分を表す型。
        using difference_type = s64;

		FixedStringBase() {}
		//! @brief FixedStringBase を初期化する。
		FixedStringBase(StringViewBase<TChar> str) {
			size_t len = std::min(N - 1, str.size());
			std::memcpy(m_data, str.data(), len);
			m_data[len + 1] = static_cast<TChar>('\0');
		}

		//! @brief 末尾に要素を追加する。
		void push_back(TChar c) {
			if (N <= m_size + 1)return;
			m_data[m_size] = c;
			m_size++;
			m_data[m_size] = 0;
		}
		//! @brief 内部データへのポインタを取得する。
		value_type* data() { return m_data; }
		//! @brief 内部データへのポインタを取得する。
		const value_type* data()const { return m_data; }
		//! @brief 要素数を取得する。
		size_t size()const { return m_size; }

		//! @brief 指定した要素へアクセスする。
		value_type& operator[](size_t index) { return m_data[index]; }
		//! @brief 指定した要素へアクセスする。
		const value_type& operator[](size_t index) const{ return m_data[index]; }
	private:
		TChar m_data[N] = {};
		size_t m_size = 0;
	};

	template<size_t N>
	using FixedString = FixedStringBase<Char,N>;

}
