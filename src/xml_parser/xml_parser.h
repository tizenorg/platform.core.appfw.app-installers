/* Copyright 2015 Samsung Electronics, license APACHE-2.0, see LICENSE file */
#ifndef XML_PARSER_XML_PARSER_H_
#define XML_PARSER_XML_PARSER_H_

#include <libxml/tree.h>
#include <map>
#include <string>
#include <vector>

namespace xml_parser {

class XmlElement {
 public:
  static const std::string& null_string();

  explicit XmlElement(xmlNode *node);
  ~XmlElement();
  const std::string& name() { return name_; }
  const std::string& content() { return content_; }
  const std::string& attr(const std::string& attrName);
  xmlNode* node() { return node_; }

 private:
  void SetAttrMap(xmlNode* node);

  std::string name_;
  std::string content_;
  std::map<std::string, std::string> attr_map_;
  xmlNode* node_;
};

class XmlTree {
 public:
  explicit XmlTree(xmlDoc* doc);
  ~XmlTree();

  // Note: Each XmlElement* is available when the XmlTree object is valid.
  XmlElement* GetRootElement();
  std::vector<XmlElement*> Children(XmlElement* parent,
      const std::string& name = "");

 private:
  void StoreElement(xmlNode* node, XmlElement *el);
  XmlElement* FindElement(xmlNode* node);

  std::map<xmlNode*, XmlElement*> elements_map_;
  XmlElement* root_;
  xmlDoc* doc_;
};


class XmlParser {
 public:
  XmlParser();
  ~XmlParser();
  XmlTree* ParseAndGetNewTree(const char* xmlFilePath = "");

 private:
};

}  // namespace xml_parser

#endif  // XML_PARSER_XML_PARSER_H_
