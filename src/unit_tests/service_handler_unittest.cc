// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache-2.0 license that can be
// found in the LICENSE file.

#include <boost/filesystem/path.hpp>
#include <gtest/gtest.h>

#include <vector>

#include "parser/application_manifest_constants.h"
#include "parser/manifest.h"
#include "parser/manifest_handler.h"
#include "parser/manifest_handlers/service_handler.h"
#include "unit_tests/common/scoped_testing_manifest_handler_registry.h"

namespace bf = boost::filesystem;

namespace {

const char kServiceKey[] = "service";

}  // namespace

namespace common_installer {

namespace parser {

std::shared_ptr<ApplicationData> CreateApplicationDataForTest(
    std::unique_ptr<Manifest> manifest) {
  return std::shared_ptr<ApplicationData>(
      new ApplicationData(bf::path(), ApplicationData::INTERNAL,
          std::move(manifest)));
}

namespace keys = application_widget_keys;

class ServiceHandlerTest : public testing::Test {
 public:
  void SetUp() override {
    std::vector<ManifestHandler*> handlers;
    handlers.push_back(new ServiceHandler());
    test_registry_.reset(new ScopedTestingManifestHandlerRegistry(handlers));
  }
 protected:
  std::unique_ptr<ScopedTestingManifestHandlerRegistry> test_registry_;
};

TEST_F(ServiceHandlerTest, NoServiceEntry) {
  // Set test values
  std::unique_ptr<utils::DictionaryValue> value(new utils::DictionaryValue());
  std::unique_ptr<Manifest> manifest(new Manifest(std::move(value)));
  // Create application data
  std::string error;
  std::shared_ptr<ApplicationData> app_data =
      CreateApplicationDataForTest(std::move(manifest));
  ManifestHandlerRegistry* registry = ManifestHandlerRegistry::GetInstance();
  // Check correctness
  ASSERT_TRUE(!!app_data);
  ASSERT_TRUE(registry);
  ASSERT_TRUE(registry->ParseAppManifest(app_data, &error));
  ASSERT_TRUE(registry->ValidateAppManifest(app_data, &error));
}

TEST_F(ServiceHandlerTest, SingleServiceEntryDefault) {
  // Set test values
  std::unique_ptr<utils::DictionaryValue> value(new utils::DictionaryValue());
  std::unique_ptr<utils::DictionaryValue> widget(new utils::DictionaryValue());
  std::unique_ptr<utils::DictionaryValue> service(new utils::DictionaryValue());
  service->SetString(keys::kTizenServiceIdKey, "correct001.appId");
  widget->Set(kServiceKey, service.release());
  value->Set(keys::kWidgetKey, widget.release());
  std::unique_ptr<Manifest> manifest(new Manifest(std::move(value)));
  // Create application data
  std::string error;
  std::shared_ptr<ApplicationData> app_data =
      CreateApplicationDataForTest(std::move(manifest));
  ManifestHandlerRegistry* registry = ManifestHandlerRegistry::GetInstance();
  // Check correctness
  ASSERT_TRUE(!!app_data);
  ASSERT_TRUE(registry);
  ASSERT_TRUE(registry->ParseAppManifest(app_data, &error));
  ASSERT_TRUE(registry->ValidateAppManifest(app_data, &error));
  ASSERT_TRUE(app_data->GetManifestData(keys::kTizenServiceKey));
  ServiceList* service_list =
      dynamic_cast<ServiceList*>(
          app_data->GetManifestData(keys::kTizenServiceKey));
  ASSERT_TRUE(service_list);
  ASSERT_EQ(service_list->services.size(), 1);
  ASSERT_EQ(service_list->services[0].id(), "correct001.appId");
  ASSERT_EQ(service_list->services[0].auto_restart(), false);
  ASSERT_EQ(service_list->services[0].on_boot(), false);
}

TEST_F(ServiceHandlerTest, SingleServiceEntryOnBootOn) {
  // Set test values
  std::unique_ptr<utils::DictionaryValue> value(new utils::DictionaryValue());
  std::unique_ptr<utils::DictionaryValue> widget(new utils::DictionaryValue());
  std::unique_ptr<utils::DictionaryValue> service(new utils::DictionaryValue());
  service->SetString(keys::kTizenServiceIdKey, "correct002.appId");
  service->SetString(keys::kTizenServiceOnBootKey, "true");
  widget->Set(kServiceKey, service.release());
  value->Set(keys::kWidgetKey, widget.release());
  std::unique_ptr<Manifest> manifest(new Manifest(std::move(value)));
  // Create application data
  std::string error;
  std::shared_ptr<ApplicationData> app_data =
      CreateApplicationDataForTest(std::move(manifest));
  ManifestHandlerRegistry* registry = ManifestHandlerRegistry::GetInstance();
  // Check correctness
  ASSERT_TRUE(!!app_data);
  ASSERT_TRUE(registry);
  ASSERT_TRUE(registry->ParseAppManifest(app_data, &error));
  ASSERT_TRUE(registry->ValidateAppManifest(app_data, &error));
  ASSERT_TRUE(app_data->GetManifestData(keys::kTizenServiceKey));
  ServiceList* service_list =
      dynamic_cast<ServiceList*>(
          app_data->GetManifestData(keys::kTizenServiceKey));
  ASSERT_TRUE(service_list);
  ASSERT_EQ(service_list->services.size(), 1);
  ASSERT_EQ(service_list->services[0].id(), "correct002.appId");
  ASSERT_EQ(service_list->services[0].auto_restart(), false);
  ASSERT_EQ(service_list->services[0].on_boot(), true);
}

TEST_F(ServiceHandlerTest, SingleServiceEntryAutoRestartOn) {
  // Set test values
  std::unique_ptr<utils::DictionaryValue> value(new utils::DictionaryValue());
  std::unique_ptr<utils::DictionaryValue> widget(new utils::DictionaryValue());
  std::unique_ptr<utils::DictionaryValue> service(new utils::DictionaryValue());
  service->SetString(keys::kTizenServiceIdKey, "correct003.appId");
  service->SetString(keys::kTizenServiceOnBootKey, "false");
  service->SetString(keys::kTizenServiceAutoRestartKey, "true");
  widget->Set(kServiceKey, service.release());
  value->Set(keys::kWidgetKey, widget.release());
  std::unique_ptr<Manifest> manifest(new Manifest(std::move(value)));
  // Create application data
  std::string error;
  std::shared_ptr<ApplicationData> app_data =
      CreateApplicationDataForTest(std::move(manifest));
  ManifestHandlerRegistry* registry = ManifestHandlerRegistry::GetInstance();
  // Check correctness
  ASSERT_TRUE(!!app_data);
  ASSERT_TRUE(registry);
  ASSERT_TRUE(registry->ParseAppManifest(app_data, &error));
  ASSERT_TRUE(registry->ValidateAppManifest(app_data, &error));
  ASSERT_TRUE(app_data->GetManifestData(keys::kTizenServiceKey));
  ServiceList* service_list =
      dynamic_cast<ServiceList*>(
          app_data->GetManifestData(keys::kTizenServiceKey));
  ASSERT_TRUE(service_list);
  ASSERT_EQ(service_list->services.size(), 1);
  ASSERT_EQ(service_list->services[0].id(), "correct003.appId");
  ASSERT_EQ(service_list->services[0].auto_restart(), true);
  ASSERT_EQ(service_list->services[0].on_boot(), false);
}

TEST_F(ServiceHandlerTest, SingleServiceEntryWrongId) {
  // Set test values
  std::unique_ptr<utils::DictionaryValue> value(new utils::DictionaryValue());
  std::unique_ptr<utils::DictionaryValue> widget(new utils::DictionaryValue());
  std::unique_ptr<utils::DictionaryValue> service(new utils::DictionaryValue());
  service->SetString(keys::kTizenServiceIdKey, "wrongid.appId");
  widget->Set(kServiceKey, service.release());
  value->Set(keys::kWidgetKey, widget.release());
  std::unique_ptr<Manifest> manifest(new Manifest(std::move(value)));
  // Create application data
  std::string error;
  std::shared_ptr<ApplicationData> app_data =
      CreateApplicationDataForTest(std::move(manifest));
  ManifestHandlerRegistry* registry = ManifestHandlerRegistry::GetInstance();
  // Check correctness
  ASSERT_TRUE(!!app_data);
  ASSERT_TRUE(registry);
  ASSERT_TRUE(registry->ParseAppManifest(app_data, &error));
  ASSERT_FALSE(registry->ValidateAppManifest(app_data, &error));
}

TEST_F(ServiceHandlerTest, SingleServiceEntryIdTypeMismatch) {
  // Set test values
  std::unique_ptr<utils::DictionaryValue> value(new utils::DictionaryValue());
  std::unique_ptr<utils::DictionaryValue> widget(new utils::DictionaryValue());
  std::unique_ptr<utils::DictionaryValue> service(new utils::DictionaryValue());
  service->SetInteger(keys::kTizenServiceIdKey, 1410);
  widget->Set(kServiceKey, service.release());
  value->Set(keys::kWidgetKey, widget.release());
  std::unique_ptr<Manifest> manifest(new Manifest(std::move(value)));
  // Create application data
  std::string error;
  std::shared_ptr<ApplicationData> app_data =
      CreateApplicationDataForTest(std::move(manifest));
  ManifestHandlerRegistry* registry = ManifestHandlerRegistry::GetInstance();
  // Check correctness
  ASSERT_TRUE(!!app_data);
  ASSERT_TRUE(registry);
  ASSERT_FALSE(registry->ParseAppManifest(app_data, &error));
}

TEST_F(ServiceHandlerTest, MultipleServiceEntry) {
  // Set test values
  std::unique_ptr<utils::DictionaryValue> value(new utils::DictionaryValue());
  std::unique_ptr<utils::DictionaryValue> widget(new utils::DictionaryValue());
  std::unique_ptr<utils::ListValue> list(new utils::ListValue());
  std::unique_ptr<utils::DictionaryValue> service1(
      new utils::DictionaryValue());
  std::unique_ptr<utils::DictionaryValue> service2(
      new utils::DictionaryValue());
  service1->SetString(keys::kTizenServiceIdKey, "correct004.appId");
  service2->SetString(keys::kTizenServiceIdKey, "correct005.appId");
  list->Append(service1.release());
  list->Append(service2.release());
  widget->Set(kServiceKey, list.release());
  value->Set(keys::kWidgetKey, widget.release());
  std::unique_ptr<Manifest> manifest(new Manifest(std::move(value)));
  // Create application data
  std::string error;
  std::shared_ptr<ApplicationData> app_data =
      CreateApplicationDataForTest(std::move(manifest));
  ManifestHandlerRegistry* registry = ManifestHandlerRegistry::GetInstance();
  // Check correctness
  ASSERT_TRUE(!!app_data);
  ASSERT_TRUE(registry);
  ASSERT_TRUE(registry->ParseAppManifest(app_data, &error));
  ASSERT_TRUE(registry->ValidateAppManifest(app_data, &error));
  ASSERT_TRUE(app_data->GetManifestData(keys::kTizenServiceKey));
  ServiceList* service_list =
      dynamic_cast<ServiceList*>(
          app_data->GetManifestData(keys::kTizenServiceKey));
  ASSERT_TRUE(service_list);
  ASSERT_EQ(service_list->services.size(), 2);
  ASSERT_EQ(service_list->services[0].id(), "correct004.appId");
  ASSERT_EQ(service_list->services[1].id(), "correct005.appId");
}

}  // namespace parser
}  // namespace common_installer

