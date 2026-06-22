//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <RHITestBase.h>
#include <magic_enum.hpp>
#include <cstring>

namespace {
    void ExecuteAndWait(const Ref<CommandList>& commandList) {
        Ref<CommandList> commandLists[] = { commandList };
        Device::Get()->executeCommandLists(commandLists);
        Device::Get()->waitForIdle();
    }
}

TYPED_TEST(RHITest, CommandList_Create) {

    for (auto type : magic_enum::enum_values<CommandListType>()) {

        CommandListDesc desc;
        desc.type = type;

        auto commandList = CommandList::Create(desc);

        ASSERT_NE(commandList, nullptr);

    }

}

TYPED_TEST(RHITest, CommandList_GraphicCommands) {

    RenderTextureDesc rtDesc;
	rtDesc.size = { 1280, 720 };
    auto target = RenderTexture::Create(rtDesc);


    CommandListDesc desc;
    desc.type = CommandListType::Graphic;

    auto commandList = CommandList::Create(desc);

    ASSERT_NE(commandList, nullptr);

    commandList->begin();

    Viewport viewport;
    viewport.left = 0;
    viewport.top = 0;
    viewport.right = 1280;
    viewport.bottom = 720;
    commandList->setViewport(&viewport, 1);

    IntRect scissor;
    scissor.left = 0;
    scissor.top = 0;
    scissor.right = 1280;
    scissor.bottom = 720;
    commandList->setScissorRect(&scissor, 1);

	//commandList->setRenderTarget(target);

    //commandList->clearColors();

    commandList->pushMarker("Marker");
    commandList->popMarker();


    commandList->end();
    ExecuteAndWait(commandList);
}

TYPED_TEST(RHITest, CommandList_ExecuteCommandLists) {

    CommandListDesc desc;
    desc.type = CommandListType::Graphic;

    auto commandList = CommandList::Create(desc);
    ASSERT_NE(commandList, nullptr);

    commandList->begin();
    commandList->pushMarker("ExecuteCommandLists");
    commandList->popMarker();
    commandList->end();

    ASSERT_NE(Device::Get(), nullptr);
    Ref<CommandList> commandLists[] = { commandList };
    const auto fenceValue = Device::Get()->executeCommandLists(commandLists);
    EXPECT_GT(fenceValue, 0);

    Device::Get()->waitForIdle();
}

TYPED_TEST(RHITest, Fence_Create) {

    FenceDesc desc;
    desc.name = "Fence_Create";
    desc.initialValue = 3;

    auto fence = Fence::Create(desc);
    ASSERT_NE(fence, nullptr);
    EXPECT_GE(fence->getCompletedValue(), 3);
    fence->wait(3);
}

TYPED_TEST(RHITest, Capability_Query) {

    ASSERT_NE(Device::Get(), nullptr);

    const auto features = Device::Get()->getFeatures();
    EXPECT_TRUE(features.computeQueue);
    EXPECT_TRUE(features.copyQueue);
    EXPECT_TRUE(features.indirectDraw);
    EXPECT_TRUE(features.indirectDispatch);
    EXPECT_TRUE(Device::Get()->supports(TextureFormat::RGBA8, FormatSupport::RenderTarget));
    EXPECT_TRUE(Device::Get()->supports(TextureFormat::RGBA8, FormatSupport::Texture));
    EXPECT_FALSE(Device::Get()->supports(TextureFormat::Unknown, FormatSupport::Texture));
    EXPECT_EQ(
        Device::Get()->supports(TextureFormat::D32, FormatSupport::DepthStencil),
        Device::Get()->supportsForRenderTexture(TextureFormat::D32));
}

TYPED_TEST(RHITest, CommandList_ManualBarriers) {

    CommandListDesc desc;
    desc.type = CommandListType::Graphic;

    auto commandList = CommandList::Create(desc);
    ASSERT_NE(commandList, nullptr);

    auto buffer = Buffer::Create(BufferDesc::ByteAddress(
        16,
        BufferState::Common,
        BufferFlag::CopySource | BufferFlag::CopyDest | BufferFlag::UnorderedAccess));
    ASSERT_NE(buffer, nullptr);

    commandList->begin();
    commandList->setEnableAutomaticBarriers(false);
    commandList->addBufferBarrier(BufferBarrier{
        buffer,
        BufferState::Common,
        BufferState::CopyDest
    });
    commandList->addUAVBarrier(buffer);
    commandList->commitBarriers();
    commandList->end();

    ASSERT_NE(Device::Get(), nullptr);
    Ref<CommandList> commandLists[] = { commandList };
    Device::Get()->executeCommandLists(commandLists);
    Device::Get()->waitForIdle();
}

TYPED_TEST(RHITest, CommandList_BufferCopyReadback) {

    constexpr u32 sourceValues[] = { 3, 1, 4, 1 };
    constexpr u64 bufferSize = sizeof(sourceValues);

    auto upload = Buffer::Create(BufferDesc::ByteAddress(
        bufferSize,
        BufferState::CopySource,
        BufferFlag::CopySource));
    ASSERT_NE(upload, nullptr);

    auto readback = Buffer::Create(BufferDesc::ByteAddress(
        bufferSize,
        BufferState::CopyDest,
        BufferFlag::CopyDest));
    ASSERT_NE(readback, nullptr);

    auto* uploadData = static_cast<u32*>(Device::Get()->mapBuffer(upload, CpuAccess::Write));
    ASSERT_NE(uploadData, nullptr);
    std::memcpy(uploadData, sourceValues, sizeof(sourceValues));
    Device::Get()->unmapBuffer(upload);

    CommandListDesc desc;
    desc.type = CommandListType::Graphic;
    auto commandList = CommandList::Create(desc);
    ASSERT_NE(commandList, nullptr);

    commandList->begin();
    commandList->copyBuffer(BufferCopyRegion{
        upload,
        readback,
        0,
        0,
        bufferSize
    });
    commandList->end();

    Ref<CommandList> commandLists[] = { commandList };
    Device::Get()->executeCommandLists(commandLists);
    Device::Get()->waitForIdle();

    auto* readbackData = static_cast<const u32*>(Device::Get()->mapBuffer(readback, CpuAccess::Read));
    ASSERT_NE(readbackData, nullptr);
    EXPECT_EQ(std::memcmp(readbackData, sourceValues, sizeof(sourceValues)), 0);
    Device::Get()->unmapBuffer(readback);
}

TYPED_TEST(RHITest, CommandList_BufferCopyOffsetReadback) {

    constexpr u32 sourceValues[] = { 10, 20, 30, 40, 50, 60 };
    constexpr u64 bufferSize = sizeof(sourceValues);
    constexpr u64 copySize = sizeof(u32) * 3;
    constexpr u64 sourceOffset = sizeof(u32) * 2;
    constexpr u64 targetOffset = sizeof(u32);

    auto source = Buffer::Create(BufferDesc::ByteAddress(
        bufferSize,
        BufferState::CopySource,
        BufferFlag::CopySource));
    ASSERT_NE(source, nullptr);
    auto* sourceData = static_cast<u32*>(Device::Get()->mapBuffer(source, CpuAccess::Write));
    ASSERT_NE(sourceData, nullptr);
    std::memcpy(sourceData, sourceValues, sizeof(sourceValues));
    Device::Get()->unmapBuffer(source);

    auto target = Buffer::Create(BufferDesc::ByteAddress(
        bufferSize,
        BufferState::CopyDest,
        BufferFlag::CopyDest));
    ASSERT_NE(target, nullptr);

    CommandListDesc desc;
    desc.type = CommandListType::Graphic;
    auto commandList = CommandList::Create(desc);
    ASSERT_NE(commandList, nullptr);

    commandList->begin();
    commandList->copyBuffer(BufferCopyRegion{
        source,
        target,
        sourceOffset,
        targetOffset,
        copySize
    });
    commandList->end();
    ExecuteAndWait(commandList);

    const auto* values = static_cast<const u32*>(Device::Get()->mapBuffer(target, CpuAccess::Read));
    ASSERT_NE(values, nullptr);
    EXPECT_EQ(values[1], 30);
    EXPECT_EQ(values[2], 40);
    EXPECT_EQ(values[3], 50);
    Device::Get()->unmapBuffer(target);
}

TYPED_TEST(RHITest, EventQuery_SignalAndWait) {

    auto query = EventQuery::Create();
    ASSERT_NE(query, nullptr);

    CommandListDesc commandListDesc;
    commandListDesc.type = CommandListType::Graphic;
    auto commandList = CommandList::Create(commandListDesc);
    ASSERT_NE(commandList, nullptr);

    commandList->begin();
    commandList->setEventQuery(query);
    commandList->end();

    Ref<CommandList> commandLists[] = { commandList };
    Device::Get()->executeCommandLists(commandLists);

    Device::Get()->waitEventQuery(query);
    EXPECT_TRUE(Device::Get()->pollEventQuery(query));
}

TYPED_TEST(RHITest, CommandList_WriteTextureReadbackBuffer) {

    constexpr IntColor sourcePixels[] = {
        IntColor(255, 0, 0, 255),
        IntColor(0, 255, 0, 255),
        IntColor(0, 0, 255, 255),
        IntColor(255, 255, 255, 255),
    };
    constexpr u32 textureWidth = 2;
    constexpr u32 textureHeight = 2;
    constexpr u32 sourceRowPitch = sizeof(IntColor) * textureWidth;
    constexpr u32 readbackRowPitch = 256;

    TextureDesc textureDesc;
    textureDesc.name = "WriteTextureReadbackBuffer";
    textureDesc.type = TextureType::Texture2D;
    textureDesc.format = TextureFormat::RGBA8;
    textureDesc.size = { textureWidth, textureHeight, 1 };
    textureDesc.mipLevels = 1;
    textureDesc.flags = TextureFlag::ShaderResource;

    auto texture = Texture::Create(textureDesc);
    ASSERT_NE(texture, nullptr);

    auto readback = Buffer::Create(BufferDesc::ByteAddress(
        readbackRowPitch * textureHeight,
        BufferState::CopyDest,
        BufferFlag::CopyDest));
    ASSERT_NE(readback, nullptr);

    TextureSubresource subresource{};
    subresource.arrayIndex = 0;
    subresource.faceIndex = 0;
    subresource.mipLevel = 0;

    CommandListDesc commandListDesc;
    commandListDesc.type = CommandListType::Graphic;
    auto commandList = CommandList::Create(commandListDesc);
    ASSERT_NE(commandList, nullptr);

    TextureBarrier toCopyDest;
    toCopyDest.texture = texture;
    toCopyDest.after = TextureState::CopyDest;

    TextureBarrier toCopySource;
    toCopySource.texture = texture;
    toCopySource.before = TextureState::CopyDest;
    toCopySource.after = TextureState::CopySource;

    commandList->begin();
    commandList->addTextureBarrier(toCopyDest);
    commandList->commitBarriers();
    commandList->writeTexture(texture, subresource, BlobView(sourcePixels), sourceRowPitch);
    commandList->addTextureBarrier(toCopySource);
    commandList->commitBarriers();
    commandList->copyTextureToBuffer(BufferTextureCopyRegion{
        readback,
        texture,
        0,
        readbackRowPitch,
        readbackRowPitch * textureHeight,
        subresource,
        {},
        { textureWidth, textureHeight, 1 }
    });
    commandList->end();

    Ref<CommandList> commandLists[] = { commandList };
    Device::Get()->executeCommandLists(commandLists);
    Device::Get()->waitForIdle();

    const auto* readbackBytes = static_cast<const u8*>(Device::Get()->mapBuffer(readback, CpuAccess::Read));
    ASSERT_NE(readbackBytes, nullptr);
    EXPECT_EQ(std::memcmp(readbackBytes, sourcePixels, sourceRowPitch), 0);
    EXPECT_EQ(std::memcmp(readbackBytes + readbackRowPitch, sourcePixels + textureWidth, sourceRowPitch), 0);
    Device::Get()->unmapBuffer(readback);
}

TYPED_TEST(RHITest, CommandList_ClearBufferUIntReadback) {

    constexpr u32 clearValue = 0x12345678;
    constexpr u64 bufferSize = sizeof(u32) * 4;

    auto buffer = Buffer::Create(BufferDesc::ByteAddress(
        bufferSize,
        BufferState::UnorderedAccess,
        BufferFlag::UnorderedAccess | BufferFlag::CopySource));
    ASSERT_NE(buffer, nullptr);

    auto readback = Buffer::Create(BufferDesc::ByteAddress(
        bufferSize,
        BufferState::CopyDest,
        BufferFlag::CopyDest));
    ASSERT_NE(readback, nullptr);

    CommandListDesc commandListDesc;
    commandListDesc.type = CommandListType::Graphic;
    auto commandList = CommandList::Create(commandListDesc);
    ASSERT_NE(commandList, nullptr);

    commandList->begin();
    commandList->clearBufferUInt(buffer, clearValue);
    commandList->addBufferBarrier(BufferBarrier{
        buffer,
        BufferState::UnorderedAccess,
        BufferState::CopySource
    });
    commandList->commitBarriers();
    commandList->copyBuffer(BufferCopyRegion{
        buffer,
        readback,
        0,
        0,
        bufferSize
    });
    commandList->end();

    Ref<CommandList> commandLists[] = { commandList };
    Device::Get()->executeCommandLists(commandLists);
    Device::Get()->waitForIdle();

    const auto* values = static_cast<const u32*>(Device::Get()->mapBuffer(readback, CpuAccess::Read));
    ASSERT_NE(values, nullptr);
    EXPECT_EQ(values[0], clearValue);
    EXPECT_EQ(values[1], clearValue);
    EXPECT_EQ(values[2], clearValue);
    EXPECT_EQ(values[3], clearValue);
    Device::Get()->unmapBuffer(readback);
}

TYPED_TEST(RHITest, CommandList_ComputeCommands) {

    auto code = R"(
        [numthreads(1, 1, 1)]
        void CS_Main(uint3 DTid : SV_DispatchThreadID) {
        }
    )";

    Ref<RootSignature> signature = RootSignature::Create({});
    ASSERT_NE(signature, nullptr);

    ComputePipelineStateDesc pipelineDesc;
    pipelineDesc.rootSignature = signature;
    pipelineDesc.cs = Shader::CompileCS(code);
    ASSERT_NE(pipelineDesc.cs, nullptr);

    auto pipeline = ComputePipelineState::Create(pipelineDesc);
    ASSERT_NE(pipeline, nullptr);

    CommandListDesc desc;
    desc.type = CommandListType::Graphic;
    auto commandList = CommandList::Create(desc);
    ASSERT_NE(commandList, nullptr);

    commandList->begin();
    commandList->setComputePipelineState(pipeline);
    commandList->dispatch(DispatchParam{ 1, 1, 1 });
    commandList->end();
    ExecuteAndWait(commandList);

    auto indirectBuffer = Buffer::Create(BufferDesc::ByteAddress(
        Device::Get()->getDispatchIndirectArgumentSize(),
        BufferState::IndirectArgument,
        BufferFlag::IndirectArgument));
    ASSERT_NE(indirectBuffer, nullptr);
    indirectBuffer->writeDispatchParam(DispatchParam{ 1, 1, 1 });
    Device::Get()->update();

    auto indirectCommandList = CommandList::Create(desc);
    ASSERT_NE(indirectCommandList, nullptr);

    indirectCommandList->begin();
    indirectCommandList->setComputePipelineState(pipeline);
    indirectCommandList->dispatchIndirect(IndirectCommandParam{ indirectBuffer });
    indirectCommandList->end();
    ExecuteAndWait(indirectCommandList);

    ASSERT_NE(Device::Get(), nullptr);
    Device::Get()->update();
}

TYPED_TEST(RHITest, CommandList_TimerQuery) {

    CommandListDesc desc;
    desc.type = CommandListType::Graphic;
    auto commandList = CommandList::Create(desc);
    ASSERT_NE(commandList, nullptr);

    auto query = TimerQuery::Create();
    ASSERT_NE(query, nullptr);

    commandList->begin();
    commandList->beginTimerQuery(query);
    commandList->pushMarker("TimerQuery");
    commandList->popMarker();
    commandList->endTimerQuery(query);
    commandList->end();
    ExecuteAndWait(commandList);

    ASSERT_NE(Device::Get(), nullptr);
    Device::Get()->update();

    EXPECT_GE(Device::Get()->getTimerQueryTime(query), 0.0f);
}
