// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include "common/tzip_interface.h"

#include <manifest_parser/utils/logging.h>

#include <boost/filesystem.hpp>

#include <dbus/dbus.h>

#include <cstdlib>

namespace bf = boost::filesystem;

namespace {
const char kTzipBusName[] = "org.tizen.system.deviced";
const char kTzipObjectPath[] = "/Org/Tizen/System/DeviceD/Tzip";
const char kTzipInterfaceName[] = "org.tizen.system.deviced.Tzip";
const char kTzipMountMethod[] = "Mount";
const char kTzipUnmountMethod[] = "Unmount";
const char kTzipIsMountedMethod[] = "IsMounted";
const char kTzipSmackRule[] = "User::Home";
const int kTzipMountMaximumRetryCount = 15;
}

void DBusMessageDeleter(DBusMessage* message) {
  dbus_message_unref(message);
}

void DBusErrorDeleter(DBusError* error) {
  dbus_error_free(error);
  std::default_delete<DBusError>();
}

namespace common_installer {

class TzipInterface::Pimpl {
 public:
  explicit Pimpl(const boost::filesystem::path& mount_path) :
    mount_path_(mount_path) { }

  bool MountZip(const boost::filesystem::path& zip_path) {
    zip_path_ = zip_path;

    DBusConnection *conn = dbus_bus_get(DBUS_BUS_SYSTEM, nullptr);
    if (!conn) {
      return false;
    }

    if (bf::exists(mount_path_)) {
      bf::remove(mount_path_);
    }

    std::unique_ptr<DBusMessage, void(*)(DBusMessage*)> msg(
        dbus_message_new_method_call(kTzipBusName, kTzipObjectPath,
                                     kTzipInterfaceName,
                                     kTzipMountMethod),
                                     DBusMessageDeleter);
    if (!msg) {
      LOG(ERROR) << "Could not create new dbus message";
      return false;
    }

    const char* mount_path_str = mount_path_.string().c_str();
    const char* zip_path_str = zip_path_.string().c_str();
    const char* tzip_smack_rule = kTzipSmackRule;

    if (!dbus_message_append_args(msg.get(),
            DBUS_TYPE_STRING, &mount_path_str,
            DBUS_TYPE_STRING, &zip_path_str,
            DBUS_TYPE_STRING, &tzip_smack_rule,
            DBUS_TYPE_INVALID)) {
      return false;
    }

    if (dbus_connection_send(conn, msg.get(), nullptr) == FALSE) {
      LOG(ERROR) << "Could not send DBUS message when mounting zip file";
      return false;
    }
    return WaitForMounted();
  }

  bool UnmountZip() {
    DBusConnection *conn = dbus_bus_get(DBUS_BUS_SYSTEM, NULL);
    if (!conn) {
      return false;
    }

    std::unique_ptr<DBusMessage, void(*)(DBusMessage*)> msg(
        dbus_message_new_method_call(kTzipBusName, kTzipObjectPath,
                                     kTzipInterfaceName,
                                     kTzipUnmountMethod),
                                     DBusMessageDeleter);
    if (!msg) {
      LOG(ERROR) << "Could not create new dbus message";
      return false;
    }

    const char* mount_path_str = mount_path_.string().c_str();
    if (!dbus_message_append_args(msg.get(),
            DBUS_TYPE_STRING, &mount_path_str,
            DBUS_TYPE_INVALID)) {
      return false;
    }

    if (dbus_connection_send(conn, msg.get(), nullptr) == FALSE) {
      LOG(ERROR) << "Could not send DBUS message when unmounting zip file";
      return false;
    }

    return true;
  }

 private:
  bool IsMounted() {
    int ret = -1;
    int r = -1;
    DBusMessage *reply;
    std::unique_ptr<DBusError, void(*)(DBusError*)> err(new DBusError(),
                                                        DBusErrorDeleter);

    DBusConnection* conn = dbus_bus_get(DBUS_BUS_SYSTEM, nullptr);
    if (!conn) {
      return false;
    }
    std::unique_ptr<DBusMessage, void(*)(DBusMessage*)> msg(
        dbus_message_new_method_call(kTzipBusName, kTzipObjectPath,
                                     kTzipInterfaceName,
                                     kTzipIsMountedMethod),
                                     DBusMessageDeleter);
    if (!msg) {
      return false;
    }

    const char* mount_path_str = mount_path_.string().c_str();
    if (!dbus_message_append_args(msg.get(),
            DBUS_TYPE_STRING, &mount_path_str,
            DBUS_TYPE_INVALID)) {
      return false;
    }

    dbus_error_init(err.get());
    reply = dbus_connection_send_with_reply_and_block(conn, msg.get(), 500,
                                                      err.get());
    if (!reply) {
      return false;
    }

    r = dbus_message_get_args(reply, err.get(), DBUS_TYPE_INT32, &ret,
                              DBUS_TYPE_INVALID);
    if (!r) {
      return false;
    }

    return ret != 0;
  }

  bool WaitForMounted() {
    if (!mount_path_.empty()) {
      bool rv = false;
      int cnt = 0;
      while (cnt < kTzipMountMaximumRetryCount) {
        rv = IsMounted();
        if (rv)
          break;
        sleep(1);
        cnt++;
      }

      if (!rv) {
        return false;
      }
    }
    return true;
  }

  boost::filesystem::path mount_path_;
  boost::filesystem::path zip_path_;
};


TzipInterface::TzipInterface(const boost::filesystem::path& mount_path)
  : impl_(new Pimpl(mount_path)) {}

TzipInterface::~TzipInterface() { }

bool TzipInterface::MountZip(const boost::filesystem::path& zip_path) {
  return impl_->MountZip(zip_path);
}

bool TzipInterface::UnmountZip() {
  return impl_->UnmountZip();
}

}  // namespace common_installer
