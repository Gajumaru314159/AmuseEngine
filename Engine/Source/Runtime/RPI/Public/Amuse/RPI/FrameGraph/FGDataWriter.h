//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/RPI/FrameGraph/FG.h>

namespace Amuse::RPI {

	//! @brief FG(FrameGraph)の内部構造データ
	struct FGData {

		//! @brief FrameGraph パスのデバッグ情報
		struct Pass {
			u32 id; //!< パスID
			String name; //!< パス名
			bool culled; //!< カリング状態
		};
		//! @brief FrameGraph リソースのデバッグ情報
		struct Resource {
			u32 id; //!< リソースID
			String name; //!< リソース名
			String desc; //!< リソース説明
			bool transient; //!< 一時リソースフラグ
			u32 createdPassId; //!< 生成パスID
			Vector<u32> readers; //!< 読み込みパスID
			Vector<u32> writers; //!< 書き込みパスID
		};

		Vector<Pass> passes; //!< パス一覧
		Vector<u32> reads; //!< 読み込みリソースID
		Vector<u32> writes; //!< 書き込みリソースID
		Map<u32, Resource> resources; //!< リソース一覧
	};

	//! @brief FG::debugOutput() を使用してFG(FrameGraph)の内部データを FGData に書き込むクラス
	class FGDataWriter{
	public:
		//! @brief 書き込み先データを指定して生成する
		FGDataWriter(FGData& data);
		//! @brief パスノードのデバッグ情報を書き込む
		void operator()(const PassNode& node, const std::vector<ResourceNode>& resourceNodes);
		//! @brief リソースノードのデバッグ情報を書き込む
		void operator()(const ResourceNode& node, const ResourceEntry& entry, const std::vector<PassNode>& passNodes);
		//! @brief 保持しているデバッグ情報をストリームへ出力する
		void flush(std::ostream& os) const;
	private:
		FGData& m_data;
	};

}
