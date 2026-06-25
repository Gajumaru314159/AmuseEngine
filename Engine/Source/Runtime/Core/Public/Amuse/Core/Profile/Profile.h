//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once

// 複数のプロファイラに対応するためのラッパー
// * Tracy
// * PIX
// * RenderDoc
#include <tracy/Tracy.hpp>

#define AMUSE_PROFILE_THREAD(name)                         tracy::SetThreadNameWithHint(name,-1)

#define AMUSE_PROFILE_SCOPE(name,color)                    ZoneNamedNC(__scope,name,color,true)

#define AMUSE_PROFILE_FRAME()                              FrameMark
#define AMUSE_PROFILE_FRAME_IMAGE(image,w,h,offset,flip)   FrameImage(image,w,h,offset,flip)

#define AMUSE_PROFILE_MESSAGE(message)                     TracyMessageL(message)
#define AMUSE_PROFILE_PLOT(name,value)                     TracyPlot(name,value)

//#define AMUSE_PROFILE_LOCK(type,var)                       TracyLockable(type,var)
#define AMUSE_PROFILE_LOCK(type,var)                       type var
#define AMUSE_PROFILE_LOCK_DESC(type,var,desc)             TracyLockableN(type,var,desc)
#define AMUSE_PROFILE_SHARED_LOCK(type,var)                TracySharedLockable(type,var)
#define AMUSE_PROFILE_SHARED_LOCK_DESC(type,var,desc)      TracySharedLockableN(type,var,desc)

#define AMUSE_PROFILE_ALLOC(category,ptr,size)             TracyAllocS(ptr,size,8)
#define AMUSE_PROFILE_FREE(category,ptr)                   TracyFreeS(ptr,8)

namespace Amuse {

    void BeginProfile();
    void EndProfile();

}