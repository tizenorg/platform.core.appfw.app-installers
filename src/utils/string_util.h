// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.
// Part of this file is redistributed from crosswalk project under BSD-style
// license. Check LICENSE-xwalk file.

#ifndef UTILS_STRING_UTIL_H_
#define UTILS_STRING_UTIL_H_

#include <string>

namespace common_installer {
namespace utils {

std::string CollapseWhitespace(const std::string& text,
                               bool trim_sequences_with_line_breaks);
std::u16string CollapseWhitespace(const std::u16string& text,
                               bool trim_sequences_with_line_breaks);
std::u16string StripWrappingBidiControlCharacters(const std::u16string& text);
std::u16string GetDirText(const std::u16string& text, const std::string& dir);
void WrapStringWithLTRFormatting(std::u16string* text);
bool AdjustStringForLocaleDirection(std::u16string* text);
}  // namespace utils
}  // namespace common_installer

#endif  // UTILS_STRING_UTIL_H_
