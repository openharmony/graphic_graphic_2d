/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "ipc_security/rs_ipc_interface_code_access_verifier_base.h"
#include "platform/ohos/rs_irender_service_connection_ipc_interface_code_access_verifier.h"
#include "ipc_callbacks/buffer_available_callback_ipc_interface_code_access_verifier.h"

namespace OHOS::Rosen {
class RSInterfaceCodeAccessVerifierBaseTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSInterfaceCodeAccessVerifierBaseTest::SetUpTestCase() {}
void RSInterfaceCodeAccessVerifierBaseTest::TearDownTestCase() {}
void RSInterfaceCodeAccessVerifierBaseTest::SetUp() {}
void RSInterfaceCodeAccessVerifierBaseTest::TearDown() {}

/**
 * @tc.name: IsSystemCallingTest
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSInterfaceCodeAccessVerifierBaseTest, IsSystemCallingTest, testing::ext::TestSize.Level1)
{
    auto verifier = std::make_unique<RSIBufferAvailableCallbackInterfaceCodeAccessVerifier>();
    const std::string callingCode = "test";
    ASSERT_EQ(verifier->IsSystemCalling(callingCode), true);
}

/**
 * @tc.name: CheckPermissionTest
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSInterfaceCodeAccessVerifierBaseTest, CheckPermissionTest, testing::ext::TestSize.Level1)
{
    auto verifier = std::make_unique<RSIBufferAvailableCallbackInterfaceCodeAccessVerifier>();
    CodeUnderlyingType code = 0;
    ASSERT_EQ(verifier->CheckPermission(code), true);
}

/**
 * @tc.name: IsPermissionAuthenticatedTest
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSInterfaceCodeAccessVerifierBaseTest, IsPermissionAuthenticatedTest, testing::ext::TestSize.Level1)
{
    auto verifier = std::make_unique<RSIBufferAvailableCallbackInterfaceCodeAccessVerifier>();
    CodeUnderlyingType code = 0;
    ASSERT_EQ(verifier->IsPermissionAuthenticated(code), true);
}

/**
 * @tc.name: IsCommonVerificationPassedTest
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSInterfaceCodeAccessVerifierBaseTest, IsCommonVerificationPassedTest, testing::ext::TestSize.Level1)
{
    auto verifier = std::make_unique<RSIBufferAvailableCallbackInterfaceCodeAccessVerifier>();
    CodeUnderlyingType code = 0;
    ASSERT_EQ(verifier->IsCommonVerificationPassed(code), true);
}
} // namespace OHOS::Rosen