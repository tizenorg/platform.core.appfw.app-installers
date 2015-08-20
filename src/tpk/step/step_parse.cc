// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include "tpk/step/step_parse.h"

#include <tpk_manifest_handlers/app_control_handler.h>
#include <tpk_manifest_handlers/application_icons_handler.h>
#include <tpk_manifest_handlers/application_manifest_constants.h>
#include <tpk_manifest_handlers/author_handler.h>
#include <tpk_manifest_handlers/datacontrol_handler.h>
#include <tpk_manifest_handlers/description_handler.h>
#include <tpk_manifest_handlers/metadata_handler.h>
#include <tpk_manifest_handlers/privileges_handler.h>
#include <tpk_manifest_handlers/service_application_handler.h>
#include <tpk_manifest_handlers/ui_application_handler.h>
#include <manifest_parser/manifest_constants.h>

#include <pkgmgr/pkgmgr_parser.h>

#include <string.h>

#include <cstdio>
#include <cstdlib>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "common/app_installer.h"
#include "common/context_installer.h"
#include "common/step/step.h"
#include "utils/clist_helpers.h"

namespace tpk {
namespace parse {

namespace {

const std::string kManifestVersion = "1.0.0";

metadata_x* GenerateMetadataListX(const tpk::parse::MetaDataInfoList& meta_info,
    metadata_x* head) {
  for (auto& meta : meta_info.items) {
    metadata_x* new_meta =
        static_cast<metadata_x*>(calloc(1, sizeof(metadata_x)));
    new_meta->key = strdup(meta.key().c_str());
    if (!meta.key().empty())
      new_meta->value = strdup(meta.val().c_str());
    LISTADD(head, new_meta);
  }
  return head;
}

}  // namespace

namespace app_keys = tpk::application_keys;
namespace bf = boost::filesystem;
namespace manifest_keys = tpk::manifest_keys;

// typedef common_installer::Step::Status Status;
// using boost::filesystem::path;

// Status StepParse::precheck() {
//   if (context_->unpacked_dir_path.get().empty()) {
//       LOG(ERROR) << "unpacked_dir_path attribute is empty";
//       return Step::Status::INVALID_VALUE;
//   }
//   if (!boost::filesystem::exists(context_->unpacked_dir_path.get())) {
//     LOG(ERROR) << "unpacked_dir_path ("
//                << context_->unpacked_dir_path.get()
//                << ") path does not exist";
//     return Step::Status::INVALID_VALUE;
//   }

//   boost::filesystem::path tmp(context_->unpacked_dir_path.get());
//   tmp /= kManifestFileName;

//   if (!boost::filesystem::exists(tmp)) {
//     LOG(ERROR) << kManifestFileName << " not found from the package";
//     return Step::Status::INVALID_VALUE;
//   }

//   return Step::Status::OK;
// }

// bf::path StepParse::LocateConfigFile() const {
//   boost::filesystem::path path(context_->unpacked_dir_path.get());
//   path /= kManifestFileName;
//   return path;
// }

// StepParse::StepParse(common_installer::ContextInstaller* context,
//                      bool check_start_file)
//     : Step(context),
//       check_start_file_(check_start_file) {
// }

// Locating tizen-manifest.xml file
bf::path StepParse::LocateConfigFile() const {
  boost::filesystem::path path(context_->unpacked_dir_path.get());
  path /= manifest_keys::kManifestFileName;
  return path;
}

// Extracting privileges
std::set<std::string> StepParse::ExtractPrivileges(
    std::shared_ptr<const PrivilegesInfo> perm_info) const {
  return perm_info->GetPrivileges();
}

// Getting manifest version
const std::string& StepParse::GetPackageVersion(
     const std::string& manifest_version) {
  if (!manifest_version.empty())
    return manifest_version;
  return kManifestVersion;
  // return manifest_keys::kVersion;
}

// Parsing icon
bool StepParse::FillIconPaths(manifest_x* manifest) {
  std::shared_ptr<const ApplicationIconsInfo> icons_info =
      std::static_pointer_cast<const ApplicationIconsInfo>(
          parser_->GetManifestData(app_keys::kIconKey));
  if (icons_info.get()) {
    for (auto& application_icon : icons_info->icons()) {
      icon_x* icon = reinterpret_cast<icon_x*> (calloc(1, sizeof(icon_x)));
      icon->name = strdup(application_icon.path().c_str());
      LISTADD(manifest->icon, icon);
    }
  }
  return true;
}

/*
  Missing application_info handler for:
   xmlns
   api_version
   package
   versio nr
*/

}  // namespace parse
}  // namespace tpk
