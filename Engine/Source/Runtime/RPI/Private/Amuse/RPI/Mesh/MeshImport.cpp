#include <Amuse/RPI/Mesh/MeshImport.h>
#include <ufbx.h>

namespace Amuse::Detail {

	//! @brief 面のコーナーごとに頂点を展開してUV境界や法線の分割を保持する
	MeshLod ImportMesh(const ufbx_mesh& mesh) {
		MeshLod lod;
		auto& source = lod.source.emplace();
		for (size_t i = 0; i < mesh.num_indices; ++i) {
			auto position = ufbx_get_vertex_vec3(&mesh.vertex_position, i);
			source.positions.emplace_back(position.x, position.y, position.z);
			if (mesh.vertex_normal.exists) {
				auto normal = ufbx_get_vertex_vec3(&mesh.vertex_normal, i);
				source.normals.emplace_back(normal.x, normal.y, normal.z);
			}
			if (mesh.vertex_tangent.exists) {
				auto tangent = ufbx_get_vertex_vec3(&mesh.vertex_tangent, i);
				source.tangents.emplace_back(tangent.x, tangent.y, tangent.z);
			}
			if (mesh.vertex_bitangent.exists) {
				auto bitangent = ufbx_get_vertex_vec3(&mesh.vertex_bitangent, i);
				source.binormals.emplace_back(bitangent.x, bitangent.y, bitangent.z);
			}
			if (mesh.vertex_color.exists) {
				auto color = ufbx_get_vertex_vec4(&mesh.vertex_color, i);
				source.colors.emplace_back(color.x, color.y, color.z, color.w);
			}
			if (mesh.vertex_uv.exists) {
				auto uv = ufbx_get_vertex_vec2(&mesh.vertex_uv, i);
				source.uvs.emplace_back(uv.x, 1 - uv.y);
			}
		}

		Vector<u32> triangles(mesh.max_face_triangles * 3);
		for (const auto& part : mesh.material_parts) {
			if (part.num_triangles == 0) continue;
			auto& section = lod.sections.emplace_back();
			section.indexStart = static_cast<s32>(source.indices.size());
			if (part.index < mesh.materials.count) {
				section.material = static_cast<s32>(mesh.materials[part.index]->typed_id);
			}
			for (auto faceIndex : part.face_indices) {
				auto face = mesh.faces[faceIndex];
				if (face.num_indices < 3) continue;
				auto count = ufbx_triangulate_face(triangles.data(), triangles.size(), &mesh, face);
				source.indices.insert(source.indices.end(), triangles.begin(), triangles.begin() + count * 3);
			}
			section.indexCount = static_cast<s32>(source.indices.size()) - section.indexStart;
		}
		return lod;
	}

}
