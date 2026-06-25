//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>
#include <Amuse/Input/Interface/IInputDevice.h>
#include <Amuse/Input/Keyboard.h>
#include <Amuse/Core/HAL/Platform.h>

#ifdef OS_WINDOWS
#include <Amuse/Core/Platform/WindowsHeaders.h>
#endif

#ifdef OS_LINUX
#include <X11/Xlib.h>
#include <X11/keysym.h>
#endif

namespace Amuse {
	//! @brief  キーボード・デバイス
	//! @ingroup AmuseInput
	class KeyboardDevice:public IInputDevice {
	public:

		//! @brief  コンストラクタ
		KeyboardDevice();

		//! @brief  デストラクタ
		~KeyboardDevice();

		//! @brief  デバイスID
		DeviceID getDeviceId()const override { return Keyboard::ID; }

		//! @brief  更新
		void update() override;


		//! @brief  ボタンの入力状態を取得
		ButtonStates getButtonStates(u32 code)const override;

		//! @brief  ボタン入力イベントをバインド
		bool bindButton(u32 code, ButtonState state, ButtonHandle& handle, const ButtonDelegate& func) override;

	private:
		//! @ingroup AmuseInput
		struct KeyState {
			HashMap<ButtonState,ButtonNotifier> notifiers;
			ButtonStates prev;
			ButtonStates next;
		};
	private:

    #ifdef OS_WINDOWS
		HWND m_hWnd = nullptr;
    #elif defined(OS_LINUX)
        ::Display* m_display = nullptr;
        bool m_ownDisplay = false;
    #endif
		HashMap<Key, KeyState> m_states;

	};


}
