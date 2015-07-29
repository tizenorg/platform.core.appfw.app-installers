// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef WGT_STEP_STEP_ENCRYPT_RESOURCES_H_
#define WGT_STEP_STEP_ENCRYPT_RESOURCES_H_

#include <boost/filesystem/path.hpp>

#include "common/step/step.h"
#include "common/utils/logging.h"
#include "wgt/wgt_backend_data.h"

namespace wgt {
namespace encrypt {

class StepEncryptResources : public common_installer::Step {
 public:
  using Step::Step;

  Status process() override;
  Status clean() override { return Status::OK; }
  Status undo() override { return Status::OK; }
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
