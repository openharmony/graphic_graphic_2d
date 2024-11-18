/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "platform/ohos/rs_irender_service_ipc_interface_code_access_verifier.h"

namespace OHOS::Rosen {
class RSIRenderServiceIpcInterfaceCodeAccessVerifierTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSIRenderServiceIpcInterfaceCodeAccessVerifierTest::SetUpTestCase() {}
void RSIRenderServiceIpcInterfaceCodeAccessVerifierTest::TearDownTestCase() {}
void RSIRenderServiceIpcInterfaceCodeAccessVerifierTest::SetUp() {}
void RSIRenderServiceIpcInterfaceCodeAccessVerifierTest::TearDown() {}

/**
 * @tc.name: IsExclusiveVerificationPassedTest
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSIRenderServiceIpcInterfaceCodeAccessVerifierTest, IsExclusiveVerificationPassedTest,
    testing::ext::TestSize.Level1)
{
    auto verifier = std::make_unique<RSIRenderServiceInterfaceCodeAccessVerifier>();
    CodeUnderlyingType code = 0;
    ASSERT_EQ(verifier->IsExclusiveVerificationPassed(code), true);
    code = 1;
    ASSERT_EQ(verifier->IsExclusiveVerificationPassed(code), true);
}

/**
 * @tc.name: IsAccessTimesVerificationPassedTest
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSIRenderServiceIpcInterfaceCodeAccessVerifierTest, IsAccessTimesVerificationPassedTest,
    testing::ext::TestSize.Level1)
{
#ifdef ENABLE_IPC_SECURITY
    auto verifier = std::make_unique<RSIRenderServiceInterfaceCodeAccessVerifier>();
    CodeUnderlyingType code = 0;
    ASSERT_EQ(verifier->IsAccessTimesVerificationPassed(code, 0), true);
#endif
}
} // namespace OHOS::Rosen