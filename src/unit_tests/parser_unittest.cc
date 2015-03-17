// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include <boost/filesystem/path.hpp>
#include <gtest/gtest.h>

#include "parser/manifest_parser.h"

namespace bf = boost::filesystem;

namespace wgt {
namespace parse {

const char kTizenApplicationKey[] = "widget.application";
const char kNamespaceKey[] = "@namespace";
const char kTizenNamespacePrefix[] = "http://tizen.org/ns/widgets";
const char kTizenApplicationIdKey[] = "@id";
const char kTizenApplicationPackageKey[] = "@package";
const char kTizenApplicationRequiredVersionKey[] = "@required_version";

class TizenApplicationInfo : public parser::ManifestData {
 public:
  TizenApplicationInfo() {}
  virtual ~TizenApplicationInfo() {}

  void set_id(const std::string& id) {
    id_ = id;
  }
  void set_package(const std::string& package) {
    package_ = package;
  }
  void set_required_version(
      const std::string& required_version) {
    required_version_ = required_version;
  }
  const std::string& id() const {
    return id_;
  }
  const std::string& package() const {
    return package_;
  }
  const std::string& required_version() const {
    return required_version_;
  }

 private:
  std::string id_;
  std::string package_;
  std::string required_version_;
};

class TizenApplicationHandler : public parser::ManifestHandler {
 public:
  TizenApplicationHandler() {}
  ~TizenApplicationHandler() {}

  bool Parse(
      const parser::Manifest& manifest,
      std::shared_ptr<parser::ManifestData>* output,
      std::string* error) override {
    std::shared_ptr<TizenApplicationInfo> app_info(new TizenApplicationInfo);

    parser::utils::Value* app_value = nullptr;
    manifest.Get(kTizenApplicationKey, &app_value);
    // Find an application element with tizen namespace
    parser::utils::DictionaryValue* app_dict;
    std::string value;
    bool find = false;
    if (app_value && app_value->IsType(parser::utils::Value::TYPE_DICTIONARY)) {
      app_value->GetAsDictionary(&app_dict);
      find = app_dict->GetString(kNamespaceKey, &value);
      find = find && (value == kTizenNamespacePrefix);
    } else if (app_value && app_value->IsType(
                   parser::utils::Value::TYPE_LIST)) {
      parser::utils::ListValue* list;
      app_value->GetAsList(&list);
      for (parser::utils::ListValue::iterator it = list->begin();
          it != list->end(); ++it) {
        (*it)->GetAsDictionary(&app_dict);
        find = app_dict->GetString(kNamespaceKey, &value);
        find = find && (value == kTizenNamespacePrefix);
        if (find)
          break;
      }
    }

    if (!find) {
      *error = "Cannot find application element with tizen namespace "
               "or the tizen namespace prefix is incorrect.\n";
      return false;
    }
    if (app_dict->GetString(kTizenApplicationIdKey, &value))
      app_info->set_id(value);
    if (app_dict->GetString(kTizenApplicationPackageKey, &value)) {
      app_info->set_package(value);
    }
    if (app_dict->GetString(kTizenApplicationRequiredVersionKey, &value))
      app_info->set_required_version(value);

    *output = std::static_pointer_cast<parser::ManifestData>(app_info);
    return true;
  }

  bool Validate(const parser::ManifestData& /*data*/,
      const parser::ManifestDataMap& /*handlers_output*/,
      std::string* /*error*/) const override {
    return true;
  }

  std::string Key() const override {
    return kTizenApplicationKey;
  }
};

}  // namespace parse
}  // namespace wgt

namespace parser {

class ParseManifestTest : public testing::Test {
 protected:
  std::unique_ptr<ManifestParser> parser_;
};

// Tests manifest parser with proper manifest
TEST_F(ParseManifestTest, HandlesProperManifestFile) {
  bf::path path = "/usr/share/app-installers-ut/test_samples/good_manifest.xml";
  std::vector<parser::ManifestHandler*> handlers = {
    new wgt::parse::TizenApplicationHandler,
  };

  std::unique_ptr<ManifestHandlerRegistry> registry;
  registry.reset(new ManifestHandlerRegistry(handlers));
  parser_.reset(new ManifestParser(std::move(registry)));

  ASSERT_TRUE(parser_->ParseManifest(path));

  std::shared_ptr<const wgt::parse::TizenApplicationInfo> app_info =
      std::static_pointer_cast<const wgt::parse::TizenApplicationInfo>(
          parser_->GetManifestData(wgt::parse::kTizenApplicationKey));
  ASSERT_TRUE(app_info.get());
  EXPECT_STREQ("nNBDOItqjN.WebSettingSample", app_info->id().c_str());
  EXPECT_STREQ("nNBDOItqjN", app_info->package().c_str());
  EXPECT_STREQ("2.2", app_info->required_version().c_str());
}

// Tests manifest parser with broken manifest
TEST_F(ParseManifestTest, HandlesBrokenManifestFile) {
  bf::path path = "/usr/share/app-installers-ut/test_samples/bad_manifest.xml";
  std::vector<parser::ManifestHandler*> handlers = {
    new wgt::parse::TizenApplicationHandler,
  };

  std::unique_ptr<ManifestHandlerRegistry> registry =
      std::unique_ptr<ManifestHandlerRegistry>(
          new ManifestHandlerRegistry(handlers));
  parser_.reset(new ManifestParser(std::move(registry)));

  ASSERT_FALSE(parser_->ParseManifest(path));
  EXPECT_STREQ("Manifest file is missing or unreadable.",
               parser_->GetErrorMessage().c_str());
}

}  // namespace parser
