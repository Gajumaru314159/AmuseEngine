//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/CoreTypes.h>

namespace Amuse {

	//! @brief メモリサイズ値を表す。
	//! @ingroup AmuseCore
	struct MemorySize {

		size_t size; //!< サイズ

		constexpr MemorySize() : size(0) {}
		//! @brief MemorySize を初期化する。
		constexpr MemorySize(size_t size) : size(size) {}
		//! @brief size_t へ型変換する。
		constexpr operator size_t() const { return size; }
		//! @brief 2進接頭辞の KiB 単位へ整数変換する。
		constexpr size_t KiB() const { return size >> 10; }
		//! @brief 2進接頭辞の MiB 単位へ整数変換する。
		constexpr size_t MiB() const { return size >> 20; }
		//! @brief 2進接頭辞の GiB 単位へ整数変換する。
		constexpr size_t GiB() const { return size >> 30; }
		//! @brief 2進接頭辞の TiB 単位へ整数変換する。
		constexpr size_t TiB() const { return size >> 40; }
		//! @brief 2進接頭辞の KiB 単位へ浮動小数点変換する。
		constexpr f64 KiBF() const { return 1.0 * size / (1ull << 10); }
		//! @brief 2進接頭辞の MiB 単位へ浮動小数点変換する。
		constexpr f64 MiBF() const { return 1.0 * size / (1ull << 20); }
		//! @brief 2進接頭辞の GiB 単位へ浮動小数点変換する。
		constexpr f64 GiBF() const { return 1.0 * size / (1ull << 30); }
		//! @brief 2進接頭辞の TiB 単位へ浮動小数点変換する。
		constexpr f64 TiBF() const { return 1.0 * size / (1ull << 40); }
		//! @brief 10進接頭辞の KB 単位へ整数変換する。
		constexpr size_t KB() const { return size / 1000ull; }
		//! @brief 10進接頭辞の MB 単位へ整数変換する。
		constexpr size_t MB() const { return size / 1000000ull; }
		//! @brief 10進接頭辞の GB 単位へ整数変換する。
		constexpr size_t GB() const { return size / 1000000000ull; }
		//! @brief 10進接頭辞の TB 単位へ整数変換する。
		constexpr size_t TB() const { return size / 1000000000000ull; }
		//! @brief 10進接頭辞の KB 単位へ浮動小数点変換する。
		constexpr f64 KBF() const { return size / 1000.0; }
		//! @brief 10進接頭辞の MB 単位へ浮動小数点変換する。
		constexpr f64 MBF() const { return size / 1000000.0; }
		//! @brief 10進接頭辞の GB 単位へ浮動小数点変換する。
		constexpr f64 GBF() const { return size / 1000000000.0; }
		//! @brief 10進接頭辞の TB 単位へ浮動小数点変換する。
		constexpr f64 TBF() const { return size / 1000000000000.0; }

	};

}
