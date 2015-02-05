/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#include "common/step/step_record.h"

#include <pkgmgr-info.h>
#include <unistd.h>

#include <boost/filesystem.hpp>
#include <cassert>
#include <cstring>
#include <cstdio>
#include <string>

#include "common/utils.h"

namespace {

const char kAilInit[] = "usr/bin/ail_initdb_user";
const char kAilInitUser[] = "usr/bin/ail_initdb";
const char kPkgInit[] = "/usr/bin/pkg_initdb";
const char kPkgInitUser[] = "/usr/bin/pkg_initdb_user";

}  // anonymous namespace

namespace common_installer {
namespace record {

namespace fs = boost::filesystem;

Step::Status StepRecord::process() {
  assert(!context_->xml_path().empty());

  char* const appinst_tags[] = {"removable=true", nullptr, };
  // TODO(sdi2): Check if data->removable is correctly setting
  // during parsing step.
  // Same check should be done for preload field.

  // Having a specific step to implement a installer commandline tool
  // for image build could be usefull also.

  int ret = context_->uid() != tzplatform_getuid(TZ_SYS_GLOBALAPP_USER) ?
      pkgmgr_parser_parse_usr_manifest_for_installation(
          context_->xml_path().c_str(), context_->uid(), appinst_tags) :
      pkgmgr_parser_parse_manifest_for_installation(
          context_->xml_path().c_str(), appinst_tags);

  if (ret != 0) {
    LOG(ERROR) << "Failed to record package into database";
    return Step::Status::ERROR;
  }
  LOG(INFO) << "Successfully install the application";
  return Status::OK;
}

Step::Status StepRecord::clean() {
  return Status::OK;
}

Step::Status StepRecord::undo() {
  std::string cmd;
  if (fs::exists(context_->xml_path()))
    fs::remove_all(context_->xml_path());

  context_->uid() != tzplatform_getuid(TZ_SYS_GLOBALAPP_USER) ?
      cmd = std::string(kAilInitUser) + ";" + kPkgInitUser :
      cmd = std::string(kAilInit) + ";" + kPkgInit;

  if (execv(cmd.c_str(), nullptr) == -1) {
      LOG(ERROR) << "Error during execvp %s";
      return Step::Status::ERROR;
  }

  LOG(INFO) << "Successfuly clean database";
  return Status::OK;
}

}  // namespace record
}  // namespace common_installer
