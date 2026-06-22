//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>
#include <Amuse/Input/Interface/IInputDevice.h>
#include <Amuse/Input/Config.h>

namespace Amuse::Platform {
    using namespace Amuse::Core;
    class WindowManager;
}

namespace Amuse::Input {
    using namespace Amuse::Core;

    //! @brief  説明
    class InputModule : public Singleton<InputModule> {
    public:

        //===============================================================
        // コンストラクタ / デストラクタ
        //===============================================================
        //! @brief 入力モジュールを初期化する
        InputModule(InputConfig*, Platform::WindowManager&);
        ~InputModule();

        //! @brief  更新
        void update();

        //! @brief  入力デバイスを生成
        template<class T,typename... Args>
        IInputDevice* createDevice([[maybe_unused]]Args&&... args) {
            AMUSE_NOTIMPLEMENTED();
            return nullptr;
        }

        //! @brief  入力デバイスを探す
        IInputDevice* findDevice(DeviceID id,u32 user);

    private:

        struct DeviceKey {
            DeviceID id{ 0 };
            u32 user{ 0 };
            bool operator==(const DeviceKey& rhs) const { return id == rhs.id && user == rhs.user; }
            bool operator!=(const DeviceKey& rhs) const { return !(*this == rhs); }
        };
        struct Hash {
            std::size_t operator()(const DeviceKey& key) const {
                return static_cast<size_t>(static_cast<size_t>(key.id) ^ ((size_t)key.user << 28));
            }
        };

        using DevicePtr = UPtr<IInputDevice>;
        HashMap<DeviceKey, DevicePtr, Hash> m_devices;

        InputConfig m_config;
        bool m_mouseAdded=false;
    };

}
