#ifndef __TPK_MANIFESTPARSER_MANIFEST_H__
#define __TPK_MANIFESTPARSER_MANIFEST_H__

#include "boost/filesystem.hpp"
#include <exception>
#include "XmlNodes.h"
#include <vector>


namespace tpk {

/* Internal exceptions */
class FileOpenFailureException : public exception {};
class ParseFailureException : public exception {};



class ManifestParser {
  public:
    ManifestParser(const char *manifestFilePath);
    ~ManifestParser();
    XmlNodeManifest &getManifest();
    XmlNodeManifest manifest;

  private:
    template <typename T> void checkAndSetNode(
        xmlTextReaderPtr reader, const char* name, T &xmlNode);
    template <typename T> void checkAndAppendNode(
        xmlTextReaderPtr reader, const char* name, std::vector<T*> &v);
    void processNode(xmlTextReaderPtr reader);
    void streamFile(const char* filePath);
};

}   // ns: tpk


#endif  // __TPK_MANIFESTPARSER_MANIFEST_H__
