// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache-2.0 license that can be
// found in the LICENSE file.

#include <gtest/gtest.h>

#include <string>

#include "parser/string_util.h"

#define TEST_FOR_COLLAPSE_CHARCTER(Name, Sequence)                             \
  TEST(StringUtilTest, CollapseWhitespaceUTF8_ ## Name) {                      \
    std::string text =  std::string(Sequence) +                                \
        std::string(Sequence) + std::string(Sequence) +                        \
        u8"Sample text wra" + std::string(Sequence) + u8"pped with sp" +       \
        std::string(Sequence) + std::string(Sequence) + u8" aces" +            \
        std::string(Sequence) + std::string(Sequence);                         \
    std::string result =                                                       \
      u8"Sample text wra pped with sp aces";                                   \
    EXPECT_EQ(result, CollapseWhitespaceUTF8(text));                           \
  }                                                                            \

#define TEST_FOR_STRIPPING_WRAPPING_BIDI_CHARACTERS(Name, Sequence)            \
  TEST(StringUtilTest,                                                         \
      StripWrappingBidiControlCharactersUTF8Strip_ ## Name) {                  \
    std::string text = u8"Sample text wrapped with BIDI";                      \
    EXPECT_EQ(text,                                                            \
        StripWrappingBidiControlCharactersUTF8(                                \
            std::string(Sequence) +                                            \
            text +                                                             \
            std::string(kPopDirectionalFormatting)));                          \
  }                                                                            \

namespace parser {
namespace utils {

class StringUtilTest : public testing::Test { };

TEST(StringUtilTest, StripWrappingBidiControlCharactersUTF8None) {
  std::string text = "I'm in ASCII";
  EXPECT_EQ(text, StripWrappingBidiControlCharactersUTF8(text));
}

TEST_FOR_STRIPPING_WRAPPING_BIDI_CHARACTERS(RTLEmbedding,
                                            kRightToLeftEmbeddingMark)
TEST_FOR_STRIPPING_WRAPPING_BIDI_CHARACTERS(LTREmbedding,
                                            kLeftToRightEmbeddingMark)
TEST_FOR_STRIPPING_WRAPPING_BIDI_CHARACTERS(RTLOverride,
                                            kRightToLeftOverride)
TEST_FOR_STRIPPING_WRAPPING_BIDI_CHARACTERS(LTROverride,
                                            kLeftToRightOverride)

TEST(StringUtilTest, CollapseWhitespaceUTF8_NoCollapse) {
  std::string text = u8"Sample text wrapped with spaces";
  EXPECT_EQ(text, CollapseWhitespaceUTF8(text));
}

TEST(StringUtilTest, CollapseWhitespaceUTF8_WhitespaceAtBothEnds) {
  std::string text =
      u8"Sample text wrapped with spaces";
  EXPECT_EQ(text, CollapseWhitespaceUTF8(
      std::string(u8"\x09\xc2\x85") +
      text +
      std::string(u8"\xe2\x80\x8A\x0B")));
}

TEST(StringUtilTest, CollapseWhitespaceUTF8_WhitespaceAtBegin) {
  std::string text =
      u8"Sample text wrapped with spaces";
  EXPECT_EQ(text, CollapseWhitespaceUTF8(
      std::string(u8"\xc2\x85\x09\xc2\x85") +
      text));
}

TEST(StringUtilTest, CollapseWhitespaceUTF8_WhitespaceAtEnd) {
  std::string text =
      u8"Sample text wrapped with spaces";
  EXPECT_EQ(text, CollapseWhitespaceUTF8(
      text +
      std::string(u8"\x0C\xe2\x80\x8A\x0B\x0C\x0C")));
}

TEST_FOR_COLLAPSE_CHARCTER(CharacterTabulation, u8"\x09")
TEST_FOR_COLLAPSE_CHARCTER(LineFeed, u8"\x0A")
TEST_FOR_COLLAPSE_CHARCTER(LineTabulation, u8"\x0B")
TEST_FOR_COLLAPSE_CHARCTER(FormFeed, u8"\x0C")
TEST_FOR_COLLAPSE_CHARCTER(CarriageReturn, u8"\x0D")
TEST_FOR_COLLAPSE_CHARCTER(Space, u8"\x20")
TEST_FOR_COLLAPSE_CHARCTER(NextLine, u8"\xc2\x85")
TEST_FOR_COLLAPSE_CHARCTER(NoBreakSpace, u8"\xc2\xa0")
TEST_FOR_COLLAPSE_CHARCTER(OghamSpaceMark, u8"\xe1\x9a\x80")
TEST_FOR_COLLAPSE_CHARCTER(EnQuad, u8"\xe2\x80\x80")
TEST_FOR_COLLAPSE_CHARCTER(EmQuad, u8"\xe2\x80\x81")
TEST_FOR_COLLAPSE_CHARCTER(EnSpace, u8"\xe2\x80\x82")
TEST_FOR_COLLAPSE_CHARCTER(EmSpace, u8"\xe2\x80\x83")
TEST_FOR_COLLAPSE_CHARCTER(ThreePerEmSpace, u8"\xe2\x80\x84")
TEST_FOR_COLLAPSE_CHARCTER(FourPerEmSpace, u8"\xe2\x80\x85")
TEST_FOR_COLLAPSE_CHARCTER(SixPerEmSpace, u8"\xe2\x80\x86")
TEST_FOR_COLLAPSE_CHARCTER(FigureSpace, u8"\xe2\x80\x87")
TEST_FOR_COLLAPSE_CHARCTER(PunctuationSpace, u8"\xe2\x80\x88")
TEST_FOR_COLLAPSE_CHARCTER(ThinSpace, u8"\xe2\x80\x89")
TEST_FOR_COLLAPSE_CHARCTER(HairSeparator, u8"\xe2\x80\x8A")
TEST_FOR_COLLAPSE_CHARCTER(LineSepator, u8"\xe2\x80\xa8")
TEST_FOR_COLLAPSE_CHARCTER(ParagraphSeperator, u8"\xe2\x80\xa9")
TEST_FOR_COLLAPSE_CHARCTER(NarrowNoBreakSpace, u8"\xe2\x80\xaf")
TEST_FOR_COLLAPSE_CHARCTER(MediumMathematicalSpace, u8"\xe2\x81\x9f")
TEST_FOR_COLLAPSE_CHARCTER(IdeographicSpace, u8"\xe3\x80\x80")

}  // namespace utils
}  // namespace parser
