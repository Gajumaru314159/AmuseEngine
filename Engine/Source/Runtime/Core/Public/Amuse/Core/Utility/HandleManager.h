//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/CorePrivate.h>
#include <Amuse/Core/Template/Container/Vector.h>
#include <Amuse/Core/Template/Container/Set.h>
#include <Amuse/Core/Template/Utility/Function.h>

namespace Amuse {

	//! @brief ノード内ポインタを使って要素を連結する侵入型キュー。
	//! @ingroup AmuseCore
	template<class T, T* T::* pMember>
	class IntrusiveQueue {
	public:
		//! @brief this_type 型のエイリアス。
		using this_type = IntrusiveQueue<T, pMember>;
	public:
		IntrusiveQueue() = default;
		//! @brief IntrusiveQueue を初期化する。
		IntrusiveQueue(this_type&& rhs)noexcept {
			m_top = rhs.m_top;
			m_last = rhs.m_last;
			rhs.clear();
		}
		~IntrusiveQueue() {
			clear();
		}
		//! @brief 値を代入する。
		this_type& operator=(this_type&& rhs)noexcept {
			return *this = rhs;
		}

		IntrusiveQueue(const this_type&) = delete;
		IntrusiveQueue& operator=(const this_type&) = delete;

		//! @brief 全要素を削除する。
		void clear() {
			while (dequeue()) {
				// NOTHING TO DO
			}
		}
		//! @brief 要素をキュー末尾へ追加する。
		void enqueue(T& instance) {
			set_next(&instance, nullptr);
			if (m_top == nullptr) {
				m_top = &instance;
			}
			if (m_last != nullptr) {
				set_next(m_last, &instance);
			}
			m_last = &instance;
		}
		//! @brief キュー先頭の要素を取り出す。
		T* dequeue() {
			auto result = m_top;
			if (result) {
				auto next = get_next(result);
				if (next == nullptr) {
					m_last = nullptr;
				}
				m_top = next;
				set_next(result, nullptr);
			}
			return result;
		}
		//! @brief キュー先頭の要素を参照する。
		T* peek() {
			return m_top;
		}

		//! @brief 空かどうかを判定する。
		bool empty()const {
			return m_top == nullptr;
		}
	private:
		static T*& get_next(T* node) {
			return static_cast<T *&>(node->*pMember);
		}

		static void set_next(T* node, T* next) {
			(node)->*pMember = next;
		}
	private:
		T* m_top = nullptr;
		T* m_last = nullptr;
	};


	//! @brief		ハンドル・マネージャ
	//! @details	このクラスは、Handleを使用してインスタンスを管理するためのハンドルマネージャです。
	//! @ingroup AmuseCore
	template<class T,class Deleter = std::default_delete<T>>
	class HandleManager {
	public:

		//! @brief ハンドル値を表す。
		class Handle {
			friend class HandleManager;
		public:
			Handle() = default;
			//! @brief 等値比較を行う。
			bool operator==(const Handle& rhs)const { return m_id == rhs.m_id; }
			//! @brief 非等値比較を行う。
			bool operator!=(const Handle& rhs)const { return m_id != rhs.m_id; }
			//! @brief 小なり比較を行う。
			bool operator<(const Handle& rhs)const { return  m_id < rhs.m_id || ((rhs.m_id <= m_id) && m_owner < rhs.m_owner); }
			//! @brief 空かどうかを判定する。
			bool empty()const { return m_owner == nullptr; }
			//! @brief ハンドルを空状態に戻す。
			void clear() { m_id = 0; m_owner = nullptr; }
			//! @brief ハンドルの世代情報を含む ID を取得する。
			auto id()const { return m_id; }
			//! @brief ハンドルが指す管理対象インスタンスを取得する。
			T* value()const { return m_owner ? m_owner->at(*this) : nullptr; }
		private:
			union {
				u32 m_id;
				struct {
					u32 m_index : 24;
					u32 m_version : 8;
				};
			};
			HandleManager* m_owner = nullptr;
		};

		//! @brief 管理対象ノードを表す。
		//! @ingroup AmuseCore
		struct Node {
			Node* next = nullptr; //!< 空きリスト上の次ノード
			T* instance = nullptr; //!< 管理対象インスタンス
			u32 version = 0; //!< 世代番号
		};

	public:

		//! @brief 指定容量のハンドル管理領域を初期化する。
		HandleManager(size_t capacity) {
			m_nodes.resize(capacity);
			for (auto& node : m_nodes) {
				m_queue.enqueue(node);
			}
		}

		~HandleManager() {
			AMUSE_ASSERT(std::all_of(m_nodes.begin(), m_nodes.end(), [](const Node& node) {return node.instance == nullptr; }),"未開放のインスタンスが含まれています。");
		}

		//! @brief インスタンスを登録してハンドルを発行する。
		Handle add(T* instance) {
			auto node = m_queue.dequeue();
			AMUSE_ASSERT(node != nullptr, "Out of capacity");

			node->instance = instance;

			Handle handle;
			handle.m_owner = this;
			handle.m_index = node - m_nodes.data();
			handle.m_version = node->version;

			return handle;
		}

		//! @brief ハンドルが指すインスタンスを削除してスロットを解放する。
		void remove(const Handle& handle) {
			if (handle.empty())return;
			if (handle.m_owner != this) return;

			auto& node = m_nodes.at(handle.m_index);

			if (node.version != handle.m_version)return;

			Deleter{}(node.instance);

			node.instance = nullptr;
			++node.version;

			m_queue.enqueue(node);
		}

		//! @brief ハンドルが有効なら管理対象インスタンスを返す。
		T* at(const Handle& handle)const {
			if (handle.empty())return nullptr;
			if (handle.m_owner != this) return nullptr;

			auto& node = m_nodes.at(handle.m_index);

			if (node.version != handle.m_version)return nullptr;

			return node.instance;
		}

	private:
		Vector<Node> m_nodes;
		IntrusiveQueue<Node, &Node::next> m_queue;
	};

	template<class T, class Deleter = std::default_delete<T>>
	//! @brief 同期付きハンドルマネージャを表す。
	//! @ingroup AmuseCore
	class SyncHandleManager {
	public:
		//! @brief manager_type 型のエイリアス。
		using manager_type = HandleManager<T, Deleter>;
		//! @brief handle_type 型のエイリアス。
		using handle_type = typename manager_type::Handle;
	public:
		//! @brief ハンドル用ハッシュ関数を表す。
		//! @ingroup AmuseCore
		struct Hasher {
			//! @brief 関数オブジェクトとして呼び出す。
			size_t operator()(const handle_type& handle)const {
				return handle.id();
			}
		};
	private:
		SpinLock m_lock;
		HandleManager<T> m_manager;
		Vector<handle_type> m_disposeQueue;
		HashSet<handle_type, Hasher> m_actives;
		HashSet<u32> m_disposedSet;
	public:

		//! @brief 指定容量の同期付きハンドル管理領域を初期化する。
		SyncHandleManager(size_t capacity) 
			: m_manager(capacity)
		{

		}

		~SyncHandleManager() {
			for (auto& handle : m_actives) {
				remove(handle);
			}
			update();
		}

		//! @brief インスタンスを同期的に登録してハンドルを発行する。
		handle_type add(T* instance) {
			ScopeLock lock(m_lock);
			auto handle = m_manager.add(instance);
			m_actives.emplace(handle);
			return handle;
		}

		//! @brief ハンドルが指すインスタンスを遅延削除キューへ追加する。
		void remove(const handle_type& handle) {
			if (handle.empty())return;
			ScopeLock lock(m_lock);
			if (m_disposedSet.contains(handle.id()))return;
			m_disposedSet.emplace(handle.id());
			m_disposeQueue.push_back(handle);
		}
		//! @brief ハンドルが有効なら管理対象インスタンスを返す。
		T* at(const handle_type& handle)const {
			return m_manager.at(handle);
		}

		//! @brief 登録済み要素を走査する。
		void visit(Func<void(T&)>visitor) {
			ScopeLock lock(m_lock);
			for (auto& handle : m_actives) {
				if (auto instance = handle.value()) {
					visitor(*instance);
				}
			}
		}

		//! @brief 遅延削除キューに積まれたハンドルを実際に解放する。
		void update() {
			ScopeLock lock(m_lock);
			for (auto& handle : m_disposeQueue) {
				m_actives.erase(handle);
				m_manager.remove(handle);
			}
			m_disposeQueue.clear();
			m_disposedSet.clear();
		}
	};
}
