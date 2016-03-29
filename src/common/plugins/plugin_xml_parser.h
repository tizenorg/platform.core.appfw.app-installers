// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_PLUGINS_PLUGIN_XML_PARSER_H_
#define COMMON_PLUGINS_PLUGIN_XML_PARSER_H_

#include <libxml2/libxml/xmlreader.h>

#include <string>
#include <vector>
#include <set>

namespace common_installer {

/** this class parse xml file*/
class PluginsXmlParser {
 public:
  __attribute__ ((visibility ("default"))) explicit PluginsXmlParser(const std::string& path)
      : path_(path), doc_ptr_(nullptr) {}
  __attribute__ ((visibility ("default"))) ~PluginsXmlParser();
  __attribute__ ((visibility ("default"))) bool Parse();
  __attribute__ ((visibility ("default"))) const std::vector<std::string>& tags_list();
  __attribute__ ((visibility ("default"))) xmlDocPtr doc_ptr();

 private:
  const std::string path_;
  xmlDocPtr doc_ptr_;
  std::vector<std::string> tags_;
  __attribute__ ((visibility ("default"))) int NextChildElement(xmlTextReaderPtr reader, int depth);

  class WrapperXMLReader {
   public:
    __attribute__ ((visibility ("default"))) WrapperXMLReader() : reader_(nullptr) {}

    __attribute__ ((visibility ("default"))) xmlTextReaderPtr Create(const xmlDocPtr doc_ptr_) {
      reader_ = xmlReaderWalker(doc_ptr_);
      return reader_;
    }
    __attribute__ ((visibility ("default"))) virtual ~WrapperXMLReader() {
      if (reader_) {
        xmlFreeTextReader(reader_);
      }
    }

   private:
    xmlTextReaderPtr reader_;
  };
};
}  // namespace common_installer
#endif  // COMMON_PLUGINS_PLUGIN_XML_PARSER_H_
