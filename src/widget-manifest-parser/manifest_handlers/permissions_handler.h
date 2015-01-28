// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.
// Part of this file is redistributed from crosswalk project under BSD-style
// license. Check LICENSE-xwalk file.

#ifndef WIDGET_MANIFEST_PARSER_MANIFEST_HANDLERS_PERMISSIONS_HANDLER_H_
#define WIDGET_MANIFEST_PARSER_MANIFEST_HANDLERS_PERMISSIONS_HANDLER_H_

#include <string>
#include <vector>

#include "utils/macros.h"

#include "widget-manifest-parser/manifest_handler.h"
#include "widget-manifest-parser/permission_types.h"

namespace common_installer {
namespace widget_manifest_parser {

class PermissionsInfo: public ApplicationData::ManifestData {
 public:
  PermissionsInfo();
  virtual ~PermissionsInfo();

  const PermissionSet& GetAPIPermissions() const {
    return api_permissions_;}
  void SetAPIPermissions(const PermissionSet& api_permissions) {
    api_permissions_ = api_permissions;
  }

 private:
  PermissionSet api_permissions_;
  DISALLOW_COPY_AND_ASSIGN(PermissionsInfo);
};

class PermissionsHandler: public ManifestHandler {
 public:
  PermissionsHandler();
  virtual ~PermissionsHandler();

  bool Parse(std::shared_ptr<ApplicationData> application,
                     std::u16string* error) override;
  bool AlwaysParseForType(Manifest::Type type) const override;
  std::vector<std::string> Keys() const override;

 private:
  DISALLOW_COPY_AND_ASSIGN(PermissionsHandler);
};

}  // namespace widget_manifest_parser
}  // namespace common_installer

#endif  // WIDGET_MANIFEST_PARSER_MANIFEST_HANDLERS_PERMISSIONS_HANDLER_H_
