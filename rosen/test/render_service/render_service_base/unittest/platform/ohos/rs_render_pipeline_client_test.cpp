/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>

#include "feature/capture/rs_ui_capture.h"
#include "transaction/rs_render_pipeline_client.h"
#include "platform/ohos/rs_render_service_connect_hub.h"
#include "platform/ohos/rs_client_to_render_connection_proxy.h"
#include "ui/rs_surface_node.h"
#include "surface_utils.h"
#include <iostream>

#include "accesstoken_kit.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
static constexpr uint32_t SET_REFRESHRATE_SLEEP_US = 50000;  // wait for refreshrate change
static constexpr uint64_t TEST_ID = 123;
class RSPipelineClientTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline std::shared_ptr<RSRenderPipelineClient> rsClient = nullptr;
};

void RSPipelineClientTest::SetUpTestCase()
{
    rsClient = std::make_shared<RSRenderPipelineClient>();
    uint64_t tokenId;
    const char* perms[2];
    perms[0] = "ohos.permission.CAPTURE_SCREEN";
    perms[1] = "ohos.permission.CAPTURE_SCREEN_ALL";
    NativeTokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = 1,
        .aclsNum = 0,
        .dcaps = NULL,
        .perms = perms,
        .acls = NULL,
        .processName = "foundation",
        .aplStr = "system_basic",
    };
    tokenId = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(tokenId);
    OHOS::Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
}
void RSPipelineClientTest::TearDownTestCase() {}
void RSPipelineClientTest::SetUp() {}
void RSPipelineClientTest::TearDown()
{
    usleep(SET_REFRESHRATE_SLEEP_US);
}

class TestSurfaceCaptureCallback : public SurfaceCaptureCallback {
public:
    explicit TestSurfaceCaptureCallback() {}
    ~TestSurfaceCaptureCallback() override {}
    void OnSurfaceCapture(std::shared_ptr<Media::PixelMap> pixelmap) override
    {
    }
    void OnSurfaceCaptureHDR(std::shared_ptr<Media::PixelMap> pixelMap,
        std::shared_ptr<Media::PixelMap> pixelMapHDR) override {}
};

/**
 * @tc.name: TakeSurfaceCapture_Test
 * @tc.desc: Test capture twice with same id
 * @tc.type:FUNC
 * @tc.require: I6X9V1
 */
HWTEST_F(RSPipelineClientTest, TakeSurfaceCapture_Test, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    std::shared_ptr<TestSurfaceCaptureCallback> cb = std::make_shared<TestSurfaceCaptureCallback>();
    RSSurfaceCaptureConfig captureConfig;
    bool ret = rsClient->TakeSurfaceCapture(TEST_ID, cb, captureConfig); // test a notfound number: 123
    ASSERT_EQ(ret, true);
    captureConfig.useDma = true;
    ret = rsClient->TakeSurfaceCapture(TEST_ID, cb, captureConfig); // test number: 123 twice
    ASSERT_EQ(ret, true);
}

/**
 * @tc.name: TakeSurfaceCapture Test nullptr
 * @tc.desc: TakeSurfaceCapture Test nullptr
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPipelineClientTest, TakeSurfaceCapture_Nullptr, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    RSSurfaceCaptureConfig captureConfig;
    bool ret = rsClient->TakeSurfaceCapture(TEST_ID, nullptr, captureConfig); // NodeId: 123
    ASSERT_NE(ret, true);
}

/**
 * @tc.name: TakeSurfaceCapture Test nullptr
 * @tc.desc: TakeSurfaceCapture Test nullptr
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPipelineClientTest, TakeSurfaceCapture01, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.isSync = true;
    bool ret = rsClient->TakeSurfaceCapture(TEST_ID, nullptr, captureConfig);
    ASSERT_NE(ret, true);
}

/**
 * @tc.name: TakeUICaptureInRangeTest
 * @tc.desc: TakeUICaptureInRangeTest
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPipelineClientTest, TakeUICaptureInRangeTest, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    RSSurfaceCaptureConfig captureConfig;
    bool ret = rsClient->TakeUICaptureInRange(TEST_ID, nullptr, captureConfig);
    ASSERT_NE(ret, true);
    std::shared_ptr<TestSurfaceCaptureCallback> cb = std::make_shared<TestSurfaceCaptureCallback>();
    ret = rsClient->TakeUICaptureInRange(TEST_ID, cb, captureConfig);
    ASSERT_EQ(ret, true);
}

/**
 * @tc.name: SetHwcNodeBounds_Test
 * @tc.desc: Test Set HwcNode Bounds
 * @tc.type:FUNC
 * @tc.require: IB2O0L
 */
HWTEST_F(RSPipelineClientTest, SetHwcNodeBounds_Test, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    bool ret = rsClient->SetHwcNodeBounds(TEST_ID, 1.0f, 1.0f,
        1.0f, 1.0f);
    ASSERT_EQ(ret, true);
}

/**
 * @tc.name: RegisterTransactionDataCallback01
 * @tc.desc: RegisterTransactionDataCallback Test callback is null
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPipelineClientTest, RegisterTransactionDataCallback01, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    bool ret = rsClient->RegisterTransactionDataCallback(1, 789, nullptr);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: RegisterTransactionDataCallback02
 * @tc.desc: RegisterTransactionDataCallback Test callback does not exist
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPipelineClientTest, RegisterTransactionDataCallback02, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    std::function<void()> callback = []() {};
    bool ret = rsClient->RegisterTransactionDataCallback(1, 789, callback);
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: RegisterTransactionDataCallback04
 * @tc.desc: RegisterTransactionDataCallback Test callback already exists
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPipelineClientTest, RegisterTransactionDataCallback04, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    std::function<void()> callback = []() {};
    uint64_t token = 123;
    uint64_t timeStamp = 456;
    rsClient->transactionDataCallbacks_[std::make_pair(token, timeStamp)] = []() {};
    bool ret = rsClient->RegisterTransactionDataCallback(token, timeStamp, callback);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: TriggerTransactionDataCallbackAndErase01
 * @tc.desc: TriggerTransactionDataCallbackAndErase Test callback exist
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPipelineClientTest, TriggerTransactionDataCallbackAndErase01, TestSize.Level1)
{
    uint64_t token = 123;
    uint64_t timeStamp = 456;
    bool callbackInvoked = false;
    rsClient->transactionDataCallbacks_[std::make_pair(token, timeStamp)] = [&callbackInvoked]() {
        callbackInvoked = true;
    };
    rsClient->TriggerTransactionDataCallbackAndErase(token, timeStamp);
    EXPECT_TRUE(callbackInvoked);
    EXPECT_EQ(rsClient->transactionDataCallbacks_.find(std::make_pair(token, timeStamp)),
        rsClient->transactionDataCallbacks_.end());
}

/**
 * @tc.name: TriggerTransactionDataCallbackAndErase02
 * @tc.desc: TriggerTransactionDataCallbackAndErase Test callback does not exist
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPipelineClientTest, TriggerTransactionDataCallbackAndErase02, TestSize.Level1)
{
    uint64_t token = 123;
    uint64_t timeStamp = 456;
    bool callbackInvoked = false;
    rsClient->transactionDataCallbacks_[std::make_pair(token, timeStamp)] = [&callbackInvoked]() {
        callbackInvoked = true;
    };
    rsClient->transactionDataCallbacks_.clear();

    rsClient->TriggerTransactionDataCallbackAndErase(token, timeStamp);
    EXPECT_FALSE(callbackInvoked);
    EXPECT_TRUE(rsClient->transactionDataCallbacks_.empty());
}

/**
 * @tc.name: SetFocusAppInfo Test
 * @tc.desc: SetFocusAppInfo Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
 */
HWTEST_F(RSPipelineClientTest, SetFocusAppInfo001, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    FocusAppInfo info = {
        .pid = 1,
        .uid = 1,
        .bundleName = "bundleNameTest",
        .abilityName = "abilityNameTest",
        .focusNodeId = 1};
    auto ret = rsClient->SetFocusAppInfo(info);
    ASSERT_EQ(ret, SUCCESS);
}

/**
 * @tc.name: SetWindowContainer Test
 * @tc.desc: SetWindowContainer, input true
 * @tc.type:FUNC
 * @tc.require: issueIBIK1X
 */
HWTEST_F(RSPipelineClientTest, SetWindowContainer001, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    NodeId nodeId = {};
    rsClient->SetWindowContainer(nodeId, true);
    rsClient->SetWindowContainer(nodeId, false);
}

/**
 * @tc.name: ClearUifirstCache Test
 * @tc.desc: ClearUifirstCache
 * @tc.type:FUNC
 * @tc.require: issueICK4SM
 */
HWTEST_F(RSPipelineClientTest, ClearUifirstCacheTest, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    NodeId nodeId = 1;
    rsClient->ClearUifirstCache(nodeId);
}

/**
 * @tc.name: TaskSurfaceCaptureWithAllWindows Test
 * @tc.desc: TaskSurfaceCaptureWithAllWindows when screen frozen
 * @tc.type:FUNC
 * @tc.require: issueICQ74B
 */
HWTEST_F(RSPipelineClientTest, TakeSurfaceCaptureWithAllWindowsTest, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    bool checkDrmAndSurfaceLock = false;
    std::shared_ptr<TestSurfaceCaptureCallback> cb;
    RSSurfaceCaptureConfig captureConfig;
    bool ret = rsClient->TakeSurfaceCaptureWithAllWindows(TEST_ID, cb, captureConfig, checkDrmAndSurfaceLock);
    ASSERT_EQ(ret, false);

    cb = std::make_shared<TestSurfaceCaptureCallback>();
    std::vector<std::shared_ptr<SurfaceCaptureCallback>> callbackVector;
    rsClient->surfaceCaptureCbMap_.emplace(std::make_pair(TEST_ID, captureConfig), callbackVector);
    ret = rsClient->TakeSurfaceCaptureWithAllWindows(TEST_ID, cb, captureConfig, checkDrmAndSurfaceLock);
    ASSERT_EQ(ret, true);

    rsClient->surfaceCaptureCbDirector_ = nullptr;
    rsClient->surfaceCaptureCbMap_.clear();
    ret = rsClient->TakeSurfaceCaptureWithAllWindows(TEST_ID, cb, captureConfig, checkDrmAndSurfaceLock);
    ASSERT_EQ(ret, true);

    ret = rsClient->TakeSurfaceCaptureWithAllWindows(TEST_ID, cb, captureConfig, checkDrmAndSurfaceLock);
    ASSERT_EQ(ret, true);
}

/**
 * @tc.name: TaskSurfaceCaptureWithAllWindows Test
 * @tc.desc: TaskSurfaceCaptureWithAllWindows for failures
 * @tc.type:FUNC
 * @tc.require: issueICS2J8
 */
HWTEST_F(RSPipelineClientTest, TakeSurfaceCaptureWithAllWindowsTest002, TestSize.Level1)
{
    class MockClientToRenderConnection : public RSClientToRenderConnectionProxy {
    public:
        explicit MockClientToRenderConnection(const sptr<IRemoteObject>& impl) : RSClientToRenderConnectionProxy(impl) {};
        ErrCode TakeSurfaceCaptureWithAllWindows(NodeId id, sptr<RSISurfaceCaptureCallback> callback,
            const RSSurfaceCaptureConfig& captureConfig, bool checkDrmAndSurfaceLock,
            RSSurfaceCapturePermissions permissions) override
        {
            return ERR_PERMISSION_DENIED;
        }
    };
    ASSERT_NE(rsClient, nullptr);
    auto renderServiceConnectHub = RSRenderServiceConnectHub::GetInstance();
    RSRenderServiceConnectHub::instance_ = nullptr;
    std::shared_ptr<TestSurfaceCaptureCallback> cb;
    RSSurfaceCaptureConfig captureConfig;
    bool ret = rsClient->TakeSurfaceCaptureWithAllWindows(TEST_ID, cb, captureConfig, false);
    ASSERT_EQ(ret, false);

    RSRenderServiceConnectHub::instance_ = renderServiceConnectHub;
    ASSERT_NE(RSRenderServiceConnectHub::GetInstance(), nullptr);
    ret = rsClient->TakeSurfaceCaptureWithAllWindows(TEST_ID, cb, captureConfig, false);
    ASSERT_EQ(ret, false);

    cb = std::make_shared<TestSurfaceCaptureCallback>();
    auto renderConn = RSRenderServiceConnectHub::GetInstance()->renderConn_;
    RSRenderServiceConnectHub::instance_->renderConn_ = new MockClientToRenderConnection(nullptr);
    ret = rsClient->TakeSurfaceCaptureWithAllWindows(TEST_ID, cb, captureConfig, false);
    ASSERT_EQ(ret, false);
    RSRenderServiceConnectHub::instance_->renderConn_ = renderConn;
}

/**
 * @tc.name: FreezeScreen Test
 * @tc.desc: FreezeScreen to freeze or unfreeze screen
 * @tc.type:FUNC
 * @tc.require: issueICS2J8
 */
HWTEST_F(RSPipelineClientTest, FreezeScreen, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    auto renderServiceConnectHub = RSRenderServiceConnectHub::GetInstance();
    RSRenderServiceConnectHub::instance_ = nullptr;
    bool ret = rsClient->FreezeScreen(TEST_ID, false);
    ASSERT_EQ(ret, false);

    RSRenderServiceConnectHub::instance_ = renderServiceConnectHub;
    ret = rsClient->FreezeScreen(TEST_ID, false);
    ASSERT_EQ(ret, true);
}

} // namespace Rosen
} // namespace OHOS