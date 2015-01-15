/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#ifndef COMMON_CONTEXT_INSTALLER_H_
#define COMMON_CONTEXT_INSTALLER_H_

#include <pkgmgr_parser.h>

#include <unistd.h>
#include <sys/types.h>

#include <memory>
#include <string>

namespace common_installer {

enum {
  APPINST_R_EINVAL = -2,    /**< Invalid argument */
  APPINST_R_ERROR = -1,   /**< General error */
  APPINST_R_OK = 0      /**< General success */
};

class ConfigData;
using ConfigDataPtr = std::unique_ptr<ConfigData>;

class ConfigData {
 public:
  ConfigData() {}

  std::string application_name() const { return application_name_; }
  void set_application_name(const std::string& app_name) {
    application_name_ = app_name;
  }
  std::string required_version() const { return required_version_; }
  void set_required_version(const std::string& required_version) {
    required_version_ = required_version;
  }

 private:
  std::string application_name_;
  std::string required_version_;
};

//TODO(p.sikorski@samsung.com) this class should be divided into:
//Base Context class
//CtxInstall class that inherits from Context
//CtxUninstall class that inherits from Context
//It is because Uninstallation does not need so many fields.
//similarly, installation may not need some of them
class ContextInstaller {
 public:
  ContextInstaller();
  ~ContextInstaller();

  int request_type() const { return req_; }
  void set_request_type(int req) {
   req_ = req;
  }

  manifest_x* manifest_data() const { return manifest_; }
  void set_manifest(manifest_x* manifest) {
    manifest_ = manifest;
  }

  std::string xml_path() const { return xml_path_; }
  void set_xml_path(const std::string& xml) {
    xml_path_ = xml;
  }

  std::string pkgid() const { return pkgid_; }
  void set_pkgid(const std::string& pkgid) {
   pkgid_ = pkgid;
  }

  std::string pkg_path() const { return pkg_path_; }
  void set_pkg_path(const std::string& package_path) {
   pkg_path_ = package_path;
  }

  std::string file_path() const { return file_path_; }
  void set_file_path(const std::string& file_path) {
   file_path_ = file_path;
  }

  uid_t uid() const { return uid_; }

  std::string unpack_directory() const { return unpack_directory_; }
  void set_unpack_directory(const std::string& unpack_dir) {
   unpack_directory_ = unpack_dir;
  }

  ConfigData* config_data() { return config_data_.get(); }

  const char* GetApplicationPath();
  const char* GetRootApplicationPath();

 private :
  // request type: Install, Reinstall, Uninstall, Update.
  int req_;

  //  manifest information used to generate xml file
  manifest_x* manifest_;

  // path to manifest xml file used to register data in databases
  std::string xml_path_;

  // pkgid used for update or uninstallation processing
  std::string pkgid_;

  // uid of the user that request the operation
  uid_t uid_;

  // package directory path containing app data
  std::string pkg_path_;

  // file path used for installation or reinstallation process
  std::string file_path_;

  // directory path where app data are temporarily extracted
  std::string unpack_directory_;

  // data from config.xml
  ConfigDataPtr config_data_;
};

}  // namespace common_installer

#endif  // COMMON_CONTEXT_INSTALLER_H_

