//***********************************************************
//! @file
//! @author Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/CorePrivate.h>
#include <Amuse/Core/Misc/CAPI.h>

namespace Amuse {

	//! @brief IPv4 アドレスを 4 オクテットで保持する。
	//! @details
	//! @code{.cpp}
	//! IPAddress ip(192, 168, 0, 1);
	//! IPAddress ip(0xc0a80001);
	//! IPAddress ip("192.168.0.1");
	//! @endcode
	//! @ingroup AmuseCore
	struct IPAddress {
		IPAddress() :a(0), b(0), c(0), d(0) {}
		//! @brief IPAddress を初期化する。
		IPAddress(u8 a, u8 b, u8 c, u8 d) :a(a), b(b), c(c), d(d) {}
		//! @brief IPAddress を初期化する。
		IPAddress(u32 ip) {
			a = (ip >> 24) & 0xff;
			b = (ip >> 16) & 0xff;
			c = (ip >> 8) & 0xff;
			d = ip & 0xff;
		}
		//! @brief IPAddress を初期化する。
		IPAddress(const char* ip) {
			if (SScanf(ip, "%hhu.%hhu.%hhu.%hhu", &a, &b, &c, &d)) {}
		}

		//! @brief ループバックアドレス 127.0.0.1 を返す。
		static IPAddress LocalHost() { return IPAddress(127, 0, 0, 1); }

		u8 a; //!< 第1オクテット
		u8 b; //!< 第2オクテット
		u8 c; //!< 第3オクテット
		u8 d; //!< 第4オクテット
	};

}


//===============================================================
// フォーマット
//===============================================================
//! @cond
template <> struct std::formatter<Amuse::IPAddress, Amuse::Char> : std::formatter<Amuse::f32, Amuse::Char> {
	template<typename FormatContext>
	auto format(const Amuse::IPAddress& value, FormatContext& ctx) const {
		ctx.advance_to(format_to(ctx.out(), "("));
		ctx.advance_to(format(value.a, ctx));
		ctx.advance_to(format_to(ctx.out(), ","));
		ctx.advance_to(format(value.b, ctx));
		ctx.advance_to(format_to(ctx.out(), ","));
		ctx.advance_to(format(value.c, ctx));
		ctx.advance_to(format_to(ctx.out(), ","));
		ctx.advance_to(format(value.d, ctx));
		ctx.advance_to(format_to(ctx.out(), ")"));
		return ctx.out();
	}
};
//! @endcond
