// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include <gtest/gtest.h>
#include <boost/filesystem/path.hpp>
#include <iostream>
#include <memory>
#include "xml_parser/xml_parser.h"

namespace {

using std::cout;
using std::cerr;
using std::endl;
using std::unique_ptr;
using xml_parser::XmlElement;
using xml_parser::XmlTree;
using xml_parser::XmlParser;

namespace bf = boost::filesystem;

class TestXmlParser : public testing::Test {
 protected:
  virtual void SetUp() {
  }
  virtual void TearDown() {
  }
};

// Tests manifest parser with proper manifest
TEST_F(TestXmlParser, ReadManifestXml) {
  const char* filename =
      "/usr/share/app-installers-ut/test_samples/tpk-sample-manifest.xml";
  XmlParser p;

  unique_ptr<XmlTree> t(p.ParseAndGetNewTree(filename));
  ASSERT_NE(t, nullptr);

  XmlElement* root = t->GetRootElement();
  ASSERT_EQ(root->name(), "manifest");
  ASSERT_EQ(root->attr("api-version"), "2.3");

  XmlElement* ui_application = t->Children(root, "ui-application")[0];
  ASSERT_EQ(ui_application->attr("appid"), "org.tizen.testapp");

  XmlElement* app_control0 = t->Children(ui_application, "app-control")[0];
  XmlElement* operation = t->Children(app_control0, "operation")[0];
  ASSERT_EQ(operation->attr("name"),
      "http://tizen.org/appcontrol/operation/edit");

  XmlElement* privileges = t->Children(root, "privileges")[0];
  XmlElement* privilege1 = t->Children(privileges, "privilege")[1];
  ASSERT_EQ(privilege1->content(),
      "http://tizen.org/privilege/packagemanager.info");

  XmlElement* feature3 = t->Children(root, "feature")[3];
  ASSERT_EQ(feature3->attr("name"),
      "http://tizen.org/feature/camera.front");
  ASSERT_EQ(feature3->content(), "true");

  // null_string equality test(should be true)
  ASSERT_EQ(feature3->attr("nonexist_attr"), XmlElement::null_string());
}

}  // namespace
