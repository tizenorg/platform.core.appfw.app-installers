// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_old_manifest.h"

#include <boost/filesystem.hpp>
#include <pkgmgr-info.h>
#include <string>

#include "common/utils/logging.h"

namespace common_installer {
namespace old_manifest {

namespace bf = boost::filesystem;

Step::Status StepOldManifest::process() {
  // TODO(t.iwanek): refactor -> this is set in StepGenerateXml too
  // but for update installation, it is too late
  // set xml file path
  bf::path xml_path = bf::path(getUserManifestPath(context_->uid.get()))
      / bf::path(context_->pkgid.get());
  xml_path += ".xml";
  context_->xml_path.set(xml_path);

  if (!bf::exists(context_->xml_path.get())) {
    LOG(ERROR) << "Missing old platform manifest file";
    return Status::ERROR;
  }

  xmlInitParser();
  manifest_x* mfx = pkgmgr_parser_usr_process_manifest_xml(
      context_->xml_path.get().c_str(), context_->uid.get());
  if (!mfx) {
    LOG(ERROR) << "Failed to parse old tizen manifest xml "
               << context_->xml_path.get();
    return Step::Status::ERROR;
  }

  context_->old_manifest_data.set(mfx);

  LOG(DEBUG) << "Successfully parse old tizen manifest xml for update";

  return Status::OK;
}

}  // namespace old_manifest
}  // namespace common_installer
