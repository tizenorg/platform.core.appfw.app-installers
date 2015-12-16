// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef TPK_STEP_STEP_CONVERT_XML_H_
#define TPK_STEP_STEP_CONVERT_XML_H_

#include <manifest_parser/utils/logging.h>

#include <boost/filesystem/path.hpp>
#include <libxml/tree.h>

#include "common/step/step.h"

namespace common_installer {
namespace tpk {

class StepConvertXml : public common_installer::Step {
 public:
  using Step::Step;

  Status process() override;
  Status clean() override;
  Status undo() override;
  Status precheck() override;

 private:
  boost::filesystem::path xml_path_;
  bool ConvertXml(xmlDocPtr doc);

  SCOPE_LOG_TAG(ConvertXml)
};

}  // namespace tpk
}  // namespace common_installer

#endif  // TPK_STEP_STEP_CONVERT_XML_H_
