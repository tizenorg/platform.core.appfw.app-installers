// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_UTILS_DYNAMIC_LIB_HANDLE_H_
#define COMMON_UTILS_DYNAMIC_LIB_HANDLE_H_

#include <boost/filesystem/path.hpp>

#include <dlfcn.h>
#include <manifest_parser/utils/logging.h>

#include <memory>
#include <string>

namespace common_installer {

class DynamicLibHandle {
 public:
  static std::unique_ptr<DynamicLibHandle> Create(
      const boost::filesystem::path& path, int flags);

  template <typename Ret, typename... Args>
  bool run(const std::string& name, Ret* result, Args&... args) {
    using PluginFunctionPtr = Ret (*)(Args...);
    auto function = reinterpret_cast<PluginFunctionPtr>(GetSymbol(name));

    if (!function) {
      LOG(ERROR) << "Failed to get symbol: " << name <<" (" << dlerror() << ")";
      return false;
    }

    LOG(DEBUG) << "Function parameters: ";
    using expander = int[];
    (void)expander{0, (void(LOG(DEBUG) << ' ' << (args)), 0)...};

    *result = function((args)...);
    return true;
  }

  DynamicLibHandle(DynamicLibHandle&&) = default;
  DynamicLibHandle& operator=(DynamicLibHandle&&) = default;

  ~DynamicLibHandle();

  SCOPE_LOG_TAG(DynamicLibHandle)

 private:
  DynamicLibHandle();
  bool CreateImpl(const boost::filesystem::path& path, int flags);

  void* GetSymbol(const std::string& name);
  void* lib_handle_;
};

}  // namespace common_installer

#endif  // COMMON_UTILS_DYNAMIC_LIB_HANDLE_H_
