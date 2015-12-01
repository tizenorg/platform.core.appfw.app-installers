// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef WGT_STEP_STEP_CREATE_SYMBOLIC_LINK_H_
#define WGT_STEP_STEP_CREATE_SYMBOLIC_LINK_H_

#include <boost/filesystem.hpp>

#include <manifest_parser/utils/logging.h>

#include "common/app_installer.h"
#include "common/installer_context.h"
#include "common/step/step.h"

namespace wgt {
namespace filesystem {

/**
 * \brief Step that create symbolic link to application
 */
class StepCreateSymbolicLink : public common_installer::Step {
 public:
  using Step::Step;

  /**
   * \brief Create symbolic link
   *
   * \return Status::ERROR when failed to create symbolic link,
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
   * \brief Delete created symbolic link
   *
   * \return Status::OK
   */
  Status undo() override;

  /**
   * \brief Empty method
   *
   * \return Status::OK
   */
  Status precheck() override { return Status::OK; }

  SCOPE_LOG_TAG(SymbolicLink)
};

}  // namespace filesystem
}  // namespace wgt

#endif  // WGT_STEP_STEP_CREATE_SYMBOLIC_LINK_H_
