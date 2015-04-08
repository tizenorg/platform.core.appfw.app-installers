// Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include <unistd.h>

#include <boost/filesystem.hpp>
#include <cassert>
#include <cstring>

#include "common/step/step_unregister_app.h"
#include "utils/file_util.h"

namespace common_installer {
namespace unregister_app {

namespace fs = boost::filesystem;

Step::Status StepUnregisterApplication::precheck() {
  if (context_->pkgid.get().empty()) {
    LOG(ERROR) << "pkgid attribute is empty";
    return Step::Status::INVALID_VALUE;
  }
  if (context_->xml_path.get().empty()) {
    LOG(ERROR) << "xml_path attribute is empty";
    return Step::Status::INVALID_VALUE;
  }
  if (!boost::filesystem::exists(context_->xml_path.get())) {
    LOG(ERROR) << "xml_path ("
               << context_->xml_path.get()
               << ") path does not exist";
    return Step::Status::INVALID_VALUE;
  }

  // TODO(p.sikorski) check context_->uid.get()

  return Step::Status::OK;
}

Step::Status StepUnregisterApplication::process() {
  const char* const appinst_tags[] = {"removable=true", nullptr, };

  int ret = context_->uid.get() != tzplatform_getuid(TZ_SYS_GLOBALAPP_USER) ?
      pkgmgr_parser_parse_usr_manifest_for_uninstallation(
          context_->xml_path.get().c_str(), context_->uid.get(),
          const_cast<char* const*>(appinst_tags)) :
      pkgmgr_parser_parse_manifest_for_uninstallation(
          context_->xml_path.get().c_str(),
          const_cast<char* const*>(appinst_tags));

  if (ret != 0) {
    LOG(ERROR) << "Failed to unregister package into database";
    return Status::ERROR;
  }
  LOG(DEBUG) << "Successfully unregister the application";

  return Status::OK;
}

}  // namespace unregister_app
}  // namespace common_installer
