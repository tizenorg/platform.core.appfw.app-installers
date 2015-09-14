// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "wgt/step/step_add_default_privileges.h"

#include <pkgmgrinfo_basic.h>
#include <security-manager.h>

#include <cstdlib>
#include <cstring>
#include <memory>

#include "common/utils/clist_helpers.h"

namespace {

const char kPlatformVersion[] = "3.0";
const char kDefaultPrivilegeForWebApp[] = "http://tizen.org/privilege/webappdefault";

bool AddDefaultPrivilegesForWebApp(manifest_x* m) {
  // prepare input structure
  std::unique_ptr<const char*[]> input_privileges(new const char*[1]);
  input_privileges[0] = strdup(kDefaultPrivilegeForWebApp);

  // get mapping
  size_t output_size = 0;
  char** output_privileges = nullptr;
  if (security_manager_get_privileges_mapping(m->api_version, kPlatformVersion,
      input_privileges.get(), 1, &output_privileges, &output_size)
      != SECURITY_MANAGER_SUCCESS) {
    LOG(ERROR) << "security_manager_get_privileges_mapping failed";
    return false;
  }

  // set pkgmgr new list
  if (!m->privileges) {
    m->privileges =
      reinterpret_cast<privileges_x*>(calloc(1, sizeof(privileges_x*)));
  }
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

namespace wgt {
namespace security {

common_installer::Step::Status StepAddDefaultPrivileges::precheck() {
  if (!context_->manifest_data.get()) {
    LOG(ERROR) << "Manifest data is not set";
    return Status::ERROR;
  }
  return Status::OK;
}

common_installer::Step::Status StepAddDefaultPrivileges::process() {
  return AddDefaultPrivilegesForWebApp(context_->manifest_data.get()) ?
      Status::OK : Status::ERROR;
}

}  // namespace security
}  // namespace wgt
