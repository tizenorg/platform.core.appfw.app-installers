#ifndef WRAPPER_OBJECT_BASE_
#define WRAPPER_OBJECT_BASE_

#include <map>
#include <boost/filesystem.hpp>

#include <pkgmgrinfo_basic.h>
#include <pkgmgr_parser.h>

#include "wrapper_data_allocator.h"

template <typename T>
class WrapperObjectBase {
 public:
  using WrapperBoolType = WrapperDataAllocator<bool>;
  using WrapperStringType = WrapperDataAllocator<const std::string &>;

  explicit WrapperObjectBase() : object_(nullptr) {}
  explicit WrapperObjectBase(T object) : object_(object) {}

 protected:
  T object_;
};

#endif  // WRAPPER_OBJECT_BASE
