#ifndef __TPK_MANIFESTPARSER_MANIFEST_H__
#define __TPK_MANIFESTPARSER_MANIFEST_H__
#include "boost/filesystem.hpp"
#include "../Exception.h"

namespace tpk {
namespace ManifestParser {

class Manifest {
  public:
    Manifest(const char *manifestFilePath);
    ~Manifest();
  private:
};

/* Internal exceptions */
class FileOpenFailureException : public Exception {};


}   // ns: ManifestParser
}   // ns: tpk

#endif  // __TPK_MANIFESTPARSER_MANIFEST_H__
