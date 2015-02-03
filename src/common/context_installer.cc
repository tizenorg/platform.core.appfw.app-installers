/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#include "common/context_installer.h"

#include <boost/filesystem.hpp>

#include <pkgmgr_installer.h>
#include <tzplatform_config.h>
#include <unistd.h>

#include <cstdlib>

namespace common_installer {

namespace fs = boost::filesystem;

ContextInstaller::ContextInstaller()
    : req_(PKGMGR_REQ_INVALID),
      manifest_(static_cast<manifest_x*>(calloc(1, sizeof(manifest_x)))),
      uid_(getuid()),
      config_data_(new ConfigData()) {
}

ContextInstaller::~ContextInstaller() {
  if (manifest_)
    pkgmgr_parser_free_manifest_xml(manifest_);
}

const char* ContextInstaller::GetRootApplicationPath() const {
  return uid_ != tzplatform_getuid(TZ_SYS_GLOBALAPP_USER)
      ? tzplatform_getenv(TZ_USER_APP) : tzplatform_getenv(TZ_SYS_RW_APP);
}

const char* ContextInstaller::GetApplicationPath() const {
  return (fs::path(GetRootApplicationPath()) / fs::path(pkgid())).c_str();
}


void ContextInstaller::set_new_temporary_pkgid() {
	static const char lookup[] =
		"abcdefghijklmnopqrstuvwxyz"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"0123456789";
	static const int len = 12;
	const size_t max_lookup = (sizeof(lookup)-1);

	std::string pkgid(len, 0);

	for(int i=0; i<len-1; i++) {
		pkgid.append(1, lookup[ rand() % max_lookup ]);
	}
	set_pkgid(pkgid);
}

}  // namespace common_installer
