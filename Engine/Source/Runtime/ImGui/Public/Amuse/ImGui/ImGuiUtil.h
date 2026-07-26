//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>
#include <Amuse/ImGui/Library/imgui.h>

namespace ImGui {
	inline namespace Amuse {

		inline ImVec4 ToImCol(const Color& col) {
			ImVec4 imcol(col.r, col.g, col.b, col.a);
			return imcol;
		}

	}
}