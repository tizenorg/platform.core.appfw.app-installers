#include "manifest_x_wrapper.h"

std::map<ManifestXWrapper::TypeApp, const std::string>
    ManifestXWrapper::mapTypeApp_ = {

        {ManifestXWrapper::TypeApp::WGT, "wgt"},
        {ManifestXWrapper::TypeApp::TPK, "tpk"}};

ManifestXWrapper::ManifestXWrapper(const boost::filesystem::path& path,
                                   uid_t uid)
    : WrapperObjectBase(), path_(path), uid_(uid) {
  utilities_ =
      std::make_shared<ManifestXUtilities>(ManifestXUtilities(object_));

  // wait for wrapper
  privileges = object_->privileges;
  application = object_->application;
}

ManifestXWrapper::~ManifestXWrapper() {
  pkgmgr_parser_free_manifest_xml(object_);
}

bool ManifestXWrapper::Create() {
  object_ = pkgmgr_parser_usr_process_manifest_xml(path_.c_str(), uid_);
  if (object_) {
    return true;
  }
  return false;
}

std::shared_ptr<ManifestXUtilities> ManifestXWrapper::Utilities() {
  return utilities_;
}

void ManifestXWrapper::SetRemovable(bool value) {
  removable_ = std::make_shared<WrapperBoolType>(WrapperBoolType());
  removable_->Init(value, &object_->removable);
}

void ManifestXWrapper::SetPreload(bool value) {
  preload_ = std::make_shared<WrapperBoolType>(WrapperBoolType());
  preload_->Init(value, &object_->preload);
}

void ManifestXWrapper::SetReadonly(bool value) {
  readonly_ = std::make_shared<WrapperBoolType>(WrapperBoolType());
  readonly_->Init(value, &object_->readonly);
}

void ManifestXWrapper::SetSystem(bool value) {
  system_ = std::make_shared<WrapperBoolType>(WrapperBoolType());
  system_->Init(value, &object_->system);
}

void ManifestXWrapper::SetUpdate(bool value) {
  update_ = std::make_shared<WrapperBoolType>(WrapperBoolType());
  update_->Init(value, &object_->update);
}

void ManifestXWrapper::SetInstalledStorage(const std::string& value) {
  installed_storage_ =
      std::make_shared<WrapperStringType>(WrapperStringType());
  installed_storage_->Init(value, &object_->installlocation);
}

void ManifestXWrapper::SetTepPath(const boost::filesystem::path& path) {
  tep_path_ = std::make_shared<WrapperStringType>(WrapperStringType());
  tep_path_->Init(path.c_str(), &object_->tep_name);
}

void ManifestXWrapper::SetApiVersion(const std::string& value) {
  api_version_ = std::make_shared<WrapperStringType>(WrapperStringType());
  api_version_->Init(value, &object_->api_version);
}

std::string ManifestXWrapper::ApiVersion() const {
  return api_version_->ToString();
}

bool ManifestXWrapper::IsApiVersionExist() const {
  return api_version_->ToString().empty();
}

void ManifestXWrapper::SetType(TypeApp type) {
  type_ = std::make_shared<WrapperStringType>(WrapperStringType());
  type_->Init(mapTypeApp_[type], &object_->type);
  type_h = type;
}

std::string ManifestXWrapper::Type() const { return type_->ToString(); }

bool ManifestXWrapper::IsWgtType() const { return type_h == TypeApp::WGT; }
