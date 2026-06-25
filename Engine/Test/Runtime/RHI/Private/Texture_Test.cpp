//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <RHITestBase.h>
#include <magic_enum.hpp>

using namespace Amuse;
using namespace Amuse;

TYPED_TEST(RHITest, Texture_Type) {

	// タイプ
	for (auto type : magic_enum::enum_values<TextureType>()) {

		TextureDesc desc;
		desc.type = type;
		if (type == TextureType::Texture1D) desc.size = Size{ 100 };
		if (type == TextureType::Texture2D) desc.size = Size{ 100,100 };
		if (type == TextureType::Texture3D) desc.size = Size{ 100,100,100 };
		if (type == TextureType::Cube) desc.size = Size{ 100,100 };

		auto texture = Texture::Create(desc);

		if (!desc.isValid()) {
			ASSERT_EQ(texture, nullptr);
			continue;
		}

		ASSERT_NE(texture, nullptr);

	}

}
TYPED_TEST(RHITest, Texture_Format) {

	// フォーマット
	for (auto format : magic_enum::enum_values<TextureFormat>()) {

		TextureDesc desc;
		desc.format = format;
		desc.size = { 100, 100 };

		auto texture = Texture::Create(desc);

		if (!desc.isValid()) {
			ASSERT_EQ(texture, nullptr);
			continue;
		}

		if (!texture)CallBreakPoint();
		ASSERT_NE(texture, nullptr);

	}

}

TYPED_TEST(RHITest, Texture_DepthStencilShaderResourceValidation) {

	TextureFormat formats[] = {
		TextureFormat::D24S8,
		TextureFormat::D32S8,
	};

	for (auto format : formats) {
		TextureDesc desc;
		desc.name = Format("DepthStencilTexture {}", magic_enum::enum_name(format));
		desc.format = format;
		desc.size = { 64, 64 };
		desc.flags = TextureFlag::ShaderResource;

		EXPECT_EQ(desc.isValid(), Texture::Supports(format, desc.type));
	}

}
TYPED_TEST(RHITest, Texture_Size) {

	// 不正サイズ
	{
		TextureDesc desc;
		desc.size = { -100, -100 };

		auto texture = Texture::Create(desc);

		ASSERT_EQ(texture, nullptr);
	}

}
TYPED_TEST(RHITest, Texture_Array) {

	// 配列
	{
		TextureDesc desc;
		desc.size = { -100, -100 };
		desc.arrayNum = 4;

		auto texture = Texture::Create(desc);

		ASSERT_EQ(texture, nullptr);
	}

	for (auto type : magic_enum::enum_values<TextureType>())
	{
		for (s32 i = 0; i < 10; ++i) {

			TextureDesc desc;
			desc.arrayNum = i;
			desc.type = type;

			if (type == TextureType::Texture1D) desc.size = Size{ 128 };
			if (type == TextureType::Texture2D) desc.size = Size{ 128,128 };
			if (type == TextureType::Cube) desc.size = Size{ 128,128 };
			if (type == TextureType::Texture3D) continue;

			auto texture = Texture::Create(desc);

			ASSERT_NE(texture, nullptr);

		}
	}

}

TYPED_TEST(RHITest, Texture_View) {

	TextureDesc desc;
	desc.name = "Texture_View_Base";
	desc.type = TextureType::Texture2D;
	desc.size = { 64, 64 };
	desc.arrayNum = 4;
	desc.mipLevels = 4;
	desc.flags = TextureFlag::ShaderResource | TextureFlag::UnorderedAccess;

	auto texture = Texture::Create(desc);
	ASSERT_NE(texture, nullptr);

	TextureViewDesc srvDesc;
	srvDesc.base = texture;
	srvDesc.type = TextureViewType::Texture;
	srvDesc.firstMip = 1;
	srvDesc.mipLevels = 2;
	srvDesc.firstArray = 1;
	srvDesc.arrayCount = 2;

	auto srv = Texture::Create(srvDesc);
	ASSERT_NE(srv, nullptr);
	EXPECT_EQ(srv->desc().format, texture->desc().format);
	EXPECT_EQ(srv->desc().size, texture->desc().size);

	TextureViewDesc uavDesc;
	uavDesc.base = texture;
	uavDesc.type = TextureViewType::RWTexture;
	uavDesc.firstMip = 2;
	uavDesc.firstArray = 3;
	uavDesc.arrayCount = 1;

	auto uav = Texture::Create(uavDesc);
	ASSERT_NE(uav, nullptr);
	EXPECT_EQ(uav->desc().format, texture->desc().format);
	EXPECT_EQ(uav->desc().size, texture->desc().size);
}

TYPED_TEST(RHITest, Texture_ViewDefaultRange) {

	TextureDesc desc;
	desc.name = "Texture_View_DefaultRange_Base";
	desc.type = TextureType::Texture2D;
	desc.size = { 32, 32 };
	desc.arrayNum = 2;
	desc.mipLevels = 3;
	desc.flags = TextureFlag::ShaderResource;

	auto texture = Texture::Create(desc);
	ASSERT_NE(texture, nullptr);

	TextureViewDesc viewDesc;
	viewDesc.base = texture;

	auto view = Texture::Create(viewDesc);
	ASSERT_NE(view, nullptr);
	EXPECT_EQ(view->desc().type, texture->desc().type);
	EXPECT_EQ(view->desc().arrayNum, texture->desc().arrayNum);
	EXPECT_EQ(view->desc().mipLevels, texture->desc().mipLevels);
}
