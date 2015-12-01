/* Copyright 2015 Samsung Electronics, license APACHE-2.0, see LICENSE file */

#include <iostream>

#include "common/pkgmgr_interface.h"
#include "manifest_parser/utils/logging.h"
#include "tpk/tpk_app_query_interface.h"
#include "tpk/tpk_installer.h"

namespace ci = common_installer;

int main(const int argc, char* argv[]) {
  tpk::TpkAppQueryInterface interface;
  ci::PkgMgrPtr pkgmgr = ci::PkgMgrInterface::Create(argc, argv, &interface);
  if (!pkgmgr) {
    LOG(ERROR) << "Failed to create pkgmgr interface";
    return -1;
  }
  tpk::TpkInstaller t(pkgmgr);
  if (t.Run() != ci::AppInstaller::Result::OK) {
    LOG(ERROR) << "TpkInstaller run failure";
    return -1;
  }
  return 0;
}
