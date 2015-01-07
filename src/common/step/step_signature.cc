// Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_signature.h"

#include <boost/filesystem/path.hpp>

#include <string>

#include "signature/signature_validator.h"

namespace bf = boost::filesystem;

namespace common_installer {
namespace signature {

int StepSignature::process(ContextInstaller* context) {
  return (SignatureValidator::Check(bf::path(context->unpack_directory()))
      == SignatureValidator::INVALID) ? -1 : 0;
}

}  // namespace signature
}  // namespace common_installer
