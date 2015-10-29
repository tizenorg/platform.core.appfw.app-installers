// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_STEP_CREATE_ICONS_H_
#define COMMON_STEP_STEP_CREATE_ICONS_H_

#include <boost/filesystem/path.hpp>
#include <utility>
#include <vector>

#include "common/installer_context.h"
#include "common/step/step.h"
#include "common/utils/logging.h"

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

  /**
   * \brief empty method
   *
   * \return Status::OK
   */
  Status clean() override { return Status::OK; }

  /**
   * \brief removes icons from system
   *
   * \return Status::OK
   */
  Status undo() override;

  /**
   * \brief empty method
   *
   * \return Status::OK
   */
  Status precheck() override { return Status::OK; }

 protected:
  virtual boost::filesystem::path GetIconRoot() const;

 private:
  std::vector<boost::filesystem::path> icons_;

  SCOPE_LOG_TAG(CreateIcons)
};

}  // namespace filesystem
}  // namespace common_installer

#endif  // COMMON_STEP_STEP_CREATE_ICONS_H_
