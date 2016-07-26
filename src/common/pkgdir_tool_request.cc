// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/pkgdir_tool_request.h"

#include <glib.h>
#include <gio/gio.h>
#include <manifest_parser/utils/logging.h>

#include "common/shared_dirs.h"

namespace {

const char kDBusServiceName[] = "org.tizen.pkgdir_tool";
const char kDBusObjectPath[] = "/org/tizen/pkgdir_tool";
const char kDBusInterfaceName[] = "org.tizen.pkgdir_tool";

bool DBusRequestForDirectoryOperation(const char* method,
    GVariant* parameter) {
  GError* err = nullptr;
  GDBusConnection* con = g_bus_get_sync(G_BUS_TYPE_SYSTEM, nullptr, &err);
  if (!con || err) {
    LOG(WARNING) << "Failed to get dbus connection: " << err->message;
    g_error_free(err);
    return false;
  }
  GDBusProxy* proxy = g_dbus_proxy_new_sync(con, G_DBUS_PROXY_FLAGS_NONE,
      nullptr, kDBusServiceName, kDBusObjectPath, kDBusInterfaceName, nullptr,
      &err);
  if (!proxy) {
    std::string err_msg;
    if (err) {
      err_msg = std::string(err->message);
      g_error_free(err);
    }
    LOG(ERROR) << "Failed to get dbus proxy: " << err_msg;
    g_object_unref(con);
    return false;
  }
  GVariant* r = g_dbus_proxy_call_sync(proxy, method,
      parameter, G_DBUS_CALL_FLAGS_NONE, -1, nullptr,
      &err);
  if (!r) {
    std::string err_msg;
    if (err) {
      err_msg = std::string(err->message);
      g_error_free(err);
    }
    LOG(ERROR) << "Failed to request: " << err_msg;
    g_object_unref(proxy);
    g_object_unref(con);
    return false;
  }
  bool result;
  g_variant_get(r, "(b)", &result);

  g_variant_unref(r);
  g_object_unref(proxy);
  g_object_unref(con);

  return result;
}

bool RequestUserDirectoryOperation(const char* method,
    const std::string& pkgid) {
  GVariant* parameter = g_variant_new("(s)", pkgid.c_str());
  return DBusRequestForDirectoryOperation(method, parameter);
}

bool RequestUserDirectoryOperationForUser(const char* method,
    const std::string& pkgid, uid_t uid) {
  GVariant* parameter = g_variant_new("(si)", pkgid.c_str(), uid);
  return DBusRequestForDirectoryOperation(method, parameter);
}

}  // namespace

namespace common_installer {

bool RequestCopyUserDirectories(const std::string& pkgid) {
  if (!RequestUserDirectoryOperation("CopyUserDirs", pkgid)) {
    LOG(INFO) << "Try to copy user directories directly";
    return CopyUserDirectories(pkgid);
  }
  return true;
}

bool RequestDeleteUserDirectories(const std::string& pkgid) {
  if (!RequestUserDirectoryOperation("DeleteUserDirs", pkgid)) {
    LOG(INFO) << "Try to delete user directories directly";
    return DeleteUserDirectories(pkgid);
  }
  return true;
}

bool RequestCreateExternalDirectories(const std::string& pkgid) {
  if (!RequestUserDirectoryOperation("CreateExternalDirs", pkgid)) {
    LOG(INFO) << "Try to create external directories directly";
    return PerformExternalDirectoryCreationForAllUsers(pkgid);
  }
  return true;
}

bool RequestDeleteExternalDirectories(const std::string& pkgid) {
  if (!RequestUserDirectoryOperation("DeleteExternalDirs", pkgid)) {
    LOG(INFO) << "Try to remove external directories directly";
    return PerformExternalDirectoryDeletionForAllUsers(pkgid);
  }
  return true;
}

bool RequestCreateLegacyDirectories(const std::string& pkgid) {
  RequestUserDirectoryOperation("CreateLegacyDirs", pkgid);
  return true;
}

bool RequestDeleteLegacyDirectories(const std::string& pkgid) {
  RequestUserDirectoryOperation("DeleteLegacyDirs", pkgid);
  return true;
}

bool RequestCreateGlobalAppSymlinks(const std::string& pkgid) {
  bool result =
      RequestUserDirectoryOperation("CreateGlobalAppSymlinks", pkgid);
  if (!result) {
    LOG(INFO) << "Try to create symlinks for global app directly";
    return CreateGlobalAppSymlinksForAllUsers(pkgid);
  }
  return result;
}

bool RequestCreateGlobalAppSymlinksForUser(const std::string& pkgid,
                                           uid_t uid) {
  bool result =
      RequestUserDirectoryOperationForUser("CreateGlobalAppSymlinksForUser",
                                     pkgid, uid);
  if (!result) {
    LOG(INFO) << "Try to create symlinks for global app directly";
    return CreateGlobalAppSymlinksForUser(pkgid, uid);
  }
  return result;
}

bool RequestDeleteGlobalAppSymlinks(const std::string& pkgid) {
  bool result =
      RequestUserDirectoryOperation("DeleteGlobalAppSymlinks", pkgid);
  if (!result) {
    LOG(INFO) << "Try to delete symlinks for global app directly";
    return DeleteGlobalAppSymlinksForAllUsers(pkgid);
  }
  return result;
}

bool RequestDeleteGlobalAppSymlinksForUser(const std::string& pkgid,
                                           uid_t uid) {
  bool result =
      RequestUserDirectoryOperationForUser("DeleteGlobalAppSymlinksForUser",
                                     pkgid, uid);
  if (!result) {
    LOG(INFO) << "Try to delete symlinks for global app directly";
    return DeleteGlobalAppSymlinksForUser(pkgid, uid);
  }
  return result;
}

}  // namespace common_installer
