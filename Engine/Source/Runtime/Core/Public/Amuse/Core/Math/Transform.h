//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Math/Vector3.h>
#include <Amuse/Core/Math/Matrix.h>

namespace Amuse::Core {

    //! @brief  トランスフォーム
    class Transform {
    public:

        //! @brief 等値比較を行う。
        bool operator==(const Transform&)const noexcept;
        //! @brief 非等値比較を行う。
        bool operator!=(const Transform&)const noexcept;

        //! @brief 2つのトランスフォームを合成した結果を返す。
        Transform operator*(const Transform&)const;
        //! @brief 右辺のトランスフォームをこのトランスフォームへ合成する。
        Transform operator*=(const Transform&);

        //! @brief Matrix へ型変換する。
        operator Matrix()const;

        //! @brief  Matrix に変換
        Matrix toMatrix()const;

        //! @brief  反転
        Transform inverse()const;

    public:

        static const Transform Identity;  //!< 初期状態

    public:

        Vec3    position;   //!< 位置
        Quat    rotation;   //!< 回転
        Vec3    scale{1,1,1};      //!< 拡縮

    };

}
