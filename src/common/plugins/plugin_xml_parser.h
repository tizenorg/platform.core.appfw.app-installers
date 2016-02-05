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
  explicit PluginsXmlParser(const std::string& path)
      : path_(path), doc_ptr_(nullptr) {}
  ~PluginsXmlParser();
  bool Parse();
  const std::vector<std::string>& tags_list();
  xmlDocPtr doc_ptr();

 private:
  const std::string path_;
  xmlDocPtr doc_ptr_;
  std::vector<std::string> tags_;
  int NextChildElement(xmlTextReaderPtr reader, int depth);

  class WrapperXMLReader {
   public:
    WrapperXMLReader() : reader_(nullptr) {}

    xmlTextReaderPtr Create(const xmlDocPtr doc_ptr_) {
      reader_ = xmlReaderWalker(doc_ptr_);
      return reader_;
    }
    virtual ~WrapperXMLReader() {
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
