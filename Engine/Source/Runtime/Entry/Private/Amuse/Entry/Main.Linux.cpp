//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/Core/HAL/Platform.h>
#include <Amuse/Entry/MainInternal.h>

#ifdef OS_LINUX

int s_args = 0;
char** s_argv = nullptr;

//! @brief  エントリ関数(Linux)
int main(int argc, char *argv[]) {
    s_args = argc;
    s_argv = argv;
    AmuseInternalMain(argc,argv);
    return 0;
}

#endif
