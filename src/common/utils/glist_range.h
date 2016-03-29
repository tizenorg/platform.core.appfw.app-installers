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

    __attribute__ ((visibility ("default"))) explicit Iterator(std::nullptr_t ptr = nullptr) : ptr_(ptr) { }
    explicit Iterator(GList* ptr) : ptr_(ptr) { }
    explicit operator bool() const {
      return ptr_;
    }
    __attribute__ ((visibility ("default"))) const reference& operator*() const {
      return reinterpret_cast<const T&>(ptr_->data);
    }
    __attribute__ ((visibility ("default"))) reference& operator*() {
      return reinterpret_cast<T&>(ptr_->data);
    }
    __attribute__ ((visibility ("default"))) const_pointer operator->() const {
      return reinterpret_cast<pointer>(&ptr_->data);
    }
    __attribute__ ((visibility ("default"))) pointer operator->() {
      return reinterpret_cast<pointer>(&ptr_->data);
    }
    __attribute__ ((visibility ("default"))) Iterator& operator++() {
      ptr_ = g_list_next(ptr_);
      return *this;
    }
    __attribute__ ((visibility ("default"))) Iterator operator++(int) {
      Iterator iter(ptr_);
      ptr_ = g_list_next(ptr_);
      return iter;
    }
    __attribute__ ((visibility ("default"))) bool operator==(const Iterator& other) const {
      return ptr_ == other.ptr_;
    }
    __attribute__ ((visibility ("default"))) bool operator!=(const Iterator& other) const {
      return !this->operator==(other);
    }

   private:
    GList* ptr_;
  };

  __attribute__ ((visibility ("default"))) explicit GListRange(GList* list) : list_(list) {  }
  __attribute__ ((visibility ("default"))) Iterator begin() {
    return Iterator(list_);
  }
  __attribute__ ((visibility ("default"))) Iterator end() {
    return Iterator();
  }

 private:
  GList* list_;
};

#endif  // COMMON_UTILS_GLIST_RANGE_H_
