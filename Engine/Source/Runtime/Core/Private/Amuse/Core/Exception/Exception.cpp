//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/Core/Exception/Exception.h>

namespace Amuse::Core {

    //! @brief	コンストラクタ
    Exception::Exception()
        :Exception("") {

    }


    //! @brief コンストラクタ(メッセージ指定)
    Exception::Exception(StringView message)
        :m_level(ExceptionLevel::Error)
        , m_message(message) {

    }


    //! @brief デストラクタ
    Exception::~Exception() = default;

}