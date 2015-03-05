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
    if (!v.size() < 1) {
      LOG(ERROR) << element_name << " is not found as a child of " <<
          parent->name();
      return nullptr;
    }
    return v[0];  // Always return only the 1st child
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

  // set context_->manifest_data()
  return SetPkgInfoManifest(context_->manifest_data(), tree, manifest);
}

bool StepParse::SetPkgInfoManifest(manifest_x* m,
    XmlTree* tree,
    XmlElement* manifest) {
  // Get required elements
  XmlElement* ui_application, * label, * icon, * description;
  if (nullptr == (ui_application = Get1stChild(tree,
          manifest, "ui-application"))) return false;
  if (nullptr == (label = Get1stChild(tree, ui_application, "label")))
    return false;
  if (nullptr == (icon = Get1stChild(tree, ui_application, "icon")))
    return false;
  if (nullptr == (description = Get1stChild(tree,
          ui_application, "description"))) return false;

  // Common values
  m->label =  static_cast<label_x*>
    (calloc(1, sizeof(label_x)));
  m->description =  static_cast<description_x*>
    (calloc(1, sizeof(description_x)));
  m->privileges =  static_cast<privileges_x*>
    (calloc(1, sizeof(privileges_x)));
  m->privileges->next = nullptr;
  m->privileges->privilege = nullptr;

  // Basic values
  m->package = strdup(manifest->attr("package").c_str());
  m->type = strdup("tpk");
  m->version = strdup(manifest->attr("version").c_str());
  m->label->name = strdup(label->content().c_str());
  m->description->name = strdup(description->content().c_str());
  m->mainapp_id = strdup(ui_application->attr("appid").c_str());

  // Privileges
  XmlElement* privileges;
  if (nullptr == (privileges = Get1stChild(tree, manifest, "privileges"))) {
    return false;
  }
  vector<XmlElement*> v_priv = tree->Children(privileges, "privilege");
  for (auto& privilege : v_priv) {
    privilege_x *p =
        static_cast<privilege_x *>(calloc(1, sizeof(privilege_x)));
    // privilege data text
    p->text = strdup(privilege->content().c_str());
    LISTADD(m->privileges->privilege, p);
    LOG(INFO) << "Add a privilege: " << p->text;
  }

  // Other app data (null initialization)
  m->serviceapplication = nullptr;  // NOTE: ignore service application
  m->uiapplication = static_cast<uiapplication_x*>
    (calloc (1, sizeof(uiapplication_x)));
  m->uiapplication->icon = static_cast<icon_x*>
    (calloc(1, sizeof(icon_x)));
  m->uiapplication->label = static_cast<label_x*>
    (calloc(1, sizeof(label_x)));
  m->description = static_cast<description_x*>
    (calloc(1, sizeof(description_x)));
  m->uiapplication->appcontrol = nullptr;

  m->uiapplication->appid = strdup(ui_application->attr("appid").c_str());
  m->uiapplication->exec = strdup(ui_application->attr("exec").c_str());
  m->uiapplication->type = strdup(ui_application->attr("type").c_str());

  m->uiapplication->label->name = strdup(label->content().c_str());
  m->uiapplication->icon->name = strdup(icon->content().c_str());
  m->uiapplication->next = nullptr;

  return true;
}

}  // namespace step
}  // namespace tpk
