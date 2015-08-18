// Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#ifndef WGT_STEP_STEP_WGT_REMOVE_FILES_SD_H_
#define WGT_STEP_STEP_WGT_REMOVE_FILES_SD_H_

#include <app2ext_interface.h>

#include <string>

#include "common/step/step_remove_files.h"
#include "common/wgt_external_storage.h"
#include "wgt/wgt_backend_data.h"

namespace wgt {
namespace filesystem {

namespace ci = common_installer;

class StepWgtRemoveFilesSD : public ci::filesystem::StepRemoveFiles {
 public:
  enum class Result {
      INTERNAL,
      EXTERNAL,
      ERROR
  };

  using StepRemoveFiles::StepRemoveFiles;

  Status precheck() override;
  Status process() override;
  Status clean() override;
  Status undo() override;

 private:
  Result Location();

  const char* pkgid_;
  app2ext_handle* app2_handle_;
};

}  //  namespace filesystem
}  //  namespace wgt

#endif  //  WGT_STEP_STEP_WGT_REMOVE_FILES_SD_H_
