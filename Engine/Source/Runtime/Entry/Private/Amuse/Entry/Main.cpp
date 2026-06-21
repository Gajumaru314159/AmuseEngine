//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/Core/Profile/Profile.h>
#include <Amuse/Core/Memory/Memory.h>
#include <Amuse/Entry/Main.h>

int AmuseInternalMain(int argc, char *argv[])
{
    Amuse::Core::Internal::InitMemory();

    int result = AmuseMain(argc,argv);

    Amuse::Core::Internal::FinalizeMemory();

    return result;
}