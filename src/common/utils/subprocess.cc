// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/utils/subprocess.h"

#include <manifest_parser/utils/logging.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <memory>

namespace common_installer {

Subprocess::Subprocess(const std::string& program)
    : program_(program),
      pid_(0),
      started_(false),
      uid_(-1) {
}

bool Subprocess::RunWithArgs(const std::vector<std::string>& args) {
  if (started_) {
    LOG(WARNING) << "Process already started";
    return false;
  }
  pid_ = fork();
  if (pid_ == 0) {
    std::unique_ptr<const char*[]> argv(new const char*[2 + args.size()]);
    argv[0] = program_.c_str();
    for (size_t i = 1; i <= args.size(); ++i) {
      argv[i] = args[i - 1].c_str();
    }
    argv[args.size() + 1] = nullptr;
    if (uid_ != -1) {
      if (setuid(uid_)) {
        LOG(ERROR) << "Failed to setuid";
        return false;
      }
    }
    execvp(argv[0], const_cast<char* const*>(argv.get()));
    LOG(ERROR) << "Failed to execv";
    return false;
  } else if (pid_ == -1) {
    LOG(ERROR) << "Failed to fork";
    return false;
  } else {
    started_ = true;
    return true;
  }
}

int Subprocess::Wait() {
  if (!started_) {
    LOG(WARNING) << "Process is not started. Cannot wait";
    return -1;
  }
  int status;
  waitpid(pid_, &status, 0);
  return status;
}

}  // namespace common_installer
