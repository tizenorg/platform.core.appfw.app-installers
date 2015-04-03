/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#include "common/context_installer.h"
#include <boost/filesystem.hpp>
#include <tzplatform_config.h>
#include <unistd.h>
#include <cstdlib>

namespace common_installer {

namespace fs = boost::filesystem;

ContextInstaller::ContextInstaller()
    : request_type(PkgMgrInterface::Type::Unknown),
      manifest_data(static_cast<manifest_x*>(calloc(1, sizeof(manifest_x)))),
      uid(getuid()) {

    std::string root_app_path =
        uid.get() != tzplatform_getuid(TZ_SYS_GLOBALAPP_USER)
          ? tzplatform_getenv(TZ_USER_APP)
          : tzplatform_getenv(TZ_SYS_RW_APP);

    root_application_path.set(root_app_path);
    application_path.set(
        (fs::path(root_app_path) / fs::path(pkgid.get())).native());
}

ContextInstaller::~ContextInstaller() {
  if (manifest_data.get())
    pkgmgr_parser_free_manifest_xml(manifest_data.get());
}

}  // namespace common_installer
