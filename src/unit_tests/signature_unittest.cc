// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include <boost/filesystem/path.hpp>
#include <gtest/gtest.h>

#include <memory>

#include "signature/signature_data.h"
#include "signature/signature_parser.h"
#include "signature/signature_validator.h"
#include "signature/signature_xmlsec_adaptor.h"

namespace bf = boost::filesystem;

namespace common_installer {
namespace signature {

class SignatureValidatorTest : public testing::Test {
 protected:
  std::unique_ptr<bf::path> signature_file;
};


// Tests signature verifier with proper signatures
TEST_F(SignatureValidatorTest, HandlesInitializedSignatureDir) {
  signature_file.reset(new bf::path(
      "/usr/share/app-installers-ut/test_samples/good_signatures"));
  ASSERT_NE(nullptr, signature_file);
  EXPECT_EQ(SignatureValidator::Check(*signature_file),
            SignatureValidator::Status::VALID);
}

// Tests signature verifier with signature directory containing bad signatures
TEST_F(SignatureValidatorTest, HandlesBadSignatureDir) {
  signature_file.reset(new bf::path(
      "/usr/share/app-installers-ut/test_samples/bad_signatures"));
  ASSERT_NE(nullptr, signature_file);
  EXPECT_EQ(SignatureValidator::Check(*signature_file),
            SignatureValidator::Status::INVALID);
}

class SignatureXMLSecAdaptorTest : public testing::Test {
 protected:
  std::unique_ptr<bf::path> signature_file;
  std::unique_ptr<SignatureData> data;
};

// Tests SignatureXMLSecAdaptor file verification with proper data
TEST_F(SignatureXMLSecAdaptorTest, HandlesValidateFileInitializedSignature) {
  signature_file.reset(new bf::path(
      "/usr/share/app-installers-ut/test_samples/good_signatures/signature1.xml"));
  data = SignatureParser::CreateSignatureData(*signature_file, 1);
  ASSERT_TRUE(data.get());
  EXPECT_TRUE(SignatureXmlSecAdaptor::ValidateFile(*data));
}

}  // namespace signature
}  // namespace common_installer
