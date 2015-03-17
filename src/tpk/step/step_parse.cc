/* Copyright 2015 Samsung Electronics, license APACHE-2.0, see LICENSE file */
#include "tpk/step/step_parse.h"
#include <boost/filesystem.hpp>
#include <memory>
#include <string>
#include <vector>
#include "common/context_installer.h"
#include "common/step/step.h"
#include "utils/logging.h"
#include "xml_parser/xml_parser.h"

using std::vector;
using std::string;
using xml_parser::XmlParser;
using xml_parser::XmlTree;
using xml_parser::XmlElement;


namespace tpk {
namespace step {

namespace {
  const char kManifestFileName[] = "tizen-manifest.xml";

  XmlElement* Get1stChild(XmlTree *tree,
      XmlElement* parent, const string element_name) {
    vector<XmlElement*> v = tree->Children(parent, element_name);
    if (v.size() < 1) {
      LOG(INFO) << element_name << " is not found as a child of " <<
          parent->name();
      return nullptr;
    }
    return v[0];  // Always return only the 1st child
  }

  const char* string_strdup(const string &s) {
    static const string nullstr = XmlElement::null_string();
    if (s == XmlElement::null_string()) {
      LOG(DEBUG) << "it is null_string";
      return nullptr;
    }
    return strdup(s.c_str());
  }


}  // namespace

SCOPE_LOG_TAG(StepParse)

/* short namespace/class name */
typedef common_installer::Step::Status Status;
using boost::filesystem::path;

/* process()
 * Parse tizen-manifest.xml and get the data from it
 * Store the data into the context_
 */
Status StepParse::process() {
  std::unique_ptr<boost::filesystem::path> mPath(
      GetManifestFilePath(context_->unpacked_dir_path()));
  if (!mPath) {
    return Status::ERROR;
  }
  LOG(INFO) << "Parse " << mPath->c_str();

  XmlParser parser;
  std::unique_ptr<XmlTree> tree(parser.ParseAndGetNewTree(mPath->c_str()));
  if (tree == nullptr) {
    LOG(ERROR) << "Failure on parsing xml";
    return Status::ERROR;
  }
  if (!SetContextByManifestParser(tree.get())) {
    return Status::ERROR;
  }
  return Status::OK;
}


/* in parse() : Get manifest file path from the package unzipped directory
 */
boost::filesystem::path* StepParse::GetManifestFilePath(
    const boost::filesystem::path& dir) {
  path* mPath = new path(dir);
  *mPath /= kManifestFileName;

  LOG(INFO) << "manifest file path: " << mPath->string();
  if (!boost::filesystem::exists(*mPath)) {
    LOG(ERROR) << kManifestFileName << " not found from the package";
    return nullptr;
  }
  return mPath;  // object copy
}


/* Read manifest xml, and set up context_ object
 */
bool StepParse::SetContextByManifestParser(XmlTree* tree) {
  // Get required elements
  XmlElement* manifest,
      * ui_application, * label;

  // manifest
  if (nullptr == (manifest = tree->GetRootElement())) return false;

  LOG(DEBUG) << "Getting manifest xml data";
  LOG(DEBUG) << "manifest: xmlns='" << manifest->attr("xmlns") <<
                "' api_version='" << manifest->attr("api_version") <<
                "' package='" << manifest->attr("package") <<
                "' versionr='" << manifest->attr("version") << "'";

  // ui_application
  if (nullptr == (ui_application = Get1stChild(tree,
          manifest, "ui-application"))) return false;
  if (nullptr == (label = Get1stChild(tree, ui_application, "label")))
    return false;

  // set context_
  context_->config_data()->set_application_name(label->content());
  context_->config_data()->set_required_version(manifest->attr("api_version"));
  context_->set_pkgid(manifest->attr("package"));
  context_->set_manifest(static_cast<manifest_x*>(
      calloc(1, sizeof(manifest_x))));

  // set context_->manifest_data()
  return SetPkgInfoManifest(context_->manifest_data(), tree, manifest);
}


bool StepParse::SetPkgInfoManifest(manifest_x* m,
    XmlTree* tree,
    XmlElement* manifest) {
  // Get required elements
  XmlElement* ui_application, * label, * icon, * description;
  ui_application = Get1stChild(tree, manifest, "ui-application");
  label = Get1stChild(tree, ui_application, "label");
  icon = Get1stChild(tree, ui_application, "icon");
  description = Get1stChild(tree, ui_application, "description");

  // manifest's attribute
  m->package = string_strdup(manifest->attr("package"));
  m->type = strdup("tpk");
  m->version = string_strdup(manifest->attr("version"));
  if (ui_application) {
    m->mainapp_id = string_strdup(ui_application->attr("appid"));
  }

  // manifest' attribute from children's values
  if (label) {
    m->label =  static_cast<label_x*>(calloc(1, sizeof(label_x)));
    m->label->name = string_strdup(label->content());
  }
  if (description) {
    m->description =  static_cast<description_x*>
      (calloc(1, sizeof(description_x)));
    m->description->name = string_strdup(description->content());
  }

  // Privileges
  XmlElement* privileges;
  privileges = Get1stChild(tree, manifest, "privileges");
  if (privileges) {
    m->privileges = static_cast<privileges_x*>
        (calloc(1, sizeof(privileges_x)));

    vector<XmlElement*> v_priv = tree->Children(privileges, "privilege");
    for (auto& privilege : v_priv) {
      privilege_x *p =
          static_cast<privilege_x *>(calloc(1, sizeof(privilege_x)));
      // privilege data text
      p->text = string_strdup(privilege->content());
      LISTADD(m->privileges->privilege, p);
      LOG(INFO) << "Add a privilege: " << p->text;
    }
  }

  // service-application
  m->serviceapplication = nullptr;  // NOTE: ignore service application

  // ui-application
  m->uiapplication = static_cast<uiapplication_x*>
    (calloc (1, sizeof(uiapplication_x)));

  m->uiapplication->appid = string_strdup(ui_application->attr("appid"));
  m->uiapplication->exec = string_strdup(ui_application->attr("exec"));
  m->uiapplication->type = string_strdup(ui_application->attr("type"));

  LOG(DEBUG) << "appid=" << m->uiapplication->appid <<
      ", exec=" << m->uiapplication->exec <<
      ", type=" << m->uiapplication->type;
  if (label) {
    m->uiapplication->label = static_cast<label_x*>
        (calloc(1, sizeof(label_x)));
    m->uiapplication->label->name = string_strdup(label->content());
  }
  if (icon) {
    m->uiapplication->icon = static_cast<icon_x*>
        (calloc(1, sizeof(icon_x)));
    m->uiapplication->icon->name = string_strdup(icon->content());
  }
  // NOTE: No description field or sub-element in the uiapplication_x struct.

  return true;
}

}  // namespace step
}  // namespace tpk
