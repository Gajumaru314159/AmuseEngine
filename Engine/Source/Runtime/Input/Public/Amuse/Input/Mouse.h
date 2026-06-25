//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>
#include <Amuse/Input/Button.h>
#include <Amuse/Input/Axis.h>

namespace Amuse {
	//! @brief マウス入力デバイス
	//! @ingroup AmuseInput
	class Mouse {
	public:
		//! @brief 入力デバイスIDを宣言する
		AMUSE_DECL_INPUT_DEVICE_ID("Mouse");

		//! @brief      ボタン取得
		static constexpr Button GetButton(MouseButton key) { return { ID,key }; }

		//! @brief      軸取得
		static constexpr Axis GetAxis(MouseAxis axis) { return { ID,axis}; }

		//! @brief  マウスポインタの座標を取得
		static Vec2 GetPos();

		//! @brief  マウスポインタの移動量を取得
		static Vec2 GetDeltaPos();

	public:

		AMUSE_DECL_BUTTON(MouseButton,Left);	//!< 左ボタン
		AMUSE_DECL_BUTTON(MouseButton,Right);	//!< 右ボタン
		AMUSE_DECL_BUTTON(MouseButton,Middle);	//!< 中ボタン
		AMUSE_DECL_BUTTON(MouseButton,X1);		//!< 拡張1ボタン
		AMUSE_DECL_BUTTON(MouseButton,X2);		//!< 拡張21ボタン
		AMUSE_DECL_BUTTON(MouseButton,X3);		//!< 拡張3ボタン
		AMUSE_DECL_BUTTON(MouseButton,X4);		//!< 拡張4ボタン
		AMUSE_DECL_AXIS(MouseAxis,Wheel);		//!< ホイール
		AMUSE_DECL_AXIS(MouseAxis,WheelX);		//!< ホイールX
		AMUSE_DECL_AXIS(MouseAxis,X);			//!< 座標X
		AMUSE_DECL_AXIS(MouseAxis,Y);			//!< 座標Y
		AMUSE_DECL_AXIS(MouseAxis,DeltaX);		//!< 移動量X
		AMUSE_DECL_AXIS(MouseAxis,DeltaY);		//!< 移動量Y

	};

}
