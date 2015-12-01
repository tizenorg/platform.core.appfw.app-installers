// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef TPK_STEP_STEP_PARSE_RECOVERY_H_
#define TPK_STEP_STEP_PARSE_RECOVERY_H_

#include <boost/filesystem/path.hpp>

#include <manifest_parser/utils/logging.h>

#include "tpk/step/step_parse.h"

namespace tpk {
namespace parse {

class StepParseRecovery : public StepParse {
 public:
  using StepParse::StepParse;

  Status process() override;
  Status precheck() override;

 protected:
  boost::filesystem::path LocateConfigFile() const override;

  SCOPE_LOG_TAG(ParseRecovery)
};
}  // namespace parse
}  // namespace tpk

#endif  // TPK_STEP_STEP_PARSE_RECOVERY_H_
