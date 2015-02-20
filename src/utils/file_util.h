/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#ifndef UTILS_FILE_UTIL_H_
#define UTILS_FILE_UTIL_H_

#include <boost/filesystem.hpp>

namespace utils {

bool CreateDir(const boost::filesystem::path& path);

bool CopyDir(const boost::filesystem::path& src,
             const boost::filesystem::path& dst);

}  // namespace utils

#endif  // UTILS_FILE_UTIL_H_
