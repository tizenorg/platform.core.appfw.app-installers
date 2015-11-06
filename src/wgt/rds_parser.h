// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef WGT_RDS_PARSER_H_
#define WGT_RDS_PARSER_H_

#include <string>
#include <vector>
namespace wgt {
namespace rds_parser {

/**
 * \brief Parse RDS config file
 */
class RDSParser {
 public:
  /**
   * \brief Explicit constructor
   *
   * \param path_to_delta path to directory
   */
  explicit RDSParser(const std::string& path_to_delta);

  /**
   * \brief Parse package xml
   *
   * \return true if parsing was successful
   */
  bool Parse();

  /**
   * \brief Accessor to vector of files to modify
   *
   * \return files to modify
   */
  const std::vector<std::string>& files_to_modify() const;

  /**
   * \brief Accessor to vector of files to add
   *
   * \return files to add
   */
  const std::vector<std::string>& files_to_add() const;

  /**
   * \brief Accessor to vector of files to delete
   *
   * \return files to delete
   */
  const std::vector<std::string>& files_to_delete() const;

 private:
  std::string path_to_delta_;
  std::vector<std::string> files_to_modify_;
  std::vector<std::string> files_to_add_;
  std::vector<std::string> files_to_delete_;
};

}  // namespace rds_parser
}  // namespace wgt

#endif  // WGT_RDS_PARSER_H_
