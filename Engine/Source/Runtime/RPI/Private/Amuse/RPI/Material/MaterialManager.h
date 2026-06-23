//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/RHI/Forward.h>
#include <Amuse/RHI/VertexLayout.h>
#include <Amuse/RPI/Material/MaterialInternalTypes.h>
#include <Amuse/RPI/Material/MaterialBlock.h>
#include <Amuse/RHI/DescriptorLayout.h>

namespace Amuse::RPI {

	//! @brief  説明
	class MaterialManager : public Singleton<MaterialManager> {
	public:

		MaterialManager(RHI::Device&, NameDictionary&);
		~MaterialManager();

		//!	@brief			レイアウトID取得
		VertexLayoutId getVertexLayoutId(const Ref<RHI::VertexLayout>& layout);

	public:

		void recordGlobalShaderProperties(Ref<RHI::CommandList>&);

	private:

	private:

		SpinLock m_lock;
		Map<Ref<RHI::VertexLayout>, VertexLayoutId> m_vertexLayoutCache;

	};

}
