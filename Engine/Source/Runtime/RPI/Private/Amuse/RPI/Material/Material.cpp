//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/RPI/Material/Material.h>
#include <Amuse/RPI/Material/MaterialShader.h>
#include <Amuse/RPI/Material/MaterialSystem.h>
#include <Amuse/RPI/Mesh/Mesh.h>
#include <Amuse/RHI/CommandList.h>
#include <Amuse/RHI/Sampler.h>

namespace Amuse {

	//! @brief  生成
	Ref<Material> Material::Create(const Ref<MaterialShader>& shader) {
		return new Material(shader);
	}

	//! @brief  生成
	Ref<Material> Material::Create(const MaterialShaderDesc& desc) {
		return Create(MaterialShader::Create(desc));
	}

	Material::Material(const Ref<MaterialShader>& shader) :m_shader(shader) {
		AMUSE_ASSERT_EXPR(m_shader);
		m_block = MaterialBlock(shader->getBlockDesc());
	}

	const MaterialShaderDesc& Material::getDesc() const {
		return m_shader->getDesc();
	}

	bool Material::hasProperty(StringView name, MaterialPropertyType type) const {
		return m_block.hasProperty(name, type);
	}

	void Material::setTexture(StringView name, const Ref<Texture>& value) {
		m_block.setTexture(name, value, Sampler::Default());
	}

	void Material::setBuffer(StringView name, const Ref<Buffer>& value) {
		m_block.setBuffer(name, value);
	}

	void Material::setMatrix(StringView name, const Matrix& value) {
		m_block.setMatrix(name, value);
	}

	void Material::setVector(StringView name, Color value) {
		m_block.setVector(name, value);
	}

	void Material::setScalar(StringView name, f32 value) {
		m_block.setScalar(name, value);
	}

	void Material::setInteger(StringView name, f32 value) {
		m_block.setInteger(name, value);
	}

	s32 Material::calcQualityIndex(StringView pass, s32 quality) const {
		return m_shader->calcQualityIndex(pass, quality);
	}

	bool Material::prepare(const Ref<Mesh>& mesh) {
		if (!mesh) return false;
		if (!mesh->lods.empty()) {
			return m_shader->prepare(mesh->lods[0].buffer.layout);
		}

		for (auto& lod : mesh->lods) {
			if (!m_shader->prepare(lod.buffer.layout)) {
				return false;
			}
		}

		return true;
	}

	void Material::record(Ref<CommandList>& commandList, MaterialBlockSet& blocks, const Ref<Mesh>& mesh, s32 submesh, StringView passName, s32 quality) {

		if (!mesh) return;
		Ref<VertexLayout> layout;
		if (!mesh->lods.empty()) {
			layout = mesh->lods[0].buffer.layout;
		} else {
			return;
		}

		Ref<PipelineState> pipeline = m_shader->getPipeline(layout, passName, quality);
		if (!pipeline) return;

		commandList->setPipelineState(pipeline);

		m_block.record(commandList, 0);
		if (blocks.global)blocks.global->record(commandList, 1);
		if (blocks.scene)blocks.scene->record(commandList, 2);
		if (blocks.view)blocks.view->record(commandList, 3);


		auto& buffer = mesh->lods[0].buffer;

		if (buffer.indices) {
			commandList->setIndexBuffer(buffer.indices);
		}
		if (buffer.vertices) {

			auto section = mesh->lods[0].sections.at(submesh);

			DrawIndexedParam param{};
			param.startVertex = section.baseVertex;
			param.startIndex = section.indexStart;
			param.indexCount = section.indexCount;

			commandList->setVertexBuffer(buffer.vertices);
			commandList->drawIndexed(param);

		}
	}


	//! @brief  グローバルマテリアルパラメータを設定
	void Material::SetGlobalTexture(StringView name, const Ref<Texture>& value) {
		if (auto system = MaterialSystem::Get()) {
			system->getGlobalBlock().setTexture(name, value, Sampler::Default());
		}
	}

	//! @brief  グローバルマテリアルパラメータを設定
	void Material::SetGlobalBuffer(StringView name, const Ref<Buffer>& value) {
		if (auto system = MaterialSystem::Get()) {
			system->getGlobalBlock().setBuffer(name, value);
		}
	}

	//! @brief  グローバルマテリアルパラメータを設定
	void Material::SetGlobalMatrix(StringView name, const Matrix& value) {
		if (auto system = MaterialSystem::Get()) {
			system->getGlobalBlock().setMatrix(name, value);
		}
	}

	//! @brief  グローバルマテリアルパラメータを設定
	void Material::SetGlobalVector(StringView name, Color value) {
		if (auto system = MaterialSystem::Get()) {
			system->getGlobalBlock().setVector(name, value);
		}
	}

	//! @brief  グローバルマテリアルパラメータを設定
	void Material::SetGlobalScalar(StringView name, f32 value) {
		if (auto system = MaterialSystem::Get()) {
			system->getGlobalBlock().setScalar(name, value);
		}
	}

	//! @brief  グローバルマテリアルパラメータを設定
	void Material::SetGlobalInteger(StringView name, s32 value) {
		if (auto system = MaterialSystem::Get()) {
			system->getGlobalBlock().setInteger(name, value);
		}
	}

}
