// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/security_registration.h"

#include <boost/filesystem/operations.hpp>
#include <security-manager.h>

#include <utility>
#include <vector>
#include <algorithm>

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

bool PrepareRequest(const std::string& pkg_id,
    const boost::filesystem::path& path, manifest_x* manifest,
    app_inst_req* req) {
  if (pkg_id.empty()) {
    LOG(ERROR) << "Pkgid is empty. Both values must be set";
    return false;
  }

  int error = security_manager_app_inst_req_set_pkg_id(req,
      pkg_id.c_str());
  if (error != SECURITY_MANAGER_SUCCESS) {
    return false;
  }

  // TODO(t.iwanek): due to the fact that we are registering package not an
  // applications, appid passing was removed from execution path and we make
  // an appid for security-manager to pass any. Either way, we do not know which
  // app id to pass. This funcation call should be removed at all when security
  // manager request will not force to pass app id.
  std::string fake_app_id = pkg_id + "." + pkg_id;
  error = security_manager_app_inst_req_set_app_id(req,
      fake_app_id.c_str());
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

    privileges_x *privileges = nullptr;
    PKGMGR_LIST_MOVE_NODE_TO_HEAD(manifest->privileges, privileges);
    for (; privileges != nullptr; privileges = privileges->next) {
      privilege_x* priv = nullptr;
      PKGMGR_LIST_MOVE_NODE_TO_HEAD(privileges->privilege, priv);
      for (; priv != nullptr; priv = priv->next) {
        priv_vec.push_back(priv->text);
      }
    }

    // privileges should be sorted.
    std::sort(priv_vec.begin(), priv_vec.end());
    for (auto& priv : priv_vec) {
      security_manager_app_inst_req_add_privilege(req, priv.c_str());
    }
  }

  return true;
}

}  // namespace

namespace common_installer {

bool RegisterSecurityContext(
    const std::string& pkg_id,
    const boost::filesystem::path& path,
    manifest_x* manifest) {
  app_inst_req* req;

  int error = security_manager_app_inst_req_new(&req);
  if (error != SECURITY_MANAGER_SUCCESS) {
    LOG(ERROR)
        << "Failed while calling security_manager_app_inst_req_new failed "
        << "(error code: " << error << ")";
    return false;
  }

  if (!PrepareRequest(pkg_id, path, manifest, req)) {
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


bool UnregisterSecurityContext(const std::string& pkg_id) {
  app_inst_req* req;

  int error = security_manager_app_inst_req_new(&req);
  if (error != SECURITY_MANAGER_SUCCESS) {
    LOG(ERROR) << "Failed while calling security_manager_app_inst_req_new  "
               << "(error code: " << error << ")";
    return false;
  }

  if (!PrepareRequest(pkg_id, bf::path(), nullptr, req)) {
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

}  // namespace common_installer
