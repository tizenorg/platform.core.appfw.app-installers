// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_PKGMGR_SIGNAL_H_
#define COMMON_PKGMGR_SIGNAL_H_

#include <pkgmgr_installer.h>

#include <memory>
#include <string>

#include "common/request.h"
#include "common/step/step.h"
#include "common/utils/macros.h"

namespace common_installer {

//
// Utility for sending signal to pkgmgr
class PkgmgrSignal {
 public:
  enum class State {
    NOT_SENT,
    STARTED,
    FINISHED
  };

  explicit PkgmgrSignal(pkgmgr_installer* pi, RequestType req_type);

  bool SendStarted(
      const std::string& type = std::string(),
      const std::string& pkgid = std::string());
  bool SendProgress(int progress,
      const std::string& type = std::string(),
      const std::string& pkgid = std::string());
  bool SendFinished(
      Step::Status result,
      const std::string& type = std::string(),
      const std::string& pkgid = std::string());
  bool IsFinished() const;

  State state() const { return state_; }

 private:
  bool SendSignal(
      const char* key,
      const char* value,
      const std::string& type = std::string(),
      const std::string& pkgid = std::string()) const;
  const char* GetResultKey(Step::Status result) const;

  pkgmgr_installer* pi_;
  static State state_;
  RequestType request_type_;

  DISALLOW_COPY_AND_ASSIGN(PkgmgrSignal);
};

}  // namespace common_installer

#endif  // COMMON_PKGMGR_SIGNAL_H_
