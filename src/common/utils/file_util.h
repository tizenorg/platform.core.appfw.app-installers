/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#ifndef COMMON_UTILS_FILE_UTIL_H_
#define COMMON_UTILS_FILE_UTIL_H_

#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include <string>

namespace common_installer {

enum FSFlag {
  FS_NONE              = 0,
  FS_MERGE_DIRECTORIES = (1 << 0)
};

__attribute__ ((visibility ("default"))) bool CreateDir(const boost::filesystem::path& path);

__attribute__ ((visibility ("default"))) bool CopyDir(const boost::filesystem::path& src,
             const boost::filesystem::path& dst, FSFlag flags = FS_NONE);

__attribute__ ((visibility ("default"))) bool CopyFile(const boost::filesystem::path& src,
             const boost::filesystem::path& dst);

__attribute__ ((visibility ("default"))) bool MoveDir(const boost::filesystem::path& src,
             const boost::filesystem::path& dst, FSFlag flags = FS_NONE);

__attribute__ ((visibility ("default"))) bool MoveFile(const boost::filesystem::path& src,
              const boost::filesystem::path& dst);

__attribute__ ((visibility ("default"))) bool SetDirPermissions(const boost::filesystem::path& path,
                       boost::filesystem::perms permissions);

__attribute__ ((visibility ("default"))) int64_t GetUnpackedPackageSize(const boost::filesystem::path& path);

__attribute__ ((visibility ("default"))) boost::filesystem::path GenerateTmpDir(const boost::filesystem::path& app_path);

__attribute__ ((visibility ("default"))) boost::filesystem::path GenerateTemporaryPath(
    const boost::filesystem::path& path);

__attribute__ ((visibility ("default"))) bool ExtractToTmpDir(const char* zip_path,
                     const boost::filesystem::path& tmp_dir);

__attribute__ ((visibility ("default"))) bool ExtractToTmpDir(const char* zip_path,
                     const boost::filesystem::path& tmp_dir,
                     const std::string& filter_prefix);

__attribute__ ((visibility ("default"))) bool CheckPathInZipArchive(const char* zip_archive_path,
                           const boost::filesystem::path& relative_zip_path,
                           bool* found);

__attribute__ ((visibility ("default"))) bool HasDirectoryClimbing(const boost::filesystem::path& path);

__attribute__ ((visibility ("default"))) boost::filesystem::path MakeRelativePath(const boost::filesystem::path& input,
                                         const boost::filesystem::path& base);

}  // namespace common_installer

#endif  // COMMON_UTILS_FILE_UTIL_H_
