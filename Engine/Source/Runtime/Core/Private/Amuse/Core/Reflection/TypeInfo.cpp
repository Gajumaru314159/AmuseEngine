//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/Core/Reflection/TypeInfo.h>
#include <Amuse/Core/Reflection/TypeInfoManager.h>
#include <Amuse/Core/Core.h>

namespace Amuse {

	bool TypeInfo::isBaseOf(const Type& super)const {

		if (auto info = TypeInfoManager::Find(super)) {
			return info->isSuperClassOf(type);
		}

		return false;
	}

	bool TypeInfo::isSuperClassOf(const Type& base)const {

		if(bases.contains(base))return true;

		for (auto& base2 : bases) {
			if (auto info = TypeInfoManager::Find(base2)) {
				if (info->isSuperClassOf(base))return true;
			}
		}

		return false;
	}

	const TypeInfo* TypeInfo::Find(const Type& type) {
		if (auto manager = TypeInfoManager::Get()) {
			return manager->find(type);
		}
		return nullptr;
	}
	const TypeInfo* TypeInfo::Find(StringView type) {
		if (auto manager = TypeInfoManager::Get()) {
			return manager->find(type);
		}
		return nullptr;
	}
	const TypeInfo* TypeInfo::Find(Type::hash_type hash) {
		if (auto manager = TypeInfoManager::Get()) {
			return manager->find(hash);
		}
		return nullptr;
	}
	void TypeInfo::Visit(const std::function<void(const TypeInfo&)>& func) {
		if (auto manager = TypeInfoManager::Get()) {
			manager->visit(func);
		}
	}
}