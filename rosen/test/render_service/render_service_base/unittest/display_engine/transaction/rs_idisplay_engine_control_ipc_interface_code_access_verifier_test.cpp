/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
#include <gtest/gtest.h>
#include "display_engine/transaction/rs_idisplay_engine_control_ipc_interface_code_access_verifier.h"
#include "display_engine/transaction/rs_idisplay_engine_control_ipc_interface_code.h"
 
namespace OHOS::Rosen {
class RSIDisplayEngineControlInterfaceCodeAccessVerifierTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};
 
void RSIDisplayEngineControlInterfaceCodeAccessVerifierTest::SetUpTestCase() {}
void RSIDisplayEngineControlInterfaceCodeAccessVerifierTest::TearDownTestCase() {}
void RSIDisplayEngineControlInterfaceCodeAccessVerifierTest::SetUp() {}
void RSIDisplayEngineControlInterfaceCodeAccessVerifierTest::TearDown() {}
 
/**
 * @tc.name: ConstructorAndValidCodeVerification
 * @tc.desc: Verify that the constructor does not crash and the created instance can successfully
 *           verify a valid interface code. This implicitly tests CheckCodeUnderlyingTypeStandardized
 *           in the constructor. Using a functional assertion (IsExclusiveVerificationPassed) instead
 *           of a bare nullptr check to validate the constructor succeeded.
 * @tc.type: FUNC
 */
HWTEST_F(RSIDisplayEngineControlInterfaceCodeAccessVerifierTest, ConstructorAndValidCodeVerification,
    testing::ext::TestSize.Level1)
{
    auto verifier = std::make_unique<RSIDisplayEngineControlInterfaceCodeAccessVerifier>();
    CodeUnderlyingType code =
        static_cast<CodeUnderlyingType>(RSIDisplayEngineControlInterfaceCode::NOTIFY_DE_STATUS_CHANGE);
    ASSERT_TRUE(verifier->IsExclusiveVerificationPassed(code));
}
 
/**
 * @tc.name: IsExclusiveVerificationPassedWithValidCodes
 * @tc.desc: Verify that all three valid interface codes pass the exclusive verification.
 *           IsSystemCalling returns true by default in the test environment, so all three
 *           codes (NOTIFY_DE_STATUS_CHANGE, REGISTER_DE_STATUS_CHANGE_CALLBACK,
 *           UNREGISTER_DE_STATUS_CHANGE_CALLBACK) should return true.
 * @tc.type: FUNC
 */
HWTEST_F(RSIDisplayEngineControlInterfaceCodeAccessVerifierTest,
    IsExclusiveVerificationPassedWithValidCodes,
    testing::ext::TestSize.Level1)
{
    auto verifier = std::make_unique<RSIDisplayEngineControlInterfaceCodeAccessVerifier>();
 
    CodeUnderlyingType code =
        static_cast<CodeUnderlyingType>(RSIDisplayEngineControlInterfaceCode::NOTIFY_DE_STATUS_CHANGE);
    ASSERT_TRUE(verifier->IsExclusiveVerificationPassed(code));
 
    code = static_cast<CodeUnderlyingType>(
        RSIDisplayEngineControlInterfaceCode::REGISTER_DE_STATUS_CHANGE_CALLBACK);
    ASSERT_TRUE(verifier->IsExclusiveVerificationPassed(code));
 
    code = static_cast<CodeUnderlyingType>(
        RSIDisplayEngineControlInterfaceCode::UNREGISTER_DE_STATUS_CHANGE_CALLBACK);
    ASSERT_TRUE(verifier->IsExclusiveVerificationPassed(code));
}
 
/**
 * @tc.name: IsExclusiveVerificationPassedWithInvalidCode
 * @tc.desc: Verify that an invalid interface code falls into the default branch and returns false.
 * @tc.type: FUNC
 */
HWTEST_F(RSIDisplayEngineControlInterfaceCodeAccessVerifierTest,
    IsExclusiveVerificationPassedWithInvalidCode,
    testing::ext::TestSize.Level1)
{
    auto verifier = std::make_unique<RSIDisplayEngineControlInterfaceCodeAccessVerifier>();
    CodeUnderlyingType code = 999;
    ASSERT_FALSE(verifier->IsExclusiveVerificationPassed(code));
}
 
/**
 * @tc.name: IsAccessTimesVerificationPassedNoRestriction
 * @tc.desc: Verify that when no access time restriction is configured for an interface code,
 *           the verification passes (returns true). The restriction map is empty by default,
 *           so all codes should return true regardless of the times parameter.
 * @tc.type: FUNC
 */
HWTEST_F(RSIDisplayEngineControlInterfaceCodeAccessVerifierTest,
    IsAccessTimesVerificationPassedNoRestriction,
    testing::ext::TestSize.Level1)
{
    auto verifier = std::make_unique<RSIDisplayEngineControlInterfaceCodeAccessVerifier>();
    CodeUnderlyingType code =
        static_cast<CodeUnderlyingType>(RSIDisplayEngineControlInterfaceCode::NOTIFY_DE_STATUS_CHANGE);
    ASSERT_TRUE(verifier->IsAccessTimesVerificationPassed(code, 0));
    ASSERT_TRUE(verifier->IsAccessTimesVerificationPassed(code, 1));
    ASSERT_TRUE(verifier->IsAccessTimesVerificationPassed(code, 100));
}
} // namespace OHOS::Rosen