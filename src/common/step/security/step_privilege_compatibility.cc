// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/step/security/step_privilege_compatibility.h"
#include "common/privileges.h"

#include <pkgmgrinfo_basic.h>
#include <privilege_manager.h>

#include <cstdlib>
#include <cstring>
#include <memory>
#include <string>

#include "common/utils/glist_range.h"

namespace {

bool TranslatePrivilegesForCompatibility(manifest_x* m) {
  if (!m->api_version) {
    LOG(WARNING) << "Skipping privileges mapping because api-version "
                 << "is not specified by package";
    return true;
  }

  // No privileges to map
  if (!m->privileges) {
    return true;
  }

  // get mapping
  GList* mapped_privilege_list;
  bool is_webapp = (strncmp(m->type, "wgt", strlen("wgt")) == 0) ? true : false;
  LOG(DEBUG) << "type = " << m->type;
  if (privilege_manager_get_mapped_privilege_list(m->api_version,
      is_webapp ? PRVMGR_PACKAGE_TYPE_WRT : PRVMGR_PACKAGE_TYPE_CORE,
      m->privileges, &mapped_privilege_list) != PRVMGR_ERR_NONE) {
    LOG(ERROR) << "privilege_manager_get_mapped_privilege_list failed";
    return false;
  }

  // set pkgmgr new list
  g_list_free_full(m->privileges, free);
  m->privileges = nullptr;
  for (GList* l = mapped_privilege_list; l != nullptr; l = l->next) {
    m->privileges = g_list_append(m->privileges,
                                  strdup(reinterpret_cast<char*>(l->data)));
  }

  g_list_free_full(mapped_privilege_list, free);
  return true;
}

}  // namespace

namespace common_installer {
namespace security {

Step::Status StepPrivilegeCompatibility::precheck() {
  if (!context_->manifest_data.get()) {
    LOG(ERROR) << "Manifest data is not set";
    return Status::MANIFEST_NOT_FOUND;
  }
  return Status::OK;
}

Step::Status StepPrivilegeCompatibility::process() {
  // Add default privileges for each certificates level.
  bool ret = true;
  switch (context_->privilege_level.get()) {
    case common_installer::PrivilegeLevel::PUBLIC:
      context_->manifest_data.get()->privileges =
          g_list_append(context_->manifest_data.get()->privileges,
                        strdup(common::privileges::kPrivForPublic));
      break;
    case common_installer::PrivilegeLevel::PARTNER:
      context_->manifest_data.get()->privileges =
          g_list_append(context_->manifest_data.get()->privileges,
                        strdup(common::privileges::kPrivForPartner));
      break;
    case common_installer::PrivilegeLevel::PLATFORM:
      context_->manifest_data.get()->privileges =
          g_list_append(context_->manifest_data.get()->privileges,
                        strdup(common::privileges::kPrivForPlatform));
      break;
    default:
      // TODO(jongmyeong.ko): temporarily, public privileges for untrusted
      // application.
      context_->manifest_data.get()->privileges =
          g_list_append(context_->manifest_data.get()->privileges,
                        strdup(common::privileges::kPrivForPublic));
      break;
  }
  if (!ret) {
    LOG(ERROR) << "Error during adding default privileges for certificates.";
    return Status::PRIVILEGE_ERROR;
  }

  return TranslatePrivilegesForCompatibility(context_->manifest_data.get()) ?
      Status::OK : Status::PRIVILEGE_ERROR;
}

}  // namespace security
}  // namespace common_installer

