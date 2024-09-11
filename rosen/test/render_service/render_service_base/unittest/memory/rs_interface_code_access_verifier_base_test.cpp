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
 * @tc.name: CheckPermissionTest001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSInterfaceCodeAccessVerifierBaseTest, CheckPermissionTest001, testing::ext::TestSize.Level1)
{
    auto verifier = std::make_unique<RSIBufferAvailableCallbackInterfaceCodeAccessVerifier>();
    CodeUnderlyingType code = 0;
    ASSERT_EQ(verifier->CheckPermission(code), true);
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

/**
 * @tc.name: IsStylusServiceCallingTest
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:IAOZFC
 */
HWTEST_F(RSInterfaceCodeAccessVerifierBaseTest, IsStylusServiceCallingTest, testing::ext::TestSize.Level1)
{
    const std::string callingCode = "RSInterfaceCodeAccessVerifierBaseTest::IsStylusServiceCallingTest";
    auto verifier = std::make_unique<RSIBufferAvailableCallbackInterfaceCodeAccessVerifier>();
    ASSERT_EQ(verifier->IsStylusServiceCalling(callingCode), true);
}
#ifdef ENABLE_IPC_SECURITY
/**
 * @tc.name: IsInterfaceCodeAccessibleTest001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSInterfaceCodeAccessVerifierBaseTest, IsInterfaceCodeAccessibleTest001, testing::ext::TestSize.Level1)
{
    CodeUnderlyingType code = 0;
    auto verifier = std::make_unique<RSIBufferAvailableCallbackInterfaceCodeAccessVerifier>();
    ASSERT_EQ(verifier->IsInterfaceCodeAccessible(code), true);
}

/**
 * @tc.name: IsInterfaceCodeAccessibleTest002
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSInterfaceCodeAccessVerifierBaseTest, IsInterfaceCodeAccessibleTest002, testing::ext::TestSize.Level1)
{
    CodeUnderlyingType code = 0;
    auto verifier = std::make_unique<RSIBufferAvailableCallbackInterfaceCodeAccessVerifier>();
    ASSERT_EQ(verifier->IsInterfaceCodeAccessible(code), true);
}

/**
 * @tc.name: CheckNativePermissionTest
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSInterfaceCodeAccessVerifierBaseTest, CheckNativePermissionTest, testing::ext::TestSize.Level1)
{
    CodeUnderlyingType interfaceName = 1.0;
    auto verifier = std::make_unique<RSIBufferAvailableCallbackInterfaceCodeAccessVerifier>();
    auto permissions = verifier->GetPermissions(interfaceName);
    auto tokenID = verifier->GetTokenID();
    for (auto& permission : permissions) {
        ASSERT_EQ(verifier->CheckNativePermission(tokenID, permission), true);
    }
}

/**
 * @tc.name: CheckHapPermissionTest
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSInterfaceCodeAccessVerifierBaseTest, CheckHapPermissionTest, testing::ext::TestSize.Level1)
{
    CodeUnderlyingType interfaceName = 1.0;
    auto verifier = std::make_unique<RSIBufferAvailableCallbackInterfaceCodeAccessVerifier>();
    auto permissions = verifier->GetPermissions(interfaceName);
    auto tokenID = verifier->GetTokenID();
    for (auto& permission : permissions) {
        ASSERT_EQ(verifier->CheckHapPermission(tokenID, permission), true);
    }
}

/**
 * @tc.name: CheckPermissionTest002
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSInterfaceCodeAccessVerifierBaseTest, CheckPermissionTest002, testing::ext::TestSize.Level1)
{
    CodeUnderlyingType code = 0;
    auto verifier = std::make_unique<RSIBufferAvailableCallbackInterfaceCodeAccessVerifier>();
    ASSERT_EQ(verifier->CheckPermission(code), true);
}

/**
 * @tc.name: PermissionEnumToStringTest001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSInterfaceCodeAccessVerifierBaseTest, PermissionEnumToStringTest001, testing::ext::TestSize.Level1)
{
    PermissionType permission = PermissionType::CAPTURE_SCREEN;
    auto verifier = std::make_unique<RSIBufferAvailableCallbackInterfaceCodeAccessVerifier>();
    ASSERT_TRUE(verifier->PermissionEnumToString(permission) != "unknown");
}


/**
 * @tc.name: AddPermissionTest
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSInterfaceCodeAccessVerifierBaseTest, AddPermissionTest, testing::ext::TestSize.Level1)
{
    CodeUnderlyingType interfaceName = 1.0;
    const std::string newPermission = "CAPTURE_SCREEN";
    auto verifier = std::make_unique<RSIBufferAvailableCallbackInterfaceCodeAccessVerifier>();
    ASSERT_EQ(verifier->AddPermission(interfaceName, newPermission), true);
}

/**
 * @tc.name: GetPermissionsTest
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSInterfaceCodeAccessVerifierBaseTest, GetPermissionsTest, testing::ext::TestSize.Level1)
{
    CodeUnderlyingType interfaceName = 1.0;
    const std::string newPermission = "CAPTURE_SCREEN";
    auto verifier = std::make_unique<RSIBufferAvailableCallbackInterfaceCodeAccessVerifier>();
    auto ret = verifier->GetPermissions(interfaceName);
    ASSERT_EQ(ret.empty(), true);
    verifier->AddPermission(interfaceName, newPermission);
    ret = verifier->GetPermissions(interfaceName);
    ASSERT_EQ(ret.empty(), false);
}

/**
 * @tc.name: GetInterfacePermissionSizeTest
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSInterfaceCodeAccessVerifierBaseTest, GetInterfacePermissionSizeTest, testing::ext::TestSize.Level1)
{
    auto verifier = std::make_unique<RSIBufferAvailableCallbackInterfaceCodeAccessVerifier>();
    ASSERT_EQ(verifier->GetInterfacePermissionSize(), 0);
}

/**
 * @tc.name: IsSystemAppTest
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSInterfaceCodeAccessVerifierBaseTest, IsSystemAppTest, testing::ext::TestSize.Level1)
{
    auto verifier = std::make_unique<RSIBufferAvailableCallbackInterfaceCodeAccessVerifier>();
    ASSERT_EQ(verifier->IsSystemApp(), false);
}

/**
 * @tc.name: IsSystemCallingTest001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSInterfaceCodeAccessVerifierBaseTest, IsSystemCallingTest001, testing::ext::TestSize.Level1)
{
    const std::string callingCode = "RSIOcclusionChangeCallbackInterfaceCode::ON_OCCLUSION_VISIBLE_CHANGED";
    auto verifier = std::make_unique<RSIBufferAvailableCallbackInterfaceCodeAccessVerifier>();
    bool res = verifier->IsSystemCalling(callingCode);
    ASSERT_EQ(res, true);
}

/**
 * @tc.name: IsStylusServiceCallingTest001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:IAOZFC
 */
HWTEST_F(RSInterfaceCodeAccessVerifierBaseTest, IsStylusServiceCallingTest001, testing::ext::TestSize.Level1)
{
    const std::string callingCode = "RSInterfaceCodeAccessVerifierBaseTest::IsStylusServiceCallingTest001";
    auto verifier = std::make_unique<RSIBufferAvailableCallbackInterfaceCodeAccessVerifier>();
    ASSERT_EQ(verifier->IsStylusServiceCalling(callingCode), false);
}
#endif
} // namespace OHOS::Rosen