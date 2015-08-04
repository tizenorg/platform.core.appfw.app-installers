// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_recover_application.h"

#include <boost/filesystem.hpp>

#include "common/pkgmgr_registration.h"

namespace bf = boost::filesystem;

namespace common_installer {
namespace pkgmgr {

Step::Status StepRecoverApplication::RecoveryNew() {
  if (!SetXmlPaths())
    return Status::OK;
  UnregisterAppInPkgmgr(context_->xml_path.get(), context_->pkgid.get(),
                        context_->uid.get());
  return Status::OK;
}

Step::Status StepRecoverApplication::RecoveryUpdate() {
  if (!SetXmlPaths()) {
    LOG(ERROR) << "Some parameters are lacking";
    return Status::ERROR;
  }
  bf::path xml_path = bf::exists(context_->backup_xml_path.get()) ?
        context_->backup_xml_path.get() : context_->xml_path.get();
  UnregisterAppInPkgmgr(xml_path, context_->pkgid.get(),
                        context_->uid.get());
  if (!RegisterAppInPkgmgr(xml_path,
                           context_->pkgid.get().c_str(),
                           context_->certificate_info.get(),
                           context_->uid.get())) {
    LOG(ERROR) << "Unsuccessful app registration";
    return Status::ERROR;
  }
  return Status::OK;
}

bool StepRecoverApplication::SetXmlPaths() {
  if (context_->pkgid.get().empty())
    return false;
  bf::path xml_path = bf::path(getUserManifestPath(context_->uid.get()))
      / context_->pkgid.get();
  xml_path += ".xml";
  context_->xml_path.set(xml_path);
  xml_path += ".bck";
  context_->backup_xml_path.set(xml_path);
  return true;
}

}  // namespace pkgmgr
}  // namespace common_installer
