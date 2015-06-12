// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "wgt/rds_parser.h"

#include <fstream>

namespace {
const char kAdd[] = "#add";
const char kModify[] = "#modify";
const char kDelete[] = "#delete";
}

namespace wgt {
namespace rds_parser {

RDSParser::RDSParser(const std::string& path_to_delta)
  : path_to_delta_(path_to_delta) {}

bool RDSParser::Parse() {
  std::vector<std::string>* current_container = nullptr;
  std::string line;

  std::ifstream file_to_parse(path_to_delta_);
  if (!file_to_parse.is_open())
    return false;
  while (getline(file_to_parse, line)) {
    if (line.compare(kDelete) == 0) {
      current_container = &files_to_delete_;
      continue;
    }
    if (line.compare(kAdd) == 0) {
      current_container = &files_to_add_;
      continue;
    }
    if (line.compare(kModify) == 0) {
      current_container = &files_to_modify_;
      continue;
    }
    if (current_container)
      current_container->push_back(line);
  }
  file_to_parse.close();
  return true;
}

const std::vector<std::string>& RDSParser::files_to_modify() const {
  return files_to_modify_;
}

const std::vector<std::string>& RDSParser::files_to_add() const {
  return files_to_add_;
}

const std::vector<std::string>& RDSParser::files_to_delete() const {
  return files_to_delete_;
}

}  // namespace rds_parser
}  // namespace wgt
