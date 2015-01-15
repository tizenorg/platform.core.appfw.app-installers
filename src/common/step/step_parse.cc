/* 2014, Copyright © Eurogiciel Coporation, APACHE-2.0, see LICENSE file */

#include "common/step/step_parse.h"

#include <pkgmgr-info.h>
#include <unistd.h>

#include <boost/filesystem.hpp>
#include <cassert>
#include <cstring>
#include <iostream>
#include <cstdio>
#include <string>
#include "common/utils.h"

#define DBG(msg) std::cout << "[Parse] " << msg << std::endl;
#define ERR(msg) std::cout << "[ERROR: Parse] " << msg << std::endl;

namespace common_installer {
namespace parse {

namespace fs = boost::filesystem;

int StepParse::process(ContextInstaller* data) {
  fs::path xml_path = fs::path(getUserManifestPath(data->uid()))
      / fs::path(data->pkgid());
  xml_path.replace_extension(".xml");

  data->set_xml_path(xml_path.string());
  xmlInitParser();
  manifest_x* mfx = pkgmgr_parser_usr_process_manifest_xml(
    data->xml_path().c_str(), data->uid());
  if (!mfx) {
    DBG("Failed to parse tizen manifest xml " << data->xml_path().c_str());
    return APPINST_R_ERROR;
  }

  data->set_manifest(mfx);
  data->set_pkg_path(data->GetApplicationPath());

  DBG("Successfully parse tizen manifest xml");

  return APPINST_R_OK;
}

int StepParse::clean(ContextInstaller* data) {
  return APPINST_R_OK;
}

int StepParse::undo(ContextInstaller* data) {
  return APPINST_R_OK;
}

}  // namespace parse
}  // namespace common_installer
