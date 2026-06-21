//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/Core/Utility/Ref.h>

namespace Amuse::Core
{

	//! @brief		コンストラクタ
	//! @details	RefObjectを直接構築することは禁止されています。
	RefObject::RefObject() {
	}

	//! @brief		デストラクタ
	RefObject::~RefObject() {
		AMUSE_ASSERT(m_referenceCount==0,"参照中のRefObjectが解放されました。[count={}]", m_referenceCount.load());
	}

	//! @brief		参照カウントが0になったときの処理
	void RefObject::finalize() {
		delete this;
	}

	//! @brief		参照カウントを増やす
	void RefObject::retain() {

		++m_referenceCount;

	}

	//! @brief		参照カウントを減らす
	void RefObject::release() {

		const auto count = (--m_referenceCount);

		if (count == 0) {
			finalize();
		}

	}

}