// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_PLUGINS_PLUGIN_LIST_PARSER_H_
#define COMMON_PLUGINS_PLUGIN_LIST_PARSER_H_

#include <boost/filesystem/path.hpp>

#include <libxml/parser.h>
#include <libxml/xmlreader.h>
#include <libxml/xmlschemas.h>

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
#include <regex>
#include <string>
#include <utility>
#include <vector>

#include "manifest_parser/utils/logging.h"

namespace common_installer {

/** this class include information about plugin */
__attribute__ ((visibility ("default"))) class PluginInfo {
 public:
  PluginInfo(int flag, const std::string& type, const std::string& name,
             const boost::filesystem::path& path);
  int flag() const;
  const std::string& type() const;
  const std::string& name() const;
  const boost::filesystem::path& path() const;

 private:
  int flag_;
  std::string type_;
  std::string name_;
  boost::filesystem::path path_;
};

/** this class parse plugin file */
__attribute__ ((visibility ("default"))) class PluginsListParser {
 public:
  using PluginList = std::vector<std::shared_ptr<PluginInfo>>;
  explicit PluginsListParser(const std::string& path) : path_(path) {}

  bool Parse();
  const PluginList& PluginInfoList() const;

 private:
  enum Column { Flag, Type, Name, Path };

  PluginsListParser() {}

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

  const std::string path_;
  std::vector<std::shared_ptr<PluginInfo>> plugin_info_list_;
};

}  // namespace common_installer

#endif  // COMMON_PLUGINS_PLUGIN_LIST_PARSER_H_
