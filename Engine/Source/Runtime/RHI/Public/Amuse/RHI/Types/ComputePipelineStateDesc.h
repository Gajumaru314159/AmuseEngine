//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>
#include <Amuse/RHI/Forward.h>

namespace Amuse {
	//! @brief  コンピュートパイプラインステート定義
	//! @ingroup AmuseRHI
	struct ComputePipelineStateDesc {
		String					name;							//!< 名前
		Ref<RootSignature>		rootSignature;					//!< ルートシグネチャ
		Ref<Shader>				cs;								//!< コンピュートシェーダ
	};

}