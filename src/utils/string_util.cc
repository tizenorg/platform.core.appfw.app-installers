// Copyright 2013 The Chromium Authors. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#include "utils/string_util.h"

#include <cstdlib>
#include <string>
#include <vector>

namespace common_installer {
namespace utils {

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
      int result = strtol(str, nullptr, 16);
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
}  // namespace common_installer
