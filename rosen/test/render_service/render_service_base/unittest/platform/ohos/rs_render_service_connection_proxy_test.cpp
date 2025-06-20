/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <if_system_ability_manager.h>
#include <iremote_stub.h>
#include <iservice_registry.h>
#include <mutex>
#include <system_ability_definition.h>
#include <unistd.h>

#include "feature/capture/rs_ui_capture.h"
#include "platform/ohos/rs_render_service_connection_proxy.h"
#include "platform/ohos/rs_render_service_connect_hub.h"
#include "command/rs_animation_command.h"
#include "command/rs_node_showing_command.h"
#include "iconsumer_surface.h"
#include "pixel_map.h"
#include "feature/capture/rs_capture_pixelmap_manager.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSRenderServiceConnectionProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline std::shared_ptr<RSRenderServiceConnectionProxy> proxy;
};

void RSRenderServiceConnectionProxyTest::SetUpTestCase()
{
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_NE(samgr, nullptr);
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    proxy = std::make_shared<RSRenderServiceConnectionProxy>(remoteObject);
}
void RSRenderServiceConnectionProxyTest::TearDownTestCase() {}
void RSRenderServiceConnectionProxyTest::SetUp() {}
void RSRenderServiceConnectionProxyTest::TearDown() {}

/**
 * @tc.name: CommitTransaction Test
 * @tc.desc: CommitTransaction Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, CommitTransaction, TestSize.Level1)
{
    std::unique_ptr<RSTransactionData> transactionData;
    proxy->CommitTransaction(transactionData);
    transactionData = std::make_unique<RSTransactionData>();
    std::unique_ptr<RSCommand> command = std::make_unique<RSAnimationCallback>(1, 1, 1, FINISHED);
    NodeId nodeId = 1;
    FollowType followType = FollowType::FOLLOW_TO_PARENT;
    transactionData->AddCommand(command, nodeId, followType);
    proxy->CommitTransaction(transactionData);
    ASSERT_EQ(proxy->transactionDataIndex_, 0);
}

/**
 * @tc.name: ExecuteSynchronousTask Test
 * @tc.desc: ExecuteSynchronousTask Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, ExecuteSynchronousTask, TestSize.Level1)
{
    std::shared_ptr<RSSyncTask> task;
    proxy->ExecuteSynchronousTask(task);
    NodeId targetId;
    std::shared_ptr<RSRenderPropertyBase> property = std::make_shared<RSRenderProperty<bool>>();
    task = std::make_shared<RSNodeGetShowingPropertyAndCancelAnimation>(targetId, property);
    proxy->ExecuteSynchronousTask(task);
    ASSERT_EQ(proxy->transactionDataIndex_, 0);
}

/**
 * @tc.name: GetUniRenderEnabled Test
 * @tc.desc: GetUniRenderEnabled Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, GetUniRenderEnabled, TestSize.Level1)
{
    bool enable;
    proxy->GetUniRenderEnabled(enable);
    ASSERT_EQ(proxy->transactionDataIndex_, 0);
}

/**
 * @tc.name: FillParcelWithTransactionData Test
 * @tc.desc: FillParcelWithTransactionData Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, FillParcelWithTransactionData, TestSize.Level1)
{
    std::shared_ptr<MessageParcel> parcel = std::make_shared<MessageParcel>();
    auto transactionData = std::make_unique<RSTransactionData>();
    ASSERT_TRUE(proxy->FillParcelWithTransactionData(transactionData, parcel));
}

/**
 * @tc.name: FillParcelWithTransactionData Test
 * @tc.desc: FillParcelWithTransactionData Test
 * @tc.type:FUNC
 * @tc.require: issueICGEDM
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, FillParcelWithTransactionData002, TestSize.Level1)
{
    std::shared_ptr<MessageParcel> parcel = std::make_shared<MessageParcel>();
    parcel->SetDataSize(102401);
    auto transactionData = std::make_unique<RSTransactionData>();
    ASSERT_TRUE(proxy->FillParcelWithTransactionData(transactionData, parcel));
}

/**
 * @tc.name: CreateNodeAndSurface Test
 * @tc.desc: CreateNodeAndSurface Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, CreateNodeAndSurface, TestSize.Level1)
{
    bool success;
    ASSERT_EQ(proxy->CreateNode(RSSurfaceRenderNodeConfig(), success), ERR_INVALID_VALUE);
    ASSERT_FALSE(success);
    sptr<Surface> surface = nullptr;
    ASSERT_EQ(proxy->CreateNodeAndSurface(RSSurfaceRenderNodeConfig(), surface), ERR_INVALID_VALUE);
}

/**
 * @tc.name: CreateVSyncConnection Test
 * @tc.desc: CreateVSyncConnection Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, CreateVSyncConnection, TestSize.Level1)
{
    sptr<VSyncIConnectionToken> token;
    std::string name("name");
    uint64_t id = 1;
    NodeId windowNodeId = 1;
    sptr<IVSyncConnection> conn = nullptr;
    VSyncConnParam vsyncConnParam = {id, windowNodeId, false};
    proxy->CreateVSyncConnection(conn, name, token, vsyncConnParam);
    ASSERT_EQ(conn, nullptr);
    token = new IRemoteStub<VSyncIConnectionToken>();
    proxy->CreateVSyncConnection(conn, name, token, vsyncConnParam);
    ASSERT_EQ(conn, nullptr);
}

/**
 * @tc.name: CreatePixelMapFromSurface Test
 * @tc.desc: CreatePixelMapFromSurface Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, CreatePixelMapFromSurface, TestSize.Level1)
{
    sptr<Surface> surface;
    Rect srcRect;
    std::shared_ptr<Media::PixelMap> pixelMap = nullptr;
    proxy->CreatePixelMapFromSurface(surface, srcRect, pixelMap);
    ASSERT_EQ(pixelMap, nullptr);
    sptr<IConsumerSurface> consumer = IConsumerSurface::Create("DisplayNode");
    sptr<IBufferProducer> producer = consumer->GetProducer();
    surface = Surface::CreateSurfaceAsProducer(producer);
    proxy->CreatePixelMapFromSurface(surface, srcRect, pixelMap);
    ASSERT_EQ(pixelMap, nullptr);
}

/**
 * @tc.name: SetFocusAppInfo Test
 * @tc.desc: SetFocusAppInfo Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, SetFocusAppInfo, TestSize.Level1)
{
    int32_t pid = 1;
    int32_t uid = 1;
    std::string bundleName("bundle");
    std::string abilityName("ability");
    uint64_t focusNodeId = 1;
    FocusAppInfo info = {
        .pid = pid,
        .uid = uid,
        .bundleName = bundleName,
        .abilityName = abilityName,
        .focusNodeId = focusNodeId};
    int32_t repCode;
    proxy->SetFocusAppInfo(info, repCode);
    ASSERT_EQ(repCode, 0);
}

/**
 * @tc.name: GetAllScreenIds Test
 * @tc.desc: GetAllScreenIds Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, GetAllScreenIds, TestSize.Level1)
{
    uint64_t screenId;
    proxy->GetDefaultScreenId(screenId);
    EXPECT_EQ(screenId, INVALID_SCREEN_ID);
    proxy->GetActiveScreenId(screenId);
    EXPECT_EQ(screenId, INVALID_SCREEN_ID);
    EXPECT_EQ(proxy->GetAllScreenIds().size(), 0);
}

/**
 * @tc.name: CreateVirtualScreen Test
 * @tc.desc: CreateVirtualScreen Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, CreateVirtualScreen, TestSize.Level1)
{
    std::string name("name");
    uint32_t width = 1;
    uint32_t height = 1;
    sptr<IConsumerSurface> consumer = IConsumerSurface::Create("DisplayNode");
    sptr<IBufferProducer> producer = consumer->GetProducer();
    sptr<Surface> surface = Surface::CreateSurfaceAsProducer(producer);
    ScreenId mirrorId = 1;
    int32_t flags = 1;
    std::vector<NodeId> whiteList;
    EXPECT_EQ(proxy->CreateVirtualScreen(name, width, height, surface, mirrorId, flags, whiteList),
        INVALID_SCREEN_ID);
}

/**
 * @tc.name: SetVirtualScreenSurface Test
 * @tc.desc: SetVirtualScreenSurface Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, SetVirtualScreenSurface, TestSize.Level1)
{
    ScreenId id = 1;
    sptr<IConsumerSurface> consumer = IConsumerSurface::Create("DisplayNode");
    sptr<IBufferProducer> producer = consumer->GetProducer();
    sptr<Surface> surface = Surface::CreateSurfaceAsProducer(producer);
    EXPECT_EQ(proxy->SetVirtualScreenSurface(id, surface), 0);
}

/**
 * @tc.name: SetVirtualScreenBlackList Test
 * @tc.desc: SetVirtualScreenBlackList Test
 * @tc.type:FUNC
 * @tc.require: issue#IC98BX
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, SetVirtualScreenBlackList, TestSize.Level1)
{
    ScreenId id = 100;
    std::vector<NodeId> blackListVector({1, 2, 3});
    EXPECT_EQ(proxy->SetVirtualScreenBlackList(id, blackListVector), 0);
}

/**
 * @tc.name: AddVirtualScreenBlackList Test
 * @tc.desc: AddVirtualScreenBlackList Test
 * @tc.type:FUNC
 * @tc.require: issue#IC98BX
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, AddVirtualScreenBlackList, TestSize.Level1)
{
    ScreenId id = 100;
    std::vector<NodeId> blackListVector({1, 2, 3});
    int32_t repCode = 0;
    EXPECT_EQ(proxy->AddVirtualScreenBlackList(id, blackListVector, repCode), 0);
}

/**
 * @tc.name: RemoveVirtualScreenBlackList Test
 * @tc.desc: RemoveVirtualScreenBlackList Test
 * @tc.type:FUNC
 * @tc.require: issue#IC98BX
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, RemoveVirtualScreenBlackList, TestSize.Level1)
{
    ScreenId id = 100;
    std::vector<NodeId> blackListVector({1, 2, 3});
    int32_t repCode = 0;
    EXPECT_EQ(proxy->RemoveVirtualScreenBlackList(id, blackListVector, repCode), 0);
}

/**
 * @tc.name: RemoveVirtualScreen Test
 * @tc.desc: RemoveVirtualScreen Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, RemoveVirtualScreen, TestSize.Level1)
{
    ScreenId id = 1;
    proxy->RemoveVirtualScreen(id);
    ASSERT_TRUE(true);
}

#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
/**
 * @tc.name: SetPointerColorInversionConfig Test
 * @tc.desc: SetPointerColorInversionConfig Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, SetPointerColorInversionConfig, TestSize.Level1)
{
    float darkBuffer = 0.5f;
    float brightBuffer = 0.5f;
    int64_t interval = 50;
    int32_t rangeSize = 20;
    proxy->SetPointerColorInversionConfig(darkBuffer, brightBuffer, interval, rangeSize);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: SetPointerColorInversionEnabled Test
 * @tc.desc: SetPointerColorInversionEnabled Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, SetPointerColorInversionEnabled, TestSize.Level1)
{
    proxy->SetPointerColorInversionEnabled(false);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: RegisterPointerLuminanceChangeCallback Test
 * @tc.desc: RegisterPointerLuminanceChangeCallback Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, RegisterPointerLuminanceChangeCallback, TestSize.Level1)
{
    sptr<RSIPointerLuminanceChangeCallback> callback;
    proxy->RegisterPointerLuminanceChangeCallback(callback);
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_NE(samgr, nullptr);
    proxy->UnRegisterPointerLuminanceChangeCallback();
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    callback = iface_cast<RSIPointerLuminanceChangeCallback>(remoteObject);
    proxy->RegisterPointerLuminanceChangeCallback(callback);
    ASSERT_NE(proxy->transactionDataIndex_, 5);
}
#endif

/**
 * @tc.name: SetScreenChangeCallback Test
 * @tc.desc: SetScreenChangeCallback Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, SetScreenChangeCallback, TestSize.Level1)
{
    sptr<RSIScreenChangeCallback> callback;
    proxy->SetScreenChangeCallback(callback);
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_NE(samgr, nullptr);
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    callback = iface_cast<RSIScreenChangeCallback>(remoteObject);
    ASSERT_EQ(proxy->SetScreenChangeCallback(callback), 0);
}

/**
 * @tc.name: SetScreenActiveMode Test
 * @tc.desc: SetScreenActiveMode Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, SetScreenActiveMode, TestSize.Level1)
{
    ScreenId id = 1;
    uint32_t modeId = 1;
    proxy->SetScreenActiveMode(id, modeId);
    ASSERT_EQ(proxy->transactionDataIndex_, 0);
}

/**
 * @tc.name: SetScreenActiveRect Test
 * @tc.desc: SetScreenActiveRect Test
 * @tc.type:FUNC
 * @tc.require: issueIB3986
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, SetScreenActiveRect, TestSize.Level1)
{
    ScreenId id = 1;
    Rect activeRect {
        .x = 0,
        .y = 0,
        .w = 0,
        .h = 0,
    };
    uint32_t repCode;
    proxy->SetScreenActiveRect(id, activeRect, repCode);
    ASSERT_EQ(proxy->transactionDataIndex_, 0);
}

/**
 * @tc.name: SetScreenRefreshRate Test
 * @tc.desc: SetScreenRefreshRate Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, SetScreenRefreshRate, TestSize.Level1)
{
    ScreenId id = 1;
    int32_t sceneId = 1;
    int32_t rate = 1;
    proxy->SetScreenRefreshRate(id, sceneId, rate);
    ASSERT_EQ(proxy->transactionDataIndex_, 0);
}

/**
 * @tc.name: SetRefreshRateMode Test
 * @tc.desc: SetRefreshRateMode Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, SetRefreshRateMode, TestSize.Level1)
{
    int32_t refreshRateMode = 1;
    proxy->SetRefreshRateMode(refreshRateMode);
    ASSERT_EQ(proxy->transactionDataIndex_, 0);
}

/**
 * @tc.name: SyncFrameRateRange Test
 * @tc.desc: SyncFrameRateRange Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, SyncFrameRateRange, TestSize.Level1)
{
    FrameRateLinkerId id = 1;
    FrameRateRange range;
    proxy->SyncFrameRateRange(id, range, 0);
    range = {0, 120, 60, OHOS::Rosen::NATIVE_VSYNC_FRAME_RATE_TYPE};
    proxy->SyncFrameRateRange(id, range, 0);
    ASSERT_EQ(proxy->transactionDataIndex_, 0);
}

/**
 * @tc.name: UnregisterFrameRateLinker Test
 * @tc.desc: UnregisterFrameRateLinker Test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, UnregisterFrameRateLinker, TestSize.Level1)
{
    FrameRateLinkerId id = 1;
    proxy->UnregisterFrameRateLinker(id);
    ASSERT_NE(proxy->transactionDataIndex_, 5);
}

/**
 * @tc.name: GetCurrentRefreshRateMode Test
 * @tc.desc: GetCurrentRefreshRateMode Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, GetCurrentRefreshRateMode, TestSize.Level1)
{
    ScreenId id = 1;
    EXPECT_EQ(proxy->GetScreenCurrentRefreshRate(id), 0);
    ASSERT_EQ(proxy->GetCurrentRefreshRateMode(), 0);
}

/**
 * @tc.name: GetScreenSupportedRefreshRates Test
 * @tc.desc: GetScreenSupportedRefreshRates Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, GetScreenSupportedRefreshRates, TestSize.Level1)
{
    ScreenId id = 1;
    ASSERT_EQ(proxy->GetScreenSupportedRefreshRates(id).size(), 0);
}

/**
 * @tc.name: SetShowRefreshRateEnabled Test
 * @tc.desc: SetShowRefreshRateEnabled Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, SetShowRefreshRateEnabled, TestSize.Level1)
{
    proxy->SetShowRefreshRateEnabled(true, 0);
    ASSERT_EQ(proxy->transactionDataIndex_, 0);
}

/**
 * @tc.name: GetCurrentRefreshRateMode Test
 * @tc.desc: GetCurrentRefreshRateMode Test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, GetRealtimeRefreshRate, TestSize.Level1)
{
    EXPECT_GE(proxy->GetRealtimeRefreshRate(INVALID_SCREEN_ID), 0);
}

/**
 * @tc.name: SetPhysicalScreenResolution Test
 * @tc.desc: SetPhysicalScreenResolution Test
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, SetPhysicalScreenResolution, TestSize.Level1)
{
    ScreenId id = INVALID_SCREEN_ID;
    uint32_t newWidth = 1920;
    uint32_t newHeight = 1080;
    auto ret = proxy->SetPhysicalScreenResolution(id, newWidth, newHeight);
    EXPECT_EQ(ret, StatusCode::RS_CONNECTION_ERROR);
}

/**
 * @tc.name: SetVirtualScreenResolution Test
 * @tc.desc: SetVirtualScreenResolution Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, SetVirtualScreenResolution, TestSize.Level1)
{
    ScreenId id = 1;
    uint32_t width = 1;
    uint32_t height = 1;
    ASSERT_EQ(proxy->SetVirtualScreenResolution(id, width, height), 2);
}

/**
 * @tc.name: SetScreenPowerStatus Test
 * @tc.desc: SetScreenPowerStatus Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, SetScreenPowerStatus, TestSize.Level1)
{
    ScreenId id = 1;
    proxy->SetScreenPowerStatus(id, ScreenPowerStatus::POWER_STATUS_STANDBY);
    ASSERT_EQ(proxy->transactionDataIndex_, 0);
}

/**
 * @tc.name: RegisterApplicationAgent Test
 * @tc.desc: RegisterApplicationAgent Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, RegisterApplicationAgent, TestSize.Level1)
{
    uint32_t pid = 1;
    sptr<IApplicationAgent> app;
    proxy->RegisterApplicationAgent(pid, app);
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_NE(samgr, nullptr);
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    app = iface_cast<IApplicationAgent>(remoteObject);
    proxy->RegisterApplicationAgent(pid, app);
    ASSERT_EQ(proxy->transactionDataIndex_, 0);
}
/**
 * @tc.name: RegisterTransactionDataCallback01
 * @tc.desc: RegisterTransactionDataCallback Test normal
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, RegisterTransactionDataCallback01, TestSize.Level1)
{
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_NE(samgr, nullptr);
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    sptr<RSITransactionDataCallback> callback = iface_cast<RSITransactionDataCallback>(remoteObject);
    proxy->RegisterTransactionDataCallback(1, 456, callback);
    ASSERT_NE(proxy->transactionDataIndex_, 5);
}

/**
 * @tc.name: TakeSurfaceCapture Test
 * @tc.desc: TakeSurfaceCapture Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, TakeSurfaceCapture, TestSize.Level1)
{
    NodeId id = 1;
    sptr<RSISurfaceCaptureCallback> callback;
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = 1.0f;
    captureConfig.scaleY = 1.0f;
    captureConfig.useDma = false;
    captureConfig.captureType = SurfaceCaptureType::UICAPTURE;
    captureConfig.isSync = true;
    captureConfig.blackList = std::vector<NodeId>{0};
    ASSERT_FALSE(captureConfig.blackList.empty());
    RSSurfaceCaptureBlurParam blurParam;
    blurParam.isNeedBlur = true;
    blurParam.blurRadius = 10;
    Drawing::Rect specifiedAreaRect(0.f, 0.f, 0.f, 0.f);
    proxy->TakeSurfaceCapture(id, callback, captureConfig, blurParam, specifiedAreaRect);

    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_NE(samgr, nullptr);
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    callback = iface_cast<RSISurfaceCaptureCallback>(remoteObject);
    proxy->TakeSurfaceCapture(id, callback, captureConfig, blurParam, specifiedAreaRect);
    ASSERT_EQ(proxy->transactionDataIndex_, 0);

    // Test isUsedClientPixelMap AbnorMal conditions
    MessageParcel data;
    bool isUsedClientPixelMap = true;
    bool ret = proxy->WriteClientSurfacePixelMap(nullptr, isUsedClientPixelMap, data);
    EXPECT_EQ(ret, false);

    Drawing::Rect rect(0.f, 0.f, 0.f, 0.f);
    auto pixelMap = RSCapturePixelMapManager::CreatePixelMap(rect, captureConfig);
    ret = proxy->WriteClientSurfacePixelMap(pixelMap, isUsedClientPixelMap, data);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: TakeUICaptureInRange Test
 * @tc.desc: TakeUICaptureInRange Test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, TakeUICaptureInRange, TestSize.Level1)
{
    NodeId id = 1;
    sptr<RSISurfaceCaptureCallback> callback;
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = 1.0f;
    captureConfig.scaleY = 1.0f;
    captureConfig.useDma = false;
    captureConfig.captureType = SurfaceCaptureType::UICAPTURE;
    captureConfig.isSync = true;

    proxy->TakeUICaptureInRange(id, callback, captureConfig);

    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_NE(samgr, nullptr);
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    callback = iface_cast<RSISurfaceCaptureCallback>(remoteObject);
    proxy->TakeUICaptureInRange(id, callback, captureConfig);
    ASSERT_NE(proxy->transactionDataIndex_, 5);
}

/**
 * @tc.name: TakeSurfaceCaptureSoloNode Test
 * @tc.desc: TakeSurfaceCaptureSoloNode Test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, TakeSurfaceCaptureSoloNode, TestSize.Level1)
{
    NodeId id = 1;
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = 1.0f;
    captureConfig.scaleY = 1.0f;
    captureConfig.useDma = false;
    captureConfig.captureType = SurfaceCaptureType::UICAPTURE;
    captureConfig.isSync = false;
    proxy->TakeSurfaceCaptureSoloNode(id, captureConfig);
    EXPECT_NE(proxy->transactionDataIndex_, 5);
}

/**
 * @tc.name: SetHwcNodeBounds Test
 * @tc.desc: SetHwcNodeBounds Test
 * @tc.type:FUNC
 * @tc.require: issueIB2O0L
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, SetHwcNodeBounds, TestSize.Level1)
{
    NodeId id = 1;
    proxy->SetHwcNodeBounds(id, 1.0f, 1.0f, 1.0f, 1.0f);
    ASSERT_EQ(proxy->transactionDataIndex_, 0);
}

/**
 * @tc.name: GetVirtualScreenResolution Test
 * @tc.desc: GetVirtualScreenResolution Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, GetVirtualScreenResolution, TestSize.Level1)
{
    ScreenId id = 1;
    ASSERT_EQ(proxy->GetVirtualScreenResolution(id).width_, 0);
}

/**
 * @tc.name: GetScreenActiveMode Test
 * @tc.desc: GetScreenActiveMode Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, GetScreenActiveMode, TestSize.Level1)
{
    ScreenId id = 1;
    RSScreenModeInfo info;
    proxy->GetScreenActiveMode(id, info);
    ASSERT_EQ(info.width_, -1);
}

/**
 * @tc.name: GetMemoryGraphics Test
 * @tc.desc: GetMemoryGraphics Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, GetMemoryGraphics, TestSize.Level1)
{
    ScreenId id = 1;
    EXPECT_EQ(proxy->GetScreenSupportedModes(id).size(), 0);

    std::vector<MemoryGraphic> memoryGraphics;
    proxy->GetMemoryGraphics(memoryGraphics);
    ASSERT_EQ(memoryGraphics.size(), 0);
}

/**
 * @tc.name: GetTotalAppMemSize Test
 * @tc.desc: GetTotalAppMemSize Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, GetTotalAppMemSize, TestSize.Level1)
{
    float cpuMemSize = 1.0f;
    float gpuMemSize = 1.0f;
    ASSERT_EQ(proxy->GetTotalAppMemSize(cpuMemSize, gpuMemSize), ERR_INVALID_VALUE);
}

/**
 * @tc.name: GetScreenPowerStatus Test
 * @tc.desc: GetScreenPowerStatus Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, GetScreenPowerStatus, TestSize.Level1)
{
    int pid = 1;
    MemoryGraphic memoryGraphic;
    proxy->GetMemoryGraphic(pid, memoryGraphic);
    EXPECT_EQ(memoryGraphic.pid_, 0);
    ScreenId id = 1;
    proxy->GetScreenCapability(id);
    EXPECT_EQ(proxy->GetScreenData(id).powerStatus_, INVALID_POWER_STATUS);
    int32_t level = -1;
    proxy->GetScreenBacklight(id, level);
    EXPECT_EQ(level, -1);
    uint32_t status = ScreenPowerStatus::POWER_STATUS_ON;
    proxy->GetScreenPowerStatus(id, status);
    ASSERT_EQ(proxy->GetScreenPowerStatus(id, status), ERR_INVALID_VALUE);
}

/**
 * @tc.name: SetScreenBacklight Test
 * @tc.desc: SetScreenBacklight Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, SetScreenBacklight, TestSize.Level1)
{
    ScreenId id = 1;
    uint32_t level = 1;
    proxy->SetScreenBacklight(id, level);
    ASSERT_EQ(proxy->transactionDataIndex_, 0);
}

/**
 * @tc.name: RegisterBufferClearListener Test
 * @tc.desc: RegisterBufferClearListener Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, RegisterBufferClearListener, TestSize.Level1)
{
    NodeId id = 1;
    sptr<RSIBufferClearCallback> callback;
    proxy->RegisterBufferClearListener(id, callback);
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_NE(samgr, nullptr);
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    callback = iface_cast<RSIBufferClearCallback>(remoteObject);
    proxy->RegisterBufferClearListener(id, callback);
    ASSERT_EQ(proxy->transactionDataIndex_, 0);
}

/**
 * @tc.name: RegisterBufferAvailableListener Test
 * @tc.desc: RegisterBufferAvailableListener Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, RegisterBufferAvailableListener, TestSize.Level1)
{
    NodeId id = 1;
    sptr<RSIBufferAvailableCallback> callback;
    bool isFromRenderThread = true;
    proxy->RegisterBufferAvailableListener(id, callback, isFromRenderThread);

    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_NE(samgr, nullptr);
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    callback = iface_cast<RSIBufferAvailableCallback>(remoteObject);
    proxy->RegisterBufferAvailableListener(id, callback, isFromRenderThread);
    ASSERT_EQ(proxy->transactionDataIndex_, 0);
}

/**
 * @tc.name: GetScreenSupportedMetaDataKeys Test
 * @tc.desc: GetScreenSupportedMetaDataKeys Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, GetScreenSupportedMetaDataKeys, TestSize.Level1)
{
    ScreenId id = 1;
    std::vector<ScreenColorGamut> mode;
    ASSERT_EQ(proxy->GetScreenSupportedColorGamuts(id, mode), 2);
    std::vector<ScreenHDRMetadataKey> keys;
    ASSERT_EQ(proxy->GetScreenSupportedMetaDataKeys(id, keys), 2);
}

/**
 * @tc.name: GetScreenColorGamut Test
 * @tc.desc: GetScreenColorGamut Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, GetScreenColorGamut, TestSize.Level1)
{
    ScreenId id = 1;
    int32_t modeIdx = 7;
    proxy->SetScreenColorGamut(id, modeIdx);
    ScreenColorGamut mode = COLOR_GAMUT_BT2020;
    ASSERT_EQ(proxy->GetScreenColorGamut(id, mode), 2);
}

/**
 * @tc.name: GetScreenGamutMap Test
 * @tc.desc: GetScreenGamutMap Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, GetScreenGamutMap, TestSize.Level1)
{
    ScreenId id = 1;
    ScreenGamutMap mode = GAMUT_MAP_EXTENSION;
    proxy->SetScreenGamutMap(id, mode);
    proxy->SetScreenCorrection(id, ScreenRotation::ROTATION_90);
    ASSERT_EQ(proxy->GetScreenGamutMap(id, mode), 2);
}

/**
 * @tc.name: GetScreenHDRCapability Test
 * @tc.desc: GetScreenHDRCapability Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, GetScreenHDRCapability, TestSize.Level1)
{
    ScreenId id = 1;
    RSScreenHDRCapability screenHdrCapability;
    ASSERT_EQ(proxy->GetScreenHDRCapability(id, screenHdrCapability), 2);
}

/**
 * @tc.name: GetPixelFormat Test
 * @tc.desc: GetPixelFormat Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, GetPixelFormat, TestSize.Level1)
{
    ScreenId id = 1;
    GraphicPixelFormat pixelFormat = GRAPHIC_PIXEL_FMT_BGRA_8888;
    int32_t resCode;
    proxy->SetPixelFormat(id, pixelFormat, resCode);
    EXPECT_EQ(resCode, RS_CONNECTION_ERROR);
    proxy->GetPixelFormat(id, pixelFormat, resCode);
    ASSERT_EQ(resCode, RS_CONNECTION_ERROR);
}

/**
 * @tc.name: GetScreenHDRFormat Test
 * @tc.desc: GetScreenHDRFormat Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, GetScreenHDRFormat, TestSize.Level1)
{
    ScreenId id = 1;
    std::vector<ScreenHDRFormat> hdrFormats;
    int32_t resCode;
    proxy->GetScreenSupportedHDRFormats(id, hdrFormats, resCode);
    ASSERT_EQ(resCode, RS_CONNECTION_ERROR);
    ScreenHDRFormat hdrFormat = IMAGE_HDR_ISO_DUAL;
    proxy->GetScreenHDRFormat(id, hdrFormat, resCode);
    ASSERT_EQ(resCode, RS_CONNECTION_ERROR);
}

/**
 * @tc.name: SetScreenHDRFormat Test
 * @tc.desc: SetScreenHDRFormat Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, SetScreenHDRFormat, TestSize.Level1)
{
    ScreenId id = 1;
    int32_t modeIdx = 1;
    int32_t resCode;
    proxy->SetScreenHDRFormat(id, modeIdx, resCode);
    ASSERT_EQ(resCode, RS_CONNECTION_ERROR);
    std::vector<GraphicCM_ColorSpaceType> colorSpaces;
    proxy->GetScreenSupportedColorSpaces(id, colorSpaces, resCode);
    ASSERT_EQ(resCode, RS_CONNECTION_ERROR);
}

/**
 * @tc.name: GetScreenColorSpace Test
 * @tc.desc: GetScreenColorSpace Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, GetScreenColorSpace, TestSize.Level1)
{
    ScreenId id = 1;
    GraphicCM_ColorSpaceType colorSpace = GraphicCM_ColorSpaceType::GRAPHIC_CM_SRGB_FULL;
    int32_t resCode;
    proxy->SetScreenColorSpace(id, colorSpace, resCode);
    ASSERT_EQ(resCode, RS_CONNECTION_ERROR);
    std::vector<GraphicCM_ColorSpaceType> colorSpaces;
    proxy->GetScreenColorSpace(id, colorSpace, resCode);
    ASSERT_EQ(resCode, RS_CONNECTION_ERROR);
}

/**
 * @tc.name: GetBitmap Test
 * @tc.desc: GetBitmap Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, GetBitmap, TestSize.Level1)
{
    ScreenId id = 1;
    RSScreenType screenType = RSScreenType::VIRTUAL_TYPE_SCREEN;
    ASSERT_EQ(proxy->GetScreenType(id, screenType), 2);
    Drawing::Bitmap bitmap;
    bool success;
    ASSERT_EQ(proxy->GetBitmap(id, bitmap, success), ERR_INVALID_VALUE);
    ASSERT_FALSE(success);
}

/**
 * @tc.name: SetVirtualMirrorScreenScaleMode Test
 * @tc.desc: SetVirtualMirrorScreenScaleMode Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, SetVirtualMirrorScreenScaleMode, TestSize.Level1)
{
    ScreenId id = 1;
    int32_t resCode;
    proxy->SetScreenSkipFrameInterval(id, 1, resCode);
    EXPECT_EQ(resCode, 2);
    EXPECT_FALSE(proxy->SetVirtualMirrorScreenCanvasRotation(id, true));
    ASSERT_FALSE(proxy->SetVirtualMirrorScreenScaleMode(id, ScreenScaleMode::UNISCALE_MODE));
}

/**
 * @tc.name: SetVirtualScreenAutoRotationTest
 * @tc.desc: SetVirtualScreenAutoRotation Test
 * @tc.type:FUNC
 * @tc.require: issueICGA54
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, SetVirtualScreenAutoRotationTest, TestSize.Level1)
{
    ScreenId id = 1;
    EXPECT_NE(proxy->SetVirtualScreenAutoRotation(id, true), StatusCode::SUCCESS);
}

/**
 * @tc.name: SetGlobalDarkColorMode Test
 * @tc.desc: SetGlobalDarkColorMode Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, SetGlobalDarkColorMode, TestSize.Level1)
{
    ASSERT_TRUE(proxy->SetGlobalDarkColorMode(true) == ERR_OK);
}

/**
 * @tc.name: GetPixelmap Test
 * @tc.desc: GetPixelmap Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, GetPixelmap, TestSize.Level1)
{
    std::shared_ptr<Media::PixelMap> pixelmap = std::make_shared<Media::PixelMap>();
    Drawing::Rect rect;
    NodeId id = 1;
    std::shared_ptr<Drawing::DrawCmdList> drawCmdList = std::make_shared<Drawing::DrawCmdList>();
    bool result;
    ASSERT_EQ(proxy->GetPixelmap(id, pixelmap, &rect, drawCmdList, result), ERR_INVALID_VALUE);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: UnRegisterTypeface Test
 * @tc.desc: UnRegisterTypeface Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, UnRegisterTypeface, TestSize.Level1)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeDefault();
    EXPECT_FALSE(proxy->RegisterTypeface(1, typeface));
    ASSERT_TRUE(proxy->UnRegisterTypeface(1));
}

/**
 * @tc.name: RegisterSurfaceOcclusionChangeCallback Test
 * @tc.desc: RegisterSurfaceOcclusionChangeCallback Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, RegisterSurfaceOcclusionChangeCallback, TestSize.Level1)
{
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_NE(samgr, nullptr);
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    sptr<RSIOcclusionChangeCallback> callback = iface_cast<RSIOcclusionChangeCallback>(remoteObject);
    int32_t repCode;
    proxy->RegisterOcclusionChangeCallback(callback, repCode);
    EXPECT_NE(repCode, -1);
    NodeId id = 1;
    proxy->UnRegisterSurfaceOcclusionChangeCallback(id);
    sptr<RSISurfaceOcclusionChangeCallback> callbackTwo = iface_cast<RSISurfaceOcclusionChangeCallback>(remoteObject);
    std::vector<float> partitionPoints;
    ASSERT_EQ(proxy->RegisterSurfaceOcclusionChangeCallback(id, callbackTwo, partitionPoints), 2);
}

/**
 * @tc.name: RegisterHgmRefreshRateUpdateCallback Test
 * @tc.desc: RegisterHgmRefreshRateUpdateCallback Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, RegisterHgmRefreshRateUpdateCallback, TestSize.Level1)
{
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_NE(samgr, nullptr);
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    sptr<RSIHgmConfigChangeCallback> callback = iface_cast<RSIHgmConfigChangeCallback>(remoteObject);
    EXPECT_EQ(proxy->RegisterHgmConfigChangeCallback(callback), 2);
    EXPECT_EQ(proxy->RegisterHgmRefreshRateModeChangeCallback(callback), 2);
    ASSERT_EQ(proxy->RegisterHgmRefreshRateUpdateCallback(callback), 2);
}

/**
 * @tc.name: RegisterFirstFrameCommitCallback Test
 * @tc.desc: RegisterFirstFrameCommitCallback Test
 * @tc.type:FUNC
 * @tc.require: issuesIBTF2E
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, RegisterFirstFrameCommitCallback, TestSize.Level1)
{
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_NE(samgr, nullptr);
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    sptr<RSIFirstFrameCommitCallback> callback = iface_cast<RSIFirstFrameCommitCallback>(remoteObject);
    EXPECT_EQ(proxy->RegisterFirstFrameCommitCallback(callback), 2);
}

/**
 * @tc.name: SetSystemAnimatedScenes Test
 * @tc.desc: SetSystemAnimatedScenes Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, SetSystemAnimatedScenes, TestSize.Level1)
{
    proxy->SetAppWindowNum(1);
    bool success;
    proxy->SetSystemAnimatedScenes(SystemAnimatedScenes::ENTER_MISSION_CENTER, false, success);
    ASSERT_FALSE(success);
}

/**
 * @tc.name: ResizeVirtualScreen Test
 * @tc.desc: ResizeVirtualScreen Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, ResizeVirtualScreen, TestSize.Level1)
{
    std::shared_ptr<Media::PixelMap> watermarkImg = std::make_shared<Media::PixelMap>();
    proxy->ShowWatermark(watermarkImg, true);
    proxy->ReportJankStats();
    ScreenId id = 1;
    ASSERT_EQ(proxy->ResizeVirtualScreen(id, 1, 1), 2);
}

/**
 * @tc.name: ReportEventJankFrame Test
 * @tc.desc: ReportEventJankFrame Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, ReportEventJankFrame, TestSize.Level1)
{
    DataBaseRs info;
    proxy->ReportEventResponse(info);
    proxy->ReportEventComplete(info);
    proxy->ReportEventJankFrame(info);
    ASSERT_EQ(proxy->transactionDataIndex_, 0);
}

/**
 * @tc.name: ReportDataBaseRs Test
 * @tc.desc: ReportDataBaseRs Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, ReportDataBaseRs, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    DataBaseRs info;
    proxy->ReportDataBaseRs(data, reply, option, info);
    ASSERT_EQ(proxy->transactionDataIndex_, 0);
}

/**
 * @tc.name: NotifyLightFactorStatus Test
 * @tc.desc: NotifyLightFactorStatus Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, NotifyLightFactorStatus, TestSize.Level1)
{
    GameStateData info;
    proxy->ReportGameStateData(info);
    NodeId id = 1;
    proxy->SetHardwareEnabled(id, true, SelfDrawingNodeType::DEFAULT, true);
    proxy->NotifyLightFactorStatus(1);
    ASSERT_EQ(proxy->transactionDataIndex_, 0);
}

/**
 * @tc.name: SetCacheEnabledForRotation Test
 * @tc.desc: SetCacheEnabledForRotation Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, SetCacheEnabledForRotation, TestSize.Level1)
{
    std::vector<std::string> packageList;
    proxy->NotifyPackageEvent(1, packageList);
    std::vector<std::pair<std::string, std::string>> newConfig;
    proxy->NotifyAppStrategyConfigChangeEvent("test", 1, newConfig);
    EventInfo eventInfo;
    proxy->NotifyRefreshRateEvent(eventInfo);
    uint32_t pid = 1U;
    std::string name = "test";
    uint32_t rateDiscount = 1U;
    proxy->NotifySoftVsyncRateDiscountEvent(pid, name, rateDiscount);
    int32_t touchStatus = 1;
    int32_t touchCnt = 0;
    proxy->NotifyTouchEvent(touchStatus, touchCnt);
    proxy->NotifyDynamicModeEvent(true);
    proxy->SetCacheEnabledForRotation(true);
    ASSERT_EQ(proxy->transactionDataIndex_, 0);
}

/**
 * @tc.name: NotifyHgmConfigEvent Test
 * @tc.desc: NotifyHgmConfigEvent Test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, NotifyHgmConfigEvent, TestSize.Level1)
{
    std::string eventName = "HGMCONFIG_HIGH_TEMP";
    bool state = false;
    proxy->NotifyHgmConfigEvent(eventName, state);
    ASSERT_TRUE(proxy);
}

/**
 * @tc.name: NotifyXComponentExpectedFrameRate Test
 * @tc.desc: NotifyXComponentExpectedFrameRate Test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, NotifyXComponentExpectedFrameRate, TestSize.Level1)
{
    std::string id = "xcomponent";
    int32_t expectedFrameRate = 5;
    proxy->NotifyXComponentExpectedFrameRate(id, expectedFrameRate);
    ASSERT_TRUE(proxy);
}

/**
 * @tc.name: RunOnRemoteDiedCallback Test
 * @tc.desc: RunOnRemoteDiedCallback Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, RunOnRemoteDiedCallback, TestSize.Level1)
{
    OnRemoteDiedCallback callback = []() {};
    proxy->SetOnRemoteDiedCallback(callback);
    proxy->RunOnRemoteDiedCallback();
    ASSERT_NE(proxy->OnRemoteDiedCallback_, nullptr);
}

/**
 * @tc.name: GetActiveDirtyRegionInfo Test
 * @tc.desc: GetActiveDirtyRegionInfo Test
 * @tc.type:FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, GetActiveDirtyRegionInfo, TestSize.Level1)
{
    NodeId id = 0;
    std::string windowName = "Test";
    RectI rectI(0, 0, 0, 0);
    std::vector<RectI> rectIs = {rectI};
    GpuDirtyRegionCollection::GetInstance().UpdateActiveDirtyInfoForDFX(id, windowName, rectIs);
    ASSERT_EQ(proxy->GetActiveDirtyRegionInfo().size(), 0);
}

/**
 * @tc.name: GetGlobalDirtyRegionInfo Test
 * @tc.desc: GetGlobalDirtyRegionInfo Test
 * @tc.type:FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, GetGlobalDirtyRegionInfo, TestSize.Level1)
{
    RectI rectI(0, 0, 0, 0);
    GpuDirtyRegionCollection::GetInstance().UpdateGlobalDirtyInfoForDFX(rectI);
    ASSERT_EQ(proxy->GetGlobalDirtyRegionInfo().globalFramesNumber, 0);
}

/**
 * @tc.name: GetLayerComposeInfo Test
 * @tc.desc: GetLayerComposeInfo Test
 * @tc.type:FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, GetLayerComposeInfo, TestSize.Level1)
{
    LayerComposeCollection::GetInstance().UpdateRedrawFrameNumberForDFX();
    ASSERT_EQ(proxy->GetLayerComposeInfo().redrawFrameNumber, 0);
}

/**
 * @tc.name: GetHwcDisabledReasonInfo Test
 * @tc.desc: GetHwcDisabledReasonInfo Test
 * @tc.type:FUNC
 * @tc.require: issueIACUOK
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, GetHwcDisabledReasonInfo, TestSize.Level1)
{
    NodeId id = 0;
    std::string nodeName = "Test";
    HwcDisabledReasonCollection::GetInstance().UpdateHwcDisabledReasonForDFX(id,
        HwcDisabledReasons::DISABLED_BY_SRC_PIXEL, nodeName);
    ASSERT_EQ(proxy->GetHwcDisabledReasonInfo().size(), 0);
}

/**
 * @tc.name: GetHdrOnDuration Test
 * @tc.desc: GetHdrOnDuration Test
 * @tc.type: FUNC
 * @tc.require: issueIB4YDF
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, GetHdrOnDuration, TestSize.Level1)
{
    ASSERT_NE(proxy, nullptr);
    int64_t hdrOnDuration;
    proxy->GetHdrOnDuration(hdrOnDuration);
    ASSERT_TRUE(proxy);
}

/**
 * @tc.name: RegisterUIExtensionCallback Test
 * @tc.desc: RegisterUIExtensionCallback Test, with empty/non-empty callback.
 * @tc.type:FUNC
 * @tc.require: issueIABHAX
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, RegisterUIExtensionCallback, TestSize.Level1)
{
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_NE(samgr, nullptr);
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    sptr<RSIUIExtensionCallback> callback = iface_cast<RSIUIExtensionCallback>(remoteObject);
    uint64_t userId = 0;
    ASSERT_EQ(proxy->RegisterUIExtensionCallback(userId, nullptr), INVALID_ARGUMENTS);
    ASSERT_EQ(proxy->RegisterUIExtensionCallback(userId, callback), RS_CONNECTION_ERROR);
}

/**
 * @tc.name: SetLayerTop Test
 * @tc.desc: SetLayerTop Test
 * @tc.type:FUNC
 * @tc.require: issueIAOZFC
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, SetLayerTop, TestSize.Level1)
{
    const std::string nodeIdStr = "123456";
    proxy->SetLayerTop(nodeIdStr, true);
    proxy->SetLayerTop(nodeIdStr, false);
    ASSERT_TRUE(proxy);
}

/**
 * @tc.name: SetForceRefresh Test
 * @tc.desc: SetForceRefresh Test
 * @tc.type:FUNC
 * @tc.require: issueIAOZFC
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, SetForceRefresh, TestSize.Level1)
{
    const std::string nodeIdStr = "123456";
    proxy->SetForceRefresh(nodeIdStr, true);
    proxy->SetForceRefresh(nodeIdStr, false);
    ASSERT_TRUE(proxy);
}

/**
 * @tc.name: SetFreeMultiWindowStatus Test
 * @tc.desc: SetFreeMultiWindowStatus Test
 * @tc.type:FUNC
 * @tc.require: issueIB31K8
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, SetFreeMultiWindowStatus, TestSize.Level1)
{
    proxy->SetFreeMultiWindowStatus(true);
    proxy->SetFreeMultiWindowStatus(false);
    ASSERT_TRUE(proxy);
}

/**
 * @tc.name: SetWindowContainer Test
 * @tc.desc: SetWindowContainer Test
 * @tc.type:FUNC
 * @tc.require: issueIBIK1X
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, SetWindowContainer, TestSize.Level1)
{
    NodeId nodeId = {};
    proxy->SetWindowContainer(nodeId, false);
    ASSERT_TRUE(proxy);
}

/**
 * @tc.name: GetPixelMapByProcessIdTest
 * @tc.desc: Test GetPixelMapByProcessId
 * @tc.type: FUNC
 * @tc.require: issueIBJFIK
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, GetPixelMapByProcessIdTest, TestSize.Level1)
{
    pid_t pid = 0;
    std::vector<PixelMapInfo> pixelMapInfoVector;
    int32_t repCode;
    ASSERT_EQ(proxy->GetPixelMapByProcessId(pixelMapInfoVector, pid, repCode), ERR_INVALID_VALUE);
    ASSERT_EQ(repCode, RS_CONNECTION_ERROR);
}

/**
 * @tc.name: SetBehindWindowFilterEnabledTest
 * @tc.desc: SetBehindWindowFilterEnabledTest
 * @tc.type: FUNC
 * @tc.require: issuesIC5OEB
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, SetBehindWindowFilterEnabledTest, TestSize.Level1)
{
    auto connectionProxy = RSRenderServiceConnectHub::GetRenderService();
    auto res = connectionProxy->SetBehindWindowFilterEnabled(true);
    EXPECT_EQ(res, ERR_OK);
}

/**
 * @tc.name: GetBehindWindowFilterEnabledTest
 * @tc.desc: GetBehindWindowFilterEnabledTest
 * @tc.type: FUNC
 * @tc.require: issuesIC5OEB
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, GetBehindWindowFilterEnabledTest, TestSize.Level1)
{
    bool enabled = true;
    auto connectionProxy = RSRenderServiceConnectHub::GetRenderService();
    auto res = connectionProxy->GetBehindWindowFilterEnabled(enabled);
    EXPECT_EQ(res, ERR_OK);
}
} // namespace Rosen
} // namespace OHOS