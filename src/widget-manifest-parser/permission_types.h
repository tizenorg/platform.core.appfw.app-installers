// Copyright (c) 2013 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WIDGET_MANIFEST_PARSER_PERMISSION_TYPES_H_
#define WIDGET_MANIFEST_PARSER_PERMISSION_TYPES_H_

#include <map>
#include <set>
#include <string>

namespace common_installer {
namespace widget_manifest_parser {

enum PermissionType {
  SESSION_PERMISSION,
  PERSISTENT_PERMISSION,
};

enum RuntimePermission {
  ALLOW_ONCE = 0,
  ALLOW_SESSION,
  ALLOW_ALWAYS,
  DENY_ONCE,
  DENY_SESSION,
  DENY_ALWAYS,
  UNDEFINED_RUNTIME_PERM,
};

enum StoredPermission {
  ALLOW = 0,
  DENY,
  PROMPT,
  UNDEFINED_STORED_PERM,
};

typedef std::map<std::string, StoredPermission> StoredPermissionMap;
typedef std::set<std::string> PermissionSet;

}  // namespace widget_manifest_parser
}  // namespace common_installer

#endif  // WIDGET_MANIFEST_PARSER_PERMISSION_TYPES_H_
