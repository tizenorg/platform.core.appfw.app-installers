// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include "common/utils/plugin_list_parser.h"

namespace common_installer {
static const int kConvertStringBase = 16;

PluginInfo::PluginInfo(int flag, const std::string& type,
                       const std::string& name,
                       const boost::filesystem::path& path)
    : flag_(flag),
      type_(std::move(type)),
      name_(std::move(name)),
      path_(std::move(path)) {}

int PluginInfo::Flag() { return flag_; }

std::string PluginInfo::Type() { return type_; }

std::string PluginInfo::Name() { return name_; }

boost::filesystem::path PluginInfo::Path() { return path_; }

// class PluginsListParser

int PluginsListParser::ConvertFlagToInt(const std::string& flag) {
  return std::strtoul(flag.c_str(), nullptr, kConvertStringBase);
}

boost::filesystem::path PluginsListParser::ConvertPathToBoostFormat(
    const std::string& path) {
  return boost::filesystem::path(path);
}

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

  std::regex re_extension(R"(.*\.so)");
  return std::regex_match(path, re_extension);
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

const std::vector<std::shared_ptr<PluginInfo>>&
PluginsListParser::PluginInfoList() {
  return plugin_info_list_;
}

bool PluginsListParser::ReadLinesFromFile(std::vector<std::string>* lines) {
  std::ifstream plugins_file;

  plugins_file.open(path_);

  if (!plugins_file.is_open()) {
    LOG(ERROR) << "File " << path_ << " no open";
    return false;
  }

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

    int _flag = ConvertFlagToInt(flag);

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

    boost::filesystem::path _path = ConvertPathToBoostFormat(path);

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
