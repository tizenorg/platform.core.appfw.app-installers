// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.
// Part of this file is redistributed from crosswalk project under BDS-style
// license. Check LICENSE-xwalk file.

#include <signature_data.h>

namespace signature {

SignatureData::SignatureData(const boost::filesystem::path& signature_file_name,
    int signature_number)
    : signature_file_name_(signature_file_name),
      signature_number_(signature_number) {
}

SignatureData::~SignatureData() {
}

boost::filesystem::path SignatureData::GetExtractedWidgetPath() const {
  return signature_file_name().parent_path();
}

}  // namespace signature
