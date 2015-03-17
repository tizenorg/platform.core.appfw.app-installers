// Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#ifndef PARSER_MANIFEST_PARSER_H_
#define PARSER_MANIFEST_PARSER_H_

#include <boost/filesystem/path.hpp>
#include <pkgmgr/pkgmgr_parser.h>

#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "parser/manifest.h"
#include "parser/manifest_handler.h"

namespace parser {

class ManifestParser {
 public:
  // This class should take ownership of ManifestHandlerRegistry
  explicit ManifestParser(std::unique_ptr<ManifestHandlerRegistry> registry);
  ~ManifestParser();

  const std::string& GetErrorMessage();

  // Parses manifest file to manfiest_ object
  bool ParseManifest(const boost::filesystem::path& path);

  std::shared_ptr<const ManifestData> GetManifestData(std::string key);

 private:
  bool ParseManifestData(std::string* error);

  void SetManifestData(const std::string& key,
      std::shared_ptr<ManifestData> data);

  // Uses manifest handlers for parsing ManifestData stucts
  bool ParseAppManifest(std::string* error);
  // Uses manifest handlers for validating ManifestData stucts
  bool ValidateAppManifest(std::string* error);

  std::unique_ptr<ManifestHandlerRegistry> registry_;
  std::shared_ptr<Manifest> manifest_;
  ManifestDataMap manifest_data_;
  bool valid_;

  std::string error_;

  // For testing
  friend class ServiceHandlerTest;
};

}  // namespace parser

#endif  // PARSER_MANIFEST_PARSER_H_
