#include <iostream>
template <typename T>
class Deque {
 private:
  T** main_f;
  T** main_l;
  T* internal_f;
  T* internal_l;
  T** data;
  size_t size_;
  size_t buckets_cnt;
  static const size_t sz = 32;

  template <typename... Args>
  T* createObject(T* ptr, Args&&... args) {
    try {
      return new(ptr) T(std::forward<Args>(args)...);
    }
    catch (...) {
      throw;
    }
  }

  void raw_memory(size_t n) {
    T** new_data = nullptr;
    size_t progress = 0;

    try {
      new_data = new T*[n];
      for (size_t i = 0; i < n; ++i) {
        new_data[i] = reinterpret_cast<T*>(new uint8_t[sz * sizeof(T)]);
        ++progress;
      }

      data = new_data;
      size_ = 0;
      buckets_cnt = n;
      main_f = data;
      main_l = data;
    } catch (...) {
      for (size_t j = 0; j < progress; ++j) {
        delete[] reinterpret_cast<uint8_t*>(new_data[j]);
      }
      delete[] new_data;
      throw;
    }
  }


  void reallocate(size_t n) {
    T** new_data = nullptr;
    size_t progress = 0;

    try {
      new_data = new T*[3 * n];

      for (size_t i = 0; i < n; ++i) {
        new_data[i] = reinterpret_cast<T*>(new uint8_t[sz * sizeof(T)]);
        ++progress;
      }

      for (size_t i = n; i < 2 * n; ++i) {
        new_data[i] = data[i - n];
      }

      progress = 2 * n;

      for (size_t i = 2 * n; i < 3 * n; ++i) {
        new_data[i] = reinterpret_cast<T*>(new uint8_t[sz * sizeof(T)]);
        ++progress;
      }

      int delta_1 = main_f - data;
      int delta_2 = main_l - main_f;
      delete[] data;

      data = new_data;
      buckets_cnt = 3 * n;
      main_f = data + n + delta_1;
      main_l = main_f + delta_2;
    } catch (...) {
      for (size_t i = 0; i < progress; ++i) {
        if (i < n || i >= 2 * n) {
          delete[] reinterpret_cast<uint8_t*>(new_data[i]);
        }
      }
      delete[] new_data;

      throw;
    }
  }


 public:
  template<bool is_const>
  struct base_iterator {
   private:
    static const size_t bucket_size = 32;
    T** this_data;
    typename std::conditional<is_const, const T*, T*>::type curr;

   public:
    using difference_type = ptrdiff_t;
    using value_type = typename std::conditional<is_const, const T, T>::type;
    using pointer = typename std::conditional<is_const, const T*, T*>::type;
    using reference = typename std::conditional<is_const, const T&, T&>::type;
    using const_reference = typename std::conditional<is_const, const T&, T&>::type;
    using iterator_category = std::random_access_iterator_tag;

    base_iterator(T** this_data, pointer curr): this_data(this_data), curr(curr) {}

    operator base_iterator<true>() const {
      return base_iterator<true>(this_data, curr);
    }

    base_iterator& operator ++ () {
      return *this += 1;
    }

    base_iterator& operator -- () {
      return *this -= 1;
    }

    base_iterator operator ++ (int) {
      base_iterator cp = *this;
      ++*this;
      return cp;
    }

    base_iterator operator -- (int) {
      base_iterator cp = *this;
      --*this;
      return cp;
    }

    base_iterator& operator += (difference_type x) {
      if (x >= 0) {
        x += curr - *this_data;
        this_data += x / bucket_size;
        curr = *this_data + x % bucket_size;
      } else {
        *this -= -x;
      }
      return *this;
    }

    base_iterator& operator -= (difference_type x) {
      if (x >= 0) {
        x += *this_data + bucket_size - 1 - curr;
        this_data -= x / bucket_size;
        curr = *this_data + bucket_size - 1 - x % bucket_size;
      } else {
        *this += -x;
      }
      return *this;
    }

    bool operator < (const base_iterator& a) const {
      return this_data < a.this_data or (this_data == a.this_data and curr < a.curr);
    }

    bool operator > (const base_iterator& a) const {
      return a < *this;
    }

    bool operator <= (const base_iterator& a) const {
      return !(a < *this);
    }

    bool operator >= (const base_iterator& a) const {
      return !(a > *this);
    }

    bool operator == (const base_iterator& a) const {
      return curr == a.curr;
    }

    bool operator != (const base_iterator& a) const {
      return !(*this == a);
    }

    base_iterator operator + (difference_type x) const {
      base_iterator cp = *this;
      cp += x;
      return cp;
    }

    base_iterator operator - (difference_type x) const {
      base_iterator cp = *this;
      cp -= x;
      return cp;
    }

    difference_type operator - (const base_iterator& a) const {
      return (this_data - a.this_data) * bucket_size + (curr - *this_data) - (a.curr - *a.this_data);
    }

    reference operator * () const {
      return *curr;
    }

    pointer operator -> () const {
      return curr;
    }
  };

  void destroyAndDeallocate(typename Deque<T>::template base_iterator<false> it) {
    for (typename Deque<T>::template base_iterator<false> jt = begin(); jt != it; ++jt) {
      jt->~T();
    }
    for (size_t i = 0; i < buckets_cnt; ++i) {
      delete[] reinterpret_cast<uint8_t*>(data[i]);
    }
    delete[] data;
    internal_f = *data;
    internal_l = *data;
    size_ = 0;
  }

 public:
  using iterator = base_iterator<false>;
  using const_iterator = base_iterator<true>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  iterator begin() {
    return iterator(main_f, internal_f);
  }

  const_iterator begin() const {
    return cbegin();
  }

  iterator end() {
    if (size() == 0) {
      return iterator(main_l, internal_l);
    } else if (internal_l - sz + 1 == *main_l) {
      return iterator(main_l + 1, *(main_l + 1));
    } else {
      return iterator(main_l, internal_l + 1);
    }
  }

  const_iterator end() const {
    return cend();
  }

  const_iterator cbegin() const {
    return const_iterator(main_f, internal_f);
  }

  const_iterator cend() const {
    if (size() == 0) {
      return const_iterator(main_l, internal_l);
    } else if (internal_l - sz + 1 == *main_l) {
      return const_iterator(main_l + 1, *(main_l + 1));
    } else {
      return const_iterator(main_l, internal_l + 1);
    }
  }

  reverse_iterator rbegin() {
    return reverse_iterator(end());
  }

  const_reverse_iterator rbegin() const {
    return const_reverse_iterator(end());
  }

  reverse_iterator rend() {
    return reverse_iterator(begin());
  }

  const_reverse_iterator rend() const {
    return const_reverse_iterator(begin());
  }

  const_reverse_iterator crbegin() const {
    return const_reverse_iterator(cend());
  }

  const_reverse_iterator crend() const {
    return const_reverse_iterator(cbegin());
  }

  void insert(iterator it, const T& x) {
    push_back(x);
    for (iterator jt = end() - 1; jt.operator->() != it.operator->(); --jt) {
      std::swap(*jt, *(jt - 1));
    }
  }

  void erase(iterator it) {
    for (iterator jt = it + 1; jt != end(); ++jt) {
      std::swap(*jt, *(jt - 1));
    }
    pop_back();
  }

  void swap(Deque<T>& a) {
    std::swap(main_f, a.main_f);
    std::swap(main_l, a.main_l);
    std::swap(internal_f, a.internal_f);
    std::swap(internal_l, a.internal_l);
    std::swap(data, a.data);
    std::swap(size_, a.size_);
    std::swap(buckets_cnt, a.buckets_cnt);
  }

  Deque() {
    raw_memory(1);
    internal_f = *data;
    internal_l = *data;
  }

  Deque(size_t count, const T& x) {
    raw_memory(int((count + sz - 1) / sz));
    internal_f = *data;
    size_ = count;
    for (base_iterator it = begin(); it < begin() + count; ++it) {
      try {
        createObject(it.operator->(), x);
      }
      catch (...) {
        destroyAndDeallocate(it);
        throw;
      }
    }
    main_f = data;
    main_l = data + std::max(int((count + sz - 1) / sz) - 1, 0);
    if (size_ == 0) {
      internal_l = *main_l;
    } else {
      internal_l = *main_l + (count % sz) - 1;
    }
  }

  explicit Deque(int count) {
    raw_memory(int((count + sz - 1) / sz) + 1);
    internal_f = *data;
    size_ = count;
    for (base_iterator it = begin(); it < begin() + count; ++it) {
      try {
        createObject(it.operator->());
      }
      catch (...) {
        destroyAndDeallocate(it);
        throw;
      }
    }
    main_f = data;
    main_l = data + std::max(int((count + sz - 1) / sz) - 1, 0);
    if (size_ == 0) {
      internal_l = *main_l;
    } else {
      internal_l = *main_l + (count % sz) - 1;
    }
  }

  Deque(const Deque<T>& a) {
    raw_memory(int((a.size_ + sz - 1) / sz));
    internal_f = *data;
    for (size_t i = 0; i < a.size(); ++i) {
      try {
        createObject(&(operator[](i)), a[i]);
      } catch (...) {
        for (size_t j = 0; j < i; ++j) {
          (operator[](j)).~T();
        }
        for (size_t e = 0; e < buckets_cnt; ++e) {
          delete[] reinterpret_cast<uint8_t*>(data[i]);
        }
        size_ = 0;
        delete[] data;
        throw;
      }
    }
    size_ = a.size_;
    main_f = data;
    main_l = data + (a.size_ / sz);
    if (size_ == 0) {
      internal_l = *main_l;
    } else {
      internal_l = *main_l + (a.size_ % sz) - 1;
    }
  }

  Deque& operator=(const Deque& a) {
    if (this == &a) {
      return *this;
    }
    Deque cp = a;
    swap(cp);
    return *this;
  }

  size_t size() const {
    return size_;
  }

  T& operator[](const size_t x) {
    return *(begin() + x);
  }

  const T& operator[](const size_t x) const {
    return *(begin() + x);
  }

  T& at(const size_t x) {
    if (x >= size()) {
      throw std::out_of_range("Out of range");
    }
    return Deque<T>::operator[](x);
  }

  const T& at(const size_t x) const {
    if (x >= size()) {
      throw std::out_of_range("Out of range");
    }
    return Deque<T>::operator[](x);
  }

  void push_back(const T& x) {
    if (size_ != 0 && main_l == data + buckets_cnt - 1 && internal_l + 1 == *main_l + sz - 1) {
      reallocate(buckets_cnt);
    }
    if (internal_l + 1 - sz == *main_l) {
      try {
        createObject((end()).operator->(), x);
        main_l += 1;
        internal_l = *main_l;
        size_ += 1;
      }
      catch (...) {
        throw;
      }
    }
    else {
      try {
        if (size_ == 0) {
          new((begin()).operator->()) T(x);
          size_ += 1;
        } else {
          createObject((end()).operator->(), x);
          internal_l += 1;
          size_ += 1;
        }
      }
      catch (...) {
        throw;
      }
    }
  }

  void pop_back() {
    if (size_ == 1) {
      (begin())->~T();
    } else if (internal_l == *main_l) {
      (--end())->~T();
      main_l -= 1;
      internal_l = *main_l + sz - 1;
    } else {
      internal_l -= 1;
      (end())->~T();
    }
    size_ -= 1;
  }

  void push_front(const T& x) {
    if (internal_f == *main_f && size_ != 0) {
      if (main_f == data) {
        reallocate(buckets_cnt);
      }
      main_f -= 1;
      internal_f = *main_f + sz - 1;
      try {
        createObject((begin()).operator->(), x);
        size_ += 1;
      }
      catch (...) {
        main_f += 1;
        internal_f = *main_f;
        throw;
      }
    }
    else {
      try {
        if (size_ == 0) {
          createObject((begin()).operator->(), x);
          size_ += 1;
        } else {
          internal_f -= 1;
          createObject((begin()).operator->(), x);
          size_ += 1;
        }
      }
      catch (...) {
        if (size_ != 0) {
          internal_f += 1;
        }
        throw;
      }
    }
  }

  void pop_front() {
    if (size_ == 1) {
      (begin())->~T();
    } else if (internal_f + 1 - sz == *main_f) {
      (begin())->~T();
      main_f += 1;
      internal_f = *main_f;
    } else {
      (begin())->~T();
      internal_f += 1;
    }
    size_ -= 1;
  }

  ~Deque() {
    for (base_iterator it = begin(); it != end(); ++it) {
      it->~T();
    }
    for (size_t i = 0; i < buckets_cnt; ++i) {
      delete[] reinterpret_cast<uint8_t*>(data[i]);
    }
    delete[] data;
  }
};