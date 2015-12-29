#ifndef MANIFESTXUTILITIES_H
#define MANIFESTXUTILITIES_H

#include "common/wrapper_object_base.h"


class ManifestXUtilities : public WrapperObjectBase<manifest_x*> {
 public:
  explicit ManifestXUtilities(manifest_x *manifest);

  int PkgmgrParserProcessUsrManifestXForInstallation(
      const boost::filesystem::path &path, uid_t uid);
  int PkgmgrParserProcessManifestXForInstallation(
      const boost::filesystem::path &path);

  int PkgmgrParserProcessUsrManifestXForUpgrade(
      const boost::filesystem::path &path, uid_t uid);
  int PkgmgrParserProcessManifestXForUpgrade(
      const boost::filesystem::path &path);

  int PkgmgrParserProcessUsrManifestXForUninstallation(
      const boost::filesystem::path &path, uid_t uid);
  int PkgmgrParserProcessManifestXForUninstallation(
      const boost::filesystem::path &path);
};

#endif // MANIFESTXUTILITIES_H
