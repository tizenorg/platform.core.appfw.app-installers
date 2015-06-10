// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef COMMON_APP_QUERY_INTERFACE_H_
#define COMMON_APP_QUERY_INTERFACE_H_

namespace common_installer {

class AppQueryInterface {
 public:
  virtual ~AppQueryInterface() { }
  virtual bool IsAppInstalledByArgv(int argc, char** argv) = 0;
};

}  // namespace common_installer

#endif  // COMMON_APP_QUERY_INTERFACE_H_
