/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#include "wgt/step/step_parse.h"

#include <string.h>

#include <pkgmgr/pkgmgr_parser.h>

#include <cstdio>
#include <cstdlib>
#include <string>

#include "common/app_installer.h"
#include "common/context_installer.h"
#include "common/step/step.h"

namespace wgt {
namespace parse {

common_installer::Step::Status StepParse::process() {
  if (!StepParse::Check(context_->unpack_directory())) {
    LOG(ERROR) << "No config.xml";
    return common_installer::Step::Status::ERROR;
  }

  const ManifestData* data = nullptr;
  const char* error = nullptr;
  if (!ParseManifest(config_.c_str(), &data, &error)) {
     LOG(ERROR) << "Parse failed. " <<  error;
    if (!ReleaseData(data, error))
      LOG(ERROR) << " Release data failed.";
    return common_installer::Step::Status::ERROR;
  }

  // Copy data from ManifestData to ContextInstaller
  context_->config_data()->set_application_name(
      std::string(data->name));
  context_->config_data()->set_required_version(
      std::string(data->api_version));
  context_->set_pkgid(
      std::string(data->package));
  fillManifest(data, context_->manifest_data());

  // TODO(t.iwanek): remove
  // --- Test ---
  std::cout << " Read data -[ " << std::endl;
  std::cout << "  package     = " <<  data->package << std::endl;
  std::cout << "  id          = " <<  data->id << std::endl;
  std::cout << "  name        = " <<  data->name << std::endl;
  std::cout << "  short_name  = " <<  data->short_name << std::endl;
  std::cout << "  version     = " <<  data->version << std::endl;
  std::cout << "  icon        = " <<  data->icon << std::endl;
  std::cout << "  api_version = " <<  data->api_version << std::endl;
  std::cout << "  privileges -[" << std::endl;
  for (unsigned int i = 0; i < data->privilege_count; ++i)
    std::cout << "    " << data->privilege_list[i] << std::endl;
  std::cout << "  ]-" << std::endl;
  std::cout << "]-" << std::endl;
  // --- End Test ---

  if (!ReleaseData(data, error)) {
    LOG(ERROR) << "Release data failed.";
    return common_installer::Step::Status::ERROR;
  }

  return common_installer::Step::Status::OK;
}

bool StepParse::Check(const boost::filesystem::path& widget_path) {
  boost::filesystem::path config = widget_path;
  config /= "config.xml";

  LOG(DEBUG) << "config.xml path: " << config;

  if (!boost::filesystem::exists(config))
    return false;

  config_ = config;
  return true;
}

void StepParse::fillManifest(const ManifestData* data, manifest_x* manifest) {
  // package data
  manifest->label =  reinterpret_cast<label_x*>
    (calloc(1, sizeof(label_x)));
  manifest->description =  reinterpret_cast<description_x*>
    (calloc(1, sizeof(description_x)));
  manifest->privileges =  reinterpret_cast<privileges_x*>
    (calloc(1, sizeof(privileges_x)));
  manifest->privileges->next = nullptr;
  manifest->privileges->privilege = nullptr;

  manifest->package = strdup(data->package);
  manifest->type = strdup("wgt");
  manifest->version = strdup(data->version);
  manifest->label->name = strdup(data->name);
  manifest->description->name = nullptr;
  manifest->mainapp_id = strdup(data->id);

  for (unsigned int i = 0; i < data->privilege_count; i++) {
     privilege_x *privilege = reinterpret_cast<privilege_x*>(
             malloc(sizeof(privilege_x)));
     privilege->text = strdup(data->privilege_list[i]);
     LISTADD(manifest->privileges->privilege, privilege);
  }
  // application data
  manifest->serviceapplication = nullptr;
  manifest->uiapplication = reinterpret_cast<uiapplication_x*>
    (calloc (1, sizeof(uiapplication_x)));
  manifest->uiapplication->icon = reinterpret_cast<icon_x*>
    (calloc(1, sizeof(icon_x)));
  manifest->uiapplication->label = reinterpret_cast<label_x*>
    (calloc(1, sizeof(label_x)));
  manifest->description = reinterpret_cast<description_x*>
    (calloc(1, sizeof(description_x)));
  manifest->uiapplication->appcontrol = nullptr;

  manifest->uiapplication->appid = strdup(data->id);
  manifest->uiapplication->type = strdup("webapp");

  // For wgt package use the long name if the short name is emty
  if ( ( data->short_name ) && ( strlen(data->short_name) ) )
    manifest->uiapplication->label->name = strdup(data->short_name);
  else
    manifest->uiapplication->label->name = strdup(data->name);

  manifest->uiapplication->icon->name = strdup(data->icon);
  manifest->uiapplication->next = nullptr;

  // context->manifest_data()->?? = strdup (data->api_version);
}


}  // namespace parse
}  // namespace wgt
