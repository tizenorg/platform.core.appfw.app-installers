/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#include "include/context_installer.h"

#include <tzplatform_config.h>
#include <unistd.h>

#include <iostream>

namespace utils {

bool CreateDir(const boost::filesystem::path& path) {
  if (boost::filesystem::exists(path))
    return true;

  boost::system::error_code error;
  boost::filesystem::create_directories(path, error);
  if (error) {
    std::cerr << "Failed to create directory: "
        << boost::system::system_error(error).what() << std::endl;
    return false;
  }

  boost::filesystem::permissions(path, boost::filesystem::owner_all
      | boost::filesystem::group_read | boost::filesystem::others_read,
      error);
  if (error) {
    std::cerr << "Failed to set permission: "
        << boost::system::system_error(error).what() << std::endl;
    return false;
  }
  return true;
}

}  // namespace utils

const char* ContextInstaller::GetApplicationPath() {
  return getuid() != tzplatform_getuid(TZ_SYS_GLOBALAPP_USER)
      ? tzplatform_getenv(TZ_USER_APP) : tzplatform_getenv(TZ_SYS_RW_APP);
}
