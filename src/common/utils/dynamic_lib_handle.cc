// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "dynamic_lib_handle.h"

namespace common_installer {

bool DynamicLibHandle::Create(const std::string& name, int flags) {
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
