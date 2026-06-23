//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/RPI/Render/RenderFeature.h>
#include <Amuse/RPI/Render/RenderView.h>
#include <Amuse/RPI/Material/MaterialBlock.h>
#include <Amuse/RPI/Material/MaterialSystem.h>

#include <Amuse/RHI/CommandList.h>

namespace Amuse::RPI {


	struct MaterialDrawPacket {
		Material* material;
		RHI::VertexLayout* layout;
		RHI::Buffer* vertices;
		RHI::Buffer* indices;
		RHI::DrawIndexedParam args;
	};

	struct MaterialDrawItem {
		RHI::PipelineState* pipeline;
		MaterialBlock* block;
		RHI::Buffer* vertices;
		RHI::Buffer* indices;
		RHI::DrawIndexedParam args;
	};

	struct MaterialDrawPass {
		Vector<MaterialDrawItem> items;
	};

	struct MaterialViewData {
		MaterialBlock block;
		HashMap<String,MaterialDrawPass> passes;
	};

	//! @brief      マテリアル描画機能
	class MaterialRenderFeature : public RenderFeature {
	public:

		AMUSE_RTTI();

		explicit MaterialRenderFeature(RenderScene& scene);

		//! @brief 描画要素を追加
		void addDrawPackets(RenderView* view,Span<MaterialDrawPacket> packets);

		//! @brief      描画
		bool render(RenderView* view,Ref<RHI::CommandList>& cmdList, MaterialBlockSet& blocks, StringView pass);

		void cleanup(RenderView* view);

		MaterialBlock& getSceneBlock() { return m_block; }




	private:

		MaterialBlock m_block;

	};

}