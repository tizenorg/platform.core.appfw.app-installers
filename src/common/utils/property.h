// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_UTILS_PROPERTY_H_
#define COMMON_UTILS_PROPERTY_H_

#include <memory>

/** Template class for defining smart attributes.
 *
 *  Property should be used when, given attribute needs to have pure
 *  setter and getter. This template class will generate getter and setter.
 *  It uses operator() overloading.
 */
template<typename Type>
class Property {
 public:
  __attribute__ ((visibility ("default"))) Property() {}
  __attribute__ ((visibility ("default"))) Property(const Type &val): value_(val) { } // NOLINT
  __attribute__ ((visibility ("default"))) Property(Type &&val): value_(std::move(val)) { } // NOLINT
  __attribute__ ((visibility ("default"))) const Type& get() const { return value_; }
  __attribute__ ((visibility ("default"))) Type& get() { return value_; }
  __attribute__ ((visibility ("default"))) void set(const Type &val) { value_ = val; }
  __attribute__ ((visibility ("default"))) void set(Type &&val) { value_ = std::move(val); }
 private:
  Type value_;
};

#endif  // COMMON_UTILS_PROPERTY_H_
