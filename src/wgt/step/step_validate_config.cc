// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "wgt/step/step_validate_config.h"

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/system/error_code.hpp>

#include <iri.h>

#include <algorithm>
#include <string>
#include <vector>

#include "utils/logging.h"
#include "wgt/wgt_backend_data.h"

namespace ba = boost::algorithm;
namespace bf = boost::filesystem;
namespace bs = boost::system;

namespace {

const char kLocaleDirectory[] = "locales";

const char* kDefaultStartFiles[] = {
  "index.htm",
  "index.html",
  "index.svg",
  "index.xhtml",
  "index.xht"
};

enum class FindResult {
  OK,
  NUL,
  ERROR
};

bool IsValidIRI(const std::string& iri_string) {
  std::unique_ptr<iri_struct, decltype(&iri_destroy)> iri(
      iri_parse(iri_string.c_str()), iri_destroy);
  return
    iri != NULL &&
    iri->scheme != NULL && (
      iri->display != NULL ||
      iri->user != NULL ||
      iri->auth != NULL ||
      iri->password != NULL ||
      iri->host != NULL ||
      iri->path != NULL ||
      iri->query != NULL ||
      iri->anchor != NULL ||
      iri->qparams != NULL ||
      iri->schemelist != NULL);
}

// http://www.w3.org/TR/widgets/#rule-for-finding-a-file-within-a-widget-package-0  // NOLINT
FindResult FindFileWithinWidget(const bf::path& widget_path,
    const std::string& content, bf::path* file = nullptr) {
  std::string content_value = content;
  if (content_value.empty())
    return FindResult::ERROR;
  if (content_value[0] == '/') {
    content_value = content_value.substr(1);
  }
  // Steps 4. && 5.
  std::vector<std::string> path_components;
  ba::split(path_components, content_value, ba::is_any_of("/"));
  if (path_components.size() >= 1 && path_components[0] == kLocaleDirectory) {
    if (path_components.size() == 1) {
      return FindResult::NUL;
    }

    // TODO(t.iwanek): validate language tag

    content_value = std::accumulate(++++path_components.begin(),
        path_components.end(), std::string(),
        [](const std::string& first, const std::string& second) {
          return first + '/' + second;
        });
  }
  // for each locale in widget
  // TODO(t.iwanek): this algorithm should accept input of preferred locale list
  bs::error_code error;
  bf::path locale_directory = widget_path / kLocaleDirectory;
  if (bf::exists(locale_directory, error)) {
    for (auto iter = bf::directory_iterator(locale_directory);
         iter != bf::directory_iterator(); ++iter) {
      const bf::path& path = *iter;

      // TODO(t.iwanek): validate language tag

      bf::path candidate = path / content_value;
      if (bf::exists(candidate, error)) {
        if (bf::is_directory(candidate, error)) {
          return FindResult::ERROR;
        }
        if (file)
          *file = candidate;
        return FindResult::OK;
      }
    }
  }

  // default locale
  bf::path root_candidate = widget_path / content_value;
  if (bf::exists(root_candidate, error)) {
    if (bf::is_directory(root_candidate, error)) {
      return FindResult::ERROR;
    }
    if (file)
      *file = root_candidate;
    return FindResult::OK;
  }

  return FindResult::NUL;
}

// http://www.w3.org/TR/widgets/#step-8-locate-the-start-file
bool CheckStartFileInWidget(const bf::path& widget_path,
                            const std::string& content) {
  if (!content.empty()) {
    if (FindFileWithinWidget(widget_path, content) == FindResult::OK) {
      LOG(INFO) << "Start file is: " << content;
      return true;
    }
  }
  for (auto& file : kDefaultStartFiles) {
    if (FindFileWithinWidget(widget_path, file) == FindResult::OK) {
      LOG(INFO) << "Start file is: " << file;
      return true;
    }
  }
  LOG(ERROR) << "No valid start file found";
  return false;
}

}  // namespace

namespace wgt {
namespace validate_config {

common_installer::Step::Status StepValidateConfig::process() {
  WgtBackendData* data =
      static_cast<WgtBackendData*>(context_->backend_data.get());
  const std::string& content = data->content.get();

  if (IsValidIRI(content)) {
    // external url in tizen:content is outside of w3c p&c spec
    return Status::OK;
  }

  return CheckStartFileInWidget(context_->unpacked_dir_path.get(), content)
      ? Status::OK : Status::ERROR;
}

}  // namespace validate_config
}  // namespace wgt
