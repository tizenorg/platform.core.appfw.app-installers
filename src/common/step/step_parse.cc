/* 2014, Copyright © Eurogiciel Coporation, APACHE-2.0, see LICENSE file */

#include "common/step/step_parse.h"

#include <pkgmgr-info.h>
#include <unistd.h>

#include <boost/filesystem.hpp>
#include <cassert>
#include <cstring>
#include <cstdio>
#include <string>

#include "common/utils.h"

namespace common_installer {
namespace parse {

namespace fs = boost::filesystem;

Step::Status StepParse::process() {
  fs::path xml_path = fs::path(getUserManifestPath(context_->uid()))
      / fs::path(context_->pkgid());
  xml_path += ".xml";

  context_->set_xml_path(xml_path.string());
  xmlInitParser();
  manifest_x* mfx = pkgmgr_parser_usr_process_manifest_xml(
    context_->xml_path().c_str(), context_->uid());
  if (!mfx) {
    LOG(ERROR) << "Failed to parse tizen manifest xml " << context_->xml_path();
    return Step::Status::ERROR;
  }

  context_->set_manifest(mfx);
  context_->set_pkg_path(context_->GetApplicationPath());

  LOG(DEBUG) << "Successfully parse tizen manifest xml";

  return Status::OK;
}

Step::Status StepParse::clean() {
  return Status::OK;
}

Step::Status StepParse::undo() {
  return Status::OK;
}

}  // namespace parse
}  // namespace common_installer
