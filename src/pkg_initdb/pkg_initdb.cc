// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/program_options.hpp>
#include <boost/system/error_code.hpp>
#include <pkgmgr_parser.h>
#include <pkgmgr_parser_db.h>
#include <pkgmgr-info.h>
#include <sys/types.h>
#include <tzplatform_config.h>

#include <string>
#include <iostream>

#include "common/utils/subprocess.h"

namespace bf = boost::filesystem;
namespace bs = boost::system;
namespace bpo = boost::program_options;
namespace ci = common_installer;

namespace {

const uid_t kUserRoot = 0;
const uid_t kGlobalUser = tzplatform_getuid(TZ_SYS_GLOBALAPP_USER);
const char kPkgInstallManifestPath[] = "/usr/bin/pkg-install-manifest";

bool IsGlobal(uid_t uid) {
  return uid == kUserRoot || uid == kGlobalUser;
}

void InitdbLoadDirectory(uid_t uid, const bf::path& directory, bool preload) {
  std::cerr << "Loading manifest files from " << directory << std::endl;
  for (bf::directory_iterator iter(directory); iter != bf::directory_iterator();
       ++iter) {
    if (!bf::is_regular_file(iter->path()))
      continue;
    std::cerr << "Running for: " << iter->path() << std::endl;
    ci::Subprocess pkg_install_manifest(kPkgInstallManifestPath);
    pkg_install_manifest.set_uid(uid);
    if (preload)
      pkg_install_manifest.Run("-x", iter->path().c_str(), "--preload");
    else
      pkg_install_manifest.Run("-x", iter->path().c_str());
    pkg_install_manifest.Wait();
  }
}

void RemoveOldDatabases(uid_t uid) {
  if (!IsGlobal(uid))
    tzplatform_set_user(uid);

  bs::error_code error;
  bf::path info_db_path(tzplatform_mkpath(
      IsGlobal(uid) ? TZ_SYS_DB : TZ_USER_DB, ".pkgmgr_parser.db"));
  bf::path info_db_journal_path(tzplatform_mkpath(
      IsGlobal(uid) ? TZ_SYS_DB : TZ_USER_DB, ".pkgmgr_parser.db-journal"));
  bf::path cert_db_path(tzplatform_mkpath(
      IsGlobal(uid) ? TZ_SYS_DB : TZ_USER_DB, ".pkgmgr_cert.db"));
  bf::path  cert_db_journal_path(tzplatform_mkpath(
      IsGlobal(uid) ? TZ_SYS_DB : TZ_USER_DB, ".pkgmgr_cert.db-journal"));

  bf::remove(info_db_path, error);
  if (error)
    std::cerr << info_db_path << " is not removed" << std::endl;
  bf::remove(info_db_journal_path, error);
  if (error)
    std::cerr << info_db_journal_path << " is not removed" << std::endl;
  bf::remove(cert_db_path, error);
  if (error)
    std::cerr << cert_db_path << " is not removed" << std::endl;
  bf::remove(cert_db_journal_path, error);
  if (error)
    std::cerr << cert_db_journal_path << " is not removed" << std::endl;

  tzplatform_reset_user();
}

}  // namespace

int main(int argc, char *argv[]) {
  if (getuid() != kUserRoot) {
    std::cerr << "This binary should be run as root user" << std::endl;
    return -1;
  }

  bpo::options_description options("Allowed options");
  options.add_options()
      ("uid,u", bpo::value<int>()->default_value(kUserRoot), "user id")
      ("help,h", "display this help message");
  bpo::variables_map opt_map;
  try {
    bpo::store(bpo::parse_command_line(argc, argv, options), opt_map);
    if (opt_map.count("help")) {
      std::cerr << options << std::endl;
      return -1;
    }
    bpo::notify(opt_map);
  } catch (const bpo::error& error) {
    std::cerr << error.what() << std::endl;
    return -1;
  }
  uid_t uid = opt_map["uid"].as<int>();

  RemoveOldDatabases(uid);

  int ret = pkgmgr_parser_create_and_initialize_db(uid);
  if (ret < 0) {
    std::cerr << "Cannot create db" << std::endl;
    return -1;
  }

  if (IsGlobal(uid)) {
    // RO location
    bf::path ro_dir(tzplatform_getenv(TZ_SYS_RO_PACKAGES));
    InitdbLoadDirectory(uid, ro_dir, true);

    // RW location
    bf::path rw_dir(tzplatform_getenv(TZ_SYS_RW_PACKAGES));
    InitdbLoadDirectory(uid, rw_dir, false);
  } else {
    // Specified user location
    tzplatform_set_user(uid);
    bf::path dir(tzplatform_getenv(TZ_USER_PACKAGES));
    InitdbLoadDirectory(uid, dir, false);
    tzplatform_reset_user();
  }

  return ret;
}
