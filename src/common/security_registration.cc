// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/security_registration.h"

#include <boost/filesystem/operations.hpp>
#include <boost/format.hpp>

#include <manifest_parser/utils/logging.h>
#include <security-manager.h>

#include <utility>
#include <vector>
#include <algorithm>

#include "common/pkgmgr_query.h"
#include "common/utils/glist_range.h"

namespace bf = boost::filesystem;
namespace ci = common_installer;

namespace {

const std::vector<std::pair<const char*,
                            app_install_path_type>> kSecurityPolicies = {
  {"/", SECURITY_MANAGER_PATH_PUBLIC_RO},
  {"bin", SECURITY_MANAGER_PATH_RO},
  {"data", SECURITY_MANAGER_PATH_RW},
  {"cache", SECURITY_MANAGER_PATH_RW},
  {"lib", SECURITY_MANAGER_PATH_RO},
  {"res", SECURITY_MANAGER_PATH_RO},
  {"shared", SECURITY_MANAGER_PATH_PUBLIC_RO},
  {"shared/data", SECURITY_MANAGER_PATH_OWNER_RW_OTHER_RO},
  {"shared/cache", SECURITY_MANAGER_PATH_OWNER_RW_OTHER_RO},
  {"shared/trusted", SECURITY_MANAGER_PATH_TRUSTED_RW},
  {"tep", SECURITY_MANAGER_PATH_RO},
  {".image", SECURITY_MANAGER_PATH_RO},
  {"tmp", SECURITY_MANAGER_PATH_RW}
};

bool PrepareRequest(const std::string& app_id, const std::string& pkg_id,
    const std::string& author_id, const std::string& api_version,
    ci::SecurityAppInstallType sec_install_type,
    const boost::filesystem::path& path,
    uid_t uid, const std::vector<std::string>& privileges,
    app_inst_req* req, std::string* error_message) {
  if (app_id.empty() || pkg_id.empty()) {
    LOG(ERROR) << "Appid or pkgid is empty. Both values must be set";
    return false;
  }

  int error = security_manager_app_inst_req_set_app_id(req,
      app_id.c_str());
  if (error != SECURITY_MANAGER_SUCCESS) {
    std::string errnum = boost::str(boost::format("%d") % error);
    *error_message = security_manager_strerror(static_cast<lib_retcode>(error));
    *error_message += ":<" + errnum + ">";
    return false;
  }

  error = security_manager_app_inst_req_set_pkg_id(req,
      pkg_id.c_str());
  if (error != SECURITY_MANAGER_SUCCESS) {
    std::string errnum = boost::str(boost::format("%d") % error);
    *error_message = security_manager_strerror(static_cast<lib_retcode>(error));
    *error_message += ":<" + errnum + ">";
    return false;
  }

  error = security_manager_app_inst_req_set_uid(req, uid);
  if (error != SECURITY_MANAGER_SUCCESS) {
    std::string errnum = boost::str(boost::format("%d") % error);
    *error_message = security_manager_strerror(static_cast<lib_retcode>(error));
    *error_message += ":<" + errnum + ">";
    return false;
  }

  if (!api_version.empty()) {
    error = security_manager_app_inst_req_set_target_version(req,
        api_version.c_str());
    if (error != SECURITY_MANAGER_SUCCESS) {
      std::string errnum = boost::str(boost::format("%d") % error);
      *error_message =
          security_manager_strerror(static_cast<lib_retcode>(error));
      *error_message += ":<" + errnum + ">";
      return false;
    }
  }

  if (!author_id.empty()) {
    error = security_manager_app_inst_req_set_author_id(req, author_id.c_str());
    if (error != SECURITY_MANAGER_SUCCESS) {
      std::string errnum = boost::str(boost::format("%d") % error);
      *error_message =
          security_manager_strerror(static_cast<lib_retcode>(error));
      *error_message += ":<" + errnum + ">";
      return false;
    }
  }

  if (sec_install_type != ci::SecurityAppInstallType::None) {
    app_install_type type = SM_APP_INSTALL_NONE;

    switch (sec_install_type) {
      case ci::SecurityAppInstallType::Local:
        type = SM_APP_INSTALL_LOCAL;
        break;
      case ci::SecurityAppInstallType::Preload:
        type = SM_APP_INSTALL_PRELOADED;
        break;
      case ci::SecurityAppInstallType::Global:
        type = SM_APP_INSTALL_GLOBAL;
        break;
      case ci::SecurityAppInstallType::None:
        break;
      default:
        break;
    }

    LOG(INFO) << "install_type(" << type << ")";
    error = security_manager_app_inst_req_set_install_type(req, type);
    if (error != SECURITY_MANAGER_SUCCESS) {
      std::string errnum = boost::str(boost::format("%d") % error);
      *error_message =
          security_manager_strerror(static_cast<lib_retcode>(error));
      *error_message += ":<" + errnum + ">";
      return false;
    }
  }

  if (!path.empty()) {
    for (auto& policy : kSecurityPolicies) {
      bf::path subpath = path / policy.first;
      if (bf::exists(subpath)) {
        if (bf::is_symlink(subpath)) {
          LOG(DEBUG) << "Path " << subpath << " is a symlink."
                     << "Path will not be registered";
          continue;
        }
        if (policy.second == SECURITY_MANAGER_PATH_TRUSTED_RW &&
            author_id.empty()) {
          LOG(WARNING) << "the path " << policy.first
              << " exists, but author_id is empty";
          continue;
        }
        error = security_manager_app_inst_req_add_path(req, subpath.c_str(),
                                                       policy.second);
        if (error != SECURITY_MANAGER_SUCCESS) {
          std::string errnum = boost::str(boost::format("%d") % error);
          *error_message =
                    security_manager_strerror(static_cast<lib_retcode>(error));
          *error_message += ":<" + errnum + ">";
          return false;
        }
      }
    }
  }

  for (auto& priv : privileges) {
    security_manager_app_inst_req_add_privilege(req, priv.c_str());
  }
  return true;
}

bool PreparePathRequest(const std::string& pkg_id,
    const boost::filesystem::path& path, uid_t uid, bool is_userdir,
    path_req* req, std::string* error_message) {
  if (pkg_id.empty() || path.empty()) {
    LOG(ERROR) << "Pkgid or path is empty. Both values must be set";
    return false;
  }

  int error = security_manager_path_req_set_pkg_id(req, pkg_id.c_str());
  if (error != SECURITY_MANAGER_SUCCESS) {
    std::string errnum = boost::str(boost::format("%d") % error);
    *error_message = security_manager_strerror(static_cast<lib_retcode>(error));
    *error_message += ":<" + errnum + ">";
    return false;
  }

  error = security_manager_path_req_set_uid(req, uid);
  if (error != SECURITY_MANAGER_SUCCESS) {
    std::string errnum = boost::str(boost::format("%d") % error);
    *error_message = security_manager_strerror(static_cast<lib_retcode>(error));
    *error_message += ":<" + errnum + ">";
    return false;
  }

  app_install_type type =
      is_userdir ? SM_APP_INSTALL_LOCAL :
      getuid() == 0 ? SM_APP_INSTALL_PRELOADED :
      SM_APP_INSTALL_GLOBAL;

  error = security_manager_path_req_set_install_type(req, type);
  if (error != SECURITY_MANAGER_SUCCESS) {
    std::string errnum = boost::str(boost::format("%d") % error);
    *error_message =
        security_manager_strerror(static_cast<lib_retcode>(error));
    *error_message += ":<" + errnum + ">";
    return false;
  }

  for (auto& policy : kSecurityPolicies) {
    bf::path subpath = path / policy.first;
    if (bf::exists(subpath)) {
      if (bf::is_symlink(subpath)) {
        LOG(DEBUG) << "Path " << subpath << " is a symlink."
                   << "Path will not be registered";
        continue;
      }
      error = security_manager_path_req_add_path(req, subpath.c_str(),
          policy.second);
      if (error != SECURITY_MANAGER_SUCCESS) {
        std::string errnum = boost::str(boost::format("%d") % error);
        *error_message =
                  security_manager_strerror(static_cast<lib_retcode>(error));
        *error_message += ":<" + errnum + ">";
        return false;
      }
    }
  }

  return true;
}

}  // namespace

namespace common_installer {

bool RegisterSecurityContext(const std::string& app_id,
    const std::string& pkg_id, const std::string& author_id,
    const std::string& api_version, SecurityAppInstallType type,
    const boost::filesystem::path& path, uid_t uid,
    const std::vector<std::string>& privileges,
    std::string* error_message) {
  app_inst_req* req;

  int error = security_manager_app_inst_req_new(&req);
  if (error != SECURITY_MANAGER_SUCCESS) {
    LOG(ERROR)
        << "Failed while calling security_manager_app_inst_req_new failed "
        << "(error code: " << error << ")";
    std::string errnum = boost::str(boost::format("%d") % error);
    *error_message = security_manager_strerror(static_cast<lib_retcode>(error));
    *error_message += ":<" + errnum + ">";
    return false;
  }

  if (!PrepareRequest(app_id, pkg_id, author_id, api_version, type, path, uid,
      privileges, req, error_message)) {
    LOG(ERROR) << "Failed while preparing security_manager_app_inst_req";
    security_manager_app_inst_req_free(req);
    return false;
  }

  error = security_manager_app_install(req);
  if (error != SECURITY_MANAGER_SUCCESS) {
    LOG(ERROR) << "Failed while calling security_manager_app_install failed "
               << "(error code: " << error << ")";
    std::string errnum = boost::str(boost::format("%d") % error);
    *error_message = security_manager_strerror(static_cast<lib_retcode>(error));
    *error_message += ":<" + errnum + ">";
    security_manager_app_inst_req_free(req);
    return false;
  }

  security_manager_app_inst_req_free(req);
  return true;
}

bool UnregisterSecurityContext(const std::string& app_id,
    const std::string& pkg_id, uid_t uid, std::string* error_message) {
  app_inst_req* req;

  int error = security_manager_app_inst_req_new(&req);
  if (error != SECURITY_MANAGER_SUCCESS) {
    LOG(ERROR) << "Failed while calling security_manager_app_inst_req_new  "
               << "(error code: " << error << ")";
    std::string errnum = boost::str(boost::format("%d") % error);
    *error_message = security_manager_strerror(static_cast<lib_retcode>(error));
    *error_message += ":<" + errnum + ">";
    return false;
  }

  if (!PrepareRequest(app_id, pkg_id, std::string(), std::string(),
                      ci::SecurityAppInstallType::None, bf::path(), uid, {},
                      req, error_message)) {
    LOG(ERROR) << "Failed while preparing security_manager_app_inst_req";
    security_manager_app_inst_req_free(req);
    return false;
  }

  error = security_manager_app_uninstall(req);
  if (error != SECURITY_MANAGER_SUCCESS) {
    LOG(ERROR) << "Failed while calling  security_manager_app_uninstall failed "
               << "(error code: " << error << ")";
    std::string errnum = boost::str(boost::format("%d") % error);
    *error_message = security_manager_strerror(static_cast<lib_retcode>(error));
    *error_message += ":<" + errnum + ">";
    security_manager_app_inst_req_free(req);
    return false;
  }

  security_manager_app_inst_req_free(req);
  return true;
}

bool RegisterSecurityContextForManifest(
    const std::string& pkg_id, const boost::filesystem::path& path, uid_t uid,
    common_installer::CertificateInfo* cert_info, manifest_x* manifest,
    std::string* error_message) {
  std::vector<std::string> priv_vec;
  for (const char* priv : GListRange<char*>(manifest->privileges)) {
    priv_vec.emplace_back(priv);
  }
  for (application_x* app : GListRange<application_x*>(manifest->application)) {
    if (!app->appid) {
      return false;
    }
    auto sec_app_type = (strcmp(manifest->preload, "true") == 0) ?
            ci::SecurityAppInstallType::Preload :
            ((uid == GLOBAL_USER || uid == 0) ?
                ci::SecurityAppInstallType::Global :
                ci::SecurityAppInstallType::Local);

    if (!RegisterSecurityContext(app->appid, pkg_id, cert_info->author_id.get(),
        manifest->api_version, sec_app_type, path, uid, priv_vec,
        error_message)) {
      return false;
    }
  }
  return true;
}

bool UnregisterSecurityContextForManifest(const std::string& pkg_id,
    uid_t uid, manifest_x* manifest, std::string* error_message) {
  for (application_x* app : GListRange<application_x*>(manifest->application)) {
    if (!app->appid) {
      return false;
    }
    if (!UnregisterSecurityContext(app->appid, pkg_id, uid, error_message)) {
      return false;
    }
  }
  return true;
}

bool UnregisterSecurityContextForPkgId(const std::string &pkg_id,
    uid_t uid, std::string* error_message) {
  std::vector<std::string> appids;
  if (!QueryAppidsForPkgId(pkg_id, &appids, uid))
    return false;
  for (auto& appid : appids) {
    if (!UnregisterSecurityContext(appid, pkg_id, uid, error_message)) {
      return false;
    }
  }
  return true;
}

bool RegisterSecurityContextForPath(const std::string &pkg_id,
    const boost::filesystem::path& path, uid_t uid, bool is_userdir,
    std::string* error_message) {
  path_req* req;
  int error = security_manager_path_req_new(&req);
  if (error != SECURITY_MANAGER_SUCCESS) {
    LOG(ERROR)
        << "Failed while calling security_manager_path_req_new failed "
        << "(error code: " << error << ")";
    std::string errnum = boost::str(boost::format("%d") % error);
    *error_message = security_manager_strerror(static_cast<lib_retcode>(error));
    *error_message += ":<" + errnum + ">";
    return false;
  }

  if (!PreparePathRequest(pkg_id, path, uid, is_userdir, req, error_message)) {
    LOG(ERROR) << "Failed while preparing security_manager_app_inst_req";
    security_manager_path_req_free(req);
    return false;
  }

  error = security_manager_paths_register(req);
  if (error != SECURITY_MANAGER_SUCCESS) {
    LOG(ERROR) << "Failed while calling security_manager_paths_register failed "
               << "(error code: " << error << ")";
    std::string errnum = boost::str(boost::format("%d") % error);
    *error_message = security_manager_strerror(static_cast<lib_retcode>(error));
    *error_message += ":<" + errnum + ">";
    security_manager_path_req_free(req);
    return false;
  }

  security_manager_path_req_free(req);

  return true;
}

}  // namespace common_installer
