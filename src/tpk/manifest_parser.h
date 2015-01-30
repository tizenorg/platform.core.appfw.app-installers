/* Copyright 2015 Samsung Electronics, license APACHE-2.0, see LICENSE file */
#ifndef TPK_MANIFEST_PARSER_H_
#define TPK_MANIFEST_PARSER_H_

#include <vector>
#include "boost/filesystem.hpp"
#include "tpk/xml_nodes.h"
#include "tpk/exception.h"


namespace tpk {

/* Internal exceptions */
class FileOpenFailureException : public Exception {};
class ParseFailureException : public Exception {};


class ManifestParser {
 public:
  explicit ManifestParser(const char *manifestFilePath);
  ~ManifestParser();
  XmlNodeManifest &GetManifest();
  XmlNodeManifest manifest;


 private:
  template <typename T> void CheckAndSetNode(
      xmlTextReaderPtr reader, const char* name, const T &xmlNode);
  template <typename T> void CheckAndAppendNode(
      xmlTextReaderPtr reader, const char* name, const std::vector<T*> &v);
  void ProcessNode(xmlTextReaderPtr reader);
  void StreamFile(const char* filePath);
};

}  // namespace tpk
#endif  // TPK_MANIFEST_PARSER_H_
