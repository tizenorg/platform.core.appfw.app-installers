#include "manifest_x_utilities.h"

ManifestXUtilities::ManifestXUtilities(manifest_x* manifest):WrapperObjectBase(manifest){}

int ManifestXUtilities::PkgmgrParserProcessUsrManifestXForInstallation(
    const boost::filesystem::path& path, uid_t uid) {
  return pkgmgr_parser_process_usr_manifest_x_for_installation(
      object_, path.c_str(), uid);
}

int ManifestXUtilities::PkgmgrParserProcessManifestXForInstallation(
    const boost::filesystem::path& path) {
  return pkgmgr_parser_process_manifest_x_for_installation(object_,
                                                           path.c_str());
}

int ManifestXUtilities::PkgmgrParserProcessUsrManifestXForUpgrade(
    const boost::filesystem::path& path, uid_t uid) {
  return pkgmgr_parser_process_usr_manifest_x_for_upgrade(object_,
                                                          path.c_str(), uid);
}

int ManifestXUtilities::PkgmgrParserProcessManifestXForUpgrade(
    const boost::filesystem::path& path) {
  return pkgmgr_parser_process_manifest_x_for_upgrade(object_, path.c_str());
}

int ManifestXUtilities::PkgmgrParserProcessUsrManifestXForUninstallation(
    const boost::filesystem::path& path, uid_t uid) {
  return pkgmgr_parser_process_usr_manifest_x_for_uninstallation(
      object_, path.c_str(), uid);
}

int ManifestXUtilities::PkgmgrParserProcessManifestXForUninstallation(
    const boost::filesystem::path& path) {
  return pkgmgr_parser_process_manifest_x_for_uninstallation(object_,
                                                             path.c_str());
}
