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

namespace Amuse {

	//! @brief  説明
	//! @ingroup AmuseRPI
	class MaterialManager : public Singleton<MaterialManager> {
	public:

		MaterialManager(Device&, NameDictionary&);
		~MaterialManager();

		//!	@brief			レイアウトID取得
		VertexLayoutId getVertexLayoutId(const Ref<VertexLayout>& layout);

	public:

		void recordGlobalShaderProperties(Ref<CommandList>&);

	private:

	private:

		SpinLock m_lock;
		Map<Ref<VertexLayout>, VertexLayoutId> m_vertexLayoutCache;

	};

}
