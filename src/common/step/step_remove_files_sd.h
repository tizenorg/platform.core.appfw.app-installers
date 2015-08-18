// Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_STEP_REMOVE_FILES_SD_H_
#define COMMON_STEP_STEP_REMOVE_FILES_SD_H_

#include <memory>
#include <string>

#include "common/step/step_remove_files.h"
#include "common/external_storage.h"

namespace common_installer {
namespace filesystem {

class StepRemoveFilesSD : public
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
  Result CheckLocation();

  bool installed_external_;
  std::string pkgid_;
  std::unique_ptr<ExternalStorage> storage_;
};

}  //  namespace filesystem
}  //  namespace common_installer

#endif  //  COMMON_STEP_STEP_REMOVE_FILES_SD_H_
