// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/utils/dynamic_lib_handle.h"

#include <memory>

namespace common_installer {

DynamicLibHandle::DynamicLibHandle() : lib_handle_(nullptr) {}

std::unique_ptr<DynamicLibHandle> DynamicLibHandle::Create(
    const std::string& name, int flags) {
  std::unique_ptr<DynamicLibHandle> new_handle(new DynamicLibHandle);
  if (!new_handle->CreateImpl(name, flags)) {
    return nullptr;
  }
  return new_handle;
}

bool DynamicLibHandle::CreateImpl(const std::string& name, int flags) {
  lib_handle_ = dlopen(name.c_str(), flags);
  if (lib_handle_ == nullptr) {
    LOG(ERROR) << "Failed to open library: " << name;
    LOG(ERROR) << dlerror();
    return false;
  }
  return true;
}

void* DynamicLibHandle::GetSymbol(const std::string& name) {
  return dlsym(lib_handle_, name.c_str());
}

DynamicLibHandle::~DynamicLibHandle() { dlclose(lib_handle_); }

}  // namespace common_installer
