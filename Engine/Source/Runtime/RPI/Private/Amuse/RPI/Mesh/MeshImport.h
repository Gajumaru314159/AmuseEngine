#pragma once
#include <Amuse/RPI/Mesh/Mesh.h>

struct ufbx_mesh;

namespace Amuse::Detail {

	//! @brief ufbxのメッシュを三角形化し、頂点属性とマテリアル別セクションを変換する
	MeshLod ImportMesh(const ufbx_mesh& mesh);

}
