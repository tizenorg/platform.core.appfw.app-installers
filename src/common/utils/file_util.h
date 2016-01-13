/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#ifndef COMMON_UTILS_FILE_UTIL_H_
#define COMMON_UTILS_FILE_UTIL_H_

#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include <string>

namespace common_installer {

bool CreateDir(const boost::filesystem::path& path);

bool CopyDir(const boost::filesystem::path& src,
             const boost::filesystem::path& dst, bool merge_dirs = false);

bool CopyFile(const boost::filesystem::path& src,
             const boost::filesystem::path& dst);

bool MoveDir(const boost::filesystem::path& src,
             const boost::filesystem::path& dst, bool merge_dirs = false);

bool MoveFile(const boost::filesystem::path& src,
              const boost::filesystem::path& dst);

bool SetDirPermissions(const boost::filesystem::path& path,
                       boost::filesystem::perms permissions);

int64_t GetUnpackedPackageSize(const boost::filesystem::path& path);

boost::filesystem::path GenerateTmpDir(const boost::filesystem::path& app_path);

boost::filesystem::path GenerateTemporaryPath(
    const boost::filesystem::path& path);

bool ExtractToTmpDir(const char* zip_path,
                     const boost::filesystem::path& tmp_dir);

bool ExtractToTmpDir(const char* zip_path,
                     const boost::filesystem::path& tmp_dir,
                     const std::string& filter_prefix);

bool CheckPathInZipArchive(const char* zip_archive_path,
                           const boost::filesystem::path& relative_zip_path,
                           bool* found);

bool HasDirectoryClimbing(const boost::filesystem::path& path);

boost::filesystem::path MakeRelativePath(const boost::filesystem::path& input,
                                         const boost::filesystem::path& base);

}  // namespace common_installer

#endif  // COMMON_UTILS_FILE_UTIL_H_
