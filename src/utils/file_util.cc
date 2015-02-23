/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#include "utils/file_util.h"

#include <boost/filesystem/path.hpp>
#include <string>

#include "utils/logging.h"

namespace common_installer {
namespace utils {

namespace fs = boost::filesystem;

bool CreateDir(const fs::path& path) {
  if (fs::exists(path))
    return true;

  boost::system::error_code error;
  fs::create_directories(path, error);
  if (error) {
    LOG(ERROR) << "Failed to create directory: "
               << boost::system::system_error(error).what();
    return false;
  }

  fs::permissions(path, fs::owner_all
      | fs::group_read | fs::others_read,
      error);
  if (error) {
    LOG(ERROR) << "Failed to set permission: "
               << boost::system::system_error(error).what();
    return false;
  }
  return true;
}

bool CopyDir(const fs::path& src, const fs::path& dst) {
  try {
    // Check whether the function call is valid
    if (!fs::exists(src) || !fs::is_directory(src)) {
      LOG(ERROR) << "Source directory " << src.string()
                 << " does not exist or is not a directory.";
      return false;
    }
    if (fs::exists(dst)) {
      LOG(ERROR) << "Destination directory " << dst.string()
                 << " already exists.";
      return false;
    }
    // Create the destination directory
    if (!CreateDir(dst)) {
      LOG(ERROR) << "Unable to create destination directory" << dst.string();
      return false;
    }
  } catch (const fs::filesystem_error& error) {
      LOG(ERROR) << error.what();
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
        LOG(ERROR) << error.what();
    }
  }
  return true;
}

}  // namespace utils
}  // namespace common_installer
