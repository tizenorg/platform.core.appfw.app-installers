// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#ifndef PARSER_STRING_UTIL_H_
#define PARSER_STRING_UTIL_H_

#include <string>

namespace parser {
namespace utils {

// utf8 representation of unicode BIDI characters
extern const char kRightToLeftMark[];
extern const char kLeftToRightMark[];
extern const char kLeftToRightEmbeddingMark[];
extern const char kRightToLeftEmbeddingMark[];
extern const char kPopDirectionalFormatting[];
extern const char kLeftToRightOverride[];
extern const char kRightToLeftOverride[];

std::string CollapseWhitespaceUTF8(const std::string& text);
std::string StripWrappingBidiControlCharactersUTF8(const std::string& text);
std::string GetDirTextUTF8(const std::string& text, const std::string& dir);

std::string DecodePercentEscapedCharacter(const std::string& path);

}  // namespace utils
}  // namespace parser

#endif  // PARSER_STRING_UTIL_H_
