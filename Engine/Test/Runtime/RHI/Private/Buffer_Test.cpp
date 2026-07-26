//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <RHITestBase.h>
#include <magic_enum.hpp>
#include <cstring>

using namespace Amuse;

TYPED_TEST(RHITest, Buffer_Create) {

	for (auto type : magic_enum::enum_values<BufferState>()) {

		size_t sizes[] = { 0 ,100,256,512 };
		for (auto size : sizes) {

			for (u32 stride = 0; stride < 256; stride += 64) {

				for (auto flags : magic_enum::enum_values<BufferFlag>()) {

					size_t checkSize = size;

					BufferDesc desc;
					desc.state = type;
					desc.size = size;
					desc.stride = stride;
					desc.flags = flags;
					Ref<Buffer> buffer = Buffer::Create(desc);

					// サイズチェック
					if (!desc.isValid()) {
						ASSERT_FALSE(buffer);
						continue;
					}

					ASSERT_NE(buffer, nullptr);
					EXPECT_EQ(buffer->getDesc().state, type);
					EXPECT_EQ(buffer->getDesc().size, checkSize);
					EXPECT_EQ(buffer->getDesc().stride, stride);
					EXPECT_EQ(buffer->getDesc().flags, BufferFlags(flags));

				}
			}
		}

	}

}

TYPED_TEST(RHITest, Buffer_CreateUtility) {

	// Constant
	{
		BufferDesc desc = BufferDesc::Constant(100);

		Ref<Buffer> buffer = Buffer::Create(desc);
		ASSERT_TRUE(buffer);
	}
	// Constant
	{
		BufferDesc desc = BufferDesc::Constant(100);
		desc.size--;

		Ref<Buffer> buffer = Buffer::Create(desc);
		ASSERT_FALSE(buffer);
	}

	// Vertex
	{
		BufferDesc desc = BufferDesc::Vertex<Vec4>(100);
		Ref<Buffer> buffer = Buffer::Create(desc);
		ASSERT_NE(buffer, nullptr);
		ASSERT_EQ(buffer->getDesc().size, sizeof(Vec4) * 100);
	}

	// Index
	{
		BufferDesc desc = BufferDesc::Index<u16>(100);
		Ref<Buffer> buffer = Buffer::Create(desc);
		ASSERT_NE(buffer, nullptr);
		ASSERT_EQ(buffer->getDesc().size,sizeof(u16)*100);
	}

}

TYPED_TEST(RHITest, Buffer_CreateFactory) {

	// Vertex
	{
		auto buffer = Buffer::CreateVertex<Vec4>("VertexBuffer", 100);
		ASSERT_NE(buffer, nullptr);
		EXPECT_EQ(buffer->getDesc().name, "VertexBuffer");
		EXPECT_EQ(buffer->getDesc().state, BufferState::Vertex);
		EXPECT_EQ(buffer->getDesc().size, sizeof(Vec4) * 100);
		EXPECT_EQ(buffer->getDesc().stride, sizeof(Vec4));
		EXPECT_EQ(buffer->getDesc().flags, BufferFlags(BufferFlag::Vertex));
	}

	// Index
	{
		auto buffer = Buffer::CreateIndex<u16>("IndexBuffer", 100);
		ASSERT_NE(buffer, nullptr);
		EXPECT_EQ(buffer->getDesc().name, "IndexBuffer");
		EXPECT_EQ(buffer->getDesc().state, BufferState::Index);
		EXPECT_EQ(buffer->getDesc().size, sizeof(u16) * 100);
		EXPECT_EQ(buffer->getDesc().stride, sizeof(u16));
		EXPECT_EQ(buffer->getDesc().flags, BufferFlags(BufferFlag::Index));
	}

	// Constant
	{
		auto buffer = Buffer::CreateConstant("ConstantBuffer", 100);
		ASSERT_NE(buffer, nullptr);
		EXPECT_EQ(buffer->getDesc().name, "ConstantBuffer");
		EXPECT_EQ(buffer->getDesc().state, BufferState::Constant);
		EXPECT_EQ(buffer->getDesc().size, 256);
		EXPECT_EQ(buffer->getDesc().stride, 0);
		EXPECT_EQ(buffer->getDesc().flags, BufferFlags(BufferFlag::Constant));
	}

	// ByteAddress
	{
		auto buffer = Buffer::CreateByteAddress("ByteAddressBuffer", 100);
		ASSERT_NE(buffer, nullptr);
		EXPECT_EQ(buffer->getDesc().name, "ByteAddressBuffer");
		EXPECT_EQ(buffer->getDesc().state, BufferState::ShaderResource);
		EXPECT_EQ(buffer->getDesc().size, 112);
		EXPECT_EQ(buffer->getDesc().stride, 0);
		EXPECT_EQ(buffer->getDesc().flags, BufferFlags(BufferFlag::ShaderResource));
	}

	// Structured
	{
		auto buffer = Buffer::CreateStructured<Vec4>("StructuredBuffer", 100);
		ASSERT_NE(buffer, nullptr);
		EXPECT_EQ(buffer->getDesc().name, "StructuredBuffer");
		EXPECT_EQ(buffer->getDesc().state, BufferState::ShaderResource);
		EXPECT_EQ(buffer->getDesc().size, sizeof(Vec4) * 100);
		EXPECT_EQ(buffer->getDesc().stride, sizeof(Vec4));
		EXPECT_EQ(
			buffer->getDesc().flags,
			BufferFlags(BufferFlag::ShaderResource | BufferFlag::UnorderedAccess));
	}

}

TYPED_TEST(RHITest, Buffer_Update) {

	// バッファの作成
	Ref<Buffer> buffer = Buffer::Create(BufferDesc::Vertex<f32>(6));
	ASSERT_NE(buffer, nullptr);

	// バッファの更新
	Array<f32,6> data = {0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f};

	// 直接渡し
	buffer->update(data.size() * sizeof(float), data.data(), 0);

	// 関数渡し
	buffer->update([&](void* ptr) {
		memcpy(ptr, data.data(), data.size() * sizeof(float));
	});

}

TYPED_TEST(RHITest, Buffer_UpdateDirect) {

	auto buffer = Buffer::Create(BufferDesc::ByteAddress(
		sizeof(Array<f32, 6>),
		BufferState::CopySource,
		BufferFlag::CopySource));
	ASSERT_NE(buffer, nullptr);

	Array<f32, 6> data = { 0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f };

	buffer->updateDirect(data.size() * sizeof(float), data.data(), 0);

	auto mapped = buffer->map(CpuAccess::Read);
	ASSERT_NE(mapped, nullptr);
	EXPECT_EQ(std::memcmp(mapped, data.data(), data.size() * sizeof(float)), 0);
	buffer->unmap();

	buffer->updateDirect([&](void* ptr) {
		std::memcpy(ptr, data.data(), data.size() * sizeof(float));
	});

	mapped = buffer->map(CpuAccess::Read);
	ASSERT_NE(mapped, nullptr);
	EXPECT_EQ(std::memcmp(mapped, data.data(), data.size() * sizeof(float)), 0);
	buffer->unmap();

	buffer->updateDirect(data, 0);

}

TYPED_TEST(RHITest, Buffer_ViewCreate) {

	Ref<Buffer> base = Buffer::Create(BufferDesc::Structured<u32>(
		16,
		BufferState::ShaderResource,
		BufferFlag::ShaderResource | BufferFlag::UnorderedAccess));
	ASSERT_NE(base, nullptr);

	BufferViewDesc srvDesc;
	srvDesc.base = base;
	srvDesc.type = BufferViewType::StructuredBuffer;
	Ref<Buffer> srv = Buffer::Create(srvDesc);
	ASSERT_NE(srv, nullptr);
	EXPECT_EQ(srv->getDesc().size, base->getDesc().size);

	BufferViewDesc uavDesc;
	uavDesc.base = base;
	uavDesc.type = BufferViewType::RWStructuredBuffer;
	Ref<Buffer> uav = Buffer::Create(uavDesc);
	ASSERT_NE(uav, nullptr);
	EXPECT_EQ(uav->getDesc().size, base->getDesc().size);
}

TYPED_TEST(RHITest, DescriptorTable_SetBufferResourceReturnsSuccess) {

	Ref<DescriptorLayout> layout = DescriptorLayout::Create({
		Binding::ByteAddressBuffer(0),
		Binding::RWByteAddressBuffer(1),
		Binding::Buffer(2),
		Binding::RWBuffer(3),
	});
	ASSERT_NE(layout, nullptr);

	Ref<DescriptorTable> table = DescriptorTable::Create({ layout });
	ASSERT_NE(table, nullptr);

	Ref<Buffer> buffer = Buffer::Create(BufferDesc::ByteAddress(
		64,
		BufferState::UnorderedAccess,
		BufferFlag::ShaderResource | BufferFlag::UnorderedAccess));
	ASSERT_NE(buffer, nullptr);

	BufferViewDesc typedSrvDesc;
	typedSrvDesc.base = buffer;
	typedSrvDesc.type = BufferViewType::Buffer;
	typedSrvDesc.format = TextureFormat::R32;
	Ref<Buffer> typedSrv = Buffer::Create(typedSrvDesc);
	ASSERT_NE(typedSrv, nullptr);

	BufferViewDesc typedUavDesc;
	typedUavDesc.base = buffer;
	typedUavDesc.type = BufferViewType::RWBuffer;
	typedUavDesc.format = TextureFormat::R32;
	Ref<Buffer> typedUav = Buffer::Create(typedUavDesc);
	ASSERT_NE(typedUav, nullptr);

	EXPECT_TRUE(table->setResource(0, buffer));
	EXPECT_TRUE(table->setResource(1, buffer));
	EXPECT_TRUE(table->setResource(2, typedSrv));
	EXPECT_TRUE(table->setResource(3, typedUav));
}

TYPED_TEST(RHITest, Buffer_WriteIndirectArguments) {

	ASSERT_NE(Device::Get(), nullptr);

	const auto drawStride = Device::Get()->getDrawIndirectArgumentSize();
	const auto indexedStride = Device::Get()->getDrawIndexedIndirectArgumentSize();
	const auto dispatchStride = Device::Get()->getDispatchIndirectArgumentSize();
	const auto drawOffset = 0u;
	const auto indexedOffset = drawStride * 2;
	const auto dispatchOffset = indexedOffset + indexedStride * 2;

	auto buffer = Buffer::Create(BufferDesc::ByteAddress(
		dispatchOffset + dispatchStride * 2,
		BufferState::CopySource,
		BufferFlag::CopySource));
	ASSERT_NE(buffer, nullptr);

	DrawParam draws[] = {
		DrawParam{ .startVertex = 3, .vertexCount = 11, .startInstance = 5, .instanceCount = 7 },
		DrawParam{ .startVertex = 13, .vertexCount = 17, .startInstance = 19, .instanceCount = 23 },
	};
	DrawIndexedParam indexed[] = {
		DrawIndexedParam{ .startVertex = 29, .startIndex = 31, .indexCount = 37, .startInstance = 41, .instanceCount = 43 },
		DrawIndexedParam{ .startVertex = 47, .startIndex = 53, .indexCount = 59, .startInstance = 61, .instanceCount = 67 },
	};
	DispatchParam dispatches[] = {
		DispatchParam{ 71, 73, 79 },
		DispatchParam{ 83, 89, 97 },
	};

	buffer->writeDrawParams(draws, drawOffset);
	buffer->writeDrawIndexedParams(indexed, indexedOffset);
	buffer->writeDispatchParams(dispatches, dispatchOffset);

	const auto* bytes = static_cast<const u8*>(Device::Get()->mapBuffer(buffer, CpuAccess::Read));
	ASSERT_NE(bytes, nullptr);

	const auto* draw0 = reinterpret_cast<const u32*>(bytes + drawOffset);
	EXPECT_EQ(draw0[0], draws[0].vertexCount);
	EXPECT_EQ(draw0[1], draws[0].instanceCount);
	EXPECT_EQ(draw0[2], draws[0].startVertex);
	EXPECT_EQ(draw0[3], draws[0].startInstance);
	const auto* draw1 = reinterpret_cast<const u32*>(bytes + drawOffset + drawStride);
	EXPECT_EQ(draw1[0], draws[1].vertexCount);
	EXPECT_EQ(draw1[1], draws[1].instanceCount);
	EXPECT_EQ(draw1[2], draws[1].startVertex);
	EXPECT_EQ(draw1[3], draws[1].startInstance);

	const auto* indexed0 = reinterpret_cast<const u32*>(bytes + indexedOffset);
	EXPECT_EQ(indexed0[0], indexed[0].indexCount);
	EXPECT_EQ(indexed0[1], indexed[0].instanceCount);
	EXPECT_EQ(indexed0[2], indexed[0].startIndex);
	EXPECT_EQ(static_cast<const s32*>(static_cast<const void*>(indexed0))[3], static_cast<s32>(indexed[0].startVertex));
	EXPECT_EQ(indexed0[4], indexed[0].startInstance);
	const auto* indexed1 = reinterpret_cast<const u32*>(bytes + indexedOffset + indexedStride);
	EXPECT_EQ(indexed1[0], indexed[1].indexCount);
	EXPECT_EQ(indexed1[1], indexed[1].instanceCount);
	EXPECT_EQ(indexed1[2], indexed[1].startIndex);
	EXPECT_EQ(static_cast<const s32*>(static_cast<const void*>(indexed1))[3], static_cast<s32>(indexed[1].startVertex));
	EXPECT_EQ(indexed1[4], indexed[1].startInstance);

	const auto* dispatch0 = reinterpret_cast<const u32*>(bytes + dispatchOffset);
	EXPECT_EQ(dispatch0[0], dispatches[0].groupX);
	EXPECT_EQ(dispatch0[1], dispatches[0].groupY);
	EXPECT_EQ(dispatch0[2], dispatches[0].groupZ);
	const auto* dispatch1 = reinterpret_cast<const u32*>(bytes + dispatchOffset + dispatchStride);
	EXPECT_EQ(dispatch1[0], dispatches[1].groupX);
	EXPECT_EQ(dispatch1[1], dispatches[1].groupY);
	EXPECT_EQ(dispatch1[2], dispatches[1].groupZ);

	Device::Get()->unmapBuffer(buffer);
}

TYPED_TEST(RHITest, Buffer_WriteIndirectArgumentSingles) {

	ASSERT_NE(Device::Get(), nullptr);

	const auto drawStride = Device::Get()->getDrawIndirectArgumentSize();
	const auto indexedStride = Device::Get()->getDrawIndexedIndirectArgumentSize();
	const auto dispatchStride = Device::Get()->getDispatchIndirectArgumentSize();
	const auto indexedOffset = drawStride;
	const auto dispatchOffset = indexedOffset + indexedStride;

	auto buffer = Buffer::Create(BufferDesc::ByteAddress(
		dispatchOffset + dispatchStride,
		BufferState::CopySource,
		BufferFlag::CopySource));
	ASSERT_NE(buffer, nullptr);

	const DrawParam draw{ .startVertex = 5, .vertexCount = 7, .startInstance = 11, .instanceCount = 13 };
	const DrawIndexedParam indexed{ .startVertex = 17, .startIndex = 19, .indexCount = 23, .startInstance = 29, .instanceCount = 31 };
	const DispatchParam dispatch{ 37, 41, 43 };

	buffer->writeDrawParam(draw, 0);
	buffer->writeDrawIndexedParam(indexed, indexedOffset);
	buffer->writeDispatchParam(dispatch, dispatchOffset);

	const auto* bytes = static_cast<const u8*>(Device::Get()->mapBuffer(buffer, CpuAccess::Read));
	ASSERT_NE(bytes, nullptr);

	const auto* drawData = reinterpret_cast<const u32*>(bytes);
	EXPECT_EQ(drawData[0], draw.vertexCount);
	EXPECT_EQ(drawData[1], draw.instanceCount);
	EXPECT_EQ(drawData[2], draw.startVertex);
	EXPECT_EQ(drawData[3], draw.startInstance);

	const auto* indexedData = reinterpret_cast<const u32*>(bytes + indexedOffset);
	EXPECT_EQ(indexedData[0], indexed.indexCount);
	EXPECT_EQ(indexedData[1], indexed.instanceCount);
	EXPECT_EQ(indexedData[2], indexed.startIndex);
	EXPECT_EQ(static_cast<const s32*>(static_cast<const void*>(indexedData))[3], static_cast<s32>(indexed.startVertex));
	EXPECT_EQ(indexedData[4], indexed.startInstance);

	const auto* dispatchData = reinterpret_cast<const u32*>(bytes + dispatchOffset);
	EXPECT_EQ(dispatchData[0], dispatch.groupX);
	EXPECT_EQ(dispatchData[1], dispatch.groupY);
	EXPECT_EQ(dispatchData[2], dispatch.groupZ);

	Device::Get()->unmapBuffer(buffer);
}
