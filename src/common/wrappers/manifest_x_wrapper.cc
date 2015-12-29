#include "manifest_x_wrapper.h"

std::map<ManifestXWrapper::TypeApp, const std::string>
    ManifestXWrapper::mapTypeApp_ = {

        {ManifestXWrapper::TypeApp::WGT, "wgt"},
        {ManifestXWrapper::TypeApp::TPK, "tpk"}};

ManifestXWrapper::ManifestXWrapper(const boost::filesystem::path& path,
                                   uid_t uid)
    : WrapperObjectBase(), path_(path), uid_(uid) {
  utilities_ =
      std::shared_ptr<ManifestXUtilities>(new ManifestXUtilities(mainStruct_));

  // wait for wrapper
  privileges = mainStruct_->privileges;
  application = mainStruct_->application;
}

ManifestXWrapper::~ManifestXWrapper() {
  pkgmgr_parser_free_manifest_xml(mainStruct_);
}

bool ManifestXWrapper::Create() {
  mainStruct_ = pkgmgr_parser_usr_process_manifest_xml(path_.c_str(), uid_);
  if (mainStruct_) {
    return true;
  }
  return false;
}

std::shared_ptr<ManifestXUtilities> ManifestXWrapper::Utilities() {
  return utilities_;
}

void ManifestXWrapper::SetRemovable(bool value) {
  removable_ =
      std::shared_ptr<WrapperBoolType>(new WrapperBoolType(value, &mainStruct_->removable));
}

void ManifestXWrapper::SetPreload(bool value) {
  preload_ =
      std::shared_ptr<WrapperBoolType>(new WrapperBoolType(value, &mainStruct_->preload));
}

void ManifestXWrapper::SetReadonly(bool value) {
  readonly_ =
      std::shared_ptr<WrapperBoolType>(new WrapperBoolType(value, &mainStruct_->readonly));
}

void ManifestXWrapper::SetSystem(bool value) {
  system_ = std::shared_ptr<WrapperBoolType>(new WrapperBoolType(value, &mainStruct_->system));
}

void ManifestXWrapper::SetUpdate(bool value) {
  update_ = std::shared_ptr<WrapperBoolType>(new WrapperBoolType(value, &mainStruct_->update));
}

void ManifestXWrapper::SetInstalledStorage(const std::string& value) {
  installed_storage_ = std::shared_ptr<WrapperStringType>(
      new WrapperStringType(value, &mainStruct_->installlocation));
}

void ManifestXWrapper::SetTepPath(const boost::filesystem::path& path) {
  tep_path_ = std::shared_ptr<WrapperStringType>(
      new WrapperStringType(path.c_str(), &mainStruct_->tep_name));
}

void ManifestXWrapper::SetApiVersion(const std::string& value) {
  api_version_ = std::shared_ptr<WrapperStringType>(
      new WrapperStringType(value, &mainStruct_->api_version));
}

std::string ManifestXWrapper::ApiVersion() const {
  return api_version_->ToString();
}

bool ManifestXWrapper::IsApiVersionExist() {
  return api_version_->ToString().empty();
}

void ManifestXWrapper::SetType(TypeApp type) {
  type_ = std::shared_ptr<WrapperStringType>(
      new WrapperStringType(mapTypeApp_[type], &mainStruct_->type));
  type_h = type;
}

std::string ManifestXWrapper::Type() const { return type_->ToString(); }

bool ManifestXWrapper::IsWgtType() const { return type_h == TypeApp::WGT; }
