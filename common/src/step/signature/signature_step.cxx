// Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.
// Part of this file is redistributed from crosswalk project under BDS-style
// license. Check LICENSE-xwalk file.

#include "step/signature/signature_step.hxx"

#include <boost/filesystem/path.hpp>

#include <string>

#include "step/signature/signature_validator.hxx"

namespace bf = boost::filesystem;

namespace common {

namespace signature {

int SignatureStep::process(Context_installer* context) {
  return (SignatureValidator::Check(bf::path(context->unpack_directory))
      != SignatureValidator::VALID) ? -1 : 0;
}

}  // namespace signature

}  // namespace common
