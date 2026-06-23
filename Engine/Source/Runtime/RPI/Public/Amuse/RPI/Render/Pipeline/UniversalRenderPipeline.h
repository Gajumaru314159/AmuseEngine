//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/RPI/Render/RenderPipeline.h>
#include <Amuse/RPI/Render/Pass/GBufferPass.h>
#include <Amuse/RPI/Render/Pass/DeferredPass.h>
#include <Amuse/RPI/Render/Pass/ImGuiPass.h>
#include <Amuse/RPI/Render/Pass/OutputPass.h>

namespace Amuse::RPI {

	//! @brief 標準的な GBuffer/Deferred/ImGui/Output を行う描画パイプライン
	class UniversalRenderPipeline : public RenderPipeline {
	public:
		//! @brief 標準描画パイプラインを生成する
		static Ref<UniversalRenderPipeline> Create() {
			return new UniversalRenderPipeline();
		}
	private:
		UniversalRenderPipeline() {}
	public:
		//! @brief 標準描画に必要な RenderFeature を登録する
		void setup(RenderScene& scene, RenderFeatureSet& features) const override;
		//! @brief RenderScene 単位の描画を登録する
		void render(FG& fg, RenderScene& scene) const override;
		//! @brief RenderView 単位の描画を登録する
		void render(FG& fg, RenderView& view) const override;
	private:
		// Global;

		// Local
		GBufferPass m_gbuffer;
		DeferredPass m_deferred;
		ImGuiPass m_imgui;
		OutputPass m_output;
	};

}
