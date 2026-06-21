//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/CorePrivate.h>
#include <Amuse/Core/String/String.h>
#include <Amuse/Core/String/StringView.h>
#include <atomic>
#include <condition_variable>
#include <exception>
#include <fstream>
#include <mutex>
#include <queue>
#include <set>
#include <thread>
#include <unordered_map>
#include <unordered_set>

namespace Amuse::Core {

	enum class ExecutorKind {
		SingleThreaded,
		MultiThreaded,
	};

	//! @brief スケジュール実行設定を表す。
	struct ScheduleExecutorConfig {
		s32 workerCount = 0; //!< ワーカー数
		bool allowWorkerSleep = true; //!< ワーカースリープ許可
		bool enableWorkStealing = true; //!< ワークスティール有効フラグ
	};

	enum class ScheduleBuildLogLevel {
		Ignore,
		Warn,
		Error,
	};

	enum class ScheduleBuildErrorType {
		DuplicateLabel,
		UnresolvedLabel,
		DependencyCycle,
		InvalidSetHierarchy,
		InvalidNode,
	};

	//! @brief スケジュール構築エラーを表す。
	struct ScheduleBuildError {
		ScheduleBuildErrorType type{}; //!< 型
		String message; //!< メッセージ
		Vector<String> nodes; //!< 対象ノード一覧
	};

	//! @brief スケジュール構築警告を表す。
	struct ScheduleBuildWarning {
		ScheduleBuildErrorType type{}; //!< 型
		String message; //!< メッセージ
		Vector<String> nodes; //!< 対象ノード一覧
	};

	//! @brief スケジュール構築設定を表す。
	struct ScheduleBuildSettings {
		ScheduleBuildLogLevel unresolvedLabel = ScheduleBuildLogLevel::Error; //!< 未解決ラベル検証フラグ
		ScheduleBuildLogLevel dependencyCycle = ScheduleBuildLogLevel::Error; //!< 依存循環検証フラグ
		bool validateConflictTag = true; //!< 競合タグ検証フラグ
		bool stableTopologicalSort = true; //!< 安定トポロジカルソートフラグ
	};

	//! @brief スケジュール構築結果を表す。
	class ScheduleBuildResult {
	public:
		//! @brief ビルドが成功したか
		explicit operator bool() const { return m_errors.empty(); }

		//! @brief エラー一覧を取得
		const Vector<ScheduleBuildError>& errors() const { return m_errors; }

		//! @brief 警告一覧を取得
		const Vector<ScheduleBuildWarning>& warnings() const { return m_warnings; }

		//! @brief エラーを追加
		void addError(ScheduleBuildError error) { m_errors.emplace_back(std::move(error)); }

		//! @brief 警告を追加
		void addWarning(ScheduleBuildWarning warning) { m_warnings.emplace_back(std::move(warning)); }

	private:
		Vector<ScheduleBuildError> m_errors;
		Vector<ScheduleBuildWarning> m_warnings;
	};

	//! @brief スケジュール実行器を表す。
	class ScheduleExecutor {
	public:
		//! @brief Executor を作成
		//! @param config ワーカースレッド数などの設定
		explicit ScheduleExecutor(const ScheduleExecutorConfig& config = {}) {
			s32 count = config.workerCount;
			if (count <= 0) {
				count = static_cast<s32>(std::thread::hardware_concurrency());
				if (count <= 0) count = 1;
			}
			m_running = true;
			for (s32 i = 0; i < count; ++i) {
				m_workers.emplace_back([this] { process(); });
			}
		}

		//! @brief デストラクタ
		//! @details 実行中のジョブがあれば完了を待って停止する。
		~ScheduleExecutor() {
			wait();
			{
				std::lock_guard lock(m_mutex);
				m_running = false;
			}
			m_condition.notify_all();
			for (auto& worker : m_workers) {
				if (worker.joinable()) worker.join();
			}
		}

		ScheduleExecutor(const ScheduleExecutor&) = delete;
		ScheduleExecutor& operator=(const ScheduleExecutor&) = delete;

		//! @brief ワーカースレッド数を取得
		auto workerCount() const -> s32 { return static_cast<s32>(m_workers.size()); }

		//! @brief ジョブを投入
		//! @param action 実行する処理
		void enqueue(Action&& action) {
			{
				std::lock_guard lock(m_mutex);
				++m_pendingCount;
				m_queue.emplace(std::move(action));
			}
			m_condition.notify_one();
		}

		//! @brief 投入済みジョブの完了を待機
		void wait() {
			std::unique_lock lock(m_mutex);
			m_doneCondition.wait(lock, [this] { return m_pendingCount == 0 && m_queue.empty(); });
		}

		//! @brief 現在のスレッドがこの Executor のワーカーか判定
		auto isWorkerThread() const -> bool { return t_currentExecutor == this; }

	private:
		//! @brief ワーカースレッドの処理ループ
		void process() {
			t_currentExecutor = this;
			while (true) {
				Action action;
				{
					std::unique_lock lock(m_mutex);
					m_condition.wait(lock, [this] { return !m_queue.empty() || !m_running; });
					if (!m_running && m_queue.empty()) break;
					action = std::move(m_queue.front());
					m_queue.pop();
				}
				try {
					action();
				} catch (...) {
					std::lock_guard lock(m_mutex);
					if (!m_exception) m_exception = std::current_exception();
				}
				{
					std::lock_guard lock(m_mutex);
					--m_pendingCount;
					if (m_pendingCount == 0 && m_queue.empty()) m_doneCondition.notify_all();
				}
			}
			t_currentExecutor = nullptr;
		}

	private:
		inline static thread_local const ScheduleExecutor* t_currentExecutor = nullptr;
		mutable std::mutex m_mutex;
		std::condition_variable m_condition;
		std::condition_variable m_doneCondition;
		bool m_running = false;
		s32 m_pendingCount = 0;
		std::queue<Action> m_queue;
		Vector<std::thread> m_workers;
		std::exception_ptr m_exception;
	};

	//! @cond
	namespace Internal {
		enum class ScheduleNodeKind {
			Task,
			Set,
		};

		//! @brief スケジュールノード参照を表す。
		struct ScheduleNodeRef {
			ScheduleNodeKind kind = ScheduleNodeKind::Task; //!< ノード種別
			s32 id = -1; //!< ノードID

			//! @brief 等値比較を行う。
			bool operator==(const ScheduleNodeRef& rhs) const {
				return kind == rhs.kind && id == rhs.id;
			}
		};

		//! @brief スケジュールノード間の依存辺を表す。
		struct ScheduleEdge {
			ScheduleNodeRef from; //!< 依存元ノード
			ScheduleNodeRef to; //!< 依存先ノード
		};
	}
	//! @endcond

	class ScheduleSet;
	class TaskGroup;

	//! @brief 非同期タスクを表す。
	class Task {
	public:
		//! @brief 空のタスクハンドルを作成
		Task() = default;

		//! @brief 表示名を設定
		auto name(StringView name) -> Task& {
			m_setName(*this, name);
			return *this;
		}

		//! @brief ラベルを追加
		//! @details ラベルは診断や可視化用であり、システム間依存の解決には使わない。
		template<class Label>
		auto label(Label&& label) -> Task& {
			m_addLabel(*this, StringView(label));
			return *this;
		}

		//! @brief セットへ所属させる
		//! @param set 所属先セット
		auto inSet(const ScheduleSet& set) -> Task&;

		//! @brief このタスクを指定タスクまたはセットより前に実行
		template<class... Nodes>
		auto precede(const Nodes&... nodes) -> Task& {
			((validateSameOwner(nodes), m_addEdge(m_owner, ref(), nodes.ref())), ...);
			return *this;
		}

		//! @brief このタスクを指定タスクまたはセットより後に実行
		template<class... Nodes>
		auto succeed(const Nodes&... nodes) -> Task& {
			((validateSameOwner(nodes), m_addEdge(m_owner, nodes.ref(), ref())), ...);
			return *this;
		}

		//! @brief 排他タグを追加
		//! @details 同じ排他タグを持つシステム同士は同時実行されない。
		template<class Label>
		auto conflict(Label&& label) -> Task& {
			m_addConflict(*this, StringView(label));
			return *this;
		}

		//! @brief メインスレッド実行を指定
		auto mainThread() -> Task& {
			m_setMainThread(*this);
			return *this;
		}

		//! @brief 並列実行を禁止
		auto exclusive() -> Task& {
			m_setExclusive(*this);
			return *this;
		}

	private:
		template<class... Args>
		friend class Schedule;
		friend class ScheduleSet;
		friend class TaskGroup;

		using SetNameFunc = void(*)(const Task&, StringView);
		using AddStringFunc = void(*)(const Task&, StringView);
		using SetFlagFunc = void(*)(const Task&);
		using AddSetFunc = void(*)(const Task&, const ScheduleSet&);
		using AddEdgeFunc = void(*)(void*, Internal::ScheduleNodeRef, Internal::ScheduleNodeRef);

		Task(void* owner, s32 id, SetNameFunc setName, AddStringFunc addLabel, AddStringFunc addConflict, SetFlagFunc setMainThread, SetFlagFunc setExclusive, AddSetFunc addSet, AddEdgeFunc addEdge)
			: m_owner(owner)
			, m_ref{ Internal::ScheduleNodeKind::Task, id }
			, m_setName(setName)
			, m_addLabel(addLabel)
			, m_addConflict(addConflict)
			, m_setMainThread(setMainThread)
			, m_setExclusive(setExclusive)
			, m_addSet(addSet)
			, m_addEdge(addEdge) {
		}

		auto ref() const -> Internal::ScheduleNodeRef { return m_ref; }
		void validate() const {
			if (!m_owner) throw std::runtime_error("invalid schedule task");
		}

		template<class Node>
		void validateSameOwner(const Node& node) const {
			validate();
			node.validate();
			if (m_owner != node.m_owner) throw std::invalid_argument("schedule nodes belong to different schedules");
		}

	private:
		void* m_owner = nullptr;
		Internal::ScheduleNodeRef m_ref{};
		SetNameFunc m_setName = [](const Task&, StringView) {};
		AddStringFunc m_addLabel = [](const Task&, StringView) {};
		AddStringFunc m_addConflict = [](const Task&, StringView) {};
		SetFlagFunc m_setMainThread = [](const Task&) {};
		SetFlagFunc m_setExclusive = [](const Task&) {};
		AddSetFunc m_addSet = [](const Task&, const ScheduleSet&) {};
		AddEdgeFunc m_addEdge = [](void*, Internal::ScheduleNodeRef, Internal::ScheduleNodeRef) {};
	};

	//! @brief スケジュール集合を表す。
	class ScheduleSet {
	public:
		//! @brief 空のセットハンドルを作成
		ScheduleSet() = default;

		//! @brief 親セットを設定
		//! @param parent 親セット
		auto inSet(const ScheduleSet& parent) -> ScheduleSet& {
			m_addParent(*this, parent);
			return *this;
		}

		//! @brief このセットを指定セットまたはタスクより前に実行
		template<class... Nodes>
		auto precede(const Nodes&... nodes) -> ScheduleSet& {
			((validateSameOwner(nodes), m_addEdge(m_owner, ref(), nodes.ref())), ...);
			return *this;
		}

		//! @brief このセットを指定セットまたはタスクより後に実行
		template<class... Nodes>
		auto succeed(const Nodes&... nodes) -> ScheduleSet& {
			((validateSameOwner(nodes), m_addEdge(m_owner, nodes.ref(), ref())), ...);
			return *this;
		}

		//! @brief セット内システムの登録順を依存順として扱う
		auto chain() -> ScheduleSet& {
			m_setChain(*this);
			return *this;
		}

	private:
		template<class... Args>
		friend class Schedule;
		friend class Task;
		friend class TaskGroup;

		using AddParentFunc = void(*)(const ScheduleSet&, const ScheduleSet&);
		using SetChainFunc = void(*)(const ScheduleSet&);
		using AddEdgeFunc = void(*)(void*, Internal::ScheduleNodeRef, Internal::ScheduleNodeRef);

		ScheduleSet(void* owner, s32 id, AddParentFunc addParent, SetChainFunc setChain, AddEdgeFunc addEdge)
			: m_owner(owner)
			, m_ref{ Internal::ScheduleNodeKind::Set, id }
			, m_addParent(addParent)
			, m_setChain(setChain)
			, m_addEdge(addEdge) {
		}

		auto ref() const -> Internal::ScheduleNodeRef { return m_ref; }
		void validate() const {
			if (!m_owner) throw std::runtime_error("invalid schedule set");
		}

		template<class Node>
		void validateSameOwner(const Node& node) const {
			validate();
			node.validate();
			if (m_owner != node.m_owner) throw std::invalid_argument("schedule nodes belong to different schedules");
		}

	private:
		void* m_owner = nullptr;
		Internal::ScheduleNodeRef m_ref{};
		AddParentFunc m_addParent = [](const ScheduleSet&, const ScheduleSet&) {};
		SetChainFunc m_setChain = [](const ScheduleSet&) {};
		AddEdgeFunc m_addEdge = [](void*, Internal::ScheduleNodeRef, Internal::ScheduleNodeRef) {};
	};

	inline auto Task::inSet(const ScheduleSet& set) -> Task& {
		m_addSet(*this, set);
		return *this;
	}

	//! @brief タスクグループを表す。
	class TaskGroup {
	public:
		//! @brief 空のタスクグループを作成
		TaskGroup() = default;

		//! @brief グループ内のタスクを登録順に直列化
		auto chain() -> TaskGroup& {
			for (size_t i = 1; i < m_tasks.size(); ++i) {
				m_tasks[i - 1].precede(m_tasks[i]);
			}
			return *this;
		}

		//! @brief グループ内のすべてのタスクをセットに所属させる
		auto inSet(const ScheduleSet& set) -> TaskGroup& {
			for (auto& task : m_tasks) task.inSet(set);
			return *this;
		}

		//! @brief 登録済みタスク一覧を取得
		const Vector<Task>& tasks() const { return m_tasks; }

	private:
		template<class... Args>
		friend class Schedule;

		explicit TaskGroup(Vector<Task> tasks)
			: m_tasks(std::move(tasks)) {
		}

	private:
		Vector<Task> m_tasks;
	};

	template<class... Args>
	//! @brief スケジュールを表す。
	class Schedule {
	public:
		//! @brief スケジュールを作成
		//! @param name 診断とプロファイルで使用する名前
		explicit Schedule(StringView name = {})
			: m_name(name) {
		}

		//! @brief システムを登録
		//! @param system 登録する callable
		//! @return 登録したシステムの設定オブジェクト
		template<class F>
		auto emplace(F&& system) -> Task {
			SystemNode node;
			node.id = static_cast<s32>(m_systems.size());
			node.name = makeDefaultTaskName(node.id);
			node.invoker = [fn = std::forward<F>(system)](Args... args) mutable {
				std::invoke(fn, args...);
			};
			m_systems.emplace_back(std::move(node));
			m_dirty = true;
			return makeTask(m_systems.back().id);
		}

		//! @brief 複数システムを登録
		//! @param systems 登録する callable 群
		//! @return 登録したシステム群の設定オブジェクト
		template<class... F>
		auto addSystems(F&&... systems) -> TaskGroup {
			Vector<Task> tasks;
			tasks.reserve(sizeof...(F));
			(tasks.emplace_back(emplace(std::forward<F>(systems))), ...);
			return TaskGroup(std::move(tasks));
		}

		//! @brief 指定セットへ複数システムを登録
		//! @param set 所属先セット
		//! @param systems 登録する callable 群
		template<class... F>
		auto addSystems(const ScheduleSet& set, F&&... systems) -> TaskGroup {
			auto group = addSystems(std::forward<F>(systems)...);
			group.inSet(set);
			return group;
		}

		//! @brief セット設定を取得または作成
		//! @param setName セット名
		//! @details セット名は診断と可視化の表示名にも使用する。
		auto configureSet(StringView setName) -> ScheduleSet {
			const String name(setName);
			for (auto& set : m_sets) {
				if (set.name == name) return makeSet(set.id);
			}
			SetNode node;
			node.id = static_cast<s32>(m_sets.size());
			node.name = name;
			m_sets.emplace_back(std::move(node));
			m_dirty = true;
			return makeSet(m_sets.back().id);
		}

		//! @brief スケジュールを 1 ノードとして合成
		//! @param child 合成する子スケジュール
		auto compose(Schedule& child) -> Task {
			auto task = emplace([&child](Args... args) {
				child.run(args...);
			});
			task.name(child.m_name.empty() ? StringView("Schedule") : StringView(child.m_name));
			return task;
		}

		//! @brief 直列実行制約を追加
		template<class... Nodes>
		void chain(const Nodes&... nodes) {
			Vector<Internal::ScheduleNodeRef> refs{ nodes.ref()... };
			for (size_t i = 1; i < refs.size(); ++i) {
				addEdge(refs[i - 1], refs[i]);
			}
		}

		//! @brief スケジュールを実行可能な形式にコンパイル
		auto compile() -> ScheduleBuildResult {
			ScheduleBuildResult result;
			m_compiledEdges.clear();
			m_order.clear();
			m_batches.clear();
			m_lastBuildSucceeded = false;

			validateLabels(result);
			expandSetHierarchy(result);
			expandEdges(result);
			if (!result) return result;

			buildExecutionPlan(result);
			m_lastBuildSucceeded = static_cast<bool>(result);
			m_dirty = !m_lastBuildSucceeded;
			return result;
		}

		//! @brief スケジュールを実行
		//! @param args システムへ渡す引数
		void run(Args... args) {
			if (m_dirty || !m_lastBuildSucceeded) {
				auto result = compile();
				if (!result) {
					throw std::runtime_error("schedule compile failed");
				}
			}

			if (m_executorKind == ExecutorKind::SingleThreaded || m_systems.size() <= 1) {
				runSingleThreaded(args...);
			} else {
				runMultiThreaded(args...);
			}
		}

		//! @brief Executor 種別を設定
		void setExecutorKind(ExecutorKind kind) {
			m_executorKind = kind;
		}

		//! @brief 並列実行に使用する Executor を設定
		//! @details 未設定の場合はデフォルト Executor を使用する。
		void setExecutor(ScheduleExecutor& executor) {
			m_executor = &executor;
		}

		//! @brief Graphviz DOT 形式でグラフを出力
		bool dumpDot(StringView path) const {
			const std::string outPath{ std::string_view(path) };
			std::ofstream out(outPath);
			if (!out) return false;
			out << "digraph \"" << escapeDot(m_name) << "\" {\n";
			for (const auto& system : m_systems) {
				out << "  n" << system.id << " [label=\"" << escapeDot(system.name) << "\"];\n";
			}
			const auto& edges = m_compiledEdges.empty() ? m_explicitEdges : m_compiledEdges;
			for (const auto& edge : edges) {
				if (edge.from.kind == Internal::ScheduleNodeKind::Task && edge.to.kind == Internal::ScheduleNodeKind::Task) {
					out << "  n" << edge.from.id << " -> n" << edge.to.id << ";\n";
				}
			}
			out << "}\n";
			return true;
		}

	private:
		struct SystemNode {
			s32 id = -1;
			String name;
			Vector<String> labels;
			Vector<s32> sets;
			Vector<String> conflicts;
			bool mainThread = false;
			bool exclusive = false;
			Func<void(Args...)> invoker;
		};

		struct SetNode {
			s32 id = -1;
			String name;
			Vector<s32> parents;
			bool chain = false;
		};

		struct ExecutionBatch {
			Vector<s32> systems;
		};

	private:
		//! @brief タスクハンドルを作成
		auto makeTask(s32 id) -> Task {
			return Task(
				this,
				id,
				&Schedule::setTaskName,
				&Schedule::addTaskLabel,
				&Schedule::addTaskConflict,
				&Schedule::setTaskMainThread,
				&Schedule::setTaskExclusive,
				&Schedule::addTaskSet,
				&Schedule::addStaticEdge
			);
		}

		//! @brief セットハンドルを作成
		auto makeSet(s32 id) -> ScheduleSet {
			return ScheduleSet(
				this,
				id,
				&Schedule::addSetParent,
				&Schedule::setSetChain,
				&Schedule::addStaticEdge
			);
		}

		//! @brief タスク名を設定
		static void setTaskName(const Task& task, StringView name) {
			auto& self = ownerFromTask(task);
			self.system(task.ref().id).name = String(name);
			self.m_dirty = true;
		}

		//! @brief タスクラベルを追加
		static void addTaskLabel(const Task& task, StringView label) {
			auto& self = ownerFromTask(task);
			self.system(task.ref().id).labels.emplace_back(label);
			self.m_dirty = true;
		}

		//! @brief 排他タグを追加
		static void addTaskConflict(const Task& task, StringView label) {
			auto& self = ownerFromTask(task);
			self.system(task.ref().id).conflicts.emplace_back(label);
			self.m_dirty = true;
		}

		//! @brief メインスレッド実行を設定
		static void setTaskMainThread(const Task& task) {
			auto& self = ownerFromTask(task);
			self.system(task.ref().id).mainThread = true;
			self.m_dirty = true;
		}

		//! @brief 排他実行を設定
		static void setTaskExclusive(const Task& task) {
			auto& self = ownerFromTask(task);
			self.system(task.ref().id).exclusive = true;
			self.m_dirty = true;
		}

		//! @brief タスクをセットへ追加
		static void addTaskSet(const Task& task, const ScheduleSet& set) {
			auto& self = ownerFromTask(task);
			if (task.m_owner != set.m_owner) throw std::invalid_argument("task and set belong to different schedules");
			auto& system = self.system(task.ref().id);
			const s32 setId = set.ref().id;
			if (std::find(system.sets.begin(), system.sets.end(), setId) == system.sets.end()) {
				system.sets.emplace_back(setId);
			}
			self.m_dirty = true;
		}

		//! @brief セットに親セットを追加
		static void addSetParent(const ScheduleSet& set, const ScheduleSet& parent) {
			auto& self = ownerFromSet(set);
			if (set.m_owner != parent.m_owner) throw std::invalid_argument("sets belong to different schedules");
			auto& node = self.set(set.ref().id);
			const s32 parentId = parent.ref().id;
			if (std::find(node.parents.begin(), node.parents.end(), parentId) == node.parents.end()) {
				node.parents.emplace_back(parentId);
			}
			self.m_dirty = true;
		}

		//! @brief セット内 chain を有効化
		static void setSetChain(const ScheduleSet& set) {
			auto& self = ownerFromSet(set);
			self.set(set.ref().id).chain = true;
			self.m_dirty = true;
		}

		//! @brief ハンドルから依存エッジを追加
		static void addStaticEdge(void* owner, Internal::ScheduleNodeRef from, Internal::ScheduleNodeRef to) {
			if (!owner) throw std::runtime_error("schedule edge owner is not set");
			static_cast<Schedule*>(owner)->addEdge(from, to);
		}

		//! @brief タスクハンドルから Schedule を取得
		static Schedule& ownerFromTask(const Task& task) {
			task.validate();
			return *static_cast<Schedule*>(task.m_owner);
		}

		//! @brief セットハンドルから Schedule を取得
		static Schedule& ownerFromSet(const ScheduleSet& set) {
			set.validate();
			return *static_cast<Schedule*>(set.m_owner);
		}

		//! @brief エッジを追加
		void addEdge(Internal::ScheduleNodeRef from, Internal::ScheduleNodeRef to) {
			if (!isValidRef(from) || !isValidRef(to)) throw std::invalid_argument("invalid schedule dependency node");
			m_explicitEdges.emplace_back(from, to);
			m_dirty = true;
		}

		//! @brief システムを取得
		auto system(s32 id) -> SystemNode& {
			if (id < 0 || static_cast<size_t>(id) >= m_systems.size()) throw std::out_of_range("invalid schedule task id");
			return m_systems[static_cast<size_t>(id)];
		}

		//! @brief セットを取得
		auto set(s32 id) -> SetNode& {
			if (id < 0 || static_cast<size_t>(id) >= m_sets.size()) throw std::out_of_range("invalid schedule set id");
			return m_sets[static_cast<size_t>(id)];
		}

		//! @brief ノード参照が有効か
		bool isValidRef(Internal::ScheduleNodeRef ref) const {
			if (ref.kind == Internal::ScheduleNodeKind::Task) {
				return 0 <= ref.id && static_cast<size_t>(ref.id) < m_systems.size();
			}
			return 0 <= ref.id && static_cast<size_t>(ref.id) < m_sets.size();
		}

		//! @brief デフォルトタスク名を作成
		static auto makeDefaultTaskName(s32 id) -> String {
			return String(std::string("Task") + std::to_string(id));
		}

		//! @brief ラベル重複を検証
		void validateLabels(ScheduleBuildResult& result) const {
			std::unordered_map<std::string, Vector<String>> labels;
			for (const auto& system : m_systems) {
				for (const auto& label : system.labels) {
					labels[std::string(label.c_str())].emplace_back(system.name);
				}
			}
			for (const auto& [label, nodes] : labels) {
				if (nodes.size() <= 1) continue;
				result.addWarning({ ScheduleBuildErrorType::DuplicateLabel, String("duplicate schedule label: ") + label.c_str(), nodes });
			}
		}

		//! @brief セット階層を所属システムへ展開
		void expandSetHierarchy(ScheduleBuildResult& result) {
			const size_t setCount = m_sets.size();
			Vector<Vector<s32>> children(setCount);
			for (const auto& set : m_sets) {
				for (s32 parent : set.parents) {
					if (parent == set.id) {
						result.addError({ ScheduleBuildErrorType::InvalidSetHierarchy, "set cannot contain itself", { set.name } });
					} else if (parent < 0 || static_cast<size_t>(parent) >= setCount) {
						result.addError({ ScheduleBuildErrorType::InvalidSetHierarchy, "invalid parent set", { set.name } });
					} else {
						children[static_cast<size_t>(parent)].emplace_back(set.id);
					}
				}
			}
			if (!result) return;

			Vector<s32> state(setCount, 0);
			for (const auto& set : m_sets) {
				if (hasSetCycle(set.id, children, state)) {
					result.addError({ ScheduleBuildErrorType::InvalidSetHierarchy, "set hierarchy has cycle", { set.name } });
					return;
				}
			}

			for (auto& system : m_systems) {
				Vector<s32> expanded = system.sets;
				for (s32 setId : system.sets) {
					collectParentSets(setId, expanded);
				}
				std::sort(expanded.begin(), expanded.end());
				expanded.erase(std::unique(expanded.begin(), expanded.end()), expanded.end());
				system.sets = std::move(expanded);
			}
		}

		//! @brief セット階層に循環があるか判定
		bool hasSetCycle(s32 setId, const Vector<Vector<s32>>& children, Vector<s32>& state) const {
			auto index = static_cast<size_t>(setId);
			if (state[index] == 1) return true;
			if (state[index] == 2) return false;
			state[index] = 1;
			for (s32 child : children[index]) {
				if (hasSetCycle(child, children, state)) return true;
			}
			state[index] = 2;
			return false;
		}

		//! @brief 親セットを再帰的に収集
		void collectParentSets(s32 setId, Vector<s32>& out) const {
			for (s32 parent : m_sets[static_cast<size_t>(setId)].parents) {
				out.emplace_back(parent);
				collectParentSets(parent, out);
			}
		}

		//! @brief セット依存をタスク依存へ展開
		void expandEdges(ScheduleBuildResult& result) {
			std::set<std::pair<s32, s32>> edges;
			for (const auto& edge : m_explicitEdges) {
				Vector<s32> fromSystems = resolveRef(edge.from);
				Vector<s32> toSystems = resolveRef(edge.to);
				if (fromSystems.empty() || toSystems.empty()) continue;
				for (s32 from : fromSystems) {
					for (s32 to : toSystems) {
						if (from == to) continue;
						edges.emplace(from, to);
					}
				}
			}

			for (const auto& set : m_sets) {
				if (!set.chain) continue;
				Vector<s32> systemsInSet = resolveRef({ Internal::ScheduleNodeKind::Set, set.id });
				std::sort(systemsInSet.begin(), systemsInSet.end());
				for (size_t i = 1; i < systemsInSet.size(); ++i) {
					edges.emplace(systemsInSet[i - 1], systemsInSet[i]);
				}
			}

			for (const auto& [from, to] : edges) {
				m_compiledEdges.emplace_back(
					Internal::ScheduleNodeRef{ Internal::ScheduleNodeKind::Task, from },
					Internal::ScheduleNodeRef{ Internal::ScheduleNodeKind::Task, to }
				);
			}
			(void)result;
		}

		//! @brief ノード参照をシステム ID 一覧へ変換
		auto resolveRef(Internal::ScheduleNodeRef ref) const -> Vector<s32> {
			Vector<s32> systems;
			if (ref.kind == Internal::ScheduleNodeKind::Task) {
				if (isValidRef(ref)) systems.emplace_back(ref.id);
				return systems;
			}
			for (const auto& system : m_systems) {
				if (std::find(system.sets.begin(), system.sets.end(), ref.id) != system.sets.end()) {
					systems.emplace_back(system.id);
				}
			}
			return systems;
		}

		//! @brief 実行計画を作成
		void buildExecutionPlan(ScheduleBuildResult& result) {
			const size_t count = m_systems.size();
			Vector<Vector<s32>> outgoing(count);
			Vector<s32> indegree(count, 0);
			for (const auto& edge : m_compiledEdges) {
				const s32 from = edge.from.id;
				const s32 to = edge.to.id;
				outgoing[static_cast<size_t>(from)].emplace_back(to);
				++indegree[static_cast<size_t>(to)];
			}

			Vector<s32> ready;
			for (const auto& system : m_systems) {
				if (indegree[static_cast<size_t>(system.id)] == 0) ready.emplace_back(system.id);
			}

			Vector<s32> indegreeForBatch = indegree;
			while (!ready.empty()) {
				if (m_buildSettings.stableTopologicalSort) std::sort(ready.begin(), ready.end());

				Vector<s32> batchSystems;
				Vector<s32> deferred;
				for (s32 id : ready) {
					if (canAddToBatch(batchSystems, id)) {
						batchSystems.emplace_back(id);
					} else {
						deferred.emplace_back(id);
					}
				}
				if (batchSystems.empty()) {
					batchSystems.emplace_back(deferred.front());
					deferred.erase(deferred.begin());
				}

				ExecutionBatch batch;
				batch.systems = batchSystems;
				m_batches.emplace_back(std::move(batch));
				for (s32 id : batchSystems) m_order.emplace_back(id);

				Vector<s32> next = deferred;
				for (s32 id : batchSystems) {
					for (s32 to : outgoing[static_cast<size_t>(id)]) {
						auto& value = indegreeForBatch[static_cast<size_t>(to)];
						--value;
						if (value == 0) next.emplace_back(to);
					}
				}
				ready = std::move(next);
			}

			if (m_order.size() != count) {
				Vector<String> nodes;
				for (const auto& system : m_systems) {
					if (std::find(m_order.begin(), m_order.end(), system.id) == m_order.end()) {
						nodes.emplace_back(system.name);
					}
				}
				result.addError({ ScheduleBuildErrorType::DependencyCycle, "schedule dependency cycle detected", nodes });
			}
		}

		//! @brief バッチに追加できるか判定
		bool canAddToBatch(const Vector<s32>& batch, s32 id) const {
			const auto& target = m_systems[static_cast<size_t>(id)];
			for (s32 otherId : batch) {
				const auto& other = m_systems[static_cast<size_t>(otherId)];
				if (target.exclusive || other.exclusive) return false;
				for (const auto& tag : target.conflicts) {
					if (std::find(other.conflicts.begin(), other.conflicts.end(), tag) != other.conflicts.end()) return false;
				}
			}
			return true;
		}

		//! @brief 単一スレッドで実行
		void runSingleThreaded(Args... args) {
			for (s32 id : m_order) {
				m_systems[static_cast<size_t>(id)].invoker(args...);
			}
		}

		//! @brief マルチスレッドで実行
		void runMultiThreaded(Args... args) {
			ScheduleExecutor localExecutor;
			ScheduleExecutor& executor = m_executor ? *m_executor : localExecutor;
			std::exception_ptr firstException;
			std::mutex exceptionMutex;

			for (const auto& batch : m_batches) {
				for (s32 id : batch.systems) {
					auto& system = m_systems[static_cast<size_t>(id)];
					if (system.mainThread) continue;
					executor.enqueue([&system, &args..., &firstException, &exceptionMutex] {
						try {
							system.invoker(args...);
						} catch (...) {
							std::lock_guard lock(exceptionMutex);
							if (!firstException) firstException = std::current_exception();
						}
					});
				}

				for (s32 id : batch.systems) {
					auto& system = m_systems[static_cast<size_t>(id)];
					if (!system.mainThread) continue;
					try {
						system.invoker(args...);
					} catch (...) {
						std::lock_guard lock(exceptionMutex);
						if (!firstException) firstException = std::current_exception();
					}
				}

				executor.wait();
				if (firstException) std::rethrow_exception(firstException);
			}
		}

		//! @brief DOT 用の文字列へエスケープ
		static auto escapeDot(StringView value) -> std::string {
			std::string result;
			for (char c : std::string_view(value)) {
				if (c == '"' || c == '\\') result.push_back('\\');
				result.push_back(c);
			}
			return result;
		}

	private:
		String m_name;
		Vector<SystemNode> m_systems;
		Vector<SetNode> m_sets;
		Vector<Internal::ScheduleEdge> m_explicitEdges;
		Vector<Internal::ScheduleEdge> m_compiledEdges;
		Vector<s32> m_order;
		Vector<ExecutionBatch> m_batches;
		ScheduleBuildSettings m_buildSettings;
		ExecutorKind m_executorKind = ExecutorKind::SingleThreaded;
		ScheduleExecutor* m_executor = nullptr;
		bool m_dirty = true;
		bool m_lastBuildSucceeded = false;
	};

}
