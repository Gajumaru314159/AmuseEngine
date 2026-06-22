//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/DirectX12RHI/pch.h>
#include <Amuse/Core/Core.h>
#include <Amuse/RHI/Shader.h>
#include <Amuse/Core/Misc/Blob.h>
#include <Amuse/RHI/Types/PipelineStateDesc.h>
#include <Amuse/RHI/Types/VertexLayout.h>

//===============================================================
// 前方宣言
//===============================================================
namespace Amuse::RHI {
    using namespace Amuse::Core;
    class DirectX12Device;
    class ITexture;
}


//===============================================================
// クラス定義
//===============================================================
namespace Amuse::RHI {
    using namespace Amuse::Core;

    //! @brief  シェーダ実装(DirectX12)
    class DirectX12Shader :public Shader {
    public:

        //===============================================================
        //	コンストラクタ
        //===============================================================

        //! @brief				シェーダーコードからシェーダーオブジェクトを生成
        DirectX12Shader(DirectX12Device& device, const ShaderCompileDesc& desc);


        //! @brief				バイナリからシェーダーオブジェクトを生成
        DirectX12Shader(BlobView blob, ShaderStage stage, StringView name="Shader");


        //! @brief  妥当な状態か
        bool isValid()const;

        
        //! @brief      名前を取得
        const String& getName()const override;


        //! @brief  シェーダステージを取得
        ShaderStage getStage()const override;


        //! @brief  バイナリデータを取得
        const void* getBinaryData()const;


        //! @brief  バイナリサイズを取得
        SIZE_T getBinarySize()const;


    public:

        //! @brief  コンパイル
        void compile(DirectX12Device& device, const ShaderCompileDesc& desc);

    private:

        String              m_name;
        ShaderStage         m_stage;                        //!< シェーダ・ステージ
        Blob                m_shaderBlob;                   //!< シェーダ・バイナリ
        ComPtr<IDxcBlob>    m_shaderBolb2;					//!< シェーダ・バイナリ
        
        Vector<VertexAttribute> m_attributes;

    };

}