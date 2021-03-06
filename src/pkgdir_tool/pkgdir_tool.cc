// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include <glib.h>
#include <gio/gio.h>
#include <manifest_parser/utils/logging.h>

#include "common/shared_dirs.h"

#define UNUSED(expr) (void)(expr)

namespace ci = common_installer;

namespace {

const char kDBusInstropectionXml[] =
  "<node>"
  "  <interface name='org.tizen.pkgdir_tool'>"
  "    <method name='CopyUserDirs'>"
  "      <arg type='s' name='pkgid' direction='in'/>"
  "      <arg type='b' name='result' direction='out'/>"
  "    </method>"
  "    <method name='DeleteUserDirs'>"
  "      <arg type='s' name='pkgid' direction='in'/>"
  "      <arg type='b' name='result' direction='out'/>"
  "    </method>"
  "    <method name='CreateExternalDirs'>"
  "      <arg type='s' name='pkgid' direction='in'/>"
  "      <arg type='b' name='result' direction='out'/>"
  "    </method>"
  "    <method name='DeleteExternalDirs'>"
  "      <arg type='s' name='pkgid' direction='in'/>"
  "      <arg type='b' name='result' direction='out'/>"
  "    </method>"
  "    <method name='CreateLegacyDirs'>"
  "      <arg type='s' name='pkgid' direction='in'/>"
  "      <arg type='b' name='result' direction='out'/>"
  "    </method>"
  "    <method name='DeleteLegacyDirs'>"
  "      <arg type='s' name='pkgid' direction='in'/>"
  "      <arg type='b' name='result' direction='out'/>"
  "    </method>"
  "  </interface>"
  "</node>";
const char kDBusServiceName[] = "org.tizen.pkgdir_tool";
const char kDBusObjectPath[] = "/org/tizen/pkgdir_tool";

class PkgdirToolService {
 public:
  PkgdirToolService();
  ~PkgdirToolService();
  bool Init();
  void Run();

 private:
  void Finish();
  void RenewTimeout(int ms);
  void HandleMethodCall(GDBusConnection* connection,
      const gchar* sender, const gchar* object_path,
      const gchar* interface_name, const gchar* method_name,
      GVariant* parameters, GDBusMethodInvocation* invocation,
      gpointer user_data);
  void OnBusAcquired(GDBusConnection* connection, const gchar* name,
      gpointer user_data);
  int GetSenderUnixId(GDBusConnection *connection, const gchar* sender);

  GDBusNodeInfo* node_info_;
  guint owner_id_;
  GMainLoop* loop_;
  guint sid_;
};

PkgdirToolService::PkgdirToolService() :
      node_info_(nullptr), owner_id_(0), loop_(nullptr), sid_(0) {
}

PkgdirToolService::~PkgdirToolService() {
  Finish();
}

bool PkgdirToolService::Init() {
  node_info_ = g_dbus_node_info_new_for_xml(kDBusInstropectionXml, nullptr);
  if (!node_info_) {
    LOG(ERROR) << "Failed to create DBus node info";
    return false;
  }
  owner_id_ = g_bus_own_name(G_BUS_TYPE_SYSTEM, kDBusServiceName,
      G_BUS_NAME_OWNER_FLAGS_NONE,
      [](GDBusConnection* connection, const gchar* name,
          gpointer user_data) {
        reinterpret_cast<PkgdirToolService*>(user_data)->OnBusAcquired(
            connection, name, user_data);
      },
      nullptr, nullptr, this, nullptr);

  loop_ = g_main_loop_new(nullptr, FALSE);
  if (!loop_) {
    LOG(ERROR) << "Failed to create main loop";
    return false;
  }

  RenewTimeout(5000);

  return true;
}

void PkgdirToolService::Run() {
  g_main_loop_run(loop_);
}

void PkgdirToolService::Finish() {
  if (owner_id_ > 0)
    g_bus_unown_name(owner_id_);
  if (node_info_)
    g_dbus_node_info_unref(node_info_);
  if (loop_)
    g_main_loop_unref(loop_);
}

void PkgdirToolService::RenewTimeout(int ms) {
  if (sid_)
    g_source_remove(sid_);
  sid_ = g_timeout_add(ms,
      [](gpointer user_data) {
        g_main_loop_quit(
            reinterpret_cast<PkgdirToolService*>(user_data)->loop_);
        return FALSE;
      },
      this);
}

int PkgdirToolService::GetSenderUnixId(GDBusConnection* connection,
    const gchar* sender) {
  int uid = -1;

  GDBusMessage* msg = nullptr;
  msg = g_dbus_message_new_method_call("org.freedesktop.DBus",
                                       "/org/freedesktop/DBus",
                                       "org.freedesktop.DBus",
                                       "GetConnectionUnixUser");
  if (!msg) {
    LOG(ERROR) << "Failed to setup dbus message";
    return -1;
  }
  g_dbus_message_set_body(msg, g_variant_new("(s)", sender));

  GError* err = nullptr;
  GDBusMessage* reply = nullptr;
  reply = g_dbus_connection_send_message_with_reply_sync(connection, msg,
      G_DBUS_SEND_MESSAGE_FLAGS_NONE, -1, nullptr, nullptr, &err);
  if (!reply) {
    LOG(ERROR) << "Failed to send dbus message";
    if (err) {
      LOG(ERROR) << "error message: " <<  err->message;
      g_error_free(err);
    }
    g_object_unref(msg);
    return -1;
  }

  GVariant* body = g_dbus_message_get_body(reply);
  g_variant_get(body, "(u)", &uid);

  g_object_unref(msg);
  g_object_unref(reply);

  return uid;
}

void PkgdirToolService::HandleMethodCall(GDBusConnection* connection,
    const gchar* sender, const gchar* object_path, const gchar* interface_name,
    const gchar* method_name, GVariant* parameters,
    GDBusMethodInvocation* invocation, gpointer user_data) {
  UNUSED(object_path);
  UNUSED(interface_name);
  UNUSED(user_data);
  char* val;
  g_variant_get(parameters, "(s)", &val);

  bool r = false;
  LOG(INFO) << "Incomming method call: " << method_name;

  if (g_strcmp0(method_name, "CopyUserDirs") == 0) {
    r = ci::CopyUserDirectories(std::string(val));
  } else if (g_strcmp0(method_name, "DeleteUserDirs") == 0) {
    r = ci::DeleteUserDirectories(std::string(val));
  } else if (g_strcmp0(method_name, "CreateExternalDirs") == 0) {
    r = ci::PerformExternalDirectoryCreationForAllUsers(std::string(val));
  } else if (g_strcmp0(method_name, "DeleteExternalDirs") == 0) {
    r = ci::PerformExternalDirectoryDeletionForAllUsers(std::string(val));
  } else if (g_strcmp0(method_name, "CreateLegacyDirs") == 0) {
    r = ci::CreateLegacyDirectories(std::string(val));
  } else if (g_strcmp0(method_name, "DeleteLegacyDirs") == 0) {
    int sender_uid = GetSenderUnixId(connection, sender);
    if (sender_uid < 0) {
      LOG(ERROR) << "Failed to get sender_uid: " << sender_uid;
    } else {
      r = ci::DeleteLegacyDirectories((uid_t)sender_uid, std::string(val));
    }
  } else {
    LOG(ERROR) << "Unknown method call: " << method_name;
  }
  g_dbus_method_invocation_return_value(invocation, g_variant_new("(b)", r));

  RenewTimeout(5000);
}

void PkgdirToolService::OnBusAcquired(
    GDBusConnection* connection, const gchar* name, gpointer user_data) {
  UNUSED(name);
  UNUSED(user_data);
  GError* err = nullptr;
  GDBusInterfaceVTable vtable = {
    [](GDBusConnection* connection, const gchar* sender,
        const gchar* object_path, const gchar* interface_name,
        const gchar* method_name, GVariant* parameters,
        GDBusMethodInvocation* invocation, gpointer user_data) {
      reinterpret_cast<PkgdirToolService*>(user_data)->HandleMethodCall(
          connection, sender, object_path, interface_name, method_name,
          parameters, invocation, user_data);
    },
    nullptr, nullptr, {0, }
  };

  guint reg_id = g_dbus_connection_register_object(connection, kDBusObjectPath,
      node_info_->interfaces[0], &vtable, this, nullptr, &err);
  if (reg_id == 0) {
    LOG(ERROR) << "Register failed";
    if (err) {
      LOG(ERROR) << "Error message: " << err->message;
      g_error_free(err);
    }
  } else {
    LOG(INFO) << "DBus service registered";
  }
}

}  // namespace

int main() {
  PkgdirToolService service;
  if (!service.Init()) {
    LOG(ERROR) << "Failed to initialize service";
    return -1;
  }
  service.Run();
  return 0;
}
