// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#ifndef WIDGET_MANIFEST_PARSER_MANIFEST_UTIL_H_
#define WIDGET_MANIFEST_PARSER_MANIFEST_UTIL_H_

#include <boost/filesystem/path.hpp>
#include <libxml2/libxml/tree.h>

#include <string>
#include <map>
#include <memory>

#include "widget-manifest-parser/application_data.h"

// Utilities for manipulating the on-disk storage of applications.
namespace common_installer {
namespace widget_manifest_parser {

// Loads an application manifest from the specified directory. Returns nullptr
// on failure, with a description of the error in |error|.
std::unique_ptr<Manifest> LoadManifest(
    const std::string& file_path, std::string* error);
std::string GetNodeDir(xmlNode* node, const std::string& inherit_dir);
std::string GetNodeText(xmlNode* root, const std::string& inherit_dir);
bool IsPropSupportDir(xmlNode* root, xmlAttr* prop);
bool IsElementSupportSpanAndDir(xmlNode* root);
bool IsSingletonElement(const std::string& name);
bool IsTrimRequiredForElement(xmlNode* root);
bool IsTrimRequiredForProp(xmlNode* root, xmlAttr* prop);
std::unique_ptr<utils::DictionaryValue> LoadXMLNode(
    xmlNode* root, const std::string& inherit_dir = "");

}  // namespace widget_manifest_parser
}  // namespace common_installer

#endif  // WIDGET_MANIFEST_PARSER_MANIFEST_UTIL_H_
