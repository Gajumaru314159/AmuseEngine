//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Reflection/Type.h>
#include <Amuse/Core/Reflection/Any.h>
#include <Amuse/Core/Template/Container/Vector.h>
#include <Amuse/Core/Template/Container/Map.h>
#include <Amuse/Core/Template/Utility/Function.h>

namespace Amuse {

	using ConstructorInvoker = Any(*)(Span<Any> args);
	using DestructorInvoker = void(*)(void*);
	using PlacedConstructorInvoker = void(*)(void*, Span<Any> args);
	using PlacedDestructorInvoker = void(*)(void*);
	using CopyInvoker = void* (*)(const void*);
	using AssignInvoker = void(*)(const void*,void*);
	using MethodInvoker = Func<Any(Any& owner, Span<Any> args)>;
	using PropertySetter = Func<void(Any& owner, const Any& value)>;
	using PropertyGetter = Func<Any(const Any& owner)>;
	using EnumValueGetter = s32(*)(const Any& owner);


	//! @brief リフレクション要素へ付与する任意のタグ情報。
	//! @ingroup AmuseCore
	struct TagInfo {
		HashMap<StringView, String> tags; //!< タグ一覧

		//! @brief 指定名のタグを持っているか判定する。
		bool hasTag(StringView name)const { return tags.contains(name); }

		//! @brief 指定名のタグ値を取得する。
		Optional<StringView> findTag(StringView name)const { auto found = tags.find(name); return (found == tags.end()) ? std::nullopt : Optional<StringView>{ found->second }; }
	};


	//! @brief 列挙型に登録された 1 要素のメタ情報。
	//! @ingroup AmuseCore
	struct EnumElementInfo : TagInfo {
		StringView				name; //!< 列挙要素名
		s32						index; //!< 登録順に割り振られた 0 ベースのインデックス
		s64						value; //!< 値
	};

	//! @brief コンストラクタやメソッド引数のメタ情報。
	//! @ingroup AmuseCore
	struct ArgumentInfo {
		Type					type; //!< 型
		StringView				name; //!< 引数名
	};

	//! @brief 型を生成するコンストラクタのメタ情報。
	//! @ingroup AmuseCore
	struct ConstructorInfo : TagInfo {
		Vector<ArgumentInfo>		arguments; //!< コンストラクタ引数の一覧
		ConstructorInvoker			invoker; //!< コンストラクタ呼び出し関数
		PlacedConstructorInvoker	placedInvoker; //!< 配置構築呼び出し関数

		template<class T>
		//! @brief Any 引数列でコンストラクタを呼び出し、所有ポインタを取得する。
		UPtr<T> invoke(Span<Any> args) const {
			return invoker(args).release<T>();
		}
		template<class T,class... Args>
		//! @brief 可変長引数を Any に包んでコンストラクタを呼び出す。
		UPtr<T> invoke(Args&&... args) const {
			// 0引数に対応するために最後尾に空要素を追加している
			Any invokeArgs[] = {args...,Any()};
			return invoker(Span<Any>(invokeArgs,sizeof...(Args))).release<T>();
		}

		template<class T>
		//! @brief Any 引数列で指定メモリ上へ配置構築する。
		void invoke_placed(void* p,Span<Any> args) const {
			placedInvoker(p,args);
		}
		template<class T, class... Args>
		//! @brief 可変長引数を Any に包んで指定メモリ上へ配置構築する。
		void invoke_placed(void* p,Args&&... args) const {
			// 0引数に対応するために最後尾に空要素を追加している
			Any invokeArgs[] = { args...,Any() };
			placedInvoker(p, Span<Any>(invokeArgs, sizeof...(Args)));
		}

		template<class... Args>
		//! @brief 指定した引数型列とコンストラクタ引数が一致するか判定する。
		bool match()const {
			// 0引数に対応するために最後尾に空要素を追加している
			Type types[] = { Type::Get<Args>()... ,Type()};
			return std::equal(arguments.begin(), arguments.end(), std::begin(types), std::end(types)-1, [](const ArgumentInfo& a, const Type& b) {return a.type == b; });
		}
	};

	//! @brief フィールドや getter/setter で公開されるプロパティのメタ情報。
	//! @ingroup AmuseCore
	struct PropertyInfo : TagInfo {
		Type					type; //!< 型
		StringView				name; //!< プロパティ名
		PropertySetter			setter; //!< プロパティへ値を書き込む関数
		PropertyGetter			getter; //!< プロパティ値を読み出す関数
		bool					isReference; //!< getter が書き換え可能な参照を返すかどうか

		template<class T,class TOwner>
		//! @brief 所有者からプロパティ値を指定型で取得する。
		T get(TOwner&& owner) const {
			return getter(owner).template as<T>();
		}

		template<class T,class TOwner, class = std::enable_if_t<!std::is_const_v<std::remove_reference_t<TOwner>>>>
		//! @brief 所有者のプロパティへ指定値を設定する。
		void set(TOwner&& owner, T&& value) const {
			if(setter) setter(owner, value);
		}

		//! @brief 読み取り関数が登録されているか判定する。
		bool					canRead() const { return !!getter; }
		//! @brief 書き込み関数が登録されているか判定する。
		bool					canWrite() const { return !!setter; }
		//! @brief 読み書き両方に対応しているか判定する。
		bool					canReadWrite() const { return canRead() && canWrite(); }
	};

	//! @brief リフレクション経由で呼び出せるメソッドのメタ情報。
	//! @ingroup AmuseCore
	struct MethodInfo : TagInfo {
		StringView				name; //!< メソッド名
		bool					isConst; //!< const メソッドとして登録されているかどうか
		Type					returnType; //!< 戻り値の型
		Vector<ArgumentInfo>	arguments; //!< メソッド引数の一覧
		MethodInvoker			invoke; //!< Any の所有者と引数列でメソッドを呼び出す関数
	};

	//! @brief  プロパティタイプマップ
	using PropertyInfoMap = HashMap<StringView, PropertyInfo>;

	//! @brief  メソッドタイプマップ
	using MethodInfoMap = HashMap<StringView, MethodInfo>;

	//! @brief 型の生成、破棄、プロパティ、メソッド、列挙要素を保持するメタ情報。
	//! @ingroup AmuseCore
	struct TypeInfo : TagInfo {
		Type					type; //!< 型
		HashSet<Type>			bases; //!< 基底型一覧

		size_t					size; //!< サイズ
		size_t					alignment; //!< アライメント

		Vector<ConstructorInfo>	constructors; //!< コンストラクタ一覧
		DestructorInvoker		destructor; //!< デストラクタ
		PlacedDestructorInvoker placedDestructor; //!< 配置済みデストラクタ
		CopyInvoker				copyInvoker; //!< コピー呼び出し情報
		AssignInvoker			assignInvoker; //!< 代入呼び出し情報

		PropertyInfoMap			properties; //!< プロパティ一覧
		MethodInfoMap			methods; //!< メソッド一覧

		Vector<StringView>		propertyOrder; //!< プロパティ順序
		Vector<StringView>		methodOrder; //!< メソッド順序

		bool					isEnum; //!< 列挙型フラグ
		EnumValueGetter			enumValueGetter; //!< 列挙値取得関数
		Vector<EnumElementInfo>	enumElements; //!< 列挙要素一覧

		//! @brief 登録済みコピー関数で値を複製する。
		void* copy(const void* pointer)const { return copyInvoker?copyInvoker(pointer):nullptr; }
		//! @brief 登録済み代入関数で値を代入する。
		void assign(const void* from, void* to)const { if(assignInvoker) assignInvoker(from,to); }
		//! @brief 登録済みデストラクタで所有オブジェクトを破棄する。
		void destroy(void* pointer)const { AMUSE_ASSERT_EXPR(destructor); destructor(pointer); }
		//! @brief 配置構築済みオブジェクトのデストラクタを呼び出す。
		void destroyPlaced(void* pointer)const { AMUSE_ASSERT_EXPR(placedDestructor);  placedDestructor(pointer); }

		//! @brief 配列要素間のバイト幅を取得する。
		size_t stride()const { return align_up(size,alignment); }

		//! @brief この型が指定型を基底に持つか判定する。
		bool isBaseOf(const Type& super)const;

		template<class T>
		//! @brief この型がテンプレート型を基底に持つか判定する。
		bool isBaseOf()const {
			return isBaseOf(Type::Get<T>());
		}

		//! @brief この型が指定型の基底型か判定する。
		bool isSuperClassOf(const Type& base)const;

		template<class T>
		//! @brief この型がテンプレート型の基底型か判定する。
		bool isSuperClassOf()const {
			return isSuperClassOf(Type::Get<T>());
		}

		template<class... Args>
		//! @brief 一致するコンストラクタ情報を検索する。
		const ConstructorInfo* findConstructor()const {
			for (auto& constructor : constructors) {
				if (constructor.match<Args...>()) {
					return &constructor;
				}
			}
			return nullptr;
		}

		template<class T = void>
		//! @brief 名前に一致するプロパティを検索する。
		const PropertyInfo* findProperty(StringView name)const {
			auto itr = properties.find(name);
			if (itr == properties.end()) return nullptr;

			if constexpr (std::is_same_v<T, void>) {
				return &itr->second;
			} else {
				if (itr->second.type.is<T>()) {
					return &itr->second;
				}
			}
			return nullptr;
		}

		//! @brief 名前に一致するメソッドを検索する。
		const MethodInfo* findMethod(StringView name)const {
			auto itr = methods.find(name);
			if (itr == methods.end()) return nullptr;
			if (!itr->second.invoke) return nullptr;
			return &itr->second;
		}

		//! @brief 名前に一致する列挙要素を検索する。
		const EnumElementInfo* findEnumElement(StringView name)const {
			for (auto& element : enumElements) {
				if (element.name != name)continue;
				return &element;
			}
			return nullptr;
		}
		//! @brief 値に一致する列挙要素を検索する。
		const EnumElementInfo* findEnumElement(s32 value)const {
			for (auto& element : enumElements) {
				if (element.value != value)continue;
				return &element;
			}
			return nullptr;
		}

	public:

		template<class T>
		//! @brief テンプレート型に一致する TypeInfo を検索する。
		static const TypeInfo* Find() { return Find(Type::Get<T>()); }
		//! @brief Type に一致する TypeInfo を検索する。
		static const TypeInfo* Find(const Type& type);
		//! @brief 型名に一致する TypeInfo を検索する。
		static const TypeInfo* Find(StringView type);
		//! @brief 型ハッシュに一致する TypeInfo を検索する。
		static const TypeInfo* Find(Type::hash_type hash);
		//! @brief 登録済み TypeInfo を順に走査する。
		static void Visit(const std::function<void(const TypeInfo&)> &func);

	};

}
