// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/security_registration.h"

#include <security-manager.h>

#include <iostream>

#include "utils/logging.h"

namespace bf = boost::filesystem;

namespace {

bool PrepareRequest(const std::string& app_id, const std::string& pkg_id,
    const boost::filesystem::path& path, manifest_x* manifest,
    app_inst_req* req) {
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
    error = security_manager_app_inst_req_add_path(req, path.string().c_str(),
        SECURITY_MANAGER_PATH_PRIVATE);
    if (error != SECURITY_MANAGER_SUCCESS) {
      return false;
    }
  }

  if (manifest) {
    for (privileges_x* privileges = manifest->privileges;
        privileges != nullptr; privileges = privileges->next) {
      for (privilege_x* priv = privileges->privilege;
          priv != nullptr; priv = priv->next) {
        security_manager_app_inst_req_add_privilege(req, priv->text);
      }
    }
  }
  return true;
}

bool RegisterSecurityContext(const std::string& app_id,
    const std::string& pkg_id, const boost::filesystem::path& path,
    manifest_x* manifest) {
  app_inst_req* req;

  int error = security_manager_app_inst_req_new(&req);
  if (error != SECURITY_MANAGER_SUCCESS) {
    LOG(ERROR)
        << "Failed while calling security_manager_app_inst_req_new failed "
        << "(error code: " << error << ")";
    return false;
  }

  if (!PrepareRequest(app_id, pkg_id, path, manifest, req)) {
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
    const std::string& pkg_id) {
  app_inst_req* req;

  int error = security_manager_app_inst_req_new(&req);
  if (error != SECURITY_MANAGER_SUCCESS) {
    LOG(ERROR) << "Failed while calling security_manager_app_inst_req_new  "
               << "(error code: " << error << ")";
    return false;
  }

  if (!PrepareRequest(app_id, pkg_id, bf::path(), nullptr, req)) {
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
    const std::string& pkg_id, const boost::filesystem::path& path,
    manifest_x* manifest) {
  for (uiapplication_x* ui = manifest->uiapplication;
      ui != nullptr; ui = ui->next) {
    if (!ui->appid) {
      return false;
    }
    if (!RegisterSecurityContext(ui->appid, pkg_id, path, manifest)) {
      return false;
    }
  }

  for (serviceapplication_x* svc =
      manifest->serviceapplication;
      svc != nullptr; svc = svc->next) {
    if (!svc->appid) {
      return false;
    }
    if (!RegisterSecurityContext(svc->appid, pkg_id, path,  manifest)) {
      return false;
    }
  }
  return true;
}

bool UnregisterSecurityContextForApps(
    const std::string& pkg_id, manifest_x* manifest) {
  for (uiapplication_x* ui = manifest->uiapplication;
      ui != nullptr; ui = ui->next) {
    if (!ui->appid) {
      return false;
    }
    if (!UnregisterSecurityContext(ui->appid, pkg_id)) {
      return false;
    }
  }

  for (serviceapplication_x* svc =
      manifest->serviceapplication;
      svc != nullptr; svc = svc->next) {
    if (!svc->appid) {
      return false;
    }
    if (!UnregisterSecurityContext(svc->appid, pkg_id)) {
      return false;
    }
  }
  return true;
}

}  // namespace common_installer
