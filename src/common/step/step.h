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

#include <package-manager.h>

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
  /** Possible code returned by process, undo, clean, precheck methods. */
  enum class Status {
    GRANT_PERMISSION_ERROR = -201, // TODO
    IMAGE_ERROR = -200,  // TODO(t.iwanek): fix value of error code...
    UNZIP_ERROR = PKGCMD_ERRCODE_UNZIP_ERROR,
    SECURITY_ERROR = PKGCMD_ERRCODE_SECURITY_ERROR,
    REGISTER_ERROR = PKGCMD_ERRCODE_REGISTER_ERROR,
    PRIVILEGE_ERROR = PKGCMD_ERRCODE_PRIVILEGE_ERROR,
    PARSE_ERROR = PKGCMD_ERRCODE_PARSE_ERROR,
    RECOVERY_ERROR = PKGCMD_ERRCODE_RECOVERY_ERROR,
    DELTA_ERROR = PKGCMD_ERRCODE_DELTA_ERROR,
    APP_DIR_ERROR = PKGCMD_ERRCODE_APP_DIR_ERROR,
    CONFIG_ERROR = PKGCMD_ERRCODE_CONFIG_ERROR,
    SIGNATURE_ERROR = PKGCMD_ERRCODE_SIGNATURE_ERROR,
    SIGNATURE_INVALID = PKGCMD_ERRCODE_SIGNATURE_INVALID,
    CERT_ERROR = PKGCMD_ERRCODE_CERT_ERROR,
    AUTHOR_CERT_NOT_MATCH = PKGCMD_ERRCODE_AUTHOR_CERT_NOT_MATCH,
    AUTHOR_CERT_NOT_FOUND = PKGCMD_ERRCODE_AUTHOR_CERT_NOT_FOUND,
    ICON_ERROR = PKGCMD_ERRCODE_ICON_ERROR,
    ICON_NOT_FOUND = PKGCMD_ERRCODE_ICON_NOT_FOUND,
    MANIFEST_ERROR = PKGCMD_ERRCODE_MANIFEST_ERROR,
    MANIFEST_NOT_FOUND = PKGCMD_ERRCODE_MANIFEST_NOT_FOUND,
    PACKAGE_NOT_FOUND = PKGCMD_ERRCODE_PACKAGE_NOT_FOUND,
    OPERATION_NOT_ALLOWED = PKGCMD_ERRCODE_OPERATION_NOT_ALLOWED,
    OUT_OF_SPACE = PKGCMD_ERRCODE_OUT_OF_SPACE,
    INVALID_VALUE = PKGCMD_ERRCODE_INVALID_VALUE,
    ERROR = PKGCMD_ERRCODE_ERROR,
    OK = PKGCMD_ERRCODE_OK
  };

  using StepErrorSignal =
    boost::signals2::signal<void(Step::Status result, const std::string&)>;

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
