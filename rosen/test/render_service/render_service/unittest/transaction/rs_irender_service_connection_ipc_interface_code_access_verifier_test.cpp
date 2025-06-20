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
#include "platform/ohos/rs_irender_service_connection_ipc_interface_code_access_verifier.h"

namespace OHOS::Rosen {
class RSIRenderServiceConnectionIpcInterfaceCodeAccessVerifierTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSIRenderServiceConnectionIpcInterfaceCodeAccessVerifierTest::SetUpTestCase() {}
void RSIRenderServiceConnectionIpcInterfaceCodeAccessVerifierTest::TearDownTestCase() {}
void RSIRenderServiceConnectionIpcInterfaceCodeAccessVerifierTest::SetUp() {}
void RSIRenderServiceConnectionIpcInterfaceCodeAccessVerifierTest::TearDown() {}

/**
 * @tc.name: IsInterfaceCodeAccessibleTest001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issue#IAS6LQ
 */
HWTEST_F(RSIRenderServiceConnectionIpcInterfaceCodeAccessVerifierTest, IsInterfaceCodeAccessibleTest001,
    testing::ext::TestSize.Level1)
{
    auto verifier = std::make_unique<RSIRenderServiceConnectionInterfaceCodeAccessVerifier>();
    CodeUnderlyingType code = 0;
    ASSERT_EQ(verifier->IsInterfaceCodeAccessible(code), true);
}

/**
 * @tc.name: IsSystemCallingTest001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issue#IAS6LQ
 */
HWTEST_F(RSIRenderServiceConnectionIpcInterfaceCodeAccessVerifierTest, IsSystemCallingTest001,
    testing::ext::TestSize.Level1)
{
    auto verifier = std::make_unique<RSIRenderServiceConnectionInterfaceCodeAccessVerifier>();
    const std::string callingCode = "test";
    ASSERT_EQ(verifier->IsSystemCalling(callingCode), true);
}

/**
 * @tc.name: IsSystemCallingTest002
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueICGEDM
 */
HWTEST_F(RSIRenderServiceConnectionIpcInterfaceCodeAccessVerifierTest, IsSystemCallingTest002,
    testing::ext::TestSize.Level1)
{
    auto verifier = std::make_unique<RSIRenderServiceConnectionInterfaceCodeAccessVerifier>();
    const std::string callingCode = "debug";
    ASSERT_EQ(verifier->IsSystemCalling(callingCode), true);
}

#ifdef ENABLE_IPC_SECURITY
/**
 * @tc.name: CheckNativePermissionTest001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issue#IAS6LQ
 */
HWTEST_F(RSIRenderServiceConnectionIpcInterfaceCodeAccessVerifierTest, CheckNativePermissionTest001,
    testing::ext::TestSize.Level1)
{
    auto verifier = std::make_unique<RSIRenderServiceConnectionInterfaceCodeAccessVerifier>();
    CodeUnderlyingType interfaceName =
        static_cast<CodeUnderlyingType>(RSIRenderServiceConnectionInterfaceCode::TAKE_SURFACE_CAPTURE);
    auto permissions = verifier->GetPermissions(interfaceName);
    auto tokenID = verifier->GetTokenID();
    for (auto& permission : permissions) {
        ASSERT_EQ(verifier->CheckNativePermission(tokenID, permission), false);
    }
}

/**
 * @tc.name: CheckHapPermissionTest001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issue#IAS6LQ
 */
HWTEST_F(RSIRenderServiceConnectionIpcInterfaceCodeAccessVerifierTest, CheckHapPermissionTest001,
    testing::ext::TestSize.Level1)
{
    auto verifier = std::make_unique<RSIRenderServiceConnectionInterfaceCodeAccessVerifier>();
    CodeUnderlyingType interfaceName =
        static_cast<CodeUnderlyingType>(RSIRenderServiceConnectionInterfaceCode::TAKE_SURFACE_CAPTURE);
    auto permissions = verifier->GetPermissions(interfaceName);
    auto tokenID = verifier->GetTokenID();
    for (auto& permission : permissions) {
        ASSERT_EQ(verifier->CheckHapPermission(tokenID, permission), false);
    }
}

/**
 * @tc.name: PermissionEnumToStringTest001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issue#IAS6LQ
 */
HWTEST_F(RSIRenderServiceConnectionIpcInterfaceCodeAccessVerifierTest, PermissionEnumToStringTest001,
    testing::ext::TestSize.Level1)
{
    auto verifier = std::make_unique<RSIRenderServiceConnectionInterfaceCodeAccessVerifier>();
    PermissionType permission = PermissionType::CAPTURE_SCREEN;
    ASSERT_TRUE(verifier->PermissionEnumToString(permission) != "unknown");
}

/**
 * @tc.name: AddPermissionTest001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issue#IAS6LQ
 */
HWTEST_F(RSIRenderServiceConnectionIpcInterfaceCodeAccessVerifierTest, AddPermissionTest001,
    testing::ext::TestSize.Level1)
{
    auto verifier = std::make_unique<RSIRenderServiceConnectionInterfaceCodeAccessVerifier>();
    CodeUnderlyingType interfaceName = 0;
    const std::string newPermission = "CAPTURE_SCREEN";
    ASSERT_EQ(verifier->AddPermission(interfaceName, newPermission), true);
}

/**
 * @tc.name: GetPermissionsTest001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issue#IAS6LQ
 */
HWTEST_F(RSIRenderServiceConnectionIpcInterfaceCodeAccessVerifierTest, GetPermissionsTest001,
    testing::ext::TestSize.Level1)
{
    auto verifier = std::make_unique<RSIRenderServiceConnectionInterfaceCodeAccessVerifier>();
    CodeUnderlyingType interfaceName = 0;
    const std::string newPermission = "CAPTURE_SCREEN";
    auto ret = verifier->GetPermissions(interfaceName);
    ASSERT_EQ(ret.empty(), true);
    verifier->AddPermission(interfaceName, newPermission);
    ret = verifier->GetPermissions(interfaceName);
    ASSERT_EQ(ret.empty(), false);
}

/**
 * @tc.name: GetInterfacePermissionSizeTest001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issue#IAS6LQ
 */
HWTEST_F(RSIRenderServiceConnectionIpcInterfaceCodeAccessVerifierTest, GetInterfacePermissionSizeTest001,
    testing::ext::TestSize.Level1)
{
    auto verifier = std::make_unique<RSIRenderServiceConnectionInterfaceCodeAccessVerifier>();
    bool isPermissionSizeEmpty = (verifier->GetInterfacePermissionSize() == 0);
    EXPECT_FALSE(isPermissionSizeEmpty);
}

/**
 * @tc.name: IsSystemAppTest001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issue#IAS6LQ
 */
HWTEST_F(RSIRenderServiceConnectionIpcInterfaceCodeAccessVerifierTest, IsSystemAppTest001,
    testing::ext::TestSize.Level1)
{
    auto verifier = std::make_unique<RSIRenderServiceConnectionInterfaceCodeAccessVerifier>();
    ASSERT_EQ(verifier->IsSystemApp(), false);
}
#endif

/**
 * @tc.name: CheckPermissionTest001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issue#IAS6LQ
 */
HWTEST_F(RSIRenderServiceConnectionIpcInterfaceCodeAccessVerifierTest, CheckPermissionTest001,
    testing::ext::TestSize.Level1)
{
    auto verifier = std::make_unique<RSIRenderServiceConnectionInterfaceCodeAccessVerifier>();
    CodeUnderlyingType code = 0;
    ASSERT_EQ(verifier->CheckPermission(code), true);
}

/**
 * @tc.name: IsStylusServiceCallingTest001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issue#IAS6LQ
 */
HWTEST_F(RSIRenderServiceConnectionIpcInterfaceCodeAccessVerifierTest, IsStylusServiceCallingTest001,
    testing::ext::TestSize.Level1)
{
    auto verifier = std::make_unique<RSIRenderServiceConnectionInterfaceCodeAccessVerifier>();
    const std::string callingCode = "test";
#ifdef ENABLE_IPC_SECURITY
    ASSERT_EQ(verifier->IsStylusServiceCalling(callingCode), false);
#else
    ASSERT_EQ(verifier->IsStylusServiceCalling(callingCode), true);
#endif
}

/**
 * @tc.name: IsCommonVerificationPassedTest001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issue#IAS6LQ
 */
HWTEST_F(RSIRenderServiceConnectionIpcInterfaceCodeAccessVerifierTest, IsCommonVerificationPassedTest001,
    testing::ext::TestSize.Level1)
{
    auto verifier = std::make_unique<RSIRenderServiceConnectionInterfaceCodeAccessVerifier>();
    CodeUnderlyingType code = 0;
    ASSERT_EQ(verifier->IsCommonVerificationPassed(code), true);
}

/**
 * @tc.name: IsAccessTimesVerificationPassedTest
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSIRenderServiceConnectionIpcInterfaceCodeAccessVerifierTest, IsAccessTimesVerificationPassedTest,
    testing::ext::TestSize.Level1)
{
    auto verifier = std::make_unique<RSIRenderServiceConnectionInterfaceCodeAccessVerifier>();
    CodeUnderlyingType code = 0;
    ASSERT_EQ(verifier->IsAccessTimesVerificationPassed(code, 0), true);
}
} // namespace OHOS::Rosen