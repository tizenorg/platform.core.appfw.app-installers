// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include "common/tzip_interface.h"

#include <dbus/dbus.h>
#include <dbus/dbus-glib-lowlevel.h>

#include <sys/stat.h>
#include <unistd.h>
#include <cstdlib>

#define TZIP_BUS_NAME "org.tizen.system.deviced"
#define TZIP_OBJECT_PATH "/Org/Tizen/System/DeviceD/Tzip"
#define TZIP_INTERFACE_NAME "org.tizen.system.deviced.Tzip"
#define TZIP_MOUNT_METHOD "Mount"
#define TZIP_UNMOUNT_METHOD "Unmount"
#define TZIP_IS_MOUNTED_METHOD "IsMounted"
#define TZIP_MOUNT_MAXIMUM_RETRY_CNT 15

namespace common_installer {

class TzipInterface::Pimpl {
 public:
  explicit Pimpl(const std::string& mount_path) :
    mount_path_(mount_path) { }

  bool MountZip(const std::string& zip_path) {
    zip_path_ = zip_path;

    DBusMessage *msg;
    int func_ret = 0;
    int rv = 0;
    struct stat link_buf = {0, };

    DBusConnection *conn = dbus_bus_get(DBUS_BUS_SYSTEM, NULL);
    if (!conn) {
      return false;
    }

    rv = lstat(&mount_path_[0], &link_buf);
    if (rv == 0) {
      rv = unlink(&mount_path_[0]);
    }

    msg = dbus_message_new_method_call(TZIP_BUS_NAME, TZIP_OBJECT_PATH,
                                       TZIP_INTERFACE_NAME, TZIP_MOUNT_METHOD);
    if (!msg) {
      return false;
    }

    if (!dbus_message_append_args(msg,
            DBUS_TYPE_STRING, &mount_path_[0],
            DBUS_TYPE_STRING, &zip_path_[0],
            DBUS_TYPE_INVALID)) {
      func_ret = -1;
      goto func_out;
    }

    if (dbus_connection_send(conn, msg, NULL) == FALSE) {
      func_ret = -1;
      goto func_out;
    }

  func_out :
    dbus_message_unref(msg);
    return func_ret == 0 && WaitForMounted();
  }

  bool UnMountZip() {
    DBusConnection *conn = dbus_bus_get(DBUS_BUS_SYSTEM, NULL);
    if (!conn) {
      return false;
    }

    DBusMessage *msg = NULL;
    msg = dbus_message_new_method_call(TZIP_BUS_NAME, TZIP_OBJECT_PATH,
                                       TZIP_INTERFACE_NAME,
                                       TZIP_UNMOUNT_METHOD);
    if (!msg) {
      return false;
    }

    if (!dbus_message_append_args(msg,
            DBUS_TYPE_STRING, &mount_path_[0],
            DBUS_TYPE_INVALID)) {
      dbus_message_unref(msg);
      return false;
    }

    if (dbus_connection_send(conn, msg, NULL) == FALSE) {
      dbus_message_unref(msg);
      return false;
    }
    dbus_message_unref(msg);
    return true;
  }

 private:
  bool IsMounted() {
    DBusMessage *msg;
    DBusMessage *reply;
    DBusError err;
    int ret = -1;
    int r = -1;

    DBusConnection* conn = dbus_bus_get(DBUS_BUS_SYSTEM, NULL);
    if (!conn) {
      return false;
    }

    msg = dbus_message_new_method_call(TZIP_BUS_NAME, TZIP_OBJECT_PATH,
                                       TZIP_INTERFACE_NAME,
                                       TZIP_IS_MOUNTED_METHOD);
    if (!msg) {
      return false;
    }

    if (!dbus_message_append_args(msg,
            DBUS_TYPE_STRING, &mount_path_[0],
            DBUS_TYPE_INVALID)) {
      dbus_message_unref(msg);
      return false;
    }

    dbus_error_init(&err);
    reply = dbus_connection_send_with_reply_and_block(conn, msg, 500, &err);
    if (!reply) {
      goto func_out;
    }

    r = dbus_message_get_args(reply, &err, DBUS_TYPE_INT32, &ret,
                              DBUS_TYPE_INVALID);
    if (!r) {
      goto func_out;
    }

  func_out :
    dbus_message_unref(msg);
    dbus_error_free(&err);
    return ret == 0;
  }

  bool WaitForMounted() {
    if (!mount_path_.empty()) {
      bool rv = false;
      int cnt = 0;
      while (cnt < TZIP_MOUNT_MAXIMUM_RETRY_CNT) {
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

  std::string mount_path_;
  std::string zip_path_;
};


TzipInterface::TzipInterface(const std::string& mount_path)
  : impl_(new Pimpl(mount_path)) {}

TzipInterface::~TzipInterface() { }

bool TzipInterface::MountZip(const std::string& zip_path) {
  return impl_->MountZip(zip_path);
}

bool TzipInterface::UnMountZip() {
  return impl_->UnMountZip();
}

}  // namespace common_installer
