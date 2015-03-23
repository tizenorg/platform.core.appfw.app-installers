// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.


#include "unit_tests/common/scoped_testing_manifest_handler_registry.h"

namespace common_installer {

namespace parser {

ScopedTestingManifestHandlerRegistry::ScopedTestingManifestHandlerRegistry(
    const std::vector<ManifestHandler*>& handlers)
    : registry_(new ManifestHandlerRegistry(handlers)),
      prev_registry_(ManifestHandlerRegistry::GetInstance()) {
  ManifestHandlerRegistry::SetInstanceForTesting(registry_);
}

ScopedTestingManifestHandlerRegistry::~ScopedTestingManifestHandlerRegistry() {
  ManifestHandlerRegistry::SetInstanceForTesting(prev_registry_);
}

}  // namespace parser

}  // namespace common_installer
