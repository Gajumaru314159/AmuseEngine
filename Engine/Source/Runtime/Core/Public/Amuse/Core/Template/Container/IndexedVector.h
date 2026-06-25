//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Log/LogMacro.h>
#include <Amuse/Core/Log/Assertion.h>
#include <Amuse/Core/Template/Container/Vector.h>
#include <limits>

namespace Amuse {

	template<class T, class TIndex = uint16_t, class TDataVector = Vector<T>, class TIndexVector = Vector<TIndex>>
	//! @brief インデックス付きベクターを表す。
	//! @ingroup AmuseCore
	class IndexedVector {
	public:
		static constexpr TIndex NoFreeSlot = std::numeric_limits<TIndex>::max(); //!< 空きスロットなしを表す値
	public:
		//! @brief 容量を予約する。
		void reserve(size_t size) {
			m_indices.reserve(size);
			m_data.reserve(size);
			m_free.reserve(size);
		}

		//! @brief 全要素と空きインデックス情報を取り除く。
		void clear() {
			m_indices.clear();
			m_data.clear();
			m_free.clear();
		}

		//! @brief 新しい論理インデックスを確保し、空きスロットがあれば再利用する。
		TIndex push() {

			// 空き領域がある場合はそこを使用
			if (!m_free.empty()) {
				TIndex index = m_free.back();
				m_free.pop_back();
				return index;
			}

			TIndex index = static_cast<TIndex>(m_indices.size());

			// 枯渇
			if (index == NoFreeSlot) {
				LOG_FATAL("インデックスが最大数{}を超えました",index);
				return NoFreeSlot;
			}

			m_indices.push_back(index);

			return index;
		}

		//! @brief 指定した論理インデックスの要素を取り除き、インデックスを空きスロットに戻す。
		void erase(TIndex index) {
			TIndex dataIndex = m_indices.at(index);

			// 後のデータを空き領域に詰める
			m_data.at(dataIndex) = m_data.back();
			m_data.pop_back();

			// 空き領域に追加
			m_free.push_back(index);
		}

		//! @brief 指定した要素ポインタに対応する要素を取り除く。
		void erase(T* data) {
			if(data == nullptr) {
				return;
			}
			auto dataIndex = data - m_data.data();

			// このコンテナで管理されているデータではない
			if (dataIndex < 0 || m_data.size() <= dataIndex) {
				return;
			}

			auto itr = std::find(m_indices.begin(), m_indices.end(), dataIndex);
			AMUSE_ASSERT_EXPR(itr != m_indices.end());
			auto index = std::distance(m_indices.beginn(),itr);

			erase(index);
		}

		//! @brief 指定位置の要素へアクセスする。
		T& at(TIndex index) {
			return m_data.at(m_indices.at(index));
		}
		//! @brief 指定位置の要素へアクセスする。
		const T& at(TIndex index) const {			
			return m_data.at(m_indices.at(index));
		}

		//! @brief 現在格納している実データの要素数を返す。
		size_t size() const {
			return m_data.size();
		}
		//! @brief 実データの先頭ポインタを返す。
		T* data() {
			return m_data.data();
		}
		//! @brief 実データの読み取り専用先頭ポインタを返す。
		const T* data() const {
			return m_data.data();
		}

		//! @brief 実データを保持する内部コンテナへの参照を返す。
		TDataVector& container() { return m_data; }
		//! @brief 実データを保持する内部コンテナへの読み取り専用参照を返す。
		const TDataVector& container() const { return m_data; }

	private:
		TIndexVector m_indices;
		TIndexVector m_free;
		TDataVector  m_data;
	};

}
