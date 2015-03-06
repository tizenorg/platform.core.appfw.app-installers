// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#ifndef WIDGET_MANIFEST_PARSER_MANIFEST_HANDLER_H_
#define WIDGET_MANIFEST_PARSER_MANIFEST_HANDLER_H_

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "widget-manifest-parser/manifest.h"
#include "widget-manifest-parser/application_data.h"

namespace common_installer {
namespace widget_manifest_parser {

class ManifestHandler {
 public:
  virtual ~ManifestHandler();

  // Returns false in case of failure and sets writes error message
  // in |error| if present.
  virtual bool Parse(std::shared_ptr<ApplicationData> application,
                     std::string* error) = 0;

  // Returns false in case of failure and sets writes error message
  // in |error| if present.
  virtual bool Validate(std::shared_ptr<const ApplicationData> application,
                        std::string* error) const;

  // Same as AlwaysParseForType, but for Validate instead of Parse.
  virtual bool AlwaysValidateForType() const;

  // The list of keys that, if present, should be parsed before calling our
  // Parse (typically, because our Parse needs to read those keys).
  // Defaults to empty.
  virtual std::vector<std::string> PrerequisiteKeys() const;

  // The keys to register handler for (in Register).
  virtual std::vector<std::string> Keys() const = 0;
};

class ManifestHandlerRegistry final {
 public:
  ~ManifestHandlerRegistry();

  static ManifestHandlerRegistry* GetInstance();

  bool ParseAppManifest(
      std::shared_ptr<ApplicationData> application, std::string* error);
  bool ValidateAppManifest(std::shared_ptr<const ApplicationData> application,
      std::string* error);

 private:
  friend class ScopedTestingManifestHandlerRegistry;
  explicit ManifestHandlerRegistry(
      const std::vector<ManifestHandler*>& handlers);

  // Register a manifest handler for keys, which are provided by Keys() method
  // in ManifestHandler implementer.
  void Register(ManifestHandler* handler);

  void ReorderHandlersGivenDependencies();

  // Sets a new global registry, for testing purposes.
  static void SetInstanceForTesting(ManifestHandlerRegistry* registry);

  static ManifestHandlerRegistry* GetInstanceForWGT();

  typedef std::map<std::string, ManifestHandler*> ManifestHandlerMap;
  typedef std::map<ManifestHandler*, int> ManifestHandlerOrderMap;

  ManifestHandlerMap handlers_;

  // Handlers are executed in order; lowest order first.
  ManifestHandlerOrderMap order_map_;

  static ManifestHandlerRegistry* widget_registry_;
};

}  // namespace widget_manifest_parser
}  // namespace common_installer

#endif  // WIDGET_MANIFEST_PARSER_MANIFEST_HANDLER_H_
