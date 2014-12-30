/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#include "include/utils.h"

#include <boost/filesystem/path.hpp>
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

}  // namespace utils
