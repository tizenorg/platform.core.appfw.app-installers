#ifndef MANIFEST_X_WRAPPER_H
#define MANIFEST_X_WRAPPER_H

#include <type_traits>
#include "common/wrapper_object_base.h"
#include "manifest_x_utilities.h"



class ManifestXWrapper : public WrapperObjectBase<manifest_x> {
 public:
  explicit ManifestXWrapper(const boost::filesystem::path &path, uid_t uid);
  ~ManifestXWrapper();

  bool Create();
  std::shared_ptr<ManifestXUtilities> Utilities();

  void SetRemovable(bool value);
  void SetPreload(bool value);
  void SetReadonly(bool value);
  void SetSystem(bool value);
  void SetUpdate(bool value);
  void SetInstalledStorage(const std::string &value);
  void SetTepPath(const boost::filesystem::path &path);

  void SetApiVersion(const std::string &value);
  std::string ApiVersion() const;
  bool IsApiVersionExist();

  enum class TypeApp :uint8_t { WGT, TPK };

  void SetType(TypeApp type);
  std::string Type() const;
  bool IsWgtType() const;

  GList *privileges;  /**< package privileges, element*/
  GList *application; /**< package's application, element*/

 private:  

  const boost::filesystem::path &path_;
  uid_t uid_;
  static std::map<TypeApp, const std::string> mapTypeApp_;  
  std::shared_ptr<ManifestXUtilities> utilities_;

  TypeApp type_h;
  std::shared_ptr<WrapperStringType> type_;

  std::shared_ptr<WrapperBoolType> removable_;
  std::shared_ptr<WrapperBoolType> preload_;
  std::shared_ptr<WrapperBoolType> readonly_;
  std::shared_ptr<WrapperBoolType> system_;
  std::shared_ptr<WrapperBoolType> update_;

  std::shared_ptr<WrapperStringType> installed_storage_;
  std::shared_ptr<WrapperStringType> tep_path_;
  std::shared_ptr<WrapperStringType> api_version_;
};

#endif  // MANIFEST_X_WRAPPER_H
