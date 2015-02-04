// Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.
// Part of this file is redistributed from crosswalk project under BSD-style
// license. Check LICENSE-xwalk file.

#ifndef WIDGET_MANIFEST_PARSER_WIDGET_MANIFEST_PARSER_H_
#define WIDGET_MANIFEST_PARSER_WIDGET_MANIFEST_PARSER_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Represents a manifest data
struct ManifestData {
  char* package;
  char* id;
  char* name;
  char* short_name;
  char* version;
  char* icon;
  char* api_version;
  unsigned int privilege_count;
  char** privilege_list;
};

// Reads manifest from specified file and filles specified data argument
// with read data. Returns true on success or false otherwise. If the error
// parameter is specified, it is also filled with proper message.
bool ParseManifest(const char* path,
    const ManifestData** data, const char** error);

// Releses the data and the error returned by ParseManifest.
void ReleaseData(const ManifestData* data, const char* error);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // WIDGET_MANIFEST_PARSER_WIDGET_MANIFEST_PARSER_H_
