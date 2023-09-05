#include <iostream>
#include <iterator>
#include <vector>

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

  using base_node_alloc = typename std::allocator_traits<Allocator>::template rebind_alloc<BaseNode>;
  using node_alloc = typename std::allocator_traits<Allocator>::template rebind_alloc<Node>;
  BaseNode *head;
  size_t sz = 0;
  node_alloc cur_alloc;

  template<bool is_const>
  struct base_iterator {
    BaseNode *ptr;
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

    reference operator->() const {
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
  List();
  List(size_t, const T&, const Allocator&);
  explicit List(size_t);
  List(size_t, const T&);
  explicit List(const Allocator&);
  List(size_t, const Allocator&);
  List(const List&);
  ~List();
  List& operator=(const List&);

  void push_back(const T&);
  void push_front(const T&);
  void pop_back();
  void pop_front();

  using iterator = base_iterator<false>;
  using const_iterator = base_iterator<true>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

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

  node_alloc get_allocator() const {
    return cur_alloc;
  }
  [[nodiscard]] size_t size() const {
    return sz;
  }
};

template<typename T, typename Allocator>
void List<T, Allocator>::insert(const_iterator position, const T& value) {
  assert(position.ptr != nullptr);
  Node* added = std::allocator_traits<node_alloc>::allocate(cur_alloc, 1);
  try {
    std::allocator_traits<node_alloc>::construct(cur_alloc, added, value, position.ptr->prev, position.ptr);
  } catch (...) {
    std::allocator_traits<node_alloc>::deallocate(cur_alloc, added, 1);
    throw;
  }
  position.ptr->prev->next = added;
  position.ptr->prev = added;
  ++sz;
}


template<typename T, typename Allocator>
void List<T, Allocator>::erase(const_iterator position) {
  Node* to_remove = static_cast<Node*>(position.ptr);
  to_remove->prev->next = to_remove->next;
  to_remove->next->prev = to_remove->prev;
  std::allocator_traits<node_alloc>::destroy(cur_alloc, to_remove);
  std::allocator_traits<node_alloc>::deallocate(cur_alloc, to_remove, 1);
  --sz;
}

template<typename T, typename Allocator>
List<T, Allocator>::List() : cur_alloc() {
  head_create();
}

template<typename T, typename Allocator>
List<T, Allocator>::List(size_t n, const T& value, const Allocator& allocator) : cur_alloc(allocator) {
  head_create();

  try {
    for (size_t i = 0; i < n; ++i) {
      insert(end(), value);
    }
  } catch (...) {
    remove();
    throw;
  }
}

template<typename T, typename Allocator>
List<T, Allocator>::List(size_t n) : List(n, Allocator()) {}

template<typename T, typename Allocator>
List<T, Allocator>::List(const List& other) {
  size_t prev_sz = sz;
  BaseNode* prev_head = head;
  base_node_alloc prev_alloc = cur_alloc;
  cur_alloc = Allocator();
  cur_alloc = std::allocator_traits<Allocator>::select_on_container_copy_construction(other.get_allocator());
  head_create();
  sz = 0;
  try {
    for (const auto& value : other) {
      insert(end(), value);
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
  head_create();
  try {
    for (size_t i = 0; i < n; ++i) {
      Node* added = std::allocator_traits<node_alloc>::allocate(cur_alloc, 1);
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
  if (std::allocator_traits<base_node_alloc>::propagate_on_container_copy_assignment::value) {
    cur_alloc = other.cur_alloc;
  }
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