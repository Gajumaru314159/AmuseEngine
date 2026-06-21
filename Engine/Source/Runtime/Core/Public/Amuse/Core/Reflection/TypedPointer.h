//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <type_traits>

#define AMUSE_TYPED_POINTER_ENABLED 1

namespace Amuse::Core {

	//! @brief		デバッガー上で型情報を表示するための型情報付きポインタ
	//! @details	デバッグビルド時のみ内部ストレージに型情報を保持します。リリースビルド時にはストレージは削除されます。
	class alignas(8) TypedPointer {
	public:
		TypedPointer() = default;

		~TypedPointer() { m_pointer = nullptr; }

		//! @brief 型情報付きポインタのデバッグ用ストレージをコピーする。
		TypedPointer(const TypedPointer& other) {
			m_storage = other.m_storage;
			m_pointer = reinterpret_cast<Base*>(m_storage.data());
		}
		//! @brief 型情報付きポインタのデバッグ用ストレージをコピー代入する。
		TypedPointer& operator=(const TypedPointer& other) {
			m_storage = other.m_storage;
			m_pointer = reinterpret_cast<Base*>(m_storage.data());
			return *this;
		}
		//! @brief 型情報付きポインタのデバッグ用ストレージをムーブする。
		TypedPointer(TypedPointer&& other) noexcept {
			m_storage = other.m_storage;
			m_pointer = reinterpret_cast<Base*>(m_storage.data());
		}
		//! @brief 型情報付きポインタのデバッグ用ストレージをムーブ代入する。
		TypedPointer& operator=(TypedPointer&& other) noexcept {
			m_storage = other.m_storage;
			m_pointer = reinterpret_cast<Base*>(m_storage.data());
			return *this;
		}

		//! @brief 型付きポインタをデバッグ表示用ストレージへ設定する。
		template<class T>
		TypedPointer(T* pointer) {
#if AMUSE_TYPED_POINTER_ENABLED
			static_assert(sizeof(Pointer<T>) <= sizeof(m_storage));
			if constexpr (std::is_same_v<T,void>)
				m_pointer = new(m_storage.data())VoidPointer(pointer);
			else
				m_pointer = new(m_storage.data())Pointer<T>(pointer);
#else
			m_pointer = pointer;
#endif
		}

		//! @brief void ポインタとして空の TypedPointer を生成する。
		explicit TypedPointer(void* ptr) : m_pointer(nullptr) { }
		//! @brief nullptr を保持する TypedPointer を生成する。
		explicit TypedPointer(std::nullptr_t) : m_pointer(nullptr) { }

		//! @brief 型付きポインタをデバッグ表示用ストレージへ代入する。
		template<class T>
		TypedPointer& operator=(T* pointer) {
#if AMUSE_TYPED_POINTER_ENABLED
			static_assert(sizeof(Pointer<T>) <= sizeof(m_storage));
			if constexpr (std::is_same_v<T, void>)
				m_pointer = new(m_storage.data())VoidPointer(pointer);
			else
				m_pointer = new(m_storage.data())Pointer<T>(pointer);
#else
			m_pointer = pointer;
#endif
			return *this;
		}

		//! @brief 保持ポインタを nullptr に設定する。
		TypedPointer& operator=(std::nullptr_t) { m_pointer = nullptr; return *this; }

		//! @brief 保持しているポインタを void ポインタとして取得する。
		void* get() {
#if AMUSE_TYPED_POINTER_ENABLED
			return m_pointer ? m_pointer->get() : nullptr;
#else
			return m_pointer;
#endif
		}
		//! @brief 保持しているポインタを const void ポインタとして取得する。
		const void* get() const {
#if AMUSE_TYPED_POINTER_ENABLED
			return m_pointer ? m_pointer->get() : nullptr;
#else
			return m_pointer;
#endif
		}

	private:
		class Base {
		public:
			virtual ~Base() = default;
			virtual void* get() = 0;
			virtual const void* get()const = 0;
		};
		template<class T>
		class Pointer : public Base {
		public:
			Pointer(T* pointer) {
				m_pointer = pointer;
			}
			void* get() override {
				return static_cast<void *>(m_pointer);
			}
			const void* get() const override {
				return static_cast<const void *>(m_pointer);
			}
		private:
			T* m_pointer;
		};
		class VoidPointer : public Base {
		public:
			VoidPointer(void* pointer) {
				m_pointer = pointer;
			}
			void* get() override {
				return (void*)m_pointer;
			}
			const void* get() const override {
				return (const void*)m_pointer;
			}
		private:
			void* m_pointer;
		};
	private:
#if AMUSE_TYPED_POINTER_ENABLED
		Base* m_pointer = nullptr;
		std::array<char,16> m_storage = {};
#else
		void* m_pointer = nullptr;
#endif
	};

}
