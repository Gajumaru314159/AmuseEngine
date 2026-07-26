//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <RPITest.h>
#include <Amuse/RPI/Mesh/Mesh.h>

using namespace Amuse;

TYPED_TEST(BindfullRPITest, MeshCreate) {

	Ref<Mesh> mesh = Mesh::Create();

	EXPECT_TRUE(mesh->lods.empty());
	EXPECT_TRUE(mesh->materials.empty());

	for (s32 i = 0; i < 3; ++i) {
		auto& lod = mesh->lods.emplace_back();
		lod.source.emplace();
		lod.source->positions = { Vec3(0,0,0), Vec3(1,0,0), Vec3(0,1,0) };
		lod.source->indices = { 0,1,2 };
	}

	mesh->apply();

	EXPECT_EQ(mesh->lods.size(), 3);

	for (s32 i = 0; i < 3; ++i) {
		auto& lod = mesh->lods.at(i);
		EXPECT_TRUE(lod.buffer.layout != nullptr);
		EXPECT_TRUE(lod.buffer.vertices != nullptr);
		EXPECT_TRUE(lod.buffer.indices != nullptr);
	}

}
