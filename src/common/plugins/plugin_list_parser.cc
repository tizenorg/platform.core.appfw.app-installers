// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/plugins/plugin_list_parser.h"

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

namespace common_installer {

PluginInfo::PluginInfo(int flag, const std::string& type,
                       const std::string& name,
                       const boost::filesystem::path& path)
    : flag_(flag),
      type_(std::move(type)),
      name_(std::move(name)),
      path_(std::move(path)) {}

int PluginInfo::flag() const { return flag_; }

const std::string& PluginInfo::type() const { return type_; }

const std::string& PluginInfo::name() const { return name_; }

const boost::filesystem::path& PluginInfo::path() const { return path_; }

// class PluginsListParser
bool PluginsListParser::ValidType(const std::string& type) {
  if (type.empty()) {
    LOG(ERROR) << "Type is empty (valid function)";
    return false;
  }
  std::regex re_valid(R"((tag|metadata|category))");
  return std::regex_search(type, re_valid);
}

bool PluginsListParser::ValidFlag(const std::string& flag) {
  if (flag.empty()) {
    LOG(ERROR) << "Flag is empty (valid function)";
    return false;
  }

  std::regex re_valid(R"(0x[01248]+)");
  return std::regex_match(flag, re_valid);
}

bool PluginsListParser::ValidName(const std::string& name) {
  if (name.empty()) {
    LOG(ERROR) << "Name is empty (valid function)";
    return false;
  }

  return true;
}

bool PluginsListParser::ValidPath(const std::string& path) {
  if (path.empty()) {
    LOG(ERROR) << "Path is empty (valid function)";
    return false;
  }

  std::smatch match;
  std::regex re_extension(R"((\.so[^/]+)?$)");

  if (!std::regex_search(path, match, re_extension)) {
    return false;
  }

  // if no matched group
  if (match.size() != 2) {
    LOG(ERROR) << "Path not included extension lib file";
    return false;
  }

  return true;
}

std::string PluginsListParser::ExtractRaw(const std::string& data,
                                          const std::regex& re_extract) {
  std::smatch match;
  std::regex_search(data, match, re_extract);

  // 2 mean matched group
  if (match.size() != 2) {
    LOG(ERROR) << "Could not find data during extracting parameter";
    return {};
  }
  return match[1];
}

std::string PluginsListParser::ExtractFlag(const std::string& flag) {
  std::regex re_extract(R"(flag\s*\=\s*\"(.*)\")");
  return ExtractRaw(flag, re_extract);
}

std::string PluginsListParser::ExtractName(const std::string& type) {
  std::regex re_extract(R"(name\s*\=\s*\"(.*)\")");
  return ExtractRaw(type, re_extract);
}

std::string PluginsListParser::ExtractType(const std::string& type) {
  std::regex re_extract(R"(type\s*\=\s*\"(.*)\")");
  return ExtractRaw(type, re_extract);
}

std::string PluginsListParser::ExtractPath(const std::string& path) {
  std::regex re_extract(R"(path\s*\=\s*\"(.*)\")");
  return ExtractRaw(path, re_extract);
}

bool PluginsListParser::Parse() {
  std::vector<std::string> lines;

  if (!ReadLinesFromFile(&lines)) {
    LOG(ERROR) << "No read lines from file";
    return false;
  }

  if (!ParsePluginsRawData(lines)) {
    LOG(ERROR) << "No parse data from lines";
    return false;
  }

  return true;
}

const PluginsListParser::PluginList& PluginsListParser::PluginInfoList() const {
  return plugin_info_list_;
}

bool PluginsListParser::ReadLinesFromFile(std::vector<std::string>* lines) {
  std::ifstream plugins_file;

  plugins_file.open(path_);

  if (!plugins_file.is_open()) {
    LOG(ERROR) << "File " << path_ << " no open";
    return false;
  }

  LOG(INFO) << "Plugin list path: " << path_;

  std::string line;
  while (plugins_file >> line) {
    lines->push_back(line);
  }

  plugins_file.close();

  if (lines->empty()) {
    LOG(ERROR) << "No data in file " << path_;
    return false;
  }

  return true;
}

bool PluginsListParser::ParsePluginsRawData(
    const std::vector<std::string>& lines) {
  plugin_info_list_.clear();

  std::vector<int> flag_container;

  for (const std::string& line : lines) {
    std::vector<std::string> parts;
    if (!SplitPluginLine(line, &parts)) {
      LOG(ERROR) << "Invalid split plugin line";
      return false;
    }

    std::string flag = ExtractFlag(parts.at(Flag));
    std::string type = ExtractType(parts.at(Type));
    std::string name = ExtractName(parts.at(Name));
    std::string path = ExtractPath(parts.at(Path));

    if (!ValidFlag(flag)) {
      LOG(ERROR) << "Invalid flag: " << flag;
      return false;
    }

    const int kConvertStringBase = 16;
    int _flag = std::strtoul(flag.c_str(), nullptr, kConvertStringBase);

    // flag should be unique
    if (std::find(flag_container.begin(), flag_container.end(), _flag) !=
        flag_container.end()) {
      LOG(ERROR) << "Flag isn't unique, flag:  " << _flag;
      return false;
    } else {
      flag_container.push_back(_flag);
    }

    if (!ValidType(type)) {
      LOG(ERROR) << "Invalid type: " << type;
      return false;
    }

    if (!ValidName(name)) {
      LOG(ERROR) << "Invalid name: " << name;
      return false;
    }

    if (!ValidPath(path)) {
      LOG(ERROR) << "Invalid path: " << path;
      return false;
    }

    boost::filesystem::path _path = boost::filesystem::path(path);

    plugin_info_list_.push_back(
        std::make_shared<PluginInfo>(_flag, type, name, _path));
  }

  return true;
}

bool PluginsListParser::SplitPluginLine(const std::string& line,
                                        std::vector<std::string>* parts) {
  static const char kPartsInLine = 4;

  std::vector<std::string> _parts;
  boost::algorithm::split(_parts, line, boost::algorithm::is_any_of(";"));

  if (_parts.size() != kPartsInLine) {
    LOG(ERROR) << "Invalid number of parts";
    return false;
  }

  parts->assign(_parts.begin(), _parts.end());

  return true;
}
}  // namespace common_installer
