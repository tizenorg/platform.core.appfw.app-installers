/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <pkgmgr_installer.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <memory>

#include "common/app_installer.h"
#include "common/step/step_unzip.h"
#include "common/step/step_signature.h"
#include "common/step/step_copy.h"

#include <iostream>
#include <widget-manifest-parser/widget-manifest-parser.h>

bool ParserTest(int argc, char **argv) {
  if (argc != 3)
    return true;

  std::string test_switch = argv[1];
  std::string file = argv[2];

  if (test_switch != "-pt")
    return true;

  std::cout << "Parsing file " << file << std::endl;

  const ManifestData* data = nullptr;
  const char* error = nullptr;
  if (!ParseManifest(file.c_str(), &data, &error)) {
    std::cout << "Parse failed. " <<  error << std::endl;
    if (!ReleaseData(data, error))
      std::cout << "Release data failed." << std::endl;
    return false;
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
    return false;
  }

  return false;
}

int main(int argc, char **argv) {
  if (!ParserTest(argc, argv))
    return 0;

  /* get request data */
  pkgmgr_installer *pi = pkgmgr_installer_new();
  if (!pi)
    return ENOMEM;

  int result = pkgmgr_installer_receive_request(pi, argc, argv);
  if (result) {
    pkgmgr_installer_free(pi);
    return -result;
  }

  /* treat the request */
  switch (pkgmgr_installer_get_request_type(pi)) {
    case PKGMGR_REQ_INSTALL: {
      common_installer::AppInstaller* installer =
          new common_installer::AppInstaller(PKGMGR_REQ_INSTALL,
              pkgmgr_installer_get_request_info(pi), "");

      common_installer::unzip::StepUnzip* step_unpack =
          new common_installer::unzip::StepUnzip();
      installer->AddStep(step_unpack);

      // FIXME: unique_ptr because steps are not freed in installer.
      std::unique_ptr<common_installer::signature::StepSignature>
          step_signature(
              new common_installer::signature::StepSignature);
      installer->AddStep(step_signature.get());

      common_installer::copy::StepCopy* step_copy =
          new common_installer::copy::StepCopy();
      installer->AddStep(step_copy);

      installer->Run();

      delete step_unpack;
      delete step_copy;
      break;
    }
    case PKGMGR_REQ_UNINSTALL: {
      break;
    }
    default: {
      /* unsupported operation */
      result = EINVAL;
      break;
    }
  }
  pkgmgr_installer_free(pi);
  return result;
}
