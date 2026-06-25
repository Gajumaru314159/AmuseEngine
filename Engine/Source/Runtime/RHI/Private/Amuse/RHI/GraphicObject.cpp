//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/RHI/GraphicObject.h>
#include <Amuse/RHI/GraphicObjectManager.h>

namespace Amuse {
    //! @brief  コンストラクタ
    GraphicObject::GraphicObject() {
    }


    //! @brief  デストラクタ
    GraphicObject::~GraphicObject() {
    }

    //! @brief  
    void GraphicObject::manage() {
        if (!m_managed) {
            if (auto manager = GraphicObjectManager::Get()) {
                manager->registerObject(*this);
                m_managed = true;
            }
        }
    }

    //! @brief      終了処理
    void GraphicObject::finalize() {
        if (m_managed) {
            if (auto manager = GraphicObjectManager::Get()) {
                manager->requestRelease(*this);
            }
        } else {
            delete this;
        }
    }

}