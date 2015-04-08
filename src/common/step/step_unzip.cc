/* 2014, Copyright © Intel Coporation, APACHE-2.0, see LICENSE file */

#include "common/step/step_unzip.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <unzip.h>
#include <zlib.h>
#include <tzplatform_config.h>

#include <boost/filesystem.hpp>
#include <boost/chrono/detail/system.hpp>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <climits>
#include <cstring>
#include <string>

#include "utils/file_util.h"

#define ZIPBUFSIZE 8192
#define ZIPMAXPATH 256

namespace bf = boost::filesystem;
namespace bs = boost::system;

namespace {

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

int64_t GetUnpackedPackageSize(const bf::path& path) {
  int64_t size = 0;
  int64_t block_size = GetBlockSizeForPath(path);

  // if failed to stat path
  if (block_size == -1)
    return -1;

  unz_global_info info;
  unz_file_info64 raw_file_info;
  char raw_file_name_in_zip[ZIPMAXPATH];

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
        sizeof(raw_file_name_in_zip), NULL, 0, NULL, 0) != UNZ_OK) {
      LOG(ERROR) << "Failed to read file info";
      return -1;
    }
    size += RoundUpToBlockSizeOf(raw_file_info.uncompressed_size, block_size);
  }

  // FIXME: calculate space needed for directories
  unzClose(zip_file);
  return size;
}

bool CheckFreeSpaceAtPath(int64_t required_size,
    const boost::filesystem::path& target_location) {
  bs::error_code error;
  boost::filesystem::path root = target_location;
  while (!bf::exists(root) && root != root.root_path()) {
    root = root.parent_path();
  }
  if (!bf::exists(root)) {
    LOG(ERROR) << "No mount point for path: " << target_location;
    return false;
  }
  bf::space_info space_info = bf::space(root, error);
  if (error) {
    LOG(ERROR) << "Failed to get space_info: " << error.message();
    return false;
  }

  return (space_info.free >= required_size);
}

}  // namespace

namespace common_installer {
namespace unzip {

StepUnzip::StepUnzip(ContextInstaller* context)
    : Step(context),
      is_extracted_(false) {}

boost::filesystem::path StepUnzip::GenerateTmpDir(const std::string &app_path) {
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

Step::Status StepUnzip::ExtractToTmpDir(const char* src,
    const bf::path& tmp_dir) {
  if (is_extracted_) {
    LOG(ERROR) << src << " is already extracted";
    return Status::OK;
  }

  unz_global_info info;
  char read_buffer[ZIPBUFSIZE];
  unz_file_info raw_file_info;
  char raw_file_name_in_zip[ZIPMAXPATH];

  current_path(tmp_dir);

  unzFile* zip_file = static_cast<unzFile*>(unzOpen(src));
  if (zip_file == NULL) {
    LOG(ERROR) << "Failed to open the source dir: " << src;
    return Step::Status::ERROR;
  }

  if (unzGetGlobalInfo(zip_file, &info) != UNZ_OK) {
    LOG(ERROR) << "Failed to read global info";
    unzClose(zip_file);
    return Step::Status::ERROR;
  }

  for (uLong i = 0; i < info.number_entry; i++) {
    if (unzGetCurrentFileInfo(zip_file, &raw_file_info, raw_file_name_in_zip,
        sizeof(raw_file_name_in_zip), NULL, 0, NULL, 0) != UNZ_OK) {
      LOG(ERROR) << "Failed to read file info";
      unzClose(zip_file);
      return Step::Status::ERROR;
    }

    if (raw_file_name_in_zip[0] == '\0')
      return Step::Status::ERROR;

    bf::path filename_in_zip_path(raw_file_name_in_zip);
    if (!filename_in_zip_path.parent_path().empty()) {
      if (!utils::CreateDir(filename_in_zip_path.parent_path())) {
        LOG(ERROR) << "Failed to create directory: "
            << filename_in_zip_path.parent_path();
        return Step::Status::ERROR;
      }
    }

    if (unzOpenCurrentFile(zip_file) != UNZ_OK) {
      LOG(ERROR) << "Failed to open file";
      unzClose(zip_file);
      return Step::Status::ERROR;
    }

    if (!is_directory(filename_in_zip_path)) {
      FILE *out = fopen(raw_file_name_in_zip, "wb");
      if (!out) {
        LOG(ERROR) << "Failed to open destination ";
        unzCloseCurrentFile(zip_file);
        return Step::Status::ERROR;
      }

      int ret = UNZ_OK;
      do {
        ret = unzReadCurrentFile(zip_file, read_buffer, ZIPBUFSIZE);
        if (ret < 0) {
          LOG(ERROR) << "Failed to read data: " << ret;
          unzCloseCurrentFile(zip_file);
          return Step::Status::ERROR;
        } else {
          fwrite(read_buffer, sizeof(char), ret, out);
        }
      } while (ret > 0);

      fclose(out);
    }

    if ((i+1) < info.number_entry) {
      if (unzGoToNextFile(zip_file) != UNZ_OK) {
        LOG(ERROR) << "Failed to read next file";
        unzCloseCurrentFile(zip_file);
        return Step::Status::ERROR;
      }
    }
  }

  unzClose(zip_file);
  is_extracted_ = true;
  return Status::OK;
}

Step::Status StepUnzip::precheck() {
  if (context_->file_path.get().empty()) {
    LOG(ERROR) << "file_path attribute is empty";
    return Step::Status::INVALID_VALUE;
  }
  if (!boost::filesystem::exists(context_->file_path.get())) {
    LOG(ERROR) << "file_path ("
               << context_->file_path.get()
               << ") path does not exist";
    return Step::Status::INVALID_VALUE;
  }

  if (context_->root_application_path.get().empty()) {
    LOG(ERROR) << "root_application_path attribute is empty";
    return Step::Status::INVALID_VALUE;
  }
  if (!boost::filesystem::exists(context_->root_application_path.get())) {
    LOG(ERROR) << "root_application_path ("
               << context_->root_application_path.get()
               << ") path does not exist";
    // TODO(p.sikorski) maybe it should be created (instead of returning error)
    // but, if so, then it should be created in a separate step, eg.
    // AppInstallerConfigure or something similar
    return Step::Status::INVALID_VALUE;
  }

  return Step::Status::OK;
}

Step::Status StepUnzip::process() {
  bf::path tmp_dir =
      GenerateTmpDir(context_->root_application_path.get());

  if (!utils::CreateDir(tmp_dir)) {
    LOG(ERROR) << "Failed to create temp directory: " << tmp_dir;
    return Step::Status::ERROR;
  }

  int64_t required_size =
      GetUnpackedPackageSize(bf::path(context_->file_path.get()));

  if (required_size == -1) {
    LOG(ERROR) << "Couldn't get uncompressed size for package: "
               << context_->file_path.get();
    return Step::Status::ERROR;
  }

  LOG(DEBUG) << "Required size for application: " << required_size << "B";

  if (!CheckFreeSpaceAtPath(required_size, tmp_dir)) {
    LOG(ERROR) << "There is not enough space to unpack application files";
    return Step::Status::OUT_OF_SPACE;
  }

  if (!CheckFreeSpaceAtPath(required_size,
      bf::path(context_->root_application_path.get()))) {
    LOG(ERROR) << "There is not enough space to install application files";
    return Step::Status::OUT_OF_SPACE;
  }

  if (ExtractToTmpDir(context_->file_path.get().c_str(), tmp_dir)
      != Step::Status::OK) {
    LOG(ERROR) << "Failed to process unpack step";
    return Step::Status::ERROR;
  }
  context_->unpacked_dir_path.set(tmp_dir.string());

  LOG(INFO) << context_->file_path.get() << " was successfully unzipped into "
      << context_->unpacked_dir_path.get();
  return Status::OK;
}

Step::Status StepUnzip::undo() {
  if (access(context_->unpacked_dir_path.get().c_str(), F_OK) == 0) {
    bf::remove_all(context_->unpacked_dir_path.get());
    LOG(DEBUG) << "remove temp dir: " << context_->unpacked_dir_path.get();
  }
  return Status::OK;
}


}  // namespace unzip
}  // namespace common_installer
