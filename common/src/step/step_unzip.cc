/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#include "include/step/step_unzip.h"

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
#include <iostream>
#include <string>

#include "include/utils.h"

#define DBG(msg) std::cout << "[Unzip] " << msg << std::endl;
#define ERR(msg) std::cout << "[ERROR: Unzip] " << msg << std::endl;

#define ZIPBUFSIZE 8192
#define ZIPMAXPATH 256

StepUnzip::StepUnzip() : is_extracted_(false) {}

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

int StepUnzip::ExtractToTmpDir(const char* src,
                               const boost::filesystem::path& tmp_dir) {
  if (is_extracted_) {
    ERR(src << " is already extracted");
    return APPINST_R_OK;
  }

  unz_global_info info;
  char read_buffer[ZIPBUFSIZE];
  unz_file_info raw_file_info;
  char raw_file_name_in_zip[ZIPMAXPATH];

  current_path(tmp_dir);

  unzFile* zip_file = static_cast<unzFile*>(unzOpen(src));
  if (zip_file == NULL) {
    ERR("Failed to open the source dir: " << src);
    return APPINST_R_ERROR;
  }

  if (unzGetGlobalInfo(zip_file, &info) != UNZ_OK) {
    ERR("Failed to read global info");
    unzClose(zip_file);
    return APPINST_R_ERROR;
  }

  for (uLong i = 0; i < info.number_entry; i++) {
    if (unzGetCurrentFileInfo(zip_file, &raw_file_info, raw_file_name_in_zip,
        sizeof(raw_file_name_in_zip), NULL, 0, NULL, 0) != UNZ_OK) {
      ERR("Failed to read file info");
      unzClose(zip_file);
      return APPINST_R_ERROR;
    }

    if (raw_file_name_in_zip[0] == '\0')
      return APPINST_R_ERROR;

    boost::filesystem::path filename_in_zip_path(raw_file_name_in_zip);
    if (!filename_in_zip_path.parent_path().empty()) {
      if (!utils::CreateDir(filename_in_zip_path.parent_path())) {
        ERR("Failed to create directory: "
            << filename_in_zip_path.parent_path());
        return APPINST_R_ERROR;
      }
    }

    if (unzOpenCurrentFile(zip_file) != UNZ_OK) {
      ERR("Failed to open file");
      unzClose(zip_file);
      return APPINST_R_ERROR;
    }

    // Do not treat directory
    if (is_directory(filename_in_zip_path))
      continue;

    FILE *out = fopen(raw_file_name_in_zip, "wb");
    if (!out) {
      ERR("Failed to open destination ");
      unzCloseCurrentFile(zip_file);
      return APPINST_R_ERROR;
    }

    int ret = UNZ_OK;
    do {
      ret = unzReadCurrentFile(zip_file, read_buffer, ZIPBUFSIZE);
      if (ret < 0) {
        ERR("Failed to read data: " << ret);
        unzCloseCurrentFile(zip_file);
        return APPINST_R_ERROR;
      } else {
        fwrite(read_buffer, sizeof(char), ret, out);
      }
    } while (ret > 0);

    fclose(out);

    if ((i+1) < info.number_entry) {
      if (unzGoToNextFile(zip_file) != UNZ_OK) {
        ERR("Failed to read next file");
        unzCloseCurrentFile(zip_file);
        return APPINST_R_ERROR;
      }
    }
  }

  unzClose(zip_file);
  is_extracted_ = true;
  return APPINST_R_OK;
}

int StepUnzip::process(ContextInstaller* data) {
  assert(!data->file_path().empty());
  assert(!access(data->file_path().c_str(), F_OK));

  boost::filesystem::path tmp_dir = GenerateTmpDir(data->GetApplicationPath());

  if (!utils::CreateDir(tmp_dir)) {
    ERR("Failed to create temp directory: " << tmp_dir);
    return APPINST_R_ERROR;
  }

  if (ExtractToTmpDir(data->file_path().c_str(), tmp_dir) != APPINST_R_OK) {
    ERR("Failed to process unpack step");
    return APPINST_R_ERROR;
  }
  data->set_unpack_directory(tmp_dir.string());

  DBG(data->file_path() << " was successfully unzipped into "
      << data->unpack_directory());
  return APPINST_R_OK;
}

int StepUnzip::undo(ContextInstaller* data) {
  if (access(data->unpack_directory().c_str(), F_OK) == 0) {
    boost::filesystem::remove_all(data->unpack_directory());
    DBG("remove temp dir: " << data->unpack_directory());
  }
  return APPINST_R_OK;
}
