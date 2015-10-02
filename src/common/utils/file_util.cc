/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#include "common/utils/file_util.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unzip.h>
#include <zlib.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/system/error_code.hpp>

#include <algorithm>
#include <string>
#include <vector>

#include "common/utils/byte_size_literals.h"
#include "common/utils/logging.h"

namespace ba = boost::algorithm;
namespace bs = boost::system;
namespace bf = boost::filesystem;

namespace {

unsigned kZipBufSize = 8_kB;
unsigned kZipMaxPath = 256;

int64_t GetBlockSizeForPath(const bf::path& path_in_partition) {
  struct stat stats;
  if (stat(path_in_partition.string().c_str(), &stats)) {
    LOG(ERROR) << "stat(" << path_in_partition.string()
               << ") failed - error code: " << errno;
    return -1;
  }
  return stats.st_blksize;
}

int64_t RoundUpToBlockSizeOf(int64_t size, int64_t block_size) {
  return ((size + block_size - 1) / block_size) * block_size;
}

class UnzFilePointer {
 public:
  UnzFilePointer()
    : zipFile_(nullptr),
      fileOpened_(false),
      currentFileOpened_(false) { }

  ~UnzFilePointer() {
    if (currentFileOpened_)
      unzCloseCurrentFile(zipFile_);
    if (fileOpened_)
      unzClose(zipFile_);
  }

  bool Open(const char* zip_path) {
    zipFile_ = static_cast<unzFile*>(unzOpen(zip_path));
    if (!zipFile_)
       return false;
    fileOpened_ = true;
    return true;
  }

  bool OpenCurrent() {
    if (unzOpenCurrentFile(zipFile_) != UNZ_OK)
      return false;
    currentFileOpened_ = true;
    return true;
  }

  void CloseCurrent() {
    if (currentFileOpened_)
      unzCloseCurrentFile(zipFile_);
    currentFileOpened_ = false;
  }

  unzFile* Get() { return zipFile_; }

 private:
  unzFile* zipFile_;
  bool fileOpened_;
  bool currentFileOpened_;
};

}  // namespace

namespace common_installer {

bool CreateDir(const bf::path& path) {
  if (bf::exists(path))
    return true;

  boost::system::error_code error;
  bf::create_directories(path, error);
  if (error) {
    LOG(ERROR) << "Failed to create directory: "
               << boost::system::system_error(error).what();
    return false;
  }

  bf::permissions(path, bf::owner_all
      | bf::group_read | bf::others_read,
      error);
  if (error) {
    LOG(ERROR) << "Failed to set permission: "
               << boost::system::system_error(error).what();
    return false;
  }
  return true;
}

bool CopyDir(const bf::path& src, const bf::path& dst, bool merge_directories) {
  try {
    // Check whether the function call is valid
    if (!bf::exists(src) || !bf::is_directory(src)) {
      LOG(ERROR) << "Source directory " << src.string()
                 << " does not exist or is not a directory.";
      return false;
    }
    if (bf::exists(dst)) {
      if (!merge_directories) {
        LOG(ERROR) << "Destination directory " << dst.string()
                   << " already exists.";
        return false;
      }
    } else {
      if (!CreateDir(dst)) {
        LOG(ERROR) << "Unable to create destination directory" << dst.string();
        return false;
      }
    }
  } catch (const bf::filesystem_error& error) {
    LOG(ERROR) << error.what();
    return false;
  }

  // Iterate through the source directory
  for (bf::directory_iterator file(src);
      file != bf::directory_iterator();
      ++file) {
    try {
      bf::path current(file->path());
      if (bf::is_directory(current)) {
        if (!CopyDir(current, dst / current.filename())) {
          return false;
        }
      } else if (bf::is_symlink(current)) {
        bf::copy_symlink(current, dst / current.filename());
      } else {
        bf::copy_file(current, dst / current.filename());
      }
    } catch (const bf::filesystem_error& error) {
      LOG(ERROR) << error.what();
      return false;
    }
  }
  return true;
}

bool MoveDir(const bf::path& src, const bf::path& dst, bool merge_directories) {
  bs::error_code error;
  bf::rename(src, dst, error);
  if (error) {
    LOG(WARNING) << "Cannot move directory: " << src << ". Will copy/remove...";
    if (!CopyDir(src, dst, merge_directories)) {
      LOG(ERROR) << "Cannot copy directory: " << src;
      bf::remove_all(dst, error);
      return false;
    }
    bf::remove_all(src, error);
    if (error) {
      LOG(ERROR) << "Cannot remove old directory when coping: " << src;
      return false;
    }
  }
  return true;
}

bool MoveFile(const bf::path& src, const bf::path& dst) {
  if (bf::exists(dst))
    return false;
  bs::error_code error;
  bf::rename(src, dst, error);
  if (error) {
    LOG(WARNING) << "Cannot move file: " << src << ". Will copy/remove...";
    bf::copy_file(src, dst, bf::copy_option::overwrite_if_exists, error);
    if (error) {
      return false;
    }
    bf::remove_all(src, error);
    if (error) {
      LOG(ERROR) << "Cannot remove old file when coping: " << src;
    }
  }
  return true;
}

int64_t GetUnpackedPackageSize(const bf::path& path) {
  int64_t size = 0;
  int64_t block_size = GetBlockSizeForPath(path);

  // if failed to stat path
  if (block_size == -1)
    return -1;

  unz_global_info info;
  unz_file_info64 raw_file_info;
  char raw_file_name_in_zip[kZipMaxPath];

  unzFile* zip_file = static_cast<unzFile*>(unzOpen(path.string().c_str()));
  if (zip_file == nullptr) {
    LOG(ERROR) << "Failed to open the source dir: " << path.string();
    return -1;
  }

  if (unzGetGlobalInfo(zip_file, &info) != UNZ_OK) {
    LOG(ERROR) << "Failed to read global info";
    unzClose(zip_file);
    return -1;
  }

  for (uLong i = 0; i < info.number_entry; i++) {
    if (unzGetCurrentFileInfo64(zip_file, &raw_file_info, raw_file_name_in_zip,
        sizeof(raw_file_name_in_zip), nullptr, 0, nullptr, 0) != UNZ_OK) {
      LOG(ERROR) << "Failed to read file info";
      return -1;
    }
    size += RoundUpToBlockSizeOf(raw_file_info.uncompressed_size, block_size);
  }

  // FIXME: calculate space needed for directories
  unzClose(zip_file);
  return size;
}

boost::filesystem::path GenerateTmpDir(const bf::path &app_path) {
  boost::filesystem::path install_tmp_dir;
  boost::filesystem::path tmp_dir(app_path);

  do {
    boost::filesystem::path model;
    boost::filesystem::path unique_dir =
        boost::filesystem::unique_path(model = "unpack-%%%%%%");

    install_tmp_dir = tmp_dir /= unique_dir;
  } while (boost::filesystem::exists(install_tmp_dir) &&
           boost::filesystem::is_directory(install_tmp_dir));

  return install_tmp_dir;
}

boost::filesystem::path GenerateTemporaryPath(
    const boost::filesystem::path& path) {
  bf::path pattern = path;
  pattern += "-%%%%%%";
  bf::path tmp_path;
  do {
    tmp_path = boost::filesystem::unique_path(pattern);
  } while (boost::filesystem::exists(tmp_path));
  return tmp_path;
}

bool ExtractToTmpDir(const char* zip_path,
                     const boost::filesystem::path& tmp_dir) {
  return ExtractToTmpDir(zip_path, tmp_dir, "");
}

bool ExtractToTmpDir(const char* zip_path, const bf::path& tmp_dir,
                     const std::string& filter_prefix) {
  unz_global_info info;
  char read_buffer[kZipBufSize];
  unz_file_info raw_file_info;
  char raw_file_name_in_zip[kZipMaxPath];

  current_path(tmp_dir);

  UnzFilePointer zip_file;
  if (!zip_file.Open(zip_path)) {
    LOG(ERROR) << "Failed to open the source dir: " << zip_path;
    return false;
  }

  if (unzGetGlobalInfo(zip_file.Get(), &info) != UNZ_OK) {
    LOG(ERROR) << "Failed to read global info";
    return false;
  }

  for (uLong i = 0; i < info.number_entry; i++) {
    if (unzGetCurrentFileInfo(zip_file.Get(),
                              &raw_file_info,
                              raw_file_name_in_zip,
                              sizeof(raw_file_name_in_zip),
                              nullptr,
                              0,
                              nullptr,
                              0) != UNZ_OK) {
      LOG(ERROR) << "Failed to read file info";
      return false;
    }

    if (raw_file_name_in_zip[0] == '\0')
      return false;

    // unpack if filter is empty or path is matched
    if (filter_prefix.empty() ||
        std::string(raw_file_name_in_zip).find(filter_prefix) == 0) {
      bf::path filename_in_zip_path(raw_file_name_in_zip);

      // prevent "directory climbing" attack
      std::vector<std::string> segments;
      ba::split(segments, filename_in_zip_path.string(), ba::is_any_of("/\\"));
      if (std::any_of(segments.begin(), segments.end(),
                      [](const std::string& segment) {
                        return segment == "..";
                      })) {
        LOG(ERROR) << "Relative path in widget in malformed";
        return false;
      }

      if (!filename_in_zip_path.parent_path().empty()) {
        if (!CreateDir(filename_in_zip_path.parent_path())) {
          LOG(ERROR) << "Failed to create directory: "
              << filename_in_zip_path.parent_path();
          return false;
        }
      }

      if (!zip_file.OpenCurrent()) {
        LOG(ERROR) << "Failed to open file";
        return false;
      }

      if (!is_directory(filename_in_zip_path)) {
        FILE *out = fopen(raw_file_name_in_zip, "wb");
        if (!out) {
          LOG(ERROR) << "Failed to open destination ";
          return false;
        }

        int ret = UNZ_OK;
        do {
          ret = unzReadCurrentFile(zip_file.Get(), read_buffer, kZipBufSize);
          if (ret < 0) {
            LOG(ERROR) << "Failed to read data: " << ret;
            return false;
          } else {
            fwrite(read_buffer, sizeof(char), ret, out);
          }
        } while (ret > 0);

        fclose(out);
      }

      zip_file.CloseCurrent();
    }

    if ((i+1) < info.number_entry) {
      if (unzGoToNextFile(zip_file.Get()) != UNZ_OK) {
        LOG(ERROR) << "Failed to read next file";
        return false;
      }
    }
  }

  return true;
}

int GetSize(const boost::filesystem::path& path) {
  int size = 0;
  for (bf::recursive_directory_iterator iter(path);
      iter != bf::recursive_directory_iterator(); ++iter) {
      struct stat buf;
      if (lstat(iter->path().c_str(), &buf) == -1) {
        LOG(ERROR) << "lstat() failed for: " << iter->path();
        return -1;
      }
      size += buf.st_size;
  }
  return size;
}

}  // namespace common_installer
