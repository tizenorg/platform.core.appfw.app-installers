// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "utils/utf_converter.h"

#include <memory>

namespace {

bool IsAlignedToMachineWord(const void* pointer) {
  uintptr_t kMachineWordAlignmentMask = sizeof(uintptr_t) - 1;
  return !(reinterpret_cast<uintptr_t>(pointer) & kMachineWordAlignmentMask);
}

template<typename T> inline T* AlignToMachineWord(T* pointer) {
  uintptr_t kMachineWordAlignmentMask = sizeof(uintptr_t) - 1;
  return reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(pointer) &
                              ~kMachineWordAlignmentMask);
}

template<size_t size, typename CharacterType> struct NonASCIIMask;
template<> struct NonASCIIMask<4, char> {
    static inline uint32_t value() { return 0x80808080U; }
};
template<> struct NonASCIIMask<8, char> {
    static inline uint64_t value() { return 0x8080808080808080ULL; }
};

}  // namespace

namespace common_installer {
namespace utils {
namespace utf_converter {

using EncSt = __gnu_cxx::encoding_state;
using codecvt_type16 = std::codecvt<char16_t, char, EncSt>;
using codecvt_type8 = std::codecvt<char, char16_t, EncSt>;

bool IsStringUTF8(const std::string& str) {
  // TODO(jizydorczyk):
  // Implement it.
  // We need icu library from chromium to make it possible
  // to properly implement this function, so it needs to be
  // converted for our needs first.
  return true;
}

bool IsStringASCII(const std::string& str) {
    const char* characters = str.c_str();
    size_t length = str.length();
    uintptr_t all_char_bits = 0;
    const char* end = characters + length;

    // Prologue: align the input.
    while (!IsAlignedToMachineWord(characters) && characters != end) {
      all_char_bits |= *characters;
      ++characters;
    }

    // Compare the values of CPU word size.
    const char* word_end = AlignToMachineWord(end);
    const size_t loop_increment = sizeof(uintptr_t) / sizeof(char);
    while (characters < word_end) {
      all_char_bits |= *(reinterpret_cast<const uintptr_t*>(characters));
      characters += loop_increment;
    }

    // Process the remaining bytes.
    while (characters != end) {
      all_char_bits |= *characters;
      ++characters;
    }

    uintptr_t non_ascii_bit_mask =
        NonASCIIMask<sizeof(uintptr_t), char>::value();
    return !(all_char_bits & non_ascii_bit_mask);
}

std::string UTF16ToUTF8(const std::u16string& s) {
  std::locale loc(std::locale::classic(), new codecvt_type16);
  EncSt state(UTF16::iconvName(), UTF8::iconvName());
  const codecvt_type16& cvt = std::use_facet<codecvt_type16>(loc);

  const char16_t* enx;

  std::string buffer(s.size()*3, '\0');
  char* inx;

  codecvt_type16::result r =
    cvt.out(state, &s[0], &s[s.size()], enx,
        &buffer[0], &buffer[buffer.size()], inx);

  if (r != codecvt_type16::ok)
    return "";

  buffer.resize(buffer.find("\0", 0, 1));
  return buffer;
}

std::u16string UTF8ToUTF16(const std::string& s) {
  std::locale loc(std::locale::classic(), new codecvt_type8);
  EncSt state(UTF8::iconvName(), UTF16::iconvName());
  const codecvt_type8& cvt = std::use_facet<codecvt_type8>(loc);

  const char* enx;

  std::u16string buffer(s.size()*2, '\0');
  char16_t* inx;
  codecvt_type8::result r =
    cvt.out(state, &s[0], &s[s.size()], enx,
        &buffer[0], &buffer[buffer.size()], inx);

  if (r != codecvt_type8::ok)
    return std::u16string();

  buffer.resize(buffer.find(u"\0", 0, 1));
  return buffer;
}

}  // namespace UtfConverter
}  // namespace utils
}  // namespace common_installer
