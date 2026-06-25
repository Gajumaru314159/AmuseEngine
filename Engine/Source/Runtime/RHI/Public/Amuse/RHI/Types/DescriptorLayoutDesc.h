//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>

namespace Amuse {
	//! @brief バインディングタイプ
	//! 
	//! @details バインディングタイプは各グラフィックAPIでは以下の表の対応関係にあります。
	//! | BindingType			| HLSL						| D3D12				| GLSL				| Vulkan				|
	//! |-----------------------|---------------------------|-------------------|-------------------|-----------------------|
	//! | Texture				| (t) Texture2D				| SRV				| sampler2D			| eSampledImage			|
	//! | RWTexture				| (u) RWTexture2D			| UAV				| image2D			| eStorageImage			|
	//! | Buffer				| (t) Buffer<T>				| SRV				| buffer			| eUniformTexelBuffer	|
	//! | RWBuffer				| (u) RWBuffer<T>			| UAV				| buffer			| eStorageTexelBuffer	|
	//! | StructuredBuffer		| (t) StructuredBuffer<T>	| SRV				| buffer			| eStorageBuffer		|
	//! | RWStructuredBuffer	| (u) RWStructuredBuffer<T>	| UAV				| buffer			| eStorageBuffer		|
	//! | ByteAddressBuffer		| (t) ByteAddressBuffer		| SRV				| buffer			| eStorageBuffer		|
	//! | RWByteAddressBuffer	| (u) RWByteAddressBuffer	| UAV				| buffer			| eStorageBuffer		|
	//! | ConstantBuffer		| (b) ConstantBuffer		| CBV				| uniform			| eUniformBuffer		|
	//! | Sampler				| (s) SamplerState			| Sampler			| sampler			| eSampler				|
	//! @ingroup AmuseRHI
	enum class BindingType : u32 {
		Unknown = 0xFFFFFFFF,
		Texture = 0,
		RWTexture,
		Buffer,
		RWBuffer,
		StructuredBuffer,
		RWStructuredBuffer,
		ByteAddressBuffer,
		RWByteAddressBuffer,
		ConstantBuffer,
		Sampler,
	};

	//! @brief シェーダバインディング項目
	//! @ingroup AmuseRHI
	struct BindingItem {
		BindingType type; //!< 種別
		s32         index; //!< レジスタ番号

		//! @brief バインディング項目を初期化する
		constexpr BindingItem(BindingType type, s32 index)
			: type(type), index(index) {}
	};

	//! @brief      BindingItem定義のユーティリティ
	//! @ingroup AmuseRHI
	struct Binding {

		#define DECL_BINDING(TYPE) \
		static constexpr BindingItem TYPE(s32 index) {\
			return BindingItem(BindingType::TYPE, index);\
		}

		// indexが負の場合は前のBindingItemのindexからの相対値を表します。
		// 先頭のBindingItemが負の数の場合は0に置き換えられます
		//! @brief Texture バインディング項目を生成する
		DECL_BINDING(Texture);
		//! @brief RWTexture バインディング項目を生成する
		DECL_BINDING(RWTexture);
		//! @brief Buffer バインディング項目を生成する
		DECL_BINDING(Buffer);
		//! @brief RWBuffer バインディング項目を生成する
		DECL_BINDING(RWBuffer);
		//! @brief StructuredBuffer バインディング項目を生成する
		DECL_BINDING(StructuredBuffer);
		//! @brief RWStructuredBuffer バインディング項目を生成する
		DECL_BINDING(RWStructuredBuffer);
		//! @brief ByteAddressBuffer バインディング項目を生成する
		DECL_BINDING(ByteAddressBuffer);
		//! @brief RWByteAddressBuffer バインディング項目を生成する
		DECL_BINDING(RWByteAddressBuffer);
		//! @brief ConstantBuffer バインディング項目を生成する
		DECL_BINDING(ConstantBuffer);
		//! @brief Sampler バインディング項目を生成する
		DECL_BINDING(Sampler);

		#undef DECL_BINDING
	};

	//! @brief ディスクリプタレイアウト生成情報
	//! @ingroup AmuseRHI
	struct DescriptorLayoutDesc {
		String				name;		//!< 名前
		Vector<BindingItem> items;		//!< バインディングスロット

		//! @brief 空のディスクリプタレイアウト情報を生成する
		DescriptorLayoutDesc() = default;
		//! @brief バインディング項目リストから生成する
		DescriptorLayoutDesc(std::initializer_list<BindingItem> items) : items(items) {}
	};

}
