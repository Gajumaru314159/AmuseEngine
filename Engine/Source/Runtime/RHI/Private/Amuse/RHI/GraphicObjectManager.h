//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>
#include <Amuse/RHI/GraphicObject.h>
#include <Amuse/RHI/Config.h>
#include <Amuse/Core/Utility/Swapper.h>

namespace Amuse {
#define GRAPHIC_OBJECT_LEAK_CHECK_ENABLED AMUSE_DEBUG

    //! @brief  グラフィック・オブジェクト・マネージャ
    //! @ingroup AmuseRHI
    class GraphicObjectManager : public Singleton<GraphicObjectManager> {
    public:

        //! @brief  コンストラクタ
        GraphicObjectManager(const RHIConfig*);

        //! @brief  デストラクタ
        ~GraphicObjectManager() override;

        //! @brief  更新
        void update();

        //! @brief  終了処理
        void finalize();

        //! @brief  登録
        void registerObject(GraphicObject&);

        //! @brief      解放予約
        //! 
        //! @details    指定フレーム更新後に削除するよう予約します。
        void requestRelease(GraphicObject&);

        //! @brief      フレームバッファ数
        s32 getFrameBufferCount()const { return m_config.frameBufferCount; }

    private:

        using ObjectQueue = Queue<GraphicObject*>;
        RHIConfig                  m_config;

        SpinLock                m_deleteStackListLock;
        Swapper<ObjectQueue>    m_deleteStackList;

#if GRAPHIC_OBJECT_LEAK_CHECK_ENABLED
        SpinLock                m_objectsLock;
        List<GraphicObject*>    m_objects;
#endif
    };

}