//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>
#include <Amuse/RHI/RHI.h>
#include <Amuse/RHI/Texture.h>

namespace Amuse {
	//! @brief      プリセットテクスチャ
	//! @ingroup AmuseRHI
	enum class PresetTexture {
		White,
		Gray,
		Black,
		Normal,
		Check,
	};

	//! @brief      プリセットサンプラー
	//! @ingroup AmuseRHI
	enum class PresetSampler {
		Default
	};

	//! @brief	システムリソース
	class SystemResource : public Singleton<SystemResource> {
	public:

		//! @brief  コンストラクタ
		SystemResource(Device&);

		//! @brief  デストラクタ
		~SystemResource() override;

		//! @brief  プリセットテクスチャ取得
		Ref<Texture> getPresetTexture(PresetTexture type);


		Ref<Sampler> getPresetSampler(PresetSampler type);

	private:

		HashMap<PresetTexture, Ref<Texture>> m_presetTextures;
		HashMap<PresetSampler, Ref<Sampler>> m_presetSamplers;

	};

}
