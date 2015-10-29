// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#ifndef WGT_WGT_APP_QUERY_INTERFACE_H_
#define WGT_WGT_APP_QUERY_INTERFACE_H_

#include "common/app_query_interface.h"

namespace wgt {

/**
 * \brief Helper functionalities used before
 *        configuring app-installer steps.
 *        Eg. it is used to check, if package is to be installed or updated
 */
class WgtAppQueryInterface : public common_installer::AppQueryInterface {
 public:
  /**
   * \brief method for checking if package is installed based
   *        on argv
   *
   * \param argc main() argc argument passed to the backend
   * \param argv main() argv argument passed to the backend
   *
   * \return true if package is installed
   */
  bool IsAppInstalledByArgv(int argc, char** argv) override;
};

}  // namespace wgt

#endif  // WGT_WGT_APP_QUERY_INTERFACE_H_
