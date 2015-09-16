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

const char kDefaultPrivilege[] = "http://tizen.org/privilege/webappdefault";

bool AddDefaultPrivilegesForWebApp(manifest_x* m) {
  if (!m->privileges) {
    m->privileges =
      reinterpret_cast<privileges_x*>(calloc(1, sizeof(privileges_x*)));
  }
  privilege_x* priv =
      reinterpret_cast<privilege_x*>(calloc(1, sizeof(privilege_x)));
  priv->text = strdup(kDefaultPrivilege);
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
  return AddDefaultPrivilegesForWebApp(context_->manifest_data.get()) ?
      Status::OK : Status::ERROR;
}

}  // namespace security
}  // namespace wgt
