/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#include "include/context_installer.h"

#include <tzplatform_config.h>
#include <unistd.h>

const char* ContextInstaller::GetApplicationPath() {
  return getuid() != tzplatform_getuid(TZ_SYS_GLOBALAPP_USER)
      ? tzplatform_getenv(TZ_USER_APP) : tzplatform_getenv(TZ_SYS_RW_APP);
}
