// Copyright 2013 The Chromium Authors. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#include "utils/string_util.h"

#include <cstdlib>
#include <string>
#include <vector>

namespace {

const char kRightToLeftMark[] = u8"\xE2\x80\x8F";
const char kLeftToRightMark[] = u8"\xE2\x80\x8E";
const char kLeftToRightEmbeddingMark[] = u8"\xE2\x80\xAA";
const char kRightToLeftEmbeddingMark[] = u8"\xE2\x80\xAB";
const char kPopDirectionalFormatting[] = u8"\xE2\x80\xAC";
const char kLeftToRightOverride[] = u8"\xE2\x80\xAD";
const char kRightToLeftOverride[] = u8"\xE2\x80\xAE";

const unsigned kBidiControlCharacterLength = 3;

const char kDirLTRKey[] = "ltr";
const char kDirRTLKey[] = "rtl";
const char kDirLROKey[] = "lro";
const char kDirRLOKey[] = "rlo";

const char* kWhitespaceSequences[] = {
  u8"\x09",          /* CHARACTER TABULATION */
  u8"\x0A",          /* LINE FEED (LF) */
  u8"\x0B",          /* LINE TABULATION */
  u8"\x0C",          /* FORM FEED (FF) */
  u8"\x0D",          /* CARRIAGE RETURN (CR) */
  u8"\x20",          /* SPACE */
  u8"\xc2\x85",      /* NEXT LINE (NEL) */
  u8"\xc2\xa0",      /* NO-BREAK SPACE */
  u8"\xe1\x9a\x80",  /* OGHAM SPACE MARK */
  u8"\xe2\x80\x80",  /* EN QUAD */
  u8"\xe2\x80\x81",  /* EM QUAD */
  u8"\xe2\x80\x82",  /* EN SPACE */
  u8"\xe2\x80\x83",  /* EM SPACE */
  u8"\xe2\x80\x84",  /* THREE-PER-EM SPACE */
  u8"\xe2\x80\x85",  /* FOUR-PER-EM SPACE */
  u8"\xe2\x80\x86",  /* SIX-PER-EM SPACE */
  u8"\xe2\x80\x87",  /* FIGURE SPACE */
  u8"\xe2\x80\x88",  /* PUNCTUATION SPACE */
  u8"\xe2\x80\x89",  /* THIN SPACE */
  u8"\xe2\x80\x8A",  /* HAIR SPACE */
  u8"\xe2\x80\xa8",  /* LINE SEPARATOR */
  u8"\xe2\x80\xa9",  /* PARAGRAPH SEPARATOR */
  u8"\xe2\x80\xaf",  /* NARROW NO-BREAK SPACE */
  u8"\xe2\x81\x9f",  /* MEDIUM MATHEMATICAL SPACE */
  u8"\xe3\x80\x80"   /* IDEOGRAPHIC SPACE */
};

// Calculates length of UTF-8 character by checking first byte only
int UTF8CharLength(const char* character) {
  if (character[0] < 0x80)
      return 1;
  int length = 0;
  unsigned char mask = 0x80;
  while (character[0] & mask) {
    mask >>= 1;
    ++length;
  }
  return length;
}

// Check string offset against occurance of given UTF-8 character
bool EqualsUTF8Char(const char* str, const char* character) {
  unsigned i = 0;
  while (character[i]) {
    if (!str[i]) {
      return false;
    }
    if (character[i] != str[i]) {
      return false;
    }
    ++i;
  }
  return true;
}

// Returns true if it's a whitespace character.
inline bool IsWhitespaceUTF8(const char* c) {
  for (unsigned i = 0;
       i < sizeof(kWhitespaceSequences) / sizeof(kWhitespaceSequences[0]);
       ++i) {
    if (EqualsUTF8Char(c, kWhitespaceSequences[i])) {
      return true;
    }
  }
  return false;
}

}  // namespace

namespace utils {

std::string CollapseWhitespaceUTF8(const std::string& text) {
  std::string result;
  result.resize(text.size());

  // Set flags to pretend we're already in a trimmed whitespace sequence, so we
  // will trim any leading whitespace.
  bool in_whitespace = true;
  int chars_written = 0;

  for (unsigned i = 0; i < text.length();) {
    int length = UTF8CharLength(&text[i]);
    if (IsWhitespaceUTF8(&text[i])) {
      if (!in_whitespace) {
        // Reduce all whitespace sequences to a single space.
        in_whitespace = true;
        result[chars_written++] = ' ';
      }
      // roll through UTF8 character
      i += length;
    } else {
      // Non-whitespace chracters are copied straight across.
      in_whitespace = false;
      while (length--) {
        result[chars_written++] = text[i++];
      }
    }
  }

  if (in_whitespace) {
    // Any trailing whitespace is eliminated.
    --chars_written;
  }

  result.resize(chars_written);
  return result;
}

std::string StripWrappingBidiControlCharactersUTF8(const std::string& text) {
  if (text.empty())
    return text;
  size_t begin_index = 0;
  const char* begin = &text[begin_index];
  if (EqualsUTF8Char(begin, kLeftToRightEmbeddingMark) ||
      EqualsUTF8Char(begin, kRightToLeftEmbeddingMark) ||
      EqualsUTF8Char(begin, kLeftToRightOverride) ||
      EqualsUTF8Char(begin, kRightToLeftOverride))
    begin_index += kBidiControlCharacterLength;
  size_t end_index = text.length() - kBidiControlCharacterLength;
  if (EqualsUTF8Char(&text[end_index], kPopDirectionalFormatting))
    end_index -= 3;
  return text.substr(begin_index,
      end_index - begin_index + kBidiControlCharacterLength);
}

std::string GetDirTextUTF8(const std::string& text, const std::string& dir) {
  if (dir == kDirLTRKey)
    return std::string(kLeftToRightEmbeddingMark)
           + text
           + kPopDirectionalFormatting;

  if (dir == kDirRTLKey)
    return std::string(kRightToLeftEmbeddingMark)
           + text
           + kPopDirectionalFormatting;

  if (dir == kDirLROKey)
    return std::string(kLeftToRightOverride)
           + text
           + kPopDirectionalFormatting;

  if (dir == kDirRLOKey)
    return std::string(kRightToLeftOverride)
           + text
           + kPopDirectionalFormatting;

  return text;
}

std::string DecodePercentEscapedCharacter(const std::string& path) {
  std::vector<int> input(path.begin(), path.end());
  std::vector<char> output;
  unsigned i = 0;
  while (i < input.size()) {
    if ('%' == input[i]) {
      if (i + 2 >= input.size())
        return std::string();
      char str[3] = {"\0", };
      str[0] = input[i + 1];
      str[1] = input[i + 2];
      int result = strtol(str, NULL, 16);
      // RFC 1738 - octets 80 to FF are not allowed
      if (result >= 128)
        return std::string();
      output.push_back(static_cast<char>(result));
      i += 3;
    } else {
      output.push_back(static_cast<char>(input[i]));
      ++i;
    }
  }
  return std::string(output.begin(), output.end());
}

}  // namespace utils
