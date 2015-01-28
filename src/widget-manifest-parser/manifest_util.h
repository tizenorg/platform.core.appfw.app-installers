// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WIDGET_MANIFEST_PARSER_MANIFEST_UTIL_H_
#define WIDGET_MANIFEST_PARSER_MANIFEST_UTIL_H_

#include <boost/filesystem/path.hpp>

#include <string>
#include <map>
#include <memory>

#include "widget-manifest-parser/application_data.h"

// Utilities for manipulating the on-disk storage of applications.
namespace common_installer {
namespace widget_manifest_parser {

// Loads an application manifest from the specified directory. Returns NULL
// on failure, with a description of the error in |error|.
std::unique_ptr<Manifest> LoadManifest(
    const std::string& file_path, Manifest::Type type, std::string* error);

}  // namespace widget_manifest_parser
}  // namespace common_installer

#endif  // WIDGET_MANIFEST_PARSER_MANIFEST_UTIL_H_
