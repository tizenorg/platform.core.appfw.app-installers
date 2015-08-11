/* Copyright 2015 Samsung Electronics, license APACHE-2.0, see LICENSE file */

#include "tpk/step/step_copy_manifest_xml.h"

#include <sys/types.h>
#include <pkgmgr-info.h>
#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>
#include <boost/tr1/regex.hpp>

#include <fstream>
#include <iostream>
#include <memory>
#include <regex>
#include <string>

#include "common/utils/file_util.h"
#include "common/utils/logging.h"

namespace bf = boost::filesystem;
using common_installer::Step;

namespace {
  const char* source_xml_filename = "tizen-manifest.xml";

  bf::path _getOldDestXmlPath(const bf::path& dest_xml_path) {
    bf::path old_dest_xml_path = dest_xml_path;
    old_dest_xml_path += ".old";
    return old_dest_xml_path;
  }

  bool _setValidSourceXmlPath(bf::path* source_xml_path_,
      const common_installer::ContextInstaller* context_) {
    bf::path& source_xml_path = *source_xml_path_;
    source_xml_path = context_->pkg_path.get() / source_xml_filename;
    if (!bf::exists(source_xml_path)) {
      // Check single app path
      source_xml_path = context_->pkg_path.get() /
          context_->manifest_data.get()->mainapp_id / source_xml_filename;
      if (!bf::exists(source_xml_path)) {
        LOG(ERROR) << "Source xml file not found: " << source_xml_path;
        return false;
      }
    }
    return true;
  }

  void _setDestXmlPath(std::shared_ptr<bf::path>* dest_xml_path_, uid_t uid,
      const std::string& pkgid) {
    *dest_xml_path_ = std::make_shared<bf::path>(getUserManifestPath(uid));
    **dest_xml_path_ /= bf::path(pkgid);
    **dest_xml_path_ += ".xml";
  }


  bool _trimCopiedXmlContent(const bf::path& xml_path,
      const common_installer::ContextInstaller* context_) {
    bf::path pkg_path = context_->pkg_path.get();

    std::ifstream infile(xml_path.string());
    std::ofstream outfile(xml_path.string() + ".out");
    std::string line;
    std::string result;

    boost::regex appid_pattern(".*<(ui|service)-application\\s+(\\S+\\s+)*"
        "appid=\"(.+?)\".*>", boost::regex_constants::ECMAScript);
    boost::regex appid_exec_replace_pattern("exec=\"(.+?)\"",
        boost::regex_constants::ECMAScript);
    boost::regex manifest_pattern(".*<manifest\\s+.*(type=\"\"){0,1}.*>",
        boost::regex_constants::ECMAScript);
    boost::regex manifest_type_replace_pattern("<manifest ",
        boost::regex_constants::ECMAScript);

    boost::smatch m;

    while (!infile.eof()) {
      getline(infile, line);
      result = line;  // Copy line by default

      try {
        // Find (ui|service)-application element, and get appid
        if (boost::regex_search(line, m, appid_pattern)) {
          pkg_path /= bf::path("bin");

          result = boost::regex_replace(line, appid_exec_replace_pattern,
              std::string("exec=\"") + pkg_path.string() +
              std::string("/$1\""));

        } else if (boost::regex_search(line, m, manifest_pattern)) {
          // Check if the 'type=' attribute is not given
          LOG(DEBUG) << "found manifest";
          if (m[1].str().size() == 0) {
            // Add an attribute
            result = boost::regex_replace(line,
                manifest_type_replace_pattern,
                std::string("<manifest type=\"tpk\" "));
            LOG(DEBUG) << "Found no type attribute. changed:" << result;
          }
        }
      } catch (boost::regex_error& e) {
        LOG(ERROR) << "Regex error:" << e.what() << ":" << e.code();
        return false;
      }

      // Write file
      outfile << result << std::endl;
    }

    // Change old file to new file
    bf::remove(xml_path);
    bf::rename(xml_path.string() + ".out", xml_path);

    return true;
  }
}  // namespace


namespace tpk {
namespace filesystem {

SCOPE_LOG_TAG(TPKCopyManifestXml)

typedef common_installer::Step::Status Status;

Status StepCopyManifestXml::process() {
  // Set source xml path
  bf::path source_xml_path;
  if (!_setValidSourceXmlPath(&source_xml_path, context_))
    return Status::ERROR;

  // Create destination xml path
  _setDestXmlPath(&(this->dest_xml_path_), context_->uid.get(),
      context_->pkgid.get());
  bf::path& dest_xml_path = *dest_xml_path_;
  if (!common_installer::CreateDir(dest_xml_path.parent_path())) {
    LOG(ERROR) << "Destionation directory creation failure" << dest_xml_path;
    return Status::ERROR;
  }

  // Backup old xml file if exists
  if (bf::exists(dest_xml_path)) {
    common_installer::MoveFile(dest_xml_path,
        _getOldDestXmlPath(dest_xml_path));
  }

  // Copy xml file
  boost::system::error_code boost_err;
  bf::copy_file(source_xml_path, dest_xml_path, boost_err);
  if (boost_err) {
    LOG(ERROR) << "Manifest xml copy failure, from " << source_xml_path <<
        " to " << dest_xml_path << ", error_code=" << boost_err;
    return Status::ERROR;
  }

  // Trim xml file content
  if (!_trimCopiedXmlContent(dest_xml_path, context_)) return Status::ERROR;

  // Set dest_xml_path (to be used by StepRegisterApplication)
  context_->xml_path.set(dest_xml_path.string());

  return Status::OK;
}


Status StepCopyManifestXml::clean() {
  // Delete old xml file if exists
  bf::path old_dest_xml_path = _getOldDestXmlPath(*dest_xml_path_);
  if (bf::exists(old_dest_xml_path)) bf::remove(old_dest_xml_path);

  return Status::OK;
}


Status StepCopyManifestXml::undo() {
  // Remove xml file
  if (bf::exists(*dest_xml_path_)) bf::remove(*dest_xml_path_);

  // Revert old xml file if exists
  bf::path old_dest_xml_path = _getOldDestXmlPath(*dest_xml_path_);
  if (bf::exists(old_dest_xml_path)) {
    if (!common_installer::MoveFile(old_dest_xml_path, *dest_xml_path_)) {
      LOG(ERROR) << "Cannot revert old xml file: " << old_dest_xml_path;
      return Status::ERROR;
    }
  }

  return Status::OK;
}

}  // namespace filesystem
}  // namespace tpk
