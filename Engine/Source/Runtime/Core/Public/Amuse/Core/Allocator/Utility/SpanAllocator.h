//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/CoreTypes.h>
#include <Amuse/Core/Template/Container/Vector.h>

namespace Amuse::Core {

	//! @brief	連続した整数範囲を管理するスパンアロケータ
	//!
	//! @details	任意サイズの連続範囲を割り当て、解放済み範囲を再利用します。
	//!			解放された範囲は一時リストに積まれ、必要時または consolidate() 呼び出し時に
	//!			ソートと結合を行います。大量の allocate/free をまとめて処理する用途に向いています。
	class SpanAllocator {
	public:

		//! @brief			コンストラクタ
		//! @param growOnly	true の場合、reset()/empty() まで maxSize() が過去最大値から縮小しません。
		explicit SpanAllocator(bool growOnly = false);

		//! @brief		連続範囲を割り当てる
		//! @param num	割り当てる要素数
		//! @return		割り当てられた範囲の開始オフセット
		s32 allocate(s32 num = 1);

		//! @brief			割り当て済みの連続範囲を解放する
		//! @param base		解放する範囲の開始オフセット
		//! @param num		解放する要素数
		void free(s32 base, s32 num = 1);

		//! @brief		割り当て状態を初期状態に戻す
		//!
		//! @details	内部バッファの容量は維持されます。
		void reset();

		//! @brief		割り当て状態を初期状態に戻し、内部バッファを解放する
		void empty();

		//! @brief		解放待ち範囲を統合し、隣接する空き範囲を結合する
		void consolidate();

		//! @brief		現在割り当てられている要素数を取得する
		//! @return		割り当て済み要素数
		s32 allocatedSize() const;

		//! @brief		使用した線形範囲の最大サイズを取得する
		//! @return		growOnly が true の場合は reset()/empty() 以降のピーク値、それ以外は現在値
		s32 maxSize() const;

		//! @brief		空きスパン数を取得する
		//! @return		統合済みと統合待ちを合計した空きスパン数
		s32 freeSpanCount() const;

		//! @brief		統合待ちの空きスパン数を取得する
		//! @return		統合待ちの空きスパン数
		s32 pendingFreeSpanCount() const;

		//! @brief		指定インデックスが未使用か判定する
		//! @param index	判定するインデックス
		//! @return		未使用であれば true
		bool isFree(s32 index) const;

	private:

		struct AllocationSpan {
			s32 start = 0;
			s32 num = 0;

			bool operator<(const AllocationSpan& other) const {
				return start < other.start;
			}
		};

		s32 searchFreeList(s32 num, s32 searchStartIndex = 0) const;

	private:

		s32 m_usedSize = 0;			//!< 現在割り当てられている要素数
		s32 m_currentMaxSize = 0;		//!< 現在使用している線形範囲サイズ
		s32 m_peakMaxSize = 0;			//!< reset()/empty() 以降の最大線形範囲サイズ
		s32 m_firstNonEmptySpan = 0;		//!< 空でない可能性がある最初の空きスパン
		Vector<AllocationSpan> m_freeSpans;			//!< ソート済みの空きスパン
		Vector<AllocationSpan> m_pendingFreeSpans;	//!< 統合待ちの空きスパン
		bool m_growOnly = false;			//!< 最大サイズを縮小させないか

	};

}
