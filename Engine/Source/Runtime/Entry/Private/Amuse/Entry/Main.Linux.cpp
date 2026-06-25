//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/Core/HAL/Platform.h>
#include <Amuse/Entry/MainInternal.h>

#ifdef OS_LINUX

//! @brief  エントリ関数(Windows)
int main(int argc, char *argv[]) {
    AmuseInternalMain(argc,argv);
    return 0;
}

#endif