//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
//! @cond
#include <Amuse/Core/HAL/Platform.h>

//===============================================================
// ログ
//===============================================================

#define AMUSE_ENABLE_LOG_FATAL
#define AMUSE_ENABLE_LOG_ERROR
#define AMUSE_ENABLE_LOG_WARNING
#define AMUSE_ENABLE_LOG_INFO


//===============================================================
// アサーション
//===============================================================

#define AMUSE_ENABLE_REQUIRE
#define AMUSE_ENABLE_ENSURE


//===============================================================
// エディタ
//===============================================================
#define AMUSE_EDITOR 1

//===============================================================
// デバッグ
//===============================================================

#define AMUSE_DEBUG_MUTEX 1
#define AMUSE_DEBUG_SPIN_LOCK 1

//===============================================================
// プラットフォーム毎の定義で置き換え
//===============================================================
#if defined(OS_WINDOWS)
#include "Windows/BuildConfig.h"

#elif defined(OS_LINUX)
#include "Linux/BuildConfig.h"

#else
#pragma error("Config("OS_NAME")がありません")
#endif
//! @endcond