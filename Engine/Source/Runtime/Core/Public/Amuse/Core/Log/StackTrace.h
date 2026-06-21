//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <stacktrace>
#include <Amuse/Core/CoreTypes.h>
#include <Amuse/Core/String/String.h>
#include <Amuse/Core/Template/Container/Vector.h>
#include <Amuse/Core/Template/Container/FixedVector.h>

namespace Amuse::Core {

	//! @brief      スタックトレース情報
	struct StackTraceElement {
		String name; //!< name
		String filename; //!< filename
		s32 line; //!< line
	};

	//! @brief      スタック情報を取得
	struct StackTrace {

		static constexpr auto MAX_ENTRY_COUNT = 32; //!< MAX_ENTRY_COUNT

		FixedVector<std::stacktrace_entry,MAX_ENTRY_COUNT> entries; //!< スタック情報のエントリ一覧

		//! @brief      スタック情報をキャプチャ
		static StackTrace Capture(s32 frameToSkip = 0);

		//! @brief      スタック情報をStackTraceElementに変換して取得
		auto elements()const -> FixedVector<StackTraceElement,MAX_ENTRY_COUNT>;

	};

}
