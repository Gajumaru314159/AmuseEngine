//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>
#include <Amuse/RHI/GraphicObject.h>
#include <Amuse/RHI/Types/DescriptorLayoutDesc.h>

namespace Amuse::RHI {
    using namespace Amuse::Core;

	//! @brief      デスクリプタレイアウト
	class DescriptorLayout :public GraphicObject {
	public:

		//! @brief インスタンスを生成する
		static Ref<DescriptorLayout> Create(const DescriptorLayoutDesc& desc);

	public:

		//! @brief getDesc を実行する
		virtual const DescriptorLayoutDesc& getDesc() const = 0;

	};

}
