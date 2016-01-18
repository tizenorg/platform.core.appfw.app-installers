// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_UTILS_DYNAMIC_LIB_HANDLE_H_
#define COMMON_UTILS_DYNAMIC_LIB_HANDLE_H_

#include <dlfcn.h>
#include <manifest_parser/utils/logging.h>

#include <memory>
#include <string>

namespace common_installer {

class DynamicLibHandle {
 public:
  static std::unique_ptr<DynamicLibHandle> Create(const std::string& name,
                                                  int flags);

  template <typename Ret, typename... Args>
  bool run(const std::string& name, Ret* result, Args... args) {
    using PluginFunctionPtr = Ret(*)(Args...);
    auto function = reinterpret_cast<PluginFunctionPtr>(GetSymbol(name));

    if (function == nullptr) {
      LOG(ERROR) << "Failed to get symbol: " << name;
      LOG(ERROR) << dlerror();
      return false;
    }

    *result = function(args...);
    return true;
  }

  DynamicLibHandle();

  DynamicLibHandle(const DynamicLibHandle& handle) = delete;
  DynamicLibHandle& operator=(const DynamicLibHandle& handle) = delete;

  DynamicLibHandle(DynamicLibHandle&& handle) = default;
  DynamicLibHandle& operator=(DynamicLibHandle&& handle) = default;

  ~DynamicLibHandle();

  SCOPE_LOG_TAG(DynamicLibHandle)

 private:
  bool CreateImpl(const std::string& name, int flags);

  void* GetSymbol(const std::string& name);
  void* lib_handle_;
};

}  // namespace common_installer

#endif  // COMMON_UTILS_DYNAMIC_LIB_HANDLE_H_
