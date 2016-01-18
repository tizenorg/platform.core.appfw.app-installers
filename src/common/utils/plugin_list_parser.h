// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_UTILS_PLUGIN_LIST_PARSER_H_
#define COMMON_UTILS_PLUGIN_LIST_PARSER_H_

#include <boost/filesystem/path.hpp>

#include <libxml/parser.h>
#include <libxml/xmlreader.h>
#include <libxml/xmlschemas.h>

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <cstring>
#include <regex>
#include <utility>
#include <string>

#include "manifest_parser/utils/logging.h"

namespace common_installer {

/** this class include information about plugin */
class PluginInfo {
 public:
  PluginInfo(int flag, const std::string& type, const std::string& name,
             const boost::filesystem::path& path);
  int Flag();
  std::string Type();
  std::string Name();
  boost::filesystem::path Path();

 private:
  int flag_;
  std::string type_;
  std::string name_;
  boost::filesystem::path path_;
};

/** this class parse plugin file */
class PluginsListParser {
 public:
  explicit PluginsListParser(const std::string& path) : path_(path) {}

  bool Parse();
  const std::vector<std::shared_ptr<PluginInfo>>& PluginInfoList();

 private:
  enum Column { Flag, Type, Name, Path };

  PluginsListParser() {}
  const std::string path_;
  std::vector<std::shared_ptr<PluginInfo>> plugin_info_list_;

  bool ReadLinesFromFile(std::vector<std::string>* lines);
  bool ParsePluginsRawData(const std::vector<std::string>& lines);
  bool SplitPluginLine(const std::string& line,
                       std::vector<std::string>* parts);

  std::string ExtractRaw(const std::string& data, const std::regex& re_extract);
  std::string ExtractFlag(const std::string& flag);
  std::string ExtractType(const std::string& type);
  std::string ExtractName(const std::string& name);
  std::string ExtractPath(const std::string& path);
  bool ValidFlag(const std::string& flag);
  bool ValidType(const std::string& type);
  bool ValidName(const std::string& name);
  bool ValidPath(const std::string& path);

  int ConvertFlagToInt(const std::string& flag);
  boost::filesystem::path ConvertPathToBoostFormat(const std::string& path);
};

}  // namespace common_installer
#endif  // COMMON_UTILS_PLUGIN_LIST_PARSER_H_
