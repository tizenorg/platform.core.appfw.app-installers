// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/rds/step_rds_parse.h"

#include <manifest_parser/utils/logging.h>

#include <memory>

#include "common/installer_context.h"
#include "common/rds_parser.h"

namespace common_installer {
namespace rds {

namespace bf = boost::filesystem;

Step::Status StepRDSParse::precheck() {
  bf::path rdsPath(context_->unpacked_dir_path.get() / ".rds_delta");
  if (!bf::exists(rdsPath)) {
    LOG(ERROR) << "no rds_delta file";
    return Step::Status::INVALID_VALUE;
  }
  rds_file_path_ = rdsPath;
  return Step::Status::OK;
}

Step::Status StepRDSParse::process() {
  RDSParser parser(rds_file_path_.native());
  if (!parser.Parse()) {
    LOG(ERROR) << "parsing of rds delta failed";
    return Step::Status::PARSE_ERROR;
  }

  context_->files_to_modify.set(parser.files_to_modify());
  context_->files_to_add.set(parser.files_to_add());
  context_->files_to_delete.set(parser.files_to_delete());
  return Step::Status::OK;
}

}  // namespace rds
}  // namespace common_installer
