#include "manifest_x_utilities.h"

ManifestXUtilities::ManifestXUtilities(manifest_x* manifest):WrapperObjectBase(manifest){}

int ManifestXUtilities::PkgmgrParserProcessUsrManifestXForInstallation(
    const boost::filesystem::path& path, uid_t uid) {
  return pkgmgr_parser_process_usr_manifest_x_for_installation(
      mainStruct_, path.c_str(), uid);
}

int ManifestXUtilities::PkgmgrParserProcessManifestXForInstallation(
    const boost::filesystem::path& path) {
  return pkgmgr_parser_process_manifest_x_for_installation(mainStruct_,
                                                           path.c_str());
}

int ManifestXUtilities::PkgmgrParserProcessUsrManifestXForUpgrade(
    const boost::filesystem::path& path, uid_t uid) {
  return pkgmgr_parser_process_usr_manifest_x_for_upgrade(mainStruct_,
                                                          path.c_str(), uid);
}

int ManifestXUtilities::PkgmgrParserProcessManifestXForUpgrade(
    const boost::filesystem::path& path) {
  return pkgmgr_parser_process_manifest_x_for_upgrade(mainStruct_, path.c_str());
}

int ManifestXUtilities::PkgmgrParserProcessUsrManifestXForUninstallation(
    const boost::filesystem::path& path, uid_t uid) {
  return pkgmgr_parser_process_usr_manifest_x_for_uninstallation(
      mainStruct_, path.c_str(), uid);
}

int ManifestXUtilities::PkgmgrParserProcessManifestXForUninstallation(
    const boost::filesystem::path& path) {
  return pkgmgr_parser_process_manifest_x_for_uninstallation(mainStruct_,
                                                             path.c_str());
}
