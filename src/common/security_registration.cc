// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/security_registration.h"

#include <boost/filesystem/operations.hpp>
#include <security-manager.h>

#include <utility>
#include <vector>

#include "common/utils/clist_helpers.h"
#include "common/utils/logging.h"

namespace bf = boost::filesystem;

namespace {

const std::vector<std::pair<const char*,
                            app_install_path_type>> kSecurityPolicies = {
  {"/", SECURITY_MANAGER_PATH_PUBLIC_RO},
  {"bin/", SECURITY_MANAGER_PATH_RO},
  {"data/", SECURITY_MANAGER_PATH_RW},
  {"cache/", SECURITY_MANAGER_PATH_RW},
  {"lib/", SECURITY_MANAGER_PATH_RO},
  {"res/", SECURITY_MANAGER_PATH_RO},
  {"shared/", SECURITY_MANAGER_PATH_PUBLIC_RO},
  {"tmp/", SECURITY_MANAGER_PATH_RW}
};

bool PrepareRequest(const std::string& app_id, const std::string& pkg_id,
    const std::string& pkg_type, const boost::filesystem::path& path,
    manifest_x* manifest, app_inst_req* req) {
  if (app_id.empty() || pkg_id.empty()) {
    LOG(ERROR) << "Appid or pkgid is empty. Both values must be set";
    return false;
  }

  int error = security_manager_app_inst_req_set_app_id(req,
      app_id.c_str());
  if (error != SECURITY_MANAGER_SUCCESS) {
    return false;
  }

  error = security_manager_app_inst_req_set_pkg_id(req,
      pkg_id.c_str());
  if (error != SECURITY_MANAGER_SUCCESS) {
    return false;
  }

  if (!path.empty()) {
    for (auto& policy : kSecurityPolicies) {
      bf::path subpath = path / policy.first;
      if (bf::exists(subpath)) {
        error = security_manager_app_inst_req_add_path(req, subpath.c_str(),
                                                       policy.second);
        if (error != SECURITY_MANAGER_SUCCESS) {
          return false;
        }
      }
    }
  }

  if (manifest) {
    privileges_x *privileges = nullptr;
    PKGMGR_LIST_MOVE_NODE_TO_HEAD(manifest->privileges, privileges);
    for (; privileges != nullptr; privileges = privileges->next) {
      privilege_x* priv = nullptr;
      PKGMGR_LIST_MOVE_NODE_TO_HEAD(privileges->privilege, priv);
      for (; priv != nullptr; priv = priv->next) {
        security_manager_app_inst_req_add_privilege(req, priv->text);
      }
    }
  }

  // Add special privilege to provide default privileges for webapp.
  // The privilege(http://tizen.org/privilege/webappdefault) is managed
  // in security-manager. The security-manager will add default privileges
  // for webapp to cynara if the privilege is included.
  if (pkg_type == "wgt") {
    security_manager_app_inst_req_add_privilege(
        req, "http://tizen.org/privilege/webappdefault");
  }

  return true;
}

bool RegisterSecurityContext(const std::string& app_id,
    const std::string& pkg_id, const std::string& pkg_type, 
    const boost::filesystem::path& path, manifest_x* manifest) {
  app_inst_req* req;

  int error = security_manager_app_inst_req_new(&req);
  if (error != SECURITY_MANAGER_SUCCESS) {
    LOG(ERROR)
        << "Failed while calling security_manager_app_inst_req_new failed "
        << "(error code: " << error << ")";
    return false;
  }

  if (!PrepareRequest(app_id, pkg_id, pkg_type, path, manifest, req)) {
      LOG(ERROR) << "Failed while preparing security_manager_app_inst_req";
      security_manager_app_inst_req_free(req);
      return false;
  }

  error = security_manager_app_install(req);
  if (error != SECURITY_MANAGER_SUCCESS) {
    LOG(ERROR) << "Failed while calling security_manager_app_install failed "
               << "(error code: " << error << ")";
    security_manager_app_inst_req_free(req);
    return false;
  }

  security_manager_app_inst_req_free(req);
  return true;
}


bool UnregisterSecurityContext(const std::string& app_id,
    const std::string& pkg_id, const std::string& pkg_type) {
  app_inst_req* req;

  int error = security_manager_app_inst_req_new(&req);
  if (error != SECURITY_MANAGER_SUCCESS) {
    LOG(ERROR) << "Failed while calling security_manager_app_inst_req_new  "
               << "(error code: " << error << ")";
    return false;
  }

  if (!PrepareRequest(app_id, pkg_id, pkg_type, bf::path(), nullptr, req)) {
    LOG(ERROR) << "Failed while preparing security_manager_app_inst_req";
    security_manager_app_inst_req_free(req);
    return false;
  }

  error = security_manager_app_uninstall(req);
  if (error != SECURITY_MANAGER_SUCCESS) {
    LOG(ERROR) << "Failed while calling  security_manager_app_uninstall failed "
               << "(error code: " << error << ")";
    security_manager_app_inst_req_free(req);
    return false;
  }

  security_manager_app_inst_req_free(req);
  return true;
}

}  // namespace

namespace common_installer {

bool RegisterSecurityContextForApps(
    const std::string& pkg_id, const std::string& pkg_type,
    const boost::filesystem::path& path, manifest_x* manifest) {
  for (uiapplication_x* ui = manifest->uiapplication;
      ui != nullptr; ui = ui->next) {
    if (!ui->appid) {
      return false;
    }
    if (!RegisterSecurityContext(ui->appid, pkg_id, pkg_type,
        path, manifest)) {
      return false;
    }
  }

  for (serviceapplication_x* svc =
      manifest->serviceapplication;
      svc != nullptr; svc = svc->next) {
    if (!svc->appid) {
      return false;
    }
    if (!RegisterSecurityContext(svc->appid, pkg_id, pkg_type,
        path, manifest)) {
      return false;
    }
  }
  return true;
}

bool UnregisterSecurityContextForApps(
    const std::string& pkg_id, const std::string& pkg_type,
    manifest_x* manifest) {
  for (uiapplication_x* ui = manifest->uiapplication;
      ui != nullptr; ui = ui->next) {
    if (!ui->appid) {
      return false;
    }
    if (!UnregisterSecurityContext(ui->appid, pkg_id, pkg_type)) {
      return false;
    }
  }

  for (serviceapplication_x* svc =
      manifest->serviceapplication;
      svc != nullptr; svc = svc->next) {
    if (!svc->appid) {
      return false;
    }
    if (!UnregisterSecurityContext(svc->appid, pkg_id, pkg_type)) {
      return false;
    }
  }
  return true;
}

}  // namespace common_installer
