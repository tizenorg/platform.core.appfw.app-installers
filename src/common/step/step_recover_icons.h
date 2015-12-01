// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_STEP_RECOVER_ICONS_H_
#define COMMON_STEP_STEP_RECOVER_ICONS_H_

#include <boost/filesystem/path.hpp>

#include <manifest_parser/utils/logging.h>

#include <utility>
#include <vector>

#include "common/installer_context.h"
#include "common/step/step_recovery.h"

namespace common_installer {
namespace filesystem {

/**
 * @brief The StepRecoverIcons class
 *        Fixes state of platform icon files in recovery mode.
 *
 * For recovery of new installation, all icons files are removed.
 * For recovery of update installation, all icons of applications of package are
 * restored to its previous locations.
 */
class StepRecoverIcons : public recovery::StepRecovery {
 public:
  using StepRecovery::StepRecovery;

  Status RecoveryNew() override;
  Status RecoveryUpdate() override;

 private:
  bool TryGatherIcons();

  std::vector<std::pair<boost::filesystem::path, boost::filesystem::path>>
      icons_;

  SCOPE_LOG_TAG(RecoverIcons)
};

}  // namespace filesystem
}  // namespace common_installer

#endif  // COMMON_STEP_STEP_RECOVER_ICONS_H_
