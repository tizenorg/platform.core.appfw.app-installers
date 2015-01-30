// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.
// Part of this file is redistributed from crosswalk project under BSD-style
// license. Check LICENSE-xwalk file.

#ifndef WIDGET_MANIFEST_PARSER_APPLICATION_MANIFEST_CONSTANTS_H_
#define WIDGET_MANIFEST_PARSER_APPLICATION_MANIFEST_CONSTANTS_H_

#include "widget-manifest-parser/manifest.h"

// Keys used in JSON representation of applications.
namespace common_installer {
namespace application_manifest_keys {
// Official fields (ordered as spec):

extern const char kNameKey[];
// extern const char kShortName[];
// extern const char kIcons[];
extern const char kDisplay[];
// extern const char kOrientation[];
extern const char kStartURLKey[];
extern const char kCSPKey[];

// Deprecated fields:

extern const char kAppKey[];
extern const char kLaunchWebURLKey[];
extern const char kLaunchLocalPathKey[];
extern const char kDeprecatedURLKey[];
extern const char kDeprecatedVersionKey[];
extern const char kDeprecatedDescriptionKey[];
extern const char kDeprecatedCSPKey[];
extern const char kLaunchScreen[];
extern const char kLaunchScreenDefault[];
extern const char kLaunchScreenImageBorderDefault[];
extern const char kLaunchScreenImageBorderLandscape[];
extern const char kLaunchScreenImageBorderPortrait[];
extern const char kLaunchScreenLandscape[];
extern const char kLaunchScreenPortrait[];
extern const char kLaunchScreenReadyWhen[];
}  // namespace application_manifest_keys

namespace application_widget_keys {
extern const char kNamespaceKey[];
extern const char kXmlLangKey[];
extern const char kDefaultLocaleKey[];
extern const char kNameKey[];
extern const char kLaunchLocalPathKey[];
extern const char kWebURLsKey[];
extern const char kWidgetKey[];
extern const char kVersionKey[];
extern const char kViewModesKey[];
extern const char kAccessKey[];
extern const char kAccessOriginKey[];
extern const char kAccessSubdomainsKey[];
extern const char kCSPKey[];
extern const char kAuthorKey[];
extern const char kDescriptionKey[];
extern const char kShortNameKey[];
extern const char kIDKey[];
extern const char kAuthorEmailKey[];
extern const char kAuthorHrefKey[];
extern const char kHeightKey[];
extern const char kWidthKey[];
extern const char kPreferencesKey[];
extern const char kPreferencesNameKey[];
extern const char kPreferencesValueKey[];
extern const char kPreferencesReadonlyKey[];
extern const char kWidgetNamespaceKey[];
extern const char kWidgetNamespacePrefix[];
extern const char kTizenWidgetKey[];
extern const char kTizenApplicationKey[];
extern const char kTizenApplicationIdKey[];
extern const char kTizenApplicationPackageKey[];
extern const char kTizenApplicationRequiredVersionKey[];
extern const char kTizenAppIdKey[];
extern const char kIcon128Key[];
extern const char kAllowNavigationKey[];
extern const char kCSPReportOnlyKey[];
extern const char kTizenSettingKey[];
extern const char kTizenBackgroundSupportKey[];
extern const char kTizenContextMenuKey[];
extern const char kTizenHardwareKey[];
extern const char kTizenEncryptionKey[];
extern const char kTizenMetaDataKey[];
extern const char kTizenMetaDataNameKey[];
extern const char kTizenMetaDataValueKey[];
extern const char kTizenPermissionsKey[];
extern const char kTizenPermissionsNameKey[];
extern const char kTizenSplashScreenKey[];
extern const char kTizenSplashScreenSrcKey[];
extern const char kContentNamespace[];
extern const char kTizenScreenOrientationKey[];
extern const char kTizenAppWidgetFullKey[];
extern const char kTizenAppWidgetKey[];
extern const char kTizenAppWidgetIdKey[];
extern const char kTizenAppWidgetPrimaryKey[];
extern const char kTizenAppWidgetUpdatePeriodKey[];
extern const char kTizenAppWidgetAutoLaunchKey[];
extern const char kTizenAppWidgetBoxLabelKey[];
extern const char kTizenAppWidgetBoxLabelLangKey[];
extern const char kTizenAppWidgetBoxLabelTextKey[];
extern const char kTizenAppWidgetBoxIconKey[];
extern const char kTizenAppWidgetBoxIconSrcKey[];
extern const char kTizenAppWidgetBoxContentKey[];
extern const char kTizenAppWidgetBoxContentSrcKey[];
extern const char kTizenAppWidgetBoxContentMouseEventKey[];
extern const char kTizenAppWidgetBoxContentTouchEffectKey[];
extern const char kTizenAppWidgetBoxContentSizeKey[];
extern const char kTizenAppWidgetBoxContentSizeTextKey[];
extern const char kTizenAppWidgetBoxContentSizePreviewKey[];
extern const char kTizenAppWidgetBoxContentSizeUseDecorationKey[];
extern const char kTizenAppWidgetBoxContentDropViewKey[];
extern const char kTizenAppWidgetBoxContentDropViewSrcKey[];
extern const char kTizenAppWidgetBoxContentDropViewWidthKey[];
extern const char kTizenAppWidgetBoxContentDropViewHeightKey[];
extern const char kTizenApplicationAppControlsKey[];
extern const char kTizenApplicationAppControlSrcKey[];
extern const char kTizenApplicationAppControlOperationKey[];
extern const char kTizenApplicationAppControlUriKey[];
extern const char kTizenApplicationAppControlMimeKey[];
extern const char kTizenApplicationAppControlChildNameAttrKey[];
extern const char kTizenImeKey[];
extern const char kTizenImeUuidKey[];
extern const char kTizenImeUuidTextKey[];
extern const char kTizenImeLanguagesKey[];
extern const char kTizenImeLanguageKey[];
extern const char kTizenImeLanguageTextKey[];
extern const char kTizenCategoryKey[];
extern const char kTizenCategoryNameKey[];
extern const char kTizenNamespacePrefix[];
}  // namespace application_widget_keys


namespace application_manifest_errors {
extern const char kInvalidDescription[];
extern const char kInvalidKey[];
extern const char kInvalidName[];
extern const char kInvalidVersion[];
extern const char kManifestParseError[];
// TODO(t.iwanek): fix unicode inconsistent usage
extern const char kManifestUnreadable[];
}  // namespace application_manifest_errors

namespace widget_manifest_parser {
const char* GetNameKey(Manifest::Type type);
const char* GetCSPKey(Manifest::Type type);
const char* GetTizenAppIdKey(Manifest::Type type);
const char* GetIcon128Key(Manifest::Type type);
}  // namespace widget_manifest_parser
}  // namespace common_installer

#endif  // WIDGET_MANIFEST_PARSER_APPLICATION_MANIFEST_CONSTANTS_H_
