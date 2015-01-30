/* Copyright 2015 Samsung Electronics, license APACHE-2.0, see LICENSE file */
#ifndef TPK_MANIFESTPARSER_H_
#define TPK_MANIFESTPARSER_H_

#include <exception>
#include <vector>
#include "boost/filesystem.hpp"
#include "tpk/XmlNodes.h"


namespace tpk {

/* Internal exceptions */
class FileOpenFailureException : public std::exception {};
class ParseFailureException : public std::exception {};


class ManifestParser {
 public:
  explicit ManifestParser(const char *manifestFilePath);
  ~ManifestParser();
  XmlNodeManifest &getManifest();
  XmlNodeManifest manifest;


 private:
  template <typename T> void checkAndSetNode(
      xmlTextReaderPtr reader, const char* name, const T &xmlNode);
  template <typename T> void checkAndAppendNode(
      xmlTextReaderPtr reader, const char* name, const std::vector<T*> &v);
  void processNode(xmlTextReaderPtr reader);
  void streamFile(const char* filePath);
};

}  // namespace tpk
#endif  // TPK_MANIFESTPARSER_H_
