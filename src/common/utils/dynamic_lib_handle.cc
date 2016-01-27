// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/utils/dynamic_lib_handle.h"

#include <memory>

namespace common_installer {

DynamicLibHandle::DynamicLibHandle() : lib_handle_(nullptr) {}

std::unique_ptr<DynamicLibHandle> DynamicLibHandle::Create(
    const boost::filesystem::path& path, int flags) {
  std::unique_ptr<DynamicLibHandle> new_handle(new DynamicLibHandle);
  if (!new_handle->CreateImpl(path, flags)) {
    return nullptr;
  }
  return new_handle;
}

bool DynamicLibHandle::CreateImpl(const boost::filesystem::path& path,
                                  int flags) {
  lib_handle_ = dlopen(path.c_str(), flags);
  if (!lib_handle_) {
    LOG(ERROR) << "Failed to open library: " << path << " (" << dlerror()
               << ")";
    return false;
  }
  return true;
}

void* DynamicLibHandle::GetSymbol(const std::string& name) {
  return dlsym(lib_handle_, name.c_str());
}

DynamicLibHandle::~DynamicLibHandle() { dlclose(lib_handle_); }

}  // namespace common_installer