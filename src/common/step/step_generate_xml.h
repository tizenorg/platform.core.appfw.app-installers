/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef  COMMON_STEP_STEP_GENERATE_XML_H_
#define  COMMON_STEP_STEP_GENERATE_XML_H_


#include <boost/filesystem/path.hpp>
#include <libxml/xmlwriter.h>

#include "common/context_installer.h"
#include "common/step/step.h"
#include "common/utils/logging.h"

namespace common_installer {
namespace generate_xml {

class StepGenerateXml : public Step {
 public:
  using Step::Step;

  Status process() override;
  Status clean() override;
  Status undo() override;
  Status precheck() override;

 private:
  // This function is used to generate common xml data
  // for uiapplication_x and applicationservice_x, as these
  // structures contain common elements
  template <typename T>
  Step::Status GenerateApplicationCommonXml(T* app, xmlTextWriterPtr writer);

  boost::filesystem::path icon_path_;

  SCOPE_LOG_TAG(GenerateXML)
};

}  // namespace generate_xml
}  // namespace common_installer

#endif  // COMMON_STEP_STEP_GENERATE_XML_H_
