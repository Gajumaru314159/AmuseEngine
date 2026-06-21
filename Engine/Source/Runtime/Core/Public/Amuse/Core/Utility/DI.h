//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/CorePrivate.h>
#include <Amuse/Core/Reflection/Type.h>
#include <Amuse/Core/Exception/Exception.h>

namespace Amuse::Core {

    constexpr size_t MAX_INJECTION = 16;    // サービスのコンストラクタ引数の最大数

    class ServiceInjector;      // 依存関係定義
    class ServiceContainer;     // 生成ごとのインスタンス管理
    
    template<class T>
    class ServiceBuilder;       // 生成情報登録
    class ServiceBuilderBase;   // 生成情報登録(基底)


    //! @brief 型ごとの生成方法と依存関係を登録するサービス注入器。
    //! @details 詳細は DI ドキュメントを確認してください
    class ServiceInjector {
    public:

        //! @brief 生成可能なサービス型 T を登録する。
        template<class T>
        ServiceBuilder<T>& bind();

        //! @brief 既存インスタンスをサービス型 T として登録する。
        template<class T>
        ServiceBuilder<T>& bind(T& instance);

        //! @brief サービス型 T のインスタンスを依存解決しながら生成する。
        //! @param container 生成されたサービスを管理させるコンテナの参照
        template<class T>
        T* create(ServiceContainer& container)const;

        //! @brief 複数のルートサービスを依存解決しながら生成する。
        //! @param container 生成されたサービスを管理させるコンテナの参照
        template<class... Ts>
        Tuple<Ts*...> createAll(ServiceContainer& container)const;

        //! @brief 登録済みサービスを登録順にまとめて生成する。
        //! @details    抽象クラスに対して
        //! @param container 生成されたサービスを管理させるコンテナの参照
        void createAll(ServiceContainer& container)const;

    private:
        template<class T> friend class ServiceBuilder;  // for m_builders
        HashMap<Type, UPtr<ServiceBuilderBase>>   m_builders;
        HashMap<Type, Vector<Type>>             m_builderMap;
        Vector<Type>                              m_orders;
    };


    namespace Internal {

        // remove_cvr_t
        template<class T>
        using base_type = std::remove_cv_t<std::remove_reference_t<T>>;

        // T(Impl) -> U(Interface) 変換
        template<class T>
        //! @brief コンストラクタ引数型から必要なサービスを遅延解決する変換オブジェクト。
        struct arg_resolver {
            const ServiceInjector& injector; //!< 依存注入定義
            ServiceContainer& container; //!< サービスコンテナ

            // コピーコンストラクタの場合無効化
            //! @brief コピーコンストラクタ引数と同一型か判定する型特性。
            template<class U> using is_copy_constructor = std::is_same<base_type<T>, base_type<U>>;
            //! @brief コピーコンストラクタ候補を除外するための SFINAE 型。
            template<class U> using no_copy_constructor = std::enable_if_t<is_copy_constructor<U>::value == false>;

            // 型変換(参照)
            template<class U, class = no_copy_constructor<U>>
            //! @brief 依存サービス U を生成し参照として渡す。
            operator U& () const {
                auto instance = injector.create<U>(container);
                if (instance == nullptr) {
                    throw Exception(Format("引数の型解決に失敗 {} => {}", Type::Get<U>().name(), Type::Get<T>().name()));
                }
                return *instance;
            }
            // 型変換(ポインタ)
            template<class U, class = no_copy_constructor<U>>
            //! @brief 依存サービス U を生成しポインタとして渡す。
            operator U* () const {
                return injector.create<U>(container);
            }
        };

        // arg_types
        template<class...>
        struct arg_types {};

        // arg_resolver
        template<class T, int... ArgIndex>   // template<class T,int ArgIndex> ?
        using arg_type = arg_resolver<T>;


        // sizeof...(ArgIndex) の引数で構築可能か
        template<class T, int... ArgIndex>
        using can_construct = std::is_constructible<T, arg_type<T, ArgIndex>...>;

        // 構築可能
        template<class T, int... ArgIndex>
        using can_construct_true = std::enable_if_t<can_construct<T, ArgIndex...>::value == true>;

        // 構築不可能
        template<class T, int... ArgIndex>
        using can_construct_false = std::enable_if_t<can_construct<T, ArgIndex...>::value == false>;



        // ベース
        template <class T, class ArgsSequence, class Constructable = void>
        struct constructor {};

        // つくれる場合
        template <class T, size_t... ArgIndex>
        //! @brief 指定引数数で構築可能な場合のコンストラクタ候補。
        struct constructor<T, std::index_sequence<ArgIndex...>, can_construct_true<T, ArgIndex...>> {
            //! @brief 依存解決に使うコンストラクタ引数列。
            using args = arg_types<arg_type<T, ArgIndex>...>;
        };

        // 引数なしで構築できる場合
        template <class T>
        //! @brief 引数なしで構築可能な場合のコンストラクタ候補。
        struct constructor<T, std::index_sequence<>, can_construct_true<T>> {
            //! @brief 空のコンストラクタ引数列。
            using args = arg_types<>;
        };

        // つくれない場合
        template <class T, size_t... ArgIndex>
        //! @brief 構築できない引数数から 1 つ少ない候補へ進む探索ノード。
        struct constructor<T, std::index_sequence<ArgIndex...>, can_construct_false<T, ArgIndex...>> {
            //! @brief 次に試す、引数数を 1 つ減らしたコンストラクタ候補型。
            using next = constructor<T, std::make_index_sequence<sizeof...(ArgIndex) - 1>>;
            //! @brief 最終的に見つかった構築可能な引数列。
            using args = typename next::args;
        };

        // これ以上引数を減らせない場合
        template <class T>
        //! @brief 構築可能な引数列が見つからない場合の終端候補。
        struct constructor<T, std::index_sequence<>, can_construct_false<T>> {
            static_assert(can_construct<T>::value, "Type is not constructible with provided dependencies.");
            //! @brief static_assert 後の形式上の空引数列。
            using args = arg_types<>;
        };



        // 構築可能なコンストラクタの引数情報
        // arg_list<T> = arg_types<arg_type<T,0>,...>;
        template<class T>
        using arg_list = typename constructor<T, std::make_index_sequence<MAX_INJECTION>>::args;


        // ファクトリべース
        template<class T, class>
        struct FactoryBase;

        // 引数の数を取り出したファクトリ
        // Args = arg_array<U,0>;
        template<class T, class... Args>
        //! @brief 解決済み引数列からサービス実体を生成するファクトリ。
        struct FactoryBase<T, arg_types<Args...>> {
            //! @brief 依存サービスを引数として解決し T のインスタンスを生成する。
            static void* Create(const ServiceInjector& injector, ServiceContainer& container) {
                return static_cast<void *>(new T(Args{injector, container}...));
            }
        };

        // ファクトリ
        template<class T>
        using Factory = FactoryBase<T, arg_list<T>>;

    }


    //! @brief 生成可能なサービス型 T を登録する。
    template<class T>
    ServiceBuilder<T>& ServiceInjector::bind() {
        // バインド済みであればBuilderを返す
        auto itr = m_builders.find(Type::Get<T>());
        if (itr != m_builders.end()) {
            return *reinterpret_cast<ServiceBuilder<T>*>(itr->second.get());
        }
        //Builderを生成
        auto builder = new ServiceBuilder<T>(*this);
        m_builders[Type::Get<T>()].reset(builder);
        m_orders.push_back(Type::Get<T>());
        return *builder;
    }

    //! @brief 既存インスタンスをサービス型 T として登録する。
    template<class T>
    ServiceBuilder<T>& ServiceInjector::bind(T& instance) {
        auto builder = new ServiceBuilder<T>(*this,instance);
        m_builders[Type::Get<T>()].reset(builder);
        m_orders.push_back(Type::Get<T>());
        return *builder;
    }


    //! @brief サービス生成情報を型消去して扱う基底クラス。
    class ServiceBuilderBase {
    public:
        virtual ~ServiceBuilderBase() = default;
    private:
        friend class ServiceInjector;
        virtual void* create(ServiceContainer&) = 0;
    };


    //! @brief サービス型 T の生成方法と公開する基底型を保持する。
    template<class T>
    class ServiceBuilder :public ServiceBuilderBase {
    public:
        //! @brief T を指定した基底型から解決できるように登録する。
        template<class... U>
        ServiceBuilder& as() {
            static_assert((std::is_base_of_v<U, T> && ...), "U must be a base class of T.");
            Type types[] = { Type::Get<U>() ... };
            for (auto& type : types) {
                m_bases.emplace(type);
                m_injector.m_builderMap[type].emplace_back(Type::Get<T>());
            }
            return *this;
        }
    private:
        //! @brief 生成可能なサービスとしてビルダーを初期化する。
        ServiceBuilder(ServiceInjector& injector)
            : m_injector(injector)
        {
            m_bases.emplace(Type::Get<T>());
            m_injector.m_builderMap[Type::Get<T>()].emplace_back(Type::Get<T>());
        }
        //! @brief 既存インスタンスを返すサービスとしてビルダーを初期化する。
        ServiceBuilder(ServiceInjector& injector,T& instance)
            : m_injector(injector)
        {
            m_bases.emplace(Type::Get<T>());
            m_injector.m_builderMap[Type::Get<T>()].emplace_back(Type::Get<T>());

            // コピー
            m_getter = [instance]() {
                return (T*)(&instance);
            };
        }
        //! @brief サービスを生成または取得し、コンテナへ保持する。
        void* create(ServiceContainer& container) override;
    private:
        friend class ServiceInjector;
        ServiceInjector& m_injector;
        HashSet<Type> m_bases;
        Func<T*()>      m_getter;
    };

    namespace Internal {

        //! @brief 型消去されたサービスインスタンス保持クラスの基底。
        struct ServiceHolderBase {
            virtual ~ServiceHolderBase() = default;
            //! @brief 保持しているサービスインスタンスを取得する。
            virtual void* get()const = 0;
        };

        //! @brief サービスインスタンスと破棄責任を保持する。
        //@―--------------------------------------------------------------------------- 
        template<class T>
        class ServiceHolder : public ServiceHolderBase {
        public:
            //! @brief サービスインスタンスと破棄責任を設定する。
            ServiceHolder(T* instance,bool destructible) {
                m_instance = instance;
                m_destructible = destructible;
            }
            ~ServiceHolder() override {
                if (m_instance && m_destructible) {
                    delete m_instance;
                }
                m_instance = nullptr;
            }
            //! @brief 保持しているサービスインスタンスを取得する。
            void* get()const override {
                return m_instance;
            }
        private:
            bool m_destructible;
            T* m_instance = nullptr;
        };

    }


    //! @brief 生成済みサービスの寿命と型インデックスを管理するコンテナ。
    class ServiceContainer {
    public:

        //! @brief 空のサービスコンテナを生成する。
        ServiceContainer() = default;

        //! @brief 生成順と逆順に保持サービスを解放する。
        ~ServiceContainer() {
            for (auto itr = m_services.rbegin(); itr != m_services.rend(); ++itr) {
                itr->reset();
            }
        }

        //! @brief 生成済みサービスを型で取得する。
        template<class T>
        T* get()const {
            auto found = m_indices.find(Type::Get<T>());
            if (found == m_indices.end()) return nullptr;
            return static_cast<T*>(m_services.at(found->second)->get());
        }

        //! @brief 指定 Type のサービスが生成済みか判定する。
        bool has(Type type)const {
            return m_indices.contains(type);
        }

        //! @brief テンプレート型 T のサービスが生成済みか判定する。
        template<class T>
        bool has()const {
            return m_indices.contains(Type::Get<T>());
        }

    private:
        template<class T> friend class ServiceBuilder;
        Vector<UPtr<Internal::ServiceHolderBase>> m_services;
        HashMap<Type, size_t> m_indices;
    };

    //! @brief 登録済みサービスを登録順にまとめて生成する。
    inline void ServiceInjector::createAll(ServiceContainer& container)const {
        for (auto& type : m_orders) {
            try {
                if (container.has(type))continue;
                m_builders.find(type)->second->create(container);
            }
            catch (const std::exception& e) {
                LOG_TRACE("[DI] {}の生成がキャンセルされました。\n{}", type.name(), e.what());
            }
        }
    }

    //! @brief サービスを生成または取得し、コンテナへ保持する。
    template<class T>
    void* ServiceBuilder<T>::create(ServiceContainer& container) {
        // 生成
        Internal::ServiceHolder<T>* holder = nullptr;
        if (m_getter) {
            holder = new Internal::ServiceHolder<T>(m_getter(), false);
        }
        else {
            holder = new Internal::ServiceHolder<T>(reinterpret_cast<T*>(Internal::Factory<T>::Create(m_injector, container)), true);
        }
        // 無効
        if (holder->get() == nullptr) {
            delete holder;
            return nullptr;
        }
        // 基底クラスをマッピング
        auto index = container.m_services.size();
        for (auto& type : m_bases) {
            container.m_indices.emplace(type, index);
        }
        return container.m_services.emplace_back(holder)->get();
    }

    //! @brief サービス型 T のインスタンスを依存解決しながら生成する。
    template<class T>
    T* ServiceInjector::create(ServiceContainer& container)const {
        // 生成済み
        if (auto instance = container.get<T>())
            return instance;
        // 抽象->具象
        Vector<Type> fallback;
        const auto& concretes = try_find(m_builderMap, Type::Get<T>(), fallback);
        // 生成
        for (auto& concrete : concretes) {
            auto& builder = m_builders.find(concrete)->second;
            try {
                return reinterpret_cast<T*>(builder->create(container));
            }
            catch (const std::exception& e) {
                LOG_TRACE("[DI] {}の生成がキャンセルされました。\n{}", Type::Get<T>().name(), e.what());
            }
        }
        return nullptr;
    }

    //! @brief 複数のルートサービスを依存解決しながら生成する。
    //! @param container 生成されたサービスを管理させるコンテナの参照
    template<class... Ts>
    Tuple<Ts*...> ServiceInjector::createAll(ServiceContainer& container)const {
        return { create<Ts>(container)... };
    }

}
