//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Template/Container/Map.h>
#include <Amuse/Core/Utility/Singleton.h>
#include <Amuse/Core/Reflection/Type.h>
#include <Amuse/Core/Reflection/TypeInfo.h>

namespace Amuse::Core {

	class TypeInfoManager : public Singleton<TypeInfoManager> {
	public:

		TypeInfoManager();

		void visit(const std::function<void(const TypeInfo&)>& func) const;
		const TypeInfo* find(Type type)const;
		const TypeInfo* find(StringView type)const;
		const TypeInfo* find(Type::hash_type type)const;
		TypeInfo& registerInfo(Type type);

	public:

		static void Visit(const std::function<void(const TypeInfo&)>& func) {
			if (auto manager = Get()) {
				manager->visit(func);
			}
		}

		static const TypeInfo* Find(Type type) {
			if (auto manager = Get()) {
				return manager->find(type);
			}
			return nullptr;
		}
		static const TypeInfo* Find(StringView type) {
			return Find(Type(type));
		}
		template<class T>
		static const TypeInfo* Find() {
			return Find(Type::Get<T>());
		}

	private:
		HashMap<Type, TypeInfo> m_infos;
	};
}