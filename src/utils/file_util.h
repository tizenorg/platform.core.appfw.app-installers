/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#ifndef UTILS_FILE_UTIL_H_
#define UTILS_FILE_UTIL_H_

#include <boost/filesystem.hpp>

#include <string>

namespace common_installer {
namespace utils {

bool CreateDir(const boost::filesystem::path& path);

bool CopyDir(const boost::filesystem::path& src,
             const boost::filesystem::path& dst);

bool MoveDir(const boost::filesystem::path& src,
             const boost::filesystem::path& dst);

bool MoveFile(const boost::filesystem::path& src,
              const boost::filesystem::path& dst);

int64_t GetUnpackedPackageSize(const boost::filesystem::path& path);

boost::filesystem::path GenerateTmpDir(const boost::filesystem::path& app_path);

bool ExtractToTmpDir(const char* zip_path,
                     const boost::filesystem::path& tmp_dir);

bool ExtractToTmpDir(const char* zip_path,
                     const boost::filesystem::path& tmp_dir,
                     const std::string& filter_prefix);

}  // namespace utils
}  // namespace common_installer

#endif  // UTILS_FILE_UTIL_H_
