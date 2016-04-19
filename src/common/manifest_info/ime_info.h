// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved

#ifndef COMMON_MANIFEST_INFO_IME_INFO_H_
#define COMMON_MANIFEST_INFO_IME_INFO_H_

#include <string>
#include <vector>

namespace common_installer {

/**
 * \brief Holds information about IME application.
 */
class ImeInfo {
 public:
  using LanguageList = std::vector<std::string>;
  using LanguageListIterator = LanguageList::iterator;
  using LanguageListConstIterator = LanguageList::const_iterator;

  ImeInfo() = default;
  ImeInfo(const ImeInfo &) = default;
  ImeInfo(ImeInfo &&) = default;
  ~ImeInfo() = default;

  std::string getUuid() const & {
    return uuid_;
  }

  std::string &&getUuid() && noexcept {
    return std::move(uuid_);
  }

  void setUuid(const std::string &uuid) {
    uuid_ = uuid;
  }

  void setUuid(std::string &&uuid) noexcept {
    uuid_ = std::move(uuid);
  }

  LanguageList getLanguages() const & {
    return languages_;
  }

  LanguageList &&getLanguages() && noexcept {
    return std::move(languages_);
  }

  void setLanguages(LanguageList languages) {
    languages_ = std::move(languages);
  }

  void addLanguage(std::string language) {
    languages_.emplace_back(std::move(language));
  }

  LanguageListIterator languagesBegin() {
    return std::begin(languages_);
  }

  LanguageListConstIterator languagesBegin() const {
    return std::begin(languages_);
  }

  LanguageListIterator languagesEnd() {
    return std::end(languages_);
  }

  LanguageListConstIterator languagesEnd() const {
    return std::end(languages_);
  }

  ImeInfo &operator =(const ImeInfo &) = default;
  ImeInfo &operator =(ImeInfo &&) = default;

 private:
  std::string uuid_;
  LanguageList languages_;
};
}

#endif
