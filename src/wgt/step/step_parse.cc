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

namespace parser = common_installer::widget_manifest_parser;

namespace wgt {
namespace parse {

common_installer::Step::Status StepParse::process() {
  if (!StepParse::Check(context_->unpacked_dir_path())) {
    LOG(ERROR) << "No config.xml";
    return common_installer::Step::Status::ERROR;
  }

  parser_.reset(new parser::WidgetManifestParser(config_));
  if (!parser_->IsValid()) {
    LOG(ERROR) << "[Parse] Parse failed. " <<  parser_->GetErrorMessage();
    return common_installer::Step::Status::ERROR;
  }
  const manifest_x* manifest = context_->manifest_data();
  if (!parser_->StoreManifest(const_cast<manifest_x*>(manifest))) {
    LOG(ERROR) << "[Parse] Storing manifest_x failed. "
               <<  parser_->GetErrorMessage();
    return common_installer::Step::Status::ERROR;
  }

  // Copy data from ManifestData to ContextInstaller
  context_->config_data()->set_application_name(
      std::string(manifest->uiapplication->label->name));
  context_->config_data()->set_required_version(
      std::string(parser_->GetRequiredAPIVersion()));
  context_->set_pkgid(
      std::string(manifest->package));

  LOG(DEBUG) << " Read data -[ ";
  LOG(DEBUG) << "  package     = " <<  manifest->package;
  LOG(DEBUG) << "  id          = " <<  manifest->mainapp_id;
  LOG(DEBUG) << "  name        = " <<  manifest->description->name;
  LOG(DEBUG) << "  short_name  = " <<  parser_->GetShortName();
  LOG(DEBUG) << "  version     = " <<  manifest->version;
  LOG(DEBUG) << "  icon        = " <<  manifest->uiapplication->icon->name;
  LOG(DEBUG) << "  api_version = " <<  parser_->GetRequiredAPIVersion();
  LOG(DEBUG) << "  privileges -[";
  privilege_x* first = manifest->privileges->privilege;                                                                                                                                                      
    if (first) {                                                                                                                                                                                               
      privilege_x* it = first;                                                                                                                                                                                 
      do {                                                                                                                                                                                                     
        LOG(DEBUG) << "    " << it->text;                                                                                                                                                          
      } while (((it = it->next) != first) && it != nullptr);                                                                                                                                                   
    }
  LOG(DEBUG) << "  ]-";
  LOG(DEBUG) << "]-";

  //--- End Test ---

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

}  // namespace parse
}  // namespace wgt
