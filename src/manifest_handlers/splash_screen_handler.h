// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#ifndef MANIFEST_HANDLERS_SPLASH_SCREEN_HANDLER_H_
#define MANIFEST_HANDLERS_SPLASH_SCREEN_HANDLER_H_

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "parser/values.h"
#include "parser/manifest_handler.h"

namespace wgt {
namespace parse {

class SplashScreenInfo : public parser::ManifestData {
 public:
  SplashScreenInfo();
  virtual ~SplashScreenInfo();

  void set_src(const std::string &src) { src_ = src; }
  const std::string& src() const { return src_; }

 private:
  std::string src_;
};

class SplashScreenHandler : public parser::ManifestHandler {
 public:
  SplashScreenHandler();
  virtual ~SplashScreenHandler();

  bool Parse(
      const parser::Manifest& manifest,
      std::shared_ptr<parser::ManifestData>* output,
      std::string* error) override;
  bool Validate(
      const parser::ManifestData& data,
      const parser::ManifestDataMap& handlers_output,
      std::string* error) const override;
  std::string Key() const override;

 private:
  DISALLOW_COPY_AND_ASSIGN(SplashScreenHandler);
};

}  // namespace parse
}  // namespace wgt

#endif  // MANIFEST_HANDLERS_SPLASH_SCREEN_HANDLER_H_
