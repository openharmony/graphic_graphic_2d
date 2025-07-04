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
#include "transaction/rs_render_service_client.h"
#include "platform/ohos/rs_render_service_connect_hub.h"
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
static constexpr uint32_t SET_OPERATION_SLEEP_US = 50000;  // wait for set-operation change
static constexpr uint64_t TEST_ID = 123;
class RSClientTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline std::shared_ptr<RSRenderServiceClient> rsClient = nullptr;
};

void RSClientTest::SetUpTestCase()
{
    rsClient = std::make_shared<RSRenderServiceClient>();
    uint64_t tokenId;
    const char* perms[1];
    perms[0] = "ohos.permission.CAPTURE_SCREEN";
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
void RSClientTest::TearDownTestCase() {}
void RSClientTest::SetUp() {}
void RSClientTest::TearDown()
{
    usleep(SET_REFRESHRATE_SLEEP_US);
}

/**
 * @tc.name: CreateNode Test
 * @tc.desc: CreateNode Test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSClientTest, CreateNode_Test, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    RSSurfaceRenderNodeConfig config;
    bool ret = rsClient->CreateNode(config);
    ASSERT_EQ(ret, true);
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
HWTEST_F(RSClientTest, TakeSurfaceCapture_Test, TestSize.Level1)
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
HWTEST_F(RSClientTest, TakeSurfaceCapture_Nullptr, TestSize.Level1)
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
HWTEST_F(RSClientTest, TakeSurfaceCapture01, TestSize.Level1)
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
HWTEST_F(RSClientTest, TakeUICaptureInRangeTest, TestSize.Level1)
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
HWTEST_F(RSClientTest, SetHwcNodeBounds_Test, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    bool ret = rsClient->SetHwcNodeBounds(TEST_ID, 1.0f, 1.0f,
        1.0f, 1.0f);
    ASSERT_EQ(ret, true);
}

/**
 * @tc.name: RegisterBufferAvailableListener Test a notfound id True
 * @tc.desc: RegisterBufferAvailableListener Test a notfound id True
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSClientTest, RegisterBufferAvailableListener_True, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    BufferAvailableCallback cb = [](){};
    rsClient->RegisterBufferAvailableListener(TEST_ID, cb, true); // test a notfound number: 123
}

/**
 * @tc.name: RegisterBufferAvailableListener Test a notfound id False
 * @tc.desc: RegisterBufferAvailableListener Test a notfound id False
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSClientTest, RegisterBufferAvailableListener_False, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    BufferAvailableCallback cb = [](){};
    rsClient->RegisterBufferAvailableListener(TEST_ID, cb, false); // test a notfound number: 123
}

/**
 * @tc.name: RegisterBufferAvailableListener Test nullptr
 * @tc.desc: RegisterBufferAvailableListener Test nullptr
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSClientTest, RegisterBufferAvailableListener_Nullptr, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    rsClient->RegisterBufferAvailableListener(TEST_ID, nullptr, false); // NodeId: 123
}

/**
 * @tc.name: UnregisterBufferAvailableListener Test a notfound id
 * @tc.desc: UnregisterBufferAvailableListener Test a notfound id
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSClientTest, UnregisterBufferAvailableListener_False, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    BufferAvailableCallback cb = [](){};
    bool ret = rsClient->UnregisterBufferAvailableListener(TEST_ID); // test a notfound number: 123
    ASSERT_EQ(ret, true);
}

/**
 * @tc.name: RegisterTransactionDataCallback01
 * @tc.desc: RegisterTransactionDataCallback Test callback is null
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSClientTest, RegisterTransactionDataCallback01, TestSize.Level1)
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
HWTEST_F(RSClientTest, RegisterTransactionDataCallback02, TestSize.Level1)
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
HWTEST_F(RSClientTest, RegisterTransactionDataCallback04, TestSize.Level1)
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
HWTEST_F(RSClientTest, TriggerTransactionDataCallbackAndErase01, TestSize.Level1)
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
HWTEST_F(RSClientTest, TriggerTransactionDataCallbackAndErase02, TestSize.Level1)
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
 * @tc.name: RegisterApplicationAgent Test nullptr
 * @tc.desc: RegisterApplicationAgent Test nullptr
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSClientTest, RegisterApplicationAgent_Nullptr, TestSize.Level1)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    ASSERT_NE(renderService, nullptr);
    renderService->RegisterApplicationAgent(TEST_ID, nullptr); // pid: 123
}

/**
 * @tc.name: CreateVirtualScreen Test
 * @tc.desc: CreateVirtualScreen Test
 * @tc.type:FUNC
 * @tc.require: issueI8FSLX
 */
HWTEST_F(RSClientTest, CreateVirtualScreen, TestSize.Level2)
{
    ASSERT_NE(rsClient, nullptr);
    std::vector<NodeId> whiteList = {1};
    ScreenId virtualScreenId = rsClient->CreateVirtualScreen(
        "virtual0", 320, 180, nullptr, INVALID_SCREEN_ID, -1, whiteList);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);
}

/**
 * @tc.name: SetVirtualScreenUsingStatus Test
 * @tc.desc: Test SetVirtualScreenUsingStatus while input is true
 * @tc.type:FUNC
 * @tc.require: issueI9ABGS
 */
HWTEST_F(RSClientTest, SetVirtualScreenUsingStatus001, TestSize.Level2)
{
    ASSERT_NE(rsClient, nullptr);
    rsClient->SetVirtualScreenUsingStatus(true);
}

/**
 * @tc.name: SetVirtualScreenUsingStatus Test
 * @tc.desc: Test SetVirtualScreenUsingStatus while input is false
 * @tc.type:FUNC
 * @tc.require: issueI9ABGS
 */
HWTEST_F(RSClientTest, SetVirtualScreenUsingStatus002, TestSize.Level2)
{
    ASSERT_NE(rsClient, nullptr);
    rsClient->SetVirtualScreenUsingStatus(false);
}

/**
 * @tc.name: SetCurtainScreenUsingStatus Test
 * @tc.desc: Test SetCurtainScreenUsingStatus while input is true
 * @tc.type:FUNC
 * @tc.require: issueI9ABGS
 */
HWTEST_F(RSClientTest, SetCurtainScreenUsingStatus001, TestSize.Level2)
{
    ASSERT_NE(rsClient, nullptr);
    rsClient->SetCurtainScreenUsingStatus(true);
}

/**
 * @tc.name: SetCurtainScreenUsingStatus Test
 * @tc.desc: Test SetCurtainScreenUsingStatus while input is false
 * @tc.type:FUNC
 * @tc.require: issueI9ABGS
 */
HWTEST_F(RSClientTest, SetCurtainScreenUsingStatus002, TestSize.Level2)
{
    ASSERT_NE(rsClient, nullptr);
    rsClient->SetCurtainScreenUsingStatus(false);
}

/**
 * @tc.name: ExecuteSynchronousTask Test
 * @tc.desc: Test ExecuteSynchronousTask
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
 */
HWTEST_F(RSClientTest, ExecuteSynchronousTask001, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    rsClient->ExecuteSynchronousTask(nullptr);

    auto task = std::make_shared<RSNodeGetShowingPropertyAndCancelAnimation>(0, nullptr);
    ASSERT_NE(task, nullptr);
    rsClient->ExecuteSynchronousTask(task);
}

/**
 * @tc.name: GetUniRenderEnabled Test
 * @tc.desc: GetUniRenderEnabled Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
 */
HWTEST_F(RSClientTest, GetUniRenderEnabled001, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    ASSERT_EQ(rsClient->GetUniRenderEnabled(), false);
}

/**
 * @tc.name: GetMemoryGraphic Test
 * @tc.desc: GetMemoryGraphic Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
 */
HWTEST_F(RSClientTest, GetMemoryGraphic001, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    int pid = 100; // for test
    rsClient->GetMemoryGraphic(pid);
    auto memGraphics = rsClient->GetMemoryGraphics();
    ASSERT_EQ(memGraphics.size(), 0);
}

/**
 * @tc.name: GetTotalAppMemSize Test
 * @tc.desc: GetTotalAppMemSize Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
 */
HWTEST_F(RSClientTest, GetTotalAppMemSize001, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    float cpuMemSize = 0;
    float gpuMemSize = 0;
    bool ret = rsClient->GetTotalAppMemSize(cpuMemSize, gpuMemSize);
    ASSERT_EQ(ret, true);
}

/**
 * @tc.name: CreateNodeAndSurface Test
 * @tc.desc: CreateNodeAndSurface Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
 */
HWTEST_F(RSClientTest, CreateNodeAndSurface001, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    RSSurfaceRenderNodeConfig config = {.id=0, .name="testSurface"};
    ASSERT_EQ(rsClient->CreateNodeAndSurface(config), nullptr);
}

/**
 * @tc.name: CreatePixelMapFromSurfaceId Test
 * @tc.desc: CreatePixelMapFromSurfaceId Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
 */
HWTEST_F(RSClientTest, CreatePixelMapFromSurfaceId001, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);
    SurfaceUtils::GetInstance()->Add(psurface->GetUniqueId(), psurface);
    Rect srcRect = {0, 0, 100, 100};
    rsClient->CreatePixelMapFromSurfaceId(psurface->GetUniqueId(), srcRect);
}

/**
 * @tc.name: SetFocusAppInfo Test
 * @tc.desc: SetFocusAppInfo Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
 */
HWTEST_F(RSClientTest, SetFocusAppInfo001, TestSize.Level1)
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
 * @tc.name: GetActiveScreenId Test
 * @tc.desc: GetActiveScreenId Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
 */
HWTEST_F(RSClientTest, GetActiveScreenId001, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    ASSERT_EQ(rsClient->GetActiveScreenId(), INVALID_SCREEN_ID);
}

/**
 * @tc.name: GetAllScreenIds Test
 * @tc.desc: GetAllScreenIds Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
 */
HWTEST_F(RSClientTest, GetAllScreenIds001, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    auto csurface = IConsumerSurface::Create();
    EXPECT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    EXPECT_NE(psurface, nullptr);
    uint32_t defaultWidth = 480;
    uint32_t defaultHeight = 320;
    ScreenId virtualScreenId = rsClient->CreateVirtualScreen(
        "virtualScreenTest", defaultWidth, defaultHeight, psurface, INVALID_SCREEN_ID, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);
    auto screenIds = rsClient->GetAllScreenIds();
    ASSERT_NE(screenIds.size(), 0);
}

/**
 * @tc.name: SetVirtualScreenSurface Test
 * @tc.desc: SetVirtualScreenSurface Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
 */
HWTEST_F(RSClientTest, SetVirtualScreenSurface001, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);
    int32_t ret = rsClient->SetVirtualScreenSurface(TEST_ID, psurface); // 123 for test
    ASSERT_EQ(ret, 0);
    rsClient->RemoveVirtualScreen(TEST_ID);
}

/**
 * @tc.name: SetVirtualScreenBlackList Test
 * @tc.desc: SetVirtualScreenBlackList Test
 * @tc.type:FUNC
 * @tc.require: issues#IC98BX
 */
HWTEST_F(RSClientTest, SetVirtualScreenBlackListTest, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    ScreenId screenId = 100;
    std::vector<NodeId> blackListVector({1, 2, 3});
    int32_t ret = rsClient->SetVirtualScreenBlackList(screenId, blackListVector);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: AddVirtualScreenBlackList Test
 * @tc.desc: AddVirtualScreenBlackList Test
 * @tc.type:FUNC
 * @tc.require: issues#IC98BX
 */
HWTEST_F(RSClientTest, AddVirtualScreenBlackListTest, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    ScreenId screenId = 100;
    std::vector<NodeId> blackListVector({1, 2, 3});
    int32_t ret = rsClient->AddVirtualScreenBlackList(screenId, blackListVector);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: RemoveVirtualScreenBlackList Test
 * @tc.desc: RemoveVirtualScreenBlackList Test
 * @tc.type:FUNC
 * @tc.require: issues#IC98BX
 */
HWTEST_F(RSClientTest, RemoveVirtualScreenBlackListTest, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    ScreenId screenId = 100;
    std::vector<NodeId> blackListVector({1, 2, 3});
    int32_t ret = rsClient->RemoveVirtualScreenBlackList(screenId, blackListVector);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: ResizeVirtualScreen Test
 * @tc.desc: ResizeVirtualScreen Test
 * @tc.type:FUNC
 * @tc.require: issues#IC98BX
 */
HWTEST_F(RSClientTest, ResizeVirtualScreenTest, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    ScreenId screenId = 100;
    uint32_t width = 500;
    uint32_t height = 500;
    int32_t ret = rsClient->ResizeVirtualScreen(screenId, width, height);
    ASSERT_EQ(ret, SCREEN_NOT_FOUND);
}

/**
 * @tc.name: SetScreenChangeCallback Test
 * @tc.desc: SetScreenChangeCallback Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
 */
HWTEST_F(RSClientTest, SetScreenChangeCallback001, TestSize.Level1)
{
    ScreenId screenId = INVALID_SCREEN_ID;
    ScreenEvent screenEvent = ScreenEvent::UNKNOWN;
    ScreenChangeReason errorReason = ScreenChangeReason::DEFAULT;
    bool callbacked = false;
    auto callback = [&screenId, &screenEvent, &errorReason, &callbacked]
        (ScreenId id, ScreenEvent event, ScreenChangeReason reason) {
        screenId = id;
        screenEvent = event;
        errorReason = reason;
        callbacked = true;
    };
    int32_t status = rsClient->SetScreenChangeCallback(callback);
    EXPECT_EQ(status, StatusCode::SUCCESS);
}

#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
/**
 * @tc.name: SetPointerColorInversionConfig Test
 * @tc.desc: SetPointerColorInversionConfig Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
 */
HWTEST_F(RSClientTest, SetPointerColorInversionConfig001, TestSize.Level1)
{
    float darkBuffer = 0.5f;
    float brightBuffer = 0.5f;
    int64_t interval = 50;
    int32_t rangeSize = 20;
    EXPECT_EQ(rsClient->SetPointerColorInversionConfig(darkBuffer, brightBuffer, interval, rangeSize),
        StatusCode::SUCCESS);
}

/**
 * @tc.name: SetPointerColorInversionEnabled Test
 * @tc.desc: SetPointerColorInversionEnabled Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
 */
HWTEST_F(RSClientTest, SetPointerColorInversionEnabled001, TestSize.Level1)
{
    EXPECT_EQ(rsClient->SetPointerColorInversionEnabled(false), StatusCode::SUCCESS);
}

/**
 * @tc.name: RegisterPointerLuminanceChangeCallback Test
 * @tc.desc: RegisterPointerLuminanceChangeCallback Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
 */
HWTEST_F(RSClientTest, RegisterPointerLuminanceChangeCallback001, TestSize.Level1)
{
    EXPECT_EQ(rsClient->RegisterPointerLuminanceChangeCallback([](int32_t brightness) -> void {}),
        StatusCode::SUCCESS);
}

/**
 * @tc.name: UnRegisterPointerLuminanceChangeCallback Test
 * @tc.desc: UnRegisterPointerLuminanceChangeCallback Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
 */
HWTEST_F(RSClientTest, UnRegisterPointerLuminanceChangeCallback001, TestSize.Level1)
{
    EXPECT_EQ(rsClient->UnRegisterPointerLuminanceChangeCallback(), StatusCode::SUCCESS);
}
#endif

/**
 * @tc.name: RegisterFirstFrameCommitCallback Test
 * @tc.desc: RegisterFirstFrameCommitCallback Test
 * @tc.type:FUNC
 * @tc.require: issuesIBTF2E
 */
HWTEST_F(RSClientTest, RegisterFirstFrameCommitCallback001, TestSize.Level1)
{
    EXPECT_EQ(rsClient->RegisterFirstFrameCommitCallback([](uint64_t screenId, int64_t timestamp) -> void {}),
        StatusCode::SUCCESS);
    EXPECT_EQ(rsClient->RegisterFirstFrameCommitCallback(nullptr), StatusCode::SUCCESS);
}

/**
 * @tc.name: SetScreenActiveMode Test
 * @tc.desc: SetScreenActiveMode Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
 */
HWTEST_F(RSClientTest, SetScreenActiveMode001, TestSize.Level1)
{
    auto screenId = rsClient->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    rsClient->SetScreenActiveMode(screenId, 0);
    auto modeInfo = rsClient->GetScreenActiveMode(screenId);
    EXPECT_EQ(modeInfo.GetScreenModeId(), 0);
}

/**
 * @tc.name: SetScreenActiveRect Test
 * @tc.desc: SetScreenActiveRect Test
 * @tc.type:FUNC
 * @tc.require: issueIB3986
 */
HWTEST_F(RSClientTest, SetScreenActiveRect001, Function | SmallTest | Level2)
{
    auto screenId = rsClient->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);

    Rect activeRect {
        .x = 0,
        .y = 0,
        .w = 0,
        .h = 0,
    };
    EXPECT_EQ(rsClient->SetScreenActiveRect(screenId, activeRect), StatusCode::SUCCESS);
}

/**
 * @tc.name: SetScreenRefreshRate Test
 * @tc.desc: SetScreenRefreshRate Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
 */
HWTEST_F(RSClientTest, SetScreenRefreshRate001, TestSize.Level1)
{
    auto screenId = rsClient->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    FrameRateLinkerId id = 0;
    FrameRateRange range;
    rsClient->SyncFrameRateRange(id, range, 0);
    uint32_t rateToSet = 990; // 990 for test
    rsClient->SetScreenRefreshRate(screenId, 0, rateToSet);
    usleep(SET_REFRESHRATE_SLEEP_US);
    uint32_t currentRate = rsClient->GetScreenCurrentRefreshRate(screenId);
    EXPECT_NE(currentRate, rateToSet);
}

/**
 * @tc.name: UnregisterFrameRateLinker Test
 * @tc.desc: UnregisterFrameRateLinker Test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSClientTest, UnregisterFrameRateLinker001, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    FrameRateLinkerId id = 0;
    rsClient->UnregisterFrameRateLinker(id);
}

/**
 * @tc.name: SetRefreshRateMode Test
 * @tc.desc: SetRefreshRateMode Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
 */
HWTEST_F(RSClientTest, SetRefreshRateMode001, TestSize.Level1)
{
    uint32_t rateMode = 100;
    rsClient->SetRefreshRateMode(rateMode);
    usleep(SET_REFRESHRATE_SLEEP_US);
    uint32_t currentRateMode = rsClient->GetCurrentRefreshRateMode();
    EXPECT_NE(currentRateMode, rateMode);
}

/**
 * @tc.name: GetScreenSupportedRefreshRates Test
 * @tc.desc: GetScreenSupportedRefreshRates Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
 */
HWTEST_F(RSClientTest, GetScreenSupportedRefreshRates001, TestSize.Level1)
{
    auto screenId = rsClient->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    auto supportedRates = rsClient->GetScreenSupportedRefreshRates(screenId);
    EXPECT_EQ(supportedRates.size(), 0);
}

/**
 * @tc.name: SetShowRefreshRateEnabled Test
 * @tc.desc: SetShowRefreshRateEnabled Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
 */
HWTEST_F(RSClientTest, SetShowRefreshRateEnabled001, TestSize.Level1)
{
    rsClient->SetShowRefreshRateEnabled(false, 0);
    EXPECT_EQ(rsClient->GetShowRefreshRateEnabled(), false);

    rsClient->SetShowRefreshRateEnabled(true, 0);
    EXPECT_EQ(rsClient->GetShowRefreshRateEnabled(), false);
}

/**
 * @tc.name: GetRealtimeRefreshRate001 Test
 * @tc.desc: GetRealtimeRefreshRate001 Test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSClientTest, GetRealtimeRefreshRate001, TestSize.Level1)
{
    EXPECT_GE(rsClient->GetRealtimeRefreshRate(INVALID_SCREEN_ID), 0);
}

/**
 * @tc.name: GetRefreshInfo Test
 * @tc.desc: GetRefreshInfo Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
 */
HWTEST_F(RSClientTest, GetRefreshInfo001, TestSize.Level1)
{
    EXPECT_EQ(rsClient->GetRefreshInfo(-1), "");
}

/**
 * @tc.name: GetRefreshInfoToSP Test
 * @tc.desc: GetRefreshInfoToSP Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
 */
HWTEST_F(RSClientTest, GetRefreshInfoToSP001, TestSize.Level1)
{
    EXPECT_EQ(rsClient->GetRefreshInfoToSP(-1), "");
}

/*
 * @tc.name: SetPhysicalScreenResolution Test
 * @tc.desc: SetPhysicalScreenResolution Test
 * @tc.type: FUNC
 */
HWTEST_F(RSClientTest, SetPhysicalScreenResolution001, TestSize.Level1)
{
    ScreenId id = INVALID_SCREEN_ID;
    uint32_t newWidth = 1920;
    uint32_t newHeight = 1080;
    auto ret = rsClient->SetPhysicalScreenResolution(id, newWidth, newHeight);
    EXPECT_EQ(ret, StatusCode::RS_CONNECTION_ERROR);
}

/**
 * @tc.name: SetVirtualScreenResolution Test
 * @tc.desc: SetVirtualScreenResolution Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
 */
HWTEST_F(RSClientTest, SetVirtualScreenResolution001, TestSize.Level1)
{
    auto csurface = IConsumerSurface::Create();
    EXPECT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    EXPECT_NE(psurface, nullptr);
    uint32_t defaultWidth = 720;
    uint32_t defaultHeight = 1280;
    ScreenId virtualScreenId = rsClient->CreateVirtualScreen(
        "virtualScreenTest", defaultWidth, defaultHeight, psurface, INVALID_SCREEN_ID, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);

    uint32_t newWidth = 1920;
    uint32_t newHeight = 1080;
    rsClient->SetVirtualScreenResolution(virtualScreenId, newWidth, newHeight);
    auto curVirtualScreenResolution = rsClient->GetVirtualScreenResolution(virtualScreenId);
    EXPECT_EQ(curVirtualScreenResolution.GetVirtualScreenWidth(), newWidth);
    EXPECT_EQ(curVirtualScreenResolution.GetVirtualScreenHeight(), newHeight);
}

/**
 * @tc.name: SetScreenPowerStatus Test
 * @tc.desc: SetScreenPowerStatus Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
 */
HWTEST_F(RSClientTest, SetScreenPowerStatus001, TestSize.Level1)
{
    auto screenId = rsClient->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    rsClient->SetScreenPowerStatus(screenId, ScreenPowerStatus::POWER_STATUS_ON);
    usleep(SET_REFRESHRATE_SLEEP_US);
    auto powerStatus = rsClient->GetScreenPowerStatus(screenId);
    EXPECT_EQ(powerStatus, ScreenPowerStatus::POWER_STATUS_ON);
}

/**
 * @tc.name: GetScreenSupportedModes Test
 * @tc.desc: GetScreenSupportedModes Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
 */
HWTEST_F(RSClientTest, GetScreenSupportedModes001, TestSize.Level1)
{
    auto screenId = rsClient->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    auto supportedScreenModes = rsClient->GetScreenSupportedModes(screenId);
    EXPECT_GT(supportedScreenModes.size(), 0);
}

/**
 * @tc.name: GetScreenCapability Test
 * @tc.desc: GetScreenCapability Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
 */
HWTEST_F(RSClientTest, GetScreenCapability001, TestSize.Level1)
{
    auto screenId = rsClient->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    auto screenCapability = rsClient->GetScreenCapability(screenId);
    EXPECT_NE(screenCapability.GetType(), DISP_INVALID);
}

/**
 * @tc.name: GetScreenData Test
 * @tc.desc: GetScreenData Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
 */
HWTEST_F(RSClientTest, GetScreenData001, TestSize.Level1)
{
    auto screenId = rsClient->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    auto screenData = rsClient->GetScreenData(screenId);
    EXPECT_GT(screenData.GetSupportModeInfo().size(), 0);
    EXPECT_NE(screenData.GetCapability().GetType(), DISP_INVALID);
}

/**
 * @tc.name: GetScreenBacklight Test
 * @tc.desc: GetScreenBacklight Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
 */
HWTEST_F(RSClientTest, GetScreenBacklight001, TestSize.Level1)
{
    auto screenId = rsClient->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    rsClient->SetScreenBacklight(screenId, 60); // for test
    usleep(SET_REFRESHRATE_SLEEP_US);
    auto backLight = rsClient->GetScreenBacklight(screenId);
    EXPECT_EQ(backLight, 60); // for test
}

/**
 * @tc.name: RegisterBufferClearListener Test
 * @tc.desc: RegisterBufferClearListener Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
 */
HWTEST_F(RSClientTest, RegisterBufferClearListener001, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    BufferClearCallback cb = [](){};
    bool ret = rsClient->RegisterBufferClearListener(TEST_ID, cb);
    ASSERT_TRUE(ret);
}

/*
 * @tc.name: GetScreenSupportedColorGamuts Test
 * @tc.desc: GetScreenSupportedColorGamuts Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
*/
HWTEST_F(RSClientTest, GetScreenSupportedColorGamuts001, TestSize.Level1)
{
    std::vector<ScreenColorGamut> modes;
    int ret = rsClient->GetScreenSupportedColorGamuts(INVALID_SCREEN_ID, modes);
    EXPECT_EQ(ret, StatusCode::SCREEN_NOT_FOUND);
}

/*
 * @tc.name: GetScreenSupportedMetaDataKeys Test
 * @tc.desc: GetScreenSupportedMetaDataKeys Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
*/
HWTEST_F(RSClientTest, GetScreenSupportedMetaDataKeys001, TestSize.Level1)
{
    auto screenId = rsClient->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    std::vector<ScreenHDRMetadataKey> keys = {ScreenHDRMetadataKey::MATAKEY_RED_PRIMARY_X};
    int ret = rsClient->GetScreenSupportedMetaDataKeys(screenId, keys);
    EXPECT_EQ(ret, StatusCode::SUCCESS);
    EXPECT_EQ(keys[0], ScreenHDRMetadataKey::MATAKEY_RED_PRIMARY_X);
}

/*
 * @tc.name: GetScreenColorGamut Test
 * @tc.desc: GetScreenColorGamut Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
*/
HWTEST_F(RSClientTest, GetScreenColorGamut001, TestSize.Level1)
{
    auto csurface = IConsumerSurface::Create();
    EXPECT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    EXPECT_NE(psurface, nullptr);
    uint32_t defaultWidth = 720;
    uint32_t defaultHeight = 1280;
    ScreenId virtualScreenId = rsClient->CreateVirtualScreen(
        "virtualScreenTest", defaultWidth, defaultHeight, psurface, INVALID_SCREEN_ID, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);

    int32_t modeIdx = 0;
    int ret = rsClient->SetScreenColorGamut(virtualScreenId, modeIdx);
    EXPECT_EQ(ret, 0);
    ScreenColorGamut mode = COLOR_GAMUT_NATIVE;
    ret = rsClient->GetScreenColorGamut(virtualScreenId, mode);
    EXPECT_EQ(ret, 0);
}

/*
 * @tc.name: SetScreenGamutMap Test
 * @tc.desc: SetScreenGamutMap Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
*/
HWTEST_F(RSClientTest, SetScreenGamutMap001, TestSize.Level1)
{
    ScreenId screenId = rsClient->GetDefaultScreenId();
    ScreenGamutMap mode = ScreenGamutMap::GAMUT_MAP_CONSTANT;
    uint32_t ret = rsClient->SetScreenGamutMap(screenId, mode);
    ASSERT_EQ(ret, StatusCode::HDI_ERROR);
    ret = rsClient->GetScreenGamutMap(screenId, mode);
    ASSERT_EQ(ret, StatusCode::HDI_ERROR);
}

/*
 * @tc.name: SetScreenCorrection Test
 * @tc.desc: SetScreenCorrection Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
*/
HWTEST_F(RSClientTest, SetScreenCorrection001, TestSize.Level1)
{
    ScreenId screenId = rsClient->GetDefaultScreenId();
    uint32_t ret = rsClient->SetScreenCorrection(screenId, ScreenRotation::ROTATION_90);
    ASSERT_EQ(ret, SUCCESS);
}

/*
 * @tc.name: SetVirtualMirrorScreenCanvasRotation Test
 * @tc.desc: SetVirtualMirrorScreenCanvasRotation Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
*/
HWTEST_F(RSClientTest, SetVirtualMirrorScreenCanvasRotation001, TestSize.Level1)
{
    auto csurface = IConsumerSurface::Create();
    EXPECT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    uint32_t defaultWidth = 1344;
    uint32_t defaultHeight = 2772;
    EXPECT_NE(psurface, nullptr);
    ScreenId virtualScreenId = rsClient->CreateVirtualScreen(
        "virtualScreenTest", defaultWidth, defaultHeight, psurface, INVALID_SCREEN_ID, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);
    EXPECT_EQ(rsClient->SetVirtualMirrorScreenCanvasRotation(virtualScreenId, true), true);
    EXPECT_EQ(rsClient->SetVirtualMirrorScreenCanvasRotation(virtualScreenId, false), true);
}

/**
 * @tc.name: SetVirtualScreenAutoRotationTest
 * @tc.desc: SetVirtualScreenAutoRotation Test
 * @tc.type:FUNC
 * @tc.require: issueICGA54
 */
HWTEST_F(RSClientTest, SetVirtualScreenAutoRotationTest, TestSize.Level1)
{
    auto csurface = IConsumerSurface::Create();
    EXPECT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    uint32_t defaultWidth = 1344;
    uint32_t defaultHeight = 2772;
    EXPECT_NE(psurface, nullptr);
    ScreenId virtualScreenId = rsClient->CreateVirtualScreen(
        "virtualScreenTest", defaultWidth, defaultHeight, psurface, INVALID_SCREEN_ID, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);
    EXPECT_EQ(rsClient->SetVirtualScreenAutoRotation(virtualScreenId, true), StatusCode::SUCCESS);
    EXPECT_EQ(rsClient->SetVirtualScreenAutoRotation(virtualScreenId, false), StatusCode::SUCCESS);

    RSRenderServiceConnectHub::Destroy();
    EXPECT_EQ(rsClient->SetVirtualScreenAutoRotation(virtualScreenId, true), StatusCode::RENDER_SERVICE_NULL);
    RSRenderServiceConnectHub::Init();
    EXPECT_EQ(rsClient->SetVirtualScreenAutoRotation(virtualScreenId, true), StatusCode::SUCCESS);
}

/*
 * @tc.name: SetVirtualMirrorScreenScaleMode Test
 * @tc.desc: SetVirtualMirrorScreenScaleMode Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
*/
HWTEST_F(RSClientTest, SetVirtualMirrorScreenScaleMode001, TestSize.Level1)
{
    auto csurface = IConsumerSurface::Create();
    EXPECT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    uint32_t defaultWidth = 1344;
    uint32_t defaultHeight = 2772;
    EXPECT_NE(psurface, nullptr);
    ScreenId virtualScreenId = rsClient->CreateVirtualScreen(
        "virtualScreenTest", defaultWidth, defaultHeight, psurface, INVALID_SCREEN_ID, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);
    EXPECT_EQ(rsClient->SetVirtualMirrorScreenScaleMode(virtualScreenId, ScreenScaleMode::UNISCALE_MODE), true);
}

/*
 * @tc.name: SetGlobalDarkColorMode Test
 * @tc.desc: SetGlobalDarkColorMode Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
*/
HWTEST_F(RSClientTest, SetGlobalDarkColorMode001, TestSize.Level1)
{
    EXPECT_EQ(rsClient->SetGlobalDarkColorMode(true), true);
}

/**
 * @tc.name: RegisterUIExtensionCallback Test
 * @tc.desc: RegisterUIExtensionCallback, expected success when callback non-empty.
 * @tc.type:FUNC
 * @tc.require: issueIABHAX
 */
HWTEST_F(RSClientTest, RegisterUIExtensionCallback_001, TestSize.Level1)
{
    UIExtensionCallback callback = [](std::shared_ptr<RSUIExtensionData>, uint64_t) {};
    uint64_t userId = 0;
    EXPECT_EQ(rsClient->RegisterUIExtensionCallback(userId, callback),
        StatusCode::SUCCESS);
}

/**
 * @tc.name: RegisterUIExtensionCallback Test
 * @tc.desc: RegisterUIExtensionCallback, expected success when callback is empty.
 * @tc.type:FUNC
 * @tc.require: issueIABHAX
 */
HWTEST_F(RSClientTest, RegisterUIExtensionCallback_002, TestSize.Level1)
{
    UIExtensionCallback callback = nullptr;
    uint64_t userId = 0;
    EXPECT_NE(rsClient->RegisterUIExtensionCallback(userId, callback),
        StatusCode::INVALID_ARGUMENTS);
}

/**
 * @tc.name: SetFreeMultiWindowStatus Test
 * @tc.desc: SetFreeMultiWindowStatus, input true
 * @tc.type:FUNC
 * @tc.require: issueIANPC2
 */
HWTEST_F(RSClientTest, SetFreeMultiWindowStatus, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    rsClient->SetFreeMultiWindowStatus(true);
}

/**
 * @tc.name: SetLayerTop001 Test
 * @tc.desc: SetLayerTop001, input true
 * @tc.type:FUNC
 * @tc.require: issueIAOZFC
 */
HWTEST_F(RSClientTest, SetLayerTop001, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    const std::string nodeIdStr = "123456";
    rsClient->SetLayerTop(nodeIdStr, true);
    rsClient->SetLayerTop(nodeIdStr, false);
}

/**
 * @tc.name: SetForceRefresh001 Test
 * @tc.desc: SetForceRefresh001, input true
 * @tc.type:FUNC
 * @tc.require: issueIAOZFC
 */
HWTEST_F(RSClientTest, SetForceRefresh001, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    const std::string nodeIdStr = "123456";
    rsClient->SetForceRefresh(nodeIdStr, true);
    rsClient->SetForceRefresh(nodeIdStr, false);
}

/**
 * @tc.name: SetWindowContainer Test
 * @tc.desc: SetWindowContainer, input true
 * @tc.type:FUNC
 * @tc.require: issueIBIK1X
 */
HWTEST_F(RSClientTest, SetWindowContainer001, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    NodeId nodeId = {};
    rsClient->SetWindowContainer(nodeId, true);
    rsClient->SetWindowContainer(nodeId, false);
}

/**
 * @tc.name: GetPixelMapByProcessIdTest
 * @tc.desc: Test GetPixelMapByProcessId
 * @tc.type: FUNC
 * @tc.require: issueIBJFIK
 */
HWTEST_F(RSClientTest, GetPixelMapByProcessIdTest, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    pid_t pid = 0;
    std::vector<PixelMapInfo> pixelMapInfoVector;
    int32_t res = rsClient->GetPixelMapByProcessId(pixelMapInfoVector, pid);
    ASSERT_EQ(res, SUCCESS);
}

/**
 * @tc.name: SetBehindWindowFilterEnabledTest
 * @tc.desc: SetBehindWindowFilterEnabledTest
 * @tc.type:FUNC
 * @tc.require: issuesIC5OEB
 */
HWTEST_F(RSClientTest, SetBehindWindowFilterEnabledTest, TestSize.Level1)
{
    auto res = rsClient->SetBehindWindowFilterEnabled(true);
    usleep(SET_OPERATION_SLEEP_US);
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: GetBehindWindowFilterEnabledTest
 * @tc.desc: GetBehindWindowFilterEnabledTest
 * @tc.type:FUNC
 * @tc.require: issuesIC5OEB
 */
HWTEST_F(RSClientTest, GetBehindWindowFilterEnabledTest, TestSize.Level1)
{
    auto enabled = false;
    auto res = rsClient->GetBehindWindowFilterEnabled(enabled);
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: GetPidGpuMemoryInMBTest
 * @tc.desc: GetPidGpuMemoryInMBTest
 * @tc.type:FUNC
 * @tc.require: issuesICE0QR
 */
HWTEST_F(RSClientTest, GetPidGpuMemoryInMBTest, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    int32_t pid = 1001;
    float gpuMemInMB = 0.0f;
    auto res = rsClient->GetPidGpuMemoryInMB(pid, gpuMemInMB);
    EXPECT_EQ(res, ERR_INVALID_DATA);
}

/**
* @tc.name: ProfilerIsSecureScreenTest
* @tc.desc: ProfilerIsSecureScreenTest
* @tc.type: FUNC
* @tc.require: issuesIC98WU
*/
HWTEST_F(RSClientTest, ProfilerIsSecureScreenTest, TestSize.Level1)
{
    auto res = rsClient->ProfilerIsSecureScreen();
    usleep(SET_OPERATION_SLEEP_US);
    ASSERT_NE(rsClient, nullptr);
    ASSERT_EQ(res, false);
}

/**
 * @tc.name: ProfilerServiceOpenFileTest
 * @tc.desc: ProfilerServiceOpenFileTest
 * @tc.type: FUNC
 * @tc.require: issuesIC98WU
 */
HWTEST_F(RSClientTest, ProfilerServiceOpenFileTest, TestSize.Level1)
{
    int32_t fd = 0;
    HrpServiceDirInfo dirInfo{HrpServiceDir::HRP_SERVICE_DIR_COMMON, "subdir", "subdir2"};

    auto res = rsClient->ProfilerServiceOpenFile(dirInfo, "filename", 0, fd);
    ASSERT_NE(rsClient, nullptr);
    ASSERT_EQ(res, RET_HRP_SERVICE_ERR_UNSUPPORTED);
}

/**
 * @tc.name: ProfilerServicePopulateFilesTest
 * @tc.desc: ProfilerServicePopulateFilesTest
 * @tc.type: FUNC
 * @tc.require: issuesIC98WU
 */
HWTEST_F(RSClientTest, ProfilerServicePopulateFilesTest, TestSize.Level1)
{
    std::vector<HrpServiceFileInfo> outFiles;
    HrpServiceDirInfo dirInfo{HrpServiceDir::HRP_SERVICE_DIR_COMMON, "subdir", "subdir2"};

    auto res = rsClient->ProfilerServicePopulateFiles(dirInfo, 0, outFiles);
    ASSERT_NE(rsClient, nullptr);
    ASSERT_EQ(res, RET_HRP_SERVICE_ERR_UNSUPPORTED);
}
} // namespace Rosen
} // namespace OHOS