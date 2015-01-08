/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#include "wgt/step/step_parse.h"

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <pkgmgr/pkgmgr_parser.h>
#include <widget-manifest-parser/widget-manifest-parser.h>

#include "common/app_installer.h"
#include "common/context_installer.h"
#include "common/step/step.h"

namespace {
//Already defined in <pkgmgr/pkgmgr_parser.h>
//API manifest_x *pkgmgr_parser_process_manifest_xml(const char *manifest) {
//  _LOGD("parsing start pkgmgr_parser_process_manifest_xml\n");
//  xmlTextReaderPtr reader;
//  manifest_x *mfx = NULL;
//
//  reader = xmlReaderForFile(manifest, NULL, 0);
//  if (reader) {
//    mfx = malloc(sizeof(manifest_x));
//    if (mfx) {
//      memset(mfx, '\0', sizeof(manifest_x));
//      if (__process_manifest(reader, mfx, GLOBAL_USER) < 0) {
//        _LOGD("Parsing Failed\n");
//        pkgmgr_parser_free_manifest_xml(mfx);
//        mfx = NULL;
//      } else {
//        _LOGD("Parsing Success\n");
//      }
//    } else {
//      _LOGD("Memory allocation error\n");
//    }
//    xmlFreeTextReader(reader);
//  } else {
//    _LOGD("Unable to create xml reader\n");
//  }
//  return mfx;
//}

}  // namespace

namespace wgt {
namespace parse {

//TODO MAYBE fill later
//ConfigFileParser::ConfigFileParser(char * file) {
//}

StepParse::StepParse() {
  config_ = "";
}

int StepParse::process(common_installer::ContextInstaller* context) {
  if (!StepParse::Check(context->unpack_directory())) {
    std::cout << "No config.xml" << std::endl;
    return -1;
  }

  const ManifestData* data = nullptr;
  const char* error = nullptr;
  if (!ParseManifest(config_.c_str(), &data, &error)) {
    std::cout << "Parse failed. " <<  error << std::endl;
    if (!ReleaseData(data, error))
      std::cout << "Release data failed." << std::endl;
    return -1;
  }

  std::cout << "Read data -[ " << std::endl;
  std::cout << "  application_name = " <<  data->application_name << std::endl;
  std::cout << "  required_version = " <<  data->required_version << std::endl;
  std::cout << "  privilege_count  = " <<  data->privilege_count << std::endl;

  std::cout << "  privileges -[" << std::endl;
  for (unsigned int i = 0; i < data->privilege_count; ++i)
    std::cout << "    " << data->privilege_list[i] << std::endl;
  std::cout << "  ]-" << std::endl;
  std::cout << "]-" << std::endl;

  if (!ReleaseData(data, error)) {
    std::cout << "Release data failed." << std::endl;
    return -1;
  }

  return 0;
}

int StepParse::Check(const boost::filesystem::path& widget_path) {
  boost::filesystem::path config = widget_path;
  config += "config.xml";

  std::cout << "config path: " << config << std::endl;

  if(!boost::filesystem::exists(config))
    return -1;

  config_ = config;
  return 1;
}

}  // namespace parse
}  // namespace wgt
