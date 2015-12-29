// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_privilege_compatibility.h"

#include <pkgmgrinfo_basic.h>
#include <privilege_manager.h>

#include <cstdlib>
#include <cstring>
#include <memory>
#include <string>

#include "common/utils/glist_range.h"

namespace {

const char kPrivForPublic[] =
    "http://tizen.org/privilege/internal/default/public";
const char kPrivForPartner[] =
    "http://tizen.org/privilege/internal/default/partner";
const char kPrivForPlatform[] =
    "http://tizen.org/privilege/internal/default/platform";
using ManifestXWrapperPtr = std::shared_ptr<ManifestXWrapper>;

bool TranslatePrivilegesForCompatibility(const ManifestXWrapperPtr& manifest) {
  if (!manifest->IsApiVersionExist()) {
    LOG(WARNING) << "Skipping privileges mapping because api-version "
                 << "is not specified by package";
    return true;
  }

  // No privileges to map
  if (!manifest->privileges) {
    return true;
  }

  // get mapping
  GList* mapped_privilege_list;

  bool is_webapp = manifest->IsWgtType();

  LOG(DEBUG) << "type = " << manifest->Type();
  if (privilege_manager_get_mapped_privilege_list(manifest->ApiVersion().c_str(),
      is_webapp ? PRVMGR_PACKAGE_TYPE_WRT : PRVMGR_PACKAGE_TYPE_CORE,
      manifest->privileges, &mapped_privilege_list) != PRVMGR_ERR_NONE) {
    LOG(ERROR) << "privilege_manager_get_mapped_privilege_list failed";
    return false;
  } else if (mapped_privilege_list == NULL) {
    LOG(ERROR) << "privilege_manager_get_mapped_privilege_list failed";
    return false;
  }

  // set pkgmgr new list
  g_list_free_full(manifest->privileges, free);
  manifest->privileges = nullptr;
  for (GList* l = mapped_privilege_list; l != NULL; l = l->next) {
    manifest->privileges = g_list_append(manifest->privileges,
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
    return Status::ERROR;
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
                        strdup(kPrivForPublic));
      break;
    case common_installer::PrivilegeLevel::PARTNER:
      context_->manifest_data.get()->privileges =
          g_list_append(context_->manifest_data.get()->privileges,
                        strdup(kPrivForPartner));
      break;
    case common_installer::PrivilegeLevel::PLATFORM:
      context_->manifest_data.get()->privileges =
          g_list_append(context_->manifest_data.get()->privileges,
                        strdup(kPrivForPlatform));
      break;
    default:
      // TODO(jongmyeong.ko): temporarily, public privileges for untrusted
      // application.
      context_->manifest_data.get()->privileges =
          g_list_append(context_->manifest_data.get()->privileges,
                        strdup(kPrivForPublic));
      break;
  }
  if (!ret) {
    LOG(ERROR) << "Error during adding default privileges for certificates.";
    return Status::ERROR;
  }

  return TranslatePrivilegesForCompatibility(context_->manifest_data.get()) ?
      Status::OK : Status::ERROR;
}

}  // namespace security
}  // namespace common_installer

