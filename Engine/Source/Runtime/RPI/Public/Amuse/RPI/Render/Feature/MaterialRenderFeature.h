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

namespace Amuse::RPI {


	//! @brief MaterialRenderFeature に渡す描画要求
	struct MaterialDrawPacket {
		Material* material; //!< マテリアル
		RHI::VertexLayout* layout; //!< 頂点レイアウト
		RHI::Buffer* vertices; //!< 頂点バッファ
		RHI::Buffer* indices; //!< インデックスバッファ
		RHI::DrawIndexedParam args; //!< 描画引数
	};

	//! @brief パイプライン解決後の描画項目
	struct MaterialDrawItem {
		RHI::PipelineState* pipeline; //!< パイプラインステート
		MaterialBlock* block; //!< マテリアルブロック
		RHI::Buffer* vertices; //!< 頂点バッファ
		RHI::Buffer* indices; //!< インデックスバッファ
		RHI::DrawIndexedParam args; //!< 描画引数
	};

	//! @brief マテリアルパス単位の描画項目リスト
	struct MaterialDrawPass {
		Vector<MaterialDrawItem> items; //!< 描画項目
	};

	//! @brief RenderView ごとのマテリアル描画データ
	struct MaterialViewData {
		MaterialBlock block; //!< ビューマテリアルブロック
		HashMap<String,MaterialDrawPass> passes; //!< パス別描画項目
	};

	//! @brief      マテリアル描画機能
	class MaterialRenderFeature : public RenderFeature {
	public:

		//! @brief RTTI 情報を宣言する
		AMUSE_RTTI();

		//! @brief シーン用マテリアル描画機能を生成する
		explicit MaterialRenderFeature(RenderScene& scene);

		//! @brief 描画要素を追加
		void addDrawPackets(RenderView* view,Span<MaterialDrawPacket> packets);

		//! @brief      描画
		bool render(RenderView* view,Ref<RHI::CommandList>& cmdList, MaterialBlockSet& blocks, StringView pass);

		//! @brief 指定ビューの描画データを破棄する
		void cleanup(RenderView* view);

		//! @brief シーン用マテリアルブロックを取得する
		MaterialBlock& getSceneBlock() { return m_block; }




	private:

		MaterialBlock m_block;

	};

}
