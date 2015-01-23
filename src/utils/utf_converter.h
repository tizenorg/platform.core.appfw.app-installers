// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.
// Part of this file is redistributed from crosswalk project under BDS-style
// license. Check LICENSE-xwalk file.

#ifndef UTILS_UTF_CONVERTER_H_
#define UTILS_UTF_CONVERTER_H_

// This is an extension to use iconv for dealing with character encodings.
// This header is needed for conversions between various character sets.
#include <ext/codecvt_specializations.h>

#include <string>

// With this macro we can determine endianess
#define IS_BIG_ENDIAN (*(uint16_t *)"\0\xff" < 0x100)

namespace common_installer {
namespace utils {

struct UTF8 {
  static const char* iconvName() { return "UTF-8"; }
};

struct UTF16 {
  static const char* iconvName() {
    return IS_BIG_ENDIAN ? "UTF-16BE" : "UTF-16LE";
  }
};

namespace utf_converter {
  bool IsStringUTF8(const std::string& str);
  bool IsStringASCII(const std::string& str);

  std::string UTF16ToUTF8(const std::u16string& str);
  std::u16string UTF8ToUTF16(const std::string& str);
}  // namespace UtfConverter

}  // namespace utils
}  // namespace common_installer

#endif  // UTILS_UTF_CONVERTER_H_
