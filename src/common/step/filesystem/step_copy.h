/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_FILESYSTEM_STEP_COPY_H_
#define COMMON_STEP_FILESYSTEM_STEP_COPY_H_

#include <manifest_parser/utils/logging.h>

#include "common/installer_context.h"

#include "common/step/step.h"

namespace common_installer {
namespace filesystem {

/**
 * \brief step responsible for moving/copying files from temp unpacked dir to
 *        final installed package destination during INSTALLATION.
 *        Used by WGT and TPK
 */
class StepCopy : public Step {
 public:
  using Step::Step;

  /**
   * \brief main logic of moving/copying files
   *
   * \return Status::OK if success, Status::ERROR otherwise
   */
  Status process() override;

  Status clean() override;

  /**
   * \brief removes files from final package destination
   *
   * \return Status::OK if success, Status::ERROR otherwise
   */
  Status undo() override;

  /**
   * \brief checks if necessary paths/data are provided
   *
   * \return Status::OK if success, Status::ERROR otherwise
   */
  Status precheck() override;

  SCOPE_LOG_TAG(Copy)
};

}  // namespace filesystem
}  // namespace common_installer

#endif  // COMMON_STEP_FILESYSTEM_STEP_COPY_H_
