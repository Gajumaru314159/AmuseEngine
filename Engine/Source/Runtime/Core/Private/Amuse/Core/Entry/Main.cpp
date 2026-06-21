//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/Core/Profile/Profile.h>
#include <Amuse/Core/Entry/Main.h>
#include <Amuse/Core/Memory/Memory.h>

int OctbitInternalMain(int argc, char *argv[])
{
    Amuse::Core::Internal::InitMemory();

    int result = OctbitMain(argc,argv);

    Amuse::Core::Internal::FinalizeMemory();

    return result;
}