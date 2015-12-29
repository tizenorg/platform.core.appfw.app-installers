#ifndef WRAPPER_BASE
#define WRAPPER_BASE

#include <map>
//#include <iostream>
#include <boost/filesystem.hpp>

#include <pkgmgrinfo_basic.h>
#include <pkgmgr_parser.h>

#include "wrapper_data_allocator.h"

template <typename T>
class WrapperObjectBase {
 public:
  using WrapperBoolType = WrapperDataAllocator<bool>;
  using WrapperStringType = WrapperDataAllocator<const std::string &>;

  explicit WrapperObjectBase() : mainStruct_(nullptr) {}
  explicit WrapperObjectBase(T *mainStruct) : mainStruct_(mainStruct) {}

 protected:
  T *mainStruct_;
};

#endif  // WRAPPER_BASE
