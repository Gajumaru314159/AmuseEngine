//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>
#include <Amuse/Input/InputType.h>


//! @brief	ボタン定義マクロ
#define AMUSE_DECL_BUTTON(type,name) static constexpr Button name{ ID,type::name };

namespace Amuse {
	//! @brief		ボタン
	//! 
	//! @details	「キーボードのAボタン」や「マウスの左ボタン」など、トグル入力を扱います。
	//! @ingroup AmuseInput
	class Button {
	public:

		//! @brief	デフォルトコンストラクタ
		Button() = default;

		//! @brief			コンストラクタ
		//! @param deviceID	デバイスタイプ
		//! @param code		コード
		//! @param user		ユーザ番号
		constexpr Button(DeviceID deviceID, u32 code, u32 user = 0) :m_deviceId(deviceID),m_code(code), m_user(user) {}

		//! @brief	コンストラクタ
		template<typename T>
		constexpr Button(DeviceID type, T code, u32 user = 0) : Button(type, enum_cast(code), user) {}

	public:

		//! @brief	状態が state か
		//StringView name()const;

		//! @brief	状態が state か
		//TimeSpan pressedTimeSpan()const;


		//! @brief	状態が state か
		bool is(ButtonState state)const;

		//! @brief	押された瞬間か
		bool down()const;

		//! @brief	離された瞬間か
		bool up()const;

		//! @brief	押されているか
		bool pressed()const;

		//! @brief	離されているか
		bool released()const;

	public:

		//! @brief	バインド
		//! 
		//! @param handle	イベントハンドル
		//! @param func		コールバック
		bool bindDown(ButtonHandle& handle, const ButtonDelegate& func)const;
		//! @brief ボタン解放時イベントを登録する
		bool bindUp(ButtonHandle& handle, const ButtonDelegate& func)const;
		//! @brief ボタン押下中イベントを登録する
		bool bindPressed(ButtonHandle& handle, const ButtonDelegate& func)const;
		//! @brief ボタン押下時イベントを登録する
		bool bindReleased(ButtonHandle& handle, const ButtonDelegate& func)const;

	private:

		bool bind(ButtonState state, ButtonHandle& handle, const ButtonDelegate& func)const;

	private:
		DeviceID m_deviceId;
		u32 m_code;
		u32 m_user;
	};

}
