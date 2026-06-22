//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>
#include <Amuse/Input/Button.h>
#include <Amuse/Core/Hash/Hash.h>

namespace Amuse::Input {
    using namespace Amuse::Core;

	//! @brief キーボード入力デバイス
	class Keyboard {
	public:
		//! @brief 入力デバイスIDを宣言する
		AMUSE_DECL_INPUT_DEVICE_ID("Keyboard");

		//! @brief      ボタン取得
		static constexpr Button GetButton(Key key) { return {ID,key}; }

	public:
        AMUSE_DECL_BUTTON(Key, F1);				//!< F1
        AMUSE_DECL_BUTTON(Key, F2);				//!< F2
        AMUSE_DECL_BUTTON(Key, F3);				//!< F3
        AMUSE_DECL_BUTTON(Key, F4);				//!< F4
        AMUSE_DECL_BUTTON(Key, F5);				//!< F5
        AMUSE_DECL_BUTTON(Key, F6);				//!< F6
        AMUSE_DECL_BUTTON(Key, F7);				//!< F7
        AMUSE_DECL_BUTTON(Key, F8);				//!< F8
        AMUSE_DECL_BUTTON(Key, F9);				//!< F9
        AMUSE_DECL_BUTTON(Key, F10);				//!< F10
        AMUSE_DECL_BUTTON(Key, F11);				//!< F11
        AMUSE_DECL_BUTTON(Key, F12);				//!< F12

        AMUSE_DECL_BUTTON(Key, Keypad0);			//!< テンキー0
        AMUSE_DECL_BUTTON(Key, Keypad1);			//!< テンキー1
        AMUSE_DECL_BUTTON(Key, Keypad2);			//!< テンキー2
        AMUSE_DECL_BUTTON(Key, Keypad3);			//!< テンキー3
        AMUSE_DECL_BUTTON(Key, Keypad4);			//!< テンキー4
        AMUSE_DECL_BUTTON(Key, Keypad5);			//!< テンキー5
        AMUSE_DECL_BUTTON(Key, Keypad6);			//!< テンキー6
        AMUSE_DECL_BUTTON(Key, Keypad7);			//!< テンキー7
        AMUSE_DECL_BUTTON(Key, Keypad8);			//!< テンキー8
        AMUSE_DECL_BUTTON(Key, Keypad9);			//!< テンキー9
        AMUSE_DECL_BUTTON(Key, KeypadPeriod);		//!< テンキー.
        AMUSE_DECL_BUTTON(Key, KeypadDivide);		//!< テンキー/
        AMUSE_DECL_BUTTON(Key, KeypadMultiply);	//!< テンキー*
        AMUSE_DECL_BUTTON(Key, KeypadSubstract);		//!< テンキー-
        AMUSE_DECL_BUTTON(Key, KeypadAdd);		//!< テンキー+
        AMUSE_DECL_BUTTON(Key, KeypadEnter);		//!< テンキーEnter

        AMUSE_DECL_BUTTON(Key, Alpha0);			//!< 数字キー0
        AMUSE_DECL_BUTTON(Key, Alpha1);			//!< 数字キー1
        AMUSE_DECL_BUTTON(Key, Alpha2);			//!< 数字キー2
        AMUSE_DECL_BUTTON(Key, Alpha3);			//!< 数字キー3
        AMUSE_DECL_BUTTON(Key, Alpha4);			//!< 数字キー4
        AMUSE_DECL_BUTTON(Key, Alpha5);			//!< 数字キー5
        AMUSE_DECL_BUTTON(Key, Alpha6);			//!< 数字キー6
        AMUSE_DECL_BUTTON(Key, Alpha7);			//!< 数字キー7
        AMUSE_DECL_BUTTON(Key, Alpha8);			//!< 数字キー8
        AMUSE_DECL_BUTTON(Key, Alpha9);			//!< 数字キー9

        AMUSE_DECL_BUTTON(Key, A);					//!< A
        AMUSE_DECL_BUTTON(Key, B);					//!< B
        AMUSE_DECL_BUTTON(Key, C);					//!< C
        AMUSE_DECL_BUTTON(Key, D);					//!< D
        AMUSE_DECL_BUTTON(Key, E);					//!< E
        AMUSE_DECL_BUTTON(Key, F);					//!< F
        AMUSE_DECL_BUTTON(Key, G);					//!< G
        AMUSE_DECL_BUTTON(Key, H);					//!< H
        AMUSE_DECL_BUTTON(Key, I);					//!< I
        AMUSE_DECL_BUTTON(Key, J);					//!< J
        AMUSE_DECL_BUTTON(Key, K);					//!< K
        AMUSE_DECL_BUTTON(Key, L);					//!< L
        AMUSE_DECL_BUTTON(Key, M);					//!< M
        AMUSE_DECL_BUTTON(Key, N);					//!< N
        AMUSE_DECL_BUTTON(Key, O);					//!< O
        AMUSE_DECL_BUTTON(Key, P);					//!< P
        AMUSE_DECL_BUTTON(Key, Q);					//!< Q
        AMUSE_DECL_BUTTON(Key, R);					//!< R
        AMUSE_DECL_BUTTON(Key, S);					//!< S
        AMUSE_DECL_BUTTON(Key, T);					//!< T
        AMUSE_DECL_BUTTON(Key, U);					//!< U
        AMUSE_DECL_BUTTON(Key, V);					//!< V
        AMUSE_DECL_BUTTON(Key, W);					//!< W
        AMUSE_DECL_BUTTON(Key, X);					//!< X
        AMUSE_DECL_BUTTON(Key, Y);					//!< Y
        AMUSE_DECL_BUTTON(Key, Z);					//!< Z

        AMUSE_DECL_BUTTON(Key, Exclaim);			//!< 感嘆符
        AMUSE_DECL_BUTTON(Key, DoubleQuote);		//!< 二重引用符
        AMUSE_DECL_BUTTON(Key, Hash);				//!< シャープ
        AMUSE_DECL_BUTTON(Key, Dollar);			//!< ドル記号
        AMUSE_DECL_BUTTON(Key, Percent);			//!< パーセント記号
        AMUSE_DECL_BUTTON(Key, Ampersand);			//!< アンパサンド
        AMUSE_DECL_BUTTON(Key, Quote);				//!< 引用符
        AMUSE_DECL_BUTTON(Key, LeftParen);			//!< 左丸括弧
        AMUSE_DECL_BUTTON(Key, RightParen);		//!< 右丸括弧
        AMUSE_DECL_BUTTON(Key, Asterisk);			//!< アスタリスク
        AMUSE_DECL_BUTTON(Key, Plus);				//!< プラス記号
        AMUSE_DECL_BUTTON(Key, Comma);				//!< カンマ
        AMUSE_DECL_BUTTON(Key, Minus);				//!< マイナス記号
        AMUSE_DECL_BUTTON(Key, Period);			//!< ピリオド
        AMUSE_DECL_BUTTON(Key, Slash);				//!< スラッシュ
        AMUSE_DECL_BUTTON(Key, Colon);				//!< コロン
        AMUSE_DECL_BUTTON(Key, Semicolon);			//!< セミコロン
        AMUSE_DECL_BUTTON(Key, Less);				//!< 小なり記号
        AMUSE_DECL_BUTTON(Key, Equals);			//!< 等号
        AMUSE_DECL_BUTTON(Key, Greater);			//!< 大なり記号
        AMUSE_DECL_BUTTON(Key, Question);			//!< 疑問符
        AMUSE_DECL_BUTTON(Key, At);				//!< アットマーク
        AMUSE_DECL_BUTTON(Key, LeftBracket);		//!< 左角括弧
        AMUSE_DECL_BUTTON(Key, Backslash);			//!< バックスラッシュ
        AMUSE_DECL_BUTTON(Key, RightBracket);		//!< 右角括弧
        AMUSE_DECL_BUTTON(Key, Caret);				//!< キャレット
        AMUSE_DECL_BUTTON(Key, Underscore);		//!< アンダースコア
        AMUSE_DECL_BUTTON(Key, BackQuote);			//!< バッククォート
        AMUSE_DECL_BUTTON(Key, LeftCurlyBracket);	//!< 左波括弧
        AMUSE_DECL_BUTTON(Key, Pipe);				//!< パイプ記号
        AMUSE_DECL_BUTTON(Key, RightCurlyBracket);	//!< 右波括弧
        AMUSE_DECL_BUTTON(Key, Tilde);				//!< チルダ

        AMUSE_DECL_BUTTON(Key, UpArrow);			//!< ↑
        AMUSE_DECL_BUTTON(Key, DownArrow);			//!< ↓
        AMUSE_DECL_BUTTON(Key, RightArrow);		//!< →
        AMUSE_DECL_BUTTON(Key, LeftArrow);			//!< ←

        AMUSE_DECL_BUTTON(Key, Escape);			//!< Escape
        AMUSE_DECL_BUTTON(Key, Space);				//!< Space
        AMUSE_DECL_BUTTON(Key, Tab);				//!< Tab
        AMUSE_DECL_BUTTON(Key, Backspace);			//!< Backspace
        AMUSE_DECL_BUTTON(Key, Delete);			//!< Delete
        AMUSE_DECL_BUTTON(Key, Enter);				//!< Enter / Return
        AMUSE_DECL_BUTTON(Key, Insert);			//!< Insert
        AMUSE_DECL_BUTTON(Key, CapsLock);			//!< CapsLock
        AMUSE_DECL_BUTTON(Key, PrintScreen);		//!< PrintScreen
        AMUSE_DECL_BUTTON(Key, ScrollLock);		//!< ScrollLock

        AMUSE_DECL_BUTTON(Key, Pause);				//!< Pause / Break
        AMUSE_DECL_BUTTON(Key, Home);				//!< Home
        AMUSE_DECL_BUTTON(Key, End);				//!< End
        AMUSE_DECL_BUTTON(Key, PageUp);				//!< PageUp
        AMUSE_DECL_BUTTON(Key, PageDown);			//!< PageDown
        AMUSE_DECL_BUTTON(Key, NumLock);			//!< NumLock

        AMUSE_DECL_BUTTON(Key, Shift);				//!< Shift
        AMUSE_DECL_BUTTON(Key, LeftShift);			//!< 左Shift
        AMUSE_DECL_BUTTON(Key, RightShift);		//!< 右Shift

        AMUSE_DECL_BUTTON(Key, Ctrl);				//!< Ctrl
        AMUSE_DECL_BUTTON(Key, LeftCtrl);			//!< 左Ctrl
        AMUSE_DECL_BUTTON(Key, RightCtrl);			//!< 右Ctrl

        AMUSE_DECL_BUTTON(Key, Alt);				//!< Alt
        AMUSE_DECL_BUTTON(Key, LeftAlt);			//!< 左Alt
        AMUSE_DECL_BUTTON(Key, RightAlt);			//!< 右Alt

        AMUSE_DECL_BUTTON(Key, Command);			//!< Command
        AMUSE_DECL_BUTTON(Key, LeftCommand);		//!< 左Command
        AMUSE_DECL_BUTTON(Key, RightCommand);		//!< 右Command

        AMUSE_DECL_BUTTON(Key, Apple);				//!< Appleキー
        AMUSE_DECL_BUTTON(Key, LeftApple);			//!< 左Appleキー
        AMUSE_DECL_BUTTON(Key, RightApple);		//!< 右Appleキー

        AMUSE_DECL_BUTTON(Key, Windows);			//!< Windowsキー
        AMUSE_DECL_BUTTON(Key, LeftWindows);		//!< 左Windowsキー
        AMUSE_DECL_BUTTON(Key, RightWindows);		//!< 右Windowsキー
	};

}
