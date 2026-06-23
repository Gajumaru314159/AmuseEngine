//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/RHI/Forward.h>
#include <Amuse/RPI/Forward.h>
#include <Amuse/Core/Utility/Swapper.h>
#include <Amuse/Core/Utility/HandleManager.h>
#include <Amuse/RPI/FrameGraph/FG.h>
#include <Amuse/RPI/FrameGraph/FGDataWriter.h>


namespace Amuse::RPI {

    class MaterialManager;

    //! @brief      システムをServiceInjectorに登録
    void RegisterGraphicsService(ServiceInjector&);

    //! @brief      グラフィック
    class Graphics :public Singleton<Graphics> {
    public:

        //! @brief      コンストラクタ
        Graphics(RHI::Device& rhi, MaterialManager&);

        //! @brief      デストラクタ
        ~Graphics();

        //! @brief      ゲームループごとの更新を実行する
        void update();

        //! @brief 並列更新の準備を開始する
        void beginForParallel(s32 threadNum);
        //! @brief 指定スレッドの並列更新を実行する
        void updateForParallel(s32 threadIndex);


        //! @brief      ネイティブの描画コマンドを発行して描画処理を開始する
        void execute();

        //! @brief      ネイティブの描画コマンドを発行して描画処理を開始する
        void wait();

        //! @brief      シーンを追加
        //! @note       追加したシーンはGraphicsの終了までに removeScene で削除される必要があります。
        void addScene(RenderScene* scene);

        //! @brief      シーンを削除
        void removeScene(RenderScene* scene);

		//! @brief      シーンを取得
        RenderScene* getScene() {
            if (m_scenes.empty())return nullptr;
            return m_scenes.front();
        }

		//! @brief      前フレームのFrameGraphのデータを取得
        FGData getFGData()const;

    private:

        RHI::Device& m_rhi; //!< RHI デバイス

        UPtr<FG> m_fg{ nullptr }; //!< フレームグラフ

        Swapper<Ref<RHI::CommandList>> m_commandLists; //!< コマンドリスト

        Pimpl<FGResourcePool> m_fgResourcePool; //!< フレームグラフリソースプール

        Vector<RenderScene*> m_scenes; //!< 描画シーン
    };

}
