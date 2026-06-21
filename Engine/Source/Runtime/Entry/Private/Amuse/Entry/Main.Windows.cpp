//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/Core/HAL/Platform.h>
#include <Amuse/Entry/MainInternal.h>

#ifdef OS_WINDOWS

#include <Windows.h>
#include <DbgHelp.h>

extern "C"
{
    static HANDLE dbgHelpLock ;
    void DbgHelpInit () { dbgHelpLock = CreateMutex ( nullptr , FALSE , nullptr ) ; }
    void DbgHelpLock () { WaitForSingleObject ( dbgHelpLock , INFINITE ) ; }
    void DbgHelpUnlock () { ReleaseMutex ( dbgHelpLock ) ; }
}

//! @brief  エントリ関数(Windows)
int main(int argc, char *argv[]) {

    // dbghelpの初期化
    SymInitialize(GetCurrentProcess(), NULL, TRUE);

    AmuseInternalMain(argc,argv);

    return 0;
}

#endif