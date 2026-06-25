//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/RHI/Shader.h>
#include <Amuse/RHI/RHI.h>

namespace Amuse {
    //! @brief      シェーダのエントリ関数名を取得
    const char* Shader::GetEntryName(ShaderStage stage) {
        switch (stage)
        {
        case Amuse::ShaderStage::Vertex:          return "VS_Main";
        case Amuse::ShaderStage::Hull:            return "HS_Main";
        case Amuse::ShaderStage::Domain:          return "DS_Main";
        case Amuse::ShaderStage::Geometry:        return "GS_Main";
        case Amuse::ShaderStage::Pixel:           return "PS_Main";
        case Amuse::ShaderStage::Compute:         return "CS_Main";
        case Amuse::ShaderStage::Task:            return "TS_Main";
        case Amuse::ShaderStage::Mesh:            return "MS_Main";
        case Amuse::ShaderStage::RayGen:          return "RGS_Main";
        case Amuse::ShaderStage::AnyHit:          return "AHS_Main";
        case Amuse::ShaderStage::ClosestHit:      return "CHS_Main";
        case Amuse::ShaderStage::Miss:            return "MS_Main";
        case Amuse::ShaderStage::Intersection:    return "IS_Main";
        case Amuse::ShaderStage::Callable:        return "CS_Main";
        case Amuse::ShaderStage::Amplification:   return "AS_Main";
		default:                                    return "";
        }
    }


    Ref<Shader> Shader::Compile(const ShaderCompileDesc& desc) {
        if (auto rhi = Device::Get()) {
            return rhi->compileShader(desc);
        }
        return nullptr;
    }

    //! @brief              コンストラクタ
    Ref<Shader> Shader::Compile(const String& code,ShaderStage stage) {
        ShaderCompileDesc desc;
		desc.code = code;
        desc.stage = stage;
        return Compile(desc);
    }
    Ref<Shader> Shader::CompileVS(const String& code) {
        return Shader::Compile(code, ShaderStage::Vertex);
    }
    Ref<Shader> Shader::CompilePS(const String& code) {
        return Shader::Compile(code, ShaderStage::Pixel);
    }
    Ref<Shader> Shader::CompileCS(const String& code) {
        return Shader::Compile(code, ShaderStage::Compute);
    }



    //! @brief              コンストラクタ
    Ref<Shader> Shader::Load(BlobView binary, ShaderStage stage) {
        if (auto device = Device::Get()) {
            return device->loadShader(binary, stage);
        }
        return nullptr;
    }
    Ref<Shader> Shader::LoadVS(BlobView binary) {
        return Shader::Load(binary, ShaderStage::Vertex);
    }
    Ref<Shader> Shader::LoadPS(BlobView binary) {
        return Shader::Load(binary, ShaderStage::Vertex);
    }


    //! @brief          シェーダステージをサポートしているか
    bool Shader::Supports(ShaderStage stage) {
        if (auto device = Device::Get()) {
            return device->supports(stage);
        }
        return false;
    }

}