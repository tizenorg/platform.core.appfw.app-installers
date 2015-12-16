/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

/*
  A step is made of 4 functions (that must be defined)
  and one data pointer.

  The functions are:
    - precheck checks the input data used during process method
    - process  handles the job to be done
    - undo     undo the step's work after failure
    - clean    remove temporary data of the step after success

  All these functions have the same signature: they do not accept any arguments
  and they return a value which states the execution result.

  The returned code of Step::Status::OK indicates a successful execution.
  Otherwise, the returned code should be set to value different than
  Step::Status::OK.

  Errornous result of processing is casted to integer value and sent to
  client which initialized request.
*/
#ifndef COMMON_STEP_STEP_H_
#define COMMON_STEP_STEP_H_

#include "common/installer_context.h"

#include <boost/signals2.hpp>

namespace common_installer {

/**
 * \brief Abstract base class for all Steps* used for requests handling.
 *
 * It is an abstract base class that demands the definition of
 * 4 methods: process, precheck, undo and clean.
 */
class Step {
 public:
  using StepErrorSignal = boost::signals2::signal<void(const std::string&)>;

  /** Possible code returned by process, undo, clean, precheck methods. */
  enum class Status {
    OUT_OF_SPACE = -3,      /**< Out of disc space */
    INVALID_VALUE = -2,     /**< Invalid argument */
    ERROR = -1,             /**< General error */
    OK = 0                  /**< General success */
  };

  /** Standard constructor */
  explicit Step(InstallerContext* context) : context_(context) { }

  /** Virtual "empty" destructor */
  virtual ~Step() { }

  /** Handles the job to be done */
  virtual Status process() = 0;

  /** Undos the step's work after failure */
  virtual Status undo() = 0;

  /** Removes temporary data of the step after success */
  virtual Status clean() = 0;

  /** Checks the input data used during process method */
  virtual Status precheck() = 0;

  StepErrorSignal on_error;
 protected:
  InstallerContext* context_;

};

}  // namespace common_installer

#endif  // COMMON_STEP_STEP_H_
