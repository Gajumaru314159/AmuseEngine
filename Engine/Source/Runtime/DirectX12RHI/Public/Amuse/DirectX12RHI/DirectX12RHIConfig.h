//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/DirectX12RHI/pch.h>
#include <Amuse/Core/Core.h>

namespace Amuse {
	//! @brief DirectX 12 RHI 設定
	//! @ingroup AmuseDirectX12RHI
	struct DirectX12RHIConfig {
        bool enablePIX = false; //!< PIX 有効フラグ
        bool enableDebugLayer = false; //!< デバッグレイヤー有効フラグ
        bool breakWithWarning = false; //!< 警告時ブレークフラグ
        bool enableDirectStorage = true; //!< DirectStorage 有効フラグ
        bool enableDirectStorageDebug = false; //!< DirectStorage デバッグ有効フラグ
	};

}
