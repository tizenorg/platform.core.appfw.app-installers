// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/feature_validator.h"

#include <boost/scope_exit.hpp>

#include <system_info.h>

namespace {

const char kTrueFeature[] = "true";

}  // namespace

namespace common_installer {

FeatureValidator::FeatureValidator(
    const std::map<std::string, std::string>& features)
    : features_(features) {
}

bool FeatureValidator::Validate(std::string* error) {
  for (auto& pair : features_) {
    const auto& feature = pair.first;
    const auto& value = pair.second;

    system_info_type_e type;
    int ret = system_info_get_platform_type(feature.c_str(), &type);
    if (ret != SYSTEM_INFO_ERROR_NONE) {
      *error = std::string("Unknown feature: ") + feature;
      return false;
    }
    bool ok = false;
    switch (type) {
    case SYSTEM_INFO_BOOL:
      ok = ValidateBoolean(feature, value, error);
      break;
    case SYSTEM_INFO_INT:
      ok = ValidateInteger(feature, value, error);
      break;
    case SYSTEM_INFO_STRING:
      ok = ValidateString(feature, value, error);
      break;
    case SYSTEM_INFO_DOUBLE:
      // There is no double typed feature on platform, no way to interpret it
      ok = true;
      break;
    default:
      ok = false;
      *error = "Unknown type of feature";
    }
    if (!ok)
      return false;
  }
  return true;
}

bool FeatureValidator::ValidateBoolean(
    const std::string& feature, const std::string& value, std::string* error) {
  bool supported = false;
  int ret = system_info_get_platform_bool(feature.c_str(), &supported);
  if (ret != SYSTEM_INFO_ERROR_NONE) {
    *error = std::string("Failed to call system_info_get_platform_bool()") +
        " for " + feature + ", error code: " + std::to_string(ret);
    return false;
  }
  return true;
}

bool FeatureValidator::ValidateInteger(
    const std::string& feature, const std::string& value, std::string* error) {
  int platform_value = 0;
  int ret = system_info_get_platform_int(feature.c_str(), &platform_value);
  if (ret != SYSTEM_INFO_ERROR_NONE) {
    *error = std::string("Failed to call system_info_get_platform_int()") +
        " for " + feature + ", error code: " + std::to_string(ret);
    return false;
  }
  return true;
}

bool FeatureValidator::ValidateString(
    const std::string& feature, const std::string& value, std::string* error) {
  char* text = nullptr;
  BOOST_SCOPE_EXIT_ALL(text) {
    free(text);
  };
  int ret = system_info_get_platform_string(feature.c_str(), &text);
  if (ret != SYSTEM_INFO_ERROR_NONE) {
    *error = std::string("Failed to call system_info_get_platform_string()") +
        " for " + feature + ", error code: " + std::to_string(ret);
    return false;
  }
  return true;
}

}  // namespace common_installer
