// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <tzplatform_config.h>

#include <regex>
#include <string>

#include "common/utils/subprocess.h"

namespace bf = boost::filesystem;
namespace ci = common_installer;

namespace {

const char kRecoveryFilePattern[] = "^recovery-*";

bool DoRecoveryProcess(uid_t uid) {
  bf::path apps_dir;
  if (uid == tzplatform_getuid(TZ_SYS_GLOBALAPP_USER)) {
    apps_dir = tzplatform_getenv(TZ_SYS_RW_APP);
  } else {
    tzplatform_set_user(uid);
    apps_dir = tzplatform_getenv(TZ_USER_APP);
    tzplatform_reset_user();
  }

  for (bf::directory_iterator iter(apps_dir); iter != bf::directory_iterator();
      ++iter) {
    if (bf::is_directory(iter->path()))
      continue;
    std::string file = iter->path().filename().string();
    std::regex recovery_regex(kRecoveryFilePattern);
    if (std::regex_search(file, recovery_regex)) {
      // How to get package type?
      ci::Subprocess backend("/usr/bin/tpk-backend");
      backend.Run("-b", file);
      int status = backend.Wait();
    }
  }

  return true;
}

}  // namespace

int main() {
  DoRecoveryProcess(getuid());
  return 0;
}
