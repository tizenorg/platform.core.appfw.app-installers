// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_UTILS_STRING_UTIL_H_
#define COMMON_UTILS_STRING_UTIL_H_

#include <string>

namespace common_installer {

std::string DecodePercentEscapedCharacter(const std::string& path);

/**
 * \brief Compare two versions provided in dot separated format
 *
 * \param version version that should be compared
 * \param compareTo version to compare to
 *
 * \return -1 if version is lower,
 *         0 if both versions are equal,
 *         1 if version is higher
 */
int CompareVersion(const std::string& version, const std::string& compareTo);

}  // namespace common_installer

#endif  // COMMON_UTILS_STRING_UTIL_H_
