//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/Core/Allocator/Utility/SpanAllocator.h>
#include <Amuse/Core/Log/Assertion.h>

#include <algorithm>
#include <limits>

namespace Amuse::Core {

	//! @brief			コンストラクタ
	//! @param growOnly 最大サイズを縮小させないか
	SpanAllocator::SpanAllocator(bool growOnly)
		: m_growOnly(growOnly)
	{
	}

	//! @brief		連続範囲を割り当てる
	//! @param num	割り当てる要素数
	//! @return		割り当てられた範囲の開始オフセット
	s32 SpanAllocator::allocate(s32 num) {

		AMUSE_ASSERT(0 < num, "割り当て数は1以上である必要があります。");

		s32 foundIndex = searchFreeList(num, m_firstNonEmptySpan);
		if (foundIndex < 0 && !m_pendingFreeSpans.empty()) {
			// 統合したら割り当てできるかも
			consolidate();
			foundIndex = searchFreeList(num, m_firstNonEmptySpan);
		}

		m_usedSize += num;

		if (foundIndex < 0) {
			// 空きがないので末尾を拡張して割り当てる
			const s32 start = m_currentMaxSize;
			m_currentMaxSize += num;
			m_peakMaxSize = std::max(m_peakMaxSize, m_currentMaxSize);
			return start;
		}

		auto& freeSpan = m_freeSpans.at(foundIndex);
		const s32 start = freeSpan.start;
		freeSpan.start += num;
		freeSpan.num -= num;

		if (freeSpan.num == 0 && m_firstNonEmptySpan == foundIndex) {
			++m_firstNonEmptySpan;
		}

		return start;
	}

	//! @brief			割り当て済みの連続範囲を解放する
	//! @param base		解放する範囲の開始オフセット
	//! @param num		解放する要素数
	void SpanAllocator::free(s32 base, s32 num) {

		AMUSE_ASSERT(0 <= base, "解放位置が範囲外です。base={}", base);
		AMUSE_ASSERT(0 < num, "解放数は1以上である必要があります。");
		AMUSE_ASSERT(base + num <= m_currentMaxSize, "解放範囲が割り当て済み範囲を超えています。base={} num={} max={}", base, num, m_currentMaxSize);
		AMUSE_ASSERT(num <= m_usedSize, "解放数が現在の割り当て数を超えています。num={} allocated={}", num, m_usedSize);

		m_pendingFreeSpans.push_back(AllocationSpan{ base, num });
		m_usedSize -= num;
	}

	//! @brief	割り当て状態を初期状態に戻す
	void SpanAllocator::reset() {
		m_usedSize = 0;
		m_currentMaxSize = 0;
		m_peakMaxSize = 0;
		m_firstNonEmptySpan = 0;
		m_freeSpans.clear();
		m_pendingFreeSpans.clear();
	}

	//! @brief	割り当て状態を初期状態に戻し、内部バッファを解放する
	void SpanAllocator::empty() {
		reset();
		m_freeSpans.shrink_to_fit();
		m_pendingFreeSpans.shrink_to_fit();
	}

	//! @brief	解放待ち範囲を統合し、隣接する空き範囲を結合する
	void SpanAllocator::consolidate() {

		if (m_pendingFreeSpans.empty() && m_firstNonEmptySpan == 0) {
			return;
		}

		std::sort(m_pendingFreeSpans.begin(), m_pendingFreeSpans.end());

		Vector<AllocationSpan> freeSpans;
		freeSpans.reserve(m_freeSpans.size() + m_pendingFreeSpans.size());

		constexpr s32 max = std::numeric_limits<s32>::max();
		s32 previousEnd = -1;
		size_t iPendingFree = 0;

		for (size_t iFree = 0; iFree < m_freeSpans.size() || iPendingFree < m_pendingFreeSpans.size();) {
			AllocationSpan allocation = (iFree < m_freeSpans.size()) ? m_freeSpans[iFree] : AllocationSpan{ max, 0 };

			if (iPendingFree < m_pendingFreeSpans.size() && m_pendingFreeSpans[iPendingFree].start < allocation.start) {
				allocation = m_pendingFreeSpans[iPendingFree];
				++iPendingFree;
			} else {
				++iFree;
			}

			AMUSE_ASSERT(allocation.start < max, "SpanAllocator の統合処理に失敗しました。");

			if (allocation.num <= 0) {
				continue;
			}

			if (previousEnd == allocation.start) {
				freeSpans.back().num += allocation.num;
			} else {
				freeSpans.push_back(allocation);
			}
			previousEnd = freeSpans.back().start + freeSpans.back().num;
		}

		if (!freeSpans.empty()) {
			auto& last = freeSpans.back();
			// 末尾のFreeSpanを除去(Span管理だけなのでm_growOnlyは関係ない)
			if (last.start + last.num == m_currentMaxSize) {
				m_currentMaxSize -= last.num;
				freeSpans.pop_back();
			}
		}

		m_freeSpans = std::move(freeSpans);
		m_pendingFreeSpans.clear();
		m_firstNonEmptySpan = 0;
	}

	//! @brief	現在割り当てられている要素数を取得する
	s32 SpanAllocator::allocatedSize() const {
		return m_usedSize;
	}

	//! @brief	使用した線形範囲の最大サイズを取得する
	s32 SpanAllocator::maxSize() const {
		return m_growOnly ? m_peakMaxSize : m_currentMaxSize;
	}

	//! @brief	空きスパン数を取得する
	s32 SpanAllocator::freeSpanCount() const {
		return static_cast<s32>(m_freeSpans.size() + m_pendingFreeSpans.size());
	}

	//! @brief	統合待ちの空きスパン数を取得する
	s32 SpanAllocator::pendingFreeSpanCount() const {
		return static_cast<s32>(m_pendingFreeSpans.size());
	}

	//! @brief		指定インデックスが未使用か判定する
	//! @param index	判定するインデックス
	//! @return		未使用であれば true
	bool SpanAllocator::isFree(s32 index) const {

		if (index < 0) {
			return true;
		}
		if (m_currentMaxSize <= index) {
			return true;
		}

		for (const auto& span : m_freeSpans) {
			if (span.start <= index && index < span.start + span.num) {
				return true;
			}
		}
		for (const auto& span : m_pendingFreeSpans) {
			if (span.start <= index && index < span.start + span.num) {
				return true;
			}
		}

		return false;
	}

	//! @brief					指定サイズを満たす空きスパンを検索する
	//! @param num				必要な要素数
	//! @param searchStartIndex	検索開始インデックス
	//! @return					見つかった空きスパンのインデックス。見つからない場合は -1
	s32 SpanAllocator::searchFreeList(s32 num, s32 searchStartIndex) const {

		for (s32 index = searchStartIndex; index < static_cast<s32>(m_freeSpans.size()); ++index) {
			if (num <= m_freeSpans[index].num) {
				return index;
			}
		}

		return -1;
	}

}
