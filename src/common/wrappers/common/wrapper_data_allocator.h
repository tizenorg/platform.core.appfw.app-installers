#ifndef WRAPPER_DATA_ALLOCATOR_H_
#define WRAPPER_DATA_ALLOCATOR_H_

#include <string>
#include <memory>

template <typename T>
class WrapperDataAllocator {
 public:
  static const std::string kTrueAsString;
  static const std::string kFalseAsString;

  explicit WrapperDataAllocator();
  void Init(T value, const char **data);
  ~WrapperDataAllocator();

  const std::string ToString() const;

 private:
  const char *data_;

  const std::string ToString(bool data);
  const std::string ToString(int data);
  const std::string ToString(const std::string &data);
};

template <typename T>
const std::string WrapperDataAllocator<T>::kTrueAsString = "true";

template <typename T>
const std::string WrapperDataAllocator<T>::kFalseAsString = "false";

template <typename T>
WrapperDataAllocator<T>::WrapperDataAllocator():data_(nullptr){
}

template <typename T>
WrapperDataAllocator<T>::~WrapperDataAllocator() {
  if (data_) {
    free((void *)data_);
  }
}

template <typename T>
const std::string WrapperDataAllocator<T>::ToString() const {
  return std::string(data_);
}

template <typename T>
const std::string WrapperDataAllocator<T>::ToString(bool data) {
  return data ? WrapperDataAllocator::kTrueAsString
              : WrapperDataAllocator::kFalseAsString;
}

template <typename T>
const std::string WrapperDataAllocator<T>::ToString(int data) {
  return std::to_string(data);
}

template <typename T>
const std::string WrapperDataAllocator<T>::ToString(const std::string &data) {
  return data;
}

template <typename T>
void WrapperDataAllocator<T>::Init(T value, const char **data) {
    *data = strdup(ToString(value).c_str());
    data_ = *data;

}

#endif  // WRAPPER_DATA_ALLOCATOR_H
