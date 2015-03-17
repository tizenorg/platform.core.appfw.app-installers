// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#ifndef MANIFEST_HANDLERS_PERMISSIONS_HANDLER_H_
#define MANIFEST_HANDLERS_PERMISSIONS_HANDLER_H_

#include <string>
#include <vector>

#include "utils/macros.h"

#include "parser/manifest_handler.h"
#include "parser/permission_types.h"

namespace wgt {
namespace parse {

class PermissionsInfo: public parser::ManifestData {
 public:
  PermissionsInfo();
  virtual ~PermissionsInfo();

  const parser::PermissionSet& GetAPIPermissions() const {
    return api_permissions_;}
  void SetAPIPermissions(const parser::PermissionSet& api_permissions) {
    api_permissions_ = api_permissions;
  }

 private:
  parser::PermissionSet api_permissions_;
  DISALLOW_COPY_AND_ASSIGN(PermissionsInfo);
};

class PermissionsHandler: public parser::ManifestHandler {
 public:
  PermissionsHandler();
  virtual ~PermissionsHandler();

  bool Parse(
      const parser::Manifest& data,
      std::shared_ptr<parser::ManifestData>* output,
      std::string* error) override;
  bool Validate(
      const parser::ManifestData& data,
      const parser::ManifestDataMap& handlers_output,
      std::string* error) const override;
  std::string Key() const override;

 private:
  DISALLOW_COPY_AND_ASSIGN(PermissionsHandler);
};

}  // namespace parse
}  // namespace wgt

#endif  // MANIFEST_HANDLERS_PERMISSIONS_HANDLER_H_
