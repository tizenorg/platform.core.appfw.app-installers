// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/security_registration.h"

#include <boost/filesystem/operations.hpp>
#include <security-manager.h>

#include <utility>
#include <vector>
#include <algorithm>

#include "common/utils/glist_range.h"
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
    std::vector<std::string> priv_vec;
    for (const char* priv : GListRange<char*>(manifest->privileges)) {
      priv_vec.emplace_back(priv);
    }

    std::sort(priv_vec.begin(), priv_vec.end());
    for (auto& priv : priv_vec) {
      security_manager_app_inst_req_add_privilege(req, priv.c_str());
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
  for (application_x* app : GListRange<application_x*>(manifest->application)) {
    if (!app->appid) {
      return false;
    }
    if (!RegisterSecurityContext(app->appid, pkg_id,
        path, manifest)) {
      return false;
    }
  }
  return true;
}

bool UnregisterSecurityContextForApps(
    const std::string& pkg_id, manifest_x* manifest) {
  for (application_x* app : GListRange<application_x*>(manifest->application)) {
    if (!app->appid) {
      return false;
    }
    if (!UnregisterSecurityContext(app->appid, pkg_id)) {
      return false;
    }
  }
  return true;
}

}  // namespace common_installer
