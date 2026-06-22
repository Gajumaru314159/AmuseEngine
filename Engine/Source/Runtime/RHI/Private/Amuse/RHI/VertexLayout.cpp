//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/RHI/VertexLayout.h>

namespace Amuse::RHI {
    using namespace Amuse::Core;

	namespace {

		//! @brief      頂点レイアウト実装
		class VertexLayoutImpl : public VertexLayout {
		public:

			//! @brief      コンストラクタ
			explicit VertexLayoutImpl(const VertexLayoutDesc& desc)
				: m_desc(desc) {
				manage();
			}

			//! @brief      定義を取得
			const VertexLayoutDesc& getDesc()const override {
				return m_desc;
			}

		private:

			VertexLayoutDesc m_desc;

		};

	}

	//! @brief  生成
	Ref<VertexLayout> VertexLayout::Create(const VertexLayoutDesc& desc) {
		return new VertexLayoutImpl(desc);
	}

}
