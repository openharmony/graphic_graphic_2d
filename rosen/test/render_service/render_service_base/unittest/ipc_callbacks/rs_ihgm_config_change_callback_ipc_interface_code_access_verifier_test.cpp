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

#include "gtest/gtest.h"
#include "ipc_callbacks/rs_ihgm_config_change_callback_ipc_interface_code_access_verifier.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSIHgmConfigChangeCallbackInterfaceCodeAccessVerifierTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSIHgmConfigChangeCallbackInterfaceCodeAccessVerifierTest::SetUpTestCase() {}
void RSIHgmConfigChangeCallbackInterfaceCodeAccessVerifierTest::TearDownTestCase() {}
void RSIHgmConfigChangeCallbackInterfaceCodeAccessVerifierTest::SetUp() {}
void RSIHgmConfigChangeCallbackInterfaceCodeAccessVerifierTest::TearDown() {}

/**
 * @tc.name: IsExclusiveVerificationPassed001
 * @tc.desc: Verify function IsExclusiveVerificationPassed incorrect code
 * @tc.type: FUNC
 */
HWTEST_F(RSIHgmConfigChangeCallbackInterfaceCodeAccessVerifierTest, IsExclusiveVerificationPassed001, TestSize.Level1)
{
    RSIHgmConfigChangeCallbackInterfaceCodeAccessVerifier verifier;
    ASSERT_TRUE(verifier.IsExclusiveVerificationPassed(-1));
}

/**
 * @tc.name: IsExclusiveVerificationPassed002
 * @tc.desc: Verify function IsExclusiveVerificationPassed code: ON_HGM_CONFIG_CHANGED
 * @tc.type: FUNC
 */
HWTEST_F(RSIHgmConfigChangeCallbackInterfaceCodeAccessVerifierTest, IsExclusiveVerificationPassed002, TestSize.Level1)
{
    auto code = static_cast<CodeUnderlyingType>(RSIHgmConfigChangeCallbackInterfaceCode::ON_HGM_CONFIG_CHANGED);
    RSIHgmConfigChangeCallbackInterfaceCodeAccessVerifier verifier;
    ASSERT_TRUE(verifier.IsExclusiveVerificationPassed(code));
}

/**
 * @tc.name: IsExclusiveVerificationPassed003
 * @tc.desc: Verify function IsExclusiveVerificationPassed code: ON_HGM_REFRESH_RATE_MODE_CHANGED
 * @tc.type: FUNC
 */
HWTEST_F(RSIHgmConfigChangeCallbackInterfaceCodeAccessVerifierTest, IsExclusiveVerificationPassed003, TestSize.Level1)
{
    auto code =
            static_cast<CodeUnderlyingType>(RSIHgmConfigChangeCallbackInterfaceCode::ON_HGM_REFRESH_RATE_MODE_CHANGED);
    RSIHgmConfigChangeCallbackInterfaceCodeAccessVerifier verifier;
    ASSERT_TRUE(verifier.IsExclusiveVerificationPassed(code));
}

/**
 * @tc.name: IsExclusiveVerificationPassed004
 * @tc.desc: Verify function IsExclusiveVerificationPassed code: ON_HGM_REFRESH_RATE_CHANGED
 * @tc.type: FUNC
 */
HWTEST_F(RSIHgmConfigChangeCallbackInterfaceCodeAccessVerifierTest, IsExclusiveVerificationPassed004, TestSize.Level1)
{
    auto code = static_cast<CodeUnderlyingType>(RSIHgmConfigChangeCallbackInterfaceCode::ON_HGM_REFRESH_RATE_CHANGED);
    RSIHgmConfigChangeCallbackInterfaceCodeAccessVerifier verifier;
    ASSERT_TRUE(verifier.IsExclusiveVerificationPassed(code));
}

} // namespace OHOS::Rosen
