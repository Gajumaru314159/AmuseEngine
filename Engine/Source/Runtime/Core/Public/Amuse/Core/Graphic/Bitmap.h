//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/String/StringView.h>
#include <Amuse/Core/Geometry/Size.h>
#include <Amuse/Core/Geometry/IntRect.h>
#include <Amuse/Core/Graphic/Color.h>

namespace Amuse {

    //! @ingroup AmuseCore
    enum class PixelFormat {
        RGBA8,
        RGBA32,
        Alpha,
    };

    //! @ingroup AmuseCore
    enum class ImageFormat {
        Unspecified,
        PNG,
    };

    
    //! @brief  ビットマップ
    //! @ingroup AmuseCore
    class Bitmap {
    public:
        using base_type                 = Vector<IntColor>;                     //!< ベース型
        using iterator                  = base_type::iterator;                  //!< イテレータ
        using const_iterator            = base_type::const_iterator;            //!< const イテレータ
        using reverse_iterator          = base_type::reverse_iterator;          //!< 逆イテレータ
        using const_reverse_iterator    = base_type::const_reverse_iterator;    //!< const 逆イテレータ
    public:

        //===============================================================
        // コンストラクタ / デストラクタ
        //===============================================================

        Bitmap() = default;
        //! @brief 指定サイズと色でビットマップを初期化する。
        Bitmap(s32 width, s32 height, IntColor color = IntColor::Black);
        template<class TGen>
        //! @brief ジェネレータで各ピクセルを初期化するビットマップを生成する。
        Bitmap(s32 width, s32 height,TGen generator);
        //! @brief 画像ファイルを読み込んでビットマップを生成する。
        explicit Bitmap(StringView path,ImageFormat format = ImageFormat::Unspecified);


        //===============================================================
        // ゲッター
        //===============================================================

        //! @brief ビットマップの幅をピクセル単位で取得する。
        s32 width()const noexcept;
        //! @brief ビットマップの高さをピクセル単位で取得する。
        s32 height()const noexcept;
        //! @brief ビットマップの幅と高さを取得する。
        Size size()const noexcept;
        //! @brief 配列要素間のバイト幅を取得する。
        s32 stride()const noexcept;
        //! @brief 保持しているピクセル数を取得する。
        size_t pixelCount()const noexcept;
        //! @brief ピクセルバッファの総バイト数を取得する。
        size_t byteSize()const noexcept;
        //! @brief 空かどうかを判定する。
        bool empty()const noexcept;


        //===============================================================
        // 領域
        //===============================================================

        //! @brief ピクセルバッファの余剰容量を解放する。
        void shrink_to_fit();
        //! @brief 全要素を削除する。
        void clear();


        //===============================================================
        // データアクセス
        //===============================================================

        //! @brief 指定した要素へアクセスする。
        IntColor* operator[](size_t y);
        //! @brief 指定した要素へアクセスする。
        const IntColor* operator[](size_t y)const;

        //! @brief 指定位置の要素へアクセスする。
        IntColor& at(s32 x, s32 y);
        //! @brief 指定位置の要素へアクセスする。
        const IntColor& at(s32 x,s32 y)const;

        //! @brief 内部データへのポインタを取得する。
        IntColor* data();
        //! @brief 内部データへのポインタを取得する。
        const IntColor* data()const;


        //===============================================================
        // 加工
        //===============================================================

        //! @brief 全ピクセルを指定色で塗りつぶす。
        void fill(IntColor color);

        //! @brief 指定サイズにリサイズし、追加領域を指定色で初期化する。
        void resize(s32 width, s32 height, IntColor color = IntColor::Black);
        //! @brief 指定サイズにリサイズし、追加領域を指定色で初期化する。
        void resize(Size size, IntColor color = IntColor::Black);


        //! @brief 画像を左右反転する。
        void flipX();
        //! @brief 画像を上下反転する。
        void flipY();

        //! @brief 指定矩形で切り出したビットマップを返す。
        Bitmap clipped(const IntRect& rect)const;
        //! @brief 指定座標とサイズで切り出したビットマップを返す。
        Bitmap clipped(s32 x,s32 y,s32 w,s32 h)const;

        template<class TFunc>
        //! @brief 各ピクセルを走査して関数を適用する。
        Bitmap& forEach(TFunc func);
        template<class TFunc>
        //! @brief 各ピクセルを読み取り専用で走査して関数を適用する。
        const Bitmap& forEach(TFunc func)const;


        //===============================================================
        // イテレータ
        //===============================================================

        //! @brief 先頭イテレータを取得する。
        iterator begin() noexcept { return m_pixels.begin(); }
        //! @brief 終端イテレータを取得する。
        iterator end() noexcept { return m_pixels.end(); }
        //! @brief 先頭イテレータを取得する。
        const_iterator begin() const noexcept { return m_pixels.begin(); }
        //! @brief 終端イテレータを取得する。
        const_iterator end() const noexcept { return m_pixels.end(); }
        //! @brief 読み取り専用の先頭イテレータを取得する。
        const_iterator cbegin() const noexcept { return m_pixels.cbegin(); }
        //! @brief 読み取り専用の終端イテレータを取得する。
        const_iterator cend() const noexcept { return m_pixels.cend(); }
        //! @brief 逆順の先頭イテレータを取得する。
        reverse_iterator rbegin() noexcept { return m_pixels.rbegin(); }
        //! @brief 逆順の終端イテレータを取得する。
        reverse_iterator rend() noexcept { return m_pixels.rend(); }
        //! @brief 逆順の先頭イテレータを取得する。
        const_reverse_iterator rbegin() const noexcept { return m_pixels.rbegin(); }
        //! @brief 逆順の終端イテレータを取得する。
        const_reverse_iterator rend() const noexcept { return m_pixels.rend(); }
        //! @brief 読み取り専用の逆順先頭イテレータを取得する。
        const_reverse_iterator crbegin() const noexcept { return m_pixels.crbegin(); }
        //! @brief 読み取り専用の逆順終端イテレータを取得する。
        const_reverse_iterator crend() const noexcept { return m_pixels.crend(); }

    private:

        Vector<IntColor> m_pixels;
        Size m_size;

    };




    template<class TGen>
    Bitmap::Bitmap(s32 width, s32 height, TGen generator) {
        resize(width, height);
		forEach(generator);
    }

    template<class TFunc>
    Bitmap& Bitmap::forEach(TFunc func) {
        size_t index = 0;
        for(s32 y = 0; y < m_size.height; ++y) {
			for(s32 x = 0; x < m_size.width; ++x) {
				func(m_pixels[index],x,y);
                index++;
			}
		}
        return *this;
    }

    template<class TFunc>
    const Bitmap& Bitmap::forEach(TFunc func)const {
        size_t index = 0;
        for (s32 y = 0; y < m_size.height; ++y) {
            for (s32 x = 0; x < m_size.width; ++x) {
                func(m_pixels[index], x, y);
                index++;
            }
        }
        return *this;
    }

}
