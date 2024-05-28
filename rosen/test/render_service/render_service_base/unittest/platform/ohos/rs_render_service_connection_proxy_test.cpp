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

#include "platform/ohos/rs_render_service_connection_proxy.h"
#include "command/rs_animation_command.h"
#include "command/rs_node_showing_command.h"
#include "iconsumer_surface.h"

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
    std::unique_ptr<RSCommand> command = std::make_unique<RSAnimationCallback>(1, 1, FINISHED);
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
    std::shared_ptr<RSRenderPropertyBase> property = std::make_shared<RSRenderPropertyBase>();
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
    proxy->GetUniRenderEnabled();
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
 * @tc.name: CreateNodeAndSurface Test
 * @tc.desc: CreateNodeAndSurface Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, CreateNodeAndSurface, TestSize.Level1)
{
    ASSERT_FALSE(proxy->CreateNode(RSSurfaceRenderNodeConfig()));
    ASSERT_EQ(proxy->CreateNodeAndSurface(RSSurfaceRenderNodeConfig()), nullptr);
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
    ASSERT_EQ(proxy->CreateVSyncConnection(name, token, id, windowNodeId), nullptr);
    token = new IRemoteStub<VSyncIConnectionToken>();
    ASSERT_EQ(proxy->CreateVSyncConnection(name, token, id, windowNodeId), nullptr);
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
    ASSERT_EQ(proxy->CreatePixelMapFromSurface(surface, srcRect), nullptr);
    sptr<IConsumerSurface> consumer = IConsumerSurface::Create("DisplayNode");
    sptr<IBufferProducer> producer = consumer->GetProducer();
    surface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_EQ(proxy->CreatePixelMapFromSurface(surface, srcRect), nullptr);
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
    ASSERT_EQ(proxy->SetFocusAppInfo(pid, uid, bundleName, abilityName, focusNodeId), 0);
}

/**
 * @tc.name: GetAllScreenIds Test
 * @tc.desc: GetAllScreenIds Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, GetAllScreenIds, TestSize.Level1)
{
    EXPECT_EQ(proxy->GetDefaultScreenId(), INVALID_SCREEN_ID);
    EXPECT_EQ(proxy->GetActiveScreenId(), INVALID_SCREEN_ID);
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
    std::vector<NodeId> filteredAppVector;
    EXPECT_EQ(proxy->CreateVirtualScreen(name, width, height, surface, mirrorId, flags, filteredAppVector),
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
 * @tc.name: Set2DRenderCtrl Test
 * @tc.desc: Set2DRenderCtrl Test
 * @tc.type:FUNC
 * @tc.require: issueI9NA1T
 */
#ifdef RS_ENABLE_VK
HWTEST_F(RSRenderServiceConnectionProxyTest, Set2DRenderCtrl, TestSize.Level1)
{
    proxy->Set2DRenderCtrl(false);
    ASSERT_TRUE(true);
    proxy->Set2DRenderCtrl(true);
    ASSERT_TRUE(true);
}
#endif

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
    proxy->SyncFrameRateRange(id, range, false);
    ASSERT_EQ(proxy->transactionDataIndex_, 0);
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
    EXPECT_FALSE(proxy->GetShowRefreshRateEnabled());
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
    proxy->SetShowRefreshRateEnabled(true);
    ASSERT_EQ(proxy->transactionDataIndex_, 0);
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
 * @tc.name: TakeSurfaceCapture Test
 * @tc.desc: TakeSurfaceCapture Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, TakeSurfaceCapture, TestSize.Level1)
{
    NodeId id = 1;
    sptr<RSISurfaceCaptureCallback> callback;
    float scaleX = 1.0f;
    float scaleY = 1.0f;
    SurfaceCaptureType surfaceCaptureType = SurfaceCaptureType::UICAPTURE;
    bool isSync = true;
    proxy->TakeSurfaceCapture(id, callback, scaleX, scaleY, surfaceCaptureType, isSync);

    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_NE(samgr, nullptr);
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    callback = iface_cast<RSISurfaceCaptureCallback>(remoteObject);
    proxy->TakeSurfaceCapture(id, callback, scaleX, scaleY, surfaceCaptureType, isSync);
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
    ASSERT_EQ(proxy->GetScreenActiveMode(id).width_, -1);
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
    ASSERT_EQ(proxy->GetMemoryGraphics().size(), 0);
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
    ASSERT_FALSE(proxy->GetTotalAppMemSize(cpuMemSize, gpuMemSize));
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
    EXPECT_EQ(proxy->GetMemoryGraphic(pid).pid_, 0);
    ScreenId id = 1;
    proxy->GetScreenCapability(id);
    EXPECT_EQ(proxy->GetScreenData(id).powerStatus_, INVALID_POWER_STATUS);
    EXPECT_EQ(proxy->GetScreenBacklight(id), -1);
    ASSERT_EQ(proxy->GetScreenPowerStatus(id), ScreenPowerStatus::INVALID_POWER_STATUS);
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
    EXPECT_EQ(proxy->SetPixelFormat(id, pixelFormat), 2);
    ASSERT_EQ(proxy->GetPixelFormat(id, pixelFormat), 2);
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
    ASSERT_EQ(proxy->GetScreenSupportedHDRFormats(id, hdrFormats), 2);
    ScreenHDRFormat hdrFormat = IMAGE_HDR_ISO_DUAL;
    ASSERT_EQ(proxy->GetScreenHDRFormat(id, hdrFormat), 2);
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
    ASSERT_EQ(proxy->SetScreenHDRFormat(id, modeIdx), 2);
    std::vector<GraphicCM_ColorSpaceType> colorSpaces;
    ASSERT_EQ(proxy->GetScreenSupportedColorSpaces(id, colorSpaces), 2);
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
    ASSERT_EQ(proxy->SetScreenColorSpace(id, colorSpace), 2);
    std::vector<GraphicCM_ColorSpaceType> colorSpaces;
    ASSERT_EQ(proxy->GetScreenColorSpace(id, colorSpace), 2);
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
    ASSERT_FALSE(proxy->GetBitmap(1, bitmap));
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
    EXPECT_EQ(proxy->SetScreenSkipFrameInterval(id, 1), 2);
    EXPECT_FALSE(proxy->SetVirtualMirrorScreenCanvasRotation(id, true));
    ASSERT_FALSE(proxy->SetVirtualMirrorScreenScaleMode(id, ScreenScaleMode::UNISCALE_MODE));
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
    ASSERT_FALSE(proxy->GetPixelmap(id, pixelmap, &rect, drawCmdList));
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
    EXPECT_EQ(proxy->RegisterOcclusionChangeCallback(callback), 2);
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
 * @tc.name: SetSystemAnimatedScenes Test
 * @tc.desc: SetSystemAnimatedScenes Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSRenderServiceConnectionProxyTest, SetSystemAnimatedScenes, TestSize.Level1)
{
    proxy->SetAppWindowNum(1);
    ASSERT_FALSE(proxy->SetSystemAnimatedScenes(SystemAnimatedScenes::ENTER_MISSION_CENTER));
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
    proxy->SetHardwareEnabled(id, true, SelfDrawingNodeType::DEFAULT);
    proxy->NotifyLightFactorStatus(true);
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
    EventInfo eventInfo;
    proxy->NotifyRefreshRateEvent(eventInfo);
    int32_t touchStatus = 1;
    int32_t touchCnt = 0;
    proxy->NotifyTouchEvent(touchStatus, touchCnt);
    proxy->SetCacheEnabledForRotation(true);
    ASSERT_EQ(proxy->transactionDataIndex_, 0);
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
} // namespace Rosen
} // namespace OHOS