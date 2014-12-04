// Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include <step/signature_step.h>

#include <boost/filesystem/path.hpp>

#include <string>

#include <signature_validator.h>

namespace bf = boost::filesystem;

namespace common {

namespace signature {

int SignatureStep::process(Context_installer* context) {
  return (SignatureValidator::Check(bf::path(context->unpack_directory))
      != SignatureValidator::VALID) ? -1 : 0;
}

}  // namespace signature

}  // namespace common
