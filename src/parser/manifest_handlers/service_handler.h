// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#ifndef PARSER_MANIFEST_HANDLERS_SERVICE_HANDLER_H_
#define PARSER_MANIFEST_HANDLERS_SERVICE_HANDLER_H_

#include <string>
#include <vector>

#include "utils/macros.h"
#include "utils/values.h"
#include "parser/application_data.h"
#include "parser/manifest_handler.h"

namespace common_installer {
namespace parser {

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

struct ServiceList  : public ApplicationData::ManifestData {
  std::vector<ServiceInfo> services;
};

class ServiceHandler : public ManifestHandler {
 public:
  ServiceHandler();
  virtual ~ServiceHandler();

  bool Parse(std::shared_ptr<ApplicationData> application,
             std::string* error) override;
  bool Validate(std::shared_ptr<const ApplicationData> application,
                std::string* error) const override;
  std::vector<std::string> Keys() const override;

 private:
  DISALLOW_COPY_AND_ASSIGN(ServiceHandler);
};

}  // namespace parser
}  // namespace common_installer

#endif  // PARSER_MANIFEST_HANDLERS_SERVICE_HANDLER_H_
