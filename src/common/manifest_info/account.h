// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#ifndef COMMON_MANIFEST_INFO_ACCOUNT_H_
#define COMMON_MANIFEST_INFO_ACCOUNT_H_

#include <string>
#include <utility>
#include <vector>

namespace common_installer {

// TODO(t.iwanek): this structure should be unified for manifest handlers of
// wgt and tpk packages

/**
 * \brief Used to store information about single account
 */
struct SingleAccountInfo {
  bool multiple_account_support;
  std::vector<std::pair<std::string, std::string>> names;
  std::vector<std::pair<std::string, std::string>> icon_paths;
  std::vector<std::string> capabilities;
  std::string appid;
  std::string providerid;
};


/**
 * \brief Holds information about all accounts
 */
class AccountInfo {
 public:
  /** \brief Constructor */
  AccountInfo() {}
  /**
   * \brief Accounts list getter
   *
   * \return accounts list
   */
  const std::vector<SingleAccountInfo>& accounts() const {
    return accounts_;
  }

  /**
   * \brief Adds account to the list
   *
   * \param single_account account to be added
   */
  void set_account(const SingleAccountInfo& single_account) {
    accounts_.push_back(single_account);
  }
 private:
  std::vector<SingleAccountInfo> accounts_;
};

}  // namespace common_installer

#endif  // COMMON_MANIFEST_INFO_ACCOUNT_H_
