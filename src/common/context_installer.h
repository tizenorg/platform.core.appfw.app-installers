/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_CONTEXT_INSTALLER_H_
#define COMMON_CONTEXT_INSTALLER_H_

#include <boost/filesystem/path.hpp>

#include <pkgmgr_parser.h>

#include <unistd.h>
#include <sys/types.h>

#include <memory>
#include <string>

namespace common_installer {

/** Template class for defining smart attributes.
 *
 *  Property should be used when, given attribute needs to have pure
 *  setter and getter. This template class will generate getter and setter.
 *  It uses operator() overloading.
 */
template<typename Type>
class Property {
 public:
  Property() {}
  Property(const Type &val): value_(val) { } // NOLINT
  const Type& get() const { return value_; }
  Type& get() { return value_; }
  void set(const Type &val) { value_ = val; }
 private:
  Type value_;
};

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

  // package_type
  Property<std::string> pkg_type;

  //  manifest information used to generate xml file
  Property<manifest_x*> manifest_data;

  //  manifest information used to revert an update
  Property<manifest_x*> old_manifest_data;

  // path to manifest xml file used to register data in databases
  Property<boost::filesystem::path> xml_path;

  // pkgid used for update or uninstallation processing
  Property<std::string> pkgid;

  // package directory path containing app data
  Property<boost::filesystem::path> pkg_path;

  // file path used for installation or reinstallation process
  Property<boost::filesystem::path> file_path;

  // directory path where app data are temporarily extracted
  Property<boost::filesystem::path> unpacked_dir_path;

  // uid of the user that request the operation
  Property<uid_t> uid;

  // data from config.xml
  Property<ConfigData> config_data;

  // path for the applications directory
  Property<boost::filesystem::path> application_path;

  // path for the applications root directory
  Property<boost::filesystem::path> root_application_path;
};

}  // namespace common_installer

#endif  // COMMON_CONTEXT_INSTALLER_H_

