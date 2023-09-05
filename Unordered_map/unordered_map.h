#include<iostream>
#include<vector>
#include <cassert>

template <size_t N>
struct StackStorage {
  int8_t data[N];
  size_t size;
  StackStorage() : size(0) {}
};


template <typename T, size_t N>
class StackAllocator {
 public:
  StackStorage<N>* st;

  using value_type = T;

  template <typename U>
  struct rebind {
    using other = StackAllocator<U, N>;
  };

  T* allocate(size_t) const;
  void deallocate(T*, size_t) const;
  StackAllocator() = default;
  ~StackAllocator() = default;
  explicit StackAllocator(StackStorage<N>& other) : st(&other) {}

  template<typename U>
  StackAllocator(const StackAllocator<U, N>& other) : st(other.st) {}
};

template<class T, size_t N>
T* StackAllocator<T, N>::allocate(size_t num_elements) const {
  size_t alignment = st->size % alignof(T);
  if (alignment > 0) {
    alignment = alignof(T) - alignment;
  }
  st->size += alignment;
  T* ptr = reinterpret_cast<T*>(st->data + st->size);
  if (num_elements == 0 or st->size + sizeof(T) * num_elements > N) {
    throw std::bad_alloc();
  }
  st->size += sizeof(T) * num_elements;
  return ptr;
}

template<class T, size_t N>
void StackAllocator<T, N>::deallocate(T* ptr, size_t count) const {
  if (reinterpret_cast<int8_t*>(ptr) + count * sizeof(T) == st->data + st->size) {
    st->size -= count * sizeof(T);
  }
}

template<typename T, typename Allocator = std::allocator<T>>
class List {
 private:
  struct BaseNode {
    BaseNode *prev;
    BaseNode *next;

    BaseNode() = default;
    BaseNode(BaseNode *pr, BaseNode *nx) : prev(pr), next(nx) {}
  };

  struct Node : BaseNode {
    T value;
    Node() = default;
    Node(const T &val, BaseNode *pr, BaseNode *nx) : BaseNode(pr, nx), value(val) {}
  };

  using AllocTraits = std::allocator_traits<Allocator>;
  typename AllocTraits::template rebind_alloc<BaseNode> rebind_allocator;
  using this_alloc = std::allocator_traits<typename AllocTraits::template rebind_alloc<BaseNode>>;
  using base_node_alloc = typename std::allocator_traits<Allocator>::template rebind_alloc<BaseNode>;
  using node_alloc = typename std::allocator_traits<Allocator>::template rebind_alloc<Node>;

  node_alloc cur_alloc;
  BaseNode *head;
  size_t sz = 0;

  template<bool is_const>
  struct base_iterator {
    BaseNode* ptr;
    using pointer = typename std::conditional<is_const, const T *, T *>::type;
    using reference = typename std::conditional<is_const, const T &, T &>::type;
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = typename std::conditional<is_const, const T, T>::type;
    using difference_type = ptrdiff_t;


    base_iterator() : ptr(nullptr) {};
    base_iterator(BaseNode *BaseNode) : ptr(BaseNode) {};

    operator base_iterator<true>() const {
      return base_iterator<true>(ptr);
    }

    reference operator*() const {
      return static_cast<Node*>(ptr)->value;
    }

    pointer operator->() const {
      return &(static_cast<Node*>(ptr)->value);
    }

    base_iterator &operator++() {
      ptr = ptr->next;
      return *this;
    }

    base_iterator &operator--() {
      ptr = ptr->prev;
      return *this;
    }

    base_iterator operator++(int) {
      base_iterator it = *this;
      ++(*this);
      return it;
    }

    base_iterator operator--(int) {
      base_iterator it = *this;
      --(*this);
      return it;
    }

    bool operator==(const base_iterator &a) const {
      return (ptr == a.ptr);
    }

    bool operator!=(const base_iterator &a) const {
      return !(*this == a);
    }

  };

  void head_create() {
    base_node_alloc base_alloc = cur_alloc;
    head = std::allocator_traits<base_node_alloc>::allocate(base_alloc, 1);
    head->prev = head;
    head->next = head;
  }

  void remove() {
    BaseNode* it = head->next;
    while (it != head) {
      BaseNode* tmp = it->next;
      std::allocator_traits<node_alloc>::destroy(cur_alloc, static_cast<Node*>(it));
      cur_alloc.deallocate(static_cast<Node*>(it), 1);
      it = tmp;
    }
  }


 public:
  List(size_t, const T&, const Allocator& cur_alloc = Allocator());
  explicit List(size_t);
  List(size_t, const T&);
  explicit List(const Allocator& allocator = Allocator());
  List(size_t, const Allocator& allocator = Allocator());
  List(const List&);
  List(List&&);
  ~List();
  void clear_list(List& list);
  List& operator=(const List&);
  List& operator=(List&&);

  void push_back(const T&);
  void push_front(const T&);
  void pop_back();
  void pop_front();

  using iterator = base_iterator<false>;
  using const_iterator = base_iterator<true>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  template<typename... Args>
  void emplace_back(Args&&... args);
  template<typename... Args>
  iterator emplace(const_iterator position, Args&&... args);
  void insert(const_iterator, const T&);
  void erase(const_iterator);

  iterator begin() {
    return iterator(head->next);
  }
  const_iterator begin() const {
    return const_iterator(head->next);
  }
  iterator end() {
    return iterator(head);
  }
  const_iterator end() const {
    return const_iterator(head);
  }
  const_iterator cbegin() const {
    return begin();
  }
  const_iterator cend() const {
    return begin();
  }
  reverse_iterator rbegin() {
    return reverse_iterator(head);
  }
  const_reverse_iterator rbegin() const {
    return reverse_iterator(head);
  }
  reverse_iterator rend() {
    return reverse_iterator(head->next);
  }
  const_reverse_iterator rend() const {
    return reverse_iterator(head->next);
  }
  const_reverse_iterator crbegin() const {
    return reverse_iterator(head);
  }
  const_reverse_iterator crend() const {
    return reverse_iterator(head->next);
  }

  Allocator get_allocator() const {
    return cur_alloc;
  }

  [[nodiscard]] size_t size() const {
    return sz;
  }
};


template<typename T, typename Alloc>
template<typename... Args>
void List<T, Alloc>::emplace_back(Args&&... args) {
  Node* cur = std::allocator_traits<node_alloc>::allocate(cur_alloc, 1);
  try {
    std::allocator_traits<node_alloc>::construct(cur_alloc, &cur->value, std::forward<Args>(args)...);
  } catch(...) {
    std::allocator_traits<node_alloc>::deallocate(cur_alloc, cur, 1);
  }
  ++sz;
  cur->next = head;
  cur->prev = head->prev;
  head->prev->next = cur;
  head->prev = cur;
}
template<typename T, typename Allocator>
template<typename... Args>
typename List<T, Allocator>::iterator List<T, Allocator>::emplace(const_iterator position, Args&&... args) {
  assert(position.ptr != nullptr);
  Node* added = std::allocator_traits<node_alloc>::allocate(cur_alloc, 1);
  try {
    std::allocator_traits<node_alloc>::construct(cur_alloc, &added->value, std::forward<Args>(args)...);
    added->next = position.ptr;
    added->prev = position.ptr->prev;
    position.ptr->prev->next = added;
    position.ptr->prev = added;
  } catch (...) {
    std::allocator_traits<node_alloc>::deallocate(cur_alloc, added, 1);
    throw;
  }

  ++sz;
  return iterator(added);
}

template<typename T, typename Allocator>
void List<T, Allocator>::insert(const_iterator position, const T& value) {
  assert(position.ptr != nullptr);
  Node* added = std::allocator_traits<node_alloc>::allocate(rebind_allocator, 1);
  try {
    std::allocator_traits<node_alloc>::construct(rebind_allocator, added, value, position.ptr->prev, position.ptr);
  } catch (...) {
    std::allocator_traits<node_alloc>::deallocate(rebind_allocator, added, 1);
    throw;
  }

  position.ptr->prev->next = added;
  added->prev = position.ptr->prev;

  position.ptr->prev = added;
  added->next = position.ptr;

  ++sz;
}


template<typename T, typename Allocator>
void List<T, Allocator>::erase(const_iterator position) {
  assert(position.ptr != nullptr && (position.ptr != head->next || sz > 0));

  Node* to_remove = static_cast<Node*>(position.ptr);
  to_remove->prev->next = to_remove->next;
  to_remove->next->prev = to_remove->prev;

  try {
    std::allocator_traits<node_alloc>::destroy(cur_alloc, to_remove);
    std::allocator_traits<node_alloc>::deallocate(cur_alloc, to_remove, 1);
  } catch (...) {
    throw;
  }
  --sz;
}

template<typename T, typename Allocator>
List<T, Allocator>::List(size_t n, const T& value, const Allocator& allocator) : cur_alloc(allocator), sz(0) {
  head_create();
  try {
    for (size_t i = 0; i < n; ++i) {
      emplace_back(value);
    }
  } catch (...) {
    remove();
    throw;
  }
}

template<typename T, typename Allocator>
List<T, Allocator>::List(size_t n) : List(n, Allocator()) {}

template<typename T, typename Allocator>
List<T, Allocator>::List(const List& other)
    : rebind_allocator(cur_alloc),
      cur_alloc(std::allocator_traits<Allocator>::select_on_container_copy_construction(other.get_allocator())),
      head(nullptr),
      sz(0) {
  size_t prev_sz = sz;
  BaseNode* prev_head = head;
  auto prev_alloc = cur_alloc;
  head_create();

  try {
    for (const auto& value : other) {
      emplace_back(value);
    }
  } catch (...) {
    remove();
    cur_alloc = prev_alloc;
    head = prev_head;
    sz = prev_sz;
    throw;
  }
}

template<typename T, typename Allocator>
List<T, Allocator>::List(size_t n, const T& value) : List(n, value, Allocator()) {}

template<typename T, typename Allocator>
List<T, Allocator>::List(const Allocator& allocator) : cur_alloc(allocator) {
  head_create();
}

template<typename T, typename Allocator>
List<T, Allocator>::List(size_t n, const Allocator& allocator) : cur_alloc(allocator) {
  typename std::allocator_traits<node_alloc>::allocator_type node_a(cur_alloc);
  head_create();
  try {
    for (size_t i = 0; i < n; ++i) {
      Node* added = std::allocator_traits<node_alloc>::allocate(node_a, 1);
      if (added == nullptr) {
        throw std::bad_alloc();
      }
      try {
        std::allocator_traits<node_alloc>::construct(cur_alloc, added);
      }
      catch (const std::exception& e) {
        std::cerr << "Error in insertion: " << e.what() << std::endl;
        std::allocator_traits<node_alloc>::deallocate(cur_alloc, added, 1);
        throw;
      }
      catch (...) {
        std::cerr << "Unknown error in insertion." << std::endl;
        std::allocator_traits<node_alloc>::deallocate(cur_alloc, added, 1);
        throw;
      }
      added->next = end().ptr;
      added->prev = end().ptr->prev;
      end().ptr->prev->next = added;
      end().ptr->prev = added;
    }
    sz = n;
  }
  catch (...) {
    remove();
    throw;
  }
}

template<typename T, typename Allocator>
void List<T, Allocator>::clear_list(List<T, Allocator>& list) {
  list.head = this_alloc::allocate(list.rebind_allocator, 1);
  list.head->next = list.head;
  list.head->prev = list.head;
  list.sz = 0;
}

template<typename T, typename Allocator>
List<T, Allocator>::List(List&& other)
    : rebind_allocator(std::move(other.rebind_allocator)),
      cur_alloc(std::move(other.cur_alloc)),
      head(other.head),
      sz(other.sz) {
  clear_list(other);
}

template<typename T, typename Allocator>
List<T, Allocator>& List<T, Allocator>::operator=(const List& other) {
  if (this == &other) {
    return *this;
  }
  List<T, Allocator> tmp(other);
  try {
    std::swap(head, tmp.head);
    std::swap(sz, tmp.sz);
  } catch (...) {
    std::swap(head, tmp.head);
    std::swap(sz, tmp.sz);
    throw;
  }
  if (std::allocator_traits<this_alloc>::propagate_on_container_copy_assignment::value) {
    cur_alloc = other.cur_alloc;
  }
  return *this;
}


template<typename T, typename Allocator>
List<T, Allocator>& List<T, Allocator>::operator=(List&& other) {
  if (this == &other) {
    return *this;
  }
  remove();
  rebind_allocator = std::move(other.rebind_allocator);
  cur_alloc = std::move(other.cur_alloc);
  head = other.head;
  sz = other.sz;
  clear_list(other);
  return *this;
}


template<typename T, typename Allocator>
void List<T, Allocator>::push_back(const T& value) {
  insert(end(), value);
}

template<typename T, typename Allocator>
void List<T, Allocator>::push_front(const T& value) {
  insert(begin(), value);
}

template<typename T, typename Allocator>
void List<T, Allocator>::pop_back() {
  if (sz > 0) {
    erase(--end());
  }
}

template<typename T, typename Allocator>
void List<T, Allocator>::pop_front() {
  if (sz > 0) {
    erase(begin());
  }
}

template<typename T, typename Allocator>
List<T, Allocator>::~List() {
  remove();
}

template<typename Key, typename Value, typename Hash = std::hash<Key>,
    typename Equal = std::equal_to<Key>, typename Alloc = std::allocator<std::pair<const Key, Value>>>
class UnorderedMap {
 private:
  using AllocTraits = std::allocator_traits<Alloc>;
  using KeyValuePair = std::pair<const Key, Value>;
  using ListType = List<KeyValuePair, Alloc>;
  using NodeType = std::pair<const Key, Value>;
  using Iterator = typename ListType::iterator;
  using ConstIterator = typename List<NodeType, Alloc>::const_iterator;
  using RebindAlloc = typename AllocTraits::template rebind_alloc<Iterator>;

  size_t sz = 0;
  static constexpr double max_sz = 0.7;
  std::vector<std::vector<Iterator, RebindAlloc>,
              typename AllocTraits::template rebind_alloc<std::vector<Iterator, RebindAlloc>>> hash_table;
  ListType elements;
  Hash using_function;
  Equal equalizer;
  Alloc cur_alloc;

 public:
  UnorderedMap();
  UnorderedMap(const UnorderedMap& other);
  UnorderedMap(UnorderedMap&& other);
  ~UnorderedMap();
  UnorderedMap& operator=(const UnorderedMap& other);
  UnorderedMap& operator=(UnorderedMap&& other);
  Value& operator[](const Key& key);
  Value& at(const Key& key);
  const Value& at(const Key& key) const;

 public:

  [[nodiscard]] size_t size() const {
    return elements.size();
  }

  Iterator begin() {
    return elements.begin();
  }
  ConstIterator begin() const {
    return elements.begin();
  }
  Iterator end() {
    return elements.end();
  }
  ConstIterator end() const {
    return elements.end();
  }
  ConstIterator cbegin() const {
    return elements.cbegin();
  }
  ConstIterator cend() const {
    return elements.cend();
  }

  std::pair<Iterator, bool> insert(const NodeType& node) {
    auto it = find(node.first);
    if (it != end()) {
      return {it, false};
    }
    if (elements.size() + 1 > sz * max_sz) {
      rehash();
    }
    size_t bucket_index = using_function(node.first) % sz;
    elements.emplace(elements.begin(), node);
    Iterator new_element_it = elements.begin();
    hash_table[bucket_index].push_back(new_element_it);
    return {new_element_it, true};
  }

  template<typename U>
  std::pair<Iterator, bool> insert(U&& node) {
    Iterator it = find(node.first);
    if (it != end()) {
      return {it, false};
    }
    if (elements.size() + 1 > sz * max_sz) {
      rehash();
    }
    size_t bucket_index = using_function(node.first) % sz;
    elements.emplace(elements.begin(), std::forward<U>(node));
    Iterator new_element_it = elements.begin();
    hash_table[bucket_index].push_back(new_element_it);
    return {new_element_it, true};
  }

  template<typename InputIterator>
  void insert(InputIterator begin, InputIterator end) {
    for (auto it = begin; it != end; ++it) {
      insert(*it);
    }
  }

  template<typename... Args>
  std::pair<Iterator, bool> emplace(Args&&... args) {
    elements.emplace_back(std::forward<Args>(args)...);
    auto it = std::prev(elements.end());
    if (find(it->first) != nullptr) {
      hash_table[using_function(it->first) % sz].push_back(it);
      if (max_sz * sz < size()) {
        sz = sz * 2 + 1;
        rehash();
      }
    }
    return std::make_pair(it, true);
  }

  void erase(ConstIterator base_iterator) {
    if (base_iterator == end()) {
      return;
    }
    size_t bucket_index = using_function(base_iterator->first) % sz;
    auto& bucket = hash_table[bucket_index];
    bucket.erase(std::remove_if(bucket.begin(), bucket.end(),
                                [&base_iterator](const Iterator& it) {
                                  return it == base_iterator;
                                }),
                 bucket.end());
    elements.erase(base_iterator);
  }

  void erase(Iterator begin, Iterator end) {
    while (begin != end) {
      Iterator next = std::next(begin);
      erase(begin);
      begin = next;
    }
  }

  Iterator find(const Key& key) {
    if (sz == 0) {
      sz = 1;
      hash_table.resize(1);
    }
    size_t bucket_index = using_function(key) % sz;
    auto& bucket = hash_table[bucket_index];
    for (auto it = bucket.begin(); it != bucket.end(); ++it) {
      if (equalizer((*it)->first, key)) {
        return *it;
      }
    }
    return elements.end();
  }

  ConstIterator find(const Key& key) const {
    find(key);
  }

  void rehash() {
    size_t new_size = 2 * sz + 1;
    using NewHashTableType = std::vector<Iterator, RebindAlloc>;
    using NewHashTableAlloc = typename AllocTraits::template rebind_alloc<std::vector<Iterator, RebindAlloc>>;
    std::vector<NewHashTableType, NewHashTableAlloc> newHashTable(new_size, std::vector<Iterator, RebindAlloc>(0), cur_alloc);
    for (auto& bucket : hash_table) {
      for (auto it = bucket.begin(); it != bucket.end(); ++it) {
        size_t new_bucket_index = using_function((*it)->first) % new_size;
        newHashTable[new_bucket_index].push_back(*it);
      }
    }
    hash_table.swap(newHashTable);
    sz = new_size;
  }

  void reserve(size_t n) {
    if (max_sz * n >= size()) {
      size_t new_hash_table_size = static_cast<size_t>(std::ceil(n / max_sz));
      if (new_hash_table_size > sz) {
        sz = new_hash_table_size;
        rehash();
      }
    }
  }

  [[nodiscard]] double load_factor() const {
    return static_cast<double>(elements.size()) / static_cast<double>(hash_table.size());
  }

  [[nodiscard]] double max_load_factor() const {
    return max_sz;
  }

  void swap(const UnorderedMap& other) noexcept {
    std::swap(hash_table, other.hash_table);
    std::swap(elements, other.elements);
    std::swap(sz, other.sz);
    std::swap(using_function, other.using_function);
    std::swap(equalizer, other.equalizer);
    if (AllocTraits::propagate_on_container_copy_assignment::value) {
      cur_alloc = other.cur_alloc;
    }
  }
};

template<typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::UnorderedMap() = default;

template<typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::UnorderedMap(const UnorderedMap& other)
    : sz(other.sz), elements(other.elements), using_function(other.using_function),
      equalizer(other.equalizer), cur_alloc(other.cur_alloc) {
  rehash();
}


template<typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::UnorderedMap(UnorderedMap&& other)
    : sz(other.sz), hash_table(std::move(other.hash_table)), elements(std::move(other.elements)),
      using_function(std::move(other.using_function)), equalizer(std::move(other.equalizer)),
      cur_alloc(std::move(other.cur_alloc)) {}

template<typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::~UnorderedMap() = default;


template<typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
UnorderedMap<Key, Value, Hash, Equal, Alloc>& UnorderedMap<Key, Value, Hash, Equal, Alloc>::operator=(const UnorderedMap& other) {
  if (this == &other) {
    return *this;
  }
  UnorderedMap tmp(other);
  swap(tmp);
  return *this;
}


template<typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
UnorderedMap<Key, Value, Hash, Equal, Alloc>& UnorderedMap<Key, Value, Hash, Equal, Alloc>::operator=(UnorderedMap&& other) {
  if (this == &other) {
    return *this;
  }
  if (AllocTraits::propagate_on_container_move_assignment::value) {
    cur_alloc = std::move(other.cur_alloc);
  }
  hash_table = std::move(other.hash_table);
  elements = std::move(other.elements);
  sz = other.sz;
  using_function = std::move(other.using_function);
  equalizer = std::move(other.equalizer);
  return *this;
}

template<typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
Value& UnorderedMap<Key, Value, Hash, Equal, Alloc>::operator[](const Key& key) {
  Iterator found_it = find(key);
  if (found_it != end()) {
    return found_it->second;
  }
  if (elements.size() + 1 > sz * max_sz) {
    rehash();
  }

  size_t bucket_index = using_function(key) % sz;
  elements.emplace(elements.begin(), key, Value());
  Iterator new_element_it = elements.begin();
  hash_table[bucket_index].push_back(new_element_it);

  return new_element_it->second;
}

template<typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
Value& UnorderedMap<Key, Value, Hash, Equal, Alloc>::at(const Key& key) {
  size_t bucket_index = using_function(key) % sz;
  for (const auto& it : hash_table[bucket_index]) {
    if (equalizer(it->first, key)) {
      return it->second;
    }
  }

  throw std::out_of_range("No such key");
}

template<typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
const Value& UnorderedMap<Key, Value, Hash, Equal, Alloc>::at(const Key& key) const {
  size_t bucket_index = using_function(key) % sz;
  for (const auto& it : hash_table[bucket_index]) {
    if (equalizer(it->first, key)) {
      return it->second;
    }
  }
  throw std::out_of_range("No such key");
}