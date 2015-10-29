// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#ifndef SRC_COMMON_MANIFEST_INFO_ACCOUNT_H_
#define SRC_COMMON_MANIFEST_INFO_ACCOUNT_H_

#include <string>

namespace common_installer {

// TODO(t.iwanek): this structure should be unified for manifest handlers of
// wgt and tpk packages

/**
 * Used to store information about single account
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
 * Holds information about all accounts
 */
class AccountInfo {
 public:
  /** Constructor */
  AccountInfo() {}
  /**
   * accounts list getter
   *
   * \return accounts list
   */
  const std::vector<SingleAccountInfo>& accounts() const {
    return accounts_;
  }

  /**
   * Adds account to the list
   *
   * \param single_account account to be added
   */
  void set_account(const SingleAccountInfo& single_account) {
    accounts_.push_back(single_account);
  }
 private:
  std::vector<SingleAccountInfo> accounts_;
};

} // namespace common_installer

#endif /* SRC_COMMON_MANIFEST_INFO_ACCOUNT_H_ */
