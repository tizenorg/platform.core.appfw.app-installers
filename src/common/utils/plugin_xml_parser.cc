// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/utils/plugin_xml_parser.h"

namespace common_installer {

bool PluginsXmlParser::Parse() {
  if (path_.empty()) {
    return false;
  }

  WrapperXMLReader objReader;

  xmlTextReaderPtr reader = objReader.Create(path_.c_str());

  if (!reader) {
    return false;
  }

  tags_.clear();

  int depth = xmlTextReaderDepth(reader);

  while ((NextChildElement(reader, depth))) {
    const xmlChar* node = xmlTextReaderConstName(reader);

    if (!node) {
      return false;
    }

    tags_.push_back(std::string((const char*)node));
  }

  // get here, after end of a "reading" file
  docPtr_ = xmlTextReaderCurrentDoc(reader);

  if (!docPtr_) {
    return false;
  }

  return true;
}

std::vector<std::string>& PluginsXmlParser::Data() { return tags_; }

xmlDocPtr PluginsXmlParser::DocPtr() { return docPtr_; }

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
  if (docPtr_) {
    xmlFreeDoc(docPtr_);
  }
}

}  // namespace common_installer
