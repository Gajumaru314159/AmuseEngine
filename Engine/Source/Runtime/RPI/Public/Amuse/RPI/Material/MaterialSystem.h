//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Utility/DI.h>
#include <Amuse/RPI/Forward.h>
#include <Amuse/RPI/Material/MaterialDesc.h>
#include <Amuse/RPI/Material/MaterialBlock.h>
#include <Amuse/RPI/Material/MaterialPropertiesDesc.h>
#include <Amuse/RHI/VertexLayout.h>

namespace Amuse {

	//! @brief マテリアルシステム初期化設定
	//! @ingroup AmuseRPI
	struct MaterialSystemDesc {
		MaterialPropertiesSetDesc properties; //!< プロパティ定義
	};

	//! @brief マテリアルシステム
	//! 
	//! マテリアルを使用するために必要な管理機構を提供します。
	//! 事前に各RenderFeatureから必要なマテリアルのプロパティを収集して初期化することで、RenderSceneやRenderViewの生成時に
	//! 自動的に MaterialBlock を生成します。
	//! @ingroup AmuseRPI
	class MaterialSystem : public Singleton<MaterialSystem> {
	public:
		//! @brief グローバル・シーン・ビュー別のレイアウト
		struct MaterialLayoutSet {
			Ref<DescriptorLayout> global; //!< グローバルレイアウト
			Ref<DescriptorLayout> scene; //!< シーンレイアウト
			Ref<DescriptorLayout> view; //!< ビューレイアウト
		};
		//! @brief グローバル・シーン・ビュー別のブロック定義
		//! @ingroup AmuseRPI
		struct MateriaBlockDescSet {
			MaterialBlockDesc global; //!< グローバルブロック定義
			MaterialBlockDesc scene; //!< シーンブロック定義
			MaterialBlockDesc view; //!< ビューブロック定義
		};
		using VertexLayout = Amuse::VertexLayout; //!< 頂点レイアウト型
	public:

		//! @brief マテリアルシステムを初期化する
		MaterialSystem(const MaterialSystemDesc& desc, Device&);

		//! @brief 共通レイアウトを取得する
		const MaterialLayoutSet& getLayouts() const { return m_layouts; }

		//! @brief シーン用マテリアルブロックを生成する
		MaterialBlock createSceneBlock(StringView name) const;

		//! @brief ビュー用マテリアルブロックを生成する
		MaterialBlock createViewBlock(StringView name) const;

		//! @brief グローバルマテリアルブロックを取得する
		MaterialBlock& getGlobalBlock() { return m_globalBlock; }
	private:

		MaterialLayoutSet		m_layouts;
		MateriaBlockDescSet		m_descs;
		MaterialBlock			m_globalBlock;	

		SpinLock					m_vertexLayoutsLock;
		Vector<Ref<VertexLayout>>	m_vertexLayouts;
	};

	//! @brief 描画時に使用するマテリアルブロック一式
	//! @ingroup AmuseRPI
	struct MaterialBlockSet {
		MaterialBlock* global = nullptr; //!< グローバルブロック
		MaterialBlock* scene = nullptr; //!< シーンブロック
		MaterialBlock* view = nullptr; //!< ビューブロック
		
		//! @brief 空のブロックセットを生成する
		MaterialBlockSet();
		//! @brief ビューからブロックセットを生成する
		MaterialBlockSet(RenderView& scene);
		//! @brief シーンからブロックセットを生成する
		MaterialBlockSet(RenderScene& scene);
	};

}
