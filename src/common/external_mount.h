// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_EXTERNAL_MOUNT_H_
#define COMMON_EXTERNAL_MOUNT_H_

#include <unistd.h>

#include <string>

namespace common_installer {

/**
 * @brief The ExternalMount class
 *        Class used to mount package content for runtime.
 *
 * This class is used by delta update mode to get old content of package to
 * create the new content of package.
 */
class ExternalMount {
 public:
  ExternalMount(const std::string& pkgid, uid_t uid);
  ~ExternalMount();

  bool IsAvailable() const;
  bool Mount();
  bool Umount();

 private:
  std::string pkgid_;
  uid_t uid_;
  bool mounted_;
};

}  // namespace common_installer

#endif  // COMMON_EXTERNAL_MOUNT_H_
