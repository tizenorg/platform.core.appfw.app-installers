// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#ifndef PARSER_MANIFEST_CONSTANTS_H_
#define PARSER_MANIFEST_CONSTANTS_H_

namespace parser {
  const char kTizenNamespacePrefix[] = "http://tizen.org/ns/widgets";

  const char kLocaleUnlocalized[] = "@unlocalized";
  const char kLocaleAuto[] = "en-gb";
  const char kLocaleFirstOne[] = "*";

  const char kWidgetNamePath[] = "widget.name";
  const char kWidgetDecriptionPath[] = "widget.description";
  const char kWidgetLicensePath[] = "widget.license";

  // Manifest
  const char kWidgetKey[] = "widget";
  const char kDefaultLocaleKey[] = "widget.@defaultlocale";
  const char kXmlLangKey[] = "@lang";
}  // namespace parser

namespace application_manifest_errors {
const char kInvalidDescription[] =
    "Invalid value for 'description'.";
const char kInvalidKey[] =
    "Value 'key' is missing or invalid.";
const char kInvalidName[] =
    "Required value 'name' is missing or invalid.";
const char kManifestParseError[] =
    "Manifest is not valid XML.";
const char kManifestUnreadable[] =
    "Manifest file is missing or unreadable.";

const char kErrMsgNoPath[] =
    "Path not specified.";
const char kErrMsgInvalidPath[] =
    "Invalid path.";
const char kErrMsgValueNotFound[] =
    "Value not found. Value name: ";
const char kErrMsgInvalidDictionary[] =
    "Cannot get key value as a dictionary. Key name: ";
const char kErrMsgInvalidList[] =
    "Cannot get key value as a list. Key name: ";
const char kErrMsgNoMandatoryKey[] =
    "Cannot find mandatory key. Key name: ";
}  // namespace application_manifest_errors

#endif  // PARSER_MANIFEST_CONSTANTS_H_
