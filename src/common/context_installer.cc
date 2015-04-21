/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/context_installer.h"
#include <unistd.h>
#include <cstdlib>

namespace common_installer {

ContextInstaller::ContextInstaller()
    : manifest_data(static_cast<manifest_x*>(calloc(1, sizeof(manifest_x)))),
      uid(getuid()) {}

ContextInstaller::~ContextInstaller() {
  if (manifest_data.get())
    pkgmgr_parser_free_manifest_xml(manifest_data.get());
}

}  // namespace common_installer
