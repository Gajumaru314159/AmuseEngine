//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/RHI/RHI.h>
#include <Amuse/RHI/GraphicObjectManager.h>

namespace Amuse::RHI {
    using namespace Amuse::Core;

    //! @brief  コンストラクタ
    Device::Device(GraphicObjectManager& manager, const RHIConfig* config)
        : m_config(config? *config:RHIConfig())
        , m_objectManager(manager)
    {
    }


    //! @brief  デストラクタ
    Device::~Device() {
    }


    //! @brief  更新
    void Device::update() {
        m_objectManager.update();
    }


    //! @brief  終了処理
    void Device::finalize() {
        m_objectManager.finalize();
    }
}