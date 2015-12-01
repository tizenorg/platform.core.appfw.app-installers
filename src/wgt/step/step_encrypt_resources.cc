// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "wgt/step/step_encrypt_resources.h"

#include <web_app_enc.h>

#include <boost/filesystem/operations.hpp>
#include <boost/system/error_code.hpp>

#include <manifest_parser/utils/logging.h>

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <set>
#include <string>

#include "common/utils/file_util.h"

namespace {

const std::set<std::string> encryptSet { ".html", ".htm", ".css", ".js"};

}  // namespace


namespace wgt {
namespace encrypt {

namespace bf = boost::filesystem;
namespace bs = boost::system;

common_installer::Step::Status StepEncryptResources::precheck() {
  backend_data_ = static_cast<WgtBackendData*>(context_->backend_data.get());
  if (!backend_data_) {
    LOG(ERROR) << "no backend data";
    return common_installer::Step::Status::ERROR;
  }

  input_ = context_->unpacked_dir_path.get();

  if (input_.empty()) {
    LOG(ERROR) << "unpacked_dir_path attribute is empty";
    return Step::Status::INVALID_VALUE;
  }
  if (!bf::exists(input_)) {
    LOG(ERROR) << "unpacked_dir_path (" << input_ << ") path does not exist";
    return Step::Status::INVALID_VALUE;
  }

  return common_installer::Step::Status::OK;
}

common_installer::Step::Status StepEncryptResources::process() {
  if (!backend_data_->settings.get().encryption_enabled()) {
    LOG(DEBUG) << "no encryption";
    return common_installer::Step::Status::OK;
  }
  LOG(DEBUG) << "Encrypting";

  if (!Encrypt(input_)) {
    LOG(ERROR) << "Error during encryption";
    return common_installer::Step::Status::ERROR;
  }

  return common_installer::Step::Status::OK;
}

bool StepEncryptResources::Encrypt(const bf::path &src) {
  // traversing through src dir (recurrence if subdir found)
  // for every file found, check if it should be encrypted (ToBeEncrypted)
  // if yes, encrypt it (and replace original one)
  // if not, leave it
  for (bf::directory_iterator file(src);
       file != bf::directory_iterator();
       ++file) {
    bs::error_code error_code;
    bf::path current(file->path());

    bool is_dir = bf::is_directory(current, error_code);
    if (error_code)
      return false;
    if (is_dir) {
      if (!Encrypt(current))
        return false;
      continue;
    }

    bool is_sym = bf::is_symlink(current, error_code);
    if (error_code)
      return false;
    if (is_sym)
      continue;  // TODO(p.sikorski) is it enough?

    // it is regular file (not dir, not symlink)
    if (ToBeEncrypted(current)) {
      LOG(INFO) << "File for encryption: " << current;
      if (!EncryptFile(current))
        return false;
    }
  }
  return true;
}

bool StepEncryptResources::EncryptFile(const bf::path &src) {
  FILE *input = fopen(src.string().c_str(), "rb");
  if (!input) {
    LOG(ERROR) << "Cannot open file for encryption: " << src;
    return false;
  }

  // read size
  fseek(input , 0 , SEEK_END);
  size_t length = ftell(input);
  rewind(input);

  char *input_buffer = new char[length];
  if (length != fread(input_buffer, sizeof(char), length, input)) {
    LOG(ERROR) << "Read error, file: " << src;
    fclose(input);
    delete []input_buffer;
    return false;
  }
  fclose(input);

  unsigned char* encrypted_data = nullptr;
  size_t enc_data_len = 0;
  // TODO(p.sikorski) check if it is Preloaded
  wae_app_type_e enc_type =
      context_->request_mode.get() == common_installer::RequestMode::GLOBAL ?
          WAE_DOWNLOADED_GLOBAL_APP : WAE_DOWNLOADED_NORMAL_APP;


  int ret = wae_encrypt_web_application(
              context_->pkgid.get().c_str(),
              enc_type,
              reinterpret_cast<const unsigned char*>(input_buffer),
              length,
              &encrypted_data,
              &enc_data_len);
  delete []input_buffer;
  if (WAE_ERROR_NONE != ret) {
    switch (ret) {
    case WAE_ERROR_INVALID_PARAMETER:
      LOG(ERROR) << "Error during encrypting: WAE_ERROR_INVALID_PARAMETER";
      break;
    case WAE_ERROR_PERMISSION_DENIED:
      LOG(ERROR) << "Error during encrypting: WAE_ERROR_PERMISSION_DENIED";
      break;
    case WAE_ERROR_NO_KEY:
      LOG(ERROR) << "Error during encrypting: WAE_ERROR_NO_KEY";
      break;
    case WAE_ERROR_KEY_MANAGER:
      LOG(ERROR) << "Error during encrypting: WAE_ERROR_KEY_MANAGER";
      break;
    case WAE_ERROR_CRYPTO:
      LOG(ERROR) << "Error during encrypting: WAE_ERROR_CRYPTO";
      break;
    case WAE_ERROR_UNKNOWN:
      LOG(ERROR) << "Error during encrypting: WAE_ERROR_UNKNOWN";
      break;
    default:
      LOG(ERROR) << "Error during encrypting: UNKNOWN";
      break;
    }
    return false;
  }

  // original file is treated as destination!
  FILE *output = fopen(src.string().c_str(), "wb");
  if (!output) {
    LOG(ERROR) << "Cannot create encrypted file: " << src;
    free(encrypted_data);
    return false;
  }

  if (enc_data_len != fwrite(reinterpret_cast<const char*>(encrypted_data),
                             sizeof(char),
                             enc_data_len,
                             output)) {
    LOG(ERROR) << "Write error, file: " << src;
    free(encrypted_data);
    fclose(output);
    return false;
  }


  fclose(output);
  free(encrypted_data);
  return true;
}

bool StepEncryptResources::ToBeEncrypted(const bf::path &file) {
  size_t found_key = file.string().rfind(".");
  if (std::string::npos != found_key) {
    std::string mimeType = file.string().substr(found_key);
    std::transform(mimeType.begin(), mimeType.end(), mimeType.begin(),
                   ::tolower);
    return encryptSet.count(mimeType) > 0;
  }
  return false;
}

}  // namespace encrypt
}  // namespace wgt
