//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/CorePrivate.h>
#include <Amuse/Core/Template/Container/FixedVector.h>
#include <Amuse/Core/String/StringView.h>
#include <Amuse/Core/String/Format.h>

namespace Amuse::Core {

	//! @brief ロケール
	//! @details IETF言語タグを表すクラス
	//!			 * language (言語)
	//! 		 * script (スクリプト)
	//! 		 * region (地域)
	//! 		 * variant (バリアント)
	//! 		 * extension (拡張)
	//! 		 * privateuse (プライベート使用)
	//!			 で構成され、それぞれの要素はハイフンで区切られる。
	//! 		 例: ja-JP-u-ca-japanese	
	//! @ref	 https://en.wikipedia.org/wiki/IETF_language_tag
	class Locale {
	public:

		Locale() = default;

		//! @brief IETFタグからロケールを生成する
		Locale(StringView tag);

		//! @brief 空か
		bool empty()const { return m_subtags.empty(); }

		//! @brief サブタグの数を取得する 
		size_t size()const { return m_subtags.size(); }

		//! @brief 対象のロケールが自身のロケールに含まれるか
		//! @details @code
		//!          Locale("en").contains(Locale("en-US")) == true
		//!          Locale("en-US").contains(Locale("en")) == false
		//!          Locale("en").contains(Locale("en")) == true
		//!          @endcode
		bool contains(const Locale& other) const;

		//! @brief 言語タグを取得する 
		StringView tag() const { return m_tag; }

		//! @brief サブタグを取得する
		StringView at(s32 index)const;

		//! @brief 言語サブタグを取得する 
		StringView language() const { return at(0); }

		//! @brief 文字体系サブタグを取得する 
		StringView script() const { return at(1); }

		//! @brief 地域サブタグを取得する 
		StringView region() const { return at(2); }

		//! @brief 異体サブタグを取得する 
		StringView variant() const { return at(3); }

		//! @brief 拡張サブタグを取得する 
		StringView extension() const { return at(4); }

		//! @brief 私用サブタグを取得する 
		StringView privateuse() const { return at(5); }

	public:

		//! @brief システム言語を取得する
		static Locale System();

	private:
		String m_tag;
		FixedVector<u8,8> m_subtags;
	};

}

//===============================================================
// フォーマット
//===============================================================
//! @cond
template <> struct std::formatter<Amuse::Core::Locale, Amuse::Core::Char> {
	template<typename ParseContext>
	constexpr auto parse(ParseContext& ctx) -> decltype(ctx.begin()) {
		return ctx.end();
	}

	template<typename FormatContext>
	auto format(const Amuse::Core::Locale& value, FormatContext& ctx) const {
		return format_to(ctx.out(), "{}", value.tag());
	}
};

//===============================================================
// ハッシュ化
//===============================================================
template<>
struct std::hash<Amuse::Core::Locale> {
	size_t operator()(const Amuse::Core::Locale& value)const noexcept {
		return std::hash<Amuse::Core::StringView>()(value.tag());
	}
};

//! @endcond