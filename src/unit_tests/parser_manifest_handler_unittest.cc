// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#include <boost/filesystem/path.hpp>
#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>

#include "parser/values.h"
#include "parser/manifest_handler.h"
#include "parser/manifest_parser.h"

namespace parser {

namespace bf = boost::filesystem;

class ManifestHandlerTest : public testing::Test {
 public:
  class ParsingWatcher {
   public:
    // Called when a manifest handler parses.
    void Record(const std::string& name) {
      parsed_names_.push_back(name);
    }

    const std::vector<std::string>& parsed_names() {
      return parsed_names_;
    }

    // Returns true if |name_before| was parsed before |name_after|.
    bool ParsedBefore(const std::string& name_before,
                      const std::string& name_after) {
      size_t prev_iterator = parsed_names_.size();
      size_t next_iterator = 0;
      for (size_t i = 0; i < parsed_names_.size(); ++i) {
        if (parsed_names_[i] == name_before)
          prev_iterator = i;
        if (parsed_names_[i] == name_after)
          next_iterator = i;
      }

      if (prev_iterator < next_iterator)
        return true;

      return false;
    }

   private:
    // The order of manifest handlers that we watched parsing.
    std::vector<std::string> parsed_names_;
  };

  class TestManifestHandler : public parser::ManifestHandler {
   public:
    TestManifestHandler(const std::string& name,
                        const std::string& key,
                        const std::vector<std::string>& prereqs,
                        ParsingWatcher* watcher)
        : name_(name), key_(key), prereqs_(prereqs), watcher_(watcher) {
    }

    ~TestManifestHandler() override {}

    bool Parse(
        const parser::Manifest& /*manifest*/,
        std::shared_ptr<ManifestData>* /*output*/,
        std::string* /*error*/) override {
      watcher_->Record(name_);
      return true;
    }

    std::vector<std::string> PrerequisiteKeys() const override {
      return prereqs_;
    }

    std::string Key() const override {
      return key_;
    }

   protected:
    std::string name_;
    std::string key_;
    std::vector<std::string> prereqs_;
    ParsingWatcher* watcher_;
  };

  class FailingTestManifestHandler : public TestManifestHandler {
   public:
    FailingTestManifestHandler(const std::string& name,
                               const std::string& key,
                               const std::vector<std::string>& prereqs,
                               ParsingWatcher* watcher)
        : TestManifestHandler(name, key, prereqs, watcher) {
    }
    bool Parse(
        const parser::Manifest& manifest,
        std::shared_ptr<ManifestData>* /*output*/,
        std::string* error) override {
      *error = name_;
      return false;
    }
  };

  class AlwaysParseTestManifestHandler : public TestManifestHandler {
   public:
    AlwaysParseTestManifestHandler(const std::string& name,
                                   const std::string& key,
                                   const std::vector<std::string>& prereqs,
                                   ParsingWatcher* watcher)
        : TestManifestHandler(name, key, prereqs, watcher) {
    }
  };

  class TestManifestValidator : public parser::ManifestHandler {
   public:
    TestManifestValidator(bool return_value,
                          bool always_validate,
                          std::string key)
        : return_value_(return_value),
          always_validate_(always_validate),
          key_(key) {
    }

    bool Parse(
        const parser::Manifest& /*manifest*/,
        std::shared_ptr<ManifestData>* output,
        std::string* /*error*/) override {
      *output = std::make_shared<ManifestData>();
      return true;
    }

    bool Validate(
        const parser::ManifestData& /*data*/,
        const parser::ManifestDataMap& /*handlers_output*/,
        std::string* error) const override {
        if (!return_value_)
          *error = "validation_error";
      return return_value_;
    }

    bool AlwaysValidateForType() const override {
      return always_validate_;
    }

    std::string Key() const override {
      return key_;
    }

   protected:
    bool return_value_;
    bool always_validate_;
    std::string key_;
  };
};

TEST_F(ManifestHandlerTest, DependentHandlers) {
  std::vector<parser::ManifestHandler*> handlers;
  ParsingWatcher watcher;
  std::vector<std::string> prereqs;
  handlers.push_back(
      new TestManifestHandler("A", "a", prereqs, &watcher));
  handlers.push_back(
      new TestManifestHandler("B", "b", prereqs, &watcher));
  handlers.push_back(
      new TestManifestHandler("J", "j", prereqs, &watcher));
  handlers.push_back(
      new AlwaysParseTestManifestHandler(
          "K", "k", prereqs, &watcher));

  std::unique_ptr<utils::DictionaryValue> dictionary =
      std::unique_ptr<utils::DictionaryValue>(new utils::DictionaryValue);
  dictionary->SetString("name", "no name");
  dictionary->SetString("version", "0");
  dictionary->SetInteger("manifest_version", 2);
  dictionary->SetInteger("a", 1);
  dictionary->SetInteger("b", 2);
  dictionary->SetInteger("c.d", 3);
  dictionary->SetInteger("c.e", 4);
  dictionary->SetInteger("c.f", 5);
  dictionary->SetInteger("g", 6);
  std::string error;

  std::shared_ptr<parser::Manifest> manifest =
      std::make_shared<parser::Manifest>(std::move(dictionary));

  ASSERT_TRUE(manifest.get());

  std::vector<std::shared_ptr<const parser::ManifestData>> data;
  for (parser::ManifestHandler* handler : handlers) {
    std::shared_ptr<ManifestData> output;
    if (handler->Parse(*manifest.get(), &output, &error)) {
      data.push_back(output);
    }
  }

  EXPECT_EQ(4u, watcher.parsed_names().size());
  EXPECT_TRUE(watcher.ParsedBefore("B", "K"));
  EXPECT_TRUE(watcher.ParsedBefore("J", "K"));
  EXPECT_TRUE(watcher.ParsedBefore("A", "B"));
}

TEST_F(ManifestHandlerTest, FailingHandlers) {
  std::unique_ptr<utils::DictionaryValue> manifest_a =
      std::unique_ptr<utils::DictionaryValue>(new utils::DictionaryValue);
  manifest_a->SetString("name", "no name");
  manifest_a->SetString("version", "0");
  manifest_a->SetInteger("manifest_version", 2);
  manifest_a->SetInteger("a", 1);

  ASSERT_TRUE(manifest_a.get());
  // Succeeds when "a" is not recognized.
  std::string error;
  std::shared_ptr<Manifest> manifest =
      std::make_shared<Manifest>(std::move(manifest_a));
  ASSERT_TRUE(manifest.get());

  // Register a handler for "a" that fails.
  std::vector<ManifestHandler*> handlers;
  ParsingWatcher watcher;
  handlers.push_back(
      new FailingTestManifestHandler(
          "A", "a", std::vector<std::string>(), &watcher));

  std::vector<std::shared_ptr<const ManifestData>> data;
  for (auto handler : handlers) {
    std::shared_ptr<ManifestData> output;
    if (handler->Parse(*manifest.get(), &output, &error)) {
      data.push_back(output);
    }
  }

  for (auto& d : data) {
    EXPECT_TRUE(!d.get());
    EXPECT_EQ("A", error);
  }
}

TEST_F(ManifestHandlerTest, Validate) {
  std::unique_ptr<utils::DictionaryValue> dictionary =
      std::unique_ptr<utils::DictionaryValue>(new utils::DictionaryValue);
  dictionary->SetString("name", "no name");
  dictionary->SetString("version", "0");
  dictionary->SetInteger("manifest_version", 2);
  dictionary->SetInteger("a", 1);
  dictionary->SetInteger("b", 2);
  std::string error;

  std::shared_ptr<Manifest> manifest =
      std::make_shared<Manifest>(std::move(dictionary));

  ASSERT_TRUE(manifest.get());

  std::vector<ManifestHandler*> handlers;
  // Always validates and fails.
  handlers.push_back(
      new TestManifestValidator(false, true, "c"));
  handlers.push_back(
      new TestManifestValidator(false, false, "c"));
  handlers.push_back
      (new TestManifestValidator(false, true, "a"));

  for (auto& handler : handlers) {
    std::shared_ptr<ManifestData> d;
    handler->Parse(*manifest.get(), &d, &error);
    EXPECT_TRUE(d.get());
    ASSERT_STREQ("", error.c_str());
    EXPECT_FALSE(handler->Validate(*d.get(), ManifestDataMap(), &error));
    EXPECT_STREQ("validation_error", error.c_str());
    error.clear();
  }
}

}  // namespace parser
