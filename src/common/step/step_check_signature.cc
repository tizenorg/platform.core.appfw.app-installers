// Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_check_signature.h"

#include <boost/filesystem/path.hpp>

#include <string>

#include "signature/signature_validator.h"

namespace bf = boost::filesystem;

namespace common_installer {
namespace signature {

Step::Status StepCheckSignature::precheck() {
  if (context_->unpacked_dir_path.get().empty()) {
    LOG(ERROR) << "unpacked_dir_path attribute is empty";
    return Step::Status::INVALID_VALUE;
  }
  if (!boost::filesystem::exists(context_->unpacked_dir_path.get())) {
    LOG(ERROR) << "unpacked_dir_path ("
               << context_->unpacked_dir_path.get()
               << ") path does not exist";
    return Step::Status::INVALID_VALUE;
  }

  return Step::Status::OK;
}

Step::Status StepCheckSignature::process() {
  return (SignatureValidator::Check(context_->unpacked_dir_path.get())
      == SignatureValidator::INVALID) ? Status::ERROR : Status::OK;
}

}  // namespace signature
}  // namespace common_installer
