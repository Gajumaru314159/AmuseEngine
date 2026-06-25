//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/Core/Profile/Profile.h>
#include <Amuse/Core/Memory/Memory.h>
#include <Amuse/Entry/Main.h>

int AmuseInternalMain(int argc, char *argv[])
{
    Amuse::Internal::InitMemory();

    int result = AmuseMain(argc,argv);

    Amuse::Internal::FinalizeMemory();

    return result;
}