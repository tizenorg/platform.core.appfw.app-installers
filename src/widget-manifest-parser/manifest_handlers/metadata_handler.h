// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#ifndef WIDGET_MANIFEST_PARSER_MANIFEST_HANDLERS_METADATA_HANDLER_H_
#define WIDGET_MANIFEST_PARSER_MANIFEST_HANDLERS_METADATA_HANDLER_H_

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "utils/values.h"
#include "widget-manifest-parser/application_data.h"
#include "widget-manifest-parser/manifest_handler.h"

namespace common_installer {
namespace widget_manifest_parser {

class MetaDataInfo : public ApplicationData::ManifestData {
 public:
  MetaDataInfo();
  virtual ~MetaDataInfo();

  bool HasKey(const std::string& key) const;
  std::string GetValue(const std::string& key) const;
  void SetValue(const std::string& key, const std::string& value);
  const std::map<std::string, std::string>& metadata() const {
    return metadata_;
  }

 private:
  std::map<std::string, std::string> metadata_;
};

class MetaDataHandler : public ManifestHandler {
 public:
  MetaDataHandler();
  virtual ~MetaDataHandler();

  bool Parse(std::shared_ptr<ApplicationData> application,
             std::string* error) override;
  std::vector<std::string> Keys() const override;

 private:
  DISALLOW_COPY_AND_ASSIGN(MetaDataHandler);
};

}  // namespace widget_manifest_parser
}  // namespace common_installer

#endif  // WIDGET_MANIFEST_PARSER_MANIFEST_HANDLERS_METADATA_HANDLER_H_
