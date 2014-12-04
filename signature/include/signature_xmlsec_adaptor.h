// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.
// Part of this file is redistributed from crosswalk project under BDS-style
// license. Check LICENSE-xwalk file.

#ifndef SIGNATURE_SIGNATURE_XMLSEC_ADAPTOR_H_
#define SIGNATURE_SIGNATURE_XMLSEC_ADAPTOR_H_

#include <boost/filesystem/path.hpp>

#include <signature_data.h>

namespace common {

namespace signature {

class SignatureXmlSecAdaptor {
 public:
  static bool ValidateFile(const SignatureData& signature_data);

 private:
  DISALLOW_COPY_AND_ASSIGN(SignatureXmlSecAdaptor);
};

}  // namespace signature

}  // namespace common

#endif  // COMMON_STEP_SIGNATURE_SIGNATURE_XMLSEC_ADAPTOR_H_
