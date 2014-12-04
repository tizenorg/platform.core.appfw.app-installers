// Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.
// Part of this file is redistributed from crosswalk project under BDS-style
// license. Check LICENSE-xwalk file.

#ifndef COMMON_STEP_SIGNATURE_SIGNATURE_STEP_H_
#define COMMON_STEP_SIGNATURE_SIGNATURE_STEP_H_

#include "context_installer.h"
#include "step.hxx"

namespace common {

namespace signature {

class SignatureStep : public Step {
 public:
  using Step::Step;

  int process(Context_installer* context) override;
  int undo(Context_installer*) override { return 0; }
  int clean(Context_installer*) override { return 0; }
};

}  // namespace signature

}  // namespace common

#endif  // COMMON_STEP_SIGNATURE_SIGNATURE_STEP_H_
