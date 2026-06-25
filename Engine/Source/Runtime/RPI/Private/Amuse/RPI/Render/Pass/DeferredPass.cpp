//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/RPI/FrameGraph/FG.h>
#include <Amuse/RPI/Material/Material.h>
#include <Amuse/RPI/Material/MaterialSystem.h>
#include <Amuse/RPI/Mesh/Mesh.h>
#include <Amuse/RPI/Render/Pass/DeferredPass.h>
#include <Amuse/RPI/Render/RenderView.h>
#include <Amuse/RHI/CommandList.h>
#include <Amuse/RHI/RenderTexture.h>
#include <Amuse/RHI/Shader.h>
namespace Amuse {


	DeferredPass::DeferredPass() {

		m_mesh = Mesh::Create();
		auto& md = m_mesh->lods.emplace_back().source.emplace();
		//md.name = "FullScreen";
		md.positions = {
			Vec3(-1,1,0),
			Vec3(1,1,0),
			Vec3(-1,-1,0),
			Vec3(1,-1,0),
		};
		md.uvs = {
			Vec2(0,0),
			Vec2(1,0),
			Vec2(0,1),
			Vec2(1,1),
		};
		md.indices = {
			0,1,2,
			1,3,2
		};
		auto& section = m_mesh->lods[0].sections.emplace_back();
		section.indexCount = 6;

		m_mesh->apply();

		/*
		struct Vertex
		{
			Vec3 position;
			Vec2 uv;
		};

		Vertex vertices[] {
			{ Vec3(-1,1,0),  Vec2(0,0)},
			{ Vec3(1,1,0),   Vec2(1,0)},
			{ Vec3(-1,-1,0), Vec2(0,1)},
			{ Vec3(1,-1,0),  Vec2(1,1)},
		};
		u16 indices[] = {
			0,1,2,
			1,3,2
		};

		auto layout = VertexLayout::Create(
			VertexLayoutDesc{
				.name = "FullScreen",
				.attributes = {
					VertexAttribute(Semantic::Position, offsetof(Vertex, position), ElementType::Float, 3),
					VertexAttribute(Semantic::TexCoord, offsetof(Vertex, uv), ElementType::Float, 2),
				},
				.vertexStride = sizeof(Vertex),
			}
		);

		m_mesh = Mesh::Create();
		m_mesh->lods.resize(1);
		m_mesh->lods[0].buffer.layout = layout;
		m_mesh->lods[0].buffer.vertices = Buffer::Create(BufferDesc::Vertex<Vertex>(std::size(vertices)),BlobView(vertices));
		m_mesh->lods[0].buffer.indices = Buffer::Create(BufferDesc::Index<u16>(std::size(indices)),BlobView(indices));
		m_mesh->lods[0].sections.emplace_back(MeshSection{ 0, 0, static_cast<s32>(std::size(indices)) });
		*/
	}

	DeferredPass::Output DeferredPass::render(FG& fg, RenderView& view, const Input& input)const {
		auto& data = view.get<DeferredData>();
		if (!data.material) {
			data.material = [&] {

				auto code = File::ReadAllText("Assets/Shader/DeferredLight.hlsl");
				AMUSE_ASSERT(code, "ファイル読み込み失敗");

				MaterialDesc desc;
				desc.name = "DeferredLight";
				desc.textures = { "Main" ,"Normal","Params", "Depth" };
				desc.integers = { "GBuffer" };

				MaterialPass& pass = desc.passes["PostProcess"];

				auto& shaders = pass.qualities.emplace_back();

				shaders.colors = { TextureFormat::RGBA8 };
				shaders.vs = Shader::CompileVS(code.value());
				shaders.ps = Shader::CompilePS(code.value());
				shaders.inputLayout = {
					{Semantic::Position,ElementType::Float,4},
				};

				return Material::Create(desc);
			}();
		}

		return fg.addPass<Output>(
			"DeferredPass",
			[&](FGBuilder& builder, Output& output) {
				output.albedo = builder.read(input.albedo);
				output.normal = builder.read(input.normal);
				output.params = builder.read(input.params);
				output.depth = builder.read(input.depth);

				RenderTextureDesc desc = fg.getTextureDesc(input.albedo);
				{
					desc.name = "Color";
					desc.format = TextureFormat::RGBA8;
					desc.clear.color = Color::Black;
					output.color = builder.write(builder.create(desc));
				}
			},
			[&](const Output& output, FGResources& resources, Ref<CommandList>& cmdList) {

				cmdList->pushMarker("Deferred Lighting");

				auto albedo = resources.getTexture(output.albedo);
				auto normal = resources.getTexture(output.normal);
				auto params = resources.getTexture(output.params);
				auto depth = resources.getTexture(output.depth);
				data.material->setTexture("Main", albedo);
				data.material->setTexture("Normal", normal);
				data.material->setTexture("Params", params);
				data.material->setTexture("Depth", depth);

				BeginPassParam param;
				param.colors.emplace_back(resources.getTexture(output.color), RenderPassBeforeAccessType::Clear, RenderPassAfterAccessType::Preserve);

				cmdList->beginRenderPass(param);

				MaterialBlockSet blocks(view);
				data.material->record(cmdList, blocks, m_mesh, 0, "PostProcess");

				cmdList->endRenderPass();

				cmdList->popMarker();

			}
		);

	}

}
