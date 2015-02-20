// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#ifndef PARSER_MANIFEST_UTIL_H_
#define PARSER_MANIFEST_UTIL_H_

#include <boost/filesystem/path.hpp>

#include <string>
#include <map>
#include <memory>

#include "parser/application_data.h"

// Utilities for manipulating the on-disk storage of applications.
namespace common_installer {
namespace parser {

// Loads an application manifest from the specified directory. Returns NULL
// on failure, with a description of the error in |error|.
std::unique_ptr<Manifest> LoadManifest(
    const std::string& file_path, Manifest::Type type, std::string* error);

}  // namespace parser
}  // namespace common_installer

#endif  // PARSER_MANIFEST_UTIL_H_
