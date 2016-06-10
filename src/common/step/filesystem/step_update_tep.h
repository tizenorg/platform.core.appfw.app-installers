// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_FILESYSTEM_STEP_UPDATE_TEP_H_
#define COMMON_STEP_FILESYSTEM_STEP_UPDATE_TEP_H_

#include <boost/filesystem/path.hpp>
#include <manifest_parser/utils/logging.h>

#include "common/installer_context.h"
#include "common/step/filesystem/step_copy_tep.h"


namespace common_installer {
namespace filesystem {

/**
 * \brief step responsible for moving/copying TEP files from designated path to
 *        final installed package destination during UPDATE.
 *        Used by WGT and TPK
 */
class StepUpdateTep : public StepCopyTep {
 public:
  using StepCopyTep::StepCopyTep;

  Status precheck() override;
  Status process() override;
  Status clean() override;
  Status undo() override;

  STEP_NAME(UpdateTep)
};

}  // namespace filesystem
}  // namespace common_installer

#endif  // COMMON_STEP_FILESYSTEM_STEP_UPDATE_TEP_H_
