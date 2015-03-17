// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#ifndef MANIFEST_HANDLERS_SERVICE_HANDLER_H_
#define MANIFEST_HANDLERS_SERVICE_HANDLER_H_

#include <string>
#include <vector>

#include "utils/macros.h"
#include "parser/manifest_handler.h"
#include "parser/values.h"

namespace wgt {
namespace parse {

class ServiceInfo {
 public:
  explicit ServiceInfo(const std::string& id, bool auto_restart = false,
      bool on_boot = false);
  virtual ~ServiceInfo();

  const std::string& id() const { return id_; }
  void set_id(const std::string& id) {
    id_ = id;
  }

  bool auto_restart() const { return auto_restart_; }
  void set_auto_restart(bool auto_restart) {
    auto_restart_ = auto_restart;
  }

  bool on_boot() const { return on_boot_; }
  void set_on_boot(bool on_boot) {
    on_boot_ = on_boot;
  }

 private:
  std::string id_;
  bool auto_restart_;
  bool on_boot_;
};

struct ServiceList  : public parser::ManifestData {
  std::vector<ServiceInfo> services;
};

class ServiceHandler : public parser::ManifestHandler {
 public:
  ServiceHandler();
  virtual ~ServiceHandler();

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
  DISALLOW_COPY_AND_ASSIGN(ServiceHandler);
};

}  // namespace parse
}  // namespace wgt

#endif  // MANIFEST_HANDLERS_SERVICE_HANDLER_H_
