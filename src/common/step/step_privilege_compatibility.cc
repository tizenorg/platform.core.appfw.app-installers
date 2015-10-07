// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_privilege_compatibility.h"

#include <pkgmgrinfo_basic.h>
#include <security-manager.h>

#include <cstdlib>
#include <cstring>
#include <memory>
#include <string>

#include "common/utils/glist_range.h"

namespace {

const char kPlatformVersion[] = "3.0";
const char kDefaultPrivilegeForWebApp[] =
    "http://tizen.org/privilege/webappdefault";

bool TranslatePrivilegesForCompatibility(const std::string& pkg_type,
                                         manifest_x* m) {
  if (!m->api_version) {
    LOG(WARNING) << "Skipping privileges mapping because api-version "
                 << "is not specified by package";
    return true;
  }
  if (strcmp(m->api_version, kPlatformVersion) == 0)
    return true;

  // add default privilege for webapp
  if (pkg_type == "wgt") {
    m->privileges = g_list_append(m->privileges,
                                  strdup(kDefaultPrivilegeForWebApp));
  }

  // No privileges to map
  if (!m->privileges) {
    return true;
  }

  size_t size = g_list_length(m->privileges);

  // prepare input structure
  std::unique_ptr<const char*[]> input_privileges(new const char*[size]);
  size_t input_size = 0;
  for (const char* priv : GListRange<char*>(m->privileges)) {
    input_privileges[input_size++] = priv;
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

  // set pkgmgr new list
  g_list_free_full(m->privileges, free);
  m->privileges = nullptr;
  for (size_t i = 0; i < output_size; ++i) {
    m->privileges = g_list_append(m->privileges, strdup(output_privileges[i]));
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
  return TranslatePrivilegesForCompatibility(context_->pkg_type.get(),
             context_->manifest_data.get()) ?
      Status::OK : Status::ERROR;
}

}  // namespace security
}  // namespace common_installer

