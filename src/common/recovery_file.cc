// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/recovery_file.h"

#include <boost/filesystem/operations.hpp>
#include <boost/system/error_code.hpp>

#include <array>
#include <cstring>
#include <map>

#include "common/installer_context.h"
#include "common/utils/logging.h"

namespace bf = boost::filesystem;
namespace bs = boost::system;
namespace ci = common_installer;

namespace {

const char kRecoveryNewInstallationString[] = "NEW";
const char kRecoveryUpdateInstallationString[] = "UPDATE";
const char kRecoveryUninstallationString[] = "UNINSTALLATION";
const char kRecoveryRdsString[] = "RDS";
const char kRecoveryDeltaString[] = "DELTA";
const char kRecoveryUnknownString[] = "UNKNOWN";

const std::map<std::string, ci::RequestType> kStringToRequestMap = {
  {kRecoveryNewInstallationString, ci::RequestType::Install},
  {kRecoveryUpdateInstallationString, ci::RequestType::Update},
  {kRecoveryUninstallationString, ci::RequestType::Uninstall},
  {kRecoveryRdsString, ci::RequestType::Reinstall},
  {kRecoveryDeltaString, ci::RequestType::Delta}
};

std::string TruncateNewLine(const char* data) {
  int length = strlen(data);
  if (data[length - 1] == '\n')
      --length;
  return std::string(data, length);
}

}  // namespace

namespace common_installer {
namespace recovery {

std::unique_ptr<RecoveryFile> RecoveryFile::CreateRecoveryFileForPath(
    const boost::filesystem::path& path) {
  if (bf::exists(path)) {
    LOG(ERROR) << "Recovery file already exists!";
    return nullptr;
  }
  std::unique_ptr<RecoveryFile> file(new RecoveryFile(path, false));
  if (file->is_detached()) {
    LOG(ERROR) << "Failed to access file";
    return nullptr;
  }
  return file;
}

std::unique_ptr<RecoveryFile> RecoveryFile::OpenRecoveryFileForPath(
    const boost::filesystem::path& path) {
  if (!bf::exists(path)) {
    LOG(ERROR) << "Cannot open recovery file";
    return nullptr;
  }
  std::unique_ptr<RecoveryFile> file(new RecoveryFile(path, true));
  if (file->is_detached()) {
    LOG(ERROR) << "Failed to read recovery file";
    return nullptr;
  }
  return file;
}

RecoveryFile::RecoveryFile(const bf::path& path, bool load)
    : path_(path) {
  if (load) {
    if (!ReadFileContent()) {
      path_.clear();
      return;
    }
  } else {
    // create file
    FILE* handle = fopen(path.c_str(), "w");
    if (!handle) {
      path_.clear();
      return;
    }
    fclose(handle);
    LOG(DEBUG) << "Recovery file " << path_ << " created";
  }
}

RecoveryFile::~RecoveryFile() {
  if (!path_.empty()) {
    bs::error_code error;
    bf::remove(path_, error);
    LOG(DEBUG) << "Recovery file " << path_ << " removed";
  }
}

void RecoveryFile::Detach() {
  path_.clear();
}

bool RecoveryFile::is_detached() const {
  return path_.empty();
}

void RecoveryFile::set_unpacked_dir(
    const boost::filesystem::path& unpacked_dir) {
  unpacked_dir_ = unpacked_dir;
}

void RecoveryFile::set_pkgid(const std::string& pkgid) {
  pkgid_ = pkgid;
}

void RecoveryFile::set_type(RequestType type) {
  type_ = type;
}

const boost::filesystem::path& RecoveryFile::unpacked_dir() const {
  return unpacked_dir_;
}

const std::string& RecoveryFile::pkgid() const {
  return pkgid_;
}

RequestType RecoveryFile::type() const {
  return type_;
}

bool RecoveryFile::ReadFileContent() {
  FILE* handle = fopen(path_.c_str(), "r");
  if (!handle) {
    LOG(ERROR) << "Cannot read recovery file";
    return false;
  }
  std::array<char, 200> data;
  data[0] = '\0';
  if (!fgets(data.data(), data.size(), handle)) {
    type_ = RequestType::Unknown;
    fclose(handle);
    return true;
  }
  std::string mode(TruncateNewLine(data.data()));
  auto iter = kStringToRequestMap.find(mode);
  if (iter == kStringToRequestMap.end()) {
    type_ = RequestType::Unknown;
  } else {
    type_ = iter->second;
  }

  if (!fgets(data.data(), data.size(), handle)) {
    fclose(handle);
    return true;
  }
  unpacked_dir_ = TruncateNewLine(data.data());
  if (!fgets(data.data(), data.size(), handle)) {
    fclose(handle);
    return true;
  }
  pkgid_ = TruncateNewLine(data.data());
  fclose(handle);
  return true;
}

bool RecoveryFile::WriteAndCommitFileContent() {
  FILE* handle = fopen(path_.c_str(), "w");
  if (!handle) {
    LOG(ERROR) << "Cannot write recovery file";
    return false;
  }
  switch (type_) {
  case RequestType::Install:
    fputs(kRecoveryNewInstallationString, handle);
    break;
  case RequestType::Update:
    fputs(kRecoveryUpdateInstallationString, handle);
    break;
  case RequestType::Uninstall:
    fputs(kRecoveryUninstallationString, handle);
    break;
  case RequestType::Reinstall:
    fputs(kRecoveryRdsString, handle);
    break;
  case RequestType::Delta:
    fputs(kRecoveryDeltaString, handle);
    break;
  default:
    fputs(kRecoveryUnknownString, handle);
    break;
  }
  fputs("\n", handle);
  fputs(unpacked_dir_.c_str(), handle);
  fputs("\n", handle);
  fputs(pkgid_.c_str(), handle);
  fputs("\n", handle);
  fclose(handle);
  return true;
}

}  // namespace recovery
}  // namespace common_installer

