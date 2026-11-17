# RDB {#RDB}

```cpp

class TableBase;

class RDB {
public:
  bool load(std::string_view path);

  template<class T>
  T* fetch(T::Key key) {
    return reinterpret_cast<Table<T>*>(m_tables[Type::Get<T>()].get())[key];
  }
private:
  HashMap<Type,UPtr<TableBase>> m_tables;
};

RDB g_rdb;

template<class T>
class TableKey {
public:
  using Table = T;
public:
  TableKey() : m_uuid() {}
  TableKey(const UUID& uuid) : m_uuid(uuid) {}

  const T* operator->() const {
    return g_rdb.fetch<T>(m_uuid);
  }
  const T& operator*() const {
    return *g_rdb.fetch<T>(m_uuid);
  }
  bool empty() const {
    return m_uuid.empty();
  }
private:
  UUID m_uuid;
};


class TableBase {
public:
  virtual ~TableBase() = default;
};

template<class T>
class Table : public TableBase {
public:
  using Key = TableKey<T>;
public:
  const T* find(Key key) {
    return &m_records[m_mapping[key]].second
  }
private:
  HashMap<Key,s32> m_mapping;
  Vector<Pair<Key,T>> m_records;
};


struct Customer {
  using Key = TableKey<Customer>;

  String        displayText;
  String        name;
};

struct Order {
  using Key = TableKey<Order>;

  String        displayText;
  Customer::Key customer;
  Item::Key     item;
  s32           count;
};

template<class T>
Text GetRecordDisplayText(const T& record) {
  return record.displayText; //
}
template<>
Text GetRecordDisplayText(const Customer& record) {
  return record.name;
}

void sample() {

  Customer::Key key;

  // 主キーに対応するデータを取得する
  const Customer* record = RDB::Find(key);

  // Orderテーブルのcutomer列がkeyであるものをを持つビューを取得してイテレート
  for(const Order& order : RDB::Query<Order::custoemr>(key)) {

  }

  // Orderテーブルをカスタムクエリでフィルタしたビューを取得してイテレート
  for(const Order& order : RDB::Query([&](const Order& order){ return order.cutomer == key && 0 < order.count; })) {

  }

  // Queryの先頭データを取得
  const Customer* record = RDB::First<Order::custoemr>(key);
  const Customer* record = RDB::First([&](const Order& order){ return order.cutomer == key && 0 < order.count; });

  // 全探査
  RDB::Each(
    [](const Order& order) {
      if(auto customer = order.customer.get()) {
        customer->name;
      }
    }
  );

}


```
