//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Misc/YesNo.h>
#include <Amuse/Core/Reflection/Any.h>
#include <Amuse/Core/Reflection/DynamicCast.h>
#include <Amuse/Core/Reflection/Type.h>
#include <Amuse/Core/Template/Utility/Memory.h>
#include <Amuse/Core/Template/Utility/TypeTraits.h>

namespace Amuse {

	class Any;
	struct TypeInfo;
	struct PropertyInfo;
	class BinaryReader;
	class BinaryWriter;


	//! @brief プロパティ
	//! @ingroup AmuseCore
	class Property {
	public:
		//! @brief プロパティ所有者への書き込み可否を表すフラグ型。
		DEFINE_YES_NO(Writable);
	public:

		//! @brief 空のプロパティを生成
		Property() = default;

		//! @brief コピー代入演算子
		Property& operator=(const Property& other) = default;

		//! @brief 値の代入演算子 
		Property& operator=(const Any& value) {
			assign(value);
			return *this;
		}

		//! @brief 値の代入演算子 
		template<class T>
		auto operator=(T&& value) -> std::enable_if_t<!std::is_same_v<remove_cvr_t<T>, Any>, Property&> {
			assign(Any(value));
			return *this;
		}

		//! @brief 値をAny型で設定
		Property& assign(const Any& value);

		//! @brief 指定した型へキャスト可能なプロパティか判定する。
		bool is(const Type& to) const {
			return IsCastable(type(), to);
		}

		//! @brief 指定したテンプレート型へキャスト可能なプロパティか判定する。
		template<class T>
		bool is() const {
			return is(Type::Get<T>());
		}

		//! @brief 参照型のプロパティか
		bool isReference() const;

		//! @brief 書き込み可能なプロパティか
		//! @details Ownerがconst、もしくはプロパティのSetterがない場合はfalseを返します。
		bool isWritable() const;

		//! @brief プロパティ値を Any として取得する。
		//! @details isReference()がfalseの場合はコピーを返します。
		Any get();

		//! @brief 読み取り専用のプロパティ値を Any として取得する。
		//! @details isReference()がfalseの場合はコピーを返します。
		//!		     取得したAnyオブジェクトは書き込み不可としてマークされます。
		Any get() const;

		//! @brief プロパティ値を所有する Any としてコピーする。
		Any copy()const;

		//! @brief プロパティ値を指定型へコピーして取得する。
		template<class T>
		auto copy() const->std::enable_if_t<!std::is_reference_v<T>, T>;

		//! @brief プロパティ値を指定型の参照として取得する。
		//! @details プロパティの型が異なる型であったり参照型でない場合はassertが発生します。
		template<class T>
		auto as() const->std::enable_if_t<std::is_reference_v<T>, const T>;

		//! @brief 取得できない場合にフォールバックを返す参照取得を行う。
		template<class T>
		auto as(const T& fallback) const->std::enable_if_t<std::is_reference_v<T>, const T>;

		//! @brief プロパティ値を指定型へコピーして取得する。
		//! @details プロパティの型が異なる型の場合はassertが発生します。
		template<class T>
		auto as() const->std::enable_if_t<!std::is_reference_v<T>&& std::is_copy_assignable_v<T>, T>;

		//! @brief 取得できない場合にフォールバックを返すコピー取得を行う。
		template<class T>
		auto as(const T& fallback) const->std::enable_if_t<!std::is_reference_v<T>&& std::is_copy_assignable_v<T>, T>;

		//! @brief プロパティの型を取得
		Type type() const;

		//------ ラッパー ------//

		//! @brief プロパティ値が持つ名前付きメンバを取得する。
		Any operator[](StringView name);
		//! @brief 読み取り専用でプロパティ値の名前付きメンバを取得する。
		Any operator[](StringView name) const;
		
		//! @brief 空のPropertyか
		bool empty() const {
			return m_owner == nullptr || m_info == nullptr;
		}

		//! @brief プロパティをクリア
		void clear() {
			m_ownerInfo = nullptr;
			m_owner = nullptr;
			m_info = nullptr;
		}

		//! @brief プロパティのオーナーを参照のAnyオブジェクトで取得
		Any owner();
		//! @brief 読み取り専用でプロパティ所有者を参照する Any を取得する。
		Any owner()const;

		//! @brief プロパティ値をバイナリライターへシリアライズする。
		void serialize(BinaryWriter& writer);
		//! @brief バイナリリーダーからプロパティ値へデシリアライズする。
		void deserialize(BinaryReader& reader);

	private:

		friend class Any;

		//! @brief 読み書き可能なプロパティを生成 
		Property(const TypeInfo& ownerInfo, void* owner, const PropertyInfo* info);

		//! @brief 読み取り専用プロパティを生成 
		Property(const TypeInfo& ownerInfo, const void* owner, const PropertyInfo* info);

	private:
		// 定義順の関係でAnyをメンバにもてない
		// 常にReference::Yesで保持するためm_referenceは不要
		const TypeInfo* m_ownerInfo;
		void* m_owner;
		const PropertyInfo* m_info; 
		Writable m_writableOwner;
	};



	//! @brief 任意の型を保持するクラス
	//! @details 型情報を保持し、型情報に基づいて値を取得、設定できます。
	//!			 内部値はconstかと、参照型かを保持します。
	//! @ingroup AmuseCore
	class Any {
	private:
		DEFINE_YES_NO(Reference);
		DEFINE_YES_NO(Writable);
	public:
		template<class T,class... Args>
		//! @brief インスタンスを生成する。
		static Any Create(Args&&... args) {
			return { GetTypeInfo<T>(),new T(std::forward<Args>(args)...),Reference::No,Writable::Yes};
		}
	public:

		//! @brief 空のAnyオブジェクト作成
		Any() = default;

		//! @brief 型情報とポインタから参照Anyオブジェクトを生成
		Any(const TypeInfo& info, void* ptr) : Any(info, ptr, Reference::Yes, Writable::Yes) { }
		//! @brief 型情報と const ポインタから読み取り専用の参照 Any を生成する。
		Any(const TypeInfo& info, const void* ptr) : Any(info,ptr, Reference::Yes, Writable::No) { }

		//! @brief デストラクタ
		~Any();

		//! @brief コピーコンストラクタ
		//! @details コピー代入されたAnyオブジェクトは常に参照Anyオブジェクトとして扱われます。
		//!			　明示的にコピーする必要がある場合は copy() を使用してください。
		//!          コピー元のAnyオブジェクトが破棄された場合はコピー先のAnyオブジェクトは無効な状態になります。
		Any(const Any& other) {
			*this = other;
		}

		//! @brief ムーブコンストラクタ
		Any(Any&& other) noexcept{
			*this = other;
		}

		//! @brief コピー代入演算子
		Any& operator=(const Any& other);

		//! @brief ムーブ代入演算子
		Any& operator=(Any&& other) noexcept;

		//! @brief 参照Anyオブジェクトを生成 
		template<class T> Any(T& value) : Any(GetTypeInfo<T>(value), &value, Reference::Yes,Writable::Yes) {}

		//! @brief 参照Anyオブジェクトを生成(const)
		template<class T> Any(const T& value) : Any(GetTypeInfo<T>(value), &value, Reference::Yes, Writable::No) {}

		//! @brief コピーAnyオブジェクトを生成 
		template<class T> Any(T&& value) : Any(GetTypeInfo<T>(value), new T(value), Reference::No, Writable::Yes) {}
		
		//! @brief 文字列からPropertyを取得
		//! @details 取得したプロパティは書き込み不可としてマークされます。
		Property operator[](StringView name) const;

		//! @brief 文字列からPropertyを取得
		Property operator[](StringView name);

		//! @brief Anyオブジェクトに値を代入する 
		//! @details 以下のタイプのAnyオブジェクトに対して代入は無視されます。
		//!			 * 空のAnyオブジェクト
		//! 		 * 書き込み不可のAnyオブジェクト
		//! 		 * 異なる型の代入
		template<class T>
		auto operator=(T&& value) -> std::enable_if_t<!std::is_same_v<remove_cvr_t<T>, Any>,Any&> {
			if (m_pointer) {
				if (m_writable) {
					if (is<T>()) {
						(*static_cast<remove_cvr_t<T>*>(m_pointer)) = value;
					}
				}
			}
			return *this;
		}

		//! @brief AnyオブジェクトにAnyオブジェクトを書き込む
		//! @details 通常の代入演算子は内部オブジェクトを変更しないため、内部オブジェクトを変更する場合はこの関数を使用してください。
		//! 　　　　　```
		//!			  int value = 0;
		//!           Any a = value;
		//!           a = Any(10);
		//!           assert(value==0);
		//!           a.assign(Any(10));
		//!           assert(value==10);
		//!           ```
		Any& assign(const Any& other);

		//! @brief 内部オブジェクトの型を取得
		Type type() const;

		//! @brief 内部オブジェクトがType型のオブジェクトにキャスト可能か 
		bool is(const Type& to) const {
			return IsCastable(type(), to);
		}

		//! @brief 内部オブジェクトがTにキャスト可能か
		template<class T>
		bool is() const {
			return is(Type::Get<T>());
		}

		template<class T>
		//! @brief 内部オブジェクトを指定型の参照として取得する。
		T& as() {
			AMUSE_ASSERT(is<T>(), "型が違います");
			return *static_cast<T*>(m_pointer);
		}

		template<class T>
		//! @brief 内部オブジェクトを指定型の const 参照として取得する。
		const T& as() const {
			AMUSE_ASSERT(is<T>(), "型が違います");
			return *static_cast<const T*>(m_pointer);
		}

		template<class T>
		//! @brief 型が一致しない場合にフォールバックを返す参照取得を行う。
		T& as(T& fallback) const {
			if (is<T>()) return *static_cast<T*>(m_pointer);
			return fallback;
		}

		template<class T>
		//! @brief 型が一致しない場合にフォールバックを返す const 参照取得を行う。
		const T& as(const T& fallback) const {
			if (is<T>()) return *static_cast<T*>(m_pointer);
			return fallback;
		}

		//! @brief 内部オブジェクトを参照として保持しているか判定する。
		bool isReference() const {
			return static_cast<bool>(m_reference);
		}
		//! @brief 内部オブジェクトへ書き込み可能か判定する。
		bool isWritable() const {
			return static_cast<bool>(m_writable);
		}

		template<class T>
		//! @brief 名前付きプロパティへ値を設定する。
		Any& set(StringView name, T&& value) {
			operator[](name) = value;
			return *this;
		}

		template< class T, class... TNames>
		//! @brief ネストした名前付きプロパティへ値を設定する。
		Any& set(StringView name, TNames&&... names, T value) {
			auto temp = operator[](name).copy();
			temp.set(names..., value);
			operator[](name) = temp;
			return *this;
		}

		//! @brief 空かどうかを判定する。
		bool empty() const {
			return m_pointer == nullptr || m_info == nullptr;
		}

		//! @brief 保持している型情報とポインタをクリアする。
		void clear() {
			m_info = nullptr;
			m_pointer = nullptr;
		}

		//! @brief 値をコピー生成する。
		Any copy()const;

		template<class T>
		//! @brief 値をコピー生成する。
		T copy()const {
			return T(as<T>());
		}

		//! @brief 内部オブジェクトをUPtrとして取得する
		//! @details Anyオブジェクトが参照型の場合はnullptrを返します。
		//! 　　　　 キャスト不可の型の場合もnullptrを返します。
		template<class T>
		UPtr<T> release() {
			if (!isReference()) {
				if (is<T>()) {
					auto pointer = m_pointer;
					clear();
					return UPtr<T>(static_cast<T*>(pointer));
				}
			}
			return nullptr;
		}

		// Vector<Any> list();
		// Map<Any, Any> map();

		//! @brief 内部オブジェクトをバイナリライターへシリアライズする。
		void serialize([[maybe_unused]] BinaryWriter& writer) {}
		//! @brief バイナリリーダーから内部オブジェクトへデシリアライズする。
		void deserialize([[maybe_unused]] BinaryReader& reader) {}

	private:

		friend class Property;

		Any(const TypeInfo& info, const void* ptr, Reference reference,Writable writable) {
			m_info = &info;
			m_pointer = const_cast<void*>(ptr);
			m_reference = reference;
			m_writable = writable;
		}

	private:

		//! @ingroup AmuseCore
		template < typename T >
		class has_get_type {
		private:
			template< typename Arg >
			static auto impl(...) -> std::false_type { return {}; }
			template < typename Arg >
			static auto impl(Arg*) -> decltype(std::declval<Arg>().getType(), std::true_type()) { return {}; }
		public:
			static constexpr bool value = decltype(impl<T>(nullptr)) ::value;
		};

		template<class T>
		static const TypeInfo& GetTypeInfo() {
			return GetTypeInfo(Type::Get<T>());
		}
		template<class T>
		static const TypeInfo& GetTypeInfo(const T& obj) {
			if constexpr (has_get_type<T>::value)
				return GetTypeInfo(obj.getType());
			else
				return GetTypeInfo<T>();
		}
		static const TypeInfo& GetTypeInfo(const Type&);
	private:
		const TypeInfo* m_info = nullptr;
		void* m_pointer = nullptr;
		Reference m_reference;
		Writable m_writable;
	};


	//! @brief プロパティ値を指定型へコピーして取得する。
	template<class T>
	auto Property::copy() const -> std::enable_if_t<!std::is_reference_v<T>, T> {
		AMUSE_ASSERT(is<T>(), "型が違います");
		return get().template as<T>();
	}

	//! @brief プロパティ値を指定型の参照として取得する。
	//! @details プロパティの型が異なる型であったり参照型でない場合はassertが発生します。
	template<class T>
	auto Property::as() const -> std::enable_if_t<std::is_reference_v<T>, const T> {
		AMUSE_ASSERT(!empty(), "空のプロパティです。");
		AMUSE_ASSERT(is<T>(), "型が違います。is<T>()でアクセス可能な型か事前に確認してください。");
		AMUSE_ASSERT(isReference(), "値型は参照型で受け取ることはできません。isReference()で参照可能か確認してください。");
		return get().template as<std::remove_reference_t<T>>();
	}

	//! @brief 取得できない場合にフォールバックを返す参照取得を行う。
	template<class T>
	auto Property::as(const T& fallback) const -> std::enable_if_t<std::is_reference_v<T>, const T> {
		if (empty() || !is<T>() || !isReference()) return fallback;
		return get().template as<std::remove_reference_t<T>>();
	}

	//! @brief プロパティ値を指定型へコピーして取得する。
	//! @details プロパティの型が異なる型の場合はassertが発生します。
	template<class T>
	auto Property::as() const -> std::enable_if_t<!std::is_reference_v<T>&& std::is_copy_assignable_v<T>, T> {
		AMUSE_ASSERT(!empty(), "空のプロパティです。");
		AMUSE_ASSERT(is<T>(), "型が違います。is<T>()でアクセス可能な型か事前に確認してください。");
		return get().template as<T>();
	}

	//! @brief 取得できない場合にフォールバックを返すコピー取得を行う。
	template<class T>
	auto Property::as(const T& fallback) const -> std::enable_if_t<!std::is_reference_v<T>&& std::is_copy_assignable_v<T>, T> {
		if (empty() || !is<T>())return fallback;
		return get().template as<T>();
	}

}
