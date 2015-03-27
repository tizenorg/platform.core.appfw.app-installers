/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#include "common/step/step_register_app.h"

#include <pkgmgr-info.h>
#include <unistd.h>

#include <boost/filesystem.hpp>
#include <cassert>
#include <cstring>
#include <cstdio>
#include <string>

#include "utils/file_util.h"

namespace {

const char kAilInit[] = "usr/bin/ail_initdb_user";
const char kAilInitUser[] = "usr/bin/ail_initdb";
const char kPkgInit[] = "/usr/bin/pkg_initdb";
const char kPkgInitUser[] = "/usr/bin/pkg_initdb_user";

}  // anonymous namespace

namespace common_installer {
namespace register_app {

namespace fs = boost::filesystem;

Step::Status StepRegisterApplication::process() {
  assert(!context_->xml_path().empty());

  const char* const appinst_tags[] = {"removable=true", nullptr, };
  // TODO(sdi2): Check if data->removable is correctly setting
  // during parsing step.
  // Same check should be done for preload field.

  // Having a specific step to implement a installer commandline tool
  // for image build could be usefull also.

  int ret = context_->uid() != tzplatform_getuid(TZ_SYS_GLOBALAPP_USER) ?
      pkgmgr_parser_parse_usr_manifest_for_installation(
          context_->xml_path().c_str(), context_->uid(),
          const_cast<char* const*>(appinst_tags)) :
      pkgmgr_parser_parse_manifest_for_installation(
          context_->xml_path().c_str(), const_cast<char* const*>(appinst_tags));

  if (ret != 0) {
    LOG(ERROR) << "Failed to record package into database";
    return Step::Status::ERROR;
  }
  LOG(INFO) << "Successfully install the application";
  return Status::OK;
}

Step::Status StepRegisterApplication::clean() {
  return Status::OK;
}

Step::Status StepRegisterApplication::undo() {
  if (context_->uid() != tzplatform_getuid(TZ_SYS_GLOBALAPP_USER)) {
    const char* ail_cmd[] = {kAilInitUser, nullptr};
    const char* pkgmgr_cmd[] = {kPkgInitUser, nullptr};

    if (execv(ail_cmd[0], const_cast<char* const*>(ail_cmd)) == -1) {
      LOG(ERROR) << "Error during execv: " << ail_cmd[0];
      return Step::Status::ERROR;
    }
    if (execv(pkgmgr_cmd[0], const_cast<char* const*>(pkgmgr_cmd)) == -1) {
      LOG(ERROR) << "Error during execv: " << pkgmgr_cmd[0];
      return Step::Status::ERROR;
    }
  } else {
    const char* ail_cmd[] = {kAilInit, nullptr};
    const char* pkgmgr_cmd[] = {kPkgInit, nullptr};

    if (execv(ail_cmd[0], const_cast<char* const*>(ail_cmd)) == -1) {
      LOG(ERROR) << "Error during execv: " << ail_cmd[0];
      return Step::Status::ERROR;
    }
    if (execv(pkgmgr_cmd[0], const_cast<char* const*>(pkgmgr_cmd)) == -1) {
      LOG(ERROR) << "Error during execv: " << pkgmgr_cmd[0];
      return Step::Status::ERROR;
    }
  }

  LOG(INFO) << "Successfuly clean database";
  return Status::OK;
}

}  // namespace register_app
}  // namespace common_installer
