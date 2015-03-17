// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <gtest/gtest.h>

#include "manifest_handlers/application_manifest_constants.h"
#include "parser/manifest.h"
#include "parser/manifest_util.h"

namespace bf = boost::filesystem;

namespace parser {

class ManifestUtilTest : public testing::Test {
};

TEST_F(ManifestUtilTest, LoadApplicationWithValidPath) {
  boost::filesystem::path install_dir("/usr/share/app-installers-ut");
  ASSERT_TRUE(&install_dir);
  install_dir /= "test_samples";
  install_dir /= "good_manifest.xml";

  std::string error;
  std::shared_ptr<Manifest> manifest(
      LoadManifest(install_dir.string(), &error));
  ASSERT_TRUE(error.empty());
  EXPECT_TRUE(manifest.get());

  std::string app_id;
  manifest->GetString("widget.application.@id", &app_id);
  EXPECT_STREQ("nNBDOItqjN.WebSettingSample", app_id.c_str());
}

TEST_F(ManifestUtilTest,
       LoadApplicationGivesHelpfullErrorOnMissingManifest) {
  boost::filesystem::path install_dir("/usr/share/app-installers-ut");
  ASSERT_TRUE(&install_dir);
  install_dir /= "test_samples";
  install_dir /= "bad_manifest.xml";
  std::string error;
  std::shared_ptr<Manifest> manifest(
      LoadManifest(install_dir.string(), &error));
  ASSERT_FALSE(error.empty());
  ASSERT_STREQ("Manifest file is missing or unreadable.", error.c_str());
  EXPECT_FALSE(manifest.get());
}

// Tests IsPropSupportDir method
TEST_F(ManifestUtilTest, IsPropSupportDirTest) {
  xmlChar kWidgetNodeKey[] = "widget";
  xmlChar kNameNodeKey[] = "name";
  xmlChar kVersionAttributeKey[] = "version";
  xmlChar kShortAttributeKey[] = "short";
  xmlNode node, sec_node;
  node.name = kWidgetNodeKey;
  sec_node.name = kNameNodeKey;
  xmlAttr attr, sec_attr;
  attr.name = kVersionAttributeKey;
  sec_attr.name = kShortAttributeKey;
  ASSERT_TRUE(IsPropSupportDir(&node, &attr));
  ASSERT_TRUE(IsPropSupportDir(&sec_node, &sec_attr));
  ASSERT_TRUE(!IsPropSupportDir(&node, &sec_attr));
  ASSERT_TRUE(!IsPropSupportDir(&sec_node, &attr));
  ASSERT_TRUE(!IsPropSupportDir(&node, &sec_attr));
}

// Tests IsElementSupportSpanAndDir method
TEST_F(ManifestUtilTest, IsElementSupportSpanAndDirTest) {
  std::map <const char*, bool> nodeNames = {
    { "name", true },
    { "description", true },
    { "author", true },
    { "license", true },
    { "badlicense", false }
  };
  xmlNode node;
  for (const auto& p : nodeNames) {
    node.name = reinterpret_cast<xmlChar*>(const_cast<char*>(p.first));
    EXPECT_TRUE(IsElementSupportSpanAndDir(&node) == p.second);
  }
}

// Tests IsSingletonElement method
TEST_F(ManifestUtilTest, IsSingletonElementTest) {
  std::map<const char*, bool> kSingletonElements = {
    { "allow-navigation", true },
    { "author", true },
    { "content-security-policy-report-only", true },
    { "content-security-policy", true },
    { "content", false },
    { "badsingleton", false }
  };
  for (const auto& p : kSingletonElements)
    EXPECT_TRUE(IsSingletonElement(p.first) == p.second);
}

// Tests IsTrimRequiredForElement
TEST_F(ManifestUtilTest, IsTrimRequiredForElementTest) {
  std::map<const char*, bool> elements {
    { "name", true },
    { "author", true },
    { "badname", false },
    { "badauthor", false }
  };
  xmlNode node;
  for (const auto& p : elements) {
    node.name = reinterpret_cast<xmlChar*>(const_cast<char*>(p.first));
    EXPECT_TRUE(IsTrimRequiredForElement(&node) == p.second);
  }
}

// Tests IsTrimRequiredForProp method
TEST_F(ManifestUtilTest, IsTrimRequiredForPropTest) {
  struct node{
    bool operator <(const node& rhs) const {
      return node_name_ < rhs.node_name_;
    }
    const char* node_name_;
    const char* attr_name_;
  };

  std::map<node, bool> nodes = {
    { {"widget", "id"}, true },
    { {"widget", "version"}, true },
    { {"widget", "defaultlocale"}, true },
    { {"name", "short"}, true },
    { {"author", "email"}, true },
    { {"author", "href"}, true },
    { {"license", "href"}, true },
    { {"icon", "path"}, true },
    { {"widget", "email"}, false },
    { {"name", "path"}, false },
    { {"author", "id"}, false }
  };

  xmlNode node;
  xmlAttr attr;
  for (const auto& p : nodes) {
    node.name =
        reinterpret_cast<xmlChar*>(const_cast<char*>(p.first.node_name_));
    attr.name =
        reinterpret_cast<xmlChar*>(const_cast<char*>(p.first.attr_name_));
    EXPECT_TRUE(IsTrimRequiredForProp(&node, &attr) == p.second);
  }
}

// Tests GetNodeDir method with proper values
TEST_F(ManifestUtilTest, GetNodeDirTestProperValues) {
  const char* xml = "<widget dir=\"rtl\">"
                    "<name>ppa emoS</name>"
                    "</widget>";
  xmlDoc* doc = xmlParseMemory(xml, strlen(xml));
  ASSERT_TRUE(doc);
  xmlNode* root = doc->children;
  ASSERT_TRUE(root);
  std::string inherit_dir("ltr");
  EXPECT_STREQ("rtl", GetNodeDir(root, inherit_dir).c_str());
}

// Tests GetNodeDir method with default dir
TEST_F(ManifestUtilTest, GetNodeDirTestDefaultValues) {
  const char* xml = "<widget>"
                    "<name>Some app</name>"
                    "</widget>";
  xmlDoc* doc = xmlParseMemory(xml, strlen(xml));
  ASSERT_TRUE(doc);
  xmlNode* root = doc->children;
  ASSERT_TRUE(root);
  std::string inherit_dir("ltr");
  EXPECT_STREQ("ltr", GetNodeDir(root, inherit_dir).c_str());
}

// Tests GetNodeText method with rtl text direction
TEST_F(ManifestUtilTest, GetNodeTextTestXmlElementNode) {
  const char* xml = "<widget dir=\"rtl\">"
                    "<name>ppa emoS</name>"
                    "</widget>";
  std::map<char, int> control_chars = {
    { '\xe2', 0 },
    { '\x80', 1 },
    { '\xab', 2 },
    { '\xe2', 11 },
    { '\x80', 12 },
    { '\xac', 13 },
  };
  xmlDoc* doc = xmlParseMemory(xml, strlen(xml));
  ASSERT_TRUE(doc);
  xmlNode* root = doc->children;
  ASSERT_TRUE(root);
  std::string inherit_dir("ltr");
  std::string s = GetNodeText(root, inherit_dir);
  for (const auto& p : control_chars)
    EXPECT_EQ(p.first, s[p.second]);
}

// Tests GetNodeText method with rtl and ltr text direction
TEST_F(ManifestUtilTest, GetNodeTextTestTwoXmlElementNodes) {
  const char* xml = "<widget dir=\"rtl\">"
                    "<name>ppa emoS</name>"
                    "<description dir=\"ltr\">Desc</description>"
                    "</widget>";
  std::map<char, int> control_chars = {
    { '\xe2', 0 }, { '\x80', 1 },
    { '\xab', 2 }, { '\xe2', 11 },
    { '\x80', 12 }, { '\xac', 13 },
    { '\xe2', 14 }, { '\x80', 15 },
    { '\xaa', 16 }, { '\xe2', 21 },
    { '\x80', 22 }, { '\xac', 23 },
  };
  xmlDoc* doc = xmlParseMemory(xml, strlen(xml));
  ASSERT_TRUE(doc);
  xmlNode* root = doc->children;
  ASSERT_TRUE(root);
  std::string inherit_dir("ltr");
  std::string s = GetNodeText(root, inherit_dir);
  for (const auto& p : control_chars)
    EXPECT_EQ(p.first, s[p.second]);
}

// Tests LoadXMLNode method with proper xml tree
TEST_F(ManifestUtilTest, LoadXMLNodeTestProperXMLTree) {
  const char* xml = "<root>"
                    "<widget dir=\"ltr\">"
                    "<name short=\"SA\">Some app</name>"
                    "<description>Desc</description>"
                    "<author dir=\"rtl\">enoemoS</author>"
                    "</widget>"
                    "</root>";
  std::map<std::string, std::string> expected_vals = {
    { "widget.@dir", "ltr" },
    { "widget.name.@short",
      std::string("\xE2\x80\xAA") + "SA" + "\xE2\x80\xAC" },
    { "widget.name.#text",
      std::string("\xE2\x80\xAA") + "Some app" + "\xE2\x80\xAC" },
    { "widget.description.#text",
      std::string("\xE2\x80\xAA") + "Desc" + "\xE2\x80\xAC" },
    { "widget.author.@dir", "rtl" },
    { "widget.author.#text",
      std::string("\xE2\x80\xAB") + "enoemoS" + "\xE2\x80\xAC" }
  };

  xmlDoc* doc = xmlParseMemory(xml, strlen(xml));
  ASSERT_TRUE(doc);
  xmlNode* root = doc->children;
  std::unique_ptr<utils::DictionaryValue> val = LoadXMLNode(root);

  std::string test_str;
  for (const auto& p : expected_vals) {
    ASSERT_TRUE(val->GetString(p.first, &test_str));
    EXPECT_STREQ(p.second.c_str(), test_str.c_str());
  }
}

}  // namespace parser
