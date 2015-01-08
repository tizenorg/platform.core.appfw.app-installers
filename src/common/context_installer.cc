/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#include "common/context_installer.h"

#include <pkgmgr_installer.h>
#include <tzplatform_config.h>
#include <unistd.h>

namespace common_installer {

ContextInstaller::ContextInstaller() : req_(PKGMGR_REQ_INVALID),
                                       uid_(getuid()) {
  config_data_.reset(new ConfigData());
  manifest_ = new manifest_x();
}

ContextInstaller::~ContextInstaller() {
  if (manifest_)
    pkgmgr_parser_free_manifest_xml(manifest_);
}

const char* ContextInstaller::GetApplicationPath() {
  return uid_ != tzplatform_getuid(TZ_SYS_GLOBALAPP_USER)
      ? tzplatform_getenv(TZ_USER_APP) : tzplatform_getenv(TZ_SYS_RW_APP);
}

}  // namespace common_installer
