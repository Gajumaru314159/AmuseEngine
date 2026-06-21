//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/Core/Allocator/Utility/SpanAllocator.h>

using namespace Amuse::Core;

TEST(SpanAllocator, AllocateExtendsLinearRange) {
	SpanAllocator allocator;

	EXPECT_EQ(allocator.allocate(), 0);
	EXPECT_EQ(allocator.allocate(3), 1);
	EXPECT_EQ(allocator.allocatedSize(), 4);
	EXPECT_EQ(allocator.maxSize(), 4);
	EXPECT_EQ(allocator.freeSpanCount(), 0);
}

TEST(SpanAllocator, ReusesFreedSpanAfterConsolidation) {
	SpanAllocator allocator;

	const s32 first = allocator.allocate(2);
	const s32 second = allocator.allocate(3);
	const s32 third = allocator.allocate(1);

	allocator.free(second, 3);

	EXPECT_EQ(allocator.pendingFreeSpanCount(), 1);
	EXPECT_TRUE(allocator.isFree(second));
	EXPECT_FALSE(allocator.isFree(first));
	EXPECT_FALSE(allocator.isFree(third));

	EXPECT_EQ(allocator.allocate(2), second);
	EXPECT_EQ(allocator.allocatedSize(), 5);
	EXPECT_EQ(allocator.maxSize(), 6);
	EXPECT_EQ(allocator.freeSpanCount(), 1);

	EXPECT_EQ(allocator.allocate(), second + 2);
	EXPECT_EQ(allocator.freeSpanCount(), 1);
}

TEST(SpanAllocator, ConsolidateMergesAdjacentFreeSpansAndShrinksTail) {
	SpanAllocator allocator;

	const s32 first = allocator.allocate(2);
	const s32 second = allocator.allocate(3);
	const s32 third = allocator.allocate(4);

	allocator.free(second, 3);
	allocator.free(third, 4);
	allocator.consolidate();

	EXPECT_EQ(allocator.allocatedSize(), 2);
	EXPECT_EQ(allocator.maxSize(), 2);
	EXPECT_EQ(allocator.freeSpanCount(), 0);

	allocator.free(first, 2);
	allocator.consolidate();

	EXPECT_EQ(allocator.allocatedSize(), 0);
	EXPECT_EQ(allocator.maxSize(), 0);
}

TEST(SpanAllocator, ConsolidateKeepsInteriorMergedSpan) {
	SpanAllocator allocator;

	const s32 first = allocator.allocate(2);
	const s32 second = allocator.allocate(3);
	const s32 third = allocator.allocate(4);
	const s32 fourth = allocator.allocate(1);

	allocator.free(second, 3);
	allocator.free(third, 4);
	allocator.consolidate();

	EXPECT_EQ(allocator.allocatedSize(), 3);
	EXPECT_EQ(allocator.maxSize(), 10);
	EXPECT_EQ(allocator.freeSpanCount(), 1);
	EXPECT_EQ(allocator.allocate(7), second);
	EXPECT_EQ(first, 0);
	EXPECT_EQ(fourth, 9);
}

TEST(SpanAllocator, GrowOnlyKeepsPeakMaxSizeUntilReset) {
	SpanAllocator allocator(true);

	const s32 first = allocator.allocate(2);
	const s32 second = allocator.allocate(5);

	allocator.free(second, 5);
	allocator.consolidate();

	EXPECT_EQ(allocator.allocatedSize(), 2);
	EXPECT_EQ(allocator.maxSize(), 7);

	allocator.free(first, 2);
	allocator.consolidate();

	EXPECT_EQ(allocator.allocatedSize(), 0);
	EXPECT_EQ(allocator.maxSize(), 7);

	allocator.reset();

	EXPECT_EQ(allocator.allocatedSize(), 0);
	EXPECT_EQ(allocator.maxSize(), 0);
}

TEST(SpanAllocator, EmptyClearsState) {
	SpanAllocator allocator;

	allocator.allocate(8);
	allocator.free(2, 3);
	allocator.empty();

	EXPECT_EQ(allocator.allocatedSize(), 0);
	EXPECT_EQ(allocator.maxSize(), 0);
	EXPECT_EQ(allocator.freeSpanCount(), 0);
	EXPECT_EQ(allocator.pendingFreeSpanCount(), 0);
	EXPECT_TRUE(allocator.isFree(0));
}
