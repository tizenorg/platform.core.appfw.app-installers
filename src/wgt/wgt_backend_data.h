// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef WGT_WGT_BACKEND_DATA_H_
#define WGT_WGT_BACKEND_DATA_H_

#include <manifest_handlers/setting_handler.h>

#include <string>
#include <vector>

#include "common/context_installer.h"
#include "common/utils/property.h"

namespace wgt {

class WgtBackendData : public common_installer::BackendData {
 public:
  WgtBackendData();

  Property<std::vector<std::string>> files_to_add;
  Property<std::vector<std::string>> files_to_modify;
  Property<std::vector<std::string>> files_to_delete;

  Property<parse::SettingInfo> settings;
};

}  // namespace wgt

#endif  // WGT_WGT_BACKEND_DATA_H_
