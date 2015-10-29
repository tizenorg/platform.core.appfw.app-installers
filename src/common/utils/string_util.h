// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_UTILS_STRING_UTIL_H_
#define COMMON_UTILS_STRING_UTIL_H_

#include <string>

namespace common_installer {

std::string DecodePercentEscapedCharacter(const std::string& path);
const unsigned int GetComparableVersion(const std::string& version);

}  // namespace common_installer

#endif  // COMMON_UTILS_STRING_UTIL_H_
