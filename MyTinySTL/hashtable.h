#ifndef MYTINYSTL_HASHTABLE_H_
#define MYTINYSTL_HASHTABLE_H_

// 这个头文件包含了一个模板类 hashtable
// hashtable : 哈希表，使用开链法处理冲突
// 对应书5.7节

#include <initializer_list>

#include "algo.h"
#include "exceptdef.h"
#include "functional.h"
#include "iterator.h"
#include "memory.h"
#include "type_traits.h"
#include "util.h"
#include "vector.h"

namespace mystl {

// 结点定义
template <typename T>
struct HashtableNode {
  HashtableNode* next;  // 指向下一结点
  T value;              // 储存实值

  HashtableNode() = default;
  HashtableNode(const T& n) : next(nullptr), value(n) {}

  HashtableNode(const HashtableNode& node) : next(node.next), value(node.value) {}
  HashtableNode(HashtableNode&& node) : next(node.next), value(mystl::move(node.value)) {
    node.next = nullptr;
  }
};

// value traits
template <typename T, bool>
struct HtValueTraitsImp {
  using key_type = T;
  using mapped_type = T;
  using value_type = T;

  template <typename Ty>
  static const key_type& get_key(const Ty& value) {
    return value;
  }

  template <typename Ty>
  static const value_type& get_value(const Ty& value) {
    return value;
  }
};

template <typename T>
struct HtValueTraitsImp<T, true> {
  using key_type = typename std::remove_cv<typename T::first_type>::type;
  using mapped_type = typename T::second_type;
  using value_type = T;

  template <typename Ty>
  static const key_type& get_key(const Ty& value) {
    return value.first;
  }

  template <typename Ty>
  static const value_type& get_value(const Ty& value) {
    return value;
  }
};

template <typename T>
struct HtValueTraits {
  static constexpr bool kIsMap = mystl::IsPair<T>::kValue;

  using value_traits_type = HtValueTraitsImp<T, kIsMap>;

  using key_type = typename value_traits_type::key_type;
  using mapped_type = typename value_traits_type::mapped_type;
  using value_type = typename value_traits_type::value_type;

  template <typename Ty>
  static const key_type& get_key(const Ty& value) {
    return value_traits_type::get_key(value);
  }

  template <typename Ty>
  static const value_type& get_value(const Ty& value) {
    return value_traits_type::get_value(value);
  }
};

// forward declaration
template <typename T, typename HashFun, typename KeyEqual>
class Hashtable;

template <typename T, typename HashFun, typename KeyEqual>
struct HtIterator;

template <typename T, typename HashFun, typename KeyEqual>
struct HtConstIterator;

template <typename T>
struct HtLocalIterator;

template <typename T>
struct HtConstLocalIterator;

template <typename T, typename Hash, typename KeyEqual>
struct HtIteratorBase : public mystl::Iterator<mystl::ForwardIteratorTag, T> {
  using hashtable = mystl::Hashtable<T, Hash, KeyEqual>;
  using base = HtIteratorBase<T, Hash, KeyEqual>;
  using iterator = mystl::HtIterator<T, Hash, KeyEqual>;
  using const_iterator = mystl::HtConstIterator<T, Hash, KeyEqual>;
  using node_ptr = HashtableNode<T>*;
  using contain_ptr = hashtable*;
  using const_node_ptr = const node_ptr;
  using const_container_ptr = const contain_ptr;

  using size_type = size_t;
  using difference_type = ptrdiff_t;

  node_ptr node;   // 迭代器当前所指结点
  contain_ptr ht;  // 保持与容器的连接

  HtIteratorBase() = default;

  bool operator==(const base& rhs) const { return node == rhs.node; }
  bool operator!=(const base& rhs) const { return node != rhs.node; }
};

template <typename T, typename Hash, typename KeyEqual>
struct HtIterator : public HtIteratorBase<T, Hash, KeyEqual> {
  using base = HtIteratorBase<T, Hash, KeyEqual>;
  using hashtable = typename base::hashtable;
  using iterator = typename base::iterator;
  using const_iterator = typename base::const_iterator;
  using node_ptr = typename base::node_ptr;
  using contain_ptr = typename base::contain_ptr;

  using value_traits = HtValueTraits<T>;
  using value_type = T;
  using pointer = value_type*;
  using reference = value_type&;

  using base::ht;
  using base::node;

  HtIterator() = default;
  HtIterator(node_ptr n, contain_ptr t) {
    node = n;
    ht = t;
  }
  HtIterator(const iterator& rhs) {
    node = rhs.node;
    ht = rhs.ht;
  }
  HtIterator(const const_iterator& rhs) {
    node = rhs.node;
    ht = rhs.ht;
  }
  iterator& operator=(const iterator& rhs) {
    if (this != &rhs) {
      node = rhs.node;
      ht = rhs.ht;
    }
    return *this;
  }
  iterator& operator=(const const_iterator& rhs) {
    if (this != &rhs) {
      node = rhs.node;
      ht = rhs.ht;
    }
  }

  reference operator*() const { return node->value; }
  pointer operator->() const { return &(operator*()); }

  iterator& operator++() {
    MYSTL_DEBUG(node != nullptr);
    const node_ptr old = node;
    node = node->next;
    if (node == nullptr) {
      // 如果下一个位置为空，跳到下一个bucket的起始处
      auto index = ht->hash(value_traits::get_key(old->value));
      while (!node && ++index < ht->bucket_size_) {
        node = ht->buckets_[index];
      }
      return *this;
    }
  }
  iterator operator++(int) {
    iterator tmp = *this;
    ++*this;
    return tmp;
  }
};

template <typename T, typename Hash, typename KeyEqual>
struct HtConstIterator : public HtIteratorBase<T, Hash, KeyEqual> {
  using base = HtIteratorBase<T, Hash, KeyEqual>;
  using hashtable = typename base::hashtable;
  using iterator = typename base::iterator;
  using const_iterator = typename base::const_iterator;
  using node_ptr = typename base::const_node_ptr;
  using contain_ptr = typename base::const_contain_ptr;

  using value_traits = HtValueTraits<T>;
  using value_type = T;
  using pointer = const value_type*;
  using reference = const value_type&;

  using base::ht;
  using base::node;

  HtConstIterator() = default;
  HtConstIterator(node_ptr n, contain_ptr t) {
    node = n;
    ht = t;
  }
  HtConstIterator(const iterator& rhs) {
    node = rhs.node;
    ht = rhs.ht;
  }
  HtConstIterator(const const_iterator& rhs) {
    node = rhs.node;
    ht = rhs.ht;
  }
  const_iterator& operator=(const iterator& rhs) {
    if (this != &rhs) {
      node = rhs.node;
      ht = rhs.ht;
    }
    return *this;
  }
  const_iterator& operator=(const const_iterator& rhs) {
    if (this != &rhs) {
      node = rhs.node;
      ht = rhs.ht;
    }
  }

  reference operator*() const { return node->value; }
  pointer operator->() const { return &(operator*()); }

  const_iterator& operator++() {
    MYSTL_DEBUG(node != nullptr);
    const node_ptr old = node;
    node = node->next;
    if (node == nullptr) {
      // 如果下一个位置为空，跳到下一个bucket的起始处
      auto index = ht->hash(value_traits::get_key(old->value));
      while (!node && ++index < ht->bucket_size_) {
        node = ht->buckets_[index];
      }
      return *this;
    }
  }
  const_iterator operator++(int) {
    const_iterator tmp = *this;
    ++*this;
    return tmp;
  }
};

template <typename T>
struct HtLocalIterator : public mystl::Iterator<mystl::ForwardIteratorTag, T> {
  using value_type = T;
  using pointer = value_type*;
  using reference = value_type&;
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using node_ptr = HashtableNode<T>*;

  using self = HtLocalIterator<T>;
  using local_iterator = HtLocalIterator<T>;
  using const_local_iterator = HtConstLocalIterator<T>;

  node_ptr node;

  HtLocalIterator(node_ptr n) : node(n) {}
  HtLocalIterator(const local_iterator& rhs) : node(rhs.node) {}
  HtLocalIterator(const const_local_iterator& rhs) : node(rhs.node) {}

  reference operator*() const { return node->value; }
  pointer operator->() const { return &(operator*()); }

  self& operator++() {
    MYSTL_DEBUG(node != nullptr);
    node = node->next;
    return *this;
  }

  self operator++(int) {
    self tmp(*this);
    ++*this;
    return tmp;
  }

  bool operator==(const self& other) const { return node == other.node; }
  bool operator!=(const self& other) const { return node != other.node; }
};

template <typename T>
struct HtConstLocalIterator : public mystl::Iterator<mystl::ForwardIteratorTag, T> {
  using value_type = T;
  using pointer = const value_type*;
  using reference = const value_type&;
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using node_ptr = const HashtableNode<T>*;

  using self = HtConstLocalIterator<T>;
  using local_iterator = HtLocalIterator<T>;
  using const_local_iterator = HtConstLocalIterator<T>;

  node_ptr node;

  HtConstLocalIterator(node_ptr n) : node(n) {}
  HtConstLocalIterator(const local_iterator& rhs) : node(rhs.node) {}
  HtConstLocalIterator(const const_local_iterator& rhs) : node(rhs.node) {}

  reference operator*() const { return node->value; }
  pointer operator->() const { return &(operator*()); }

  self& operator++() {
    MYSTL_DEBUG(node != nullptr);
    node = node->next;
    return *this;
  }

  self operator++(int) {
    self tmp(*this);
    ++*this;
    return tmp;
  }

  bool operator==(const self& other) const { return node == other.node; }
  bool operator!=(const self& other) const { return node != other.node; }
};

// bucket 使用的大小

#if (_MSC_VER && _WIN64) || ((__GNUC__ || __clang__) && __SIZEOF_POINTER__ == 8)
#define SYSTEM_64 1
#else
#define SYSTEM_32 1
#endif

#ifdef SYSTEM_64

#define PRIME_NUM 99

// 1. start with p = 101
// 2. p = next_prime(p * 1.7)
// 3. if p < (2 << 63), go to step 2, otherwise, go to step 4
// 4. end with p = prev_prime(2 << 63 - 1)
static constexpr size_t kHtPrimeList[] = {
    101ULL,
    173ULL,
    263ULL,
    397ULL,
    599ULL,
    907ULL,
    1361ULL,
    2053ULL,
    3083ULL,
    4637ULL,
    6959ULL,
    10453ULL,
    15683ULL,
    23531ULL,
    35311ULL,
    52967ULL,
    79451ULL,
    119179ULL,
    178781ULL,
    268189ULL,
    402299ULL,
    603457ULL,
    905189ULL,
    1357787ULL,
    2036687ULL,
    3055043ULL,
    4582577ULL,
    6873871ULL,
    10310819ULL,
    15466229ULL,
    23199347ULL,
    34799021ULL,
    52198537ULL,
    78297827ULL,
    117446801ULL,
    176170229ULL,
    264255353ULL,
    396383041ULL,
    594574583ULL,
    891861923ULL,
    1337792887ULL,
    2006689337ULL,
    3010034021ULL,
    4515051137ULL,
    6772576709ULL,
    10158865069ULL,
    15238297621ULL,
    22857446471ULL,
    34286169707ULL,
    51429254599ULL,
    77143881917ULL,
    115715822899ULL,
    173573734363ULL,
    260360601547ULL,
    390540902329ULL,
    585811353559ULL,
    878717030339ULL,
    1318075545511ULL,
    1977113318311ULL,
    2965669977497ULL,
    4448504966249ULL,
    6672757449409ULL,
    10009136174239ULL,
    15013704261371ULL,
    22520556392057ULL,
    33780834588157ULL,
    50671251882247ULL,
    76006877823377ULL,
    114010316735089ULL,
    171015475102649ULL,
    256523212653977ULL,
    384784818980971ULL,
    577177228471507ULL,
    865765842707309ULL,
    1298648764060979ULL,
    1947973146091477ULL,
    2921959719137273ULL,
    4382939578705967ULL,
    6574409368058969ULL,
    9861614052088471ULL,
    14792421078132871ULL,
    22188631617199337ULL,
    33282947425799017ULL,
    49924421138698549ULL,
    74886631708047827ULL,
    112329947562071807ULL,
    168494921343107851ULL,
    252742382014661767ULL,
    379113573021992729ULL,
    568670359532989111ULL,
    853005539299483657ULL,
    1279508308949225477ULL,
    1919262463423838231ULL,
    2878893695135757317ULL,
    4318340542703636011ULL,
    6477510814055453699ULL,
    9716266221083181299ULL,
    14574399331624771603ULL,
    18446744073709551557ULL};

#else

#define PRIME_NUM 44

// 1. start with p = 101
// 2. p = next_prime(p * 1.7)
// 3. if p < (2 << 31), go to step 2, otherwise, go to step 4
// 4. end with p = prev_prime(2 << 31 - 1)
static constexpr size_t ht_prime_list[] = {
    101u,        173u,        263u,       397u,       599u,       907u,        1361u,
    2053u,       3083u,       4637u,      6959u,      10453u,     15683u,      23531u,
    35311u,      52967u,      79451u,     119179u,    178781u,    268189u,     402299u,
    603457u,     905189u,     1357787u,   2036687u,   3055043u,   4582577u,    6873871u,
    10310819u,   15466229u,   23199347u,  34799021u,  52198537u,  78297827u,   117446801u,
    176170229u,  264255353u,  396383041u, 594574583u, 891861923u, 1337792887u, 2006689337u,
    3010034021u, 4294967291u,
};

#endif

// 找出最接近并大于等于n的那个质数
inline size_t ht_next_prime(size_t n) {
  const size_t* first = kHtPrimeList;
  const size_t* last = kHtPrimeList + PRIME_NUM;
  const size_t* pos = mystl::lower_bound(first, last, n);
  return pos == last ? *(last - 1) : *pos;
}

// 模板类hashtable
// 参数一代表数据类型，参数二代表哈希函数，参数三代表键值相等的比较函数
template <typename T, typename Hash, typename KeyEqual>
class Hashtable {
  friend struct mystl::HtIterator<T, Hash, KeyEqual>;
  friend struct mystl::HtConstIterator<T, Hash, KeyEqual>;

 public:
  using value_traits = HtValueTraits<T>;
  using key_type = typename value_traits::key_type;
  using mapped_type = typename value_traits::mapped_type;
  using value_type = typename value_traits::value_type;
  using hasher = Hash;
  using key_equal = KeyEqual;

  using node_type = HashtableNode<T>;
  using node_ptr = node_type*;
  using bucket_type = mystl::Vector<node_ptr>;

  using allocator_type = mystl::Allocator<T>;
  using data_allocator = mystl::Allocator<T>;
  using node_allocator = mystl::Allocator<node_type>;

  using pointer = typename allocator_type::pointer;
  using const_pointer = typename allocator_type::const_pointer;
  using reference = typename allocator_type::reference;
  using const_reference = typename allocator_type::const_reference;
  using size_type = typename allocator_type::size_type;
  using difference_type = typename allocator_type::difference_type;

  using iterator = mystl::HtIterator<T, Hash, KeyEqual>;
  using const_iterator = mystl::HtConstIterator<T, Hash, KeyEqual>;
  using local_iterator = mystl::HtLocalIterator<T>;
  using const_local_iterator = mystl::HtConstLocalIterator<T>;

  allocator_type get_allocator() const { return allocator_type(); }

 private:
  bucket_type buckets_;
  size_type bucket_size_;
  size_type size_;
  float mlf_;
  hasher hash_;
  key_equal equal_;

 private:
  bool is_equal(const key_type& key1, const key_type& key2) { return equal_(key1, key2); }

  bool is_equal(const key_type& key1, const key_type& key2) const { return equal_(key1, key2); }

  const_iterator M_cit(node_ptr node) const noexcept {
    return const_iterator(node, const_cast<Hashtable*>(this));
  }

  iterator M_begin() noexcept {
    for (size_type n = 0; n < bucket_size_; ++n) {
      if (buckets_[n]) {
        // 找到第一个右结点的位置就返回
        return iterator(buckets_[n], this);
      }
    }
    return iterator(nullptr, this);
  }

  const_iterator M_begin() const noexcept {
    for (size_type n = 0; n < bucket_size_; ++n) {
      if (buckets_[n]) {
        return M_cit(buckets_[n]);
      }
    }
    return M_cit(nullptr);
  }

 public:
  explicit Hashtable(
      size_type bucket_count, const Hash& hash = Hash(), const KeyEqual& equal = KeyEqual())
      : size_(0), mlf_(1.0F), hash_(hash), equal_(equal) {
    init(bucket_count);
  }

  template <
      typename Iter,
      typename std::enable_if<mystl::IsInputIterator<Iter>::kValue, int>::typep = 0>
  Hashtable(
      Iter first,
      Iter last,
      size_type bucket_count,
      const Hash& hash = Hash(),
      const KeyEqual& equal = KeyEqual())
      : size_(mystl::distance(first, last)), mlf_(1.0F), hash_(hash), equal_(equal) {
    init(mystl::max(bucket_count, static_cast<size_type>(mystl::distance(first, last))));
  }

  Hashtable(const Hashtable& rhs) : hash_(rhs.hash_), equal_(rhs.equal_) { copy_init(rhs); }
  Hashtable(Hashtable&& rhs) noexcept
      : bucket_size_(rhs.bucket_size_),
        size_(rhs.size_),
        mlf_(rhs.mlf_),
        hash_(rhs.hash_),
        equal_(rhs.equal_) {
    buckets_ = mystl::move(rhs.buckets_);
    rhs.bucket_size_ = 0;
    rhs.size_ = 0;
    rhs.mlf_ = 0.0F;
  }

  Hashtable& operator=(const Hashtable& rhs);
  Hashtable& operator=(Hashtable&& rhs) noexcept;

  ~Hashtable() { clear(); }

  iterator begin() noexcept { return M_begin(); }
  const_iterator begin() const noexcept { return M_begin(); }
  iterator end() noexcept { return iterator(nullptr, this); }
  const_iterator end() const noexcept { return M_cit(nullptr); }

  const_iterator cbegin() const noexcept { return begin(); }
  const_iterator cend() const noexcept { return end(); }

  bool empty() const noexcept { return size_ == 0; }
  size_type size() const noexcept { return size_; }
  size_type max_size() const noexcept { return static_cast<size_type>(-1); }

  // 修改容器相关操作
  template <typename... Args>
  iterator emplace_multi(Args&&... args);

  template <typename... Args>
  pair<iterator, bool> emplace_unique(Args&&... args);

  // [note]: hint对于hash_table其实没有意义，因为即使提供了hint，也要做一次hash，
  // 来确保hash_table的性质，所以选择忽略它
  template <typename... Args>
  iterator emplace_multi_use_hint(const_iterator /*hint*/, Args&&... args) {
    return emplace_multi(mystl::forward<Args>(args)...);
  }

  template <typename... Args>
  iterator emplace_unique_use_hint(const_iterator /*hint*/, Args&&... args) {
    return emplace_unique(mystl::forward<Args>(args)...).first;
  }

  iterator insert_multi_noresize(const value_type& value);
  pair<iterator, bool> insert_unique_noresize(const value_type& value);

  iterator insert_multi(const value_type& value) {
    rehash_if_need(1);
    return insert_multi_noresize(value);
  }
  iterator insert_multi(value_type&& value) { return emplace_multi(mystl::move(value)); }

  pair<iterator, bool> insert_unique(const value_type& value) {
    rehash_if_need(1);
    return insert_unique_noresize(value);
  }
  pair<iterator, bool> insert_unique(value_type&& value) {
    return emplace_unique(mystl::move(value));
  }

  iterator insert_multi_use_hint(const_iterator /*hint*/, const value_type& value) {
    return insert_multi(value);
  }
  iterator insert_multi_use_hint(const_iterator /*hint*/, value_type&& value) {
    return emplace_multi(mystl::move(value));
  }

  iterator insert_unique_use_hint(const_iterator /*hint*/, const value_type& value) {
    return insert_unique(value).first;
  }
  iterator insert_unique_use_hint(const_iterator /*hint*/, value_type&& value) {
    return emplace_unique(mystl::move(value)).first;
  };

  template <typename InputIter>
  void insert_multi(InputIter first, InputIter last) {
    copy_insert_multi(first, last, iterator_category(first));
  }

  template <typename InputIter>
  void insert_unique(InputIter first, InputIter last) {
    copy_insert_multi(first, last, iterator_category(first));
  }

  void erase(const_iterator position);
  void erase(const_iterator first, const_iterator last);

  size_type erase_multi(const key_type& key);
  size_type erase_unique(const key_type& key);

  void clear();

  void swap(Hashtable& rhs) noexcept;

  // 查找相关操作
  size_type count(const key_type& key) const;

  iterator find(const key_type& key);
  const_iterator find(const key_type& key) const;

  pair<iterator, iterator> equal_range_multi(const key_type& key);
  pair<const_iterator, const_iterator> equal_range_multi(const key_type& key) const;

  pair<iterator, iterator> equal_range_unique(const key_type& key);
  pair<const_iterator, const_iterator> equal_range_unique(const key_type& key) const;

  // bucket interface
  local_iterator begin(size_type n) noexcept {
    MYSTL_DEBUG(n < size_);
    return buckets_[n];
  }
  const_local_iterator begin(size_type n) const noexcept {
    MYSTL_DEBUG(n < size);
    return bucket_size_[n];
  }
  const_local_iterator cbegin(size_type n) const noexcept {
    MYSTL_DEBUG(n < size_);
    return buckets_[n];
  }
  local_iterator end(size_type n) noexcept {
    MYSTL_DEBUG(n < size_);
    return nullptr;
  }
  const_local_iterator end(size_type n) const noexcept {
    MYSTL_DEBUG(n < size_);
    return nullptr;
  }
  const_local_iterator cend(size_type n) const noexcept {
    MYSTL_DEBUG(n < size_);
    return nullptr;
  }

  size_type bucket_count() const noexcept { return bucket_size_; }
  size_type max_bucket_count() const noexcept { return kHtPrimeList[PRIME_NUM - 1]; }

  size_type bucket_size(size_type n) const noexcept;
  size_type bucket(const size_type& key) const { return hash(key); }

  // hash policy
  float load_factor() const noexcept {
    // TODO(lzj): size_是什么
    return bucket_size_ != 0 ? (float)size_ / bucket_size_ : 0.0F;
  }

  float max_load_factor() const noexcept { return mlf_; }
  void max_load_factor(float ml) {
    THROW_OUT_OF_RANGE_IF(ml != ml || ml < 0, "invalid hash load factor");
    mlf_ = ml;
  }

  void rehash(size_type count);

  void reserve(size_type count) {
    rehash(static_cast<size_type>((float)count / max_load_factor() + 0.5F));
  }

  hasher hash_fcn() const { return hash_; }
  key_equal key_eq() const { return equal_; }

 private:
  void init(size_type n);
  void copy_init(const Hashtable& ht);

  template <typename... Args>
  node_ptr create_node(Args&&... args);
  void destroy_node(node_ptr n);

  size_type next_size(size_type n) const;
  size_type hash(const key_type& key, size_type n) const;
  size_type hash(const key_type& key) const;
  void rehash_if_need(size_type n);

  template <typename InputIter>
  void copy_insert_multi(InputIter first, InputIter last, mystl::InputIteratorTag);
  template <typename InputIter>
  void copy_insert_unique(InputIter first, InputIter last, mystl::InputIteratorTag);
};

}  // namespace mystl
#endif  // !MYTINYSTL_HASHTABLE_H_
