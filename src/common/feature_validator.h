// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_FEATURE_VALIDATOR_H_
#define COMMON_FEATURE_VALIDATOR_H_

#include <map>
#include <string>

namespace common_installer {

class FeatureValidator {
 public:
  explicit FeatureValidator(const std::map<std::string, std::string>& features);
  bool Validate(std::string* error);

 private:
  bool ValidateBoolean(
      const std::string& feature, const std::string& value, std::string* error);
  bool ValidateInteger(
      const std::string& feature, const std::string& value, std::string* error);
  bool ValidateString(
      const std::string& feature, const std::string& value, std::string* error);

  const std::map<std::string, std::string>& features_;
};

}  // namespace common_installer

#endif  // COMMON_FEATURE_VALIDATOR_H_
