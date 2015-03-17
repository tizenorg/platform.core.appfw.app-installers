// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#ifndef MANIFEST_HANDLERS_APPLICATION_ICONS_HANDLER_H_
#define MANIFEST_HANDLERS_APPLICATION_ICONS_HANDLER_H_

#include <string>
#include <vector>

#include "manifest_handlers/application_manifest_constants.h"
#include "parser/manifest_handler.h"

namespace wgt {
namespace parse {

class ApplicationIconsInfo : public parser::ManifestData {
 public:
  std::vector<std::string> get_icon_paths() const;
  void add_icon_path(std::string icon_path);
 private:
  std::vector<std::string> icon_paths_;
};

class ApplicationIconsHandler : public parser::ManifestHandler {
 public:
  ApplicationIconsHandler();
  ~ApplicationIconsHandler();

  bool ExtractIconSrc(const parser::utils::Value& dict,
                      std::string* value, std::string* error);

  bool Parse(
      const parser::Manifest& manifest,
      std::shared_ptr<parser::ManifestData>* output,
      std::string* error) override;
  bool Validate(
      const parser::ManifestData& data,
      const parser::ManifestDataMap& handlers_output,
      std::string* error) const override;
  bool AlwaysParseForType() const override;
  std::string Key() const override;

 private:
  DISALLOW_COPY_AND_ASSIGN(ApplicationIconsHandler);
};

}  // namespace parse
}  // namespace wgt

#endif  // MANIFEST_HANDLERS_APPLICATION_ICONS_HANDLER_H_
