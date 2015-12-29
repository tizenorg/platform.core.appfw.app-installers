#ifndef WRAPPER_UTILITIES_H
#define WRAPPER_UTILITIES_H

#include <boost/filesystem.hpp>

#include <pkgmgrinfo_basic.h>
#include <pkgmgr_parser.h>

template <typename T>
class UtilitiesBase {
 public:
  UtilitiesBase(T *mainStruct) : mainStruct_(mainStruct) {}

 protected:
  T *mainStruct_;
};

#endif  // WRAPPER_UTILITIES_H
