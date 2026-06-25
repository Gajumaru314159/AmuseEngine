//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>
#include <utility>
#include <Amuse/RHI/GraphicObject.h>
#include <Amuse/RHI/Types/VertexLayout.h>

namespace Amuse {
	//! @brief      頂点レイアウト
	//! @ingroup AmuseRHI
	class VertexLayout : public GraphicObject {
	public:

		//! @brief  生成
		static Ref<VertexLayout> Create(const VertexLayoutDesc& desc);

		//! @brief  生成
		template<class... TArgs>
		static Ref<VertexLayout> Create(StringView name, s32 vertexStride, TArgs&&... attributes) {
			VertexLayoutDesc desc;
			desc.name = name;
			desc.vertexStride = vertexStride;
			(desc.attributes.emplace_back(std::forward<TArgs>(attributes)), ...);
			return Create(desc);
		}

	public:

		//! @brief      定義を取得
		virtual const VertexLayoutDesc& getDesc()const = 0;

		//! @brief      名前を取得
		const String& getName()const override { return getDesc().name; }

	};

}
