//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/Core/Hash/Hash.h>

using namespace Amuse;

TEST(Hash, FNV32) {

    // 空文字列のハッシュ
    EXPECT_EQ(Hash::FNV32(""), 0x811c9dc5);

    // 単一文字列のハッシュ
    EXPECT_EQ(Hash::FNV32("a"), 0xe40c292c);
    EXPECT_EQ(Hash::FNV32("b"), 0xe70c2de5);
    EXPECT_EQ(Hash::FNV32("c"), 0xe60c2c52);

    // 数字のみの文字列のハッシュ
    EXPECT_EQ(Hash::FNV32("123"), 0x7238631b);
    EXPECT_EQ(Hash::FNV32("456"), 0x1e377b3c);
    EXPECT_EQ(Hash::FNV32("789"), 0x91db9c09);

    // アルファベット文字列のハッシュ
    EXPECT_EQ(Hash::FNV32("abcdefghijklmnopqrstuvwxyz"), 0xb0bc0c82);
    EXPECT_EQ(Hash::FNV32("ABCDEFGHIJKLMNOPQRSTUVWXYZ"), 0x8a88dd82);

}

TEST(Hash, FNV64) {

    // 空文字列のハッシュ
    EXPECT_EQ(Hash::FNV64(""), 0xcbf29ce484222325);

    // 単一文字列のハッシュ
    EXPECT_EQ(Hash::FNV64("a"), 0xaf63dc4c8601ec8c);
    EXPECT_EQ(Hash::FNV64("b"), 0xaf63df4c8601f1a5);
    EXPECT_EQ(Hash::FNV64("c"), 0xaf63de4c8601eff2);

    // 数字のみの文字列のハッシュ
    EXPECT_EQ(Hash::FNV64("123"), 0x456fc2181822c4db);
    EXPECT_EQ(Hash::FNV64("456"), 0x2be33a1809c9a97c);
    EXPECT_EQ(Hash::FNV64("789"), 0x350a6a180f1c6dc9);

    // アルファベット文字列のハッシュ
    EXPECT_EQ(Hash::FNV64("abcdefghijklmnopqrstuvwxyz"), 0x8450deb1cdc382a2);
    EXPECT_EQ(Hash::FNV64("ABCDEFGHIJKLMNOPQRSTUVWXYZ"), 0xe276b7953364d122);

}

TEST(Hash, FNV32Const) {

    // 空文字列のハッシュ
    EXPECT_EQ(AMUSE_FNV32(""), 0x811c9dc5);

    // 単一文字列のハッシュ
    EXPECT_EQ(AMUSE_FNV32("a"), 0xe40c292c);
    EXPECT_EQ(AMUSE_FNV32("b"), 0xe70c2de5);
    EXPECT_EQ(AMUSE_FNV32("c"), 0xe60c2c52);

    // 数字のみの文字列のハッシュ
    EXPECT_EQ(AMUSE_FNV32("123"), 0x7238631b);
    EXPECT_EQ(AMUSE_FNV32("456"), 0x1e377b3c);
    EXPECT_EQ(AMUSE_FNV32("789"), 0x91db9c09);

    // アルファベット文字列のハッシュ
    EXPECT_EQ(AMUSE_FNV32("abcdefghijklmnopqrstuvwxyz"), 0xb0bc0c82);
    EXPECT_EQ(AMUSE_FNV32("ABCDEFGHIJKLMNOPQRSTUVWXYZ"), 0x8a88dd82);

}

TEST(Hash, FNV64Const) {

    // 空文字列のハッシュ
    EXPECT_EQ(AMUSE_FNV64(""), 0xcbf29ce484222325);

    // 単一文字列のハッシュ
    EXPECT_EQ(AMUSE_FNV64("a"), 0xaf63dc4c8601ec8c);
    EXPECT_EQ(AMUSE_FNV64("b"), 0xaf63df4c8601f1a5);
    EXPECT_EQ(AMUSE_FNV64("c"), 0xaf63de4c8601eff2);

    // 数字のみの文字列のハッシュ
    EXPECT_EQ(AMUSE_FNV64("123"), 0x456fc2181822c4db);
    EXPECT_EQ(AMUSE_FNV64("456"), 0x2be33a1809c9a97c);
    EXPECT_EQ(AMUSE_FNV64("789"), 0x350a6a180f1c6dc9);

    // アルファベット文字列のハッシュ
    EXPECT_EQ(AMUSE_FNV64("abcdefghijklmnopqrstuvwxyz"), 0x8450deb1cdc382a2);
    EXPECT_EQ(AMUSE_FNV64("ABCDEFGHIJKLMNOPQRSTUVWXYZ"), 0xe276b7953364d122);

}

TEST(Hash, Combine) {

    // 単一の値からハッシュを取得できる
    {
        size_t hash = Hash::Combine(42);
        EXPECT_EQ(hash, Hash::Combine(42));
        EXPECT_NE(hash, Hash::Combine(100));
    }

    // 2つの値のどちらも結果に反映される
    {
        size_t hash = Hash::Combine(42, 100);
        EXPECT_EQ(hash, Hash::Combine(42, 100));
        EXPECT_NE(hash, Hash::Combine(43, 100));
        EXPECT_NE(hash, Hash::Combine(42, 101));
    }

    // 3つの値のすべてが結果に反映される
    {
        size_t hash = Hash::Combine(1, 2, 3);
        EXPECT_NE(hash, Hash::Combine(4, 2, 3));
        EXPECT_NE(hash, Hash::Combine(1, 4, 3));
        EXPECT_NE(hash, Hash::Combine(1, 2, 4));
    }

    // 異なる型の値もすべて結果に反映される
    {
        size_t hash = Hash::Combine(42, 3.14f, true, std::string("test"));
        EXPECT_EQ(hash, Hash::Combine(42, 3.14f, true, std::string("test")));
        EXPECT_NE(hash, Hash::Combine(43, 3.14f, true, std::string("test")));
        EXPECT_NE(hash, Hash::Combine(42, 2.71f, true, std::string("test")));
        EXPECT_NE(hash, Hash::Combine(42, 3.14f, false, std::string("test")));
        EXPECT_NE(hash, Hash::Combine(42, 3.14f, true, std::string("other")));
    }

    // 順序の違いで異なるハッシュ値になることを確認
    {
        EXPECT_NE(Hash::Combine(1, 2, 3), Hash::Combine(3, 2, 1));
    }

    // 多くの引数の末尾まで結果に反映される
    {
        size_t hash = Hash::Combine(1, 2, 3, 4, 5, 6);
        EXPECT_EQ(hash, Hash::Combine(1, 2, 3, 4, 5, 6));
        EXPECT_NE(hash, Hash::Combine(1, 2, 3, 4, 5, 7));
    }

    // size_tの変数も入力値として扱い、書き換えない
    {
        size_t value = 42;
        const size_t constValue = value;
        EXPECT_EQ(Hash::Combine(value), Hash::Combine(size_t{42}));
        EXPECT_EQ(Hash::Combine(value, 100), Hash::Combine(constValue, 100));
        EXPECT_EQ(Hash::Combine(value, 100, 200), Hash::Combine(size_t{42}, 100, 200));
        EXPECT_EQ(value, 42u);
    }
}
