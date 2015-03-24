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

const char* const kAppinstTags[] = {"removable=true", nullptr, };

}  // anonymous namespace

namespace common_installer {
namespace register_app {

namespace fs = boost::filesystem;

Step::Status StepRegisterApplication::precheck() {
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

Step::Status StepRegisterApplication::process() {
  // TODO(sdi2): Check if data->removable is correctly setting
  // during parsing step.
  // Same check should be done for preload field.

  // Having a specific step to implement a installer commandline tool
  // for image build could be usefull also.

  int ret = context_->uid.get() != tzplatform_getuid(TZ_SYS_GLOBALAPP_USER) ?
      pkgmgr_parser_parse_usr_manifest_for_installation(
          context_->xml_path.get().c_str(), context_->uid.get(),
          const_cast<char* const*>(kAppinstTags)) :
      pkgmgr_parser_parse_manifest_for_installation(
          context_->xml_path.get().c_str(),
          const_cast<char* const*>(kAppinstTags));

  if (ret) {
    LOG(ERROR) << "Failed to record package into database";
    return Step::Status::ERROR;
  }
  in_registry_ = true;

  LOG(INFO) << "Successfully install the application";
  return Status::OK;
}

Step::Status StepRegisterApplication::clean() {
  return Status::OK;
}

Step::Status StepRegisterApplication::undo() {
  if (in_registry_) {
    int ret = context_->uid.get() != tzplatform_getuid(TZ_SYS_GLOBALAPP_USER) ?
        pkgmgr_parser_parse_usr_manifest_for_uninstallation(
            context_->xml_path.get().c_str(), context_->uid.get(),
            const_cast<char* const*>(kAppinstTags)) :
        pkgmgr_parser_parse_manifest_for_uninstallation(
            context_->xml_path.get().c_str(),
            const_cast<char* const*>(kAppinstTags));

    if (ret) {
      LOG(ERROR) << "Failed to restore old content pkgmgr database";
      return Step::Status::ERROR;
    }
    LOG(INFO) << "Successfuly clean database";
  }
  return Status::OK;
}

}  // namespace register_app
}  // namespace common_installer
