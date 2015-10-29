// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

// This file was rewritten completely in
// 7c807fc0d4561d178b7c2d8b8d532f48e78ab1bc so fixing license.

#include "common/utils/string_util.h"

#include <cstdlib>
#include <string>
#include <vector>

namespace common_installer {

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

const unsigned int GetComparableVersion(const std::string& version) {
  int major, minor;

  std::sscanf(version.c_str(), "%d.%d", &major, &minor);

  if (major < 0) major = 0;
  if (minor < 0) minor = 0;

  return major + (10 * minor);
}

}  // namespace common_installer
