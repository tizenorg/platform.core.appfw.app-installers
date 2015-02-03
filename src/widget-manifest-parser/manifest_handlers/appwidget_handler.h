// Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WIDGET_MANIFEST_PARSER_MANIFEST_HANDLERS_APPWIDGET_HANDLER_H_
#define WIDGET_MANIFEST_PARSER_MANIFEST_HANDLERS_APPWIDGET_HANDLER_H_

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "widget-manifest-parser/application_data.h"
#include "widget-manifest-parser/manifest_handler.h"

namespace common_installer {
namespace widget_manifest_parser {

typedef std::map<std::string, std::string> AppWidgetLabelLangValueMap;

struct AppWidgetLabel {
  // may be empty
  std::string default_value;

  // may be empty if default is set
  AppWidgetLabelLangValueMap lang_value_map;
};

enum AppWidgetSizeType {
  k1x1, k2x1, k2x2
};

struct AppWidgetSize {
  // mandatory
  AppWidgetSizeType type;

  // optional, relative to web app directory
  std::string preview;

  // optional, default: true
  bool use_decoration;
};

typedef std::vector<AppWidgetSize> AppWidgetSizeVector;

struct AppWidgetDropView {
  // mandatory, relative to web app directory or remote URL
  std::string src;

  // mandatory
  int width;

  // mandatory, <1, 380>
  int height;
};

typedef std::vector<AppWidgetDropView> AppWidgetDropViewVector;

struct AppWidget {
  // mandatory, unique, must start with application id and end with label
  // separated with dot, the label can contain only 0-9, a-z, A-Z
  std::string id;

  // mandatory, if 2 or more app widgets have the primary attribute set to true,
  // the default icon and title of the parent web app can be used
  bool primary;

  // optional(0-1), min: 1800.0, default: no update
  std::vector<double> update_period;

  // optional, default: false
  bool auto_launch;

  // box label, multiple(1+)
  AppWidgetLabel label;

  // box icon, optional(0-1), src, mandatory, relative to web app directory
  std::string icon_src;

  // box content, mandatory(1) -[

  // mandatory, relative to web app directory or remote URL
  std::string content_src;

  // optional, default: false
  bool content_mouse_event;

  // optional, default: true
  bool content_touch_effect;

  // box size, mandatory(1-3), 1x1 must exist
  AppWidgetSizeVector content_size;

  // drop view, optional(0-1)
  AppWidgetDropViewVector content_drop_view;

  // ]- box content
};

typedef std::vector<AppWidget> AppWidgetVector;

class AppWidgetInfo : public ApplicationData::ManifestData {
 public:
  explicit AppWidgetInfo(const AppWidgetVector& app_widgets);
  virtual ~AppWidgetInfo();

  const AppWidgetVector& app_widgets() const {
    return app_widgets_;
  }

 private:
  // multiple(0+)
  AppWidgetVector app_widgets_;
};

class AppWidgetHandler : public ManifestHandler {
 public:
  AppWidgetHandler();
  virtual ~AppWidgetHandler();

  bool Parse(std::shared_ptr<ApplicationData> application,
             std::string* error) override;
  bool Validate(std::shared_ptr<const ApplicationData> application,
                std::string* error) const override;
  std::vector<std::string> Keys() const override;

 private:
  DISALLOW_COPY_AND_ASSIGN(AppWidgetHandler);
};

}  // namespace widget_manifest_parser
}  // namespace common_installer

#endif  // WIDGET_MANIFEST_PARSER_MANIFEST_HANDLERS_APPWIDGET_HANDLER_H_
