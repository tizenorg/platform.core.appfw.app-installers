// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/system/error_code.hpp>
#include <gtest/gtest.h>
#include <gtest/gtest-death-test.h>
#include <pkgmgr-info.h>
#include <signal.h>
#include <unistd.h>
#include <tzplatform_config.h>

#include <array>
#include <cstdio>

#include "common/backup_paths.h"
#include "common/pkgmgr_interface.h"
#include "common/step/step_fail.h"
#include "tpk/tpk_app_query_interface.h"
#include "tpk/tpk_installer.h"
#include "wgt/wgt_app_query_interface.h"
#include "wgt/wgt_installer.h"

#define SIZEOFARRAY(ARR)                                                       \
  sizeof(ARR) / sizeof(ARR[0])                                                 \

namespace bf = boost::filesystem;
namespace bs = boost::system;
namespace ci = common_installer;

namespace {

enum class RequestResult {
  NORMAL,
  FAIL,
  CRASH
};

enum class PackageType {
  TPK,
  WGT
};

class StepCrash : public ci::Step {
 public:
  using Step::Step;

  ci::Step::Status process() override {
    raise(SIGSEGV);
    return Status::OK;
  }
  ci::Step::Status clean() override { return ci::Step::Status::OK; }
  ci::Step::Status undo() override { return ci::Step::Status::OK; }
  ci::Step::Status precheck() override { return ci::Step::Status::OK; }
};

void RemoveAllRecoveryFiles() {
  bf::path root_path = getuid() != tzplatform_getuid(TZ_SYS_GLOBALAPP_USER)
      ? tzplatform_getenv(TZ_USER_APP) : tzplatform_getenv(TZ_SYS_RW_APP);
  for (auto& dir_entry : boost::make_iterator_range(
         bf::directory_iterator(root_path), bf::directory_iterator())) {
    if (bf::is_regular_file(dir_entry)) {
      if (dir_entry.path().string().find("/recovery") != std::string::npos) {
        bs::error_code error;
        bf::remove(dir_entry.path(), error);
      }
    }
  }
}

bf::path FindRecoveryFile() {
  bf::path root_path = getuid() != tzplatform_getuid(TZ_SYS_GLOBALAPP_USER)
      ? tzplatform_getenv(TZ_USER_APP) : tzplatform_getenv(TZ_SYS_RW_APP);
  for (auto& dir_entry : boost::make_iterator_range(
         bf::directory_iterator(root_path), bf::directory_iterator())) {
    if (bf::is_regular_file(dir_entry)) {
      if (dir_entry.path().string().find("/recovery") != std::string::npos) {
        return dir_entry.path();
      }
    }
  }
  return {};
}

bool IsPackageInstalled(const std::string& pkg_id) {
  pkgmgrinfo_pkginfo_h handle;
  int ret = pkgmgrinfo_pkginfo_get_usr_pkginfo(pkg_id.c_str(), getuid(),
                                               &handle);
  if (ret != PMINFO_R_OK)
    return false;
  pkgmgrinfo_pkginfo_destroy_pkginfo(handle);
  return true;
}

bool ValidateFileContentInPackage(const std::string& pkgid,
                                  const std::string& relative,
                                  const std::string& expected) {
  bf::path root_path = getuid() != tzplatform_getuid(TZ_SYS_GLOBALAPP_USER)
      ? tzplatform_getenv(TZ_USER_APP) : tzplatform_getenv(TZ_SYS_RW_APP);
  bf::path file_path = root_path / pkgid / relative;
  if (!bf::exists(file_path)) {
    LOG(ERROR) << file_path << " doesn't exist";
    return false;
  }
  FILE* handle = fopen(file_path.c_str(), "r");
  if (!handle) {
    LOG(ERROR) << file_path << " cannot  be open";
    return false;
  }
  std::string content;
  std::array<char, 200> buffer;
  while (fgets(buffer.data(), buffer.size(), handle)) {
    content += buffer.data();
  }
  fclose(handle);
  return content == expected;
}

void ValidatePackageFS(const std::string& pkgid, const std::string& appid,
                       PackageType type) {
  bf::path root_path = getuid() != tzplatform_getuid(TZ_SYS_GLOBALAPP_USER)
      ? tzplatform_getenv(TZ_USER_APP) : tzplatform_getenv(TZ_SYS_RW_APP);
  bf::path package_path = root_path / pkgid;
  bf::path binary_path = package_path / "bin" / appid;
  bf::path data_path = package_path / "data";
  bf::path shared_path = package_path / "shared";
  ASSERT_TRUE(bf::exists(root_path));
  ASSERT_TRUE(bf::exists(package_path));
  ASSERT_TRUE(bf::exists(binary_path));
  ASSERT_TRUE(bf::exists(data_path));
  ASSERT_TRUE(bf::exists(shared_path));

  bf::path manifest_path =
      bf::path(getUserManifestPath(getuid())) / (pkgid + ".xml");
  bf::path icon_path = bf::path(getIconPath(getuid())) / (appid + ".png");
  ASSERT_TRUE(bf::exists(manifest_path));
  ASSERT_TRUE(bf::exists(icon_path));

  if (type == PackageType::WGT) {
    bf::path widget_root_path = package_path / "res" / "wgt";
    bf::path config_path = widget_root_path / "config.xml";
    ASSERT_TRUE(bf::exists(widget_root_path));
    ASSERT_TRUE(bf::exists(config_path));
  }

  // backups should not exist
  bf::path package_backup = ci::GetBackupPathForPackagePath(package_path);
  bf::path manifest_backup = ci::GetBackupPathForManifestFile(manifest_path);
  bf::path icon_backup = ci::GetBackupPathForIconFile(icon_path);
  ASSERT_FALSE(bf::exists(package_backup));
  ASSERT_FALSE(bf::exists(manifest_backup));
  ASSERT_FALSE(bf::exists(icon_backup));
}

void PackageCheckCleanup(const std::string& pkgid, const std::string& appid) {
  bf::path root_path = getuid() != tzplatform_getuid(TZ_SYS_GLOBALAPP_USER)
      ? tzplatform_getenv(TZ_USER_APP) : tzplatform_getenv(TZ_SYS_RW_APP);
  bf::path package_path = root_path / pkgid;
  ASSERT_FALSE(bf::exists(package_path));

  bf::path manifest_path =
      bf::path(getUserManifestPath(getuid())) / (pkgid + ".xml");
  bf::path icon_path = bf::path(getIconPath(getuid())) / (appid + ".png");
  ASSERT_FALSE(bf::exists(manifest_path));
  ASSERT_FALSE(bf::exists(icon_path));

  // backups should not exist
  bf::path package_backup = ci::GetBackupPathForPackagePath(package_path);
  bf::path manifest_backup = ci::GetBackupPathForManifestFile(manifest_path);
  bf::path icon_backup = ci::GetBackupPathForIconFile(icon_path);
  ASSERT_FALSE(bf::exists(package_backup));
  ASSERT_FALSE(bf::exists(manifest_backup));
  ASSERT_FALSE(bf::exists(icon_backup));
}

void ValidatePackage(const std::string& pkgid, const std::string& appid,
                     PackageType type) {
  ASSERT_TRUE(IsPackageInstalled(pkgid));
  ValidatePackageFS(pkgid, appid, type);
}

void CheckPackageNonExistance(const std::string& pkgid,
                              const std::string& appid) {
  ASSERT_FALSE(IsPackageInstalled(pkgid));
  PackageCheckCleanup(pkgid, appid);
}

std::unique_ptr<ci::AppQueryInterface> CreateQueryInterface(PackageType type) {
  std::unique_ptr<ci::AppQueryInterface> query_interface;
  switch (type) {
  case PackageType::TPK:
    query_interface.reset(new tpk::TpkAppQueryInterface());
    break;
  case PackageType::WGT:
    query_interface.reset(new wgt::WgtAppQueryInterface());
    break;
  }
  return query_interface;
}

std::unique_ptr<ci::AppInstaller> CreateInstaller(ci::PkgMgrPtr pkgmgr,
                                                  PackageType type) {
  std::unique_ptr<ci::AppInstaller> installer;
  switch (type) {
  case PackageType::TPK:
    installer.reset(new tpk::TpkInstaller(pkgmgr));
    break;
  case PackageType::WGT:
    installer.reset(new wgt::WgtInstaller(pkgmgr));
    break;
  }
  return installer;
}

ci::AppInstaller::Result RunInstallerWithPkgrmgr(ci::PkgMgrPtr pkgmgr,
                                                 PackageType type,
                                                 RequestResult mode) {
  std::unique_ptr<ci::AppInstaller> installer = CreateInstaller(pkgmgr, type);
  switch (mode) {
  case RequestResult::FAIL:
    installer->AddStep<ci::configuration::StepFail>();
    break;
  case RequestResult::CRASH:
    installer->AddStep<StepCrash>();
  default:
    break;
  }
  return installer->Run();
}

ci::AppInstaller::Result Install(const bf::path& path,
                                 PackageType type,
                                 RequestResult mode = RequestResult::NORMAL) {
  const char* argv[] = {"", "-i", path.c_str()};
  std::unique_ptr<ci::AppQueryInterface> query_interface =
      CreateQueryInterface(type);
  auto pkgmgr =
      ci::PkgMgrInterface::Create(SIZEOFARRAY(argv), const_cast<char**>(argv),
                                  query_interface.get());
  if (!pkgmgr) {
    LOG(ERROR) << "Failed to initialize pkgmgr interface";
    return ci::AppInstaller::Result::UNKNOWN;
  }
  return RunInstallerWithPkgrmgr(pkgmgr, type, mode);
}

ci::AppInstaller::Result Update(const bf::path& path_old,
                                const bf::path& path_new,
                                PackageType type,
                                RequestResult mode = RequestResult::NORMAL) {
  if (Install(path_old, type) != ci::AppInstaller::Result::OK) {
    LOG(ERROR) << "Failed to install application. Cannot update";
    return ci::AppInstaller::Result::UNKNOWN;
  }
  return Install(path_new, type, mode);
}

ci::AppInstaller::Result Uninstall(const std::string& pkgid,
                                   PackageType type,
                                   RequestResult mode = RequestResult::NORMAL) {
  const char* argv[] = {"", "-d", pkgid.c_str()};
  std::unique_ptr<ci::AppQueryInterface> query_interface =
      CreateQueryInterface(type);
  auto pkgmgr =
      ci::PkgMgrInterface::Create(SIZEOFARRAY(argv), const_cast<char**>(argv),
                                  query_interface.get());
  if (!pkgmgr) {
    LOG(ERROR) << "Failed to initialize pkgmgr interface";
    return ci::AppInstaller::Result::UNKNOWN;
  }
  return RunInstallerWithPkgrmgr(pkgmgr, type, mode);
}

ci::AppInstaller::Result Reinstall(const bf::path& path,
                                   const bf::path& delta_dir,
                                   PackageType type,
                                   RequestResult mode = RequestResult::NORMAL) {
  if (Install(path, type) != ci::AppInstaller::Result::OK) {
    LOG(ERROR) << "Failed to install application. Cannot perform RDS";
    return ci::AppInstaller::Result::UNKNOWN;
  }
  const char* argv[] = {"", "-r", delta_dir.c_str()};
  std::unique_ptr<ci::AppQueryInterface> query_interface =
      CreateQueryInterface(type);
  auto pkgmgr =
      ci::PkgMgrInterface::Create(SIZEOFARRAY(argv), const_cast<char**>(argv),
                                  query_interface.get());
  if (!pkgmgr) {
    LOG(ERROR) << "Failed to initialize pkgmgr interface";
    return ci::AppInstaller::Result::UNKNOWN;
  }
  return RunInstallerWithPkgrmgr(pkgmgr, type, mode);
}

ci::AppInstaller::Result Recover(const bf::path& recovery_file,
                                 PackageType type,
                                 RequestResult mode = RequestResult::NORMAL) {
  const char* argv[] = {"", "-e", recovery_file.c_str()};
  std::unique_ptr<ci::AppQueryInterface> query_interface =
      CreateQueryInterface(type);
  auto pkgmgr =
      ci::PkgMgrInterface::Create(SIZEOFARRAY(argv), const_cast<char**>(argv),
                                  query_interface.get());
  if (!pkgmgr) {
    LOG(ERROR) << "Failed to initialize pkgmgr interface";
    return ci::AppInstaller::Result::UNKNOWN;
  }
  return RunInstallerWithPkgrmgr(pkgmgr, type, mode);
}

}  // namespace

namespace common_installer {

// TODO(t.iwanek): before tests cleanup...
class SmokeTest : public testing::Test {
};

TEST_F(SmokeTest, InstallationMode) {
  bf::path path = "/usr/share/app-installers-ut/test_samples/smoke/InstallationMode.wgt";  // NOLINT
  std::string pkgid = "smokeapp03";
  std::string appid = "smokeapp03.InstallationMode";
  ASSERT_EQ(Install(path, PackageType::WGT), ci::AppInstaller::Result::OK);
  ValidatePackage(pkgid, appid, PackageType::WGT);
}

TEST_F(SmokeTest, UpdateMode) {
  bf::path path_old = "/usr/share/app-installers-ut/test_samples/smoke/UpdateMode.wgt";  // NOLINT
  bf::path path_new = "/usr/share/app-installers-ut/test_samples/smoke/UpdateMode_2.wgt";  // NOLINT
  std::string pkgid = "smokeapp04";
  std::string appid = "smokeapp04.UpdateMode";
  ASSERT_EQ(Update(path_old, path_new, PackageType::WGT),
            ci::AppInstaller::Result::OK);
  ValidatePackage(pkgid, appid, PackageType::WGT);

  ASSERT_TRUE(ValidateFileContentInPackage(pkgid, "res/wgt/VERSION", "2\n"));
}

TEST_F(SmokeTest, DeinstallationMode) {
  bf::path path = "/usr/share/app-installers-ut/test_samples/smoke/DeinstallationMode.wgt";  // NOLINT
  std::string pkgid = "smokeapp05";
  std::string appid = "smokeapp05.DeinstallationMode";
  ASSERT_EQ(Install(path, PackageType::WGT),
            ci::AppInstaller::Result::OK);
  ASSERT_EQ(Uninstall(pkgid, PackageType::WGT), ci::AppInstaller::Result::OK);
  CheckPackageNonExistance(pkgid, appid);
}

TEST_F(SmokeTest, RDSMode) {
  bf::path path = "/usr/share/app-installers-ut/test_samples/smoke/RDSMode.wgt";  // NOLINT
  std::string delta_directory = "/usr/share/app-installers-ut/test_samples/smoke/delta_dir/"; // NOLINT
  std::string pkgid = "smokeapp11";
  std::string appid = "smokeapp11.RDSMode";
  ASSERT_EQ(Reinstall(path, delta_directory, PackageType::WGT),
            ci::AppInstaller::Result::OK);
  ValidatePackage(pkgid, appid, PackageType::WGT);

  // Check delta modifications
  bf::path root_path = getuid() != tzplatform_getuid(TZ_SYS_GLOBALAPP_USER)
      ? tzplatform_getenv(TZ_USER_APP) : tzplatform_getenv(TZ_SYS_RW_APP);
  ASSERT_FALSE(bf::exists(root_path / pkgid / "res" / "wgt" / "DELETED"));
  ASSERT_TRUE(bf::exists(root_path / pkgid / "res" / "wgt" / "ADDED"));
  ValidateFileContentInPackage(pkgid, "res/wgt/MODIFIED", "2\n");
}

TEST_F(SmokeTest, RecoveryMode_ForInstallation) {
  bf::path path = "/usr/share/app-installers-ut/test_samples/smoke/RecoveryMode_ForInstallation.wgt";  // NOLINT
  ASSERT_DEATH(Install(path, PackageType::WGT, RequestResult::CRASH), ".*");

  std::string pkgid = "smokeapp09";
  std::string appid = "smokeapp09.RecoveryModeForInstallation";
  bf::path recovery_file = FindRecoveryFile();
  ASSERT_FALSE(recovery_file.empty());
  ASSERT_EQ(Recover(recovery_file, PackageType::WGT),
            ci::AppInstaller::Result::OK);
  CheckPackageNonExistance(pkgid, appid);
}

TEST_F(SmokeTest, RecoveryMode_ForUpdate) {
  bf::path path_old = "/usr/share/app-installers-ut/test_samples/smoke/RecoveryMode_ForUpdate.wgt";  // NOLINT
  bf::path path_new = "/usr/share/app-installers-ut/test_samples/smoke/RecoveryMode_ForUpdate_2.wgt";  // NOLINT
  RemoveAllRecoveryFiles();
  ASSERT_DEATH(Update(path_old, path_new, PackageType::WGT,
                      RequestResult::CRASH), ".*");

  std::string pkgid = "smokeapp10";
  std::string appid = "smokeapp10.RecoveryModeForUpdate";
  bf::path recovery_file = FindRecoveryFile();
  ASSERT_FALSE(recovery_file.empty());
  ASSERT_EQ(Recover(recovery_file, PackageType::WGT),
            ci::AppInstaller::Result::OK);
  ValidatePackage(pkgid, appid, PackageType::WGT);

  ASSERT_TRUE(ValidateFileContentInPackage(pkgid, "res/wgt/VERSION", "1\n"));
}

TEST_F(SmokeTest, InstallationMode_GoodSignature) {
  bf::path path = "/usr/share/app-installers-ut/test_samples/smoke/InstallationMode_GoodSignature.wgt";  // NOLINT
  ASSERT_EQ(Install(path, PackageType::WGT), ci::AppInstaller::Result::OK);
}

TEST_F(SmokeTest, InstallationMode_WrongSignature) {
  bf::path path = "/usr/share/app-installers-ut/test_samples/smoke/InstallationMode_WrongSignature.wgt";  // NOLINT
  ASSERT_EQ(Install(path, PackageType::WGT), ci::AppInstaller::Result::ERROR);
}

TEST_F(SmokeTest, InstallationMode_Rollback) {
  bf::path path = "/usr/share/app-installers-ut/test_samples/smoke/InstallationMode_Rollback.wgt";  // NOLINT
  std::string pkgid = "smokeapp06";
  std::string appid = "smokeapp06.InstallationModeRollback";
  ASSERT_EQ(Install(path, PackageType::WGT, RequestResult::FAIL),
            ci::AppInstaller::Result::ERROR);
  CheckPackageNonExistance(pkgid, appid);
}

TEST_F(SmokeTest, UpdateMode_Rollback) {
  bf::path path_old = "/usr/share/app-installers-ut/test_samples/smoke/UpdateMode_Rollback.wgt";  // NOLINT
  bf::path path_new = "/usr/share/app-installers-ut/test_samples/smoke/UpdateMode_Rollback_2.wgt";  // NOLINT
  std::string pkgid = "smokeapp07";
  std::string appid = "smokeapp07.UpdateModeRollback";
  ASSERT_EQ(Update(path_old, path_new, PackageType::WGT, RequestResult::FAIL),
                   ci::AppInstaller::Result::ERROR);
  ValidatePackage(pkgid, appid, PackageType::WGT);

  ASSERT_TRUE(ValidateFileContentInPackage(pkgid, "res/wgt/VERSION", "1\n"));
}

TEST_F(SmokeTest, DeinstallationMode_Rollback) {
  bf::path path = "/usr/share/app-installers-ut/test_samples/smoke/DeinstallationMode_Rollback.wgt";  // NOLINT
  std::string pkgid = "smokeapp08";
  std::string appid = "smokeapp08.DeinstallationModeRollback";
  ASSERT_EQ(Install(path, PackageType::WGT), ci::AppInstaller::Result::OK);
  ASSERT_EQ(Uninstall(pkgid, PackageType::WGT, RequestResult::FAIL),
                      ci::AppInstaller::Result::ERROR);
  ValidatePackage(pkgid, appid, PackageType::WGT);
}

TEST_F(SmokeTest, InstallationMode_Tpk) {
  bf::path path = "/usr/share/app-installers-ut/test_samples/smoke/InstallationMode_Tpk.tpk";  // NOLINT
  std::string pkgid = "smokeapp12";
  std::string appid = "smokeapp12.InstallationModeTpk";
  ASSERT_EQ(Install(path, PackageType::TPK), ci::AppInstaller::Result::OK);
  ValidatePackage(pkgid, appid, PackageType::TPK);
}

TEST_F(SmokeTest, UpdateMode_Tpk) {
  bf::path path_old = "/usr/share/app-installers-ut/test_samples/smoke/UpdateMode_Tpk.tpk";  // NOLINT
  bf::path path_new = "/usr/share/app-installers-ut/test_samples/smoke/UpdateMode_Tpk_2.tpk";  // NOLINT
  std::string pkgid = "smokeapp13";
  std::string appid = "smokeapp13.UpdateModeTpk";
  ASSERT_EQ(Update(path_old, path_new, PackageType::TPK),
            ci::AppInstaller::Result::OK);
  ValidatePackage(pkgid, appid, PackageType::TPK);

  ASSERT_TRUE(ValidateFileContentInPackage(pkgid, "VERSION", "2\n"));
}

TEST_F(SmokeTest, DeinstallationMode_Tpk) {
  bf::path path = "/usr/share/app-installers-ut/test_samples/smoke/DeinstallationMode_Tpk.tpk";  // NOLINT
  std::string pkgid = "smokeapp14";
  std::string appid = "smokeapp14.DeinstallationModeTpk";
  ASSERT_EQ(Install(path, PackageType::TPK),
            ci::AppInstaller::Result::OK);
  ASSERT_EQ(Uninstall(pkgid, PackageType::TPK), ci::AppInstaller::Result::OK);
  CheckPackageNonExistance(pkgid, appid);
}

}  // namespace common_installer
