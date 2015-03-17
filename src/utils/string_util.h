// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#ifndef UTILS_STRING_UTIL_H_
#define UTILS_STRING_UTIL_H_

#include <string>

namespace common_installer {
namespace utils {

std::string DecodePercentEscapedCharacter(const std::string& path);

}  // namespace utils
}  // namespace common_installer

#endif  // UTILS_STRING_UTIL_H_
