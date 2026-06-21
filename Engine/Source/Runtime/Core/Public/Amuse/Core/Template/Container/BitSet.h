//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <bitset>

namespace Amuse::Core {

    //! @brief ビットセット
    template<size_t N>
    using BitSet = std::bitset<N>;

}