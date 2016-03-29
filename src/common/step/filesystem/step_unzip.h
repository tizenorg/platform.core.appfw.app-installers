/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_FILESYSTEM_STEP_UNZIP_H_
#define COMMON_STEP_FILESYSTEM_STEP_UNZIP_H_

#include <boost/filesystem/path.hpp>

#include <manifest_parser/utils/logging.h>

#include "common/installer_context.h"
#include "common/step/step.h"

namespace common_installer {
namespace filesystem {

/**
 * \brief Installation and Update.
 *        Responsible for unpacking the archive (wgt/tpk)
 *
 * process method implements unpacking the archive. It also checks the
 * rough space requirements vs availability.
 * Since process method unpacks the package to given directory, undo method
 * removes them.
 *
 * Unzip unpacks resources to following directory:
 * * TZ_SYS_RW/tmpuniquedir (/usr/apps/tmpuniquedir)
 * * TZ_SER_APPS/tmpdir  (/{HOME}/apps_rw/tmpuniquedir)
 * InstallerContext::unpacked_dir_path points to this location.
 */
class StepUnzip : public Step {
 public:
  using Step::Step;
  __attribute__ ((visibility ("default"))) Status process() override;
  __attribute__ ((visibility ("default"))) Status clean() override { return Status::OK; }
  __attribute__ ((visibility ("default"))) Status undo() override;
  __attribute__ ((visibility ("default"))) Status precheck() override;

  SCOPE_LOG_TAG(Unzip)
};

}  // namespace filesystem
}  // namespace common_installer


#endif  // COMMON_STEP_FILESYSTEM_STEP_UNZIP_H_
