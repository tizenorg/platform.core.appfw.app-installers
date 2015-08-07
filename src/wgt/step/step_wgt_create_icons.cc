// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "wgt/step/step_wgt_create_icons.h"

namespace {

const char kResWgt[] = "res/wgt";

}

namespace wgt {
namespace filesystem {

boost::filesystem::path StepWgtCreateIcons::GetIconRoot() const {
  return context_->pkg_path.get() / kResWgt;
}

}  // namespace filesystem
}  // namespace wgt

