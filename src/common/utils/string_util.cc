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

int CompareVersion(const std::string& version, const std::string& compareTo) {
  int ver1_major = 0, ver1_minor = 0, ver1_build = 0;
  int ver2_major = 0, ver2_minor = 0, ver2_build = 0;

  std::sscanf(version.c_str(), "%d.%d.%d",
      &ver1_major, &ver1_minor, &ver1_build);
  std::sscanf(compareTo.c_str(), "%d.%d.%d",
      &ver2_major, &ver2_minor, &ver2_build);

  if (ver1_major > ver2_major) return 1;
  if (ver1_major < ver2_major) return -1;

  if (ver1_minor > ver2_minor) return 1;
  if (ver1_minor < ver2_minor) return -1;

  if (ver1_build > ver2_build) return 1;
  if (ver1_build < ver2_build) return -1;

  return 0;
}

}  // namespace common_installer
