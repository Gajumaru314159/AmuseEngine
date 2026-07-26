//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/RPI/Mesh/Mesh.h>
#include <Amuse/RPI/Material/MaterialManager.h>
#include <Amuse/RPI/Material/Material.h>
#include <Amuse/RHI/CommandList.h>
#include <Amuse/RHI/Buffer.h>

#include <Amuse/Core/String/StringEncoder.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


namespace Amuse {

	Ref<Mesh> Mesh::Load(StringView path) {

		String directory = Path::Parent(path);
		std::string pathStr(path.data());

		Assimp::Importer importer;
		importer.ReadFile(pathStr, 0);
		auto scene = importer.ApplyPostProcessing(aiProcess_Triangulate);

		if (scene == nullptr) {
			LOG_ERROR("Failed to load {}", path);
			return nullptr;
		}

		if (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
			LOG_WARNING("Assimp scene is incomplete [{}]",path);
		}


		// テクスチャ
		Map<String, Ref<Texture>, std::less<>> textures;
		Vector<Ref<Material>> materials;


		// マテリアル
		for (auto m : Span<aiMaterial*>(scene->mMaterials, scene->mNumMaterials)) {

			String name(m->GetName().C_Str());

			// マテリアル生成
			Ref<Material> material = [&] {
				MaterialShaderDesc desc;
				desc.name = path;
				desc.textures = { "Main", "Normal", "Params" };
				desc.matrices = { "Matrix" };
				desc.vectors = { "Color" };

				{
					auto code = File::ReadAllText("Assets/Shader/GraphicTest.hlsl");
					AMUSE_ASSERT(code, "ファイル読み込み失敗");

					MaterialPass& pass = desc.passes["Opaque"];
					auto& shaders = pass.qualities.emplace_back();

					shaders.depthStencil.depth.enable = true;
					shaders.depthStencil.depth.func = ComparisonFunc::GreaterEqual;
					shaders.colors = { TextureFormat::RGBA8 ,TextureFormat::RGBA8,TextureFormat::RGBA8 };	// Shaderに情報を持たせたい
					shaders.depth = TextureFormat::D32;
					shaders.vs = Shader::CompileVS(code.value());
					shaders.ps = Shader::CompilePS(code.value());
					shaders.inputLayout = {
						{Semantic::Position,ElementType::Float,4},
						{Semantic::Normal,ElementType::Float,3},
						{Semantic::Tangent,ElementType::Float,3},
						{Semantic::TexCoord,ElementType::Float,2},
					};
				}
				{
					auto code = File::ReadAllText("Assets/Shader/EarlyZ.hlsl");
					AMUSE_ASSERT(code, "ファイル読み込み失敗");

					MaterialPass& pass = desc.passes["EarlyZ"];
					auto& shaders = pass.qualities.emplace_back();

					shaders.depthStencil.depth.enable = true;
					shaders.depth = TextureFormat::D32;
					shaders.vs = Shader::CompileVS(code.value());
					shaders.ps = Shader::CompilePS(code.value());
					shaders.inputLayout = {
						{Semantic::Position,ElementType::Float,4},
					};
				}

				return Material::Create(desc);
				}();

			material->setMatrix("Matrix", Matrix::Identity);

			// テクスチャ読み込み
			{
				Ref<Texture> texture;
				aiString texturePath;
				if (texturePath.length == 0)m->Get(AI_MATKEY_TEXTURE_DIFFUSE(0), texturePath);
				if (texturePath.length == 0)m->Get(AI_MATKEY_TEXTURE_EMISSIVE(0), texturePath);
				if (texturePath.length) {
					String fullPath = texturePath.C_Str();
					if (Path::IsRelative(fullPath)) {
						fullPath = Path::Combine(directory, texturePath.C_Str());
					}
					if (File::Exists(fullPath)) {
						texture = textures[fullPath] = Texture::Load(fullPath);
					}
				}
				if (!texture) texture = Texture::White();
				material->setTexture("Main", texture);
			}
			{
				Ref<Texture> texture;
				aiString texturePath;
				if (texturePath.length == 0)m->Get(AI_MATKEY_TEXTURE_NORMALS(0), texturePath);
				if (texturePath.length) {
					String fullPath = texturePath.C_Str();
					if (Path::IsRelative(fullPath)) {
						fullPath = Path::Combine(directory, texturePath.C_Str());
					}
					if (File::Exists(fullPath)) {
						texture = textures[fullPath] = Texture::Load(fullPath);
					}
				}
				if (!texture) texture = Texture::White();
				material->setTexture("Normal", texture);
			}
			{
				Ref<Texture> texture;
				if (!texture) texture = Texture::White();
				material->setTexture("Params", Texture::Black());
			}

			Color color = Color::White;
			if (aiColor4D c; m->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
				color = Color(c.r, c.g, c.b, c.a);
			}
			material->setVector("Color", color);

			Map<String, aiColor4D> colors;
			for (auto p : Span<aiMaterialProperty*>(m->mProperties, m->mNumProperties)) {
				if (p->mType == aiPTI_Float) {
					aiColor4D c;
					if (m->Get(p->mKey.C_Str(), 0, 0, c) == AI_SUCCESS) {
						colors[String(p->mKey.C_Str())] = c;
					}
				}
			}

			materials.push_back(material);
		}

		// ジオメトリ
		for (auto mesh : Span<aiMesh*>(scene->mMeshes, scene->mNumMeshes)) {

			if (mesh->mPrimitiveTypes != aiPrimitiveType_TRIANGLE) {
				auto getPrimitiveName = [](int type) {
					String message;
					if (type & aiPrimitiveType_POINT) message += "POINT,";
					if (type & aiPrimitiveType_LINE) message += "LINE,";
					if (type & aiPrimitiveType_TRIANGLE) message += "TRIANGLE,";
					if (type & aiPrimitiveType_POLYGON) message += "POLYGON,";
					if (type & aiPrimitiveType_NGONEncodingFlag) message += "NGON,";
					if(!message.empty()) message.pop_back();
					return message;
				};
				LOG_ERROR("非対応のプリミティブライプです [{}]", getPrimitiveName(mesh->mPrimitiveTypes));
				//continue;
			}

			auto result = Amuse::Mesh::Create();
			auto& lod0 = result->lods.emplace_back();
			lod0.source.emplace();

			// 頂点
			if (mesh->mVertices) {
				for (auto v : Span<aiVector3D>(mesh->mVertices, mesh->mNumVertices)) {
					lod0.source->positions.emplace_back(v.x, v.y, v.z);
				}
			}
			if (mesh->mNormals) {
				for (auto v : Span<aiVector3D>(mesh->mNormals, mesh->mNumVertices)) {
					lod0.source->normals.emplace_back(v.x, v.y, v.z);
				}
			}
			if (mesh->mTangents) {
				for (auto v : Span<aiVector3D>(mesh->mTangents, mesh->mNumVertices)) {
					lod0.source->tangents.emplace_back(v.x, v.y, v.z);
				}
			}
			if (mesh->mColors[0]) {
				for (auto c : Span<aiColor4D>(mesh->mColors[0], mesh->mNumVertices)) {
					lod0.source->colors.emplace_back(c.r, c.g, c.b, c.a);
				}
			}
			if (mesh->mTextureCoords[0]) {
				for (auto v : Span<aiVector3D>(mesh->mTextureCoords[0], mesh->mNumVertices)) {
					lod0.source->uvs.emplace_back(v.x, 1 - v.y);
				}
			}

			// インデックス
			for (auto face : Span<aiFace>(mesh->mFaces, mesh->mNumFaces)) {
				if (face.mNumIndices == 3) {
					lod0.source->indices.emplace_back(face.mIndices[0]);
					lod0.source->indices.emplace_back(face.mIndices[1]);
					lod0.source->indices.emplace_back(face.mIndices[2]);
				}
			}

			// セクション
			auto& section = lod0.sections.emplace_back();
			section.indexCount = lod0.source->indices.size();
			section.material = mesh->mMaterialIndex;

			result->materials = materials;

			result->apply();

			return result;
		}

		return nullptr;
	}


	//!	@brief			生成
	Ref<Mesh> Mesh::Create() {
		return new Mesh();
	}

	void Mesh::apply()
	{
		bounds.box = Box::Empty;

		for (auto& lod : lods)
		{
			// source情報がありレイアウトが未設定の場合更新対象とする
			if (lod.source && !lod.buffer.layout)
			{
#define ADD_ATTRIBUTE(member,semantic,type,dimension,index) \
				if(!lod.source->member.empty()) { \
					layoutDesc.attributes.emplace_back(semantic,layoutDesc.vertexStride,type,dimension,index);\
					layoutDesc.vertexStride += sizeof(Vec4);\
					vertexCount = std::max(vertexCount,lod.source->member.size());\
				}

				size_t vertexCount = 0;
				VertexLayoutDesc layoutDesc;
				ADD_ATTRIBUTE(positions	, Semantic::Position, ElementType::Float, 3, 0);
				ADD_ATTRIBUTE(normals	, Semantic::Normal	, ElementType::Float, 3, 0);
				ADD_ATTRIBUTE(binormals , Semantic::Binormal, ElementType::Float, 3, 0);
				ADD_ATTRIBUTE(tangents	, Semantic::Tangent	, ElementType::Float, 3, 0);
				ADD_ATTRIBUTE(uvs		, Semantic::TexCoord, ElementType::Float, 2, 0);
				ADD_ATTRIBUTE(colors	, Semantic::Color	, ElementType::Float, 4, 0);
#undef ADD_ATTRIBUTE

				BufferDesc vertexDesc = BufferDesc::Vertex<s32>(0);
				vertexDesc.stride = layoutDesc.vertexStride;
				vertexDesc.size = vertexDesc.stride * lod.source->positions.size();


				lod.buffer.layout = VertexLayout::Create(layoutDesc);
				lod.buffer.indices = Buffer::Create(BufferDesc::Index<u32>(lod.source->indices.size()), lod.source->indices);
				lod.buffer.vertices = Buffer::Create(vertexDesc);
				lod.buffer.vertices->update(
					[&](void* ptr) {

						size_t offset = 0;

#define					UPLOAD_ELEMENT(member)\
						{\
							using type = std::remove_const_t<decltype(lod.source->member)::value_type>;\
							if (!lod.source->member.empty()) {\
								if (lod.source->member.size() != vertexCount) {\
									LOG_WARNING("{}の頂点数が一致していません", #member);\
								} else {\
									for (size_t i = 0; i < vertexCount; ++i) { \
										*GetOffsetPtr<type>(ptr, i * vertexDesc.stride  + offset) = lod.source->member[i]; \
									}\
								}\
								offset+=sizeof(Vec4);\
							}\
						}

						UPLOAD_ELEMENT(positions);
						UPLOAD_ELEMENT(normals);
						UPLOAD_ELEMENT(binormals);
						UPLOAD_ELEMENT(tangents);
						UPLOAD_ELEMENT(uvs);
						UPLOAD_ELEMENT(colors);

#undef UPLOAD_ELEMENT
					}
				);


				lod.bounds.box = Box::Empty;
				for (auto& vertex : lod.source->positions) {
					lod.bounds.box += vertex;
				}

				// TODO SphereBoundsも計算する

				if (!lod.bKeepSource) {
					lod.source.reset();
				}
			}

			bounds.box += lod.bounds.box.min();
			bounds.box += lod.bounds.box.max();
		}
	}

	void Mesh::clearSources()
	{
		for (auto& lod : lods)
		{
			lod.source = {};
			lod.buffer = {};
		}
	}

}