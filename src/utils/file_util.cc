/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#include "utils/file_util.h"

#include <boost/filesystem/path.hpp>
#include <boost/system/error_code.hpp>
#include <string>

#include "utils/logging.h"

namespace common_installer {
namespace utils {

namespace bs = boost::system;
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
      } else if (fs::is_symlink(current)) {
        // Found symlink
        fs::copy_symlink(current, dst / current.filename());
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

bool MoveDir(const fs::path& src, const fs::path& dst) {
  if (fs::exists(dst))
    return false;
  bs::error_code error;
  fs::rename(src, dst, error);
  if (error) {
    LOG(WARNING) << "Cannot move directory: " << src << ". Will copy/remove...";
    if (!utils::CopyDir(src, dst)) {
      LOG(ERROR) << "Cannot copy directory: " << src;
      return false;
    }
    fs::remove_all(src, error);
    if (error) {
      LOG(ERROR) << "Cannot remove old directory when coping: " << src;
      return false;
    }
  }
  return true;
}

bool MoveFile(const fs::path& src, const fs::path& dst) {
  if (fs::exists(dst))
    return false;
  bs::error_code error;
  fs::rename(src, dst, error);
  if (error) {
    LOG(WARNING) << "Cannot move file: " << src << ". Will copy/remove...";
    fs::copy_file(src, dst, fs::copy_option::overwrite_if_exists, error);
    if (error) {
      return false;
    }
    fs::remove_all(src, error);
    if (error) {
      LOG(ERROR) << "Cannot remove old file when coping: " << src;
    }
  }
  return true;
}

}  // namespace utils
}  // namespace common_installer
