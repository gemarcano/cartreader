#ifndef UNIQUE_PTR_H_
#define UNIQUE_PTR_H_

template<class T>
class unique_ptr {
public:
  unique_ptr()
  :data(nullptr)
  {}

  explicit unique_ptr(T* data)
  :data(data)
  {}

  unique_ptr(unique_ptr&& ptr)
  :data(ptr.data)
  {
    ptr.data = nullptr;
  }

  unique_ptr& operator=(unique_ptr&& ptr)
  {
    if (data)
      delete(data);
    data = ptr.data;
    ptr.data = nullptr;
    return *this;
  }

  unique_ptr(const unique_ptr&) = delete;
  unique_ptr& operator=(const unique_ptr&) = delete;

  ~unique_ptr()
  {
    delete(data);
  }

  T* get()
  {
    return data;
  }

  T* operator->()
  {
    return data;
  }

  T& operator*()
  {
    return *data;
  }

  T* release() {
    T* result = data;
    data = nullptr;
    return result;
  }

  void reset(T *new_) {
    if (data)
      delete(data);
    data = new_;
  }

  explicit operator bool() const {
    return !!data;
  }

private:
  T* data;
};

#endif//UNIQUE_PTR_H_
