/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#include "wgt/step/step_parse.h"

#include <string.h>

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
    std::cout << "[Parse] No config.xml" << std::endl;
    return common_installer::APPINST_R_ERROR;
  }

  const ManifestData* data = nullptr;
  const char* error = nullptr;
  if (!ParseManifest(config_.c_str(), &data, &error)) {
    std::cout << "[Parse] Parse failed. " <<  error << std::endl;
    if (!ReleaseData(data, error))
      std::cout << "[Parse] Release data failed." << std::endl;
    return common_installer::APPINST_R_ERROR;
  }

  // Copy data from ManifestData to ContextInstaller
  context->config_data()->set_application_name(
      std::string(data->application_name));
  context->config_data()->set_required_version(
      std::string(data->required_version));
  unsigned int privilege_len;
  privileges_x *ctx_privileges = context->manifest_data()->privileges;

  if(!ctx_privileges) {
    privileges_x *privileges = reinterpret_cast<privileges_x*>(
        malloc(sizeof(privileges_x)));
    if(!privileges)
      return common_installer::APPINST_R_ERROR;

    memset(privileges, '\0', sizeof(privileges_x));
    LISTADD(ctx_privileges, privileges);
    context->manifest_data()->privileges = ctx_privileges;
  }
  for (unsigned int i = 0; i < data->privilege_count; ++i) {
    privilege_x *privilege = reinterpret_cast<privilege_x*>(
        malloc(sizeof(privilege_x)));
    if(!privilege)
      return common_installer::APPINST_R_ERROR;

    privilege_len = strlen(data->privilege_list[i]);
    char* tmp = reinterpret_cast<char*>(malloc(privilege_len + 1));
    if(!tmp)
      return common_installer::APPINST_R_ERROR;

    strcpy(tmp, data->privilege_list[i]);
    memset(privilege, '\0', sizeof(privilege_x));
    privilege->text = const_cast<const char*>(tmp);
    LISTADD(ctx_privileges->privilege, privilege);
  }

  //--- Test ---
  std::cout << "[Parse] Read data -[ " << std::endl;
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
    std::cout << "[Parse] Release data failed." << std::endl;
    return common_installer::APPINST_R_ERROR;
  }

  return common_installer::APPINST_R_OK;
}

bool StepParse::Check(const boost::filesystem::path& widget_path) {
  boost::filesystem::path config = widget_path;
  config /= "config.xml";

  std::cout << "[Parse] config.xml path: " << config << std::endl;

  if(!boost::filesystem::exists(config))
    return false;

  config_ = config;
  return true;
}

}  // namespace parse
}  // namespace wgt
