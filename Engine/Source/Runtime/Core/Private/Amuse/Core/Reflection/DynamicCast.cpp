#include <Amuse/Core/Reflection/DynamicCast.h>
#include <Amuse/Core/Reflection/TypeInfoManager.h>

namespace Amuse::Core {

	bool IsCastable(Type from, Type to) {
        if (from == to)return true;
        if (auto info = TypeInfoManager::Find(from)) {
            return info->isSuperClassOf(to);
        }
		return false;
	}


}