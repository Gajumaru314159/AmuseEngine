//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/RPI/Render/RenderFeature.h>
#include <Amuse/RPI/Material/MaterialPropertiesDesc.h>

namespace Amuse {

	//! @brief      マテリアル描画機能
	//! @ingroup AmuseRPI
	class CameraRenderFeature : public RenderFeature {
	public:
		//! @brief カメラ描画で使用するマテリアルプロパティを取得する
		static MaterialPropertiesSetDesc GetProperties() {
			MaterialPropertiesSetDesc desc;
			desc.global.matrices = {
				"MatrixTest"
			};
			desc.view.matrices = {
				"MatrixV",
				"MatrixP",
				"MatrixVP",
				"MatrixInvV",
				"MatrixInvP",
				"MatrixInvVP",
			};
			desc.view.vectors = {
				"CameraPos",
				"ScreenSize",
				"CameraUp",
				"CameraRight",
				"CameraFront",
			};
			return desc;
		}
	public:

		//! @brief RTTI 情報を宣言する
		AMUSE_RTTI();

		//! @brief シーン用カメラ描画機能を生成する
		CameraRenderFeature(RenderScene& scene) {

		}

	private:

	};

}
