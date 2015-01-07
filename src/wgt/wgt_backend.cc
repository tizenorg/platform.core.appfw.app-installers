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
#include "common/step/step_generate_xml.h"
#include "common/step/step_record.h"
#include "wgt/step/step_parse.h"

namespace ci = common_installer;

int main(int argc, char** argv) {
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
      ci::AppInstaller installer(
          PKGMGR_REQ_INSTALL,
          pkgmgr_installer_get_request_info(pi),
          "");

      installer.AddStep<ci::unzip::StepUnzip>();
      installer.AddStep<ci::signature::StepSignature>();
      installer.AddStep<wgt::parse::StepParse>();
      installer.AddStep<ci::copy::StepCopy>();
      installer.AddStep<ci::generate_xml::StepGenerateXml>();
      installer.AddStep<ci::record::StepRecord>();

      installer.Run();
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
