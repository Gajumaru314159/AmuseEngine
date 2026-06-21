//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <cstddef>
#include <iterator>
#include <initializer_list>
#include <type_traits>
#include <stdexcept>
#include <Amuse/Core/Log/Assertion.h>
#include <Amuse/Core/Template/Allocator/STLAllocator.h>
#include <Amuse/Core/Template/Container/Vector.h>
#include <Amuse/Core/Template/Container/Map.h>
#include <Amuse/Core/Template/Utility/Pair.h>
#include <Amuse/Core/Template/Utility/Optional.h>

namespace Amuse::Core {

	//! @brief 安定した反復順序を持つ順序付きハッシュマップ
	//! @details VectorストレージとHashMapインデックスにより挿入順序を維持しながら、平均O(1)の検索/更新を実現します。
	//!          内部のキーとインデックスのマッピングが壊れないようにするため、キーは挿入後も不変である必要があります。
	template<
		class TKey,
		class TValue,
		class THasher = std::hash<TKey>,
		class TKeyEqual = std::equal_to<TKey>,
		class TAllocator = Amuse::Core::STLAllocator<Pair<TKey, TValue>>
	>
	class IndexMap {
	private:
		using key_allocator_type = typename std::allocator_traits<TAllocator>::template rebind_alloc<TKey>;
		using mapped_allocator_type = typename std::allocator_traits<TAllocator>::template rebind_alloc<TValue>;
		using key_storage_type = Vector<TKey, key_allocator_type>;
		using mapped_storage_type = Vector<TValue, mapped_allocator_type>;
		using index_value_type = Pair<const TKey, std::size_t>;
		using index_allocator_type = typename std::allocator_traits<TAllocator>::template rebind_alloc<index_value_type>;
		using index_map_type = HashMap<TKey, std::size_t, THasher, TKeyEqual, index_allocator_type>;

	public:
		//! @brief キーの型。
		using key_type = TKey;
		//! @brief キーに対応する値の型。
		using mapped_type = TValue;
		//! @brief 格納する値の型。
		using value_type = Pair<TKey, TValue>;
		//! @brief 要素数を表す型。
		using size_type = std::size_t;
		//! @brief イテレータ差分を表す型。
		using difference_type = std::ptrdiff_t;
		//! @brief hasher 型のエイリアス。
		using hasher = THasher;
		//! @brief key_equal 型のエイリアス。
		using key_equal = TKeyEqual;
		//! @brief 要素確保に使用するアロケータ型。
		using allocator_type = TAllocator;

	private:
		struct value_reference {
			key_type& first;
			mapped_type& second;

			value_reference(key_type& key, mapped_type& value) : first(key), second(value) {}
			value_reference(const value_reference&) = default;
			value_reference& operator=(const value_reference& other) {
				first = other.first;
				second = other.second;
				return *this;
			}
			value_reference& operator=(const value_type& other) {
				first = other.first;
				second = other.second;
				return *this;
			}
			operator value_type() const {
				return value_type(first, second);
			}
		};

		struct const_value_reference {
			const key_type& first;
			const mapped_type& second;

			const_value_reference(const key_type& key, const mapped_type& value) : first(key), second(value) {}
			const_value_reference(const value_reference& ref) : first(ref.first), second(ref.second) {}
			operator value_type() const {
				return value_type(first, second);
			}
		};

		template<bool IsConst>
		struct pointer_proxy {
			using ref_type = std::conditional_t<IsConst, const_value_reference, value_reference>;
			using pointer_type = std::conditional_t<IsConst, const value_type*, value_type*>;
			value_type value;
			explicit pointer_proxy(ref_type ref) : value(ref) {}
			pointer_type operator->() { return &value; }
			pointer_type operator->() const { return &value; }
		};

		template<bool IsConst>
		class iterator_base {
			using owner_type = std::conditional_t<IsConst, const IndexMap*, IndexMap*>;
			owner_type m_owner = nullptr;
			size_type m_index = 0;
			template<bool>
			friend class iterator_base;
			friend class IndexMap;
		public:
			using iterator_category = std::random_access_iterator_tag;
			using value_type = IndexMap::value_type;
			using difference_type = IndexMap::difference_type;
			using reference = std::conditional_t<IsConst, const_value_reference, value_reference>;
			using pointer = pointer_proxy<IsConst>;

			iterator_base() = default;
			iterator_base(owner_type owner, size_type index) : m_owner(owner), m_index(index) {}
			template<bool B = IsConst, std::enable_if_t<B, int> = 0>
			iterator_base(const iterator_base<false>& other) : m_owner(other.m_owner), m_index(other.m_index) {}

			reference operator*() const {
				return reference(m_owner->m_keys[m_index], m_owner->m_values[m_index]);
			}

            pointer operator->() const {
                return pointer_proxy<IsConst>(**this);
            }

			reference operator[](difference_type n) const {
				return *(*this + n);
			}

			iterator_base& operator++() {
				++m_index;
				return *this;
			}

			iterator_base operator++(int) {
				iterator_base tmp = *this;
				++(*this);
				return tmp;
			}

			iterator_base& operator--() {
				--m_index;
				return *this;
			}

			iterator_base operator--(int) {
				iterator_base tmp = *this;
				--(*this);
				return tmp;
			}

			iterator_base& operator+=(difference_type n) {
				m_index = static_cast<size_type>(static_cast<difference_type>(m_index) + n);
				return *this;
			}

			iterator_base& operator-=(difference_type n) {
				m_index = static_cast<size_type>(static_cast<difference_type>(m_index) - n);
				return *this;
			}

			iterator_base operator+(difference_type n) const {
				iterator_base tmp = *this;
				return tmp += n;
			}

			iterator_base operator-(difference_type n) const {
				iterator_base tmp = *this;
				return tmp -= n;
			}

			difference_type operator-(const iterator_base& other) const {
				return static_cast<difference_type>(m_index) - static_cast<difference_type>(other.m_index);
			}

			bool operator==(const iterator_base& other) const {
				return m_owner == other.m_owner && m_index == other.m_index;
			}

			bool operator!=(const iterator_base& other) const {
				return !(*this == other);
			}

			bool operator<(const iterator_base& other) const {
				return m_index < other.m_index;
			}

			bool operator>(const iterator_base& other) const {
				return other < *this;
			}

			bool operator<=(const iterator_base& other) const {
				return !(other < *this);
			}

			bool operator>=(const iterator_base& other) const {
				return !(*this < other);
			}
		};

	public:
		//! @brief 要素参照型。
		using reference = value_reference;
		//! @brief 読み取り専用の要素参照型。
		using const_reference = const_value_reference;
		//! @brief 要素ポインタ型。
		using pointer = value_type*;
		//! @brief 読み取り専用の要素ポインタ型。
		using const_pointer = const value_type*;
		//! @brief 要素を走査するイテレータ型。
		using iterator = iterator_base<false>;
		//! @brief 読み取り専用で要素を走査するイテレータ型。
		using const_iterator = iterator_base<true>;
		//! @brief 逆順に要素を走査するイテレータ型。
		using reverse_iterator = std::reverse_iterator<iterator>;
		//! @brief 読み取り専用で逆順に要素を走査するイテレータ型。
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;

	public:
		static constexpr size_type npos = static_cast<size_type>(-1); //!< 無効インデックス

	public:
		IndexMap() = default;

		//! @brief 指定容量を予約した空のマップを構築する。
		explicit IndexMap(size_type capacity) {
			reserve(capacity);
		}

		//! @brief 初期化リストのキーと値でマップを構築する。
		IndexMap(std::initializer_list<value_type> init) {
			reserve(init.size());
			insert(init);
		}

		//! @brief 他のマップからコピー構築する。
		IndexMap(const IndexMap&) = default;
		//! @brief 他のマップからムーブ構築する。
		IndexMap(IndexMap&&) noexcept = default;
		//! @brief 他のマップをコピー代入する。
		IndexMap& operator=(const IndexMap&) = default;
		//! @brief 他のマップをムーブ代入する。
		IndexMap& operator=(IndexMap&&) noexcept = default;

		//! @brief 初期化リストのキーと値を代入する。
		IndexMap& operator=(std::initializer_list<value_type> init) {
			clear();
			reserve(init.size());
			insert(init);
			return *this;
		}

	public:
		//! @brief 現在格納している要素数を返す。
		[[nodiscard]] size_type size() const noexcept {
			return m_values.size();
		}

		//! @brief 空かどうかを判定する。
		[[nodiscard]] bool empty() const noexcept {
			return m_values.empty();
		}

		//! @brief 容量を予約する。
		void reserve(size_type newCapacity) {
			m_keys.reserve(newCapacity);
			m_values.reserve(newCapacity);
			m_index.reserve(newCapacity);
		}

		//! @brief 値ストレージの確保済み容量を返す。
		[[nodiscard]] size_type capacity() const noexcept {
			return m_values.capacity();
		}

		//! @brief 値ストレージの先頭ポインタを返す。
		mapped_type* data() noexcept {
			return m_values.data();
		}

		//! @brief 値ストレージの読み取り専用先頭ポインタを返す。
		const mapped_type* data() const noexcept {
			return m_values.data();
		}

		//! @brief 余剰容量を解放し、キーからインデックスへの対応を再構築する。
		void shrink_to_fit() {
			m_keys.shrink_to_fit();
			m_values.shrink_to_fit();

			index_map_type rebuilt;
			rebuilt.reserve(m_values.size());
			for (size_type i = 0; i < m_values.size(); ++i) {
				rebuilt.emplace(m_keys[i], i);
			}
			m_index.swap(rebuilt);
		}

		//! @brief 全要素を取り除き、キーと値とインデックスを空にする。
		void clear() noexcept {
			m_keys.clear();
			m_values.clear();
			m_index.clear();
		}

		//! @brief 先頭要素を指すイテレータを返す。
		iterator begin() noexcept { return iterator(this, 0); }
		//! @brief 先頭要素を指す読み取り専用イテレータを返す。
		const_iterator begin() const noexcept { return const_iterator(this, 0); }
		//! @brief 先頭要素を指す読み取り専用イテレータを返す。
		const_iterator cbegin() const noexcept { return const_iterator(this, 0); }

		//! @brief 末尾要素の次を指すイテレータを返す。
		iterator end() noexcept { return iterator(this, size()); }
		//! @brief 末尾要素の次を指す読み取り専用イテレータを返す。
		const_iterator end() const noexcept { return const_iterator(this, size()); }
		//! @brief 末尾要素の次を指す読み取り専用イテレータを返す。
		const_iterator cend() const noexcept { return const_iterator(this, size()); }

		//! @brief 逆順の先頭要素を指すイテレータを返す。
		reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
		//! @brief 逆順の先頭要素を指す読み取り専用イテレータを返す。
		const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
		//! @brief 逆順の先頭要素を指す読み取り専用イテレータを返す。
		const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(cend()); }

		//! @brief 逆順の末尾要素の次を指すイテレータを返す。
		reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
		//! @brief 逆順の末尾要素の次を指す読み取り専用イテレータを返す。
		const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }
		//! @brief 逆順の末尾要素の次を指す読み取り専用イテレータを返す。
		const_reverse_iterator crend() const noexcept { return const_reverse_iterator(cbegin()); }

	public:
		//! @brief キーに一致する要素を検索し、見つからない場合は end() を返す。
		iterator find(const key_type& key) noexcept {
			auto it = m_index.find(key);
			return it == m_index.end() ? end() : iterator(this, it->second);
		}

		//! @brief キーに一致する要素を検索し、見つからない場合は cend() を返す。
		const_iterator find(const key_type& key) const noexcept {
			auto it = m_index.find(key);
			return it == m_index.end() ? cend() : const_iterator(this, it->second);
		}

		//! @brief 要素が存在するか判定する。
		[[nodiscard]] bool contains(const key_type& key) const noexcept {
			return m_index.find(key) != m_index.end();
		}

		//! @brief キーに対応する格納順インデックスを返し、見つからない場合は npos を返す。
		[[nodiscard]] size_type index_of(const key_type& key) const noexcept {
			auto it = m_index.find(key);
			return it == m_index.end() ? npos : it->second;
		}

		//! @brief 格納順インデックスにあるキーと値の参照を返す。
		reference at_index(size_type index) {
			AMUSE_ASSERT_EXPR(index < size());
			return reference(m_keys[index], m_values[index]);
		}

		//! @brief 格納順インデックスにあるキーと値の読み取り専用参照を返す。
		const_reference at_index(size_type index) const {
			AMUSE_ASSERT_EXPR(index < size());
			return const_reference(m_keys[index], m_values[index]);
		}

		//! @brief 格納順インデックスにある値のポインタを返し、範囲外の場合は nullptr を返す。
		mapped_type* try_at_index(size_type index) noexcept {
			return index < size() ? &m_values[index] : nullptr;
		}

		//! @brief 格納順インデックスにある値の読み取り専用ポインタを返し、範囲外の場合は nullptr を返す。
		const mapped_type* try_at_index(size_type index) const noexcept {
			return index < size() ? &m_values[index] : nullptr;
		}

		//! @brief 指定位置の要素へアクセスする。
		mapped_type& at(const key_type& key) {
			auto it = m_index.find(key);
			if (it == m_index.end()) {
				throw std::out_of_range("IndexMap::at: key not found");
			}
			return m_values[it->second];
		}

		//! @brief 指定位置の要素へアクセスする。
		const mapped_type& at(const key_type& key) const {
			auto it = m_index.find(key);
			if (it == m_index.end()) {
				throw std::out_of_range("IndexMap::at: key not found");
			}
			return m_values[it->second];
		}

		//! @brief キーに対応する値のポインタを返し、見つからない場合は nullptr を返す。
		mapped_type* try_get(const key_type& key) noexcept {
			auto it = m_index.find(key);
			return it == m_index.end() ? nullptr : &m_values[it->second];
		}

		//! @brief キーに対応する値の読み取り専用ポインタを返し、見つからない場合は nullptr を返す。
		const mapped_type* try_get(const key_type& key) const noexcept {
			auto it = m_index.find(key);
			return it == m_index.end() ? nullptr : &m_values[it->second];
		}

		//! @brief 指定した要素へアクセスする。
		mapped_type& operator[](const key_type& key) {
			return try_emplace_impl(key).first.operator*().second;
		}

		template<class K>
		//! @brief 指定した要素へアクセスする。
		mapped_type& operator[](K&& key) {
			return try_emplace_impl(std::forward<K>(key)).first.operator*().second;
		}

	public:
		//! @brief キーが未登録なら要素を挿入し、挿入結果を返す。
		Pair<iterator, bool> insert(const value_type& value) {
			auto found = m_index.find(value.first);
			if (found != m_index.end()) {
				return { iterator(this, found->second), false };
			}

			m_keys.push_back(value.first);
			try {
				m_values.push_back(value.second);
			}
			catch (...) {
				m_keys.pop_back();
				throw;
			}

			auto index = size() - 1;
			m_index.emplace(m_keys[index], index);
			return { iterator(this, index), true };
		}

		//! @brief キーが未登録なら要素をムーブ挿入し、挿入結果を返す。
		Pair<iterator, bool> insert(value_type&& value) {
			auto found = m_index.find(value.first);
			if (found != m_index.end()) {
				return { iterator(this, found->second), false };
			}

			m_keys.push_back(std::move(value.first));
			try {
				m_values.push_back(std::move(value.second));
			}
			catch (...) {
				m_keys.pop_back();
				throw;
			}

			auto index = size() - 1;
			m_index.emplace(m_keys[index], index);
			return { iterator(this, index), true };
		}

		//! @brief ヒント付きでキーが未登録なら要素を挿入し、要素のイテレータを返す。
		iterator insert([[maybe_unused]] const_iterator hint, const value_type& value) {
			return insert(value).first;
		}

		//! @brief ヒント付きでキーが未登録なら要素をムーブ挿入し、要素のイテレータを返す。
		iterator insert([[maybe_unused]] const_iterator hint, value_type&& value) {
			return insert(std::move(value)).first;
		}

		template<class InputIt>
		//! @brief イテレータ範囲 [first, last) の未登録キーを挿入する。
		void insert(InputIt first, InputIt last) {
			for (; first != last; ++first) {
				insert(*first);
			}
		}

		//! @brief 初期化リスト内の未登録キーを挿入する。
		void insert(std::initializer_list<value_type> init) {
			insert(init.begin(), init.end());
		}

		template<class... Args>
		//! @brief 要素を直接構築し、キーが未登録なら挿入する。
		Pair<iterator, bool> emplace(Args&&... args) {
			value_type value(std::forward<Args>(args)...);
			return insert(std::move(value));
		}

		template<class... Args>
		//! @brief キーが未登録の場合だけ値を直接構築して挿入する。
		Pair<iterator, bool> try_emplace(const key_type& key, Args&&... args) {
			return try_emplace_impl(key, std::forward<Args>(args)...);
		}

		template<class K, class... Args>
		//! @brief 転送されたキーが未登録の場合だけ値を直接構築して挿入する。
		Pair<iterator, bool> try_emplace(K&& key, Args&&... args) {
			return try_emplace_impl(std::forward<K>(key), std::forward<Args>(args)...);
		}

		template<class M>
		//! @brief キーが未登録なら挿入し、登録済みなら値を代入する。
		Pair<iterator, bool> insert_or_assign(const key_type& key, M&& obj) {
			auto it = m_index.find(key);
			if (it != m_index.end()) {
				m_values[it->second] = std::forward<M>(obj);
				return { iterator(this, it->second), false };
			}

			m_keys.push_back(key);
			try {
				m_values.emplace_back(std::forward<M>(obj));
			}
			catch (...) {
				m_keys.pop_back();
				throw;
			}

			auto index = size() - 1;
			m_index.emplace(m_keys[index], index);
			return { iterator(this, index), true };
		}

		template<class K, class M>
		//! @brief 転送されたキーが未登録なら挿入し、登録済みなら値を代入する。
		Pair<iterator, bool> insert_or_assign(K&& key, M&& obj) {
			auto it = m_index.find(key);
			if (it != m_index.end()) {
				m_values[it->second] = std::forward<M>(obj);
				return { iterator(this, it->second), false };
			}

			m_keys.emplace_back(std::forward<K>(key));
			try {
				m_values.emplace_back(std::forward<M>(obj));
			}
			catch (...) {
				m_keys.pop_back();
				throw;
			}

			auto index = size() - 1;
			m_index.emplace(m_keys[index], index);
			return { iterator(this, index), true };
		}

		//! @brief 先頭要素へアクセスする。
		reference front() {
			AMUSE_ASSERT_EXPR(!empty());
			return reference(m_keys.front(), m_values.front());
		}

		//! @brief 先頭要素へアクセスする。
		const_reference front() const {
			AMUSE_ASSERT_EXPR(!empty());
			return const_reference(m_keys.front(), m_values.front());
		}

		//! @brief 末尾要素へアクセスする。
		reference back() {
			AMUSE_ASSERT_EXPR(!empty());
			return reference(m_keys.back(), m_values.back());
		}

		//! @brief 末尾要素へアクセスする。
		const_reference back() const {
			AMUSE_ASSERT_EXPR(!empty());
			return const_reference(m_keys.back(), m_values.back());
		}

	public:
		//! @brief 末尾要素を取り除いて返し、空の場合は空の Optional を返す。
		Optional<value_type> pop_back() {
			if (empty()) {
				return Optional<value_type>();
			}
			return Optional<value_type>(remove_swap_last(size() - 1));
		}

		//! @brief キーに一致する要素を取り除き、削除できたかを返す。
		bool erase(const key_type& key) {
			return static_cast<bool>(erase_entry(key));
		}

		//! @brief キーに一致する要素を取り除いて返し、見つからない場合は空の Optional を返す。
		Optional<value_type> erase_entry(const key_type& key) {
			auto it = m_index.find(key);
			if (it == m_index.end()) {
				return Optional<value_type>();
			}
			return Optional<value_type>(remove_preserve_order(it->second));
		}

		//! @brief 格納順インデックスの要素を順序を保って取り除き、範囲外の場合は空の Optional を返す。
		Optional<value_type> erase_index(size_type index) {
			if (index >= size()) {
				return Optional<value_type>();
			}
			return Optional<value_type>(remove_preserve_order(index));
		}

		//! @brief 格納順インデックスの要素を末尾要素と入れ替えて取り除き、範囲外の場合は空の Optional を返す。
		Optional<value_type> swap_erase_index(size_type index) {
			if (index >= size()) {
				return Optional<value_type>();
			}
			return Optional<value_type>(remove_swap_last(index));
		}

		//! @brief キーに一致する要素を末尾要素と入れ替えて取り除き、見つからない場合は空の Optional を返す。
		Optional<value_type> swap_erase(const key_type& key) {
			auto it = m_index.find(key);
			if (it == m_index.end()) {
				return Optional<value_type>();
			}
			return Optional<value_type>(remove_swap_last(it->second));
		}

		//! @brief 指定位置の要素を取り除き、次の要素を指すイテレータを返す。
		iterator erase(const_iterator pos) {
			auto index = pos.m_index;
			erase_index(index);
			return iterator(this, index);
		}

		//! @brief 範囲 [first, last) の要素を取り除き、次の要素を指すイテレータを返す。
		iterator erase(const_iterator first, const_iterator last) {
			if (first == last) {
				return iterator(this, first.m_index);
			}

			size_type start = first.m_index;
			size_type count = static_cast<size_type>(last.m_index - first.m_index);
			for (size_type i = 0; i < count; ++i) {
				m_index.erase(m_keys[start + i]);
			}

			m_keys.erase(m_keys.begin() + static_cast<difference_type>(start),
				m_keys.begin() + static_cast<difference_type>(start + count));
			m_values.erase(m_values.begin() + static_cast<difference_type>(start),
				m_values.begin() + static_cast<difference_type>(start + count));

			rebuild_index_from(start);
			return iterator(this, start);
		}

	public:
		//! @brief 内容を入れ替える。
		void swap(IndexMap& other) noexcept {
			using std::swap;
			swap(m_keys, other.m_keys);
			swap(m_values, other.m_values);
			swap(m_index, other.m_index);
		}

		//! @brief 2 つのマップの内容を入れ替える。
		friend void swap(IndexMap& lhs, IndexMap& rhs) noexcept {
			lhs.swap(rhs);
		}

	private:
		template<class K, class... Args>
		Pair<iterator, bool> try_emplace_impl(K&& key, Args&&... args) {
			auto found = m_index.find(key);
			if (found != m_index.end()) {
				return { iterator(this, found->second), false };
			}

			m_keys.emplace_back(std::forward<K>(key));
			try {
				m_values.emplace_back(std::forward<Args>(args)...);
			}
			catch (...) {
				m_keys.pop_back();
				throw;
			}

			auto index = size() - 1;
			m_index.emplace(m_keys[index], index);
			return { iterator(this, index), true };
		}

		void rebuild_index_from(size_type start) {
			for (size_type i = start; i < size(); ++i) {
				m_index[m_keys[i]] = i;
			}
		}

		value_type remove_preserve_order(size_type index) {
			AMUSE_ASSERT_EXPR(index < size());
			value_type removed(m_keys[index], m_values[index]);
			key_type keyCopy = m_keys[index];

			m_keys.erase(m_keys.begin() + static_cast<difference_type>(index));
			m_values.erase(m_values.begin() + static_cast<difference_type>(index));
			m_index.erase(keyCopy);
			rebuild_index_from(index);

			return removed;
		}

		value_type remove_swap_last(size_type index) {
			AMUSE_ASSERT_EXPR(index < size());
			value_type removed(m_keys[index], m_values[index]);
			const size_type lastIndex = size() - 1;
			key_type keyCopy = m_keys[index];

			m_index.erase(keyCopy);

			if (index != lastIndex) {
				m_keys[index] = std::move(m_keys[lastIndex]);
				m_values[index] = std::move(m_values[lastIndex]);
				m_index[m_keys[index]] = index;
			}

			m_keys.pop_back();
			m_values.pop_back();

			return removed;
		}

	private:
		key_storage_type m_keys;
		mapped_storage_type m_values;
		index_map_type m_index;
	};

}
