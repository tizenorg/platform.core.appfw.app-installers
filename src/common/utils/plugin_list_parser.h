// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_UTILS_PLUGIN_LIST_PARSER_H_
#define COMMON_UTILS_PLUGIN_LIST_PARSER_H_

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

class PluginInfo {
 public:
  PluginInfo(int flag, const std::string& type, const std::string& name,
             const std::string& path);
  int Flag();
  std::string Type();
  std::string Name();
  std::string Path();

 private:
  int flag_;
  std::string type_;
  std::string name_;
  std::string path_;
};

class PluginsListParser {
 public:
  static std::string ExtractRaw(const std::string& data,
                                const std::regex& re_extract);
  static std::string ExtractFlag(const std::string& flag);
  static std::string ExtractType(const std::string& type);
  static std::string ExtractName(const std::string& name);
  static std::string ExtractPath(const std::string& path);
  static bool ValidFlag(const std::string& flag);
  static bool ValidType(const std::string& type);
  static bool ValidName(const std::string& name);
  static bool ValidPath(const std::string& path);

  static int ConvertFlagToInt(const std::string& flag);

  explicit PluginsListParser(const std::string& path) : path_(path) {}

  static void Split(const std::string& line, const char* delim,
                    std::vector<std::string>* parts);
  bool Parse();
  const std::vector<std::shared_ptr<PluginInfo>>& Data();

 private:
  enum Column { Flag, Type, Name, Path };

  PluginsListParser() {}
  const std::string path_;
  std::vector<std::shared_ptr<PluginInfo>> data_;

  bool ReadLinesFromFile(std::vector<std::string>* lines);
  bool ParsePluginsRawData(const std::vector<std::string>& lines);
  bool SplitPluginLine(const std::string& line,
                       std::vector<std::string>* parts);
  void AddData(const std::shared_ptr<PluginInfo>& data);
};

}  // namespace common_installer
#endif  // COMMON_UTILS_PLUGIN_LIST_PARSER_H_
