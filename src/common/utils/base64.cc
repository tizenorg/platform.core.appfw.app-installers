// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/utils/base64.h"

#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/transform_width.hpp>

#include <sstream>
#include <string>

namespace bai = boost::archive::iterators;

namespace {

typedef bai::base64_from_binary<bai::transform_width<const char*, 6, 8>>
    base64_encode;

}  // namespace

namespace common_installer {

std::string EncodeBase64(const std::string& val) {
  std::stringstream os;
  std::copy(base64_encode(val.c_str()),
      base64_encode(val.c_str() + val.size()), std::ostream_iterator<char>(os));
  return os.str();
}

}  // namespace common_installer
