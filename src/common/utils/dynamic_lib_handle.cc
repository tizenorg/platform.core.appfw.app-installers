// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/utils/dynamic_lib_handle.h"

#include <memory>

namespace common_installer {

DynamicLibHandle::DynamicLibHandle() : lib_handle_(nullptr) {}

bool DynamicLibHandle::Load(const boost::filesystem::path& path, int flags) {
  if (lib_handle_) {
    return true;
  }

  lib_handle_ = dlopen(path.c_str(), flags);
  if (!lib_handle_) {
    LOG(ERROR) << "Failed to open library: " << path << " (" << dlerror()
               << ")";
    return false;
  }
  return true;
}

void* DynamicLibHandle::GetSymbol(const std::string& name) const {
  return dlsym(lib_handle_, name.c_str());
}

DynamicLibHandle::~DynamicLibHandle() {
  if (lib_handle_) {
    dlclose(lib_handle_);
  }
}

}  // namespace common_installer
