// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_UTILS_SUBPROCESS_H_
#define COMMON_UTILS_SUBPROCESS_H_

#include <string>
#include <vector>

namespace common_installer {

__attribute__ ((visibility ("default"))) class Subprocess {
 public:
  explicit Subprocess(const std::string& program);

  template<typename ...Args> bool Run(Args&&... args) {
    std::vector<std::string> argv{std::forward<Args>(args)...};
    return RunWithArgs(argv);
  }

  bool RunWithArgs(
      const std::vector<std::string>& args = std::vector<std::string>());
  int Wait();

 private:
  std::string program_;
  int pid_;
  bool started_;
};

}  // namespace common_installer

#endif  // COMMON_UTILS_SUBPROCESS_H_
