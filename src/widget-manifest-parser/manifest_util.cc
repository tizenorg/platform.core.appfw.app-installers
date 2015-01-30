// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.
// Part of this file is redistributed from crosswalk project under BSD-style
// license. Check LICENSE-xwalk file.

#include "widget-manifest-parser/manifest_util.h"

#include <boost/filesystem/operations.hpp>
#include <libxml2/libxml/tree.h>

#include <algorithm>
#include <cassert>
#include <map>
#include <vector>

#include "utils/string_util.h"
#include "utils/values.h"
#include "widget-manifest-parser/application_data.h"
#include "widget-manifest-parser/application_manifest_constants.h"
#include "widget-manifest-parser/manifest.h"
#include "widget-manifest-parser/manifest_handler.h"

namespace errors = common_installer::application_manifest_errors;
namespace keys = common_installer::application_manifest_keys;
namespace widget_keys = common_installer::application_widget_keys;

namespace bf = boost::filesystem;

namespace {
const char kAttributePrefix[] = "@";
const char kNamespaceKey[] = "@namespace";
const char kTextKey[] = "#text";

const char kContentKey[] = "content";

const xmlChar kWidgetNodeKey[] = "widget";
const xmlChar kNameNodeKey[] = "name";
const xmlChar kDescriptionNodeKey[] = "description";
const xmlChar kAuthorNodeKey[] = "author";
const xmlChar kLicenseNodeKey[] = "license";
const xmlChar kIconNodeKey[] = "icon";

const xmlChar kVersionAttributeKey[] = "version";
const xmlChar kShortAttributeKey[] = "short";
const xmlChar kDirAttributeKey[] = "dir";
const xmlChar kEmailAttributeKey[] = "email";
const xmlChar kHrefAttributeKey[] = "href";
const xmlChar kIdAttributeKey[] = "id";
const xmlChar kDefaultLocaleAttributeKey[] = "defaultlocale";
const xmlChar kPathAttributeKey[] = "path";

const char* kSingletonElements[] = {
  "allow-navigation",
  "author",
  "content-security-policy-report-only",
  "content-security-policy",
  "content"
};

std::string GetNodeDir(xmlNode* node, const std::string& inherit_dir) {
  assert(node);
  std::string dir(inherit_dir);

  xmlAttr* prop = nullptr;
  for (prop = node->properties; prop; prop = prop->next) {
    if (xmlStrEqual(prop->name, kDirAttributeKey)) {
      char* prop_value = reinterpret_cast<char *>(xmlNodeListGetString(
          node->doc, prop->children, 1));
      dir = prop_value;
      xmlFree(prop_value);
      break;
    }
  }
  return dir;
}

std::string GetNodeText(xmlNode* root, const std::string& inherit_dir) {
  assert(root);
  if (root->type != XML_ELEMENT_NODE)
    return std::string();

  std::string current_dir(GetNodeDir(root, inherit_dir));
  std::string text;
  for (xmlNode* node = root->children; node; node = node->next) {
// TODO(jizydorczyk):
// i18n support is needed
    if (node->type == XML_TEXT_NODE || node->type == XML_CDATA_SECTION_NODE) {
      text = text
          + common_installer::utils::StripWrappingBidiControlCharactersUTF8(
          std::string(reinterpret_cast<char*>(node->content)));
    } else {
      text = text + GetNodeText(node, current_dir);
    }
  }
  return common_installer::utils::GetDirTextUTF8(text, current_dir);
}

// According to widget specification, this two prop need to support dir.
// see detail on http://www.w3.org/TR/widgets/#the-dir-attribute
inline bool IsPropSupportDir(xmlNode* root, xmlAttr* prop) {
  if (xmlStrEqual(root->name, kWidgetNodeKey)
     && xmlStrEqual(prop->name, kVersionAttributeKey))
    return true;
  if (xmlStrEqual(root->name, kNameNodeKey)
     && xmlStrEqual(prop->name, kShortAttributeKey))
    return true;
  return false;
}

// Only this four items need to support span and ignore other element.
// Besides xmlNodeListGetString can not support dir prop of span.
// See http://www.w3.org/TR/widgets/#the-span-element-and-its-attributes
inline bool IsElementSupportSpanAndDir(xmlNode* root) {
  if (xmlStrEqual(root->name, kNameNodeKey)
     || xmlStrEqual(root->name, kDescriptionNodeKey)
     || xmlStrEqual(root->name, kAuthorNodeKey)
     || xmlStrEqual(root->name, kLicenseNodeKey))
    return true;
  return false;
}

bool IsSingletonElement(const std::string& name) {
  for (const char* str : kSingletonElements)
      if (name == str)
        return name != kContentKey;
  return false;
}

// According to spec 'name' and 'author' should be result of applying the rule
// for getting text content with normalized white space to this element.
// http://www.w3.org/TR/widgets/#rule-for-getting-text-content-with-normalized-white-space-0
inline bool IsTrimRequiredForElement(xmlNode* root) {
  if (xmlStrEqual(root->name, kNameNodeKey) ||
      xmlStrEqual(root->name, kAuthorNodeKey)) {
    return true;
  }
  return false;
}

// According to spec some attributes requaire applying the rule for getting
// a single attribute value.
// http://www.w3.org/TR/widgets/#rule-for-getting-a-single-attribute-value-0
inline bool IsTrimRequiredForProp(xmlNode* root, xmlAttr* prop) {
  if (xmlStrEqual(root->name, kWidgetNodeKey) &&
      (xmlStrEqual(prop->name, kIdAttributeKey) ||
      xmlStrEqual(prop->name, kVersionAttributeKey) ||
      xmlStrEqual(prop->name, kDefaultLocaleAttributeKey))) {
    return true;
  }
  if (xmlStrEqual(root->name, kNameNodeKey) &&
      xmlStrEqual(prop->name, kShortAttributeKey)) {
    return true;
  }
  if (xmlStrEqual(root->name, kAuthorNodeKey) &&
      (xmlStrEqual(prop->name, kEmailAttributeKey) ||
      xmlStrEqual(prop->name, kHrefAttributeKey))) {
    return true;
  }
  if (xmlStrEqual(root->name, kLicenseNodeKey) &&
      xmlStrEqual(prop->name, kHrefAttributeKey)) {
    return true;
  }
  if (xmlStrEqual(root->name, kIconNodeKey) &&
      xmlStrEqual(prop->name, kPathAttributeKey)) {
    return true;
  }
  return false;
}

}  // namespace

namespace common_installer {
namespace widget_manifest_parser {

namespace {

// Load XML node into Dictionary structure.
// The keys for the XML node to Dictionary mapping are described below:
// XML                                 Dictionary
// <e></e>                             "e":{"#text": ""}
// <e>textA</e>                        "e":{"#text":"textA"}
// <e attr="val">textA</e>             "e":{ "@attr":"val", "#text": "textA"}
// <e> <a>textA</a> <b>textB</b> </e>  "e":{
//                                       "a":{"#text":"textA"}
//                                       "b":{"#text":"textB"}
//                                     }
// <e> <a>textX</a> <a>textY</a> </e>  "e":{
//                                       "a":[ {"#text":"textX"},
//                                             {"#text":"textY"}]
//                                     }
// <e> textX <a>textY</a> </e>         "e":{ "#text":"textX",
//                                           "a":{"#text":"textY"}
//                                     }
//
// For elements that are specified under a namespace, the dictionary
// will add '@namespace' key for them, e.g.,
// XML:
// <e xmln="linkA" xmlns:N="LinkB">
//   <sub-e1> text1 </sub-e>
//   <N:sub-e2 text2 />
// </e>
// will be saved in Dictionary as,
// "e":{
//   "#text": "",
//   "@namespace": "linkA"
//   "sub-e1": {
//     "#text": "text1",
//     "@namespace": "linkA"
//   },
//   "sub-e2": {
//     "#text":"text2"
//     "@namespace": "linkB"
//   }
// }

// converting dictionaryvalue to std::map<
// std::string*, std::map<std::string*,std::string*>> or
// std::map<std::string*, std::map<std::string*,std::string>>

std::unique_ptr<utils::DictionaryValue> LoadXMLNode(
    xmlNode* root, const std::string& inherit_dir = "") {
  std::unique_ptr<utils::DictionaryValue> value(new utils::DictionaryValue());
  if (root->type != XML_ELEMENT_NODE)
    return nullptr;

  std::string current_dir(GetNodeDir(root, inherit_dir));

  xmlAttr* prop = nullptr;
  for (prop = root->properties; prop; prop = prop->next) {
    xmlChar* value_ptr = xmlNodeListGetString(root->doc, prop->children, 1);
    std::string prop_value(reinterpret_cast<char*>(value_ptr));
    xmlFree(value_ptr);

    if (IsPropSupportDir(root, prop))
      prop_value = utils::GetDirTextUTF8(prop_value, current_dir);

    if (IsTrimRequiredForProp(root, prop))
      prop_value = utils::CollapseWhitespaceUTF8(prop_value, false);

    value->SetString(
        std::string(kAttributePrefix)
        + reinterpret_cast<const char*>(prop->name),
        prop_value);
  }

  if (root->ns)
    value->SetString(kNamespaceKey,
        reinterpret_cast<const char*>(root->ns->href));

  for (xmlNode* node = root->children; node; node = node->next) {
    std::string sub_node_name(reinterpret_cast<const char*>(node->name));
    std::unique_ptr<utils::DictionaryValue> sub_value =
        LoadXMLNode(node, current_dir);
    if (!sub_value)
      continue;

    if (!value->HasKey(sub_node_name)) {
      value->Set(sub_node_name, sub_value.release());
      continue;
    } else if (IsSingletonElement(sub_node_name)) {
      continue;
    } else if (sub_node_name == kContentKey) {
      std::string current_namespace, new_namespace;
      utils::DictionaryValue* current_value;
      value->GetDictionary(sub_node_name, &current_value);

      current_value->GetString(kNamespaceKey, &current_namespace);
      sub_value->GetString(kNamespaceKey, &new_namespace);
      if (current_namespace != new_namespace &&
          new_namespace == widget_keys::kTizenNamespacePrefix)
        value->Set(sub_node_name, sub_value.release());
      continue;
    }

    utils::Value* temp;
    value->Get(sub_node_name, &temp);
    assert(temp);

    if (temp->IsType(utils::Value::TYPE_LIST)) {
      utils::ListValue* list;
      temp->GetAsList(&list);
      list->Append(sub_value.release());
    } else {
      assert(temp->IsType(utils::Value::TYPE_DICTIONARY));
      utils::DictionaryValue* dict;
      temp->GetAsDictionary(&dict);
      utils::DictionaryValue* prev_value = dict->DeepCopy();

      utils::ListValue* list = new utils::ListValue();
      list->Append(prev_value);
      list->Append(sub_value.release());
      value->Set(sub_node_name, list);
    }
  }

  std::string text;
  if (IsElementSupportSpanAndDir(root)) {
    text = GetNodeText(root, current_dir);
  } else {
    xmlChar* text_ptr = xmlNodeListGetString(root->doc, root->children, 1);
    if (text_ptr) {
      text = reinterpret_cast<char*>(text_ptr);
      xmlFree(text_ptr);
    }
  }

  if (IsTrimRequiredForElement(root))
    text = utils::CollapseWhitespaceUTF8(text, false);

  if (!text.empty())
    value->SetString(kTextKey, text);

  return value;
}

}  // namespace

std::unique_ptr<Manifest> LoadManifest(const std::string& manifest_path,
    Manifest::Type type, std::string* error) {
  xmlDoc * doc = nullptr;
  xmlNode* root_node = nullptr;
  doc = xmlReadFile(manifest_path.c_str(), nullptr, 0);
  if (!doc) {
    *error = errors::kManifestUnreadable;
    return nullptr;
  }
  root_node = xmlDocGetRootElement(doc);
  std::unique_ptr<utils::DictionaryValue> dv = LoadXMLNode(root_node);
  std::unique_ptr<utils::DictionaryValue> result(new utils::DictionaryValue);
  if (dv)
    result->Set(reinterpret_cast<const char*>(root_node->name), dv.release());

  return std::unique_ptr<Manifest>(
      new Manifest(std::move(result), Manifest::TYPE_WIDGET));
}

bf::path ApplicationURLToRelativeFilePath(const std::string& url) {
  std::string url_path = url;
  if (url_path.empty() || url_path[0] != '/')
    return bf::path();

  // TODO(jizydorczyk):
  // We need to unescappe %-encoded UTF8 chars here
  // for now its left undone
  // Drop the leading slashes and convert %-encoded UTF8 to regular UTF8.
//  std::string file_path = net::UnescapeURLComponent(url_path,
//      net::UnescapeRule::SPACES | net::UnescapeRule::URL_SPECIAL_CHARS);
//  size_t skip = file_path.find_first_not_of("/\\");
//  if (skip != file_path.npos)
//    file_path = file_path.substr(skip);

  std::string file_path = url_path;
  bf::path path(file_path);

  // It's still possible for someone to construct an annoying URL whose path
  // would still wind up not being considered relative at this point.
  // For example: app://id/c:////foo.html
  if (path.is_absolute())
    return bf::path();

  return path;
}

}  // namespace widget_manifest_parser
}  // namespace common_installer
