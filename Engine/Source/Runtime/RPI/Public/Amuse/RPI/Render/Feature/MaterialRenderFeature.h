//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/RPI/Render/RenderFeature.h>
#include <Amuse/RPI/Render/RenderView.h>
#include <Amuse/RPI/Material/MaterialBlock.h>
#include <Amuse/RPI/Material/MaterialSystem.h>

#include <Amuse/RHI/CommandList.h>

namespace Amuse {


	//! @brief MaterialRenderFeature に渡す描画要求
	//! @ingroup AmuseRPI
	struct MaterialDrawPacket {
		Material* material; //!< マテリアル
		VertexLayout* layout; //!< 頂点レイアウト
		Buffer* vertices; //!< 頂点バッファ
		Buffer* indices; //!< インデックスバッファ
		DrawIndexedParam args; //!< 描画引数
	};

	//! @brief パイプライン解決後の描画項目
	//! @ingroup AmuseRPI
	struct MaterialDrawItem {
		PipelineState* pipeline; //!< パイプラインステート
		MaterialBlock* block; //!< マテリアルブロック
		Buffer* vertices; //!< 頂点バッファ
		Buffer* indices; //!< インデックスバッファ
		DrawIndexedParam args; //!< 描画引数
	};

	//! @brief マテリアルパス単位の描画項目リスト
	//! @ingroup AmuseRPI
	struct MaterialDrawPass {
		Vector<MaterialDrawItem> items; //!< 描画項目
	};

	//! @brief RenderView ごとのマテリアル描画データ
	struct MaterialViewData {
		MaterialBlock block; //!< ビューマテリアルブロック
		HashMap<String,MaterialDrawPass> passes; //!< パス別描画項目
	};

	//! @brief      マテリアル描画機能
	//! @ingroup AmuseRPI
	class MaterialRenderFeature : public RenderFeature {
	public:

		//! @brief RTTI 情報を宣言する
		AMUSE_RTTI();

		//! @brief シーン用マテリアル描画機能を生成する
		explicit MaterialRenderFeature(RenderScene& scene);

		//! @brief 描画要素を追加
		void addDrawPackets(RenderView* view,Span<MaterialDrawPacket> packets);

		//! @brief      描画
		bool render(RenderView* view,Ref<CommandList>& cmdList, MaterialBlockSet& blocks, StringView pass);

		//! @brief 指定ビューの描画データを破棄する
		void cleanup(RenderView* view);

		//! @brief シーン用マテリアルブロックを取得する
		MaterialBlock& getSceneBlock() { return m_block; }




	private:

		MaterialBlock m_block;

	};

}
