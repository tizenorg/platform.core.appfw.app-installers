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
class PluginInfo {
 public:
  __attribute__ ((visibility ("default"))) PluginInfo(int flag, const std::string& type, const std::string& name,
             const boost::filesystem::path& path);
  __attribute__ ((visibility ("default"))) int flag() const;
  __attribute__ ((visibility ("default"))) const std::string& type() const;
  __attribute__ ((visibility ("default"))) const std::string& name() const;
  __attribute__ ((visibility ("default"))) const boost::filesystem::path& path() const;

 private:
  int flag_;
  std::string type_;
  std::string name_;
  boost::filesystem::path path_;
};

/** this class parse plugin file */
class PluginsListParser {
 public:
  using PluginList = std::vector<std::shared_ptr<PluginInfo>>;
  __attribute__ ((visibility ("default"))) explicit PluginsListParser(const std::string& path) : path_(path) {}

  __attribute__ ((visibility ("default"))) bool Parse();
  __attribute__ ((visibility ("default"))) const PluginList& PluginInfoList() const;

 private:
  enum Column { Flag, Type, Name, Path };

  __attribute__ ((visibility ("default"))) PluginsListParser() {}

  __attribute__ ((visibility ("default"))) bool ReadLinesFromFile(std::vector<std::string>* lines);
  __attribute__ ((visibility ("default"))) bool ParsePluginsRawData(const std::vector<std::string>& lines);
  __attribute__ ((visibility ("default"))) bool SplitPluginLine(const std::string& line,
                       std::vector<std::string>* parts);

  __attribute__ ((visibility ("default"))) std::string ExtractRaw(const std::string& data, const std::regex& re_extract);
  __attribute__ ((visibility ("default"))) std::string ExtractFlag(const std::string& flag);
  __attribute__ ((visibility ("default"))) std::string ExtractType(const std::string& type);
  __attribute__ ((visibility ("default"))) std::string ExtractName(const std::string& name);
  __attribute__ ((visibility ("default"))) std::string ExtractPath(const std::string& path);
  __attribute__ ((visibility ("default"))) bool ValidFlag(const std::string& flag);
  __attribute__ ((visibility ("default"))) bool ValidType(const std::string& type);
  __attribute__ ((visibility ("default"))) bool ValidName(const std::string& name);
  __attribute__ ((visibility ("default"))) bool ValidPath(const std::string& path);

  const std::string path_;
  std::vector<std::shared_ptr<PluginInfo>> plugin_info_list_;
};

}  // namespace common_installer

#endif  // COMMON_PLUGINS_PLUGIN_LIST_PARSER_H_
