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
#include "wgt/step/step_parse.h"
#include "common/step/step_generate_xml.h"
#include "common/step/step_record.h"

//uninstall includes:
#include "common/step/step_parse.h"
#include "common/step/step_remove.h"
#include "common/step/step_unregister.h"

int main(int argc, char **argv) {
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
      common_installer::signature::StepSignature* step_signature =
              new common_installer::signature::StepSignature;
      wgt::parse::StepParse* step_parse = new
              wgt::parse::StepParse();
      common_installer::copy::StepCopy* step_copy =
          new common_installer::copy::StepCopy();
      common_installer::generate_xml::StepGenerateXml* step_xml =
          new common_installer::generate_xml::StepGenerateXml();
      common_installer::record::StepRecord* step_record =
          new common_installer::record::StepRecord();

      installer->AddStep(step_unpack);
      installer->AddStep(step_signature);
      installer->AddStep(step_parse);
      installer->AddStep(step_copy);
      installer->AddStep(step_xml);
      installer->AddStep(step_record);
      installer->Run();

      delete step_unpack;
      delete step_signature;
      delete step_parse;
      delete step_copy;
      delete installer;
      delete step_xml;
      delete step_record;
      break;
    }
    case PKGMGR_REQ_UNINSTALL: {
      common_installer::AppInstaller* installer =
          new common_installer::AppInstaller(PKGMGR_REQ_UNINSTALL,
              "", pkgmgr_installer_get_request_info(pi));

      common_installer::parse::StepParse* step_parse =
          new common_installer::parse::StepParse();
      common_installer::remove::StepRemove* step_remove =
          new common_installer::remove::StepRemove();
      common_installer::unregister::StepUnregister* step_unregister =
          new common_installer::unregister::StepUnregister();

      installer->AddStep(step_parse);
      installer->AddStep(step_unregister);
      installer->AddStep(step_remove);

      installer->Run();
  
      delete step_remove;
      delete step_unregister;
      delete step_parse;
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
