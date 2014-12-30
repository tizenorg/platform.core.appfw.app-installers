/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#ifndef COMMON_INCLUDE_UTILS_H_
#define COMMON_INCLUDE_UTILS_H_

#include <boost/filesystem.hpp>

namespace utils {

bool CreateDir(const boost::filesystem::path& path);

}  // namespace utils

#endif  // COMMON_INCLUDE_UTILS_H_
