//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/RPI/Render/RenderFeature.h>
#include <Amuse/RPI/Material/MaterialPropertiesDesc.h>

namespace Amuse::RPI {

	//! @brief      マテリアル描画機能
	class CameraRenderFeature : public RenderFeature {
	public:
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

		AMUSE_RTTI();

		CameraRenderFeature(RenderScene& scene) {

		}

	private:

	};

}