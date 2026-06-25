//***********************************************************
//! @file
//! @author Gajumaru
//***********************************************************
#include <Amuse/Core/Misc/Locale.h>
#include <Amuse/Core/Template/Utility/Utility.h>

#ifdef OS_LINUX

namespace Amuse {

	//! @brief システム言語を取得する
	Locale Locale::System() {
		std::string name = std::locale("").name();
		while (!name.empty())
		{
			if (name.back()=='.')
			{
				name.pop_back();
				break;
			}
			name.pop_back();
		}
		for (auto& c:name)
		{
			if (c=='_') c='-';
		}
		return Locale(name);
	}

}
#endif