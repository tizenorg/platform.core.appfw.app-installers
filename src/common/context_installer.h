/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#ifndef COMMON_CONTEXT_INSTALLER_H_
#define COMMON_CONTEXT_INSTALLER_H_

#include <pkgmgr_parser.h>

#include <unistd.h>
#include <sys/types.h>

#include <memory>
#include <string>

#include "common/pkgmgr_signal.h"

namespace common_installer {

// Base class for attributes within ContextInstaller
template<typename Type>
class Property {
 public:
    Property() {}  // TODO(p.sikorski@samsung.com) not sure, if it is needed
    Property(const Type &val): value_(val) { } // NOLINT
    const Type& operator()() const { return value_; }
    void operator()(const Type &val) { value_ = val; }
 private:
    Type value_;
};

class ConfigData;
using ConfigDataPtr = std::unique_ptr<ConfigData>;

class ConfigData {
 public:
  ConfigData() {}
  Property<std::string> application_name;
  Property<std::string> required_version;
};

// TODO(p.sikorski@samsung.com) this class should be divided into:
//  Base Context class
//  CtxInstall class that inherits from Context
//  CtxUninstall class that inherits from Context
//  It is because Uninstallation does not need so many fields.
//  similarly, installation may not need some of them
class ContextInstaller {
 public:
  ContextInstaller();
  ~ContextInstaller();

  // request type: Install, Reinstall, Uninstall, Update.
  Property<int> request_type;

  // package_type
  Property<std::string> pkg_type;

  //  manifest information used to generate xml file
  Property<manifest_x*> manifest_data;

  // path to manifest xml file used to register data in databases
  Property<std::string> xml_path;

  // pkgid used for update or uninstallation processing
  Property<std::string> pkgid;
  void set_new_temporary_pkgid(void);

  // package directory path containing app data
  Property<std::string> pkg_path;

  // file path used for installation or reinstallation process
  Property<std::string> file_path;

  // directory path where app data are temporarily extracted
  Property<std::string> unpacked_dir_path;

  // uid of the user that request the operation
  Property<uid_t> uid;


  // TODO(p.sikorski@samsung.com) change config_data to "read-only" Property?
  ConfigData* config_data() const { return config_data_.get(); }

  // TODO(p.sikorski@samsung.com) change "pi" to Property
  PkgmgrSignal* pi() const { return pi_.get(); }
  void set_pi(std::unique_ptr<PkgmgrSignal> pi) {
    pi_ = std::move(pi);
  }

  // TODO(p.sikorski@samsung.com) change to read-only property?
  const char* GetApplicationPath() const;
  const char* GetRootApplicationPath() const;

 private :
  // data from config.xml
  ConfigDataPtr config_data_;

  // data used to send signal
  std::unique_ptr<PkgmgrSignal> pi_;
};

}  // namespace common_installer

#endif  // COMMON_CONTEXT_INSTALLER_H_

