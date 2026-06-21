//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/Core/Misc/Scheduler.h>
#include <atomic>
#include <fstream>

using namespace Amuse::Core;

TEST(Scheduler, DependencyOrder)
{
	Vector<s32> order;
	Schedule<Vector<s32>&> schedule("DependencyOrder");

	auto taskA = schedule.emplace([](Vector<s32>& out) { out.emplace_back(1); }).name("A");
	auto taskB = schedule.emplace([](Vector<s32>& out) { out.emplace_back(2); }).name("B");
	auto taskC = schedule.emplace([](Vector<s32>& out) { out.emplace_back(3); }).name("C");

	taskA.precede(taskB);
	taskB.precede(taskC);

	auto result = schedule.compile();
	ASSERT_TRUE(result);

	schedule.run(order);

	ASSERT_EQ(order.size(), 3);
	EXPECT_EQ(order[0], 1);
	EXPECT_EQ(order[1], 2);
	EXPECT_EQ(order[2], 3);
}

TEST(Scheduler, Chain)
{
	Vector<s32> order;
	Schedule<Vector<s32>&> schedule("Chain");

	auto taskA = schedule.emplace([](Vector<s32>& out) { out.emplace_back(1); });
	auto taskB = schedule.emplace([](Vector<s32>& out) { out.emplace_back(2); });
	auto taskC = schedule.emplace([](Vector<s32>& out) { out.emplace_back(3); });

	schedule.chain(taskA, taskB, taskC);

	ASSERT_TRUE(schedule.compile());
	schedule.run(order);

	EXPECT_EQ(order, (Vector<s32>{ 1, 2, 3 }));
}

TEST(Scheduler, DetectCycle)
{
	Schedule<> schedule("Cycle");

	auto taskA = schedule.emplace([] {});
	auto taskB = schedule.emplace([] {});

	taskA.precede(taskB);
	taskB.precede(taskA);

	auto result = schedule.compile();

	EXPECT_FALSE(result);
	ASSERT_FALSE(result.errors().empty());
	EXPECT_EQ(result.errors().front().type, ScheduleBuildErrorType::DependencyCycle);
}

TEST(Scheduler, DuplicateLabelWarning)
{
	Schedule<> schedule("DuplicateLabel");

	schedule.emplace([] {}).label("Shared");
	schedule.emplace([] {}).label("Shared");

	auto result = schedule.compile();

	EXPECT_TRUE(result);
	ASSERT_FALSE(result.warnings().empty());
	EXPECT_EQ(result.warnings().front().type, ScheduleBuildErrorType::DuplicateLabel);
}

TEST(Scheduler, SetOrder)
{
	Vector<s32> order;
	Schedule<Vector<s32>&> schedule("SetOrder");

	auto setA = schedule.configureSet("SetA");
	auto setB = schedule.configureSet("SetB");
	setA.precede(setB);

	schedule.emplace([](Vector<s32>& out) { out.emplace_back(1); }).inSet(setA);
	schedule.emplace([](Vector<s32>& out) { out.emplace_back(2); }).inSet(setB);

	ASSERT_TRUE(schedule.compile());
	schedule.run(order);

	EXPECT_EQ(order, (Vector<s32>{ 1, 2 }));
}

TEST(Scheduler, MultiThreadMatchesSingleThread)
{
	std::atomic<s32> value = 0;
	Schedule<std::atomic<s32>&> schedule("MultiThread");

	auto taskA = schedule.emplace([](std::atomic<s32>& v) { v.fetch_add(1); });
	auto taskB = schedule.emplace([](std::atomic<s32>& v) { v.fetch_add(10); });
	auto taskC = schedule.emplace([](std::atomic<s32>& v) { v.fetch_add(100); });
	schedule.chain(taskA, taskB, taskC);
	schedule.setExecutorKind(ExecutorKind::MultiThreaded);

	ASSERT_TRUE(schedule.compile());
	schedule.run(value);

	EXPECT_EQ(value.load(), 111);
}

TEST(Scheduler, ConflictTagPreventsSameBatch)
{
	std::atomic<s32> active = 0;
	std::atomic<s32> maxActive = 0;
	Schedule<std::atomic<s32>&, std::atomic<s32>&> schedule("Conflict");

	auto system = [](std::atomic<s32>& current, std::atomic<s32>& maxValue) {
		const s32 now = current.fetch_add(1) + 1;
		s32 old = maxValue.load();
		while (old < now && !maxValue.compare_exchange_weak(old, now)) {}
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
		current.fetch_sub(1);
	};

	schedule.emplace(system).conflict("WorldMutation");
	schedule.emplace(system).conflict("WorldMutation");
	schedule.setExecutorKind(ExecutorKind::MultiThreaded);

	ASSERT_TRUE(schedule.compile());
	schedule.run(active, maxActive);

	EXPECT_EQ(maxActive.load(), 1);
}

TEST(Scheduler, MainThread)
{
	std::thread::id mainThreadId = std::this_thread::get_id();
	std::thread::id executedThreadId;
	Schedule<std::thread::id&> schedule("MainThread");

	schedule.emplace([](std::thread::id& out) { out = std::this_thread::get_id(); }).mainThread();
	schedule.setExecutorKind(ExecutorKind::MultiThreaded);

	ASSERT_TRUE(schedule.compile());
	schedule.run(executedThreadId);

	EXPECT_EQ(executedThreadId, mainThreadId);
}

TEST(Scheduler, DumpDot)
{
	Schedule<> schedule("Dot");
	auto taskA = schedule.emplace([] {}).name("A");
	auto taskB = schedule.emplace([] {}).name("B");
	taskA.precede(taskB);

	ASSERT_TRUE(schedule.compile());

	const char* path = "Scheduler_Test.dot";
	ASSERT_TRUE(schedule.dumpDot(path));

	{
		std::ifstream in(path);
		ASSERT_TRUE(in);
		std::string text((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());

		EXPECT_NE(text.find("A"), std::string::npos);
		EXPECT_NE(text.find("B"), std::string::npos);
		EXPECT_NE(text.find("n0 -> n1"), std::string::npos);
	}

	File::Delete(path);
}
