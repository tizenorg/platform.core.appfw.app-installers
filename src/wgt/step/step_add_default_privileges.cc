// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "wgt/step/step_add_default_privileges.h"

#include <pkgmgrinfo_basic.h>

#include <cstdlib>
#include <cstring>
#include <memory>

#include "common/utils/clist_helpers.h"

namespace {

const char kPrivForWebApp[] =
    "http://tizen.org/privilege/internal/webappdefault";
const char kPrivForPublic[] =
    "http://tizen.org/privilege/internal/default/public";
const char kPrivForPartner[] =
    "http://tizen.org/privilege/internal/default/partner";
const char kPrivForPlatform[] =
    "http://tizen.org/privilege/internal/default/platform";

bool AddPrivilegeToList(manifest_x* m, const char* priv_str) {
  if (!m->privileges) {
    m->privileges =
      reinterpret_cast<privileges_x*>(calloc(1, sizeof(privileges_x*)));
    if (!m->privileges)
      return false;
  }
  privilege_x* priv =
      reinterpret_cast<privilege_x*>(calloc(1, sizeof(privilege_x)));
  if (!priv)
    return false;

  priv->text = strdup(priv_str);
  LISTADD(m->privileges->privilege, priv);
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
  if (!AddPrivilegeToList(context_->manifest_data.get(), kPrivForWebApp)) {
    LOG(ERROR) << "Error during adding default privileges for webapp.";
    return Status::ERROR;
  }

  bool ret = true;
  switch(context_->privilege_level.get()) {
    case common_installer::PrivilegeLevel::PUBLIC:
      ret = AddPrivilegeToList(context_->manifest_data.get(),
                               kPrivForPublic);
      break;
    case common_installer::PrivilegeLevel::PARTNER:
      ret = AddPrivilegeToList(context_->manifest_data.get(),
                               kPrivForPartner);
      break;
    case common_installer::PrivilegeLevel::PLATFORM:
      ret = AddPrivilegeToList(context_->manifest_data.get(),
                               kPrivForPlatform);
      break;
    default:
      // No default privileges for untrusted application.
      break;
  }
  if (!ret) {
    LOG(ERROR) << "Error during adding default privileges for certlevel.";
    return Status::ERROR;
  }

  return Status::OK;
}

}  // namespace security
}  // namespace wgt
