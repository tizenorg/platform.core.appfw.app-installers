// Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef UTILS_LOGGING_H_
#define UTILS_LOGGING_H_

#include <iostream>
#include <sstream>

// TODO(tiwanek): Logging...

class LogCatcher {
 public:
  LogCatcher() { }
  void operator&(const std::ostream& str) const {
    // TODO(tiwanek): this cast is error-prone - fix it
    std::cerr << static_cast<const std::ostringstream*>(&str)->str()
              << std::endl;
  }
};

#define LOG(LEVEL)                                                             \
    LogCatcher() & std::ostringstream() << "[" << #LEVEL << "] "               \

#endif  // UTILS_LOGGING_H_
