// Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_check_signature.h"

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <tzplatform_config.h>
#include <vcore/Certificate.h>
#include <vcore/SignatureReader.h>
#include <vcore/SignatureFinder.h>
#include <vcore/WrtSignatureValidator.h>
#include <vcore/VCore.h>

#include <string>

namespace bf = boost::filesystem;

namespace {

const bf::path kSignatureXmlSchemaPath =
    bf::path(tzplatform_getenv(TZ_SYS_SHARE))
        / "app-installers/signature_schema.xsd";

common_installer::PrivilegeLevel CertStoreIdToPrivilegeLevel(
    ValidationCore::CertStoreId::Type id) {
  switch (id) {
    case ValidationCore::CertStoreId::VIS_PUBLIC:
      return common_installer::PrivilegeLevel::PUBLIC;
    case ValidationCore::CertStoreId::VIS_PARTNER:
      return common_installer::PrivilegeLevel::PARTNER;
    case ValidationCore::CertStoreId::VIS_PLATFORM:
      return common_installer::PrivilegeLevel::PLATFORM;
    default:
      return common_installer::PrivilegeLevel::UNTRUSTED;
  }
}

common_installer::Step::Status ValidateSignatureFile(
    const bf::path& base_path,
    const ValidationCore::SignatureFileInfo& file_info,
    common_installer::PrivilegeLevel* level) {
  bf::path path = base_path / file_info.getFileName();
  LOG(INFO) << "Processing signature: " << path;
  ValidationCore::SignatureData data(path.string(), file_info.getFileNumber());
  try {
    // Validate file syntax and schema
    ValidationCore::SignatureReader xml;
    xml.initialize(data, kSignatureXmlSchemaPath.string());
    xml.read(data);

    // Validate file semantic
    ValidationCore::WrtSignatureValidator validator(
        ValidationCore::WrtSignatureValidator::TIZEN, true, true, false);
    ValidationCore::WrtSignatureValidator::Result result =
        validator.check(data, base_path.string());

    switch (result) {
      case ValidationCore::WrtSignatureValidator::SIGNATURE_REVOKED: {
        LOG(ERROR) << "Certificate is revoked";
        return common_installer::Step::Status::ERROR;
      };
      case ValidationCore::WrtSignatureValidator::SIGNATURE_INVALID: {
        LOG(ERROR) << "Certificate is invalid";
        return common_installer::Step::Status::ERROR;
      };
      case ValidationCore::WrtSignatureValidator::SIGNATURE_DISREGARD: {
          if (data.isAuthorSignature()) {
            LOG(ERROR) << "Author-signiture is disregarded";
            return common_installer::Step::Status::ERROR;
          }
          LOG(WARNING) << "Signature disregarded: " << path;
          break;
      };
      case ValidationCore::WrtSignatureValidator::SIGNATURE_VERIFIED: {
        if (!data.isAuthorSignature()) {
          // First distributor signature sets the privilege level
          // (wrt spec. 0620.)
          if (file_info.getFileNumber() == 1 &&
              *level == common_installer::PrivilegeLevel::UNTRUSTED) {
            *level = CertStoreIdToPrivilegeLevel(data.getVisibilityLevel());
          }
        }
        break;
      };
      default: {
        return common_installer::Step::Status::ERROR;
      };
    }
  } catch (const ValidationCore::ParserSchemaException::Base& exception) {
      // Needs to catch parser exceptions
      LOG(ERROR) << "Error occured in ParserSchema: "
                 << exception.DumpToString();
      return common_installer::Step::Status::ERROR;
  }
  return common_installer::Step::Status::OK;
}

}  // namespace

namespace common_installer {
namespace signature {

Step::Status ValidateSignatures(const bf::path& base_path,
                                PrivilegeLevel* level) {
  ValidationCore::VCoreInit();
  // Find signature files
  ValidationCore::SignatureFileInfoSet signature_files;
  ValidationCore::SignatureFinder signature_finder(base_path.string());
  if (signature_finder.find(signature_files) !=
      ValidationCore::SignatureFinder::NO_ERROR) {
    LOG(ERROR) << "Error while searching for signatures";
    ValidationCore::VCoreDeinit();
    return Step::Status::ERROR;
  }
  LOG(INFO) << "Number of signature files: " << signature_files.size();

  // Read xml schema for signatures
  for (auto& file_info : signature_files) {
    Step::Status status = ValidateSignatureFile(base_path, file_info, level);
    if (status != Step::Status::OK) {
      ValidationCore::VCoreDeinit();
      return status;
    }
  }
  ValidationCore::VCoreDeinit();
  return Step::Status::OK;
}

Step::Status StepCheckSignature::precheck() {
  if (context_->unpacked_dir_path.get().empty()) {
    LOG(ERROR) << "unpacked_dir_path attribute is empty";
    return Step::Status::INVALID_VALUE;
  }
  if (!boost::filesystem::exists(context_->unpacked_dir_path.get())) {
    LOG(ERROR) << "unpacked_dir_path ("
               << context_->unpacked_dir_path.get()
               << ") path does not exist";
    return Step::Status::INVALID_VALUE;
  }

  return Step::Status::OK;
}

Step::Status StepCheckSignature::process() {
  PrivilegeLevel level = PrivilegeLevel::UNTRUSTED;
  Status status =
      ValidateSignatures(context_->unpacked_dir_path.get(), &level);
  if (status != Status::OK) {
    return status;
  }

  LOG(INFO) << "Privilege level: " << PrivilegeLevelToString(level);
  context_->privilege_level.set(level);

  // TODO(t.iwanek): check settings for privilege level...

  // TODO(t.iwanek): verify privileges according to privilege level...

  // TODO(t.iwanek): check old certificate during update...

  LOG(INFO) << "Signature done";
  return Status::OK;
}

}  // namespace signature
}  // namespace common_installer
