// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_privilege_compatibility.h"

#include <pkgmgrinfo_basic.h>
#include <security-manager.h>

#include <cstdlib>
#include <cstring>
#include <memory>

#include "common/utils/clist_helpers.h"

namespace {

const char kPlatformVersion[] = "3.0";

bool TranslatePrivilegesForCompatibility(manifest_x* m) {
  if (!m->api_version) {
    LOG(WARNING) << "Skipping privileges mapping because api-version "
                 << "is not specified by package";
    return true;
  }
  if (strcmp(m->api_version, kPlatformVersion) == 0)
    return true;

  // calculate number of privileges
  size_t size = 0;
  privileges_x *privileges = nullptr;
  PKGMGR_LIST_MOVE_NODE_TO_HEAD(m->privileges, privileges);
  for (; privileges; privileges = privileges->next) {
    privilege_x* privilege = privileges->privilege;
    size += PKGMGR_LIST_LEN(privilege);
  }

  // prepare input structure
  std::unique_ptr<const char*[]> input_privileges(new const char*[size]);
  size_t input_size = 0;
  privileges = nullptr;
  PKGMGR_LIST_MOVE_NODE_TO_HEAD(m->privileges, privileges);
  for (; privileges; privileges = privileges->next) {
    privilege_x* priv = nullptr;
    PKGMGR_LIST_MOVE_NODE_TO_HEAD(privileges->privilege, priv);
    for (; priv; priv = priv->next) {
      input_privileges[input_size++] = priv->text;
    }
  }

  // get mapping
  size_t output_size = 0;
  char** output_privileges = nullptr;
  if (security_manager_get_privileges_mapping(m->api_version, kPlatformVersion,
      input_privileges.get(), input_size, &output_privileges, &output_size)
      != SECURITY_MANAGER_SUCCESS) {
    LOG(ERROR) << "security_manager_get_privileges_mapping failed";
    return false;
  }

  // delete pkgmgr old list
  privileges = nullptr;
  privileges_x* privileges_next = nullptr;
  PKGMGR_LIST_MOVE_NODE_TO_HEAD(m->privileges, privileges);
  for (; privileges; privileges = privileges_next) {
    privileges_next = privileges->next;
    privilege_x* priv = nullptr;
    privilege_x* next = nullptr;
    PKGMGR_LIST_MOVE_NODE_TO_HEAD(privileges->privilege, priv);
    for (; priv; priv = next) {
      next = priv->next;
      // mark as const but we actually have ownership
      free(const_cast<char*>(priv->text));
      free(priv);
    }
    free(privileges);
  }

  // set pkgmgr new list
  m->privileges =
      reinterpret_cast<privileges_x*>(calloc(1, sizeof(privileges_x)));
  for (size_t i = 0; i < output_size; ++i) {
    privilege_x* priv =
        reinterpret_cast<privilege_x*>(calloc(1, sizeof(privilege_x)));
    priv->text = strdup(output_privileges[i]);
    LISTADD(m->privileges->privilege, priv);
  }

  security_manager_privilege_mapping_free(output_privileges, output_size);
  return true;
}

}  // namespace

namespace common_installer {
namespace security {

Step::Status StepPrivilegeCompatibility::precheck() {
  if (!context_->manifest_data.get()) {
    LOG(ERROR) << "Manifest data is not set";
    return Status::ERROR;
  }
  return Status::OK;
}

Step::Status StepPrivilegeCompatibility::process() {
  return TranslatePrivilegesForCompatibility(context_->manifest_data.get()) ?
      Status::OK : Status::ERROR;
}

}  // namespace security
}  // namespace common_installer

