// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "wgt/step/step_rds_parse.h"

#include <manifest_parser/utils/logging.h>

#include <memory>

#include "wgt/rds_parser.h"
#include "wgt/wgt_backend_data.h"


namespace wgt {
namespace rds {

namespace bf = boost::filesystem;

common_installer::Step::Status StepRDSParse::precheck() {
  bf::path rdsPath(context_->unpacked_dir_path.get() / ".rds_delta");
  if (!bf::exists(rdsPath)) {
    LOG(ERROR) << "no rds_delta file";
    return common_installer::Step::Status::ERROR;
  }
  rds_file_path_ = rdsPath;
  return common_installer::Step::Status::OK;
}

common_installer::Step::Status StepRDSParse::process() {
  wgt::rds_parser::RDSParser parser(rds_file_path_.native());
  if (!parser.Parse()) {
    LOG(ERROR) << "parsing of rds delta failed";
    return common_installer::Step::Status::ERROR;
  }

  WgtBackendData* backend_data =
      static_cast<WgtBackendData*>(context_->backend_data.get());
  if (!backend_data) {
    LOG(ERROR) << "no wgt backend data available";
    return common_installer::Step::Status::ERROR;
  }
  backend_data->files_to_modify.set(parser.files_to_modify());
  backend_data->files_to_add.set(parser.files_to_add());
  backend_data->files_to_delete.set(parser.files_to_delete());
  return common_installer::Step::Status::OK;
}

}  // namespace rds
}  // namespace wgt
