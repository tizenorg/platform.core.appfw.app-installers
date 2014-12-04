// Copyright (c) 2013 Intel Corporation. All rights reserved.
// Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.
// Part of this file is redistributed from crosswalk project under BDS-style
// license. Check LICENSE-xwalk file.

#ifndef COMMON_STEP_SIGNATURE_SIGNATURE_PARSER_H_
#define COMMON_STEP_SIGNATURE_SIGNATURE_PARSER_H_

#include <boost/filesystem/path.hpp>

#include <memory>

#include "step/signature/signature_data.hxx"
#include "utils/marcos.hxx"

namespace common {

namespace signature {

class SignatureParser {
 public:
  static std::unique_ptr<SignatureData> CreateSignatureData(
      const boost::filesystem::path& signature_path, int signature_number);

 private:
  DISALLOW_COPY_AND_ASSIGN(SignatureParser);
};

}  // namespace signature

}  // namespace common

#endif  // COMMON_STEP_SIGNATURE_SIGNATURE_PARSER_H_
