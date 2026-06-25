//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/Core/Misc/Arguments.h>
#include <Amuse/Core/String/StringEncoder.h>

#include <Amuse/Core/Core.h>

#ifdef OS_WINDOWS

#include <Windows.h>
#include <shellapi.h>

#elif defined(OS_LINUX)

extern int s_args;
extern char** s_argv;

#endif

namespace Amuse {

#ifdef OS_WINDOWS

    static void GetCommandLine(Arguments::container_type& dest) {
        dest.clear();
        int numArgs;
        LPWSTR* ppArgs = ::CommandLineToArgvW(::GetCommandLineW(), &numArgs);
        for (int i = 0; i < numArgs; ++i) {
            StringBase<wchar_t> warg = ppArgs[i];
            String arg;
            StringEncoder::Encode(warg, arg);
            dest.push_back(arg);
        }
        ::LocalFree(ppArgs);
    }

#elif defined(OS_LINUX)

    static void GetCommandLine(Arguments::container_type& dest) {
        dest.clear();
        if (s_argv == nullptr) {
            return;
        }
        for (int i = 0; i < s_args; ++i) {
            if (s_argv[i] == nullptr) {
                continue;
            }
            dest.push_back(s_argv[i]);
        }
    }

#endif

    //! @brief  コンストラクタ
    Arguments::Arguments() {
        GetCommandLine(m_args);
    }


    //! @brief  引数取得
    const String& Arguments::at(s32 index)const {
        return m_args.at(index);
    }


    //! @brief  開始イテレータ(const)
    Arguments::const_iterator Arguments::begin()const noexcept {
        return m_args.cbegin();
    }


    //! @brief  終了イテレータ(const)
    Arguments::const_iterator Arguments::end()const noexcept {
        return m_args.cend();
    }


    //! @brief  引数サイズ
    s32 Arguments::size()const noexcept {
        return static_cast<s32>(m_args.size());
    }


    //! @brief  空か
    bool Arguments::empty()const noexcept {
        return m_args.empty();
    }

}
