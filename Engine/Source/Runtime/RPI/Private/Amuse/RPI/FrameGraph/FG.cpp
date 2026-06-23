//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/RPI/FrameGraph/FG.h>
#include <Amuse/RPI/FrameGraph/FGDummy.h>
#include <Amuse/RPI/FrameGraph/FGTexture.h>
#include <Amuse/RPI/FrameGraph/FGBuffer.h>
#include <fg/FrameGraph.hpp>
#include <fstream>

namespace Amuse::RPI {

	//! @brief      FGResourceのRenderTextureDescを取得する
	const RHI::RenderTextureDesc& FG::getTextureDesc(FGResource texture) {
		AMUSE_ASSERT_EXPR(texture.type == FGResourceType::Texture);
		return m_fg.getDescriptor<FGTexture>(static_cast<FrameGraphResource>(texture.value));
	}

	//! @brief      FGResourceのBufferDescを取得する
	const RHI::BufferDesc& FG::getBufferDesc(FGResource buffer) {
		AMUSE_ASSERT_EXPR(buffer.type == FGResourceType::Buffer);
		return m_fg.getDescriptor<FGBuffer>(static_cast<FrameGraphResource>(buffer.value));
	}

	//! @brief      RenderTextureをインポートする
	FGResource FG::import(const Ref<RHI::RenderTexture>& texture) {
		if (!texture)return {};
		return { FGResourceType::Texture, m_fg.import(texture->desc().name.str(), texture->descOfRenderTexture(), FGTexture{ texture }) };
	}

	//! @brief      Bufferをインポートする
	FGResource FG::import(const Ref<RHI::Buffer>& buffer) {
		if (!buffer)return {};
		return { FGResourceType::Buffer, m_fg.import(buffer->getDesc().name.str(), buffer->getDesc(), FGBuffer{ buffer }) };
	}

	//! @brief      FGResourceが有効な値か
	bool FG::isValid(FGResource resource) const {
		return m_fg.isValid(static_cast<FrameGraphResource>(resource.value));
	}

	//! @brief      FrameGraphをコンパイルして実行可能な状態にする
	//! @details	追加されたパスの依存関係を考慮してソートし、必要なパスのみを実行します
	void FG::compile() {
		m_fg.compile();
	}

	//! @brief     コンパイルされたパスを実行する
	void FG::execute(Ref<RHI::CommandList>& cmd, FGResourcePool& pool) {
		m_fg.execute(&cmd, &pool);
	}

	//! @brief      dot形式でFrameGraphの依存関係を出力する
	void FG::save(StringView name) {
		std::ofstream f{ name.data() };
		f << m_fg;
	}



	//! @brief      コンストラクタ
	FGResources::FGResources(FrameGraphPassResources& resources)
		: m_resources(resources)
	{}

	//! @brief      FGResourceからRenderTextureのインスタンスを取得する
	Ref<RHI::RenderTexture> FGResources::getTexture(FGResource texture) {
		AMUSE_ASSERT_EXPR(texture.type == FGResourceType::Texture);
		return m_resources.get<FGTexture>(texture.value).instance;
	}

	//! @brief      FGResourceからBufferのインスタンスを取得する
	Ref<RHI::Buffer> FGResources::getBuffer(FGResource buffer) {
		AMUSE_ASSERT_EXPR(buffer.type == FGResourceType::Buffer);
		return m_resources.get<FGBuffer>(buffer.value).instance;
	}

	//! @brief      FGResourceのRenderTextureDescを取得する
	const RHI::RenderTextureDesc& FGResources::getTextureDesc(FGResource texture) {
		AMUSE_ASSERT_EXPR(texture.type == FGResourceType::Texture);
		return m_resources.getDescriptor<FGTexture>(texture.value);
	}

	//! @brief      FGResourceのBufferDescを取得する
	const RHI::BufferDesc& FGResources::getBufferDesc(FGResource buffer) {
		AMUSE_ASSERT_EXPR(buffer.type == FGResourceType::Buffer);
		return m_resources.getDescriptor<FGBuffer>(buffer.value);
	}




	FGBuilder::FGBuilder(FrameGraph::Builder& builder)
		: m_builder(builder)
	{}

	FGResource FGBuilder::create(const RHI::RenderTextureDesc& desc) {
		return {FGResourceType::Texture,m_builder.create<FGTexture>(std::string_view(desc.name.data(), desc.name.size()), desc)};
	}

	FGResource FGBuilder::create(const RHI::BufferDesc& desc) {
		return {FGResourceType::Buffer,m_builder.create<FGBuffer>(std::string_view(desc.name.data(), desc.name.size()), desc)};
	}

	FGResource FGBuilder::createDummy() {
		return { FGResourceType::Dummy, m_builder.create<FGDummy>("Dummy", 0) };
	}

	FGResource FGBuilder::read(FGResource resource, u32 flags) {
		return { resource.type, m_builder.read(resource.value, flags) };
	}

	FGResource FGBuilder::write(FGResource resource, u32 flags) {
		return { resource.type, m_builder.write(resource.value, flags) };
	}

	FGBuilder& FGBuilder::setSideEffect() {
		m_builder.setSideEffect();
		return *this;
	}

}