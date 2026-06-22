//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/RHI/Buffer.h>
#include <Amuse/RHI/Constants.h>
#include <Amuse/RHI/RenderTexture.h>
#include <Amuse/RHI/Types/CommandParam.h>
#include <Amuse/VulkanRHI/Buffer/VulkanBuffer.h>
#include <Amuse/VulkanRHI/Command/VulkanCommandList.h>
#include <Amuse/VulkanRHI/Descriptor/VulkanDescriptorTable.h>
#include <Amuse/VulkanRHI/PipelineState/VulkanComputePipelineState.h>
#include <Amuse/VulkanRHI/PipelineState/VulkanPipelineState.h>
#include <Amuse/VulkanRHI/Query/VulkanTimerQuery.h>
#include <Amuse/VulkanRHI/RootSignature/VulkanRootSignature.h>
#include <Amuse/VulkanRHI/SwapChain/VulkanSwapChain.h>
#include <Amuse/VulkanRHI/Texture/VulkanTexture.h>
#include <Amuse/VulkanRHI/Utility/TypeConverter.h>
#include <Amuse/VulkanRHI/VulkanDevice.h>

namespace Amuse::RHI {
    using namespace Amuse::Core;

	namespace {

		struct VulkanResourceState {
			vk::ImageLayout layout;
			vk::AccessFlags access;
		};

		//! @brief  TextureState を Vulkan のレイアウト/アクセスに変換
		VulkanResourceState ConvertTextureState(TextureState state) {
			switch (state) {
			case TextureState::Unknown:
			case TextureState::Common:
				return { vk::ImageLayout::eUndefined, vk::AccessFlagBits::eNone };
			case TextureState::ShaderResource:
				return { vk::ImageLayout::eShaderReadOnlyOptimal, vk::AccessFlagBits::eShaderRead };
			case TextureState::UnorderedAccess:
				return { vk::ImageLayout::eGeneral, vk::AccessFlags(VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT) };
			case TextureState::RenderTarget:
				return { vk::ImageLayout::eColorAttachmentOptimal, vk::AccessFlags(VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT) };
			case TextureState::DepthRead:
				return { vk::ImageLayout::eDepthStencilReadOnlyOptimal, vk::AccessFlagBits::eDepthStencilAttachmentRead };
			case TextureState::DepthWrite:
				return { vk::ImageLayout::eDepthStencilAttachmentOptimal, vk::AccessFlags(VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT) };
			case TextureState::CopySource:
			case TextureState::ResolveSource:
				return { vk::ImageLayout::eTransferSrcOptimal, vk::AccessFlagBits::eTransferRead };
			case TextureState::CopyDest:
			case TextureState::ResolveDest:
				return { vk::ImageLayout::eTransferDstOptimal, vk::AccessFlagBits::eTransferWrite };
			case TextureState::Present:
				return { vk::ImageLayout::ePresentSrcKHR, vk::AccessFlagBits::eNone };
			}
			return { vk::ImageLayout::eUndefined, vk::AccessFlagBits::eNone };
		}

		//! @brief  BufferState を Vulkan のアクセスに変換
		VulkanResourceState ConvertBufferState(BufferState state) {
			switch (state) {
			case BufferState::Unknown:
			case BufferState::Common:
				return { vk::ImageLayout::eGeneral, vk::AccessFlagBits::eNone };
			case BufferState::Vertex:
				return { vk::ImageLayout::eGeneral, vk::AccessFlagBits::eVertexAttributeRead };
			case BufferState::Index:
				return { vk::ImageLayout::eGeneral, vk::AccessFlagBits::eIndexRead };
			case BufferState::Constant:
				return { vk::ImageLayout::eGeneral, vk::AccessFlagBits::eUniformRead };
			case BufferState::ShaderResource:
				return { vk::ImageLayout::eGeneral, vk::AccessFlagBits::eShaderRead };
			case BufferState::UnorderedAccess:
				return { vk::ImageLayout::eGeneral, vk::AccessFlags(VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT) };
			case BufferState::IndirectArgument:
				return { vk::ImageLayout::eGeneral, vk::AccessFlagBits::eIndirectCommandRead };
			case BufferState::CopySource:
				return { vk::ImageLayout::eGeneral, vk::AccessFlagBits::eTransferRead };
			case BufferState::CopyDest:
				return { vk::ImageLayout::eGeneral, vk::AccessFlagBits::eTransferWrite };
			}
			return { vk::ImageLayout::eGeneral, vk::AccessFlagBits::eNone };
		}

		//! @brief  TextureFormat からアスペクトマスクを取得
		vk::ImageAspectFlags GetAspectFlags(TextureFormat format) {
			vk::ImageAspectFlags flags;
			if (TextureFormatUtility::HasColor(format)) flags |= vk::ImageAspectFlagBits::eColor;
			if (TextureFormatUtility::HasDepth(format)) flags |= vk::ImageAspectFlagBits::eDepth;
			if (TextureFormatUtility::HasStencil(format)) flags |= vk::ImageAspectFlagBits::eStencil;
			return flags;
		}

		//! @brief  サブリソースレイヤを作成
		vk::ImageSubresourceLayers MakeSubresource(const TextureDesc& desc, const TextureSubresource& subresource) {
			vk::ImageSubresourceLayers layers;
			layers.aspectMask = GetAspectFlags(desc.format);
			layers.mipLevel = subresource.mipLevel;
			layers.baseArrayLayer = GetTextureSubresourceArrayLayer(desc, subresource);
			layers.layerCount = 1;
			return layers;
		}

		//! @brief  サブリソース範囲を作成
		vk::ImageSubresourceRange MakeSubresourceRange(TextureFormat format, const TextureSubresourceRange& range) {
			vk::ImageSubresourceRange result;
			result.aspectMask = GetAspectFlags(format);
			result.baseMipLevel = range.baseMipLevel;
			result.levelCount = range.mipLevelCount == TextureSubresourceRange::All ? VK_REMAINING_MIP_LEVELS : range.mipLevelCount;
			result.baseArrayLayer = range.baseArrayLayer;
			result.layerCount = range.arrayLayerCount == TextureSubresourceRange::All ? VK_REMAINING_ARRAY_LAYERS : range.arrayLayerCount;
			return result;
		}

		u32 GetTextureFormatByteSize(TextureFormat format) {
			switch (format) {
			case TextureFormat::SDR:
			case TextureFormat::RGBA8:
			case TextureFormat::RGBA8_SRGB:
				return 4;
			case TextureFormat::HDR:
			case TextureFormat::RGBA16:
				return 8;
			case TextureFormat::RGBA32:
				return 16;
			case TextureFormat::RGB32:
				return 12;
			case TextureFormat::RGB8:
				return 3;
			case TextureFormat::RG32:
				return 8;
			case TextureFormat::RG16:
				return 4;
			case TextureFormat::RG8:
				return 2;
			case TextureFormat::R32:
				return 4;
			case TextureFormat::R16:
				return 2;
			case TextureFormat::R8:
				return 1;
			default:
				return 0;
			}
		}

	}

	//! @brief  コンストラクタ
	VulkanCommandList::VulkanCommandList(VulkanDevice& device, const CommandListDesc& desc)
		: m_device(device)
		, m_desc(desc)
	{
		vk::CommandPoolCreateInfo info;
		info.queueFamilyIndex = m_device.getQueryFamilyIndex();
		info.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;

		m_commandPool = device.getDevice().createCommandPool(info, m_device.getAllocationCallbacks());

		vk::CommandBufferAllocateInfo allocInfo;
		allocInfo.commandPool = *m_commandPool;
		allocInfo.commandBufferCount = 1;
		allocInfo.level = vk::CommandBufferLevel::ePrimary;

		m_commandBuffer = std::move(m_device.getDevice().allocateCommandBuffers(allocInfo).front());
		
		m_device.setName(m_commandPool, m_desc.name);
		m_device.setName(m_commandBuffer, m_desc.name);

		manage();
	}

	//! @brief デストラクタ
	VulkanCommandList::~VulkanCommandList() {
	}

	//! @brief      名前を取得
	const String& VulkanCommandList::getName() const {
		return m_desc.name;
	}

	//! @brief  描画開始
	void VulkanCommandList::begin() {
		m_commandPool.reset();
		m_commandBuffer.reset();
		m_temporaryBuffers.clear();
		m_commandBuffer.begin({});
	}


	//! @brief  描画終了
	void VulkanCommandList::end() {
		m_commandBuffer.end();
	}


	//! @brief  描画終了
	//! @brief 自動バリアを有効化
	void VulkanCommandList::setEnableAutomaticBarriers(bool enable) {
		m_enableAutomaticBarriers = enable;
	}

	//! @brief テクスチャ遷移バリアを追加
	void VulkanCommandList::addTextureBarrier(const TextureBarrier& barrier) {
		if (barrier.after == TextureState::Unknown) return;
		if (auto texture = barrier.texture.cast<VulkanTexture>()) {
			const auto before = ConvertTextureState(barrier.before);
			const auto after = ConvertTextureState(barrier.after);
			m_cache.addTexture(
				*texture->getNative(),
				before.layout,
				after.layout,
				GetAspectFlags(texture->desc().format),
				before.access,
				after.access,
				barrier.range);
		}
	}

	//! @brief バッファ遷移バリアを追加
	void VulkanCommandList::addBufferBarrier(const BufferBarrier& barrier) {
		if (barrier.after == BufferState::Unknown) return;
		if (auto buffer = barrier.buffer.cast<VulkanBuffer>()) {
			const auto before = ConvertBufferState(barrier.before == BufferState::Unknown ? buffer->getDesc().state : barrier.before);
			const auto after = ConvertBufferState(barrier.after);
			m_cache.addBuffer(buffer->getNative(), before.access, after.access);
		}
	}

	//! @brief テクスチャUAVバリアを追加
	void VulkanCommandList::addUAVBarrier(const Ref<Texture>& texture) {
		if (texture) {
			m_cache.addUAV();
		}
	}

	//! @brief バッファUAVバリアを追加
	void VulkanCommandList::addUAVBarrier(const Ref<Buffer>& buffer) {
		if (buffer) {
			m_cache.addUAV();
		}
	}

	//! @brief 追加済みバリアを記録
	void VulkanCommandList::commitBarriers() {
		m_cache.recordCommand(*m_commandBuffer);
	}

	//! @brief バッファをコピー
	void VulkanCommandList::copyBuffer(const BufferCopyRegion& region) {
		auto source = region.source.cast<VulkanBuffer>();
		auto target = region.target.cast<VulkanBuffer>();
		if (!source || !target) return;

		vk::BufferCopy copy;
		copy.srcOffset = region.sourceOffset;
		copy.dstOffset = region.targetOffset;
		copy.size = region.size == 0 ? std::min(source->getDesc().size - region.sourceOffset, target->getDesc().size - region.targetOffset) : region.size;
		m_commandBuffer.copyBuffer(source->getNative(), target->getNative(), copy);
	}

	//! @brief テクスチャをコピー
	void VulkanCommandList::copyTexture(const TextureCopyRegion& region) {
		auto source = region.source.cast<VulkanTexture>();
		auto target = region.target.cast<VulkanTexture>();
		if (!source || !target) return;

		vk::ImageCopy copy;
		copy.srcSubresource = MakeSubresource(source->desc(), region.sourceSubresource);
		copy.dstSubresource = MakeSubresource(target->desc(), region.targetSubresource);
		copy.srcOffset = vk::Offset3D(region.sourceOrigin.x, region.sourceOrigin.y, 0);
		copy.dstOffset = vk::Offset3D(region.targetOrigin.x, region.targetOrigin.y, 0);
		copy.extent = vk::Extent3D(region.size.width, region.size.height, std::max<s32>(region.size.depth, 1));
		m_commandBuffer.copyImage(*source->getNative(), vk::ImageLayout::eTransferSrcOptimal, *target->getNative(), vk::ImageLayout::eTransferDstOptimal, copy);
	}

	//! @brief バッファからテクスチャへコピー
	void VulkanCommandList::copyBufferToTexture(const BufferTextureCopyRegion& region) {
		auto buffer = region.buffer.cast<VulkanBuffer>();
		auto texture = region.texture.cast<VulkanTexture>();
		if (!buffer || !texture) return;

		vk::BufferImageCopy copy;
		copy.bufferOffset = region.bufferOffset;
		const auto bytesPerPixel = GetTextureFormatByteSize(texture->desc().format);
		copy.bufferRowLength = bytesPerPixel == 0 ? 0 : region.rowPitch / bytesPerPixel;
		copy.bufferImageHeight = region.rowPitch == 0 ? 0 : region.depthPitch / region.rowPitch;
		copy.imageSubresource = MakeSubresource(texture->desc(), region.textureSubresource);
		copy.imageOffset = vk::Offset3D(region.textureOrigin.x, region.textureOrigin.y, 0);
		copy.imageExtent = vk::Extent3D(region.size.width, region.size.height, std::max<s32>(region.size.depth, 1));
		m_commandBuffer.copyBufferToImage(buffer->getNative(), *texture->getNative(), vk::ImageLayout::eTransferDstOptimal, copy);
	}

	//! @brief テクスチャからバッファへコピー
	void VulkanCommandList::copyTextureToBuffer(const BufferTextureCopyRegion& region) {
		auto buffer = region.buffer.cast<VulkanBuffer>();
		auto texture = region.texture.cast<VulkanTexture>();
		if (!buffer || !texture) return;

		vk::BufferImageCopy copy;
		copy.bufferOffset = region.bufferOffset;
		const auto bytesPerPixel = GetTextureFormatByteSize(texture->desc().format);
		copy.bufferRowLength = bytesPerPixel == 0 ? 0 : region.rowPitch / bytesPerPixel;
		copy.bufferImageHeight = region.rowPitch == 0 ? 0 : region.depthPitch / region.rowPitch;
		copy.imageSubresource = MakeSubresource(texture->desc(), region.textureSubresource);
		copy.imageOffset = vk::Offset3D(region.textureOrigin.x, region.textureOrigin.y, 0);
		copy.imageExtent = vk::Extent3D(region.size.width, region.size.height, std::max<s32>(region.size.depth, 1));
		m_commandBuffer.copyImageToBuffer(*texture->getNative(), vk::ImageLayout::eTransferSrcOptimal, buffer->getNative(), copy);
	}

	//! @brief テクスチャへデータを書き込む
	void VulkanCommandList::writeTexture(const Ref<Texture>& texture, const TextureSubresource& subresource, BlobView data, u32 rowPitch, u32 depthPitch) {
		if (!texture || data.empty() || rowPitch == 0) return;

		const auto& desc = texture->desc();
		const auto width = subresource.region ? subresource.region->width() : desc.size.width;
		const auto height = subresource.region ? subresource.region->height() : desc.size.height;
		const auto depth = std::max<s32>(desc.type == TextureType::Texture3D ? desc.size.depth : 1, 1);
		const auto uploadDepthPitch = depthPitch == 0 ? rowPitch * height : depthPitch;
		const auto uploadSize = static_cast<u64>(uploadDepthPitch) * depth;

		auto upload = Buffer::Create(BufferDesc::ByteAddress(
			uploadSize,
			BufferState::CopySource,
			BufferFlag::CopySource));
		if (!upload) return;
		if (auto mapped = upload->map(CpuAccess::Write)) {
			std::memcpy(mapped, data.data(), std::min<size_t>(data.size(), uploadSize));
			upload->unmap();
		} else {
			upload->updateDirect(std::min<size_t>(data.size(), uploadSize), data.data(), 0);
		}

		BufferTextureCopyRegion region;
		region.buffer = upload;
		region.texture = texture;
		region.rowPitch = rowPitch;
		region.depthPitch = uploadDepthPitch;
		region.textureSubresource = subresource;
		region.textureOrigin = subresource.region ? Point{ subresource.region->left, subresource.region->top, 0 } : Point{};
		region.size = { width, height, depth };
		copyBufferToTexture(region);

		m_temporaryBuffers.emplace_back(upload);
	}

	//! @brief カラーテクスチャをクリア
	void VulkanCommandList::clearColorTexture(const Ref<Texture>& texture, const TextureSubresourceRange& range, const Color& color) {
		if (auto native = texture.cast<VulkanTexture>()) {
			vk::ClearColorValue value;
			value.float32 = vk::ArrayWrapper1D<float, 4>({ color.r, color.g, color.b, color.a });
			m_commandBuffer.clearColorImage(*native->getNative(), vk::ImageLayout::eTransferDstOptimal, value, MakeSubresourceRange(native->desc().format, range));
		}
	}

	//! @brief 深度ステンシルテクスチャをクリア
	void VulkanCommandList::clearDepthStencilTexture(const Ref<Texture>& texture, const TextureSubresourceRange& range, bool clearDepth, f32 depth, bool clearStencil, u8 stencil) {
		if (auto native = texture.cast<VulkanTexture>()) {
			vk::ClearDepthStencilValue value(depth, stencil);
			auto subresource = MakeSubresourceRange(native->desc().format, range);
			subresource.aspectMask = {};
			if (clearDepth && TextureFormatUtility::HasDepth(native->desc().format)) subresource.aspectMask |= vk::ImageAspectFlagBits::eDepth;
			if (clearStencil && TextureFormatUtility::HasStencil(native->desc().format)) subresource.aspectMask |= vk::ImageAspectFlagBits::eStencil;
			if (!subresource.aspectMask) return;
			m_commandBuffer.clearDepthStencilImage(*native->getNative(), vk::ImageLayout::eTransferDstOptimal, value, subresource);
		}
	}

	//! @brief バッファを32bit整数値でクリア
	void VulkanCommandList::clearBufferUInt(const Ref<Buffer>& buffer, u32 value) {
		auto native = buffer.cast<VulkanBuffer>();
		if (!native) return;
		m_commandBuffer.fillBuffer(native->getNative(), 0, native->getDesc().size, value);
	}

	//! @brief テクスチャをResolve
	void VulkanCommandList::resolveTexture(const Ref<Texture>& source, const Ref<Texture>& target, const TextureSubresourceRange& sourceRange, const TextureSubresourceRange& targetRange) {
		auto src = source.cast<VulkanTexture>();
		auto dst = target.cast<VulkanTexture>();
		if (!src || !dst) return;
		vk::ImageResolve region;
		TextureSubresource sourceSubresource{};
		sourceSubresource.arrayIndex = sourceRange.baseArrayLayer;
		sourceSubresource.mipLevel = sourceRange.baseMipLevel;
		TextureSubresource targetSubresource{};
		targetSubresource.arrayIndex = targetRange.baseArrayLayer;
		targetSubresource.mipLevel = targetRange.baseMipLevel;
		region.srcSubresource = MakeSubresource(src->desc(), sourceSubresource);
		region.dstSubresource = MakeSubresource(dst->desc(), targetSubresource);
		region.extent = vk::Extent3D(dst->desc().size.width, dst->desc().size.height, std::max<s32>(dst->desc().size.depth, 1));
		m_commandBuffer.resolveImage(*src->getNative(), vk::ImageLayout::eTransferSrcOptimal, *dst->getNative(), vk::ImageLayout::eTransferDstOptimal, region);
	}


	//! @brief レンダーパスを開始する 
	void VulkanCommandList::beginRenderPass(const BeginPassParam& param) {

		clearRenderTargets();

		if (m_enableAutomaticBarriers) {
			m_cache.clear();
		}

		s32 width = 0;
		s32 height = 0;

		FixedVector<vk::RenderingAttachmentInfo, RENDER_TARGET_MAX> colorAttachments;
		FixedVector<vk::RenderingAttachmentInfo, 1> depthAttachments;
		FixedVector<vk::RenderingAttachmentInfo, 1> stencilAttachments;

		for (auto [index,color] : Indexed(param.colors)) {
			if (auto p = color.texture.cast<VulkanTexture>()) {
				auto& attachment = colorAttachments.emplace_back();
				attachment.imageView = *p->getRTV();
				attachment.imageLayout = vk::ImageLayout::eColorAttachmentOptimal;
				attachment.loadOp = TypeConverter::Convert(color.beforeAccess);
				attachment.storeOp = TypeConverter::Convert(color.afterAccess);
				auto fcol = p->descOfRenderTexture().clear.color;
				attachment.clearValue.color.float32 = vk::ArrayWrapper1D<float, 4>({ fcol.r,fcol.g,fcol.b,fcol.a });

				width = color.texture->width();
				height = color.texture->height();

				m_colorTextures.push_back(color.texture);

				if (m_enableAutomaticBarriers) {
					m_cache.addTexture(*p->getNative(), vk::ImageLayout::eUndefined, vk::ImageLayout::eColorAttachmentOptimal, vk::ImageAspectFlagBits::eColor);
				}
			} else {
				LOG_FATAL("不正な引数。レンダーテクスチャが不正です。");
			}
		}
		if (param.depth.texture) {
			if (auto p = param.depth.texture.cast<VulkanTexture>()) {
				auto& attachment = depthAttachments.emplace_back();
				attachment.imageView = *p->getDSV();
				attachment.imageLayout = vk::ImageLayout::eDepthAttachmentOptimal;
				attachment.loadOp = TypeConverter::Convert(param.depth.beforeAccess);
				attachment.storeOp = TypeConverter::Convert(param.depth.afterAccess);
				attachment.clearValue.depthStencil.depth = p->descOfRenderTexture().clear.depth;

				width = param.depth.texture->width();
				height = param.depth.texture->height();

				m_depthStencilTextures.push_back(DepthStencilAttachmentState{ param.depth.texture, vk::ImageLayout::eDepthStencilAttachmentOptimal, vk::ImageAspectFlagBits::eDepth });

				if (m_enableAutomaticBarriers) {
					m_cache.addTexture(*p->getNative(), vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthAttachmentOptimal, vk::ImageAspectFlagBits::eDepth);
				}
			} else {
				LOG_FATAL("不正な引数。レンダーテクスチャが不正です。");
			}
		}
		if (param.stencil.texture) {
			if (auto p = param.stencil.texture.cast<VulkanTexture>()) {
				auto& attachment = stencilAttachments.emplace_back();
				attachment.imageView = *p->getDSV();
				attachment.imageLayout = vk::ImageLayout::eStencilAttachmentOptimal;
				attachment.loadOp = TypeConverter::Convert(param.stencil.beforeAccess);
				attachment.storeOp = TypeConverter::Convert(param.stencil.afterAccess);
				attachment.clearValue.depthStencil.stencil = p->descOfRenderTexture().clear.stencil;

				width = param.stencil.texture->width();
				height = param.stencil.texture->height();

				m_depthStencilTextures.push_back(DepthStencilAttachmentState{ param.stencil.texture, vk::ImageLayout::eStencilAttachmentOptimal, vk::ImageAspectFlagBits::eStencil });

				if (m_enableAutomaticBarriers) {
					m_cache.addTexture(*p->getNative(), vk::ImageLayout::eUndefined, vk::ImageLayout::eStencilAttachmentOptimal, vk::ImageAspectFlagBits::eStencil);
				}
			}
			else {
				LOG_FATAL("不正な引数。レンダーテクスチャが不正です。");
			}
		}

		// バリア設定
		if (m_enableAutomaticBarriers) {
			m_cache.recordCommand(*m_commandBuffer);
		}

		// 描画開始コマンド
		vk::RenderingInfo info;
		info.flags = vk::RenderingFlagBits{};
		info.renderArea.extent.width = width;
		info.renderArea.extent.height = height;
		info.layerCount = 1;
		info.viewMask = 0;
		if (!colorAttachments.empty()) {
			info.setColorAttachments(colorAttachments);
		}
		if (!depthAttachments.empty()) {
			info.pDepthAttachment = depthAttachments.data();
		}
		if (!stencilAttachments.empty()) {
			info.pStencilAttachment = stencilAttachments.data();
		}
		m_commandBuffer.beginRendering(info);

		// 初期設定としてViewportとScissorRectを設定
		Viewport viewport(0, 0, 1.0f * width, 1.0f * height);
		IntRect scissor(0, 0, 1.0f * width, 1.0f * height);

		setViewport(&viewport, 1);
		setScissorRect(&scissor, 1);

	}


	//! @brief 	レンダーパスを終了する
	void VulkanCommandList::endRenderPass() {
		
		m_commandBuffer.endRendering();

		if (m_enableAutomaticBarriers) {
			m_cache.clear();
		}
		
		if (m_enableAutomaticBarriers) {
			for (auto [i, color] : Indexed(m_colorTextures)) {
				if (auto texture = color.cast<VulkanTexture>()) {
					m_cache.addTexture(*texture->getNative(), vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::eShaderReadOnlyOptimal, vk::ImageAspectFlagBits::eColor);
				}
			}
			for (auto& attachment : m_depthStencilTextures) {
				if (auto texture = attachment.texture.cast<VulkanTexture>()) {
					m_cache.addTexture(*texture->getNative(), attachment.layout, vk::ImageLayout::eGeneral, attachment.aspectMask);
				}
			}
		}
		
		// リソースバリア
		if (m_enableAutomaticBarriers) {
			m_cache.recordCommand(*m_commandBuffer);
		}

		clearRenderTargets();
	}


	//! @brief レンダーターゲットをクリア
	void VulkanCommandList::clearRenderTargets() {
		m_colorTextures.clear();
		m_depthStencilTextures.clear();
	}


	//! @brief      スワップチェーンにテクスチャを適用
	void VulkanCommandList::applySwapChain(const Ref<SwapChain>& swapChain, const Ref<RenderTexture>& texture) {
		AMUSE_ASSERT_EXPR(*m_commandBuffer);
		if (auto pSwapChain = swapChain.cast<VulkanSwapChain>()) {
			Ref<CommandList> commandList = this;
			pSwapChain->recordApplySwapChain(commandList, texture);
		}
		else {
			LOG_ERROR("不正な引数。スワップチェーンが不正です。");
		}
	}


	//! @brief  シザー矩形を設定
	void VulkanCommandList::setScissorRect(const IntRect* pRect, s32 num) {

		FixedVector<vk::Rect2D, RENDER_TARGET_MAX> rects;
		for (s32 i = 0; i < num; ++i) {
			auto& rectIn = pRect[i];
			auto& rectOut = rects.emplace_back();
			rectOut.offset.x = rectIn.left;
			rectOut.offset.y = rectIn.top;
			rectOut.extent.width = rectIn.width();
			rectOut.extent.height = rectIn.height();
		}

		m_commandBuffer.setScissor(0, rects);

	}


	//! @brief  ビューポートを設定
	void VulkanCommandList::setViewport(const Viewport* pViewport, s32 num) {

		FixedVector<vk::Viewport, RENDER_TARGET_MAX> viewports;
		for (s32 i = 0; i < num;++i) {
			auto& viewportIn = pViewport[i];
			auto& viewportOut = viewports.emplace_back();
			viewportOut.x = viewportIn.top;
			viewportOut.y = viewportIn.left;
			viewportOut.width = viewportIn.width();
			viewportOut.height = viewportIn.height();
			viewportOut.minDepth = viewportIn.nearZ;
			viewportOut.maxDepth = viewportIn.farZ;
		}

		m_commandBuffer.setViewport(0, viewports);

	}


	//! @brief      頂点バッファを設定
	void VulkanCommandList::setVertexBuffers(Span<Ref<Buffer>> buffers, s32 first) {

		FixedVector<vk::Buffer, VERTEX_BUFFER_MAX> vkBuffers;
		FixedVector<vk::DeviceSize, VERTEX_BUFFER_MAX> offsets;

		for (auto& buffer : buffers) {
			if (auto p = buffer.cast<VulkanBuffer>()) {
				vkBuffers.push_back(p->getNative());
				offsets.push_back(0);
			} else {
				LOG_FATAL("不正な引数。頂点バッファが不正です。");
			}
		}

		m_commandBuffer.bindVertexBuffers(first,vkBuffers, offsets);
	}


	//! @brief      インデックスバッファを設定
	void VulkanCommandList::setIndexBuffer(const Ref<Buffer>& buffer) {

		if (auto p = buffer.cast<VulkanBuffer>()) {

			Optional<vk::IndexType> type;

			s32 stride = buffer->getDesc().stride;
			if (stride == 2) type = vk::IndexType::eUint16;
			if (stride == 4) type = vk::IndexType::eUint32;
			if (!type) LOG_FATAL("インデックスバッファが不正なストライド幅です。16bitか32bitから選択してください。");

			m_commandBuffer.bindIndexBuffer(p->getNative(), 0, type.value());
		} else {
			LOG_FATAL("不正な引数。インデックスバッファが不正です。");
		}

	}


	//! @brief      パイプラインステートを設定
	void VulkanCommandList::setPipelineState(const Ref<PipelineState>& pipeline) {

		if (auto p = pipeline.cast<VulkanPipelineState>()) {
			m_pipeline = pipeline;
			m_computePipeline = nullptr;
			m_commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *p->getNative());

			auto& desc = p->getDesc().rootSignature->getDesc();
			if (desc.flags.has(RootSignatureFlag::EnableBindless)) {
				s32 bindlessSlot = gsl::narrow<s32>(desc.layouts.size());
				m_device.setDescriptorHeaps(m_commandBuffer,p->getLayout(), bindlessSlot);			
			}
		} else {
			LOG_FATAL("不正な引数。パイプラインステートが不正です。");
		}

	}

	//! @brief      コンピュートパイプラインステートを設定
	void VulkanCommandList::setComputePipelineState(const Ref<ComputePipelineState>& pipeline) {

		if (auto p = pipeline.cast<VulkanComputePipelineState>()) {
			m_pipeline = nullptr;
			m_computePipeline = pipeline;
			m_commandBuffer.bindPipeline(vk::PipelineBindPoint::eCompute, p->getNative());
		} else {
			LOG_FATAL("不正な引数。コンピュートパイプラインステートが不正です。");
		}

	}


	//! @brief      描画
	void VulkanCommandList::draw(const DrawParam& param) {
		m_commandBuffer.draw(param.vertexCount, param.instanceCount, param.startVertex, param.startInstance);
	}


	//! @brief      インデックス描画
	void VulkanCommandList::drawIndexed(const DrawIndexedParam& param) {
		m_commandBuffer.drawIndexed(param.indexCount,param.instanceCount, param.startIndex,gsl::narrow_cast<int32_t>(param.startVertex),param.startInstance);
	}

	//! @brief      コンピュートディスパッチ
	void VulkanCommandList::dispatch(const DispatchParam& param) {
		m_commandBuffer.dispatch(param.groupX, param.groupY, param.groupZ);
	}

	//! @brief      Indirect描画
	void VulkanCommandList::drawIndirect(const IndirectCommandParam& param) {
		if (auto buffer = param.argumentBuffer.cast<VulkanBuffer>()) {
			m_commandBuffer.drawIndirect(buffer->getNative(), param.argumentOffset, param.commandCount, m_device.getDrawIndirectArgumentSize());
		} else {
			LOG_FATAL("不正な引数。Indirect引数バッファが不正です。");
		}
	}

	//! @brief      Indirectインデックス描画
	void VulkanCommandList::drawIndexedIndirect(const IndirectCommandParam& param) {
		if (auto buffer = param.argumentBuffer.cast<VulkanBuffer>()) {
			m_commandBuffer.drawIndexedIndirect(buffer->getNative(), param.argumentOffset, param.commandCount, m_device.getDrawIndexedIndirectArgumentSize());
		} else {
			LOG_FATAL("不正な引数。Indirect引数バッファが不正です。");
		}
	}

	//! @brief      Indirectコンピュートディスパッチ
	void VulkanCommandList::dispatchIndirect(const IndirectCommandParam& param) {
		if (auto buffer = param.argumentBuffer.cast<VulkanBuffer>()) {
			m_commandBuffer.dispatchIndirect(buffer->getNative(), param.argumentOffset);
		} else {
			LOG_FATAL("不正な引数。Indirect引数バッファが不正です。");
		}
	}


	//! @brief      デスクリプタテーブルを設定
	void VulkanCommandList::setDescriptorTables(const Amuse::RHI::SetDescriptorTableParam* params, s32 num) {

		auto pipeline = m_pipeline.cast<VulkanPipelineState>();
		auto computePipeline = m_computePipeline.cast<VulkanComputePipelineState>();
		AMUSE_ASSERT(pipeline != nullptr || computePipeline != nullptr, "先にPipelineStateまたはComputePipelineStateを設定してください");
		auto layout = pipeline ? pipeline->getLayout() : computePipeline->getRootSignature()->getNative();

		for (s32 i = 0; i < num; ++i) {
			auto& param = params[i];

			if (auto impl = param.table.cast<VulkanDescriptorTable>()) {
				if (computePipeline) {
					impl->recordCompute(*m_commandBuffer, layout, param.slot);
				} else {
					impl->record(*m_commandBuffer, layout, param.slot);
				}
			} else {
				LOG_FATAL("不正な引数。デスクリプタテーブルが不正です。");
			}

		}

	}


	//! @brief      ルート定数を設定
	void VulkanCommandList::setRootConstant(const SetRootConstantsParam& param) {

		if (auto pipeline = m_pipeline.cast<VulkanPipelineState>()) {
			if (auto signature = pipeline->getDesc().rootSignature.cast<VulkanRootSignature>()) {
				m_commandBuffer.pushConstants<byte>(signature->getNative(), vk::ShaderStageFlags{} | vk::ShaderStageFlagBits::eAll,param.offset, param.blob);
				return;
			}
		}
		if (auto pipeline = m_computePipeline.cast<VulkanComputePipelineState>()) {
			if (auto signature = pipeline->getRootSignature()) {
				m_commandBuffer.pushConstants<byte>(signature->getNative(), vk::ShaderStageFlags{} | vk::ShaderStageFlagBits::eAll, param.offset, param.blob);
				return;
			}
		}

		AMUSE_ASSERT(m_pipeline || m_computePipeline, "PipelineStateが未設定です。");
	}


	//! @brief      GPUマーカーをプッシュ
	void VulkanCommandList::pushMarker(StringView name) {
		if (m_device.getFeaturesEx().debugMarkerEnabled) m_commandBuffer.beginDebugUtilsLabelEXT(name.data());
	}


	//! @brief      GPUマーカーをポップ
	void VulkanCommandList::popMarker() {
		if (m_device.getFeaturesEx().debugMarkerEnabled) m_commandBuffer.endDebugUtilsLabelEXT();
	}

	void VulkanCommandList::setEventQuery(const Ref<EventQuery>& query) {
		if (query) {
			m_eventQueries.push_back(query);
		}
	}

	//! @brief      GPUタイマークエリを開始
	void VulkanCommandList::beginTimerQuery(const Ref<TimerQuery>& query) {
		if (auto timer = query.cast<VulkanTimerQuery>()) {
			timer->recordBegin(*m_commandBuffer);
		}
	}

	//! @brief      GPUタイマークエリを終了
	void VulkanCommandList::endTimerQuery(const Ref<TimerQuery>& query) {
		if (auto timer = query.cast<VulkanTimerQuery>()) {
			timer->recordEnd(*m_commandBuffer);
		}
	}
	
}
