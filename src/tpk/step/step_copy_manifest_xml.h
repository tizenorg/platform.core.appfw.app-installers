#ifndef TPK_STEP_STEP_COPY_MANIFEST_XML_H_
#define TPK_STEP_STEP_COPY_MANIFEST_XML_H_

/* Copyright 2015 Samsung Electronics, license APACHE-2.0, see LICENSE file */
// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.
#include "common/step/step.h"
#include <boost/filesystem.hpp>

#include <memory>

namespace tpk {
namespace step {

/** Copy tizen-manifest.xml into the application directory
 *
 */
class StepCopyManifestXml : public common_installer::Step {
 public:
  using Step::Step;
  Status precheck() override { return Status::OK; }
  Status process() override;
  Status clean() override;
  Status undo() override;

 private:
  std::shared_ptr<boost::filesystem::path> dest_xml_path_;
};

}  // namespace step
}  // namespace tpk

#endif  // TPK_STEP_STEP_COPY_MANIFEST_XML_H_
