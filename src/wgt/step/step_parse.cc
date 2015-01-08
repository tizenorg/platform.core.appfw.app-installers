/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#include "wgt/step/step_parse.h"

#include <pkgmgr/pkgmgr_parser.h>
#include <widget-manifest-parser/widget-manifest-parser.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "common/app_installer.h"
#include "common/context_installer.h"
#include "common/step/step.h"

namespace wgt {
namespace parse {

//TODO MAYBE fill later
//ConfigFileParser::ConfigFileParser(char * file) {
//}

int StepParse::process(common_installer::ContextInstaller* context) {
  if (!StepParse::Check(context->unpack_directory())) {
    std::cout << "No config.xml" << std::endl;
    return common_installer::APPINST_R_ERROR;
  }

  const ManifestData* data = nullptr;
  const char* error = nullptr;
  if (!ParseManifest(config_.c_str(), &data, &error)) {
    std::cout << "Parse failed. " <<  error << std::endl;
    if (!ReleaseData(data, error))
      std::cout << "Release data failed." << std::endl;
    return common_installer::APPINST_R_ERROR;
  }

  // Copy data from ManifestData to ContextInstaller
  context->config_data()->set_application_name(
      std::string(data->application_name));
  context->config_data()->set_required_version(
      std::string(data->required_version));
  for (unsigned int i = 0; i < data->privilege_count; ++i)
    context->config_data()->AddPrivilege(std::string(data->privilege_list[i]));

  //--- Test ---
  std::cout << "Read data -[ " << std::endl;
  std::cout << "  application_name = " <<  data->application_name << std::endl;
  std::cout << "  required_version = " <<  data->required_version << std::endl;
  std::cout << "  privilege_count  = " <<  data->privilege_count << std::endl;

  std::cout << "  privileges -[" << std::endl;
  for (unsigned int i = 0; i < data->privilege_count; ++i)
    std::cout << "    " << data->privilege_list[i] << std::endl;
  std::cout << "  ]-" << std::endl;
  std::cout << "]-" << std::endl;
  //--- End Test ---

  if (!ReleaseData(data, error)) {
    std::cout << "Release data failed." << std::endl;
    return common_installer::APPINST_R_ERROR;
  }

  return common_installer::APPINST_R_OK;
}

int StepParse::Check(const boost::filesystem::path& widget_path) {
  boost::filesystem::path config = widget_path;
  config /= "config.xml";

  std::cout << "config path: " << config << std::endl;

  if(!boost::filesystem::exists(config))
    return -1;

  config_ = config;
  return 1;
}

}  // namespace parse
}  // namespace wgt
