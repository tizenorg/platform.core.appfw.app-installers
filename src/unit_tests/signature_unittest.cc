// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include <boost/filesystem/path.hpp>
#include <gtest/gtest.h>

#include <memory>

#include "common/step/step_check_signature.h"

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
  PrivilegeLevel level = PrivilegeLevel::UNTRUSTED;
  common_installer::CertificateInfo cert_info;
  EXPECT_EQ(ValidateSignatures(*signature_file, &level, &cert_info),
            Step::Status::OK);
}

// Tests signature verifier with signature directory containing bad signatures
TEST_F(SignatureValidatorTest, HandlesBadSignatureDir) {
  signature_file.reset(new bf::path(
      "/usr/share/app-installers-ut/test_samples/bad_signatures"));
  PrivilegeLevel level = PrivilegeLevel::UNTRUSTED;
  common_installer::CertificateInfo cert_info;
  EXPECT_EQ(ValidateSignatures(*signature_file, &level, &cert_info),
            Step::Status::ERROR);
}

}  // namespace signature
}  // namespace common_installer
