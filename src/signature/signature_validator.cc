// Copyright (c) 2013 Intel Corporation. All rights reserved.
// Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.
// Part of this file is redistributed from crosswalk project under BDS-style
// license. Check LICENSE-xwalk file.

#include "signature/signature_validator.h"

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xmlschemas.h>

#include <memory>
#include <regex>
#include <set>
#include <string>

#include "signature/signature_data.h"
#include "signature/signature_parser.h"
#include "signature/signature_xmlsec_adaptor.h"

namespace bf = boost::filesystem;

namespace {

const int kXMLLogSize = 1024;

const char kAuthorSignatureName[] = "author-signature.xml";
const char kTokenRoleAuthorURI[] =
    "http://www.w3.org/ns/widgets-digsig#role-author";
const char kTokenRoleDistributor[] =
    "http://www.w3.org/ns/widgets-digsig#role-distributor";
const char kTokenProfileURI[] =
    "http://www.w3.org/ns/widgets-digsig#profile";
const char kSignatureSchemaPath[] =
    "/usr/share/app-installers/signature_schema.xsd";

const std::regex kDistributorSignatureRegex("^signature([1-9][0-9]*)\\.xml$");

//  A wrapper of LOG(ERROR) function, which  is used as parameter of function
//  xmlSchemaSetValidErrors
void LogErrorLibxml2(void *, const char *msg, ...) {
  char buffer[kXMLLogSize];
  va_list args;
  va_start(args, msg);
  vsnprintf(buffer, sizeof(buffer), msg, args);
  va_end(args);
  LOG(ERROR) << "ERROR: " << buffer;
}

//  A wrapper of LOG(WARNING) function, which  is used as parameter of function
//  xmlSchemaSetValidErrors
void LogWarningLibxml2(void *, const char *msg, ...) {
  char buffer[kXMLLogSize];
  va_list args;
  va_start(args, msg);
  vsnprintf(buffer, sizeof(buffer), msg, args);
  va_end(args);
  LOG(WARNING) << "Warning: " << buffer;
}

class SignatureFile {
 public:
  SignatureFile(const std::string& file_name, int file_number)
    : file_name_(file_name), file_number_(file_number) {
  }

  std::string file_name() const {
    return file_name_;
  }

  int file_number() const {
    return file_number_;
  }

  bool operator<(const SignatureFile &second) const {
    return file_number_ < second.file_number();
  }

 private:
  std::string file_name_;
  int file_number_;
};
typedef std::set<SignatureFile> SignatureFileSet;

const SignatureFileSet GetSignatureFiles(
    const boost::filesystem::path& widget_path) {
  SignatureFileSet signature_set;

  for (bf::directory_iterator dir_iterator(widget_path);
      dir_iterator != bf::directory_iterator(); ++dir_iterator) {
    const bf::path& path = dir_iterator->path();
    if (path.extension() == ".xml" && bf::is_regular_file(path)) {
      std::string file_name = path.filename().string();
      if (file_name == kAuthorSignatureName) {
        // Find author signature file.
        signature_set.insert(SignatureFile(file_name, -1));
        continue;
      }
      std::smatch match;
      if (std::regex_match(file_name, match, kDistributorSignatureRegex)) {
        if (match.size() == 2) {
          // Find distributor signature file.
          signature_set.insert(
              SignatureFile(file_name, std::stoi(match[1].str())));
        }
      }
    }
  }
  return signature_set;
}

bool XMLSchemaValidate(
    const SignatureFile& signature_file,
    const boost::filesystem::path& widget_path) {
  xmlDocPtr schema_doc = xmlReadFile(
      kSignatureSchemaPath, nullptr, XML_PARSE_NONET|XML_PARSE_NOENT);
  if (!schema_doc) {
    LOG(ERROR) << "Reading schema file failed.";
    return false;
  }

  xmlSchemaParserCtxtPtr ctx = xmlSchemaNewParserCtxt(kSignatureSchemaPath);
  if (!ctx) {
    LOG(ERROR) << "Initing xml schema parser context failed.";
    return false;
  }

  xmlSchemaPtr xschema = xmlSchemaParse(ctx);
  if (!xschema) {
    LOG(ERROR) << "Parsing xml schema failed.";
    return false;
  }

  xmlSchemaValidCtxtPtr vctx = xmlSchemaNewValidCtxt(xschema);
  if (!vctx) {
    LOG(ERROR) << "Initing xml schema context failed.";
    return false;
  }
  xmlSchemaSetValidErrors(vctx, (xmlSchemaValidityErrorFunc)&LogErrorLibxml2,
      (xmlSchemaValidityWarningFunc)&LogWarningLibxml2, nullptr);

  bf::path signature_path = widget_path / signature_file.file_name();
  int ret = xmlSchemaValidateFile(vctx, signature_path.string().c_str(), 0);

  if (ret != 0) {
    LOG(ERROR) << "Validating " << signature_file.file_name()
               << " schema failed.";
    return false;
  }
  return true;
}

bool CheckObjectID(
    const common_installer::signature::SignatureData& signature_data) {
  std::string object_id = signature_data.object_id();
  std::set<std::string> reference_set = signature_data.reference_set();

  std::set<std::string>::const_iterator result =
    reference_set.find(std::string("#") + object_id);
  if (result == reference_set.end()) {
    LOG(ERROR) << "No reference to object.";
    return false;
  }
  return true;
}

bool CheckRoleURI(
    const common_installer::signature::SignatureData& signature_data) {
  std::string role_uri = signature_data.role_uri();

  if (role_uri.empty()) {
    LOG(ERROR) << "URI attribute in Role tag couldn't be empty.";
    return false;
  }

  if (role_uri != kTokenRoleAuthorURI && signature_data.isAuthorSignature()) {
    LOG(ERROR) << "URI attribute in Role tag does not "
               << "match with signature filename.";
    return false;
  }

  if (role_uri != kTokenRoleDistributor &&
      !signature_data.isAuthorSignature()) {
    LOG(ERROR) << "URI attribute in Role tag does not "
               << "match with signature filename.";
    return false;
  }

  return true;
}

bool CheckProfileURI(
    const common_installer::signature::SignatureData& signature_data) {
  if (kTokenProfileURI != signature_data.profile_uri()) {
    LOG(ERROR) << "Profile tag contains unsupported value in URI attribute.";
    return false;
  }
  return true;
}

boost::filesystem::path RelativePath(const boost::filesystem::path& path,
    const boost::filesystem::path& root) {
  if (root.empty())
    return path;

  std::string path_str = path.string();
  std::string root_str = root.string();

  if (root_str != path_str.substr(0, root_str.size()))
    return bf::path();

  auto index = root_str.size();

  if (root_str[root_str.size() - 1] != '/') {
    if (path_str[root_str.size()] != '/') {
      return bf::path();
    } else {
      ++index;
    }
  }

  return path_str.substr(index);
}

bool CheckReference(
    const common_installer::signature::SignatureData& signature_data) {
  const bf::path& widget_path = signature_data.GetExtractedWidgetPath();
  const std::set<std::string>& reference_set = signature_data.reference_set();

  for (bf::recursive_directory_iterator dir_iterator(widget_path);
      dir_iterator != bf::recursive_directory_iterator(); ++dir_iterator) {
    const bf::path& path = dir_iterator->path();
    if (bf::is_regular_file(path)) {
      std::string relative_path = RelativePath(path, widget_path).string();
      if (relative_path == kAuthorSignatureName ||
          std::regex_match(relative_path, kDistributorSignatureRegex)) {
        // Skip signtature file.
        continue;
      }
      std::set<std::string>::iterator ref_iter =
          reference_set.find(relative_path);
      if (ref_iter == reference_set.end()) {
        LOG(ERROR) << relative_path << " is not in signature ds:Reference.";
        return false;
      }
    }
  }
  return true;
}

}  // anonymous namespace

namespace common_installer {
namespace signature {
// static
SignatureValidator::Status SignatureValidator::Check(
    const boost::filesystem::path& widget_path) {
  LOG(INFO) << "Verifying widget signature file.";
  // Process every signature files (author and distributor) according to
  // http://www.w3.org/TR/widgets-digsig/#signature-verification.
  const SignatureFileSet& signature_set = GetSignatureFiles(widget_path);
  if (signature_set.empty()) {
    LOG(INFO) << "No signed signature in the package.";
    return UNTRUSTED;
  }

  SignatureFileSet::reverse_iterator iter = signature_set.rbegin();
  for (; iter != signature_set.rend(); ++iter) {
    // Verify whether signature xml is a valid [XMLDSIG] document.
    if (!XMLSchemaValidate(*iter, widget_path)) {
      LOG(ERROR) << "Validating " << iter->file_name() << "schema failed.";
      return INVALID;
    }

    std::unique_ptr<SignatureData> data = SignatureParser::CreateSignatureData(
        widget_path / iter->file_name(), iter->file_number());
    // Check whether each file in the widget can be found from ds:Reference.
    if (!CheckReference(*data.get()))
      return INVALID;

    // Validate the profile property.
    if (!CheckProfileURI(*data.get()))
      return INVALID;

    // Validate the identifier property.
    if (!CheckObjectID(*data.get()))
      return INVALID;

    // Validate role property.
    if (!CheckRoleURI(*data.get()))
      return INVALID;

    // Perform reference validation and signature validation on signature
    if (!SignatureXmlSecAdaptor::ValidateFile(*data.get()))
      return INVALID;
  }
  return VALID;
}

}  // namespace signature
}  // namespace common_installer
