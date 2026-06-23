//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/RPI/FrameGraph/FG.h>

namespace Amuse::RPI {

	//! @brief FG(FrameGraph)の内部構造データ
	struct FGData {

		struct Pass {
			u32 id;
			String name;
			bool culled;
		};
		struct Resource {
			u32 id;
			String name;
			String desc;
			bool transient;
			u32 createdPassId;
			Vector<u32> readers;
			Vector<u32> writers;
		};

		Vector<Pass> passes;
		Vector<u32> reads;
		Vector<u32> writes;
		Map<u32, Resource> resources;
	};

	//! @brief FG::debugOutput() を使用してFG(FrameGraph)の内部データを FGData に書き込むクラス
	class FGDataWriter{
	public:
		FGDataWriter(FGData& data);
		void operator()(const PassNode& node, const std::vector<ResourceNode>& resourceNodes);
		void operator()(const ResourceNode& node, const ResourceEntry& entry, const std::vector<PassNode>& passNodes);
		void flush(std::ostream& os) const;
	private:
		FGData& m_data;
	};

}