// Copyright (c) 2013 Intel Corporation. All rights reserved.
// Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.
// Part of this file is redistributed from crosswalk project under BDS-style
// license. Check LICENSE-xwalk file.

#ifndef SIGNATURE_SIGNATURE_VALIDATOR_H_
#define SIGNATURE_SIGNATURE_VALIDATOR_H_

#include <boost/filesystem.hpp>

#include <string>

#include "utils/logging.h"
#include "utils/macros.h"

namespace common_installer {
namespace signature {

class SignatureValidator {
 public:
  enum Status {
    UNTRUSTED,
    VALID,
    INVALID
  };

  static Status Check(const boost::filesystem::path& widget_path);

 private:
  SCOPE_LOG_TAG(SignatureValidator)

  DISALLOW_COPY_AND_ASSIGN(SignatureValidator);
};

}  // namespace signature
}  // namespace common_installer

#endif  // SIGNATURE_SIGNATURE_VALIDATOR_H_
