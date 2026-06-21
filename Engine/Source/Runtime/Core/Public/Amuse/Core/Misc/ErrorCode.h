//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/CoreTypes.h>
#include <Amuse/Core/String/String.h>

namespace Amuse::Core {

    //! @brief エラーコード
    class ErrorCode {
    public:
        ErrorCode() : m_value(0) {}
        //! @brief ErrorCode を初期化する。
        ErrorCode(u32 value) : m_value(value) {}

        //! @brief 数値のエラーコードを取得する。
        u32 value() const { return m_value; }
        //! @brief エラーコードに対応するメッセージを取得する。
        String message() const;

    private:
        u32 m_value;
    };

}

//===============================================================
// フォーマット
//===============================================================
//! @cond
template <> struct std::formatter<Amuse::Core::ErrorCode, Amuse::Core::Char> {
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx) -> decltype(ctx.begin()) {
        return ctx.end();
    }

    template<typename FormatContext>
    auto format(Amuse::Core::ErrorCode value, FormatContext& ctx) const {
        return format_to(ctx.out(), "0x{:08X} {}", value.value(), value.message().c_str());
    }
};
//! @endcond
