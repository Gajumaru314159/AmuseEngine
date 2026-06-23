//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Geometry/Bounding.h>
#include <Amuse/RHI/VertexLayout.h>
#include <Amuse/RHI/CommandList.h>
#include <Amuse/RPI/Material/Material.h>

namespace Amuse::RPI {

	struct MeshLodBuffer
	{
		Ref<RHI::VertexLayout> layout;
		Ref<RHI::Buffer> vertices;
		Ref<RHI::Buffer> indices;
	};
	struct MeshLodSource
	{
		Vector<u32>		indices;
		Vector<Vec3>	positions;
		Vector<Vec3>	normals;
		Vector<Vec3>	binormals;
		Vector<Vec3>	tangents;
		Vector<Vec2>	uvs;
		Vector<Color>	colors;
	};

	struct MeshSection
	{
		s32 material = 0;
		s32 baseVertex = 0;
		s32 indexStart = 0;
		s32 indexCount = 0;
	};

	struct MeshLod
	{
		MeshLodBuffer buffer;
		Optional<MeshLodSource> source;
		Vector<MeshSection> sections;
		Bounds bounds;
		bool bKeepSource = false;
	};

	class Mesh : public RefObject
	{
	public:
		using VertexLayout = Amuse::RHI::VertexLayout;

		Bounds bounds;
		Vector<MeshLod> lods;
		Vector<Ref<Material>> materials;
	public:
		static Ref<Mesh> Create();
		static Ref<Mesh> Load(StringView path);
	public:

		void apply();
		void clearSources();

	protected:
		Mesh() = default;
	};

}
