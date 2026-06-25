//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/VulkanRHI/pch.h>
#include <Amuse/Core/Core.h>
#include <Amuse/RHI/Shader.h>
#include <Amuse/Core/Misc/Blob.h>
#include <Amuse/RHI/Types/PipelineStateDesc.h>

namespace Amuse {
    //! @brief  シェーダ実装(DirectX12)
    //! @ingroup AmuseVulkanRHI
    class VulkanShader :public Shader {
    public:

        //! @brief				シェーダーコードからシェーダーオブジェクトを生成
        VulkanShader(VulkanDevice& device, const ShaderCompileDesc& desc);


        //! @brief				バイナリからシェーダーオブジェクトを生成
        //!
        //! @param src			バイナリ
        //! @param stage		シェーダステージ
        //! @param errorDest	エラー出力先文字列
        VulkanShader(VulkanDevice& device, BlobView blob, ShaderStage stage, StringView name = "Shader");


        //! @brief      名前を取得
        const String& getName()const override;


        //! @brief  シェーダステージを取得
        ShaderStage getStage()const override;

		vk::ShaderModule getNative()const {
			return *m_shaderModule;
		}


    public:

        //! @brief  コンパイル
        void compile(VulkanDevice& device, const ShaderCompileDesc& desc);

    private:

        String                  m_name;
        ShaderStage             m_stage;                        //!< シェーダ・ステージ
        Blob                    m_shaderBlob;                   //!< シェーダ・バイナリ
		vk::raii::ShaderModule  m_shaderModule = nullptr;       //!< シェーダ・モジュール

    };

}