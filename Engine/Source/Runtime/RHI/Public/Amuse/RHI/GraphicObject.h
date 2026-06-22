//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>
#include <Amuse/Core/Utility/Ref.h>

namespace Amuse::RHI {
    using namespace Amuse::Core;

	//! @brief      グラフィック・オブジェクト
	//! 
	//! @details    グラフィック・オブジェクトは描画時まで削除されない必要があります。
	//!             描画はフレーム内に収まらず数フレーム遅れる場合もあるため、
	//!             所有者の寿命で管理するのが難しくなします。
	//!             そこで、このクラスを継承するクラスは所有者のデストラクタで削除されずに
	//!             システムの遅延開放スタックに積まれます。
	//!             システムのスタックに積まれている間は破棄されていないことが保証できるため
	//!             GPUからのアクセスにも対応できます。
	class GraphicObject : public RefObject {
	public:

		//! @brief		デストラクタ
		~GraphicObject() override;

		//! @brief      名前を取得
		virtual const String& getName()const = 0;

	protected:

		//! @brief		コンストラクタ
		GraphicObject();

		//! @brief グラフィックオブジェクトを管理対象にする
		void manage();

	private:

		void finalize()override;

	private:

		bool m_managed = false;

	};

}
