/* Copyright 2015 Samsung Electronics, license APACHE-2.0, see LICENSE file */
#include "tpk/Step/StepParse.h"
#include <string>
#include <vector>
#include "boost/filesystem.hpp"
#include "common/context_installer.h"
#include "common/step/step.h"
#include "tpk/ManifestParser.h"
#include "tpk/XmlNodes.h"

using std::vector;

namespace tpk {
namespace Step {

/* short namespace/class name */
typedef common_installer::Step::Status Status;
using boost::filesystem::path;

/* Error codes to be returned */
static const Status
  &status_ok = common_installer::Step::Status::OK;
static const Status
  &status_error = common_installer::Step::Status::ERROR;

/* Internal exceptions */
class FileNotFoundException : public std::exception {};


/* process()
 * Parse tizen-manifest.xml and get the data from it
 * Store the data into the context_
 */
Status StepParse::process() {
  try {
    boost::filesystem::path mPath  = getManifestFilePath(
        context_->unpacked_dir_path());
    ManifestParser m(mPath.c_str());
    setContextByManifestParser(m);
  } catch (FileNotFoundException *e) {
    delete e;   // Consume this exception here
    return status_error;
  } catch (FileOpenFailureException *e) {
    delete e;
    return status_error;
  }
  return status_ok;
}


/* in parse() : Get manifest file path from the package unzipped directory
 */
boost::filesystem::path StepParse::getManifestFilePath(
    const boost::filesystem::path& dir) {
  static const char *manifestFileName = "tizen-manifest.xml";
  path mPath(dir);
  mPath /= manifestFileName;

  std::cout << "[StepParse] manifest file path: " << mPath << std::endl;
  if (!boost::filesystem::exists(mPath)) {
    std::cerr << "[StepParse] " << manifestFileName <<
        " not found from the package" << std::endl;
    throw new FileNotFoundException();
  }
  return mPath;  // object copy
}


/* Read manifest xml, and set up context_ object
 */
void StepParse::setContextByManifestParser(const ManifestParser &m) {
  const XmlNodeManifest &manifest = m.manifest;
  // debug
  std::cout << "[StepParse] Parse manifest xml values:";
  std::cout << "xmlns(" << manifest.xmlns << ") api_version(" <<
      manifest.api_version << ") package(" << manifest.package <<
      ") version(" << manifest.version << ")" << std::endl;

  // set context_
  context_->config_data()->set_application_name(
      std::string(reinterpret_cast<char*>(manifest.ui_application.label.data)));
  context_->config_data()->set_required_version(
      std::string(reinterpret_cast<char*>(manifest.api_version)));

  context_->set_pkgid(std::string(reinterpret_cast<char*>(manifest.package)));

  // set context_->manifest_data()
  setPkgInfoManifest(context_->manifest_data(), manifest);
}

void StepParse::setPkgInfoManifest(manifest_x* m,
    const XmlNodeManifest &manifest) {
  // Common values
  m->label =  reinterpret_cast<label_x*>
    (calloc(1, sizeof(label_x)));
  m->description =  reinterpret_cast<description_x*>
    (calloc(1, sizeof(description_x)));
  m->privileges =  reinterpret_cast<privileges_x*>
    (calloc(1, sizeof(privileges_x)));
  m->privileges->next = nullptr;
  m->privileges->privilege = nullptr;

  // Basic values
  m->package = strdup(reinterpret_cast<char*>(manifest.package));
  m->type = strdup("tpk");
  m->version = strdup(reinterpret_cast<char*>(manifest.version));
  m->label->name = strdup(
      reinterpret_cast<char*>(manifest.ui_application.label.name));
  // TODO(youmin.ha@samsung.com): get name from XML if exists
  m->description->name = nullptr;
  m->mainapp_id = strdup(
      reinterpret_cast<char*>(manifest.ui_application.appid));

  // Privileges
  vector<XmlNodePrivilege *> vp =
      const_cast<XmlNodeManifest &>(manifest).privileges.getPrivilegeVector();
  vector<XmlNodePrivilege *>::iterator it;
  for (it = vp.begin(); it != vp.end(); it++) {
    privilege_x *p =
        reinterpret_cast<privilege_x *>(calloc(1, sizeof(privilege_x)));
    // privilege data text
    p->text = strdup(reinterpret_cast<char*>((*it)->data));
    LISTADD(m->privileges->privilege, p);
    std::cout << "[TPK:Parser] add privilege: " << p->text << std::endl;
  }

  // Other app data (null initialization)
  m->serviceapplication = nullptr;  // ignore service application
  m->uiapplication = reinterpret_cast<uiapplication_x*>
    (calloc (1, sizeof(uiapplication_x)));
  m->uiapplication->icon = reinterpret_cast<icon_x*>
    (calloc(1, sizeof(icon_x)));
  m->uiapplication->label = reinterpret_cast<label_x*>
    (calloc(1, sizeof(label_x)));
  m->description = reinterpret_cast<description_x*>
    (calloc(1, sizeof(description_x)));
  m->uiapplication->appcontrol = nullptr;

  m->uiapplication->appid = strdup(
      reinterpret_cast<char*>(manifest.ui_application.appid));
  m->uiapplication->exec = strdup(
      reinterpret_cast<char*>(manifest.ui_application.exec));
  m->uiapplication->type = strdup(
      reinterpret_cast<char*>(manifest.ui_application.type));

  m->uiapplication->label->name = strdup(
      reinterpret_cast<char*>(manifest.ui_application.label.data));
  m->uiapplication->icon->name = strdup(
      reinterpret_cast<char*>(manifest.ui_application.icon.data));
  m->uiapplication->next = nullptr;
}

}  // namespace Step
}  // namespace tpk
