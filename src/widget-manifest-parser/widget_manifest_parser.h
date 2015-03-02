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

#include "widget-manifest-parser/app_control_info.h"
#include "widget-manifest-parser/application_data.h"
#include "widget-manifest-parser/manifest.h"
#include "widget-manifest-parser/manifest_handlers/tizen_application_handler.h"
#include "widget-manifest-parser/manifest_handlers/widget_handler.h"

namespace common_installer {
namespace widget_manifest_parser {

class WidgetManifestParser {
 public:
  WidgetManifestParser();
  ~WidgetManifestParser();

  const std::string& GetErrorMessage();

  // Parses manifest file to manfiest_ object
  bool ParseManifest(const bf::path& path);
  // Fills manifest_x structure with data read from config.xml
  bool FillManifestX(manifest_x* manifest);

  const std::string& GetShortName() const;
  const std::string& GetRequiredAPIVersion() const;

 private:
  TizenApplicationInfo* ExtractApplicationInfo(const ApplicationData& app_data);
  WidgetInfo* ExtractWidgetInfo(const ApplicationData& app_data);
  bool ExtractIconSrc(const common_installer::utils::Value& dict,
      std::string* value, std::string* error);
  std::vector<std::string> ExtractIcons(const Manifest& manifest,
                                        std::string* error);
  std::set<std::string> ExtractPrivileges(const ApplicationData& app_data,
                                          std::string* error);
  AppControlInfoList ExtractAppControls(const ApplicationData& app_data,
                                          std::string* error);

  std::unique_ptr<Manifest> manifest_;
  bool valid_;

  std::string error_;

  // Elements of config.xml listed below don't have equivalent in manifest_x
  // structure. They are members of this class to provide access to them in
  // parse step.
  std::string short_name_;
  std::string api_version_;
};

}  // namespace widget_manifest_parser
}  // namespace common_installer

#endif  // WIDGET_MANIFEST_PARSER_WIDGET_MANIFEST_PARSER_H_
