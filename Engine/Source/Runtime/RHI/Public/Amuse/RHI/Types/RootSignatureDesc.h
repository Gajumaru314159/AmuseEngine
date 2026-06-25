//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>
#include <Amuse/RHI/Forward.h>
#include <Amuse/RHI/Constants.h>
#include <Amuse/RHI/Types/ShaderStage.h>
#include <Amuse/RHI/Types/SamplerDesc.h>

namespace Amuse {
	//! @brief      ルートシグネチャ・フラグ
	//! @see        RootSignatureDesc
	//! @ingroup AmuseRHI
	enum class RootSignatureFlag :u32 {
		None							= 0,
		AllowInputAssemblerInputLayout  = get_bit(0),	//!< 頂点入力あり
		DenyVertexShaderAccess			= get_bit(1),	//!< 
		DenyHullShaderAccess			= get_bit(2),	//!< 
		DenyDomainShaderAccess			= get_bit(3),	//!< 
		DenyGeometryShaderAccess		= get_bit(4),	//!< 
		DenyPixelShaderAccess			= get_bit(5),	//!< 
		AllowStreamOutput				= get_bit(6),	//!< 
		DenyAmplificationShaderAccess	= get_bit(7),	//!< 
		DenyMeshShaderAccess			= get_bit(8),	//!< 
		EnableBindless					= get_bit(9),	//!< 
	};
	//! @brief      ルートシグネチャ・フラグ・セット
	//! @see        RootSignatureDesc
	using RootSignatureFlags = BitFlags<RootSignatureFlag>;


	//! @brief      ルートコンスタント定義
	//! @see        RootParameter
	//! @ingroup AmuseRHI
	struct RootConstantsDesc {
		u32 size;          //!< 値
		u32 registerNo;     //!< レジスタ番号 (D3D12のみ)
		u32 registerSpace;  //!< レジスタ空間 (D3D12のみ)
	public:
		//! @brief      コンストラクタ
		RootConstantsDesc() : RootConstantsDesc(0, 0, 0) {}
		//! @brief      コンストラクタ(DescriptorTable)
		RootConstantsDesc(u32 size,u32 registerNo, u32 registerSpace=0)
			:size(size),registerNo(registerNo), registerSpace(registerSpace) {}


		//! @brief set を実行する
		RootConstantsDesc& set(u32 size, u32 registerNo, u32 registerSpace = 0) {
			this->size = size;
			this->registerNo = registerNo;
			this->registerSpace = registerSpace;
			return *this;
		}
	};

	//! @brief      スタティックサンプラー定義
	//! 
	//! @details    レジスタ番号とレジスタ空間はシェーダで定義された値と一致させる必要があります。
	//!             レジスタ番号が1、レジスタ空間が0の場合はHLSLでは以下のように定義されます。
	//!             ```SamplerState  mainSampler : register(s1,space0)```
	//!             レジスタ空間を分けることで、同じレジスタ番号を使用することができます。
	//! @see        RootSignatureDesc
	//! @ingroup AmuseRHI
	struct StaticSamplerDesc {
		SamplerDesc     sampler;        //!< サンプラー設定
		u32             registerNo;     //!< レジスタ番号
		u32             registerSpace;  //!< レジスタ空間
		ShaderStage		visibility;		//!< どのシェーダステージから利用可能か
	public:
		//! @brief      コンストラクタ
		StaticSamplerDesc() = default;

		//! @brief      コンストラクタ
		StaticSamplerDesc(const SamplerDesc& sampler, u32 registerNo, u32 registerSpace=0, ShaderStage visibility = ShaderStage::All)
			:sampler(sampler), registerNo(registerNo), registerSpace(registerSpace), visibility(visibility) {}
	};

	//! @brief      静的サンプラー・配列
	using StaticSamplerArray = FixedVector<StaticSamplerDesc, STATIC_SAMPLER_MAX>;


	//! @brief ルートシグネチャ定義
	//! @ingroup AmuseRHI
	struct RootSignatureDesc {
		String							name;		//!< 名前
		Vector<Ref<DescriptorLayout>>	layouts;	//!< バインディングスロット
		StaticSamplerArray				samplers;	//!< 静的サンプラー
		RootConstantsDesc				constants;	//!< ルートコンスタント
		RootSignatureFlags				flags;		//!< フラグ

		RootSignatureDesc() = default;
		//! @brief RootSignatureDesc を実行する
		RootSignatureDesc(
			std::initializer_list<Ref<DescriptorLayout>> layouts,
			std::initializer_list<StaticSamplerDesc> samplers = {},
			const RootConstantsDesc& constants = {},
			RootSignatureFlags flags = {});
	};

}
