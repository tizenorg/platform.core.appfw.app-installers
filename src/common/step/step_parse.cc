/* 2014, Copyright © Eurogiciel Coporation, APACHE-2.0, see LICENSE file */
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_parse.h"

#include <pkgmgr-info.h>
#include <unistd.h>

#include <boost/filesystem.hpp>
#include <cassert>
#include <cstring>
#include <cstdio>
#include <string>

#include "common/utils/file_util.h"

namespace common_installer {
namespace parse {

namespace bf = boost::filesystem;

Step::Status StepParse::process() {
  bf::path xml_path = bf::path(getUserManifestPath(context_->uid.get()))
      / bf::path(context_->pkgid.get());
  xml_path += ".xml";

  context_->xml_path.set(xml_path.string());
  xmlInitParser();
  manifest_x* mfx = pkgmgr_parser_usr_process_manifest_xml(
    context_->xml_path.get().c_str(), context_->uid.get());
  if (!mfx) {
    LOG(ERROR) << "Failed to parse tizen manifest xml "
        << context_->xml_path.get();
    return Step::Status::PARSE_ERROR;
  }

  context_->manifest_data.set(mfx);

  context_->pkg_path.set(
      context_->root_application_path.get() / context_->pkgid.get());

  LOG(DEBUG) << "Successfully parse tizen manifest xml";

  return Status::OK;
}

}  // namespace parse
}  // namespace common_installer
