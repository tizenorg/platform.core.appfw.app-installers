// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <gtest/gtest.h>

#include "parser/application_data.h"
#include "parser/application_manifest_constants.h"
#include "parser/manifest.h"
#include "parser/manifest_util.h"

using common_installer::parser::ApplicationData;
using common_installer::parser::Manifest;

namespace keys = common_installer::application_manifest_keys;
namespace bf = boost::filesystem;

namespace common_installer {
namespace parser {

class ManifestUtilTest : public testing::Test {
};

TEST_F(ManifestUtilTest, LoadApplicationWithValidPath) {
  boost::filesystem::path install_dir("/usr/share/app-installers-ut");
  ASSERT_TRUE(&install_dir);
  install_dir /= "test_samples";
  install_dir /= "good_manifest.xml";

  std::string error;
  std::unique_ptr<Manifest> manifest(LoadManifest(install_dir.string(),
                                                  Manifest::Type::TYPE_WIDGET,
                                                  &error));
  ASSERT_TRUE(error.empty());
  std::shared_ptr<common_installer::parser::ApplicationData>
      app_data =
      common_installer::parser::ApplicationData::Create(
          bf::path(), std::string(),
          common_installer::parser::ApplicationData::INTERNAL,
          std::move(manifest), &error);
  ASSERT_TRUE(error.empty());
  EXPECT_EQ("nNBDOItqjN.WebSettingSample", app_data->ID());
}

TEST_F(ManifestUtilTest,
       LoadApplicationGivesHelpfullErrorOnMissingManifest) {
  boost::filesystem::path install_dir("/usr/share/app-installers-ut");
  ASSERT_TRUE(&install_dir);
  install_dir /= "test_samples";
  install_dir /= "bad_manifest.xml";
  std::string error;
  std::unique_ptr<Manifest> manifest(LoadManifest(install_dir.string(),
                                                  Manifest::Type::TYPE_WIDGET,
                                                  &error));
  ASSERT_TRUE(!error.empty());
  ASSERT_FALSE(error.empty());
  ASSERT_STREQ("Manifest file is missing or unreadable.", error.c_str());
}

}  // namespace parser
}  // namespace common_installer
