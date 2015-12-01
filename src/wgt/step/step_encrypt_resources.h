// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef WGT_STEP_STEP_ENCRYPT_RESOURCES_H_
#define WGT_STEP_STEP_ENCRYPT_RESOURCES_H_

#include <boost/filesystem/path.hpp>

#include <manifest_parser/utils/logging.h>

#include "common/step/step.h"
#include "wgt/wgt_backend_data.h"

namespace wgt {
namespace encrypt {

/**
 * \brief Step that encrypt application resources files if flagged to do so
 */
class StepEncryptResources : public common_installer::Step {
 public:
  using Step::Step;

  /**
   * \brief Encrypt files
   *
   * \return Status::ERROR when error occurred during encryption,
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
   * \brief Check requirements for this step
   *
   * \return Status::ERROR when manifest data are missing,
   *         Status::INVALID_VALUE when requirements are not meet,
   *         Status::OK otherwise
   */
  Status precheck() override;

 private:
  bool Encrypt(const boost::filesystem::path &src);
  bool EncryptFile(const boost::filesystem::path &src);
  bool ToBeEncrypted(const boost::filesystem::path &file);
  WgtBackendData* backend_data_;
  boost::filesystem::path input_;
  SCOPE_LOG_TAG(EncryptResources)
};
}  // namespace encrypt
}  // namespace wgt
#endif  // WGT_STEP_STEP_ENCRYPT_RESOURCES_H_
