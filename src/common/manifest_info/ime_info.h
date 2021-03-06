// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_MANIFEST_INFO_IME_INFO_H_
#define COMMON_MANIFEST_INFO_IME_INFO_H_

#include <string>
#include <vector>

namespace common_installer {

/**
 * \brief Holds information about IME application.
 */
class ImeInfo final {
 public:
  using LanguageList = std::vector<std::string>;
  using LanguageListIterator = LanguageList::iterator;
  using LanguageListConstIterator = LanguageList::const_iterator;

  ImeInfo() = default;
  ImeInfo(const ImeInfo &) = default;
  ImeInfo(ImeInfo &&) = default;
  ~ImeInfo() = default;

  const std::string& uuid() const & {
    return uuid_;
  }

  std::string&& uuid() && noexcept {  // NOLINT
    return std::move(uuid_);
  }

  void setUuid(const std::string& uuid) {
    uuid_ = uuid;
  }

  void setUuid(std::string&& uuid) noexcept {  // NOLINT
    uuid_ = std::move(uuid);
  }

  const LanguageList& languages() const & {
    return languages_;
  }

  LanguageList&& languages() && noexcept {  // NOLINT
    return std::move(languages_);
  }

  void setLanguages(LanguageList languages) {
    languages_ = std::move(languages);
  }

  void AddLanguage(std::string language) {
    languages_.emplace_back(std::move(language));
  }

  LanguageListIterator LanguagesBegin() {
    return std::begin(languages_);
  }

  LanguageListConstIterator LanguagesBegin() const {
    return std::begin(languages_);
  }

  LanguageListIterator LanguagesEnd() {
    return std::end(languages_);
  }

  LanguageListConstIterator LanguagesEnd() const {
    return std::end(languages_);
  }

  ImeInfo& operator=(const ImeInfo&) = default;
  ImeInfo& operator=(ImeInfo&&) = default;

 private:
  std::string uuid_;
  LanguageList languages_;
};

}  // namespace common_installer

#endif  // COMMON_MANIFEST_INFO_IME_INFO_H_
