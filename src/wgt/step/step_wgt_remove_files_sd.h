// Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#ifndef WGT_STEP_STEP_WGT_REMOVE_FILES_SD_H_
#define WGT_STEP_STEP_WGT_REMOVE_FILES_SD_H_

#include <memory>
#include<string>

#include "common/step/step_remove_files.h"
#include "common/wgt_external_storage.h"

namespace wgt {
namespace filesystem {

class StepWgtRemoveFilesSD : public
    common_installer::filesystem::StepRemoveFiles {
 public:
  using StepRemoveFiles::StepRemoveFiles;

  Status precheck() override;
  Status process() override;
  Status clean() override;
  Status undo() override;

 private:
  enum class Result {
      INTERNAL,
      EXTERNAL,
      ERROR
  };
  Result CalculateLocation();

  bool initialized_;
  std::string pkgid_;
  std::unique_ptr<common_installer::WgtExternalStorage> storage_;
};

}  //  namespace filesystem
}  //  namespace wgt

#endif  //  WGT_STEP_STEP_WGT_REMOVE_FILES_SD_H_
