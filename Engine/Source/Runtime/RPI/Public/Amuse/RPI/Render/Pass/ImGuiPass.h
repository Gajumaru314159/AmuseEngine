//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/RPI/Render/RenderPass.h>

namespace Amuse::RPI {

	AMUSE_EVENT_NOTIFIER(ImGui);

	//! @brief		RenderView毎のImGui描画処理
	class ImGuiPass : public RenderPass {
	public:
		//! @brief ImGuiPass の入力リソース
		struct Input {
			FGResource color; //!< カラー
		};
		//! @brief ImGuiPass の出力リソース
		struct Output {
			FGResource color; //!< カラー
		};
	public:
		//! @brief RTTI 情報を宣言する
		AMUSE_RTTI();

		//! @brief		コンストラクタ
		ImGuiPass();

		//! @brief		デストラクタ
		~ImGuiPass() override;

		//! @brief		描画
		Output render(FG& fg, RenderView& view, const Input& input) const;
	public:
		//! @brief RenderView に ImGui 描画タスクを追加する
		static void AddTask(RenderView& view, ImGuiHandle& handle, ImGuiDelegate func);
	};

}
