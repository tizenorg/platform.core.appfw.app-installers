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
#include <climits>
#include <cstring>
#include <string>

#include "utils/file_util.h"

#define ZIPBUFSIZE 8192
#define ZIPMAXPATH 256

namespace common_installer {
namespace unzip {

StepUnzip::StepUnzip(ContextInstaller* context)
    : Step(context),
      is_extracted_(false) {}

boost::filesystem::path StepUnzip::GenerateTmpDir(const char* app_path) {
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
    const boost::filesystem::path& tmp_dir) {
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

    boost::filesystem::path filename_in_zip_path(raw_file_name_in_zip);
    if (!filename_in_zip_path.parent_path().empty()) {
      if (!utils::CreateDir(filename_in_zip_path.parent_path())) {
        LOG(ERROR) << "Failed to create directory: "
            << filename_in_zip_path.parent_path();
        return Step::Status::OUT_OF_SPACE;
      }
    }

    if (unzOpenCurrentFile(zip_file) != UNZ_OK) {
      LOG(ERROR) << "Failed to open file";
      unzClose(zip_file);
      return Step::Status::OUT_OF_SPACE;
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

Step::Status StepUnzip::process() {
  assert(!context_->file_path().empty());
  assert(!access(context_->file_path().c_str(), F_OK));

  boost::filesystem::path tmp_dir =
      GenerateTmpDir(context_->GetRootApplicationPath());

  if (!utils::CreateDir(tmp_dir)) {
    LOG(ERROR) << "Failed to create temp directory: " << tmp_dir;
    return Step::Status::ERROR;
  }

  Step::Status extract_status =
      ExtractToTmpDir(context_->file_path().c_str(), tmp_dir);
  if (extract_status != Step::Status::OK) {
    LOG(ERROR) << "Failed to process unpack step";
    return extract_status;
  }
  context_->set_unpacked_dir_path(tmp_dir.string());

  LOG(INFO) << context_->file_path() << " was successfully unzipped into "
      << context_->unpacked_dir_path();
  return Status::OK;
}

Step::Status StepUnzip::undo() {
  if (access(context_->unpacked_dir_path().c_str(), F_OK) == 0) {
    boost::filesystem::remove_all(context_->unpacked_dir_path());
    LOG(DEBUG) << "remove temp dir: " << context_->unpacked_dir_path();
  }
  return Status::OK;
}


}  // namespace unzip
}  // namespace common_installer
