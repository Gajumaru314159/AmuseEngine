//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/CorePrivate.h>
#include <Amuse/Core/Hash/Hash.h>

namespace Amuse::Core {

	//! @cond
	namespace internal::type_id {

		//! @brief コンパイラが出力する関数シグネチャから型名抽出の基準文字列を取得する。
		constexpr StringView GetTypeName(void) {
			return FUNC_NAME;
		}

		//! @brief テンプレート型 T の完全修飾型名を取得する。
		template<class T>
		constexpr StringView GetTypeName(void) {

			static_assert(!std::is_volatile_v<T>, "Type does not support volatile.");

			using namespace std::string_view_literals;

			constexpr StringView signature = FUNC_NAME;

			// TODO __PRETTY_FUNCTION__ 対応
			// TODO GCC Clang 対応
			
#if defined(__clang__) || defined(COMPILER_GCC)
			constexpr size_t prefix = GetTypeName().size() + " [with T = "sv.size();
			constexpr size_t suffix = signature.size() - signature.find(';',prefix);
#elif defined(COMPILER_MSVC)
			constexpr size_t prefix2 = GetTypeName().size() - "(void)"sv.size() + "<"sv.size();
			constexpr size_t suffix = ">(void)"sv.size();
			constexpr size_t prefix = prefix2 +
				(
					signature.substr(prefix2).starts_with("enum ") ? "enum "sv.size() :
					signature.substr(prefix2).starts_with("class ") ? "class "sv.size() :
					signature.substr(prefix2).starts_with("struct ") ? "struct "sv.size() :
					signature.substr(prefix2).starts_with("union ") ? "union "sv.size() : 0
					);
#endif


			constexpr StringView name = signature.substr(prefix, signature.size() - prefix - suffix);
			

			return name;
		}

	}
	//! @endcond

#define AMUSE_RTTI()	virtual Type getType()const{return Type::Get<std::remove_cv_t<std::remove_reference_t<decltype(*this)>>>();}

	//! @brief 型名と固定ハッシュでリフレクション対象の型を識別する。
	class Type {
	public:
		//! @brief 型名ハッシュを表す型。
		using hash_type = u64;
	public:
		//! @brief テンプレート型 T に対応する Type を取得する。
		template<class T>
		static constexpr Type Get() {
			constexpr auto name = internal::type_id::GetTypeName<std::remove_cv_t<std::remove_reference_t<T>>>();
			return name;
		}
		//! @brief 値の静的型に対応する Type を取得する。
		template<class T>
		static constexpr Type Get(T&&) {
			auto name = internal::type_id::GetTypeName<std::remove_cv_t<std::remove_reference_t<T>>>();
			return name;
		}
		//! @brief 無効な型を表す Type を取得する。
		static constexpr Type Invalid() {
			return Type();
		}
	public:

		//! @brief 無効な型を表す Type を生成する。
		constexpr Type() : Type( Type::Get<InvalidType>().name() ){}

		//! @brief 完全修飾型名から Type を生成する。
		constexpr Type(StringView fullName)
			: m_name(fullName)
			, m_hash(0)
		{
			// FNV64
			constexpr u64 offset_basis = 14695981039346656037u;
			constexpr u64 fnv_prime = 1099511628211u;
			u64 result = offset_basis;

			for (size_t i = 0; i < fullName.size(); ++i) {
				result ^= static_cast<u64>(fullName[i]);
				result *= fnv_prime;
			}
			m_hash = result;
		}

		//! @brief 型名が空か判定する。
		constexpr bool empty()const { return m_name.empty(); }

		//! @brief 名前空間を含む型名を取得する。
		//! @details	名前空間を含みます。
		//!				未設定の場合は空文字列を返します。
		constexpr StringView name() const { return m_name; }

		//! @brief 名前空間を除いた型名を取得する。
		//! @details	名前空間を含みません。
		//!				未設定の場合は空文字列を返します。
		constexpr StringView shortName() const { 
			auto limit = m_name.find('<');
			if (limit == m_name.npos) limit = m_name.size();

			if (auto index = m_name.rfind(':',limit); index != StringView::npos) {
				return m_name.substr(index + 1);
			}
			else {
				return m_name;
			}		
		}

		//! @brief 型名から名前空間部分を取得する。
		//! @details	名前空間を含みません。
		//!				未設定の場合は空文字列を返します。
		constexpr StringView nameSpace() const {
			auto limit = m_name.find('<');
			if (limit == m_name.npos) limit = m_name.size();

			if (auto index = m_name.rfind(':',limit); index != StringView::npos) {
				return m_name.substr(0,index - 1);
			}
			else {
				return "";
			}
		}

		//! @brief 型名から計算した固定ハッシュ値を取得する。
		//! @details	プラットフォームによらず固定です。
		//! @note		内部実装は型名のハッシュ値です。
		constexpr hash_type hash() const { return m_hash; }

		//! @brief 指定したテンプレート型と同じ Type か判定する。
		//! @details	この比較ではダイナミックキャストを考慮しません。
		template<class T>
		constexpr bool is() const { return *this == Type::Get<T>(); }

		//! @cond
		constexpr operator hash_type()const { return m_hash; }
		constexpr bool operator==(Type rhs)const { return m_hash == rhs.m_hash; }
		constexpr bool operator!=(Type rhs)const { return m_hash != rhs.m_hash; }
		constexpr bool operator<(Type rhs)const { return m_hash < rhs.m_hash; }
		constexpr bool operator>(Type rhs)const { return m_hash > rhs.m_hash; }
		//! @endcond

	private:

		friend class TypeInfoManager;
		Type(Type::hash_type hash) {
			m_hash = hash;
		}

		class InvalidType {

		};

	private:
		StringView	m_name;
		hash_type	m_hash;
	};


}


//===============================================================
// ハッシュ化
//===============================================================
//! @cond
template<>
struct std::hash<Amuse::Core::Type> {
public:
	size_t operator()(const Amuse::Core::Type& value)const noexcept {
		return value.hash();
	}
};
//! @endcond
