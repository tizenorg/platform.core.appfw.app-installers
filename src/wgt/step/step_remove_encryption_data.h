// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef WGT_STEP_STEP_REMOVE_ENCRYPTION_DATA_H_
#define WGT_STEP_STEP_REMOVE_ENCRYPTION_DATA_H_

#include <manifest_parser/utils/logging.h>

#include "common/step/step.h"

namespace wgt {
namespace encrypt {

/**
 * This step remove encryption keys used for encryption during installation
 *
 * This step is called during uninstallation process
 */
class StepRemoveEncryptionData : public common_installer::Step {
 public:
  using Step::Step;

  /**
   * \brief Empty method - the actual work is done in clean() to delay it
   *
   * \return Status::OK
   */
  Status process() override { return Status::OK; }

  /**
   * \brief Remove encryption keys from database
   *
   * \return Status::ERROR in case of failure,
   *         Status::OK otherwise
   */
  Status clean() override;

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

  SCOPE_LOG_TAG(RemoveEncryptionData)
};
}  // namespace encrypt
}  // namespace wgt
#endif  // WGT_STEP_STEP_REMOVE_ENCRYPTION_DATA_H_
