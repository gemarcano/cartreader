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

  explicit unique_ptr(T&& ptr)
  :data(ptr.data)
  {
    ptr.data = nullptr;
  }

  T& operator=(T&& ptr)
  {
    if (data)
      delete(data);
    data = ptr.data;
    ptr.data = nullptr;
  }

  explicit unique_ptr(const T&) = delete;
  T& operator=(const T&) = delete;

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
