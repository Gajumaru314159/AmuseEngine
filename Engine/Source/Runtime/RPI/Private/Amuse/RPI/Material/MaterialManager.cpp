//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/RPI/Material/MaterialManager.h>
#include <Amuse/RPI/Material/MaterialSystem.h>
#include <Amuse/RHI/Texture.h>
#include <Amuse/RHI/Buffer.h>
#include <Amuse/RHI/Sampler.h>
#include <Amuse/RHI/DescriptorTable.h>
#include <Amuse/RHI/CommandList.h>
#include <Amuse/RHI/RootSignature.h>

namespace Amuse::RPI {

	enum class MaterialBlockScope {
		Material,
		Global,
		Scene,
		View,
		Draw,
	};


	//!	@brief			コンストラクタ
	MaterialManager::MaterialManager(RHI::Device&, NameDictionary&) {
	}

	//!	@brief			デストラクタ
	MaterialManager::~MaterialManager() {
	}

	//!	@brief			レイアウトID取得
	VertexLayoutId MaterialManager::getVertexLayoutId(const Ref<RHI::VertexLayout>& layout) {
		ScopeLock lock(m_lock);
		auto found = m_vertexLayoutCache.find(layout);
		if (found == m_vertexLayoutCache.end()) {
			return m_vertexLayoutCache[layout] = static_cast<VertexLayoutId>(m_vertexLayoutCache.size());
		} else {
			return found->second;
		}
	}

	//! @brief  
	void MaterialManager::recordGlobalShaderProperties(Ref<RHI::CommandList>& cmdList) {
		auto& m_block = MaterialSystem::Instance().getGlobalBlock();
		return m_block.record(cmdList, enum_cast(MaterialBlockScope::Global));
	}


}
