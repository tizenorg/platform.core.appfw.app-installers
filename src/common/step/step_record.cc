/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#include "common/step/step_record.h"

#include <pkgmgr-info.h>
#include <unistd.h>

#include <boost/filesystem.hpp>
#include <cassert>
#include <cstring>
#include <iostream>
#include <cstdio>
#include <string>
#include "common/utils.h"

#define DBG(msg) std::cout << "[Record] " << msg << std::endl;
#define ERR(msg) std::cout << "[ERROR: Record] " << msg << std::endl;

namespace {

const char kAilInit[] = "usr/bin/ail_initdb_user";
const char kAilInitUser[] = "usr/bin/ail_initdb";
const char kPkgInit[] = "/usr/bin/pkg_initdb";
const char kPkgInitUser[] = "/usr/bin/pkg_initdb_user";

}  // anonymous namespace

namespace common_installer {
namespace record {

namespace fs = boost::filesystem;

int StepRecord::process(ContextInstaller* data) {
  assert(!data->xml_path().empty());

  char* const appinst_tags[] = {"removable=true", nullptr, };
  // TODO(sdi2): Check if data->removable is correctly setting
  // during parsing step.
  // Same check should be done for preload field.

  // Having a specific step to implement a installer commandline tool
  // for image build could be usefull also.

  int ret = data->uid() != tzplatform_getuid(TZ_SYS_GLOBALAPP_USER) ?
      pkgmgr_parser_parse_usr_manifest_for_installation(
          data->xml_path().c_str(), data->uid(), appinst_tags) :
      pkgmgr_parser_parse_manifest_for_installation(
          data->xml_path().c_str(), appinst_tags);

  if (ret != 0) {
    ERR("Failed to record package into database");
    return APPINST_R_ERROR;
  }
  DBG("Successfully install the application");
  return APPINST_R_OK;
}

int StepRecord::clean(ContextInstaller* data) {
  return APPINST_R_OK;
}

int StepRecord::undo(ContextInstaller* data) {
  std::string cmd;
  if (fs::exists(data->xml_path()))
    fs::remove_all(data->xml_path());

  data->uid() != tzplatform_getuid(TZ_SYS_GLOBALAPP_USER) ?
      cmd = std::string(kAilInitUser) + ";" + kPkgInitUser :
      cmd = std::string(kAilInit) + ";" + kPkgInit;

  if (execv(cmd.c_str(), nullptr) == -1) {
      ERR("Error during execvp %s");
      return APPINST_R_ERROR;
  }
  DBG("Successfuly clean database");
  return APPINST_R_OK;
}

}  // namespace record
}  // namespace common_installer
