// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_MOUNT_STEP_MOUNT_UPDATE_H_
#define COMMON_STEP_MOUNT_STEP_MOUNT_UPDATE_H_

#include <manifest_parser/utils/logging.h>

#include "common/installer_context.h"
#include "common/step/step.h"

namespace common_installer {
namespace mount {

/**
 * \brief Responsible for mounting package zip in package installation directory
 *        in process of update installation request.
 *
 * Mounts package zip in installation location to enable step to access package
 * content for security applying and others. This step copies package zip into
 * $HOME/$PKGID/.image. This location will be used in runtime to mount package
 * when applcation from package is about being started.
 *
 * Resources to following directory:
 * * TZ_SYS_RW/$PKGID (/usr/apps/$PKGID)
 * * TZ_SER_APPS/$PKGID  (/{HOME}/apps_rw/$PKGID)
 */
class StepMountUpdate : public Step {
 public:
  using Step::Step;

  Status process() override;
  Status clean() override;
  Status undo() override;
  Status precheck() override;

  STEP_NAME(MountUpdate)
};

}  // namespace mount
}  // namespace common_installer

#endif  // COMMON_STEP_MOUNT_STEP_MOUNT_UPDATE_H_
