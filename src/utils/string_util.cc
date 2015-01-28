// Copyright 2013 The Chromium Authors. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#include "utils/string_util.h"

#include <string>

namespace common_installer {
namespace utils {

#define WHITESPACE_UNICODE \
  0x0009, /* CHARACTER TABULATION */      \
  0x000A, /* LINE FEED (LF) */            \
  0x000B, /* LINE TABULATION */           \
  0x000C, /* FORM FEED (FF) */            \
  0x000D, /* CARRIAGE RETURN (CR) */      \
  0x0020, /* SPACE */                     \
  0x0085, /* NEXT LINE (NEL) */           \
  0x00A0, /* NO-BREAK SPACE */            \
  0x1680, /* OGHAM SPACE MARK */          \
  0x2000, /* EN QUAD */                   \
  0x2001, /* EM QUAD */                   \
  0x2002, /* EN SPACE */                  \
  0x2003, /* EM SPACE */                  \
  0x2004, /* THREE-PER-EM SPACE */        \
  0x2005, /* FOUR-PER-EM SPACE */         \
  0x2006, /* SIX-PER-EM SPACE */          \
  0x2007, /* FIGURE SPACE */              \
  0x2008, /* PUNCTUATION SPACE */         \
  0x2009, /* THIN SPACE */                \
  0x200A, /* HAIR SPACE */                \
  0x2028, /* LINE SEPARATOR */            \
  0x2029, /* PARAGRAPH SEPARATOR */       \
  0x202F, /* NARROW NO-BREAK SPACE */     \
  0x205F, /* MEDIUM MATHEMATICAL SPACE */ \
  0x3000, /* IDEOGRAPHIC SPACE */         \
  0

const char16_t kRightToLeftMark = 0x200F;
const char16_t kLeftToRightMark = 0x200E;
const char16_t kLeftToRightEmbeddingMark = 0x202A;
const char16_t kRightToLeftEmbeddingMark = 0x202B;
const char16_t kPopDirectionalFormatting = 0x202C;
const char16_t kLeftToRightOverride = 0x202D;
const char16_t kRightToLeftOverride = 0x202E;

const char kDirLTRKey[] = "ltr";
const char kDirRTLKey[] = "rtl";
const char kDirLROKey[] = "lro";
const char kDirRLOKey[] = "rlo";

namespace {
const wchar_t kWhitespaceWide[] = {
  WHITESPACE_UNICODE
};

// Returns true if it's a whitespace character.
inline bool IsWhitespace(wchar_t c) {
  return wcschr(kWhitespaceWide, c) != nullptr;
}

template<typename STR>
STR CollapseWhitespaceT(const STR& text,
                        bool trim_sequences_with_line_breaks) {
  STR result;
  result.resize(text.size());

  // Set flags to pretend we're already in a trimmed whitespace sequence, so we
  // will trim any leading whitespace.
  bool in_whitespace = true;
  bool already_trimmed = true;

  int chars_written = 0;
  for (typename STR::const_iterator i(text.begin()); i != text.end(); ++i) {
    if (IsWhitespace(*i)) {
      if (!in_whitespace) {
        // Reduce all whitespace sequences to a single space.
        in_whitespace = true;
        result[chars_written++] = L' ';
      }
      if (trim_sequences_with_line_breaks && !already_trimmed &&
          ((*i == '\n') || (*i == '\r'))) {
        // Whitespace sequences containing CR or LF are eliminated entirely.
        already_trimmed = true;
        --chars_written;
      }
    } else {
      // Non-whitespace chracters are copied straight across.
      in_whitespace = false;
      already_trimmed = false;
      result[chars_written++] = *i;
    }
  }

  if (in_whitespace && !already_trimmed) {
    // Any trailing whitespace is eliminated.
    --chars_written;
  }

  result.resize(chars_written);
  return result;
}
}  // namespace

std::string CollapseWhitespace(const std::string& text,
                               bool trim_sequences_with_line_breaks) {
  return CollapseWhitespaceT(text, trim_sequences_with_line_breaks);
}

std::u16string CollapseWhitespace(const std::u16string& text,
                               bool trim_sequences_with_line_breaks) {
  return CollapseWhitespaceT(text, trim_sequences_with_line_breaks);
}

std::u16string StripWrappingBidiControlCharacters(const std::u16string& text) {
  if (text.empty())
    return text;
  size_t begin_index = 0;
  char16_t begin = text[begin_index];
  if (begin == kLeftToRightEmbeddingMark ||
      begin == kRightToLeftEmbeddingMark ||
      begin == kLeftToRightOverride ||
      begin == kRightToLeftOverride)
    ++begin_index;
  size_t end_index = text.length() - 1;
  if (text[end_index] == kPopDirectionalFormatting)
    --end_index;
  return text.substr(begin_index, end_index - begin_index + 1);
}

std::u16string GetDirText(const std::u16string& text, const std::string& dir) {
  if (dir == kDirLTRKey)
    return kLeftToRightEmbeddingMark
           + text
           + kPopDirectionalFormatting;

  if (dir == kDirRTLKey)
    return kRightToLeftEmbeddingMark
           + text
           + kPopDirectionalFormatting;

  if (dir == kDirLROKey)
    return kLeftToRightOverride
           + text
           + kPopDirectionalFormatting;

  if (dir == kDirRLOKey)
    return kRightToLeftOverride
           + text
           + kPopDirectionalFormatting;

  return text;
}

}  // namespace utils
}  // namespace common_installer
