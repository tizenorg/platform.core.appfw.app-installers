// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#include <gtest/gtest.h>

#include <algorithm>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "utils/values.h"
#include "parser/application_manifest_constants.h"
#include "parser/manifest.h"

namespace errors = common_installer::application_manifest_errors;
namespace keys = common_installer::application_widget_keys;

namespace common_installer {
namespace parser {

class ManifestTest : public testing::Test {
 public:
  ManifestTest() : default_value_("test") {}

 protected:
  // Helper function that replaces the Manifest held by |manifest| with a copy
  // with its |key| changed to |value|. If |value| is nullptr, then |key| will
  // instead be deleted.
  void MutateManifest(std::unique_ptr<Manifest>* manifest,
                      const std::string& key,
                      utils::Value* value) {
    std::unique_ptr<utils::DictionaryValue> manifest_value(
        manifest->get()->value()->DeepCopy());
    if (value)
      manifest_value->Set(key, value);
    else
      manifest_value->Remove(key, nullptr);
    manifest->reset(new Manifest(std::move(manifest_value)));
  }

  std::string default_value_;
};

// Verifies that application can access the correct keys.
TEST_F(ManifestTest, ApplicationData) {
  std::unique_ptr<utils::DictionaryValue> manifest_value(
      new utils::DictionaryValue());
  manifest_value->SetString(keys::kNameKey, "extension");
  manifest_value->SetString(keys::kVersionKey, "1");
  manifest_value->SetString("unknown_key", "foo");

  std::unique_ptr<Manifest> manifest(
      new Manifest(std::move(manifest_value)));
  std::string error;
  EXPECT_TRUE(manifest->ValidateManifest(&error));
  EXPECT_TRUE(error.empty());

  // The unknown key 'unknown_key' should be accesible.
  std::string value;
  EXPECT_TRUE(manifest->GetString("unknown_key", &value));
  EXPECT_EQ("foo", value);

  // Test DeepCopy and Equals.
  std::unique_ptr<Manifest> manifest2(manifest->DeepCopy());
  EXPECT_TRUE(manifest->Equals(manifest2.get()));
  EXPECT_TRUE(manifest2->Equals(manifest.get()));
  MutateManifest(
      &manifest, "foo", new utils::StringValue("blah"));
  EXPECT_FALSE(manifest->Equals(manifest2.get()));
}

// Verifies that key restriction based on type works.
TEST_F(ManifestTest, ApplicationTypes) {
  std::unique_ptr<utils::DictionaryValue> value(new utils::DictionaryValue());
  value->SetString(keys::kNameKey, "extension");
  value->SetString(keys::kVersionKey, "1");

  std::unique_ptr<Manifest> manifest(
      new Manifest(std::move(value)));
  std::string error;
  EXPECT_TRUE(manifest->ValidateManifest(&error));
  EXPECT_TRUE(error.empty());
}

}  // namespace parser
}  // namespace common_installer


