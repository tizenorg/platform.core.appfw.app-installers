/* Copyright 2015 Samsung Electronics, license APACHE-2.0, see LICENSE file */

#include "tpk/xml_parser/xml_parser.h"
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlstring.h>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

using std::map;
using std::string;
using std::vector;

namespace xml_parser {

namespace {  // static namespace

string xmlChar2string(const xmlChar *str) {
  return string(const_cast<char*>(reinterpret_cast<const char*>(str)));
}

}  // namespace


// class XmlElement
XmlElement::XmlElement(xmlNode *node) {
  name_ = xmlChar2string(node->name);
  xmlChar* content = xmlNodeGetContent(node);  // NOTE: Needs to be freed
  content_ = xmlChar2string(content);
  xmlFree(content);
  SetAttrMap(node);
  node_ = node;
}

XmlElement::~XmlElement() {
  attr_map_.clear();
}

const string& XmlElement::attr(const string& attrName) {
  map<string, string>::iterator it = attr_map_.find(attrName);
  if (it != attr_map_.end()) {
    return it->second;
  }
  return XmlElement::null_string();
}

void XmlElement::SetAttrMap(xmlNode* node) {
  xmlAttr* attr;
  const xmlChar* name;
  xmlChar* value;

  for (attr=node->properties; attr != nullptr; attr=attr->next) {
    name = attr->name;
    value = xmlGetProp(node, name);  // NOTE: Needs to be freed

    attr_map_.insert(map<string, string>::value_type(
          xmlChar2string(name), xmlChar2string(value)));

    xmlFree(value);
  }
}

const string& XmlElement::null_string() {
  // NOTE: check-coding-rule says that const/static string is to be changed to
  // const char[], but here we need a string object reference, instead of
  // C-style const char.
  static const string nullstr = string("");
  return nullstr;
}


// class XmlTree
XmlTree::XmlTree(xmlDoc* doc) : root_(nullptr), doc_(doc) {
}

XmlTree::~XmlTree() {
  xmlFreeDoc(doc_);

  // Clear element_map_
  for (auto& it : elements_map_) {
    delete it.second;
  }
  elements_map_.clear();
}

void XmlTree::StoreElement(xmlNode* node, XmlElement *el) {
  elements_map_.insert(std::pair<xmlNode*, XmlElement*>(node, el));
}

XmlElement* XmlTree::FindElement(xmlNode* node) {
  map<xmlNode*, XmlElement*>::iterator it = elements_map_.find(node);
  if (it != elements_map_.end()) {
    return it->second;
  }
  return nullptr;
}

XmlElement* XmlTree::GetRootElement() {
  if (root_) return root_;

  xmlNode* root_node = xmlDocGetRootElement(doc_);
  if (root_node == nullptr) {
    return nullptr;
  }

  // Get information of the node
  XmlElement* root = new XmlElement(root_node);
  StoreElement(root_node, root);
  root_ = root;

  return root_;
}

vector<XmlElement*> XmlTree::Children(
    XmlElement* parent,
    const std::string& name) {
  xmlNode* child;
  XmlElement *el;
  vector<XmlElement*> children;

  for (child = xmlFirstElementChild(parent->node());
      nullptr != child;
      child = xmlNextElementSibling(child)) {
    if (name == xmlChar2string(child->name)) {
      // Found a matched node
      // Search map
      el = FindElement(child);
      if (!el) {  // Not in elements_map_
        el = new XmlElement(child);
        StoreElement(child, el);
      }
      // Add to children
      children.push_back(el);
    }
  }
  return children;
}



// class XmlParser
XmlParser::XmlParser() {
}

XmlParser::~XmlParser() {
  // NOTE: These libxml2 cleanup funcs must be called after all libxml2 funcs
  // are compelte.

  // xmlCleanupParser();
  // xmlMemoryDump();
}

XmlTree* XmlParser::ParseAndGetNewTree(const char* xmlFilePath) {
  LIBXML_TEST_VERSION

  XmlTree* t = nullptr;
  xmlDocPtr doc = xmlReadFile(xmlFilePath, nullptr, 0);
  if (doc) {
    t = new XmlTree(doc);
  } else {
    // TODO(youmin.ha@samsung.com): Error log
  }
  return t;
  // NOTE: doc will be freed by XmlTree.
}

}  // namespace xml_parser
