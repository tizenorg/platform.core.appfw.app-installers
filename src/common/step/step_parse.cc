/* 2014, Copyright © Eurogiciel Coporation, APACHE-2.0, see LICENSE file */

#include "common/step/step_parse.h"

#include <pkgmgr-info.h>
#include <unistd.h>

#include <boost/filesystem.hpp>
#include <cassert>
#include <cstring>
#include <cstdio>
#include <string>

#include "utils/file_util.h"

namespace common_installer {
namespace parse {

namespace fs = boost::filesystem;

Step::Status StepParse::process() {
  fs::path xml_path = fs::path(getUserManifestPath(context_->uid.get()))
      / fs::path(context_->pkgid.get());
  xml_path += ".xml";

  context_->xml_path.set(xml_path.string());
  xmlInitParser();
  manifest_x* mfx = pkgmgr_parser_usr_process_manifest_xml(
    context_->xml_path.get().c_str(), context_->uid.get());
  if (!mfx) {
    LOG(ERROR) << "Failed to parse tizen manifest xml "
        << context_->xml_path.get();
    return Step::Status::ERROR;
  }

  context_->manifest_data.set(mfx);
  context_->pkg_path.set(context_->application_path.get());

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
