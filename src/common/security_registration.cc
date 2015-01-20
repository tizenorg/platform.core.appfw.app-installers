// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/security_registration.h"

#include <security-manager.h>

#include <iostream>

// TODO(t.iwanek): logging mechanism...
#define DBG(msg) std::cout << "[DEBUG:SecurityContext] " << msg << std::endl;
#define ERR(msg) std::cout << "[ERROR:SecurityContext] " << msg << std::endl;

namespace bf = boost::filesystem;

namespace {

bool PrepareRequest(const std::string& app_id, const std::string& pkg_id,
    uid_t uid, const boost::filesystem::path& path, manifest_x* manifest,
    app_inst_req* req) {
  if (app_id.empty() || pkg_id.empty()) {
    ERR("Appid or pkgid is empty. Both values must be set");
    return false;
  }

  // FIXME remove setting app_id at all
  // this is broken for multiple apps in one package
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

  // TODO(t.iwanek): uncomment after security-manager release
  // error = security_manager_app_inst_req_set_uid(req, uid);
  // if (error != SECURITY_MANAGER_SUCCESS) {
  //   return false;
  // }

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

}  // namespace

namespace common_installer {

bool RegisterSecurityContext(const std::string& app_id,
    const std::string& pkg_id, uid_t uid, const boost::filesystem::path& path,
    manifest_x* manifest) {
  app_inst_req* req;

  int error = security_manager_app_inst_req_new(&req);
  if (error != SECURITY_MANAGER_SUCCESS) {
    ERR("Failed while calling security_manager_app_inst_req_new failed "
        << "(error code: " << error << ")");
    return false;
  }

  if (!PrepareRequest(app_id, pkg_id, uid, path, manifest, req)) {
      ERR("Failed while preparing security_manager_app_inst_req");
      security_manager_app_inst_req_free(req);
      return false;
  }

  error = security_manager_app_install(req);
  if (error != SECURITY_MANAGER_SUCCESS) {
    ERR("Failed while calling security_manager_app_install failed "
        << "(error code: " << error << ")");
    security_manager_app_inst_req_free(req);
    return false;
  }

  security_manager_app_inst_req_free(req);
  return true;
}


bool UnregisterSecurityContext(const std::string& app_id,
    const std::string& pkg_id, uid_t uid) {
  app_inst_req* req;

  int error = security_manager_app_inst_req_new(&req);
  if (error != SECURITY_MANAGER_SUCCESS) {
    ERR("Failed while calling security_manager_app_inst_req_new  "
        << "(error code: " << error << ")");
    return false;
  }

  if (!PrepareRequest(app_id, pkg_id, uid, bf::path(), nullptr, req)) {
    ERR("Failed while preparing security_manager_app_inst_req");
    security_manager_app_inst_req_free(req);
    return false;
  }

  error = security_manager_app_uninstall(req);
  if (error != SECURITY_MANAGER_SUCCESS) {
    ERR("Failed while calling  security_manager_app_uninstall failed "
        << "(error code: " << error << ")");
    security_manager_app_inst_req_free(req);
    return false;
  }

  security_manager_app_inst_req_free(req);
  return true;
}

}  // namespace common_installer
