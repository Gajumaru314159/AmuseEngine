//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>
#include <Amuse/Core/Reflection/TypeInfo.h>
#include <Amuse/Core/Reflection/TypeInfoManager.h>

namespace Amuse {

	//! @brief 静的初期化で収集されるリフレクション登録関数ノード。
	//! @details リフレクション登録関数を連結リストとして管理するためのオブジェクトです。
	//! @ingroup AmuseCore
	struct ReflectionFunction {
		//! @brief 型情報登録関数の関数ポインタ型。
		using func_type = void(*)();
		//! @brief 登録関数を保持するノードを初期化する。
		ReflectionFunction(func_type func);
		func_type func; //!< 型情報登録関数
		ReflectionFunction* next = nullptr; //!< 次の登録関数ノード
	};

	//! @brief		リフレクション登録関数を追加する
	//! @details	AMUSE_REGISTER_RTTI から呼び出される関数です。
	//!				登録されたリフレクション登録関数は GetReflectionFunction で取得できます。
	void PushReflectionFunction(ReflectionFunction*);

	//! @brief		ルートのリフレクション関数を取得する
	//! @details	リフレクション登録関数は ReflectionFunction の連結リストとして登録されています。
	//!				next が有効なポインタであれば次のリフレクション登録関数が存在します。
	ReflectionFunction* GetReflectionFunction();

	//! @brief		リフレクション登録オブジェクト
	//!	@details	AMUSE_DEFINE_INFO_BASE 経由で使用してください。
	//! @ingroup AmuseCore
	template<class T>
	struct TypeRegisterTemplate {
		//! @brief 対象型の登録関数を明示的にリンクする。
		//! @details TypeRegister<T>::Link() という風に呼び出すことでリフレクション登録関数を登録します。
		static void Link();
		//! @brief 対象型の TypeInfo を登録する。
		static void Register();
		static ReflectionFunction s_register; //!< 型登録インスタンス
	};

	//! @brief		リフレクション登録関数をリンクする
	//! @ingroup AmuseCore
	struct TypeRegister {
		template<class... Ts>
		//! @brief 指定した型群の登録関数を明示的にリンクする。
		static void Link() {
			[[maybe_unused]] auto funcs = { TypeRegisterTemplate<Ts>::Link... };
		}
	};

}


//! @brief		型情報の定義
//! @details	Builderを通じて型情報を登録するRegister()と、翻訳単位を明示的にリンクするためのLink()を定義します。
#define AMUSE_DEFINE_INFO_BASE(builder_type,type)\
namespace Amuse {\
	template<> void TypeRegisterTemplate<::type>::Register() {\
		builder_type<::type> builder{};\
	}\
	template<> void TypeRegisterTemplate<::type>::Link() {}\
	template<> ReflectionFunction TypeRegisterTemplate<::type>::s_register(TypeRegisterTemplate<::type>::Register);\
}\
template<> void builder_type<::type>::Register()

//! @brief		Primitive型情報の定義
//!	@details	```cpp
//!				AMUSE_DEFINE_PRIMITIVE_INFO(int) {
//!					desc("32bit符号付き整数型");
//! 			}	
//!				```
#define AMUSE_DEFINE_PRIMITIVE_INFO(type) AMUSE_DEFINE_INFO_BASE(Amuse::PrimitiveBuilderTemplate,type)

//! @brief		Enum型情報の定義
//!	@details	```cpp
//!				AMUSE_DEFINE_ENUM_INFO(EnumType) {
//!					element(T::First);
//!					element(T::Second);
//! 			}	
//!				```
#define AMUSE_DEFINE_ENUM_INFO(type) AMUSE_DEFINE_INFO_BASE(Amuse::EnumBuilderTemplate,type)

//! @brief		Class型情報の定義
//!	@details	```cpp
//!				AMUSE_DEFINE_CLASS_INFO(Foo) {
//!					constructor();
//!					field("field",&T::field);
//!					property("prop",&T::getProp,&T::setProp);
//!					method("method",&T::method,"arg0","arg1");
//! 			}	
//!				```
#define AMUSE_DEFINE_CLASS_INFO(type) AMUSE_DEFINE_INFO_BASE(Amuse::ClassBuilderTemplate,type)


namespace Amuse {

	//! @brief TypeInfo や要素へタグ情報を追加するビルダー。
	//! @ingroup AmuseCore
	class TagBuilder {
	public:

		//! @brief タグを書き込む TagInfo を受け取って初期化する。
		TagBuilder(TagInfo&);

		//! @brief 任意キーと値のタグを追加する。
		TagBuilder& tag(StringView key, StringView value = "");

		//! @brief 説明文を Description タグとして追加する。
		TagBuilder& desc(StringView value);

	private:
		TagInfo& m_tagInfo;
	};




	//! @brief		Primitive型情報ビルダー
	//! @ingroup AmuseCore
	class PrimitiveBuilder : public TagBuilder {
	public:

		//! @brief		コンストラクタ
		PrimitiveBuilder(TypeInfo& info)
			: TagBuilder(info)
			, m_info(info)
		{
		}

	protected:
		TypeInfo& m_info; //!< 登録中のプリミティブ型情報
	};

	//! @brief 列挙型の TypeInfo と列挙要素を登録するビルダー。
	//! @ingroup AmuseCore
	class EnumBuilder : public TagBuilder {
	public:

		//! @brief		コンストラクタ
		EnumBuilder(TypeInfo&);

		//! @brief 列挙要素を名前と値で追加する。
		//! @details	追加した順番にインデックスが割り振られます。インデックスは0ベースです。
		template<typename E>
		TagBuilder element(StringView name, E value) {
			auto& element = m_info.enumElements.emplace_back();
			element.name = name;
			element.index = gsl::narrow_cast<s32>(m_info.enumElements.size()) - 1;
			element.value = enum_cast(value);
			return element;
		}

	protected:
		TypeInfo& m_info; //!< 登録中の列挙型情報
	};

	//! @brief クラス型の基底、コンストラクタ、プロパティ、メソッドを登録するビルダー。
	//! @ingroup AmuseCore
	class ClassBuilder : public TagBuilder {
	public:

		//! @brief			コンストラクタ
		ClassBuilder(TypeInfo&);

	protected:

		//! @brief 引数名未指定時に使用するデフォルト引数名を取得する。
		static StringView _GetDefaultArgumentName(size_t index);

	protected:
		TypeInfo& m_info; //!< 登録中のクラス型情報
	};

	//! @brief コンストラクタやデストラクタなどのオペレータを間接的に呼び出すためのクラス
	//! @ingroup AmuseCore
	template<class T>
	class ClassTrait {
	public:
		//! @brief デフォルトコンストラクタで所有 Any を生成する。
		static Any _New([[maybe_unused]] Span<Any> args) {
			return Any::Create<T>();
		}
		//! @brief デフォルトコンストラクタで指定メモリ上へ配置構築する。
		static void _PlacedNew(void* p, [[maybe_unused]] Span<Any> args) {
			AMUSE_ASSERT(p, "pがnullです");
			new(p)T;
		}
		//! @brief 1 引数コンストラクタで所有 Any を生成する。
		static Any _NewWith(Span<Any> args) {
			return Any::Create<T>(args[0].as<T>());
		}
		//! @brief 1 引数コンストラクタで指定メモリ上へ配置構築する。
		static void _PlacedNewWith(void* ptr, Span<Any> args) {
			AMUSE_ASSERT(ptr, "ptrがnullです"); new(ptr) T(args[0].as<T>());
		}
		//! @brief 所有オブジェクトを delete で破棄する。
		static void _Delete(void* ptr) {
			AMUSE_ASSERT(ptr, "ptrがnullです");
			delete static_cast<T*>(ptr);
		}
		//! @brief 配置構築済みオブジェクトのデストラクタを呼び出す。
		static void _PlacedDelete(void* ptr) {
			AMUSE_ASSERT(ptr, "ptrがnullです");
			static_cast<T*>(ptr)->~T();
		}
		//! @brief コピーコンストラクタで新しいオブジェクトを生成する。
		static void* _Copy(const void* ptr) {
			AMUSE_ASSERT(ptr, "ptrがnullです");
			return static_cast<void *>(new T(*static_cast<const T *>(ptr)));
		}
		//! @brief 登録型の代入演算子で値をコピーする。
		static void _Assign(const void* from, void* to) {
			AMUSE_ASSERT(from, "fromがnullです");
			AMUSE_ASSERT(to, "toがnullです");
			(*static_cast<T*>(to)) = (*static_cast<const T*>(from));
		}
	};


	//! @brief		Primitive型情報ビルダー
	//! @ingroup AmuseCore
	template<class _T>
	class PrimitiveBuilderTemplate :public PrimitiveBuilder {
	public:
		//! @brief 登録対象のプリミティブ型。
		using T = _T;
	public:

		//! @brief			コンストラクタ
		PrimitiveBuilderTemplate()
			: PrimitiveBuilder(TypeInfoManager::Instance().registerInfo(Type::Get<T>()))
		{
			m_info.size = sizeof(T);
			m_info.alignment = alignof(T);

			// コンストラクタ登録(デフォルト)
			{
				auto& ctor = m_info.constructors.emplace_back();
				ctor.invoker = ClassTrait<T>::_New;
				ctor.placedInvoker = ClassTrait<T>::_PlacedNew;
			}

			// コンストラクタ登録(初期値あり)
			{
				auto& ctor = m_info.constructors.emplace_back();
				ctor.arguments = { {Type::Get<T>(),"value"} };
				ctor.invoker = ClassTrait<T>::_NewWith;
				ctor.placedInvoker = ClassTrait<T>::_PlacedNewWith;
			}

			// デストラクタ登録
			{
				m_info.destructor = ClassTrait<T>::_Delete;
				m_info.placedDestructor = ClassTrait<T>::_PlacedDelete;
			}

			// コピー
			if constexpr (std::is_copy_assignable_v<T>) {
				m_info.copyInvoker = ClassTrait<T>::_Copy;
				m_info.assignInvoker = ClassTrait<T>::_Assign;
			}

			// タイプ登録
			Register();
		}

	private:

		//! @brief ユーザー定義のプリミティブ型登録内容を TypeInfo へ反映する。
		void Register() {}

	};


	//! @brief		Enum型情報ビルダー
	//! @ingroup AmuseCore
	template<class _T>
	class EnumBuilderTemplate :public EnumBuilder {
	public:
		//! @brief 登録対象の列挙型。
		using T = _T;
	public:

		//! @brief			コンストラクタ
		EnumBuilderTemplate() 
			: EnumBuilder(TypeInfoManager::Instance().registerInfo(Type::Get<T>()))
		{
			// 基底型登録
			m_info.bases.emplace(Type::Get<std::underlying_type_t<T>>());
			m_info.isEnum = true;
			m_info.size = sizeof(T);
			m_info.alignment = alignof(T);

			// コンストラクタ登録(デフォルト)
			{
				auto& ctor = m_info.constructors.emplace_back();
				ctor.invoker = ClassTrait<T>::_New;
				ctor.placedInvoker = ClassTrait<T>::_PlacedNew;
			}

			// コンストラクタ登録(初期値あり)
			{
				auto& ctor = m_info.constructors.emplace_back();
				ctor.arguments = { {Type::Get<T>(),"value"} };
				ctor.invoker = ClassTrait<T>::_NewWith;
				ctor.placedInvoker = ClassTrait<T>::_PlacedNewWith;
			}

			// デストラクタ登録
			{
				m_info.destructor = ClassTrait<T>::_Delete;
				m_info.placedDestructor = ClassTrait<T>::_PlacedDelete;
			}

			// コピー
			if constexpr (std::is_copy_assignable_v<T>) {
				m_info.copyInvoker = ClassTrait<T>::_Copy;
				m_info.assignInvoker = ClassTrait<T>::_Assign;
			}

			// 値取得
			m_info.enumValueGetter = _GetEnumValue;

			// タイプ登録
			Register();
		}

	private:
		//! @brief ユーザー定義の列挙型登録内容を TypeInfo へ反映する。
		void Register() {}

		static s32 _GetEnumValue(const Any& instance) {
			return enum_cast(instance.as<T>());
		}
	};

	//! @brief		Class型情報ビルダー
	//! @ingroup AmuseCore
	template<class _T>
	class ClassBuilderTemplate : public ClassBuilder {
	public:
		//! @brief 登録対象のクラス型。
		using T = _T;
	public:

		//! @brief			コンストラクタ
		ClassBuilderTemplate() : ClassBuilder(TypeInfoManager::Instance().registerInfo(Type::Get<T>())) {

			m_info.size = sizeof(T);
			m_info.alignment = alignof(T);

			// デストラクタ登録
			{
				m_info.destructor = ClassTrait<T>::_Delete;
				m_info.placedDestructor = ClassTrait<T>::_PlacedDelete;
			}

			// コピー
			if constexpr(std::is_copy_assignable_v<T>){
				m_info.copyInvoker = ClassTrait<T>::_Copy;
				m_info.assignInvoker = ClassTrait<T>::_Assign;
			}

			// タイプ登録
			Register();
		}

	private:

		//! @brief ユーザー定義のクラス型登録内容を TypeInfo へ反映する。
		void Register() {}

		//===============================================================
		//  基底型
		//===============================================================

		//! @brief			基底クラスを追加
		template<class TBase, class = std::enable_if_t<std::is_base_of_v<TBase, T>>>
		void base() {
			m_info.bases.emplace(::Amuse::Type::Get<TBase>());
		}

		//===============================================================
		//  コンストラクタ
		//===============================================================
		
		//! @brief			デフォルトコンストラクタを追加
		TagBuilder constructor() {
			static_assert(std::is_constructible_v<T>,"0引数のコンストラクタがありません");
			auto& info = m_info.constructors.emplace_back();
			info.invoker = ClassTrait<T>::_New;
			info.placedInvoker = ClassTrait<T>::_PlacedNew;
			return info;
		}

		//! @brief			引数有コンストラクタを追加
		//! @details		引数名を指定する場合は引数の数と一致させる必要があります。
		//!					引数名を指定しない場合はデフォルトの引数名が使用されます。
		template<class... Args,class... Names>
		auto constructor(Names&&... argNames)
			-> std::enable_if_t<std::is_constructible_v<T, Args...> && (sizeof...(Args)==sizeof...(Names) || sizeof...(Names) == 0), TagBuilder>
		{
			auto& info = m_info.constructors.emplace_back();

			// 0引数(引数名未指定)に対応するために最後尾に空要素を追加している
			StringView names[] = { StringView(argNames)... ,"" };
			Array<Type,sizeof...(Args)> types = { Type::Get<Args>() ... };

			// 型と名前を登録
			for (s32 i = 0; i < std::size(types); ++i) {
				auto& arg = info.arguments.emplace_back();
				arg.type = types.at(i);
				arg.name = (sizeof...(Names) == 0) ? _GetDefaultArgumentName(i) : names[i];
			}

			// invokerを登録
			info.invoker = &_New<Args...>;
			info.placedInvoker = &_PlacedNew<Args...>;

			return info;
		}

		//===============================================================
		//  メソッド
		//===============================================================
		
		//! @cond
		//! テンプレートメタプログラミングで関数ポインタの引数型を取得
		template<typename U>
		struct MethodTraits;

		//! 関数ポインタの特殊化
		//! @ingroup AmuseCore
		template<typename OwnerType, typename ReturnType, typename... Args>
		struct MethodTraits<ReturnType(OwnerType::*)(Args...)> {
			using return_type = ReturnType;
			static constexpr Array<Type, sizeof...(Args) + 1> Types() {
				return { Type::Get<Args>()... ,Type() };
			}
			static constexpr size_t Count = sizeof...(Args);
			static constexpr bool Const = false;
		};

		//! @ingroup AmuseCore
		template<typename OwnerType, typename ReturnType, typename... Args>
		struct MethodTraits<ReturnType(OwnerType::*)(Args...)const> {
			using return_type = ReturnType;
			static constexpr Array<Type, sizeof...(Args) + 1> Types() {
				return { Type::Get<Args>()... ,Type() };
			}
			static constexpr size_t Count = sizeof...(Args);
			static constexpr bool Const = true;
		};
		//! @ingroup AmuseCore
		template<typename OwnerType, typename ReturnType, typename... Args>
		struct MethodTraits<ReturnType(OwnerType::*)(Args...)noexcept> {
			using return_type = ReturnType;
			static constexpr Array<Type, sizeof...(Args) + 1> Types() {
				return { Type::Get<Args>()... ,Type() };
			}
			static constexpr size_t Count = sizeof...(Args);
			static constexpr bool Const = false;
		};

		//! @ingroup AmuseCore
		template<typename OwnerType, typename ReturnType, typename... Args>
		struct MethodTraits<ReturnType(OwnerType::*)(Args...)const noexcept> {
			using return_type = ReturnType;
			static constexpr Array<Type, sizeof...(Args) + 1> Types() {
				return { Type::Get<Args>()... ,Type() };
			}
			static constexpr size_t Count = sizeof...(Args);
			static constexpr bool Const = true;
		};
		//! @endcond

		template<class FCallback>
		static constexpr bool IsValidOnEditCallback = !std::is_same_v<FCallback, std::nullptr_t>
			&& MethodTraits<FCallback>::Count == 0
			&& std::is_same_v<typename MethodTraits<FCallback>::return_type, void>;

		//! @brief 非 const メソッドをリフレクションへ追加する。
		template< class R, class... Args, class... Names>
		auto method(StringView name, R(T::* m)(Args...), Names&&... argNames)
			-> std::enable_if_t<MethodTraits<decltype(m)>::Count == sizeof...(Names) || 0 == sizeof...(Names), TagBuilder >
		{
			return _method_impl<decltype(m),Args...>(name, m, argNames...);
		}

		//! @brief const メソッドをリフレクションへ追加する。
		template< class R, class... Args, class... Names>
		auto method(StringView name, R(T::* m)(Args...)const, Names&&... argNames)
			-> std::enable_if_t<MethodTraits<decltype(m)>::Count == sizeof...(Names) || 0 == sizeof...(Names), TagBuilder >
		{
			return _method_impl<decltype(m), Args...>(name, m, argNames...);
		}

		//! @brief noexcept メソッドをリフレクションへ追加する。
		template< class R, class... Args, class... Names>
		auto method(StringView name, R(T::* m)(Args...)noexcept, Names&&... argNames)
			-> std::enable_if_t<MethodTraits<decltype(m)>::Count == sizeof...(Names) || 0 == sizeof...(Names), TagBuilder >
		{
			return _method_impl<decltype(m), Args...>(name, m, argNames...);
		}

		//! @brief const noexcept メソッドをリフレクションへ追加する。
		template< class R, class... Args, class... Names>
		auto method(StringView name, R(T::* m)(Args...)const noexcept, Names&&... argNames)
			-> std::enable_if_t<MethodTraits<decltype(m)>::Count == sizeof...(Names) || 0 == sizeof...(Names), TagBuilder >
		{
			return _method_impl<decltype(m), Args...>(name, m, argNames...);
		}

		//! @brief メソッド情報と Any 経由の呼び出し関数を登録する。
		template< class M, class... Args, class... Names>
		auto _method_impl(StringView name, M method, Names&&... argNames)
			-> std::enable_if_t<MethodTraits<M>::Count == sizeof...(Names) || 0 == sizeof...(Names), TagBuilder >
		{
			AMUSE_ASSERT(!m_info.methods.contains(name), "{}は登録済みのメソッドです [{}]", name, m_info.type.name());

			m_info.methodOrder.emplace_back(name);
			auto& info = m_info.methods[name];
			info.name = name;
			info.returnType = Type::Get<typename MethodTraits<M>::return_type>();
			info.isConst = MethodTraits<M>::Const;

			// 0引数(引数名未指定)に対応するために最後尾に空要素を追加している
			Array<StringView,sizeof...(Args)+1> names = {StringView(argNames)... ,""};
			auto types = MethodTraits<M>::Types();

			for (s32 i = 0; i + 1 < std::size(types); ++i) {
				auto& arg = info.arguments.emplace_back();
				arg.type = types.at(i);
				arg.name = (sizeof...(Names) == 0) ? _GetDefaultArgumentName(i) : names.at(i);
			}

			if constexpr (sizeof...(Args) == 0) {
				info.invoke = [=](Any& owner, Span<Any> args) { return _InvokeWithoutArgs<M, Args...>(owner, args, method); };
			}
			else {
				info.invoke = [=](Any& owner, Span<Any> args) { return _InvokeMethod<M, Args...>(owner, args, method); };
			}

			return info;
		}

		//===============================================================
		//  フィールド
		//===============================================================

		//! @brief メンバ変数を参照プロパティとして追加する。
		template<class TField, class FCallback = std::nullptr_t>
		TagBuilder field(StringView name, TField T::* address, FCallback callback = nullptr) {
			AMUSE_ASSERT(!m_info.properties.contains(name), "{}は登録済みのプロパティです [{}]", name, m_info.type.name());
			m_info.propertyOrder.emplace_back(name);
			auto& info = m_info.properties[name];
			info.type = Type::Get<TField>();
			info.name = name;
			info.isReference = true;
			if constexpr (!std::is_same_v<FCallback, std::nullptr_t>) {
				static_assert(IsValidOnEditCallback<FCallback>, "callback must be a member function with signature void(T::*)()");
			}
			info.getter = [=](const Any& owner) {
				if (owner.isReference()) {
					return Any(const_cast<std::remove_const_t<Any &>>(owner).as<T>().*address);
				} else {
					return Any(owner.as<T>().*address);
				}
			};
			if constexpr (!std::is_const_v<std::remove_reference_t<TField>>) {
				info.setter = [=](Any& owner, const Any& value) {
					(owner.as<T>().*(address)) = value.as<TField>();
					if constexpr (!std::is_same_v<FCallback, std::nullptr_t>) {
						(owner.as<T>().*callback)();
					}
				};
			} else {
				static_assert(std::is_same_v<FCallback, std::nullptr_t>, "const field cannot have on-edit callback because it is not writable");
			}
			return info;
		}

		//===============================================================
		//  プロパティ
		//===============================================================
		
		//! @brief getter のみを持つ読み取り専用プロパティを追加する。
		template<class F>
		TagBuilder property(StringView name, F getter) {
			return property(name, getter, getter);
		}

		//! @brief getter と setter を持つプロパティを追加する。
		template<class F1, class F2, class FCallback = std::nullptr_t>
		TagBuilder property(StringView name, F1 getter, F2 setter, FCallback callback = nullptr) {
			using return_type = typename MethodTraits<F1>::return_type;
			AMUSE_ASSERT(!m_info.properties.contains(name), "{}は登録済みのプロパティです [{}]", name, m_info.type.name());
			m_info.propertyOrder.emplace_back(name);
			auto& info = m_info.properties[name];
			info.type = Type::Get<return_type>();
			info.name = name;
			info.isReference = std::is_reference_v<return_type> && !std::is_const_v<return_type>;
			if constexpr (!std::is_same_v<FCallback, std::nullptr_t>) {
				static_assert(IsValidOnEditCallback<FCallback>, "callback must be a member function with signature void(T::*)()");
			}
			info.getter = [=](const Any& owner) {
				if (owner.isWritable()) {
					return Any((const_cast<std::remove_const_t<Any&>>(owner).as<T>().*(getter))());
				} else {
					return Any((const_cast<std::remove_const_t<Any&>>(owner).as<T>().*(getter))());
				}
			};
			if constexpr (!std::is_same_v<F1, F2>) {
				info.setter = [=](Any& owner,const Any& value) {
					// 適切な実装ではないが、setterとgetterが同じシグネチャならgetterのみとして扱う
					(owner.as<T>().*(setter))(value.as<remove_cvr_t<return_type>>());
					if constexpr (!std::is_same_v<FCallback, std::nullptr_t>) {
						(owner.as<T>().*callback)();
					}
				};
			} else {
				static_assert(std::is_same_v<FCallback, std::nullptr_t>, "read-only property cannot have on-edit callback because it has no setter");
			}
			return info;
		}

		//===============================================================
		//  メソッド定義
		//===============================================================

		//! @brief 引数なしメソッドを Any の所有者から呼び出す。
		template<class M, class... Args>
		static Any _InvokeWithoutArgs(Any& owner, [[maybe_unused]] Span<Any>, M method) {
			if constexpr (std::is_same_v<typename MethodTraits<M>::return_type, void>) {
				(owner.as<T>().*(method))();
				return Any();
			} else {
				return Any((owner.as<T>().*(method))());
			}
		}

		//! @brief インデックス列で Any 引数を展開してメソッドを呼び出す。
		template<class M,class... Args, size_t... I>
		static Any _InvokeMethodImpl(Any& owner, Span<Any> args, M method, std::index_sequence<I...>) {			
			if constexpr (std::is_same_v<typename MethodTraits<M>::return_type, void>) {
				(owner.as<T>().*(method))(args[I].template as<std::remove_reference_t<Args>>()...);
				return Any();
			} else {
				return Any((owner.as<T>().*(method))(args[I].template as<std::remove_reference_t<Args>>()...));
			}
		}

		//! @brief Any 引数列を検証して引数ありメソッドを呼び出す。
		template<class M, class... Args>
		static Any _InvokeMethod(Any& owner, Span<Any> args, M method) {
			Type types[] = { Type::Get<Args>()... };
			if (!std::equal(args.begin(), args.end(), std::begin(types), std::end(types), [](const Any& a, const Type& b) {return a.is(b); })) {
				AMUSE_ABORT("関数の呼出し引数が一致しません");
				return {};
			}
			return _InvokeMethodImpl<M,Args...>(owner, args, method, std::make_index_sequence<sizeof...(Args)>());
		}

		//===============================================================
		//  コンストラクタ定義
		//===============================================================

		//! @brief インデックス列で Any 引数を展開して所有 Any を生成する。
		template<class T,class... Args,size_t ...I>
		static Any _NewImpl(Span<Any> args, std::index_sequence<I...>) {
			return Any::Create<T>(args[I].template as<std::remove_reference_t<Args>>()...);
		}

		//! @brief Any 引数列を検証して所有 Any を生成する。
		template<class... Args>
		static Any _New(Span<Any> args) {
			Type types[] = {Type::Get<Args>()...};
			if (!std::equal(args.begin(), args.end(), std::begin(types), std::end(types), [](const Any& a, const Type& b) {return a.is(b); })) {
				AMUSE_ABORT("関数の呼出し引数が一致しません");
				return {};
			}
			return _NewImpl<T, Args...>(args, std::make_index_sequence<sizeof...(Args)>());
		}

		//! @brief インデックス列で Any 引数を展開して配置構築する。
		template<class T, class... Args, size_t ...I>
		static void _PlacedNewImpl(void* ptr, Span<Any> args, std::index_sequence<I...>) {
			new(ptr)T(args[I].template as<std::remove_reference_t<Args>>()...);
		}

		//! @brief Any 引数列を検証して指定メモリ上へ配置構築する。
		template<class... Args>
		static void _PlacedNew(void* ptr, Span<Any> args) {
			Type types[] = { Type::Get<Args>()... };
			if (!std::equal(args.begin(), args.end(), std::begin(types), std::end(types), [](const Any& a, const Type& b) {return a.is(b); })) {
				AMUSE_ABORT("関数の呼出し引数が一致しません");
				return;
			}
			_PlacedNewImpl<T, Args...>(ptr,args, std::make_index_sequence<sizeof...(Args)>());
		}

	};

}
