/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#include "include/step/step_record.h"

#include <pkgmgr-info.h>

#include <cstring>
#include <iostream>

#include "include/utils.h"

#define DBG(msg) std::cout << "[Record] " << msg << std::endl;
#define ERR(msg) std::cout << "[ERROR: Record] " << msg << std::endl;

namespace common_installer {
namespace record {

int StepRecord::process(ContextInstaller* data) {
  fs::path xml_path_ = fs::path(getUserManifestPath(data->uid()))
      / fs::path(data->pkgid());
  xml_path_.replace_extension(".xml");

  int ret;
  char* const appinst_tags[] = {"removable=true", NULL, };
  // TODO Check if data->removable is correctly setting during parsing step.
  // Same check should be done for preload field.

  // Having a specific step to implement a installer commandline tool
  // for image build could be usefull also.

  ret = data->uid() != tzplatform_getuid(TZ_SYS_GLOBALAPP_USER) ?
      pkgmgr_parser_parse_usr_manifest_for_installation(
          xml_path_.string().c_str(), data->uid(), appinst_tags) :
      pkgmgr_parser_parse_manifest_for_installation(
          xml_path_.string().c_str(), appinst_tags);

  if (ret != 0) {
    DBG("Failed to record package into database");
    return APPINST_R_ERROR;
  }
  DBG("Successfully install the application");
  return APPINST_R_OK;
}

int StepRecord::clean(ContextInstaller* data) {
  return APPINST_R_OK;
}

int StepRecord::undo(ContextInstaller* data) {
  if (fs::exists(xml_path_))
    fs::remove_all(xml_path_);

  int ret = data->uid() != tzplatform_getuid(TZ_SYS_GLOBALAPP_USER) ?
      system("/usr/bin/ail_initdb_user; /usr/bin/pkg_initdb_user") :
      system("/usr/bin/ail_initdb; /usr/bin/pkg_initdb");
  if (ret == -1)
      return APPINST_R_ERROR;

  DBG("Successfuly clean database");
  return APPINST_R_OK;
}

}  // namespace record
}  // namespace common_installer
