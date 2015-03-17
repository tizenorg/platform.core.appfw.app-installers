// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#ifndef MANIFEST_HANDLERS_METADATA_HANDLER_H_
#define MANIFEST_HANDLERS_METADATA_HANDLER_H_

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "parser/values.h"
#include "parser/manifest_handler.h"

namespace wgt {
namespace parse {

class MetaDataInfo : public parser::ManifestData {
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

class MetaDataHandler : public parser::ManifestHandler {
 public:
  MetaDataHandler();
  virtual ~MetaDataHandler();

  bool Parse(
      const parser::Manifest& manifest,
      std::shared_ptr<parser::ManifestData>* output,
      std::string* error) override;
  bool Validate(
      const parser::ManifestData& data,
      const parser::ManifestDataMap& handlers_output,
      std::string* error) const override;
  std::string Key() const override;

 private:
  DISALLOW_COPY_AND_ASSIGN(MetaDataHandler);
};

}  // namespace parse
}  // namespace wgt

#endif  // MANIFEST_HANDLERS_METADATA_HANDLER_H_
