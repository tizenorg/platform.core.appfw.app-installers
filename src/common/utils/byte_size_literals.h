// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_UTILS_BYTE_SIZE_LITERALS_H_
#define COMMON_UTILS_BYTE_SIZE_LITERALS_H_

__attribute__ ((visibility ("default"))) constexpr unsigned long long operator"" _kB(unsigned long long v) {  // NOLINT
  return v * (1 << 10);
}

__attribute__ ((visibility ("default"))) constexpr unsigned long long operator"" _MB(unsigned long long v) {  // NOLINT
  return v * (1 << 20);
}

__attribute__ ((visibility ("default"))) constexpr unsigned long long operator"" _GB(unsigned long long v) {  // NOLINT
  return v * (1 << 30);
}

#endif  // COMMON_UTILS_BYTE_SIZE_LITERALS_H_

