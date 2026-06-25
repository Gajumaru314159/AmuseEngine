//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/DirectX12RHI/pch.h>
#include <Amuse/Core/Core.h>
namespace Amuse {
	//! @brief  デスクリプタ・タイプ
	//! @ingroup AmuseDirectX12RHI
	enum class DescriptorHeapType :u32 {
		CBV_SRV_UAV,    //!< 定数バッファ/シェーダリソース/アンオーダードアクセス
		RTV,            //!< レンダーターゲット
		DSV,            //!< デプスステンシル
		Sampler,        //!< サンプラー
	};

}