// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#ifndef PARSER_MANIFEST_HANDLERS_PERMISSIONS_HANDLER_H_
#define PARSER_MANIFEST_HANDLERS_PERMISSIONS_HANDLER_H_

#include <string>
#include <vector>

#include "utils/macros.h"

#include "parser/manifest_handler.h"
#include "parser/permission_types.h"

namespace common_installer {
namespace parser {

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
             std::string* error) override;
  bool AlwaysParseForType(Manifest::Type type) const override;
  std::vector<std::string> Keys() const override;

 private:
  DISALLOW_COPY_AND_ASSIGN(PermissionsHandler);
};

}  // namespace parser
}  // namespace common_installer

#endif  // PARSER_MANIFEST_HANDLERS_PERMISSIONS_HANDLER_H_
