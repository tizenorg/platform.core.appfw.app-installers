// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#ifndef WGT_WGT_APP_QUERY_INTERFACE_H_
#define WGT_WGT_APP_QUERY_INTERFACE_H_

#include "common/app_query_interface.h"

namespace wgt {

class WgtAppQueryInterface : public common_installer::AppQueryInterface {
 public:
  bool IsAppInstalledByArgv(int argc, char** argv) override;
};

}  // namespace wgt

#endif  // WGT_WGT_APP_QUERY_INTERFACE_H_
