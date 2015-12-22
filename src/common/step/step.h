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

#include <boost/signals2.hpp>
#include <string>

#include "common/installer_context.h"

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
    SECURITY_ERROR = -22,             /**< Security error */
    REGISTER_ERROR = -21,             /**< Register application error */
    PRIVILEGE_ERROR = -20,            /**< Privilege error */
    PARSE_ERROR = -19,                /**< Parsing error */
    RECOVERY_ERROR = -18,             /**< Recovery error */
    DELTA_ERROR = -17,                /**< Delta patch error */
    APP_DIR_ERROR = -16,              /**< Application directory error */
    CONFIG_ERROR = -15,               /**< Configuration error */
    SIGNATURE_ERROR = -14,            /**< Signature error */
    SIGNATURE_INVALID = -13,          /**< Signature invalid */
    CERT_ERROR = -12,                 /**< Check certificate error */
    AUTHOR_CERT_NOT_MATCH = -11,      /**< Author certificate not match */
    AUTHOR_CERT_NOT_FOUND = -10,      /**< Author certificate not found */
    ICON_ERROR = -9,                  /**< Icon error */
    ICON_NOT_FOUND = -8,              /**< Icon not found */
    MANIFEST_ERROR = -7,              /**< Manifest error */
    MANIFEST_NOT_FOUND = -6,          /**< Manifest not found */
    PACKAGE_NOT_FOUND = -5,           /**< Package not found */
    OPERATION_NOT_ALLOWED = -4,       /**< Operation not allowed */
    OUT_OF_SPACE = -3,                /**< Out of disc space */
    INVALID_VALUE = -2,               /**< Invalid argument */
    ERROR = -1,                       /**< General error */
    OK = 0                            /**< General success */
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
