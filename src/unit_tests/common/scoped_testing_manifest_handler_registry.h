// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UNIT_TESTS_COMMON_SCOPED_TESTING_MANIFEST_HANDLER_REGISTRY_H_
#define UNIT_TESTS_COMMON_SCOPED_TESTING_MANIFEST_HANDLER_REGISTRY_H_

#include <vector>

#include "parser/manifest_handler.h"

namespace common_installer {

namespace parser {

class ScopedTestingManifestHandlerRegistry {
 public:
  ScopedTestingManifestHandlerRegistry(
      const std::vector<ManifestHandler*>& handlers);
  ~ScopedTestingManifestHandlerRegistry();

  ManifestHandlerRegistry* registry_;
  ManifestHandlerRegistry* prev_registry_;
};

}  // namespace parser

}  // namespace common_installer

#endif  // UNIT_TESTS_COMMON_SCOPED_TESTING_MANIFEST_HANDLER_REGISTRY_H_
