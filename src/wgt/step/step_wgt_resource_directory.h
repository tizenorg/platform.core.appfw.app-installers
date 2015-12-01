// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef WGT_STEP_STEP_WGT_RESOURCE_DIRECTORY_H_
#define WGT_STEP_STEP_WGT_RESOURCE_DIRECTORY_H_

#include "common/app_installer.h"
#include "common/installer_context.h"
#include "common/step/step.h"
#include "manifest_parser/utils/logging.h"

namespace wgt {
namespace filesystem {

/**
 * \brief This step fixes unpacked directory structure so that all widget
 * content is moved from root path to res/wgt before we copy whole directory in
 * StepCopy
 */
class StepWgtResourceDirectory : public common_installer::Step {
 public:
  using Step::Step;

  /**
   * \brief Create directory structure and copy content of widget package
   *
   * \return Status::ERROR when failed to create proper directory structure or
   *                       move source directory to destination directory,
   *         Status::OK otherwise
   */
  Status process() override;

  /**
   * \brief Empty method
   *
   * \return Status::OK
   */
  Status clean() override { return Status::OK; }

  /**
   * \brief Empty method
   *
   * \return Status::OK
   */
  Status undo() override { return Status::OK; }

  /**
   * \brief Empty method
   *
   * \return Status::OK
   */
  Status precheck() override { return Status::OK; }

  SCOPE_LOG_TAG(CreateWgtResourceDirectory)
};

}  // namespace filesystem
}  // namespace wgt

#endif  // WGT_STEP_STEP_WGT_RESOURCE_DIRECTORY_H_
