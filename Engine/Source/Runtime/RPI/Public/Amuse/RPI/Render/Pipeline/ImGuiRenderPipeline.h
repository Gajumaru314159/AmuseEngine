//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/RPI/Render/RenderPipeline.h>
#include <Amuse/RPI/Render/Pass/ImGuiPass.h>
#include <Amuse/RPI/Render/Pass/OutputPass.h>

namespace Amuse::RPI {

	//! @brief ImGui 表示用の描画パイプライン
	class ImGuiRenderPipeline : public RenderPipeline {
	public:
		//! @brief ImGui 描画パイプラインを生成する
		static Ref<ImGuiRenderPipeline> Create() {
			return new ImGuiRenderPipeline();
		}
	private:
		ImGuiRenderPipeline() {}
	public:
		//! @brief ImGui 描画に必要な RenderFeature を登録する
		void setup(RenderScene& scene, RenderFeatureSet& features) const override;
		//! @brief RenderView 単位の ImGui 描画を登録する
		void render(FG& fg, RenderView& view) const override;
	private:
		ImGuiPass m_imgui;
		OutputPass m_output;
	};

}
