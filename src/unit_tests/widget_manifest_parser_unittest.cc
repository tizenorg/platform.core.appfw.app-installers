// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include "parser/widget_manifest_parser.h"

#include <boost/filesystem/path.hpp>
#include <gtest/gtest.h>
#include <pkgmgr/pkgmgr_parser.h>

namespace bf = boost::filesystem;

namespace common_installer {
namespace parser {

class ParseManifestTest : public testing::Test {
 protected:
  manifest_x* manifest_;
  std::unique_ptr<WidgetManifestParser> parser_;

  virtual void SetUp() {
    parser_.reset(new WidgetManifestParser);
    manifest_ = static_cast<manifest_x*>(calloc(1, sizeof(manifest_x)));
  }

  virtual void TearDown() {
    pkgmgr_parser_free_manifest_xml(manifest_);
  }
};

// Tests manifest parser with proper manifest
TEST_F(ParseManifestTest, HandlesProperManifestFile) {
  bf::path path = "/usr/share/app-installers-ut/test_samples/good_manifest.xml";
  ASSERT_TRUE(parser_->ParseManifest(path));
  ASSERT_TRUE(parser_->FillManifestX(manifest_));
  EXPECT_STREQ("nNBDOItqjN.WebSettingSample", manifest_->uiapplication->appid);
  EXPECT_STREQ("nNBDOItqjN", manifest_->package);
  EXPECT_STREQ("WebSettingSample", manifest_->description->name);
  EXPECT_STREQ("en-us", parser_->GetShortName().c_str());
  EXPECT_STREQ("1.0.0", manifest_->version);
  EXPECT_STREQ("icon.png", manifest_->icon->name);
  EXPECT_STREQ("2.2", parser_->GetRequiredAPIVersion().c_str());
  EXPECT_STREQ("http://tizen.org/privilege/websetting",
               manifest_->privileges->privilege->text);
}

// Tests manifest parser with broken manifest
TEST_F(ParseManifestTest, HandlesBrokenManifestFile) {
  bf::path path = "/usr/share/app-installers-ut/test_samples/bad_manifest.xml";
  ASSERT_FALSE(parser_->ParseManifest(path));
  EXPECT_STREQ("Manifest file is missing or unreadable.",
               parser_->GetErrorMessage().c_str());
}

}  // namespace parser
}  // namespace common_installer
