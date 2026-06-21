//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/CorePrivate.h>
#include <Amuse/Core/Reflection/Type.h>
#include <Amuse/Core/Exception/Exception.h>

namespace Amuse::Core {

    //! @brief          シングルトン・パターン
    template<class T>
    class Singleton {
    public:

        //! @brief      インスタンスを取得
        static T* Get()noexcept {
            return s_instance;
        }

        //! @brief      インスタンスを取得
        static T& Instance()noexcept {
            AMUSE_ASSERT(s_instance,"{}は生成されていません。", Type::Get<T>().name());
            return *s_instance;
        }

        //! @brief      デストラクタ
        virtual ~Singleton() {
            if (s_instance == this) {
                s_instance = nullptr;
            }
        }

    protected:

        //! @brief      コンストラクタ
        Singleton() {
            if (s_instance != nullptr) throw Exception("既に生成されています。");
            s_instance = reinterpret_cast<T*>(this);
        }

    private:

        //! @cond  
        Singleton(const Singleton&) = delete;
        Singleton(Singleton&&) = delete;
        Singleton& operator = (const Singleton&) = delete;
        Singleton& operator = (Singleton&&) = delete;
        //! @endcond  

        static T* s_instance;

    };

    template<class T>
    T* Singleton<T>::s_instance = nullptr;

}