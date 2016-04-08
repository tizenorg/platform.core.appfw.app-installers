// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_MOUNT_STEP_MOUNT_UNPACKED_H_
#define COMMON_STEP_MOUNT_STEP_MOUNT_UNPACKED_H_

#include <manifest_parser/utils/logging.h>

#include "common/installer_context.h"
#include "common/step/step.h"

namespace common_installer {
namespace mount {

/**
 * \brief Responsible for mounting package zip in temporary directory (wgt/tpk)
 *
 * Mounts package zip in temporary location to enable step to access package
 * content for parsing, signature check and others.
 *
 * Unzip mounts resources to following directory:
 * * TZ_SYS_RW/tmpuniquedir (/usr/apps/tmpuniquedir)
 * * TZ_SER_APPS/tmpdir  (/{HOME}/apps_rw/tmpuniquedir)
 */
class StepMountUnpacked : public Step {
 public:
  using Step::Step;

  Status process() override;
  Status clean() override { return Status::OK; }
  Status undo() override;
  Status precheck() override;

  SCOPE_LOG_TAG(MountUnpacked)
};

}  // namespace mount
}  // namespace common_installer

#endif  // COMMON_STEP_MOUNT_STEP_MOUNT_UNPACKED_H_
