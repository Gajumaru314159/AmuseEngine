//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/RPI/Render/Feature/MaterialRenderFeature.h>
#include <Amuse/RPI/Material/Material.h>
#include <Amuse/RPI/Mesh/Mesh.h>
#include <Amuse/RHI/CommandList.h>
#include <Amuse/RHI/PipelineState.h>
#include "Amuse/RPI/Material/MaterialShader.h"

namespace Amuse {


	//! @brief      コンストラクタ
	MaterialRenderFeature::MaterialRenderFeature(RenderScene&) {
		m_block = MaterialSystem::Instance().createSceneBlock("Test");
	}

	void MaterialRenderFeature::addDrawPackets(RenderView* view,Span<MaterialDrawPacket> packets) {
		if(!view) return;
		auto& mvd = view->get<MaterialViewData>();
		for(auto& packet:packets) {
			MaterialDrawItem item;
			item.block = &packet.material->getBlock();
			item.vertices = packet.vertices;
			item.indices = packet.indices;
			item.args = packet.args;

			auto& shader =packet.material->getShader();
			for(auto& [passName, pass]:shader->getDesc().passes) {
				item.pipeline = shader->getPipeline(packet.layout,passName,0);
				mvd.passes[passName].items.push_back(item);
			}
		}
	}

	//! @brief      描画
	bool MaterialRenderFeature::render(RenderView* view,Ref<CommandList>& commandList,MaterialBlockSet& blocks, StringView passName) {

		if (!view) return false;
		if (!commandList) return false;
		auto& mvd = view->get<MaterialViewData>();


		for(auto& item : mvd.passes[String(passName)].items) {
			Ref<PipelineState> pipeline = item.pipeline;
			commandList->setPipelineState(pipeline);
			m_block.record(commandList,0); // scene
			mvd.block.record(commandList,1); // view
			item.block->record(commandList, 2); // material
			commandList->setVertexBuffer(item.vertices);
			commandList->setIndexBuffer(item.indices);
			commandList->drawIndexed(item.args);
		}

		return true;
	}

	void MaterialRenderFeature::cleanup(RenderView* view) {
		if(!view) return;
		auto& mvd = view->get<MaterialViewData>();
		for(auto& [passName,pass]:mvd.passes) {
			pass.items.clear();
		}
	}

}
