// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#include "parser/app_control_info.h"

namespace common_installer {
namespace parser {

AppControlInfo::AppControlInfo(
    const std::string& src,
    const std::string& operation,
    const std::string& uri,
    const std::string& mime)
    : src_(src),
      operation_(operation),
      uri_(uri),
      mime_(mime) { }

}   // namespace parser
}   // namespace common_installer
