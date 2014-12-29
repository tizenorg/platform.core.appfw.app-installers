/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#include "include/context_installer.h"

#include <tzplatform_config.h>
#include <unistd.h>

#include <iostream>

#define ERR(msg) std::cout << "[ERROR] " << msg << std::endl;

namespace utils {

bool CreateDir(const boost::filesystem::path& path) {
  if (boost::filesystem::exists(path))
    return true;

  boost::system::error_code error;
  boost::filesystem::create_directories(path, error);
  if (error) {
    ERR("Failed to create directory: "
        << boost::system::system_error(error).what());
    return false;
  }

  boost::filesystem::permissions(path, boost::filesystem::owner_all
      | boost::filesystem::group_read | boost::filesystem::others_read,
      error);
  if (error) {
    ERR("Failed to set permission: "
        << boost::system::system_error(error).what());
    return false;
  }
  return true;
}

bool CopyDir(const boost::filesystem::path& src,
    const boost::filesystem::path& dst) {
  namespace fs = boost::filesystem;
  try {
    // Check whether the function call is valid
    if (!fs::exists(src) || !fs::is_directory(src)) {
      ERR("Source directory " << src.string()
          << " does not exist or is not a directory.");
      return false;
    }
    if (fs::exists(dst)) {
      ERR("Destination directory " << dst.string() << " already exists.");
      return false;
    }
    // Create the destination directory
    if (!fs::create_directory(dst)) {
      ERR("Unable to create destination directory" << dst.string());
      return false;
    }
  } catch (const fs::filesystem_error& error) {
      ERR(error.what());
  }

  // Iterate through the source directory
  for (fs::directory_iterator file(src);
      file != fs::directory_iterator();
      ++file) {
    try {
      fs::path current(file->path());
      if (fs::is_directory(current)) {
        // Found directory: Recursion
        if (!CopyDir(current, dst / current.filename())) {
          return false;
        }
      } else {
        // Found file: Copy
        fs::copy_file(current, dst / current.filename());
      }
    } catch (const fs::filesystem_error& error) {
        ERR(error.what());
    }
  }
  return true;
}

}  // namespace utils

const char* ContextInstaller::GetApplicationPath() {
  return getuid() != tzplatform_getuid(TZ_SYS_GLOBALAPP_USER)
      ? tzplatform_getenv(TZ_USER_APP) : tzplatform_getenv(TZ_SYS_RW_APP);
}
