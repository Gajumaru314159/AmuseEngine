//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/RPI/Render/RenderFeature.h>

namespace Amuse {

	//! @brief      ImGui描画機能
	//! @ingroup AmuseRPI
	class ImGuiRenderFeature : public RenderFeature {
	public:

		//! @brief RTTI 情報を宣言する
		AMUSE_RTTI();

		//! @brief シーン用 ImGui 描画機能を生成する
		ImGuiRenderFeature(RenderScene& scene);
		//! @brief デストラクタ
		virtual ~ImGuiRenderFeature();

	};

}
