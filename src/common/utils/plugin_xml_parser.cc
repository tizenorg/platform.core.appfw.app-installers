// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "manifest_parser/utils/logging.h"
#include "common/utils/plugin_xml_parser.h"

namespace common_installer {

bool PluginsXmlParser::Parse() {
  if (path_.empty()) {
    return false;
  }

  WrapperXMLReader obj_reader;

  xmlTextReaderPtr reader = obj_reader.Create(path_.c_str());

  if (!reader) {
    return false;
  }

  LOG(INFO) << "Xml path: " << path_;

  tags_.clear();

  int depth = xmlTextReaderDepth(reader);

  // use set to remove duplicate
  std::set<std::string> tags;

  while ((NextChildElement(reader, depth))) {
    const xmlChar* node = xmlTextReaderConstName(reader);

    if (!node) {
      return false;
    }

    tags.insert(std::string(reinterpret_cast<const char*>(node)));
  }

  tags_.assign(tags.begin(), tags.end());

  // get here, after end of a "reading" file
  doc_ptr_ = xmlTextReaderCurrentDoc(reader);

  if (!doc_ptr_) {
    return false;
  }

  return true;
}

const std::vector<std::string>& PluginsXmlParser::tags_list() { return tags_; }

xmlDocPtr PluginsXmlParser::doc_ptr() { return doc_ptr_; }

int PluginsXmlParser::NextChildElement(xmlTextReaderPtr reader, int depth) {
  int ret = xmlTextReaderRead(reader);
  int cur = xmlTextReaderDepth(reader);

  while (ret == 1) {
    switch (xmlTextReaderNodeType(reader)) {
      case XML_READER_TYPE_ELEMENT:
        if (cur == depth + 1) return 1;
        break;
      default:

        if (cur <= depth) return 0;
        break;
    }

    ret = xmlTextReaderRead(reader);
    cur = xmlTextReaderDepth(reader);
  }

  return ret;
}

PluginsXmlParser::~PluginsXmlParser() {
  if (doc_ptr_) {
    xmlFreeDoc(doc_ptr_);
  }
}

}  // namespace common_installer
