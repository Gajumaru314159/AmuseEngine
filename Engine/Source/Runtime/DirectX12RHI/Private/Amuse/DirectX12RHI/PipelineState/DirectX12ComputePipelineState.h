//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/DirectX12RHI/pch.h>
#include <Amuse/Core/Core.h>
#include <Amuse/RHI/ComputePipelineState.h>
#include <Amuse/RHI/Types/ComputePipelineStateDesc.h>
#include <Amuse/DirectX12RHI/RootSignature/DirectX12RootSignature.h>

//===============================================================
// 前方宣言
//===============================================================
namespace Amuse {
    class DirectX12Device;
}


//===============================================================
// クラス定義
//===============================================================
namespace Amuse {
    //! @brief		コンピュート・パイプライン実装(DirectX12)
    //! @ingroup AmuseDirectX12RHI
    class DirectX12ComputePipelineState :public ComputePipelineState {
    public:

        //! @brief		コンストラクタ
        DirectX12ComputePipelineState(DirectX12Device&, const ComputePipelineStateDesc& desc);


        //! @brief  妥当な状態か
        bool isValid()const { return m_pipelineState != nullptr; }


        //! @brief      名前を取得
        const String& getName()const override;


        //! @brief      ネイティブオブジェクトを取得
        ID3D12PipelineState* getNative()const noexcept { return m_pipelineState.Get(); }

        DirectX12RootSignature* getRootSignature()const { 
            if (auto p = m_desc.rootSignature.cast<DirectX12RootSignature>()) {
                return p;
            }
            return nullptr;        
        }

    private:

        ComputePipelineStateDesc m_desc;
        ComPtr<ID3D12PipelineState> m_pipelineState;    //!< パイプラインステート
            
    };




    //===============================================================
    // インライン関数
    //===============================================================
    //! @cond

    //! @endcond
}