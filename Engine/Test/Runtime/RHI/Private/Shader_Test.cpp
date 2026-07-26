//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <RHITestBase.h>
#include <Amuse/Core/File/Directory.h>
#include <Amuse/Core/File/File.h>
#include <Amuse/Core/File/Path.h>
#include <magic_enum.hpp>

using namespace Amuse;

static const char* s_code =
R"(
    // Vertex Shader
    struct VSInput {
        float3 position : POSITION;
        float3 color : COLOR;
    };

    struct VSOutput {
        float4 position : SV_POSITION;
        [[vk::location(0)]] float3 color : COLOR;
    };

    VSOutput VS_Main(VSInput input) {
        VSOutput output;
        output.position = float4(input.position, 1.0);
        output.color = input.color;
        return output;
    }

    // Hull Shader
    struct HSInput {
        float3 position : POSITION;
    };

    struct HSOutput {
        float3 position : POSITION;
    };

    struct PatchConstant {
        float edges[3] : SV_TessFactor;         // 外側のテッセレーション係数
        float inside : SV_InsideTessFactor;    // 内側のテッセレーション係数（必要に応じて）
    };

    [domain("tri")]
    [partitioning("fractional_odd")]
    [outputtopology("triangle_cw")]
    [patchconstantfunc("PatchConstantFunction")]
    [outputcontrolpoints(3)]
    HSOutput HS_Main(InputPatch<HSInput, 3> patch, uint id : SV_OutputControlPointID) {
        HSOutput output;
        output.position = patch[id].position;
        return output;
    }

    PatchConstant PatchConstantFunction(InputPatch<HSInput, 3> patch) {
        PatchConstant pc;
        pc.edges[0] = 1.0;
        pc.edges[1] = 1.0;
        pc.edges[2] = 1.0;
        return pc;
    }

    // Domain Shader
    struct DSInput {
        float3 position : POSITION;
    };

    struct DSOutput {
        float4 position : SV_POSITION;
    };

    [domain("tri")]
    DSOutput DS_Main(PatchConstant pc, float3 bary : SV_DomainLocation, const OutputPatch<DSInput, 3> patch) {
        DSOutput output;
        output.position = float4(0.0, 0.0, 0.0, 1.0);
        for (int i = 0; i < 3; ++i) {
            output.position += float4(patch[i].position, 1.0) * bary[i];
        }
        return output;
    }

    // Geometry Shader
    struct GSInput {
        float4 position : SV_POSITION;
        [[vk::location(0)]] float3 color : COLOR;
    };

    struct GSOutput {
        float4 position : SV_POSITION;
        [[vk::location(0)]] float3 color : COLOR;
    };

    [maxvertexcount(3)]
    void GS_Main(triangle GSInput input[3], inout TriangleStream<GSOutput> stream) {
        for (int i = 0; i < 3; ++i) {
            GSOutput output;
            output.position = input[i].position;
            output.color = input[i].color;
            stream.Append(output);
        }
    }

    // Pixel Shader
    struct PSInput {
        [[vk::location(0)]] float3 color : COLOR;
    };

    [[vk::location(0)]] float4 PS_Main(PSInput input) : SV_TARGET {
        return float4(input.color, 1.0);
    }

    // Compute Shader
    [[vk::binding(0)]] RWStructuredBuffer<float4> buffer;

    [numthreads(1, 1, 1)]
    void CS_Main(uint3 DTid : SV_DispatchThreadID) {
        buffer[DTid.x] = float4(DTid.x, DTid.y, DTid.z, 1.0);
    }
)";

TYPED_TEST(RHITest, Shader_Vertex) {
	ShaderCompileDesc desc;
    desc.code = s_code;
    desc.stage = ShaderStage::Vertex;
    auto shader = Shader::Compile(desc);

	if (Shader::Supports(desc.stage)) {
		EXPECT_TRUE(shader != nullptr);
	} else {
		EXPECT_TRUE(shader == nullptr);
	}
}
TYPED_TEST(RHITest, Shader_Hull) {
	ShaderCompileDesc desc;
	desc.code = s_code;
	desc.stage = ShaderStage::Hull;
	auto shader = Shader::Compile(desc);
	if (Shader::Supports(desc.stage)) {
		EXPECT_TRUE(shader != nullptr);
	} else {
		EXPECT_TRUE(shader == nullptr);
	}
}
TYPED_TEST(RHITest, Shader_Domain) {
	ShaderCompileDesc desc;
	desc.code = s_code;
	desc.stage = ShaderStage::Domain;
	auto shader = Shader::Compile(desc);
	if (Shader::Supports(desc.stage)) {
		EXPECT_TRUE(shader != nullptr);
	} else {
		EXPECT_TRUE(shader == nullptr);
	}
}
TYPED_TEST(RHITest, Shader_Geometry) {
	ShaderCompileDesc desc;
	desc.code = s_code;
	desc.stage = ShaderStage::Geometry;
	auto shader = Shader::Compile(desc);
	if (Shader::Supports(desc.stage)) {
		EXPECT_TRUE(shader != nullptr);
	} else {
		EXPECT_TRUE(shader == nullptr);
	}
}
TYPED_TEST(RHITest, Shader_Pixel) {
	ShaderCompileDesc desc;
	desc.code = s_code;
	desc.stage = ShaderStage::Pixel;
	auto shader = Shader::Compile(desc);
	if (Shader::Supports(desc.stage)) {
		EXPECT_TRUE(shader != nullptr);
	} else {
		EXPECT_TRUE(shader == nullptr);
	}
}

TYPED_TEST(RHITest, Shader_CompileResourceFiles) {
	const String shaderRoot = "Shaders";
	ASSERT_TRUE(Directory::Exists(shaderRoot)) << "Shaders directory is not found.";

	const Array<ShaderStage, 6> stages{
		ShaderStage::Vertex,
		ShaderStage::Hull,
		ShaderStage::Domain,
		ShaderStage::Geometry,
		ShaderStage::Pixel,
		ShaderStage::Compute,
	};

	size_t shaderFileCount = 0;
	size_t compileCount = 0;
	for (const auto& path : Directory::Contents(shaderRoot, Recursive::Yes)) {
		const String shaderPath = Path::Normalize(path);
		if (!File::Exists(shaderPath) || Path::Extension(shaderPath, WithDot::Yes) != ".hlsl") {
			continue;
		}

		++shaderFileCount;
		const auto code = File::ReadAllText(shaderPath);
		ASSERT_TRUE(code.has_value()) << shaderPath;

		size_t entryPointCount = 0;
		for (const auto stage : stages) {
			if (!code->contains(Shader::GetEntryName(stage))) {
				continue;
			}
			++entryPointCount;

			if (!Shader::Supports(stage)) {
				continue;
			}

			ShaderCompileDesc desc;
			desc.name = shaderPath;
			desc.code = code.value();
			desc.stage = stage;
			desc.directories.push_back(".");
			desc.directories.push_back(Path::Parent(shaderPath));

			EXPECT_TRUE(Shader::Compile(desc) != nullptr)
				<< shaderPath << " " << magic_enum::enum_name(stage);
			++compileCount;
		}
		EXPECT_NE(entryPointCount, 0u) << shaderPath;
	}

	EXPECT_NE(shaderFileCount, 0u);
	EXPECT_NE(compileCount, 0u);
}
