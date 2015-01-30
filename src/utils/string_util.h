// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#ifndef UTILS_STRING_UTIL_H_
#define UTILS_STRING_UTIL_H_

#include <string>

namespace common_installer {
namespace utils {

std::string CollapseWhitespaceUTF8(const std::string& text,
                               bool trim_sequences_with_line_breaks);
std::string StripWrappingBidiControlCharactersUTF8(const std::string& text);
std::string GetDirTextUTF8(const std::string& text, const std::string& dir);

}  // namespace utils
}  // namespace common_installer

#endif  // UTILS_STRING_UTIL_H_
