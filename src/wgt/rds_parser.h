// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef WGT_RDS_PARSER_H_
#define WGT_RDS_PARSER_H_

#include <string>
#include <vector>
namespace wgt {
namespace rds_parser {

class RDSParser {
 public:
  explicit RDSParser(const std::string& path_to_delta);

  bool Parse();

  const std::vector<std::string>& files_to_modify() const;
  const std::vector<std::string>& files_to_add() const;
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
