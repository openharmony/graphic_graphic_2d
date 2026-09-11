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
#include "platform/ohos/transaction/rs_iclient_to_service_connection_ipc_interface_code_access_verifier.h"
#include "platform/ohos/transaction/rs_iclient_to_render_connection_ipc_interface_code_access_verifier.h"
#include "mock/mock_accesstoken_kit.h"
#ifdef RS_PROFILER_ENABLED
#include "rs_profiler.h"
#endif

namespace OHOS::Rosen {
namespace {
    constexpr uint32_t ROOT_UID = 0;
    constexpr uint32_t EXFUSION_UID = 7015;
    constexpr uint32_t GAME_SERVICE_UID = 7011;
    constexpr const char* EXFUSION_SERVICE_PROCESS_NAME = "exfusion_display_service";
    constexpr const char* GAME_SERVICE_PROCESS_NAME = "gameservice_server";
}
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
    auto verifier = std::make_unique<RSIClientToServiceConnectionInterfaceCodeAccessVerifier>();
    CodeUnderlyingType code = 0;
    // unlisted codes fall into the default branch and are denied
    ASSERT_EQ(verifier->IsInterfaceCodeAccessible(code), false);
    // explicitly allowed codes are accessible
    code = static_cast<CodeUnderlyingType>(
        RSIClientToServiceConnectionInterfaceCodeAccessVerifier::CodeEnumType::GET_UNI_RENDER_ENABLED);
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
    auto verifier = std::make_unique<RSIClientToServiceConnectionInterfaceCodeAccessVerifier>();
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
    auto verifier = std::make_unique<RSIClientToServiceConnectionInterfaceCodeAccessVerifier>();
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
    auto verifier = std::make_unique<RSIClientToServiceConnectionInterfaceCodeAccessVerifier>();
    CodeUnderlyingType interfaceName =
        static_cast<CodeUnderlyingType>(RSIClientToRenderConnectionInterfaceCode::TAKE_SURFACE_CAPTURE);
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
    auto verifier = std::make_unique<RSIClientToServiceConnectionInterfaceCodeAccessVerifier>();
    CodeUnderlyingType interfaceName =
        static_cast<CodeUnderlyingType>(RSIClientToRenderConnectionInterfaceCode::TAKE_SURFACE_CAPTURE);
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
    auto verifier = std::make_unique<RSIClientToServiceConnectionInterfaceCodeAccessVerifier>();
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
    auto verifier = std::make_unique<RSIClientToServiceConnectionInterfaceCodeAccessVerifier>();
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
    auto verifier = std::make_unique<RSIClientToServiceConnectionInterfaceCodeAccessVerifier>();
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
    auto verifier = std::make_unique<RSIClientToServiceConnectionInterfaceCodeAccessVerifier>();
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
    auto verifier = std::make_unique<RSIClientToServiceConnectionInterfaceCodeAccessVerifier>();
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
    auto verifier = std::make_unique<RSIClientToServiceConnectionInterfaceCodeAccessVerifier>();
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
    auto verifier = std::make_unique<RSIClientToServiceConnectionInterfaceCodeAccessVerifier>();
    const std::string callingCode = "test";
#ifdef ENABLE_IPC_SECURITY
    ASSERT_EQ(verifier->IsStylusServiceCalling(callingCode), false);
#else
    ASSERT_EQ(verifier->IsStylusServiceCalling(callingCode), true);
#endif
}

/**
 * @tc.name: IsExfusionServiceCallingTest001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issue#IAS6LQ
 */
HWTEST_F(RSIRenderServiceConnectionIpcInterfaceCodeAccessVerifierTest, IsExfusionServiceCallingTest001,
    testing::ext::TestSize.Level1)
{
    auto verifier = std::make_unique<RSIClientToServiceConnectionInterfaceCodeAccessVerifier>();
    const std::string callingCode = "exfusion_test";
#ifdef ENABLE_IPC_SECURITY
    ASSERT_EQ(verifier->IsExfusionServiceCalling(callingCode), false);
    MockAccessTokenKit::MockAccessTokenKitRet(0);
    MockAccessTokenKit::MockTokenType(true);
    MockAccessTokenKit::MockProcessName(EXFUSION_SERVICE_PROCESS_NAME);
    ASSERT_EQ(verifier->IsExfusionServiceCalling(callingCode), false);
    setuid(EXFUSION_UID);
    ASSERT_EQ(verifier->IsExfusionServiceCalling(callingCode), true);
#else
    ASSERT_EQ(verifier->IsExfusionServiceCalling(callingCode), false);
    MockAccessTokenKit::MockAccessTokenKitRet(0);
    MockAccessTokenKit::MockTokenType(true);
    MockAccessTokenKit::MockProcessName(EXFUSION_SERVICE_PROCESS_NAME);
    ASSERT_EQ(verifier->IsExfusionServiceCalling(callingCode), false);
    setuid(EXFUSION_UID);
    ASSERT_EQ(verifier->IsExfusionServiceCalling(callingCode), true);
#endif
    setuid(ROOT_UID);
}

/**
 * @tc.name: IsGameServiceCallingTest001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issue#IAS6LQ
 */
HWTEST_F(RSIRenderServiceConnectionIpcInterfaceCodeAccessVerifierTest, IsGameServiceCallingTest001,
    testing::ext::TestSize.Level1)
{
    auto verifier = std::make_unique<RSIClientToServiceConnectionInterfaceCodeAccessVerifier>();
    const std::string callingCode = "gameservice_test";
#ifdef ENABLE_IPC_SECURITY
    MockAccessTokenKit::MockAccessTokenKitRet(-1);
    ASSERT_EQ(verifier->IsGameServiceCalling(callingCode), false);
    MockAccessTokenKit::MockAccessTokenKitRet(0);
    MockAccessTokenKit::MockTokenType(false);
    ASSERT_EQ(verifier->IsGameServiceCalling(callingCode), false);
    MockAccessTokenKit::MockTokenType(true);
    MockAccessTokenKit::MockProcessName(GAME_SERVICE_PROCESS_NAME);
    ASSERT_EQ(verifier->IsGameServiceCalling(callingCode), false);
    setuid(GAME_SERVICE_UID);
    ASSERT_EQ(verifier->IsGameServiceCalling(callingCode), true);
    setuid(ROOT_UID);
#else
    ASSERT_EQ(verifier->IsGameServiceCalling(callingCode), true);
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
    auto verifier = std::make_unique<RSIClientToServiceConnectionInterfaceCodeAccessVerifier>();
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
    auto verifier = std::make_unique<RSIClientToServiceConnectionInterfaceCodeAccessVerifier>();
    CodeUnderlyingType code = 0;
    ASSERT_EQ(verifier->IsAccessTimesVerificationPassed(code, 0), true);
}

/**
 * @tc.name: IsTaskManagerCallingTest
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueICK4SM
 */
HWTEST_F(RSIRenderServiceConnectionIpcInterfaceCodeAccessVerifierTest, IsTaskManagerCallingTest,
    testing::ext::TestSize.Level1)
{
    auto verifier = std::make_unique<RSIClientToServiceConnectionInterfaceCodeAccessVerifier>();
    const std::string callingCode = "test";
    ASSERT_EQ(verifier->IsTaskManagerCalling(callingCode), false);
}

/**
 * @tc.name: TaskSurfaceCaptureWithAllWindows
 * @tc.desc: test TaskSurfaceCaptureWithAllWindows permission checking
 * @tc.type: FUNC
 * @tc.require: issueICQ74B
 */
HWTEST_F(RSIRenderServiceConnectionIpcInterfaceCodeAccessVerifierTest, TaskSurfaceCaptureWithAllWindows,
    testing::ext::TestSize.Level1)
{
    auto verifier = std::make_unique<RSIClientToRenderConnectionInterfaceCodeAccessVerifier>();
    CodeUnderlyingType code = static_cast<CodeUnderlyingType>(
        RSIClientToRenderConnectionInterfaceCode::TAKE_SURFACE_CAPTURE_WITH_ALL_WINDOWS);
    auto hasPermission = verifier->IsInterfaceCodeAccessible(code);
    ASSERT_EQ(hasPermission, false);
}

/**
 * @tc.name: TakeSurfaceCapture
 * @tc.desc: test TAKE_SURFACE_CAPTURE permission checking is enforced via CAPTURE_SCREEN
 * @tc.type: FUNC
 * @tc.require: issue#25929
 */
HWTEST_F(RSIRenderServiceConnectionIpcInterfaceCodeAccessVerifierTest, TakeSurfaceCapture,
    testing::ext::TestSize.Level1)
{
    auto verifier = std::make_unique<RSIClientToRenderConnectionInterfaceCodeAccessVerifier>();
    CodeUnderlyingType code = static_cast<CodeUnderlyingType>(
        RSIClientToRenderConnectionInterfaceCode::TAKE_SURFACE_CAPTURE);
    auto hasPermission = verifier->IsInterfaceCodeAccessible(code);
    ASSERT_EQ(hasPermission, false);
}

/**
 * @tc.name: FreezeScreen
 * @tc.desc: test FreezeScreen permission checking
 * @tc.type: FUNC
 * @tc.require: issueICS2J8
 */
HWTEST_F(RSIRenderServiceConnectionIpcInterfaceCodeAccessVerifierTest, FreezeScreen,
    testing::ext::TestSize.Level1)
{
    auto verifier = std::make_unique<RSIClientToRenderConnectionInterfaceCodeAccessVerifier>();
    CodeUnderlyingType code = static_cast<CodeUnderlyingType>(RSIClientToRenderConnectionInterfaceCode::FREEZE_SCREEN);
    auto hasPermission = verifier->IsInterfaceCodeAccessible(code);
    ASSERT_EQ(hasPermission, true);
}

/**
 * @tc.name: SetLogicalCameraRotationCorrection
 * @tc.desc: test SetLogicalCameraRotationCorrection permission checking
 * @tc.type: FUNC
 * @tc.require: issueICS2J8
 */
HWTEST_F(RSIRenderServiceConnectionIpcInterfaceCodeAccessVerifierTest, SetLogicalCameraRotationCorrection,
    testing::ext::TestSize.Level1)
{
    auto verifier = std::make_unique<RSIClientToRenderConnectionInterfaceCodeAccessVerifier>();
    CodeUnderlyingType code = static_cast<CodeUnderlyingType>(
        RSIClientToRenderConnectionInterfaceCode::SET_LOGICAL_CAMERA_ROTATION_CORRECTION);
    auto hasPermission = verifier->IsInterfaceCodeAccessible(code);
    ASSERT_EQ(hasPermission, true);
}

/**
 * @tc.name: SetGlobalBlackListAccessCheck
 * @tc.desc: test SET_GLOBAL_BLACKLIST permission checking gated by system calling
 * @tc.type: FUNC
 * @tc.require: issue26140
 */
HWTEST_F(RSIRenderServiceConnectionIpcInterfaceCodeAccessVerifierTest, SetGlobalBlackListAccessCheck,
    testing::ext::TestSize.Level2)
{
    auto verifier = std::make_unique<RSIClientToRenderConnectionInterfaceCodeAccessVerifier>();
    CodeUnderlyingType code = static_cast<CodeUnderlyingType>(
        RSIClientToRenderConnectionInterfaceCode::SET_GLOBAL_BLACKLIST);
    auto hasPermission = verifier->IsInterfaceCodeAccessible(code);
    ASSERT_EQ(hasPermission, true);
}

/**
 * @tc.name: AddGlobalBlackListAccessCheck
 * @tc.desc: test ADD_GLOBAL_BLACKLIST permission checking gated by system calling
 * @tc.type: FUNC
 * @tc.require: issue26140
 */
HWTEST_F(RSIRenderServiceConnectionIpcInterfaceCodeAccessVerifierTest, AddGlobalBlackListAccessCheck,
    testing::ext::TestSize.Level2)
{
    auto verifier = std::make_unique<RSIClientToRenderConnectionInterfaceCodeAccessVerifier>();
    CodeUnderlyingType code = static_cast<CodeUnderlyingType>(
        RSIClientToRenderConnectionInterfaceCode::ADD_GLOBAL_BLACKLIST);
    auto hasPermission = verifier->IsInterfaceCodeAccessible(code);
    ASSERT_EQ(hasPermission, true);
}

/**
 * @tc.name: RemoveGlobalBlackListAccessCheck
 * @tc.desc: test REMOVE_GLOBAL_BLACKLIST permission checking gated by system calling
 * @tc.type: FUNC
 * @tc.require: issue26140
 */
HWTEST_F(RSIRenderServiceConnectionIpcInterfaceCodeAccessVerifierTest, RemoveGlobalBlackListAccessCheck,
    testing::ext::TestSize.Level2)
{
    auto verifier = std::make_unique<RSIClientToRenderConnectionInterfaceCodeAccessVerifier>();
    CodeUnderlyingType code = static_cast<CodeUnderlyingType>(
        RSIClientToRenderConnectionInterfaceCode::REMOVE_GLOBAL_BLACKLIST);
    auto hasPermission = verifier->IsInterfaceCodeAccessible(code);
    ASSERT_EQ(hasPermission, true);
}

#ifdef RS_PROFILER_ENABLED
/**
 * @tc.name: IsFeatureVerificationPassedProfilerEnabledTest001
 * @tc.desc: test IsFeatureVerificationPassed with PROFILER codes when HRP service is enabled
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSIRenderServiceConnectionIpcInterfaceCodeAccessVerifierTest,
    IsFeatureVerificationPassedProfilerEnabledTest001, testing::ext::TestSize.Level1)
{
    auto verifier = std::make_unique<RSIClientToServiceConnectionInterfaceCodeAccessVerifier>();
    RSProfiler::SetHrpServiceEnabled(true);
    CodeUnderlyingType code = static_cast<CodeUnderlyingType>(
        RSIClientToServiceConnectionInterfaceCode::PROFILER_SERVICE_OPEN_FILE);
    ASSERT_EQ(verifier->IsFeatureVerificationPassed(code), true);
    code = static_cast<CodeUnderlyingType>(
        RSIClientToServiceConnectionInterfaceCode::PROFILER_SERVICE_POPULATE_FILES);
    ASSERT_EQ(verifier->IsFeatureVerificationPassed(code), true);
    code = static_cast<CodeUnderlyingType>(
        RSIClientToServiceConnectionInterfaceCode::PROFILER_IS_SECURE_SCREEN);
    ASSERT_EQ(verifier->IsFeatureVerificationPassed(code), true);
    RSProfiler::SetHrpServiceEnabled(false);
}

/**
 * @tc.name: IsFeatureVerificationPassedProfilerDisabledTest001
 * @tc.desc: test IsFeatureVerificationPassed with PROFILER codes when HRP service is disabled
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSIRenderServiceConnectionIpcInterfaceCodeAccessVerifierTest,
    IsFeatureVerificationPassedProfilerDisabledTest001, testing::ext::TestSize.Level1)
{
    auto verifier = std::make_unique<RSIClientToServiceConnectionInterfaceCodeAccessVerifier>();
    RSProfiler::SetHrpServiceEnabled(false);
    CodeUnderlyingType code = static_cast<CodeUnderlyingType>(
        RSIClientToServiceConnectionInterfaceCode::PROFILER_SERVICE_OPEN_FILE);
    ASSERT_EQ(verifier->IsFeatureVerificationPassed(code), false);
    code = static_cast<CodeUnderlyingType>(
        RSIClientToServiceConnectionInterfaceCode::PROFILER_SERVICE_POPULATE_FILES);
    ASSERT_EQ(verifier->IsFeatureVerificationPassed(code), false);
    code = static_cast<CodeUnderlyingType>(
        RSIClientToServiceConnectionInterfaceCode::PROFILER_IS_SECURE_SCREEN);
    ASSERT_EQ(verifier->IsFeatureVerificationPassed(code), false);
    RSProfiler::SetHrpServiceEnabled(true);
}

/**
 * @tc.name: IsInterfaceCodeAccessibleProfilerDisabledTest001
 * @tc.desc: test IsInterfaceCodeAccessible returns false when PROFILER code and HRP service disabled
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSIRenderServiceConnectionIpcInterfaceCodeAccessVerifierTest,
    IsInterfaceCodeAccessibleProfilerDisabledTest001, testing::ext::TestSize.Level1)
{
    auto verifier = std::make_unique<RSIClientToServiceConnectionInterfaceCodeAccessVerifier>();
    RSProfiler::SetHrpServiceEnabled(false);
    CodeUnderlyingType code = static_cast<CodeUnderlyingType>(
        RSIClientToServiceConnectionInterfaceCode::PROFILER_SERVICE_OPEN_FILE);
    ASSERT_EQ(verifier->IsInterfaceCodeAccessible(code), false);
    code = static_cast<CodeUnderlyingType>(
        RSIClientToServiceConnectionInterfaceCode::PROFILER_SERVICE_POPULATE_FILES);
    ASSERT_EQ(verifier->IsInterfaceCodeAccessible(code), false);
    code = static_cast<CodeUnderlyingType>(
        RSIClientToServiceConnectionInterfaceCode::PROFILER_IS_SECURE_SCREEN);
    ASSERT_EQ(verifier->IsInterfaceCodeAccessible(code), false);
    RSProfiler::SetHrpServiceEnabled(true);
}

/**
 * @tc.name: IsInterfaceCodeAccessibleProfilerEnabledTest001
 * @tc.desc: test IsInterfaceCodeAccessible returns false for PROFILER codes even when HRP service
 *           enabled, because PROFILER codes have no case in IsExclusiveVerificationPassed and
 *           fall to default:false when ENABLE_IPC_SECURITY is defined
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSIRenderServiceConnectionIpcInterfaceCodeAccessVerifierTest,
    IsInterfaceCodeAccessibleProfilerEnabledTest001, testing::ext::TestSize.Level1)
{
    auto verifier = std::make_unique<RSIClientToServiceConnectionInterfaceCodeAccessVerifier>();
    RSProfiler::SetHrpServiceEnabled(true);
    CodeUnderlyingType code = static_cast<CodeUnderlyingType>(
        RSIClientToServiceConnectionInterfaceCode::PROFILER_SERVICE_OPEN_FILE);
#ifdef ENABLE_IPC_SECURITY
    // PROFILER codes pass IsFeatureVerificationPassed but have no case in
    // IsExclusiveVerificationPassed, so IsInterfaceCodeAccessible returns false
    ASSERT_EQ(verifier->IsInterfaceCodeAccessible(code), false);
    code = static_cast<CodeUnderlyingType>(
        RSIClientToServiceConnectionInterfaceCode::PROFILER_SERVICE_POPULATE_FILES);
    ASSERT_EQ(verifier->IsInterfaceCodeAccessible(code), false);
    code = static_cast<CodeUnderlyingType>(
        RSIClientToServiceConnectionInterfaceCode::PROFILER_IS_SECURE_SCREEN);
    ASSERT_EQ(verifier->IsInterfaceCodeAccessible(code), false);
#else
    // Without ENABLE_IPC_SECURITY, only IsFeatureVerificationPassed is checked,
    // so PROFILER codes are accessible when HRP service is enabled
    ASSERT_EQ(verifier->IsInterfaceCodeAccessible(code), true);
    code = static_cast<CodeUnderlyingType>(
        RSIClientToServiceConnectionInterfaceCode::PROFILER_SERVICE_POPULATE_FILES);
    ASSERT_EQ(verifier->IsInterfaceCodeAccessible(code), true);
    code = static_cast<CodeUnderlyingType>(
        RSIClientToServiceConnectionInterfaceCode::PROFILER_IS_SECURE_SCREEN);
    ASSERT_EQ(verifier->IsInterfaceCodeAccessible(code), true);
#endif
    RSProfiler::SetHrpServiceEnabled(false);
}
#endif

/**
 * @tc.name: IsFeatureVerificationPassedDefaultBranchTest001
 * @tc.desc: Verify IsFeatureVerificationPassed returns true for non-PROFILER codes (default branch)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSIRenderServiceConnectionIpcInterfaceCodeAccessVerifierTest,
    IsFeatureVerificationPassedDefaultBranchTest001, testing::ext::TestSize.Level1)
{
    auto verifier = std::make_unique<RSIClientToServiceConnectionInterfaceCodeAccessVerifier>();
    CodeUnderlyingType code = static_cast<CodeUnderlyingType>(
        RSIClientToServiceConnectionInterfaceCode::GET_UNI_RENDER_ENABLED);
    ASSERT_EQ(verifier->IsFeatureVerificationPassed(code), true);
}

/**
 * @tc.name: IsFeatureVerificationPassedProfilerCodeTest001
 * @tc.desc: Verify IsFeatureVerificationPassed for PROFILER codes (covers PROFILER case branch).
 *           When RS_PROFILER_ENABLED is not defined, RS_PROFILER_HRP_SERVICE_ENABLED() expands
 *           to false, so PROFILER codes should return false. When defined, HRP service state
 *           determines the result.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSIRenderServiceConnectionIpcInterfaceCodeAccessVerifierTest,
    IsFeatureVerificationPassedProfilerCodeTest001, testing::ext::TestSize.Level1)
{
    auto verifier = std::make_unique<RSIClientToServiceConnectionInterfaceCodeAccessVerifier>();
    CodeUnderlyingType code = static_cast<CodeUnderlyingType>(
        RSIClientToServiceConnectionInterfaceCode::PROFILER_SERVICE_OPEN_FILE);
#ifdef RS_PROFILER_ENABLED
    RSProfiler::SetHrpServiceEnabled(false);
    ASSERT_EQ(verifier->IsFeatureVerificationPassed(code), false);
    RSProfiler::SetHrpServiceEnabled(true);
    ASSERT_EQ(verifier->IsFeatureVerificationPassed(code), true);
    RSProfiler::SetHrpServiceEnabled(false);
#else
    // When RS_PROFILER_ENABLED is not defined, RS_PROFILER_HRP_SERVICE_ENABLED() expands to false
    ASSERT_EQ(verifier->IsFeatureVerificationPassed(code), false);
#endif
}

/**
 * @tc.name: IsFeatureVerificationPassedProfilerCodeTest002
 * @tc.desc: Verify IsFeatureVerificationPassed for PROFILER_SERVICE_POPULATE_FILES code
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSIRenderServiceConnectionIpcInterfaceCodeAccessVerifierTest,
    IsFeatureVerificationPassedProfilerCodeTest002, testing::ext::TestSize.Level1)
{
    auto verifier = std::make_unique<RSIClientToServiceConnectionInterfaceCodeAccessVerifier>();
    CodeUnderlyingType code = static_cast<CodeUnderlyingType>(
        RSIClientToServiceConnectionInterfaceCode::PROFILER_SERVICE_POPULATE_FILES);
#ifdef RS_PROFILER_ENABLED
    RSProfiler::SetHrpServiceEnabled(false);
    ASSERT_EQ(verifier->IsFeatureVerificationPassed(code), false);
    RSProfiler::SetHrpServiceEnabled(true);
    ASSERT_EQ(verifier->IsFeatureVerificationPassed(code), true);
    RSProfiler::SetHrpServiceEnabled(false);
#else
    ASSERT_EQ(verifier->IsFeatureVerificationPassed(code), false);
#endif
}

/**
 * @tc.name: IsFeatureVerificationPassedProfilerCodeTest003
 * @tc.desc: Verify IsFeatureVerificationPassed for PROFILER_IS_SECURE_SCREEN code
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSIRenderServiceConnectionIpcInterfaceCodeAccessVerifierTest,
    IsFeatureVerificationPassedProfilerCodeTest003, testing::ext::TestSize.Level1)
{
    auto verifier = std::make_unique<RSIClientToServiceConnectionInterfaceCodeAccessVerifier>();
    CodeUnderlyingType code = static_cast<CodeUnderlyingType>(
        RSIClientToServiceConnectionInterfaceCode::PROFILER_IS_SECURE_SCREEN);
#ifdef RS_PROFILER_ENABLED
    RSProfiler::SetHrpServiceEnabled(false);
    ASSERT_EQ(verifier->IsFeatureVerificationPassed(code), false);
    RSProfiler::SetHrpServiceEnabled(true);
    ASSERT_EQ(verifier->IsFeatureVerificationPassed(code), true);
    RSProfiler::SetHrpServiceEnabled(false);
#else
    ASSERT_EQ(verifier->IsFeatureVerificationPassed(code), false);
#endif
}

} // namespace OHOS::Rosen