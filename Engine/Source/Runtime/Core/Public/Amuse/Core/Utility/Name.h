//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/CorePrivate.h>
#include <Amuse/Core/CoreForward.h>

namespace Amuse::Core {

    namespace internal {
        class NameData;
    }

    //! @brief      識別用名
    //! 
    //! @details    Name クラスは高速な比較が可能です。<br>
    //!             下記の問題点を解決しています。
    //!             * Stringを直接比較：Ο(n)
    //!             * Stringのハッシュ値を比較：重複
    //!             処理のコストは以下の通りです。
    //!             * 新しい名前の構築：低速
    //!             * 辞書に存在する名前の構築：
    //!             * コピー：高速
    //!             * 比較：高速
    //!             文字リテラルから生成する場合はstatic変数として宣言することで
    //!             2回目以降のアクセスが高速になります。
    class Name {
    public:
        //! @brief Hash 型のエイリアス。
        using Hash = u32;
        friend struct std::hash<Name>;
    public:
        //! @brief 型情報を登録する。
        static void Register(ServiceInjector&);
    public:

        //! @brief  デフォルトコンストラクタ
        Name();

        //! @brief  コンストラクタ(文字列から)
        explicit Name(StringView name);

        //! @brief  コンストラクタ(ハッシュ値から)
        //! 
        //! @details    ハッシュ値の計算は内部実装に依存します。
        explicit Name(Hash hash);

        //! @brief 名前オブジェクトをコピー構築する。
        Name(const Name&) = default;
        //! @brief 名前オブジェクトをムーブ構築する。
        Name(Name&&) = default;
        //! @brief 値を代入する。
        Name& operator=(const Name&) = default;
        //! @brief 値を代入する。
        Name& operator=(Name&&) = default;

        //! @brief  代入演算子(StringView)
        Name& operator=(StringView name);
        //! @brief 等値比較を行う。
        bool operator==(const Name&)const;
        //! @brief 非等値比較を行う。
        bool operator!=(const Name&)const;

        //! @brief  名前をStringViewで取得
        StringView toSV()const;

        //! @brief  空か
        bool empty()const;

    public:

        //! @brief  NameData から生成(NameDictionary用)
        Name(const internal::NameData& nameData);

    private:
        void setName(StringView name);
        void clear();
    private:
        StringView m_view;
        Hash m_hash;
    };






    //===============================================================
    // インライン関数
    //===============================================================
    //! @cond

    //! @brief  等価演算子
    inline bool Name::operator==(const Name& rhs)const {
        return m_hash == rhs.m_hash;
    }

    //! @brief  否等価演算子
    inline bool Name::operator!=(const Name& rhs)const {
        return !(*this == rhs);
    }

    //! @brief  名前をStringViewで取得
    inline StringView Name::toSV()const {
        return m_view;
    }

    //! @brief  空か
    inline bool Name::empty()const {
        return m_view.empty();
    }

    //! @endcond
}


//===============================================================
// ハッシュ化
//===============================================================
//! @cond
template<>
struct std::hash<Amuse::Core::Name> {
public:
    size_t operator()(const Amuse::Core::Name& name)const noexcept {
        return name.m_hash;
    }
};
//! @endcond


//===============================================================
// フォーマット
//===============================================================
//! @cond
template <> struct std::formatter<Amuse::Core::Name, Amuse::Core::Char> {
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx) -> decltype(ctx.begin()) {
        return ctx.end();
    }

    template<typename FormatContext>
    auto format(const Amuse::Core::Name& value, FormatContext& ctx) const {
        return format_to(ctx.out(), "{}", value.toSV());
    }
};
//! @endcond
