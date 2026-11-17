//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/Core/Template/Container/BitFlags.h>
#include <Amuse/Core/Template/Utility/Utility.h>

using namespace Amuse;

enum class Flags : s32
{
    A= get_bit(0),
    B= get_bit(1),
    C= get_bit(2),
};

TEST(BitFlags, Constructor)
{
    {
        BitFlags<Flags> flags;
        EXPECT_EQ(static_cast<s32>(0), static_cast<s32>(flags));
    }
    {
        BitFlags<Flags> flags(Flags::A);
        EXPECT_EQ(static_cast<s32>(Flags::A), static_cast<s32>(flags));
    }
    {
        BitFlags<Flags> flags(static_cast<s32>(Flags::A));
        EXPECT_EQ(static_cast<s32>(Flags::A), static_cast<s32>(flags));
    }
    {
        BitFlags<Flags> flags(Flags::A);
        BitFlags<Flags> flags2(flags);
        EXPECT_EQ(static_cast<s32>(Flags::A), static_cast<s32>(flags2));
    }
    {
        BitFlags<Flags> flags(Flags::A);
        BitFlags<Flags> flags2;
        flags2 = flags;
        EXPECT_EQ(static_cast<s32>(Flags::A), static_cast<s32>(flags2));
    }
}

TEST(BitFlags, Operaor)
{
    {
        BitFlags<Flags> flags(Flags::A);
        flags |= Flags::B;
        EXPECT_EQ(static_cast<s32>(Flags::A | Flags::B), static_cast<s32>(flags));
    }
    {
        BitFlags<Flags> flags(Flags::A | Flags::B);
        BitFlags<Flags> flags2(Flags::A);
        flags &= flags2;
        EXPECT_EQ(static_cast<s32>(Flags::A), static_cast<s32>(flags));
    }
    {
        BitFlags<Flags> flags(Flags::A | Flags::B);
        BitFlags<Flags> flags2(Flags::A | Flags::C);
        flags ^= flags2;
        EXPECT_EQ(static_cast<s32>(Flags::B | Flags::C), static_cast<s32>(flags));
    }
    {
        BitFlags<Flags> flags(Flags::A | Flags::B);
        BitFlags<Flags> result = ~flags;
        EXPECT_EQ(static_cast<s32>(~static_cast<s32>(Flags::A | Flags::B)), static_cast<s32>(result));
    }
    {
        BitFlags<Flags> flags(Flags::A);
        EXPECT_TRUE(static_cast<bool>(flags));
        EXPECT_FALSE(!flags);
    }

}

TEST(BitFlags, HasFlag) {
    BitFlags<Flags> flag;

    flag.on(Flags::A);

    ASSERT_TRUE(flag.has(Flags::A));
    ASSERT_FALSE(flag.has(Flags::B));

    flag.off(Flags::A);

    ASSERT_FALSE(flag.has(Flags::A));
    ASSERT_FALSE(flag.has(Flags::B));

    flag.set(Flags::B, true);

    ASSERT_FALSE(flag.has(Flags::A));
    ASSERT_TRUE(flag.has(Flags::B));
}

TEST(BitFlags, Cast) {
    BitFlags<Flags> flag(3);

    ASSERT_TRUE(flag.has(Flags::A));
    ASSERT_TRUE(flag.has(Flags::B));
    ASSERT_FALSE(flag.has(Flags::C));
}
