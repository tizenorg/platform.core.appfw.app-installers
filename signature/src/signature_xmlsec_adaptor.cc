// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Copyright (C) 2002-2003 Aleksey Sanin.  All Rights Reserved.
// Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.
// Part of this file is redistributed from crosswalk project under BDS-style
// license. Check LICENSE-xwalk file.

#include <signature_xmlsec_adaptor.h>

#include <libxml/parser.h>
#include <openssl/bio.h>
#include <openssl/x509.h>
#include <xmlsec/crypto.h>
#include <xmlsec/io.h>
#include <xmlsec/keysmngr.h>
#include <xmlsec/xmlsec.h>
#include <xmlsec/xmltree.h>
#include <xmlsec/xmldsig.h>
#ifndef XMLSEC_NO_XSLT
#include <libxslt/xslt.h>
#endif

#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/filesystem/operations.hpp>

#include <algorithm>
#include <array>
#include <cassert>
#include <list>
#include <map>
#include <sstream>
#include <string>
#include <utility>

#include <logging.h>

namespace bai = boost::archive::iterators;
namespace bf = boost::filesystem;

namespace {

const bf::path cert_prefix_path = "/usr/share/ca-certificates/tizen/";

// TODO(XU): Once tizen platform provide certificate manager util APIs,
// we should call API from system to query certificate's file path.
class CertificateUtil {
 public:
  static const std::map<std::string, std::string>& certificate_path() {
    return certificate_path_;
  }

 private:
  static std::map<std::string, std::string> InitCertificatePath() {
    std::map<std::string, std::string> root_certificates;
    root_certificates["Tizen Partner-Manufacturer Distributor Root CA"] =
        "tizen-distributor-root-ca-partner-manufacturer.pem";
    root_certificates["SLP WebApp Temporary CA"] =
        "tizen.root.preproduction.cert.pem";
    root_certificates["Tizen Test Developer Root CA"] =
        "tizen-developer-root-ca.pem";
    root_certificates["Tizen Developers Root"] =
        "tizen-developers-root.pem";
    root_certificates["Tizen Partner Distributor Root CA"] =
        "tizen-distributor-root-ca-partner.pem";
    root_certificates["Tizen Partner-Operator Distributor Root CA"] =
        "tizen-distributor-root-ca-partner-operator.pem";
    root_certificates["Tizen Public Distributor Root CA"] =
        "tizen-distributor-root-ca-public.pem";
    root_certificates["Partner Class Developer Root"] =
        "tizen-partner-class-developer-root.pem";
    root_certificates["Partner Class Root Authority"] =
        "tizen-partner-class-root-authority.pem";
    root_certificates["Platform Class Developer Root"] =
        "tizen-platform-class-developer-root.pem";
    root_certificates["Platform Class Root Authority"] =
        "tizen-platform-class-root-authority.pem";
    root_certificates["Public Class Developer Root"] =
        "tizen-public-class-developer-root.pem";
    root_certificates["Public Class Root Authority"] =
        "tizen-public-class-root-authority.pem";

    return root_certificates;
  }

  static std::map<std::string, std::string> certificate_path_;
};

std::map<std::string, std::string>
    CertificateUtil::certificate_path_ = InitCertificatePath();

std::string ConvertBase64ToPemCert(const std::string& cert_in) {
  return std::string("-----BEGIN CERTIFICATE-----") + cert_in +
      "-----END CERTIFICATE-----";
}

std::string ConvertBase64ToDerCert(const std::string& cert_in) {
  typedef bai::transform_width<
      bai::binary_from_base64<std::string::const_iterator>, 8, 6>
          base64_dec;

  std::string cert = cert_in;

  // remove newlines from xml format
  cert.erase(std::remove(cert.begin(), cert.end(), '\n'), cert.end());

  std::stringstream der;

  // padding
  unsigned int size = cert.size();
  int padding = 0;
  if (size && cert[size - 1] == '=') {
    ++padding;
    if (size && cert[size - 2] == '=') ++padding;
  }

  // replace padding to 'zero'
  std::replace(cert.begin(), cert.end(), '=', 'A');

  std::copy(base64_dec(cert.begin()), base64_dec(cert.end()),
      std::ostream_iterator<char>(der));

  // remove padding
  cert.erase(cert.end() - padding, cert.end());

  return der.str();
}

class XmlSecContext {
 public:
  static void GetExtractedPath(const signature::SignatureData& data);
  static xmlSecKeysMngrPtr LoadTrustedCerts(
      const signature::SignatureData& signature_data);
  static bool VerifyFile(
      xmlSecKeysMngrPtr mngr, const signature::SignatureData& data);

 private:
  static int FileMatchCallback(const char* file_name);
  static void* FileOpenCallback(const char* file_name);
  static int FileReadCallback(void* context, char* buffer, int len);
  static int FileCloseCallback(void* context);
  static boost::filesystem::path GetCertFromStore(const std::string& subject);

  static bf::path prefix_path_;
  static std::pair<void*, bool> file_wrapper_;
};

bf::path XmlSecContext::prefix_path_;
std::pair<void*, bool> XmlSecContext::file_wrapper_;

void XmlSecContext::GetExtractedPath(
    const signature::SignatureData& data) {
  prefix_path_ = data.GetExtractedWidgetPath();
}

int XmlSecContext::FileMatchCallback(const char* file_name) {
  bf::path path = prefix_path_ / file_name;
  return xmlFileMatch(path.string().c_str());
}

void* XmlSecContext::FileOpenCallback(const char* file_name) {
  bf::path path = prefix_path_ / file_name;
  XmlSecContext::file_wrapper_ =
      std::make_pair(xmlFileOpen(path.string().c_str()), false);
  return &(XmlSecContext::file_wrapper_);
}

int XmlSecContext::FileReadCallback(void* context, char* buffer, int len) {
  std::pair<void*, bool>* file_wrapper =
      static_cast<std::pair<void*, bool>*>(context);
  assert(file_wrapper);
  if (file_wrapper->second)
    return 0;

  int output = xmlFileRead(file_wrapper->first, buffer, len);
  if (output == 0) {
    file_wrapper->second = true;
    xmlFileClose(file_wrapper->first);
  }
  return output;
}

int XmlSecContext::FileCloseCallback(void* context) {
  std::pair<void*, bool>* file_wrapper =
      static_cast<std::pair<void*, bool>*>(context);
  assert(file_wrapper);
  int output = 0;
  if (!file_wrapper->second)
    output = xmlFileClose(file_wrapper->first);

  return output;
}

xmlSecKeysMngrPtr XmlSecContext::LoadTrustedCerts(
    const signature::SignatureData& signature_data) {
  xmlSecKeysMngrPtr mngr = xmlSecKeysMngrCreate();
  if (!mngr) {
    LOG(ERROR) << "Error: failed to create keys manager.";
    return nullptr;
  }
  if (xmlSecCryptoAppDefaultKeysMngrInit(mngr) < 0) {
    LOG(ERROR) << "Error: failed to initialize keys manager.";
    xmlSecKeysMngrDestroy(mngr);
    return nullptr;
  }

  std::list<std::string> certificate_list = signature_data.certificate_list();
  std::string issuer;
  for (std::list<std::string>::iterator it = certificate_list.begin();
      it != certificate_list.end(); ++it) {
    std::string cert = ConvertBase64ToDerCert(*it);

    X509* x509_cert = nullptr;
    const unsigned char* data_ptr =
        reinterpret_cast<const unsigned char*>(cert.data());
    if (!d2i_X509(&x509_cert, &data_ptr, cert.size())) {
      LOG(ERROR) << "Cannot parse X509 certificate";
      return nullptr;
    }

    X509_NAME* x509_name = X509_get_issuer_name(x509_cert);
    if (!x509_name) {
      LOG(ERROR) << "Cannot extract issuer of X509 certificate";
      X509_free(x509_cert);
      return nullptr;
    }

    std::array<char, 200> buffer;
    if (X509_NAME_get_text_by_NID(x509_name,
        NID_commonName, buffer.data(), buffer.size()) == -1) {
      LOG(ERROR) << "Cannot extract DisplayName of X509 certificate";
      X509_free(x509_cert);
      return nullptr;
    }
    X509_free(x509_cert);

    issuer = buffer.data();
    LOG(INFO) << "Issuer: " << issuer;

    std::string pem = ConvertBase64ToPemCert(*it);
    if (xmlSecCryptoAppKeysMngrCertLoadMemory(mngr,
        reinterpret_cast<const unsigned char*>(pem.c_str()), pem.size(),
        xmlSecKeyDataFormatCertPem, xmlSecKeyDataTypeTrusted) < 0) {
      LOG(ERROR) << "Error: failed to load pem certificate.";
      xmlSecKeysMngrDestroy(mngr);
      return nullptr;
    }
  }

  // TODO(tiwanek): extract to seperate file...
  const bf::path& root_cert_path =
      XmlSecContext::GetCertFromStore(issuer);
  if (!bf::exists(root_cert_path.string().c_str())) {
    LOG(ERROR) << "Failed to find root certificate.";
    return nullptr;
  }

  if (xmlSecCryptoAppKeysMngrCertLoad(mngr,
      root_cert_path.string().c_str(), xmlSecKeyDataFormatPem,
      xmlSecKeyDataTypeTrusted) < 0) {
    LOG(ERROR) << "Error: failed to load root certificate";
    xmlSecKeysMngrDestroy(mngr);
    return nullptr;
  }

  return mngr;
}

bool XmlSecContext::VerifyFile(xmlSecKeysMngrPtr mngr,
                              const signature::SignatureData& data) {
  LOG(INFO) << "Verify " << data.signature_file_name();
  xmlSecIOCleanupCallbacks();
  XmlSecContext::GetExtractedPath(data);
  xmlSecIORegisterCallbacks(
      XmlSecContext::FileMatchCallback,
      XmlSecContext::FileOpenCallback,
      XmlSecContext::FileReadCallback,
      XmlSecContext::FileCloseCallback);

  xmlDocPtr doc = xmlParseFile(data.signature_file_name().string().c_str());
  if (!doc) {
    LOG(ERROR) << "Error: failed to parse "
               << data.signature_file_name().string();
    return false;
  }

  if (!xmlDocGetRootElement(doc)) {
    LOG(ERROR) << "Error: unable to get root element.";
    xmlFreeDoc(doc);
    return false;
  }

  xmlNodePtr node = xmlSecFindNode(
      xmlDocGetRootElement(doc), xmlSecNodeSignature, xmlSecDSigNs);
  if (!node) {
    LOG(ERROR) << "Error: unable to find SecNodeSignature node.";
    xmlFreeDoc(doc);
    return false;
  }

  xmlSecDSigCtxPtr dsig_ctx = xmlSecDSigCtxCreate(mngr);
  if (!dsig_ctx) {
    LOG(ERROR) << "Error: failed to create signature context.";
    xmlFreeDoc(doc);
    return false;
  }

  if (xmlSecDSigCtxVerify(dsig_ctx, node) < 0) {
    LOG(ERROR) << "Error: signature verify.";
    xmlFreeDoc(doc);
    xmlSecDSigCtxDestroy(dsig_ctx);
    return false;
  }

  if (dsig_ctx->status != xmlSecDSigStatusSucceeded)
    LOG(ERROR) << "Signature " << data.signature_file_name() <<" is INVALID";

  LOG(INFO) << "Signature  "<< data.signature_file_name() << " is OK.";

  xmlFreeDoc(doc);
  xmlSecDSigCtxDestroy(dsig_ctx);
  return true;
}

boost::filesystem::path XmlSecContext::GetCertFromStore(
    const std::string& subject) {
  std::map<std::string, std::string>::const_iterator iter =
      CertificateUtil::certificate_path().find(subject);

  if (iter == CertificateUtil::certificate_path().end()) {
    LOG(ERROR) << "Failing to find root certificate.";
    return "";
  }
  LOG(INFO) << "root cert path is " << cert_prefix_path / iter->second;
  return cert_prefix_path / iter->second;
}

}  // namespace

namespace signature {

// static
bool SignatureXmlSecAdaptor::ValidateFile(
    const SignatureData& signature_data) {
  xmlInitParser();
  xmlSubstituteEntitiesDefault(1);
#ifndef XMLSEC_NO_XSLT
  xsltSecurityPrefsPtr xslt_sec_prefs = xsltNewSecurityPrefs();
  xsltSetSecurityPrefs(
      xslt_sec_prefs, XSLT_SECPREF_READ_FILE, xsltSecurityForbid);
  xsltSetSecurityPrefs(
      xslt_sec_prefs, XSLT_SECPREF_WRITE_FILE, xsltSecurityForbid);
  xsltSetSecurityPrefs(
      xslt_sec_prefs, XSLT_SECPREF_CREATE_DIRECTORY, xsltSecurityForbid);
  xsltSetSecurityPrefs(
      xslt_sec_prefs, XSLT_SECPREF_READ_NETWORK, xsltSecurityForbid);
  xsltSetSecurityPrefs(
      xslt_sec_prefs, XSLT_SECPREF_WRITE_NETWORK, xsltSecurityForbid);
  xsltSetDefaultSecurityPrefs(xslt_sec_prefs);
#endif  // XMLSEC_NO_XSLT

  if (xmlSecInit() < 0) {
    LOG(ERROR) << "Error: xmlsec initialization failed.";
    return false;
  }

  if (xmlSecCheckVersion() != 1) {
    LOG(ERROR) << "Error: loaded xmlsec library version is not compatible.";
    return false;
  }

#ifdef XMLSEC_CRYPTO_DYNAMIC_LOADING
  if (xmlSecCryptoDLLoadLibrary(BAD_CAST XMLSEC_CRYPTO) < 0) {
    LOG(ERROR) << "Error: unable to load default xmlsec-crypto library.";
    return false;
  }
#endif  // XMLSEC_CRYPTO_DYNAMIC_LOADING

  if (xmlSecCryptoAppInit(nullptr) < 0) {
    LOG(ERROR) << "Error: crypto initialization failed.";
    return false;
  }

  if (xmlSecCryptoInit() < 0) {
    LOG(ERROR) << "Error: xmlsec-crypto initialization failed.";
    return false;
  }

  xmlSecKeysMngrPtr mngr = XmlSecContext::LoadTrustedCerts(signature_data);
  if (!mngr)
    return false;

  if (!XmlSecContext::VerifyFile(mngr, signature_data)) {
    xmlSecKeysMngrDestroy(mngr);
    return false;
  }

  xmlSecKeysMngrDestroy(mngr);
  xmlSecCryptoShutdown();
  xmlSecCryptoAppShutdown();
  xmlSecShutdown();

#ifndef XMLSEC_NO_XSLT
  xsltFreeSecurityPrefs(xslt_sec_prefs);
  xsltCleanupGlobals();
#endif  // XMLSEC_NO_XSLT
  xmlCleanupParser();

  return true;
}

}  // namespace signature
