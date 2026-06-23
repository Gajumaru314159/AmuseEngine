//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/RPI/Render/Feature/MaterialRenderFeature.h>
#include <Amuse/RPI/Material/MaterialSystem.h>
#include <Amuse/RHI/RHI.h>
#include <Amuse/RPI/Render/RenderScene.h>
#include <Amuse/RPI/Render/RenderView.h>

namespace Amuse::RPI {

	static MaterialBlockDesc Convert(StringView name, const MaterialPropertiesDesc& desc) {
		MaterialBlockDesc blockDesc;
		blockDesc.name = name;
		blockDesc.textures = desc.textures;
		blockDesc.buffers = desc.buffers;
		blockDesc.matrices = desc.matrices;
		blockDesc.vectors = desc.vectors;
		blockDesc.scalars = desc.scalars;
		blockDesc.integers = desc.integers;
		return blockDesc;
	}

	MaterialSystem::MaterialSystem(const MaterialSystemDesc& desc) {

		m_descs.global = Convert("MaterialGlobal", desc.properties.global);
		m_descs.scene = Convert("MaterialScene", desc.properties.scene);
		m_descs.view = Convert("MaterialView", desc.properties.view);
		if (!RHI::Device::Instance().getConfig().enableBindless) {
			m_descs.global.layout = m_layouts.global = MaterialBlock::CreateLayout(m_descs.global);
			m_descs.scene.layout = m_layouts.scene = MaterialBlock::CreateLayout(m_descs.scene);
			m_descs.view.layout = m_layouts.view = MaterialBlock::CreateLayout(m_descs.view);
		}

		m_globalBlock = MaterialBlock(m_descs.global);
	}

	MaterialBlock MaterialSystem::createSceneBlock(StringView name) const {
		MaterialBlockDesc desc = m_descs.scene;
		desc.name = name;
		return MaterialBlock(desc);
	}

	MaterialBlock MaterialSystem::createViewBlock(StringView name) const {
		MaterialBlockDesc desc = m_descs.scene;
		desc.name = name;
		return MaterialBlock(desc);
	}

	MaterialBlockSet::MaterialBlockSet()
	{
		global = &MaterialSystem::Instance().getGlobalBlock();
	}

	MaterialBlockSet::MaterialBlockSet(RenderView& view)
	{
		global = &MaterialSystem::Instance().getGlobalBlock();
		if (auto feature = view.findFeature<MaterialRenderFeature>()) {
			scene = &feature->getSceneBlock();
		}
		if (view.contains<MaterialViewData>()) {
			this->view = &view.get<MaterialViewData>().block;
		}
	}

	MaterialBlockSet::MaterialBlockSet(RenderScene& scene)
	{
		global = &MaterialSystem::Instance().getGlobalBlock();
		if (auto feature = scene.findFeature<MaterialRenderFeature>()) {
			this->scene = &feature->getSceneBlock();
		}
	}


}