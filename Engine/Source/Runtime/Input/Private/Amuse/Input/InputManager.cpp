//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/Input/InputManager.h>

#include <Amuse/Input/Device/KeyboardDevice.h>
#include <Amuse/Input/Device/MouseDevice.h>
#include <Amuse/Input/Device/GamePadDevice.h>

#include <Amuse/Input/Config.h>

namespace Amuse{

    //! @brief  コンストラクタ
    InputModule::InputModule(InputConfig* config, WindowManager&) {

        if (config) {
            m_config = *config;
        }

        // キーボード
        if(m_config.useKeyboard) {
            auto device = std::make_unique<KeyboardDevice>();
            DeviceKey key{ device->getDeviceId() ,0 };
            m_devices[key] = std::move(device);
        }

    }

    //! @brief  デストラクタ
    InputModule::~InputModule() {
    }


    //! @brief  更新
    void InputModule::update() {
        if (m_mouseAdded == false && m_config.useMouse) {
            if (auto window = Window::Main(); window && window->isValid()) {

                auto device = std::make_unique<MouseDevice>(*window);
                DeviceKey key{ device->getDeviceId() ,0 };
                m_devices[key] = std::move(device);
                
                m_mouseAdded = true;
            }
        }

        for (auto& [id, device] : m_devices) {
            device->update();
        }
    
    }

    //! @brief  デバイス取得
    IInputDevice* InputModule::findDevice(DeviceID id,u32 user) {

        auto found = m_devices.find({ id,user });
        if (found == m_devices.end())return nullptr;
        return found->second.get();
    
    }

}
