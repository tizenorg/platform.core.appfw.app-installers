/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

/*
  A step is made of 3 functions (that can be defined or NULL)
  and one data pointer.

  The functions are:
    - process  process the installation step
    - undo     undo the installation step after failure
    - clean    remove temporary data of the step after success

  These functions all have the same signature: they accept
  a pointer to something and they return a value which states
  the execution issue.

  The returned code of Step::Status::OK indicates a succeful execution.
  Otherwise, the returned code should be set to value different than
  Step::Status::OK.

  Errornous result of processing is casted to interger value and sent to
  client which sent request.
*/
#ifndef COMMON_STEP_STEP_H_
#define COMMON_STEP_STEP_H_

#include "common/context_installer.h"

namespace common_installer {

class Step {
 public:
  enum class Status {
    OUT_OF_SPACE = -3,      /**< Out of disc space */
    INVALID_VALUE = -2,     /**< Invalid argument */
    ERROR = -1,             /**< General error */
    OK = 0                  /**< General success */
  };

  explicit Step(ContextInstaller* context) : context_(context) { }
  virtual ~Step() { }

  virtual Status process() = 0;
  virtual Status undo() = 0;
  virtual Status clean() = 0;

 protected:
  ContextInstaller* context_;
};

}  // namespace common_installer

#endif  // COMMON_STEP_STEP_H_
