//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/RPI/Mesh/Mesh.h>
#include <Amuse/RPI/Material/MaterialManager.h>
#include <Amuse/RPI/Material/Material.h>
#include <Amuse/RHI/CommandList.h>
#include <Amuse/RHI/Buffer.h>

#include <Amuse/RPI/Mesh/MeshImport.h>
#include <ufbx.h>
#include <memory>


namespace Amuse {

	//! @brief ufbxで最初の描画可能なメッシュとマテリアルを読み込む
	Ref<Mesh> Mesh::Load(StringView path) {
		ufbx_load_opts opts{};
		opts.load_external_files = true;
		opts.ignore_missing_external_files = true;
		ufbx_error error{};
		std::unique_ptr<ufbx_scene, decltype(&ufbx_free_scene)> scene(
			ufbx_load_file_len(path.data(), path.size(), &opts, &error), &ufbx_free_scene);
		if (!scene) {
			char message[1024]{};
			ufbx_format_error(message, sizeof(message), &error);
			LOG_ERROR("Failed to load {}: {}", path, message);
			return nullptr;
		}
		const ufbx_mesh* mesh = nullptr;
		for (auto candidate : scene->meshes) {
			if (candidate->num_triangles > 0) {
				mesh = candidate;
				break;
			}
		}
		if (!mesh) {
			LOG_WARNING("No triangle mesh found in {}", path);
			return nullptr;
		}

		String directory = Path::Parent(path);
		Map<String, Ref<Texture>, std::less<>> textures;
		Vector<Ref<Material>> materials;

		// ファイルテクスチャを解決し、同じ画像の重複読み込みを避ける
		auto loadTexture = [&](const ufbx_texture* texture) -> Ref<Texture> {
			if (!texture) return nullptr;
			for (auto file : texture->file_textures) {
				for (auto filename : { file->filename, file->relative_filename, file->absolute_filename }) {
					if (filename.length == 0) continue;
					String fullPath(filename.data, filename.length);
					if (!File::Exists(fullPath) && Path::IsRelative(fullPath)) {
						fullPath = Path::Combine(directory, fullPath);
					}
					if (!File::Exists(fullPath)) continue;
					if (auto it = textures.find(fullPath); it != textures.end()) return it->second;
					if (auto loaded = Texture::Load(fullPath)) {
						textures[fullPath] = loaded;
						return loaded;
					}
				}
			}
			return nullptr;
		};

		// マテリアル未指定のファイルにも既定のマテリアルを用意する
		for (size_t i = 0; i < std::max(size_t(1), scene->materials.count); ++i) {
			const ufbx_material* m = scene->materials.count ? scene->materials[i] : nullptr;
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
				shaders.colors = { TextureFormat::RGBA8, TextureFormat::RGBA8, TextureFormat::RGBA8 };
				shaders.depth = TextureFormat::D32;
				shaders.vs = Shader::CompileVS(code.value());
				shaders.ps = Shader::CompilePS(code.value());
				shaders.inputLayout = {
					{Semantic::Position, ElementType::Float, 4},
					{Semantic::Normal, ElementType::Float, 3},
					{Semantic::Tangent, ElementType::Float, 3},
					{Semantic::TexCoord, ElementType::Float, 2},
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
				shaders.inputLayout = { {Semantic::Position, ElementType::Float, 4} };
			}
			auto material = Material::Create(desc);
			material->setMatrix("Matrix", Matrix::Identity);

			Ref<Texture> mainTexture;
			Ref<Texture> normalTexture;
			Color color = Color::White;
			if (m) {
				mainTexture = loadTexture(m->pbr.base_color.texture);
				if (!mainTexture) mainTexture = loadTexture(m->fbx.diffuse_color.texture);
				if (!mainTexture) mainTexture = loadTexture(m->fbx.emission_color.texture);
				normalTexture = loadTexture(m->pbr.normal_map.texture);
				if (!normalTexture) normalTexture = loadTexture(m->fbx.normal_map.texture);
				const auto& baseColor = m->pbr.base_color.has_value ? m->pbr.base_color : m->fbx.diffuse_color;
				if (baseColor.has_value) {
					auto c = baseColor.value_vec4;
					color = Color(c.x, c.y, c.z, baseColor.value_components == 4 ? c.w : 1);
				}
			}
			material->setTexture("Main", mainTexture ? mainTexture : Texture::White());
			material->setTexture("Normal", normalTexture ? normalTexture : Texture::White());
			material->setTexture("Params", Texture::Black());
			material->setVector("Color", color);
			materials.push_back(material);
		}

		auto result = Mesh::Create();
		result->lods.push_back(Detail::ImportMesh(*mesh));
		result->materials = materials;
		result->apply();
		return result;
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
