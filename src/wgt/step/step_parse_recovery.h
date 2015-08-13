// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef WGT_STEP_STEP_PARSE_RECOVERY_H_
#define WGT_STEP_STEP_PARSE_RECOVERY_H_

#include "common/utils/logging.h"
#include "wgt/step/step_parse.h"

namespace wgt {
namespace parse {
/**
 * @brief The StepParseRecovery class
 *        Retrievies package information from config.xml during RECOVERY.
 *
 * Step is used in recovery mode.
 *
 * Parse config.xml file by guessing possible locations:
 *  - backup package directory
 *  - package installation directory
 * to get information about widget package to be recovered.
 *
 */
class StepParseRecovery : public StepParse {
 public:
  explicit StepParseRecovery(common_installer::ContextInstaller* context);

  Status process() override;
  Status precheck() override;
  bool LocateConfigFile() override;

  SCOPE_LOG_TAG(ParseRecovery)
};

}  // namespace parse
}  // namespace wgt

#endif  // WGT_STEP_STEP_PARSE_RECOVERY_H_
