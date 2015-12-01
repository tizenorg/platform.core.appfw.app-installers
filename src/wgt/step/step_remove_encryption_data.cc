// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "wgt/step/step_remove_encryption_data.h"

#include <web_app_enc.h>

#include <manifest_parser/utils/logging.h>

namespace wgt {
namespace encrypt {

common_installer::Step::Status StepRemoveEncryptionData::clean() {
  wae_app_type_e enc_type =
      context_->request_mode.get() == common_installer::RequestMode::GLOBAL ?
          WAE_DOWNLOADED_GLOBAL_APP : WAE_DOWNLOADED_NORMAL_APP;

  // There is no check, if application was encrypted or not
  // (it is not saved anywhere in tizen manifest)
  // so, if WAE_ERROR_NO_KEY error, then application was not encrypted
  int ret = wae_remove_app_dek(context_->pkgid.get().c_str(), enc_type);
  if (WAE_ERROR_NONE == ret || WAE_ERROR_NO_KEY == ret) {
    LOG(DEBUG) << "Encryption data removed (if existed)";
    return common_installer::Step::Status::OK;
  }

  switch (ret) {
  case WAE_ERROR_INVALID_PARAMETER:
    LOG(ERROR) << "Error while removing encryption data: "
                  "WAE_ERROR_INVALID_PARAMETER";
    break;
  case WAE_ERROR_PERMISSION_DENIED:
    LOG(ERROR) << "Error while removing encryption data: "
                  "WAE_ERROR_PERMISSION_DENIED";
    break;
  case WAE_ERROR_KEY_MANAGER:
    LOG(ERROR) << "Error while removing encryption data: "
                  "WAE_ERROR_KEY_MANAGER";
    break;
  case WAE_ERROR_UNKNOWN:
    LOG(ERROR) << "Error while removing encryption data: "
                  "WAE_ERROR_UNKNOWN";
    break;
  default:
    LOG(ERROR) << "Error while removing encryption data: "
                  "UNKNOWN";
    break;
  }
  return common_installer::Step::Status::ERROR;
}

}  // namespace encrypt
}  // namespace wgt
