// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_FILESYSTEM_STEP_CREATE_ICONS_H_
#define COMMON_STEP_FILESYSTEM_STEP_CREATE_ICONS_H_

#include <boost/filesystem/path.hpp>

#include <manifest_parser/utils/logging.h>

#include <string>
#include <utility>
#include <vector>

#include "common/installer_context.h"
#include "common/step/step.h"

namespace common_installer {
namespace filesystem {

/**
 * \brief Step responsible for creating icons for applications used during
 *        installation and update. Used by WGT and TPK backend
 */
class StepCreateIcons : public Step {
 public:
  using Step::Step;

  /**
   * \brief main logic for creating icons
   *
   * \return Status::OK if success, Status::ERROR otherwise
   */
  Status process() override;
  Status clean() override { return Status::OK; }
  /**
   * \brief removes icons from system
   *
   * \return Status::OK
   */
  Status undo() override;
  Status precheck() override { return Status::OK; }

 private:
  std::vector<boost::filesystem::path> icons_;

  SCOPE_LOG_TAG(CreateIcons)
};

}  // namespace filesystem
}  // namespace common_installer

#endif  // COMMON_STEP_FILESYSTEM_STEP_CREATE_ICONS_H_
