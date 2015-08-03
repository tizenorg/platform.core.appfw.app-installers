/* Copyright 2015 Samsung Electronics, license APACHE-2.0, see LICENSE file */
#include "tpk/step/step_parse.h"

#include <boost/filesystem.hpp>
#include <memory>
#include <string>
#include <vector>
#include "common/context_installer.h"
#include "common/step/step.h"
#include "common/utils/logging.h"

using std::vector;
using std::string;
using xml_parser::XmlParser;
using xml_parser::XmlTree;
using xml_parser::XmlElement;


namespace tpk {
namespace parse {

namespace {
  const char kManifestFileName[] = "tizen-manifest.xml";

  XmlElement* Get1stChild(XmlTree *tree,
      XmlElement* parent, const string element_name) {
    vector<XmlElement*> v = tree->Children(parent, element_name);
    if (v.size() < 1) {
      LOG(DEBUG) << element_name << " is not found as a child of " <<
          parent->name();
      return nullptr;
    }
    return v[0];  // Always return only the 1st child
  }

  const char* string_strdup(const string &s) {
    static const string nullstr = XmlElement::null_string();
    if (s == XmlElement::null_string()) {
      return nullptr;
    }
    return strdup(s.c_str());
  }

  // Set child element's data structure, based on given parent and
  // children name.
  // setter's arguments
  //     T*: Child data structure's pointer
  //     L: setter function(XmlElement* element, T* child_struct)
  //     tree: tree pointer
  //     parent: parent XmlElement object
  //     childName: child name
  //     setter : child element setter (lambda function)
  template <typename T, typename L>
    bool SetChildren(T** listptr, XmlTree* tree, XmlElement* parent,
        const char* childName, L setter) {
      vector<XmlElement*> v = tree->Children(parent, childName);
      for (auto& el : v) {
        T* p = static_cast<T*>(calloc(1, sizeof(T)));
        setter(el, p);
        LISTADD(*listptr, p);
      }
      return true;
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
      GetManifestFilePath(context_->unpacked_dir_path.get()));
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
      * ui_application, * service_application, * label;

  // manifest
  if (nullptr == (manifest = tree->GetRootElement())) {
    LOG(ERROR) << "No mandatory manifest element in xml";
    return false;
  }

  LOG(DEBUG) << "Getting manifest xml data";
  LOG(DEBUG) << "manifest: xmlns='" << manifest->attr("xmlns") <<
                "' api_version='" << manifest->attr("api_version") <<
                "' package='" << manifest->attr("package") <<
                "' versionr='" << manifest->attr("version") << "'";

  // At least one of ui_application or service_application must be given
  ui_application = Get1stChild(tree, manifest, "ui-application");
  service_application = Get1stChild(tree, manifest, "service-application");
  if (nullptr == ui_application && nullptr == service_application) {
    LOG(ERROR) << "Neither <ui-application> nor <service-application>" <<
        " element is found in manifest xml";
    return false;
  }
  // label must be given
  label = ui_application ? Get1stChild(tree, ui_application, "label") :
      Get1stChild(tree, service_application, "label");
  if (nullptr == label) {
    LOG(ERROR) << "No mandatory <label> element in manifest xml";
    return false;
  }

  // set context_
  context_->config_data.get().required_api_version.set(
      manifest->attr("api-version"));
  context_->pkgid.set(manifest->attr("package"));

  // write pkgid for recovery file
  if (context_->recovery_info.get().recovery_file) {
    context_->recovery_info.get().recovery_file->set_pkgid(
        manifest->attr("package"));
    context_->recovery_info.get().recovery_file->WriteAndCommitFileContent();
  }

  context_->manifest_data.set(static_cast<manifest_x*>(
      calloc(1, sizeof(manifest_x))));

  // set context_->manifest_data()
  return SetPkgInfoManifest(context_->manifest_data.get(), tree, manifest);
}


bool StepParse::SetPkgInfoManifest(manifest_x* m,
    XmlTree* tree,
    XmlElement* manifest) {

  // Get required elements
  XmlElement* ui_application = Get1stChild(tree, manifest, "ui-application");
  XmlElement* service_application = Get1stChild(tree, manifest,
      "service-application");
  if (!(ui_application || service_application)) {   // mandatory check
    LOG(ERROR) << "Neither <ui-application> nor <service-application>" <<
        " element is found in manifest xml";
    return false;
  }

  // manifest's attribute
  m->package = string_strdup(manifest->attr("package"));
  m->type = strdup("tpk");
  m->version = string_strdup(manifest->attr("version"));
  m->installlocation = string_strdup(manifest->attr("install-location"));

  // Choose main app among ui-application or service-application
  // NOTE: main app must have appid attribute
  XmlElement* main_app = ui_application ? ui_application : service_application;
  m->mainapp_id = string_strdup(main_app->attr("appid"));

  // manifest' attribute from children's values
  XmlElement* label = Get1stChild(tree, main_app, "label");
  if (label) {
    m->label =  static_cast<label_x*>(calloc(1, sizeof(label_x)));
    m->label->name = string_strdup(label->content());
  }

  // Set children elements
  return SetPkgInfoChildren(m, tree, manifest);
}

// Read and fill struct hierarchy
// The spec of tizen-manifest is referred from Tizen 2.2.1 header.
bool StepParse::SetPkgInfoChildren(manifest_x* m,
    XmlTree* tree, XmlElement* manifest) {
  manifest_x* p = m;
  XmlElement* el = manifest;

  // author
  SetChildren(&(p->author), tree, el, "author",
      [&](XmlElement *el, author_x* p){
    p->email = string_strdup(el->attr("email"));
    p->href = string_strdup(el->attr("href"));
    p->text = string_strdup(el->content());
    // p->lang = string_strdup(el->attr("xml:lang")); // NOTE: not in spec
  });

  // description
  SetChildren(&(p->description), tree, el, "description",
      [&](XmlElement *el, description_x* p){
    // p->name = string_strdup(el->attr("name"));  // NOTE: not in spec
    p->text = string_strdup(el->content());
    p->lang = string_strdup(el->attr("xml:lang"));  // NOTE: not in spec
  });

  // privileges
  SetChildren(&(p->privileges), tree, el, "privileges",
      [&](XmlElement *el, privileges_x* p){
    // privilge
    SetChildren(&(p->privilege), tree, el, "privilege",
      [&](XmlElement *el, privilege_x* p){
        p->text = string_strdup(el->content());
        LOG(DEBUG) << "Add a privilege: " << p->text;
    });
  });

  // service-application
  SetChildren(&(p->serviceapplication), tree, el, "service-application",
      [&](XmlElement *el, serviceapplication_x* p){
    p->appid = string_strdup(el->attr("appid"));
    p->autorestart = string_strdup(el->attr("auto-restart"));
    p->exec = string_strdup(el->attr("exec"));
    p->onboot = string_strdup(el->attr("on-boot"));
    p->type = string_strdup(el->attr("type"));

    // FIXME: temporary fix to avoid build break
    // app-control
    SetChildren(&(p->appcontrol), tree, el, "app-control",
        [&](XmlElement *el, appcontrol_x* p){
      vector<XmlElement*> v = tree->Children(el, "operation");
      if (!v.empty())
        p->operation = string_strdup(v.front()->attr("name"));
      v = tree->Children(el, "uri");
      if (!v.empty())
        p->uri = string_strdup(v.front()->attr("name"));
      v = tree->Children(el, "mime");
      if (!v.empty())
        p->mime = string_strdup(v.front()->attr("name"));
    });

    // datacontrol
    SetChildren(&(p->datacontrol), tree, el, "datacontrol",
        [&](XmlElement *el, datacontrol_x* p){
      p->access = string_strdup(el->attr("access"));
      p->providerid = string_strdup(el->attr("providerid"));
      p->type = string_strdup(el->attr("type"));
    });

    // icon
    SetChildren(&(p->icon), tree, el, "icon",
        [&](XmlElement *el, icon_x* p) {  // NOLINT
      p->text = string_strdup(el->content());
      // NOTE: name is an attribute, but the xml writer uses it as text.
      // This must be fixed in whole app-installer modules, including wgt.
      // Current implementation is just for compatibility.
      // p->name = string_strdup(el->attr("name"));
      p->name = string_strdup(el->content());
    });

    // label
    SetChildren(&(p->label), tree, el, "label",
        [&](XmlElement *el, label_x* p) {
      // NOTE: name is an attribute, but the xml writer uses it as text.
      // This must be fixed in whole app-installer modules, including wgt.
      // Current implementation is just for compatibility.
      // p->name = string_strdup(el->attr("name"));
      p->text = string_strdup(el->content());
      p->name = string_strdup(el->content());
      p->lang = string_strdup(el->attr("xml:lang"));
    });

    // metadata
    SetChildren(&(p->metadata), tree, el, "metadata",
        [&](XmlElement *el, metadata_x* p){
      p->key = string_strdup(el->attr("key"));
      p->value = string_strdup(el->attr("value"));
    });
  });

  // ui-application
  SetChildren(&(p->uiapplication), tree, el, "ui-application",
      [&](XmlElement *el, uiapplication_x* p){
    p->appid = string_strdup(el->attr("appid"));
    p->exec = string_strdup(el->attr("exec"));
    p->multiple = string_strdup(el->attr("multiple"));
    p->nodisplay = string_strdup(el->attr("nodisplay"));
    p->taskmanage = string_strdup(el->attr("taskmanage"));
    p->type = string_strdup(el->attr("type"));
    // NOTE: onboot and auto-restart are in spec, but not in uiapplication_x

    // FIXME: temporary fix to avoid build break
    // app-control
    SetChildren(&(p->appcontrol), tree, el, "app-control",
        [&](XmlElement *el, appcontrol_x* p){
      vector<XmlElement*> v = tree->Children(el, "operation");
      if (!v.empty())
        p->operation = string_strdup(v.front()->attr("name"));
      v = tree->Children(el, "uri");
      if (!v.empty())
        p->uri = string_strdup(v.front()->attr("name"));
      v = tree->Children(el, "mime");
      if (!v.empty())
        p->mime = string_strdup(v.front()->attr("name"));
    });

    // datacontrol
    SetChildren(&(p->datacontrol), tree, el, "datacontrol",
        [&](XmlElement *el, datacontrol_x* p){
      p->access = string_strdup(el->attr("access"));
      p->providerid = string_strdup(el->attr("providerid"));
      p->type = string_strdup(el->attr("type"));
    });

    // icon
    SetChildren(&(p->icon), tree, el, "icon",
        [&](XmlElement *el, icon_x* p){
      p->text = string_strdup(el->content());
      // NOTE: name is an attribute, but the xml writer uses it as text.
      // This must be fixed in whole app-installer modules, including wgt.
      // Current implementation is just for compatibility.
      // p->name = string_strdup(el->attr("name"));
      p->name = string_strdup(el->content());
    });

    // label
    SetChildren(&(p->label), tree, el, "label",
        [&](XmlElement *el, label_x* p){
      // NOTE: name is an attribute, but the xml writer uses it as text.
      // This must be fixed in whole app-installer modules, including wgt.
      // Current implementation is just for compatibility.
      // p->name = string_strdup(el->attr("name"));
      p->text = string_strdup(el->content());
      p->name = string_strdup(el->content());
      p->lang = string_strdup(el->attr("xml:lang"));
    });

    // metadata
    SetChildren(&(p->metadata), tree, el, "metadata",
        [&](XmlElement *el, metadata_x* p){
      p->key = string_strdup(el->attr("key"));
      p->value = string_strdup(el->attr("value"));
    });
  });


  return true;
}

}  // namespace parse
}  // namespace tpk
