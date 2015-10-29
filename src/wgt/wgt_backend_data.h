// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef WGT_WGT_BACKEND_DATA_H_
#define WGT_WGT_BACKEND_DATA_H_

#include <manifest_handlers/setting_handler.h>

#include <string>
#include <vector>

#include "common/installer_context.h"
#include "common/utils/property.h"

namespace wgt {

/**
 * \brief Class that is used within specific backends to keep additional
 *        information regarding package
 */
class WgtBackendData : public common_installer::BackendData {
 public:
  /**
   * \brief Property of vector of files to add
   */
  Property<std::vector<std::string>> files_to_add;

  /**
   * \brief Property of vector of files to modify
   */
  Property<std::vector<std::string>> files_to_modify;

  /**
   * \brief Property of vector of files to delete
   */
  Property<std::vector<std::string>> files_to_delete;

  /**
   * \brief Property of SettingInfo
   */
  Property<parse::SettingInfo> settings;
};

}  // namespace wgt

#endif  // WGT_WGT_BACKEND_DATA_H_
