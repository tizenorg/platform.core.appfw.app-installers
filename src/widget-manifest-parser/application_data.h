// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WIDGET_MANIFEST_PARSER_APPLICATION_DATA_H_
#define WIDGET_MANIFEST_PARSER_APPLICATION_DATA_H_

#include <boost/filesystem/path.hpp>
#include <string.h>

#include <algorithm>
#include <iosfwd>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "utils/values.h"
#include "widget-manifest-parser/manifest.h"
#include "widget-manifest-parser/permission_types.h"
// TODO(jizydorczyk) Conver package from xwalk for our needs
// #include "widget-manifest-parser/package/package.h"

namespace bf = boost::filesystem;

namespace common_installer {
namespace widget_manifest_parser {

// TODO(t.iwanek): there should be private inheritence
// FIX ME!
class ApplicationData : public std::enable_shared_from_this<ApplicationData> {
 public:
  // Where an application was loaded from.
  enum SourceType {
    INTERNAL,         // From internal application registry.
    LOCAL_DIRECTORY,  // From a persistently stored unpacked application
    TEMP_DIRECTORY,   // From a temporary folder
    EXTERNAL_URL      // From an arbitrary URL
  };

  struct ManifestData;

  struct ApplicationIdCompare {
    bool operator()(const std::string& s1, const std::string& s2) const {
      return strcasecmp(s1.c_str(), s2.c_str()) < 0;
    }
  };

  typedef std::map<const std::string, std::shared_ptr<ManifestData> >
      ManifestDataMap;
  typedef std::map<std::string,
      std::shared_ptr<ApplicationData>, ApplicationIdCompare>
        ApplicationDataMap;
  typedef ApplicationDataMap::iterator ApplicationDataMapIterator;

  // A base class for parsed manifest data that APIs want to store on
  // the application. Related to base::SupportsUserData, but with an immutable
  // thread-safe interface to match Application.
  struct ManifestData {
    virtual ~ManifestData() {}
  };

  static std::shared_ptr<ApplicationData> Create(const bf::path& app_path,
      const std::string& explicit_id, SourceType source_type,
          std::unique_ptr<Manifest> manifest, std::string* error_message);

  // Returns the base application url for a given |application_id|.
  static std::string GetBaseURLFromApplicationId(
      const std::string& application_id);

  // Get the manifest data associated with the key, or NULL if there is none.
  // Can only be called after InitValue is finished.
  ManifestData* GetManifestData(const std::string& key) const;

  // Sets |data| to be associated with the key. Takes ownership of |data|.
  // Can only be called before InitValue is finished. Not thread-safe;
  // all SetManifestData calls should be on only one thread.
  void SetManifestData(const std::string& key,
                       std::shared_ptr<ManifestData> data);

  // Accessors:
  const bf::path& path() const { return path_; }
  const std::string& URL() const { return application_url_; }
  SourceType source_type() const { return source_type_; }
  Manifest::Type manifest_type() const { return manifest_->type(); }
  const std::string& ID() const { return application_id_; }
  std::string GetPackageID() const;
  const std::string Version() const { return version_; }
  const std::string VersionString() const;
  const std::string& Name() const { return name_; }
  const std::string& NonLocalizedName() const { return non_localized_name_; }
  const std::string& Description() const { return description_; }

  const Manifest* GetManifest() const {
    return manifest_.get();
  }

  // App-related.
  bool IsHostedApp() const;

  bool SetApplicationLocale(const std::string& locale, std::u16string* error);

  virtual ~ApplicationData();

 private:
  ApplicationData(const bf::path& path,
      SourceType source_type, std::unique_ptr<Manifest> manifest);

  // Initialize the application from a parsed manifest.
  bool Init(const std::string& explicit_id, std::u16string* error);

  // Chooses the application ID for an application based on a variety of
  // criteria. The chosen ID will be set in |manifest|.
  bool LoadID(const std::string& explicit_id, std::u16string* error);
  // The following are helpers for InitFromValue to load various features of the
  // application from the manifest.
  bool LoadName(std::u16string* error);
  bool LoadVersion(std::u16string* error);

  // The application's human-readable name. Name is used for display purpose. It
  // might be wrapped with unicode bidi control characters so that it is
  // displayed correctly in RTL context.
  // NOTE: Name is UTF-8 and may contain non-ascii characters.
  std::string name_;

  // A non-localized version of the application's name. This is useful for
  // debug output.
  std::string non_localized_name_;

  // The version of this application's manifest. We increase the manifest
  // version when making breaking changes to the application system.
  // Version 1 was the first manifest version (implied by a lack of a
  // manifest_version attribute in the application's manifest). We initialize
  // this member variable to 0 to distinguish the "uninitialized" case from
  // the case when we know the manifest version actually is 1.
  int manifest_version_;

  // The absolute path to the directory the application is stored in.
  bf::path path_;

  // A persistent, globally unique ID. An application's ID is used in things
  // like directory structures and URLs, and is expected to not change across
  // versions.
  std::string application_id_;

  // The base application url for the application.
  std::string application_url_;

  // The application's version.
  std::string version_;

  // An optional longer description of the application.
  std::string description_;

  // The manifest from which this application was created.
  std::unique_ptr<Manifest> manifest_;

  // Stored parsed manifest data.
  ManifestDataMap manifest_data_;

  // Set to true at the end of InitValue when initialization is finished.
  bool finished_parsing_manifest_;

  // The source the application was loaded from.
  SourceType source_type_;

//  DISALLOW_COPY_AND_ASSIGN(ApplicationData);
};

}  // namespace widget_manifest_parser
}  // namespace common_installer

#endif  // WIDGET_MANIFEST_PARSER_APPLICATION_DATA_H_
