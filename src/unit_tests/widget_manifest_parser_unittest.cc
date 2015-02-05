// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include "widget-manifest-parser/widget_manifest_parser.h"

#include <gtest/gtest.h>

namespace common_installer {
namespace widget_manifest_parser {

class ParseManifestTest : public testing::Test {
 protected:
  const char* path;
  const ManifestData* data;
  const char* error;

  virtual void SetUp() {
    data = nullptr;
    error = nullptr;
  }

  virtual void TearDown() {
    if (data)
      ReleaseData(data, error);
  }
};

// Tests manifest parser with proper manifest
TEST_F(ParseManifestTest, HandlesProperManifestFile) {
  path = "/usr/share/app-installers-ut/test_samples/good_manifest.xml";
  ASSERT_TRUE(ParseManifest(path, &data, &error));
  ASSERT_EQ(nullptr, error);
  EXPECT_STREQ("nNBDOItqjN.WebSettingSample", data->id);
  EXPECT_STREQ("nNBDOItqjN", data->package);
  EXPECT_STREQ("WebSettingSample", data->name);
  EXPECT_STREQ("en-us", data->short_name);
  EXPECT_STREQ("1.0.0", data->version);
  EXPECT_STREQ("icon.png", data->icon);
  EXPECT_STREQ("2.2", data->api_version);
  EXPECT_EQ(1, data->privilege_count);
  EXPECT_STREQ("http://tizen.org/privilege/websetting",
               *(data->privilege_list));
}

// Tests manifest parser with broken manifest
TEST_F(ParseManifestTest, HandlesBrokenManifestFile) {
  path = "/usr/share/app-installers-ut/test_samples/bad_manifest.xml";
  ASSERT_FALSE(ParseManifest(path, &data, &error));
  EXPECT_STREQ("Manifest file is missing or unreadable.",
               error);
}

class ReleaseDataTest : public testing::Test {
 protected:
  const char* path;
  const ManifestData* data;
  const char* error;

  virtual void SetUp() {
    data = nullptr;
    error = nullptr;
  }
};

// Tests releasing data with proper manifest structure
TEST_F(ReleaseDataTest, HandlesProperManifestStructure) {
  path = "/usr/share/app-installers-ut/test_samples/good_manifest.xml";
  ASSERT_TRUE(ParseManifest(path, &data, &error));
  EXPECT_TRUE(ReleaseData(data, error));
}

// Tests factorial of positive numbers.
TEST_F(ReleaseDataTest, HandlesBrokenManifestStructure) {
  path = "/usr/share/app-installers-ut/test_samples/bad_manifest.xml";
  ASSERT_TRUE(!ParseManifest(path, &data, &error));
  EXPECT_TRUE(ReleaseData(data, error));
}

}  // namespace widget_manifest_parser
}  // namespace common_installer
