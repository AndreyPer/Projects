#include <atomic>
#include <mutex>

template <typename T>
class SharedPtr;

template <typename T>
class WeakPtr;

template <typename T>
class EnableSharedFromThis;

template <typename T>
class SharedPtr {
  template <typename U, typename Alloc, typename... Args>
  friend SharedPtr<U> allocateShared(Alloc, Args&&...);

  template <typename U>
  friend class WeakPtr;

  template <typename U>
  friend class SharedPtr;

 public:
  struct BaseControlBlock {
    int shared_count = 0;
    int weak_count = 0;
    virtual void useDeleter() = 0;
    virtual void deallocate() = 0;
    virtual T* get() = 0;
    virtual ~BaseControlBlock() = default;
  };

  template <typename Deleter, typename Alloc>
  struct ControlBlockRegular : BaseControlBlock {
    T* object;
    Deleter d;
    Alloc alloc;

    ControlBlockRegular(T* object, Deleter d, Alloc a) :
        object(object), d(d), alloc(a) {
      this->shared_count = 1;
      this->weak_count = 0;
    }

    void useDeleter() override {
      d.operator()(object);
    }

    T* get() override {
      return object;
    }

    void deallocate() override {
      typename std::allocator_traits<Alloc>::template rebind_alloc<ControlBlockRegular> ctrl_block = alloc;
      ctrl_block.deallocate(this, 1);
    }

    ~ControlBlockRegular() = default;
  };

  template <typename U, typename Alloc>
  struct ControlBlockMakeShared : BaseControlBlock {
    U object;
    Alloc alloc;

    template<typename... Args>
    explicit ControlBlockMakeShared(Alloc a, Args&&... args) :
        object(std::forward<Args>(args)...), alloc(a) {
      this->shared_count = 1;
      this->weak_count = 0;
    };

    void useDeleter() override {
      std::allocator_traits<Alloc>::destroy(alloc, &object);
    }

    T* get() override {
      return &object;
    }

    void deallocate() override {
      typename std::allocator_traits<Alloc>::template rebind_alloc<ControlBlockMakeShared> this_block = alloc;
      this_block.deallocate(this, 1);
    }
  };

 private:
  BaseControlBlock* control_block_ = nullptr;
  T* ptr_ = nullptr;

 public:

  void increase_shared_count() {
    if (control_block_) {
      ++control_block_->shared_count;
    }
  }

  SharedPtr() = default;

  explicit SharedPtr(typename SharedPtr::BaseControlBlock* control_block) {
    control_block_ = control_block;
    ptr_ = control_block_->get();
  }

  template <typename U>
  SharedPtr(const SharedPtr<U>& other) : SharedPtr() {
    if (other.control_block_) {
      control_block_ = reinterpret_cast<BaseControlBlock*>(other.control_block_);
      ptr_ = other.get();
      increase_shared_count();
    }
  }

  SharedPtr(const SharedPtr& other) : control_block_(other.control_block_), ptr_(other.ptr_) {
    increase_shared_count();
  }

  template<typename U>
  explicit SharedPtr(SharedPtr<U>&& other) noexcept {
    control_block_ = reinterpret_cast<BaseControlBlock*>(other.control_block_);
    ptr_ = other.get();
    other.control_block_ = nullptr;
    other.ptr_ = nullptr;
  }

  SharedPtr(SharedPtr&& other) noexcept {
    control_block_ = other.control_block_;
    ptr_ = other.ptr_;
    other.control_block_ = nullptr;
    other.ptr_ = nullptr;
  }

  template<typename U>
  explicit SharedPtr(BaseControlBlock* control_block, U* object) : control_block_(control_block), ptr_(object) {};

  template<typename U>
  explicit SharedPtr(U* object) {
    control_block_ = new ControlBlockRegular<std::default_delete<U>, std::allocator<U>>(object, std::default_delete<U>(), std::allocator<U>());
    ptr_ = object;
  }

  template<typename U, typename Deleter>
  explicit SharedPtr(U* object, Deleter deleter) {
    control_block_ = new ControlBlockRegular<Deleter, std::allocator<U>>(object, deleter, std::allocator<U>());
    ptr_ = object;
  }

  template<typename U, typename Deleter, typename Alloc>
  explicit SharedPtr(U* object, Deleter deleter, Alloc alloc) : ptr_(object) {
    control_block_ = createControlBlock<U, Deleter, Alloc>(object, deleter, alloc);
  };

  template<typename U, typename Deleter, typename Alloc>
  BaseControlBlock* createControlBlock(U* object, Deleter deleter, Alloc alloc) {
    typename std::allocator_traits<Alloc>::template rebind_alloc<ControlBlockRegular<Deleter, Alloc>> this_block = alloc;
    auto* control_block = this_block.allocate(1);
    new(control_block) ControlBlockRegular<Deleter, Alloc>(object, deleter, alloc);
    return control_block;
  }

  void check_and_deallocate() {
    if (control_block_->shared_count == 0) {
      control_block_->useDeleter();
      if (control_block_->weak_count == 0) {
        control_block_->deallocate();
      }
    }
  }

  void reset(T* ptr = nullptr) {
    bool flag = false;
    if (ptr != nullptr) {
      flag = true;
    }
    if (!control_block_) {
      return;
    }
    --control_block_->shared_count;
    check_and_deallocate();
    control_block_ = nullptr;
    ptr_ = nullptr;
    if (flag) {
      *this = std::move(SharedPtr(ptr));
    }
  }

  SharedPtr& operator=(const SharedPtr& other) {
    SharedPtr(other).swap(*this);
    return *this;
  }

  template<typename U>
  SharedPtr& operator=(SharedPtr<U>&& other) noexcept {
    SharedPtr temp(std::move(other));
    temp.swap(*this);
    return *this;
  }

  T* get() const {
    if (!control_block_) {
      return nullptr;
    }
    return control_block_->get();
  }

  void swap(SharedPtr& other) {
    std::swap(control_block_, other.control_block_);
    std::swap(ptr_, other.ptr_);
  }

  [[nodiscard]] int use_count() const {
    if (!control_block_) {
      return 0;
    }
    return control_block_->shared_count;
  }

  T* operator->() const {
    return ptr_;
  }

  T& operator*() const {
    return *ptr_;
  }

  ~SharedPtr() {
    if (!control_block_) {
      return;
    }
    --control_block_->shared_count;
    check_and_deallocate();
  }
};

template <typename T, typename Alloc, typename... Args>
SharedPtr<T> allocateShared(Alloc alloc, Args&&... args) {
  using ControlBlock = typename SharedPtr<T>::template ControlBlockMakeShared<T, Alloc>;
  using ControlBlockAlloc = typename std::allocator_traits<Alloc>::template rebind_alloc<ControlBlock>;

  ControlBlockAlloc this_block = alloc;
  auto control_block = this_block.allocate(1);
  std::allocator_traits<Alloc>::construct(alloc, control_block, alloc, std::forward<Args>(args)...);
  SharedPtr<T> shared_ptr(dynamic_cast<typename SharedPtr<T>::BaseControlBlock*>(control_block),
                          dynamic_cast<typename SharedPtr<T>::BaseControlBlock*>(control_block)->get());
  if constexpr (std::is_base_of_v<EnableSharedFromThis<T>, T>) {
    T* obj_ptr = shared_ptr.get();
    obj_ptr->weak_this_ = shared_ptr;
  }

  return shared_ptr;
}

template<typename U, typename... Args>
SharedPtr<U> makeShared(Args&&... args) {
  return allocateShared<U, std::allocator<U>>(std::allocator<U>(), std::forward<Args>(args)...);
}

template <typename T>
class WeakPtr {

  template <typename U>
  friend class WeakPtr;

 private:
  typename SharedPtr<T>::BaseControlBlock* control_block_ = nullptr;
  T* ptr_ = nullptr;

 public:

  void increase_weak_count() {
    if (control_block_) {
      ++control_block_->weak_count;
    }
  }
  WeakPtr() = default;

  WeakPtr(WeakPtr<T>&& other) noexcept : control_block_(other.control_block_), ptr_(other.ptr_) {
    other.control_block_ = nullptr;
    other.ptr_ = nullptr;
  }

  WeakPtr(const WeakPtr& other) : control_block_(other.control_block_), ptr_(other.ptr_) {
    increase_weak_count();
  }

  template <typename U>
  WeakPtr(const WeakPtr<U>& other): control_block_(reinterpret_cast<typename SharedPtr<T>::BaseControlBlock*>(other.control_block_)), ptr_(static_cast<T*>(other.ptr_)) {
    increase_weak_count();
  }

  template <typename U>
  WeakPtr(WeakPtr<U>&& other): control_block_(reinterpret_cast<typename SharedPtr<T>::BaseControlBlock*>(other.control_block_)), ptr_(static_cast<T*>(other.ptr_)) {
    other.control_block_ = nullptr;
    other.ptr_ = nullptr;
  }

  template <typename U>
  WeakPtr(const SharedPtr<U>& shared) : control_block_(reinterpret_cast<typename SharedPtr<T>::BaseControlBlock*>(shared.control_block_)), ptr_(shared.ptr_) {
    increase_weak_count();
  }

  WeakPtr<T>& operator=(WeakPtr<T>&& other) {
    if (this->control_block_ == other.control_block_ && this->ptr_ == other.ptr_) {
      return *this;
    }
    control_block_ = other.control_block_;
    ptr_ = other.ptr_;
    other.control_block_ = nullptr;
    other.ptr_ = nullptr;
    return *this;
  }

  template <typename U>
  WeakPtr<T>& operator=(const WeakPtr<U>& other) {
    control_block_ = reinterpret_cast<typename SharedPtr<T>::BaseControlBlock*>(other.control_block_);
    ptr_ = static_cast<T*>(other.ptr_);
    return *this;
  }

  void check_and_deallocate() {
    if (control_block_->shared_count == 0) {
      control_block_->useDeleter();
      if (control_block_->weak_count == 0) {
        control_block_->deallocate();
      }
    }
  }

  SharedPtr<T> lock() const {
    if (!control_block_) {
      throw;
    }
    ++control_block_->shared_count;
    return SharedPtr<T>(control_block_, ptr_);
  }

  [[nodiscard]] int use_count() const {
    if (!control_block_) {
      return 0;
    }
    return control_block_->shared_count;
  }

  [[nodiscard]] bool expired() const {
    if (!control_block_) {
      return true;
    }
    return control_block_->shared_count == 0;
  }

  void reset() {
    if (control_block_) {
      if (control_block_->weak_count == 0) {
        throw;
      }
      --control_block_->weak_count;
      if (control_block_->shared_count == 0 && control_block_->weak_count == 0) {
        control_block_->deallocate();
      }
    }
    ptr_ = nullptr;
  }

  ~WeakPtr() {
    if (!control_block_) {
      return;
    }
    --control_block_->weak_count;
    if (control_block_->shared_count == 0 and control_block_->weak_count == 0) {
      control_block_->deallocate();
    }
  }
};

template <typename T>
class EnableSharedFromThis {
 private:
  template <typename U>
  friend class SharedPtr;

  mutable WeakPtr<T> weak_this_;
 public:
  SharedPtr<T> shared_from_this() {
    return SharedPtr<T>(weak_this_.lock());
  }

  SharedPtr<const T> shared_from_this() const {
    return SharedPtr<const T>(weak_this_.lock());
  }

 protected:
  EnableSharedFromThis() = default;

  EnableSharedFromThis(EnableSharedFromThis const &) {}

  EnableSharedFromThis& operator=(EnableSharedFromThis const &) {
    return *this;
  }

  virtual ~EnableSharedFromThis() = default;
};