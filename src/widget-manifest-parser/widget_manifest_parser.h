// Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#ifndef WIDGET_MANIFEST_PARSER_WIDGET_MANIFEST_PARSER_H_
#define WIDGET_MANIFEST_PARSER_WIDGET_MANIFEST_PARSER_H_

#include <pkgmgr/pkgmgr_parser.h>

#include <memory>
#include <set>
#include <string>
#include <vector>

#include <widget-manifest-parser/manifest.h>
#include <widget-manifest-parser/application_data.h>

namespace common_installer {
namespace widget_manifest_parser {

class WidgetManifestParser {
 public:
  explicit WidgetManifestParser();
  ~WidgetManifestParser();

  const std::string& GetErrorMessage();

  // Parses manifest file to manfiest_ object
  bool ParseManifest(const bf::path& path);
  // Fills manifest_x structure with data read from config.xml
  bool SetManifestX(manifest_x* manifest);

  const std::string& GetShortName() const;
  const std::string& GetRequiredAPIVersion() const;

 private:
  bool ExtractApplicationInfo(const ApplicationData& app_data);
  bool ExtractWidgetInfo(const ApplicationData& app_data);
  bool ExtractIconSrc(const common_installer::utils::Value& dict,
      std::string* value, std::string* error);
  bool ExtractIcons(const Manifest& manifest, std::string* error);
  bool ExtractPrivileges(const ApplicationData& app_data, std::string* error);
  void FillManifestX();

  std::unique_ptr<Manifest> manifest_;
  bool valid_;

  std::string error_;

  std::string package_;
  std::string id_;
  std::string name_;
  std::string short_name_;
  std::string version_;
  std::vector<std::string> icons_;
  std::string api_version_;
  std::set<std::string> privileges_;
  manifest_x* manifest_x_;
};

}  // namespace widget_manifest_parser
}  // namespace common_installer

#endif  // WIDGET_MANIFEST_PARSER_WIDGET_MANIFEST_PARSER_H_
