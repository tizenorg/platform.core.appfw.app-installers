// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a APACHE-2.0 license that can be
// found in the LICENSE file.

#include "wgt/wgt_installer.h"

#include <pkgmgr_installer.h>

#include <iostream>

#include "common/step/step_unzip.h"
#include "common/step/step_signature.h"
#include "common/step/step_copy.h"
#include "wgt/step/step_parse.h"

namespace ci = common_installer;

WgtInstaller::WgtInstaller(int request, const char* file, const char* pkgid)
    : ci::AppInstaller(request, file, pkgid) {
  if (request == PKGMGR_REQ_INSTALL) {
    AddStep(
        std::unique_ptr<ci::unzip::StepUnzip>(
            new ci::unzip::StepUnzip(context_.get())));

    AddStep(
        std::unique_ptr<ci::signature::StepSignature>(
            new ci::signature::StepSignature(context_.get())));

    AddStep(
      std::unique_ptr<wgt::parse::StepParse>(
            new wgt::parse::StepParse(context_.get())));

    AddStep(
        std::unique_ptr<ci::copy::StepCopy>(
            new ci::copy::StepCopy(context_.get())));
  } else {
    std::cerr << "Not supported request" << std::endl;
  }
}
