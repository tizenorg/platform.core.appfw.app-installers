// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_UTILS_GLIST_RANGE_H_
#define COMMON_UTILS_GLIST_RANGE_H_

#include <glib.h>

#include <cstddef>
#include <iterator>

// Range with mutable forward iterator based on GList
// supporting language foreach construct
template<typename T>
class GListRange {
 public:
  class Iterator {
   public:
    typedef T value_type;
    typedef T& reference;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef std::size_t difference_type;
    typedef std::forward_iterator_tag iterator_category;

    explicit Iterator(std::nullptr_t ptr = nullptr) : ptr_(ptr) { }
    explicit Iterator(GList* ptr) : ptr_(ptr) { }
    explicit operator bool() const {
      return ptr_;
    }
    const reference& operator*() const {
      return reinterpret_cast<const T&>(ptr_->data);
    }
    reference& operator*() {
      return reinterpret_cast<T&>(ptr_->data);
    }
    const_pointer operator->() const {
      return reinterpret_cast<pointer>(&ptr_->data);
    }
    pointer operator->() {
      return reinterpret_cast<pointer>(&ptr_->data);
    }
    Iterator& operator++() {
      ptr_ = g_list_next(ptr_);
      return *this;
    }
    Iterator operator++(int) {
      Iterator iter(ptr_);
      ptr_ = g_list_next(ptr_);
      return iter;
    }
    bool operator==(const Iterator& other) const {
      return ptr_ == other.ptr_;
    }
    bool operator!=(const Iterator& other) const {
      return !this->operator==(other);
    }

   private:
    GList* ptr_;
  };

  explicit GListRange(GList* list) : list_(list) {  }
  Iterator begin() {
    return Iterator(list_);
  }
  Iterator end() {
    return Iterator();
  }

 private:
  GList* list_;
};

#endif  // COMMON_UTILS_GLIST_RANGE_H_
