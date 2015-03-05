/* Copyright 2015 Samsung Electronics, license APACHE-2.0, see LICENSE file */
#include <cassert>
#include <iostream>
#include <memory>
#include "xml_parser/xml_parser.h"

using std::cout;
using std::cerr;
using std::endl;
using std::unique_ptr;
using xml_parser::XmlElement;
using xml_parser::XmlTree;
using xml_parser::XmlParser;

int main(int argc, char** argv) {
  const char* filename = "test-manifest.xml";
  XmlParser p;

  unique_ptr<XmlTree> t(p.ParseAndGetNewTree(filename));
  if (t == nullptr) {
    cerr << "Cannot get a xml tree" << endl;
    return -1;
  }

  XmlElement* root = t->GetRootElement();
  assert(root->name() == "manifest");
  assert(root->attr("api-version") == "2.3");

  XmlElement* ui_application = t->Children(root, "ui-application")[0];
  assert(ui_application->attr("appid") == "org.tizen.testapp");

  XmlElement* app_control0 = t->Children(ui_application, "app-control")[0];
  XmlElement* operation = t->Children(app_control0, "operation")[0];
  assert(operation->attr("name") ==
      "http://tizen.org/appcontrol/operation/edit");

  XmlElement* privileges = t->Children(root, "privileges")[0];
  XmlElement* privilege1 = t->Children(privileges, "privilege")[1];
  assert(privilege1->content() ==
      "http://tizen.org/privilege/packagemanager.info");

  XmlElement* feature3 = t->Children(root, "feature")[3];
  assert(feature3->attr("name") == "http://tizen.org/feature/camera.front");
  assert(feature3->content() == "true");

  // null_string equality test(should be true)
  assert(feature3->attr("nonexist_attr") == XmlElement::null_string());

  cout << "Test complete. No error." << endl;

  return 0;
}
