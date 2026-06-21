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

	//! @brief 登録済み TypeInfo を Type で管理するシングルトン。
	class TypeInfoManager : public Singleton<TypeInfoManager> {
	public:

		TypeInfoManager();

		//! @brief 登録済み TypeInfo を順に走査する。
		void visit(const std::function<void(const TypeInfo&)>& func) const;
		//! @brief Type に一致する TypeInfo を検索する。
		const TypeInfo* find(Type type)const;
		//! @brief 型名に一致する TypeInfo を検索する。
		const TypeInfo* find(StringView type)const;
		//! @brief 型ハッシュに一致する TypeInfo を検索する。
		const TypeInfo* find(Type::hash_type type)const;
		//! @brief 型情報を登録する。
		TypeInfo& registerInfo(Type type);

	public:

		//! @brief 登録済み TypeInfo を順に走査する。
		static void Visit(const std::function<void(const TypeInfo&)>& func) {
			if (auto manager = Get()) {
				manager->visit(func);
			}
		}

		//! @brief Type に一致する TypeInfo を検索する。
		static const TypeInfo* Find(Type type) {
			if (auto manager = Get()) {
				return manager->find(type);
			}
			return nullptr;
		}
		//! @brief 型名に一致する TypeInfo を検索する。
		static const TypeInfo* Find(StringView type) {
			return Find(Type(type));
		}
		template<class T>
		//! @brief テンプレート型 T に一致する TypeInfo を検索する。
		static const TypeInfo* Find() {
			return Find(Type::Get<T>());
		}

	private:
		HashMap<Type, TypeInfo> m_infos;
	};
}
