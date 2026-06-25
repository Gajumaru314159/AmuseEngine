//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>

namespace Amuse {
#pragma region Enum

    //! @brief      テクスチャ・フィルター
    //! @ingroup AmuseRHI
    enum class TextureFilter :u32 {
        Point,  //!< 最近ピクセルのテクセルを使用
        Linear, //!< 2*2の線形補完値を使用
    };


    //! @brief      ミップマップ・フィルター
    //! @ingroup AmuseRHI
    enum class MipFilter :u32 {
        Point,  //!< 最近ピクセルのテクセルを使用
        Linear, //!< 2*2の線形補完値を使用
    };


    //! @brief      異方性レベル
    //! @ingroup AmuseRHI
    enum class Anisotropy : u32 {
        None,
        Level1,
        Level2,
        Level4,
        Level8,
        Level16,
    };


    //! @brief      テクスチャ繰り返し設定
    //! 
    //! @details    テクスチャサンプル時、UV座標が0～1を超えた場合にどのようにサンプルするか。
    //! @ingroup AmuseRHI
    enum class TextureAddress {
        Repeat, //!< 繰り返して表示
        Clamp,  //!< 端を伸ばして表示
        Mirror, //!< ミラー処理して表示
    };

#pragma endregion

    //! @brief      サンプラー定義
    //! @ingroup AmuseRHI
    struct SamplerDesc {

        String			name;			    //!< 名前
        TextureFilter  filter;             //!< 拡縮時のフィルタ
        MipFilter      mipFilter;          //!< ミップ時のフィルタ
        bool            zFilter;            //!< 3DテクスチャのZ軸フィルタを設定するか
        Anisotropy      anisotropy;         //!< 異方性フィルタレベル
        TextureAddress  addressU;           //!< テクスチャ・アドレス・モード U
        TextureAddress  addressV;           //!< テクスチャ・アドレス・モード V
        TextureAddress  addressW;           //!< テクスチャ・アドレス・モード W
        f32             mipLodBias;         //!< ミップLODバイアス
        Range           mipLodRange;        //!< ミップLOD範囲

    public:

        //! @brief      コンストラクタ
        SamplerDesc(
            TextureFilter filter = TextureFilter::Linear,
            TextureAddress addressU = TextureAddress::Repeat,
            TextureAddress addressV = TextureAddress::Repeat,
            TextureAddress addressW = TextureAddress::Repeat,
            f32 mipLodBias = 0,
            Anisotropy anisotropy = Anisotropy::None,
            f32 minLod = 0,
            f32 maxLod = (std::numeric_limits<f32>::max)(),
            MipFilter mipFilter = MipFilter::Linear,
            bool zFilter = false
        ) {
            setFilter(filter, mipFilter);
            setZFilter(zFilter);
            setAnisotropy(anisotropy);
            setAddress(addressU, addressV, addressW);
            setMipLod(mipLodBias, minLod, maxLod);
        }

        //! @brief      フィルタ設定
        SamplerDesc& setFilter(TextureFilter filter, MipFilter mipFilter = MipFilter::Linear) {
            this->filter = filter;
            this->mipFilter = mipFilter;
            return *this;
        }

        //! @brief      Zフィルタ設定
        SamplerDesc& setZFilter(bool zFilter) {
            this->zFilter = zFilter;
            return *this;
        }

        //! @brief      異方性フィルタレベル設定
        SamplerDesc& setAnisotropy(Anisotropy anisotropy) {
            this->anisotropy = anisotropy;
            return *this;
        }

        //! @brief      テクスチャ・アドレス・モード設定
        SamplerDesc& setAddress(TextureAddress u, TextureAddress v, TextureAddress w = TextureAddress::Repeat) {
            this->addressU = u;
            this->addressV = v;
            this->addressW = w;
            return *this;
        }

        //! @brief      テクスチャ・アドレス・モード設定
        SamplerDesc& setAddress(TextureAddress mode) {
            return setAddress(mode,mode,mode);
        }

        //! @brief      ミップLOD設定
        SamplerDesc& setMipLod(f32 bias, f32 min, f32 max) {
            this->mipLodBias = bias;
            this->mipLodRange.min = min;
            this->mipLodRange.max = max;
            return *this;
        }

    };



	//! @brief サンプラ設定のハッシュ関数
	//! @ingroup AmuseRHI
	struct SamplerDescHasher {
		//! @brief 演算子処理を行う
		size_t operator()(const SamplerDesc& desc) const noexcept
		{
			size_t hash = 0;
			Hash::Combine(hash, desc.name);
			Hash::Combine(hash, desc.filter);
			Hash::Combine(hash, desc.mipFilter);
			Hash::Combine(hash, desc.zFilter);
			Hash::Combine(hash, desc.anisotropy);
			Hash::Combine(hash, desc.addressU);
			Hash::Combine(hash, desc.addressV);
			Hash::Combine(hash, desc.addressW);
			Hash::Combine(hash, desc.mipLodBias);
			Hash::Combine(hash, desc.mipLodRange.min);
			Hash::Combine(hash, desc.mipLodRange.max);
			return hash;
		}
	};
	//! @brief サンプラ設定の等値比較
	//! @ingroup AmuseRHI
	struct SamplerDescEqual {
        //! @brief 演算子処理を行う
        bool operator()(const SamplerDesc& lhs, const SamplerDesc& rhs) const noexcept
        {
            return lhs.name == rhs.name &&
                   lhs.filter == rhs.filter &&
                   lhs.mipFilter == rhs.mipFilter &&
                   lhs.zFilter == rhs.zFilter &&
                   lhs.anisotropy == rhs.anisotropy &&
                   lhs.addressU == rhs.addressU &&
                   lhs.addressV == rhs.addressV &&
                   lhs.addressW == rhs.addressW &&
                   lhs.mipLodBias == rhs.mipLodBias &&
                   lhs.mipLodRange.min == rhs.mipLodRange.min &&
                   lhs.mipLodRange.max == rhs.mipLodRange.max;
        }
    };
}
