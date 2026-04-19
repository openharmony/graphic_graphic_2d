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
#include <gmock/gmock.h>

#include <if_system_ability_manager.h>
#include <iremote_stub.h>
#include <iservice_registry.h>
#include <mutex>
#include <system_ability_definition.h>
#include <unistd.h>

#include "feature/capture/rs_ui_capture.h"
#include "file_ex.h"
#include "platform/ohos/transaction/zidl/rs_client_to_render_connection_proxy.h"
#include "platform/ohos/rs_render_service_connect_hub.h"
#include "command/rs_animation_command.h"
#include "command/rs_node_showing_command.h"
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
#include "ipc_callbacks/rs_icanvas_surface_buffer_callback.h"
#include "platform/ohos/backend/surface_buffer_utils.h"
#endif
#include "iconsumer_surface.h"
#include "pixel_map.h"
#include "feature/capture/rs_capture_pixelmap_manager.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
class MockRSBrightnessInfoChangeCallback : public IRemoteProxy<RSIBrightnessInfoChangeCallback> {
public:
    explicit MockRSBrightnessInfoChangeCallback() : IRemoteProxy<RSIBrightnessInfoChangeCallback>(nullptr) {};
    virtual ~MockRSBrightnessInfoChangeCallback() noexcept = default;

    void OnBrightnessInfoChange(ScreenId screenId, const BrightnessInfo& brightnessInfo) override {}
};

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
class MockRSCanvasSurfaceBufferCallbackProxy : public IRemoteProxy<RSICanvasSurfaceBufferCallback> {
public:
    explicit MockRSCanvasSurfaceBufferCallbackProxy(const sptr<IRemoteObject>& impl)
        : IRemoteProxy<RSICanvasSurfaceBufferCallback>(impl) {};
    virtual ~MockRSCanvasSurfaceBufferCallbackProxy() noexcept = default;
    void OnCanvasSurfaceBufferChanged(NodeId nodeId, sptr<SurfaceBuffer> buffer, uint32_t resetSurfaceIndex) override {}
};
#endif
} // namespace
class RSClientToRenderConnectionProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline std::shared_ptr<RSClientToRenderConnectionProxy> proxy;
};

void RSClientToRenderConnectionProxyTest::SetUpTestCase()
{
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_NE(samgr, nullptr);
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    proxy = std::make_shared<RSClientToRenderConnectionProxy>(remoteObject);
}
void RSClientToRenderConnectionProxyTest::TearDownTestCase() {}
void RSClientToRenderConnectionProxyTest::SetUp() {}
void RSClientToRenderConnectionProxyTest::TearDown() {}
// Add test function forward declaration
void WaitNeedRegisterTypefaceReply(uint8_t rspRpc, int& retryCount);

class IRemoteObjectMock : public IRemoteObject {
public:
    IRemoteObjectMock() : IRemoteObject {u"IRemoteObjectMock"}
    {
    }

    ~IRemoteObjectMock()
    {
    }

    int32_t GetObjectRefCount()
    {
        return 0;
    }

    bool AddDeathRecipient(const sptr<DeathRecipient> &recipient)
    {
        return true;
    }

    bool RemoveDeathRecipient(const sptr<DeathRecipient> &recipient)
    {
        return true;
    }

    int Dump(int fd, const std::vector<std::u16string> &args)
    {
        return 0;
    }

    MOCK_METHOD4(SendRequest, int32_t(uint32_t, MessageParcel&, MessageParcel&, MessageOption&));
};

/**
 * @tc.name: CommitTransaction Test
 * @tc.desc: CommitTransaction Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSClientToRenderConnectionProxyTest, CommitTransaction, TestSize.Level1)
{
    std::unique_ptr<RSTransactionData> transactionData;
    proxy->CommitTransaction(transactionData);
    transactionData = std::make_unique<RSTransactionData>();
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(1, 1, 1, AnimationCallbackEvent::FINISHED);
    NodeId nodeId = 1;
    FollowType followType = FollowType::FOLLOW_TO_PARENT;
    transactionData->AddCommand(command, nodeId, followType);
    proxy->CommitTransaction(transactionData);
    ASSERT_EQ(proxy->transactionDataIndex_, 1);
}

/**
 * @tc.name: ExecuteSynchronousTask Test
 * @tc.desc: ExecuteSynchronousTask Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSClientToRenderConnectionProxyTest, ExecuteSynchronousTask, TestSize.Level1)
{
    std::shared_ptr<RSSyncTask> task;
    proxy->ExecuteSynchronousTask(task);
    NodeId targetId;
    std::shared_ptr<RSRenderPropertyBase> property = std::make_shared<RSRenderProperty<bool>>();
    task = std::make_shared<RSNodeGetShowingPropertyAndCancelAnimation>(targetId, property);
    proxy->ExecuteSynchronousTask(task);
    ASSERT_NE(proxy->transactionDataIndex_, 5);
}

/**
 * @tc.name: CreateNode Test
 * @tc.desc: CreateNode Test
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSClientToRenderConnectionProxyTest, CreateNodeTest, TestSize.Level1)
{
    RSDisplayNodeConfig rsDisplayNodeConfig;
    NodeId nodeId = 100;
    bool success = false;
    ASSERT_EQ(proxy->CreateNode(rsDisplayNodeConfig, nodeId, success), ERR_INVALID_VALUE);
}

/**
 * @tc.name: CreateNodeAndSurface Test
 * @tc.desc: CreateNodeAndSurface Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSClientToRenderConnectionProxyTest, CreateNodeAndSurface, TestSize.Level1)
{
    bool success;
    ASSERT_EQ(proxy->CreateNode(RSSurfaceRenderNodeConfig(), success), ERR_INVALID_VALUE);
    ASSERT_FALSE(success);
    sptr<Surface> surface = nullptr;
    bool unobscure = false;
    ASSERT_EQ(proxy->CreateNodeAndSurface(RSSurfaceRenderNodeConfig(), surface, unobscure), ERR_INVALID_VALUE);
}

/**
 * @tc.name: FillParcelWithTransactionData Test
 * @tc.desc: FillParcelWithTransactionData Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSClientToRenderConnectionProxyTest, FillParcelWithTransactionData, TestSize.Level1)
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
HWTEST_F(RSClientToRenderConnectionProxyTest, FillParcelWithTransactionData002, TestSize.Level1)
{
    std::shared_ptr<MessageParcel> parcel = std::make_shared<MessageParcel>();
    parcel->SetDataSize(102401);
    auto transactionData = std::make_unique<RSTransactionData>();
    ASSERT_TRUE(proxy->FillParcelWithTransactionData(transactionData, parcel));
}

/**
 * @tc.name: RegisterApplicationAgent Test
 * @tc.desc: RegisterApplicationAgent Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSClientToRenderConnectionProxyTest, RegisterApplicationAgent, TestSize.Level1)
{
    uint32_t pid = 1;
    sptr<IApplicationAgent> app;
    proxy->RegisterApplicationAgent(pid, app);
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_NE(samgr, nullptr);
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    app = iface_cast<IApplicationAgent>(remoteObject);
    proxy->RegisterApplicationAgent(pid, app);
    ASSERT_NE(proxy->transactionDataIndex_, 5);
}

/**
 * @tc.name: RegisterBufferAvailableListener Test
 * @tc.desc: RegisterBufferAvailableListener Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSClientToRenderConnectionProxyTest, RegisterBufferAvailableListener, TestSize.Level1)
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
    ASSERT_NE(proxy->transactionDataIndex_, 5);
}

/**
 * @tc.name: GetBitmap Test
 * @tc.desc: GetBitmap Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSClientToRenderConnectionProxyTest, GetBitmap, TestSize.Level1)
{
    ScreenId id = 1;
    Drawing::Bitmap bitmap;
    bool success;
    ASSERT_EQ(proxy->GetBitmap(id, bitmap, success), ERR_INVALID_VALUE);
    ASSERT_FALSE(success);
}

/**
 * @tc.name: SetGlobalDarkColorMode Test
 * @tc.desc: SetGlobalDarkColorMode Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSClientToRenderConnectionProxyTest, SetGlobalDarkColorMode, TestSize.Level1)
{
    ASSERT_TRUE(proxy->SetGlobalDarkColorMode(true) == ERR_OK);
}

/**
 * @tc.name: GetPixelmap Test
 * @tc.desc: GetPixelmap Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSClientToRenderConnectionProxyTest, GetPixelmap, TestSize.Level1)
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
 * @tc.name: SetSystemAnimatedScenes Test
 * @tc.desc: SetSystemAnimatedScenes Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSClientToRenderConnectionProxyTest, SetSystemAnimatedScenes, TestSize.Level1)
{
    bool success;
    proxy->SetSystemAnimatedScenes(SystemAnimatedScenes::ENTER_MISSION_CENTER, false, success);
    ASSERT_FALSE(success);
}

/**
 * @tc.name: SetFocusAppInfo Test
 * @tc.desc: SetFocusAppInfo Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSClientToRenderConnectionProxyTest, SetFocusAppInfo, TestSize.Level1)
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
 * @tc.name: RegisterTransactionDataCallback01
 * @tc.desc: RegisterTransactionDataCallback Test normal
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSClientToRenderConnectionProxyTest, RegisterTransactionDataCallback01, TestSize.Level1)
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
HWTEST_F(RSClientToRenderConnectionProxyTest, TakeSurfaceCapture, TestSize.Level1)
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
}

/**
 * @tc.name: TakeUICaptureInRange Test
 * @tc.desc: TakeUICaptureInRange Test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToRenderConnectionProxyTest, TakeUICaptureInRange, TestSize.Level1)
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
 * @tc.name: GetBrightnessInfoTest
 * @tc.desc: test results of GetBrightnessInfo
 * @tc.type: FUNC
 */
HWTEST_F(RSClientToRenderConnectionProxyTest, GetBrightnessInfoTest, TestSize.Level1)
{
    ASSERT_NE(proxy, nullptr);
    ScreenId screenId = 0;
    BrightnessInfo brightnessInfo = { 0 };
    ASSERT_NE(proxy->GetBrightnessInfo(screenId, brightnessInfo), SUCCESS);
    screenId = INVALID_SCREEN_ID;
    ASSERT_NE(proxy->GetBrightnessInfo(screenId, brightnessInfo), SUCCESS);
}

/**
 * @tc.name: ReadBrightnessInfoTest
 * @tc.desc: test results of ReadBrightnessInfo
 * @tc.type: FUNC
 */
HWTEST_F(RSClientToRenderConnectionProxyTest, ReadBrightnessInfoTest, TestSize.Level1)
{
    ASSERT_NE(proxy, nullptr);
    BrightnessInfo brightnessInfo = { 0 };

    // case 1: valid data
    {
        MessageParcel data;
        data.WriteFloat(1.0f);
        data.WriteFloat(1.0f);
        data.WriteFloat(1.0f);
        ASSERT_TRUE(proxy->ReadBrightnessInfo(brightnessInfo, data));
    }

    // case 2: invalid data
    {
        MessageParcel data;
        data.WriteFloat(1.0f);
        data.WriteFloat(1.0f);
        ASSERT_FALSE(proxy->ReadBrightnessInfo(brightnessInfo, data));
    }

    // case 3: invalid data
    {
        MessageParcel data;
        data.WriteFloat(1.0f);
        ASSERT_FALSE(proxy->ReadBrightnessInfo(brightnessInfo, data));
    }

    // case 4: invalid data
    {
        MessageParcel data;
        ASSERT_FALSE(proxy->ReadBrightnessInfo(brightnessInfo, data));
    }
}

/**
 * @tc.name: TakeSurfaceCaptureSoloNode Test
 * @tc.desc: TakeSurfaceCaptureSoloNode Test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToRenderConnectionProxyTest, TakeSurfaceCaptureSoloNode, TestSize.Level1)
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
HWTEST_F(RSClientToRenderConnectionProxyTest, SetHwcNodeBounds, TestSize.Level1)
{
    NodeId id = 1;
    proxy->SetHwcNodeBounds(id, 1.0f, 1.0f, 1.0f, 1.0f);
    ASSERT_EQ(proxy->transactionDataIndex_, 0);
}

/**
 * @tc.name: SetWindowContainer Test
 * @tc.desc: SetWindowContainer Test
 * @tc.type:FUNC
 * @tc.require: issueIBIK1X
 */
HWTEST_F(RSClientToRenderConnectionProxyTest, SetWindowContainer, TestSize.Level1)
{
    NodeId nodeId = {};
    proxy->SetWindowContainer(nodeId, false);
    ASSERT_TRUE(proxy);
}

/**
 * @tc.name: ClearUifirstCache Test
 * @tc.desc: ClearUifirstCache Test
 * @tc.type:FUNC
 * @tc.require: issueICK4SM
 */
HWTEST_F(RSClientToRenderConnectionProxyTest, ClearUifirstCacheTest, TestSize.Level1)
{
    NodeId nodeId = 1;
    proxy->ClearUifirstCache(nodeId);
    ASSERT_TRUE(proxy);
}

/**
 * @tc.name: TakeSurfaceCaptureWithAllWindowsTest
 * @tc.desc: TakeSurfaceCaptureWithAllWindows test to capture screen
 * @tc.type:FUNC
 * @tc.require: issueICQ74B
 */
HWTEST_F(RSClientToRenderConnectionProxyTest, TakeSurfaceCaptureWithAllWindowsTest, TestSize.Level1)
{
    ASSERT_NE(proxy, nullptr);
    NodeId nodeId = 1;
    bool checkDrmAndSurfaceLock = false;
    sptr<RSISurfaceCaptureCallback> callback;
    RSSurfaceCaptureConfig captureConfig;
    auto ret = proxy->TakeSurfaceCaptureWithAllWindows(nodeId, callback, captureConfig, checkDrmAndSurfaceLock);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);

    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_NE(samgr, nullptr);
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    callback = iface_cast<RSISurfaceCaptureCallback>(remoteObject);
    ret = proxy->TakeSurfaceCaptureWithAllWindows(nodeId, callback, captureConfig, checkDrmAndSurfaceLock);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: NotifyLightFactorStatus Test
 * @tc.desc: NotifyLightFactorStatus Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSClientToRenderConnectionProxyTest, NotifyLightFactorStatus, TestSize.Level1)
{
    NodeId id = 1;
    proxy->SetHardwareEnabled(id, true, SelfDrawingNodeType::DEFAULT, true);
    ASSERT_EQ(proxy->transactionDataIndex_, 5);
}

/**
 * @tc.name: FreezeScreenTest
 * @tc.desc: FreezeScreen test to freeze or unfreeze screen.
 * @tc.type:FUNC
 * @tc.require: issueICS2J8
 */
HWTEST_F(RSClientToRenderConnectionProxyTest, FreezeScreenTest, TestSize.Level1)
{
    ASSERT_NE(proxy, nullptr);
    NodeId nodeId = 1;
    bool isFreeze = false;
    auto ret = proxy->FreezeScreen(nodeId, isFreeze);
    EXPECT_EQ(ret, ERR_OK);
}

/*
 * @tc.name: DropFrameByPidWithInvalidParameter Test
 * @tc.desc: DropFrameByPidWithInvalidParameter Test
 * @tc.type:FUNC
 * @tc.require: issueICK4SM
 */
HWTEST_F(RSClientToRenderConnectionProxyTest, DropFrameByPidWithInvalidParameter, TestSize.Level1)
{
    // MAX_DROP_FRAME_PID_LIST_SIZE = 1024
    std::vector<int32_t> vec(1025);
    ASSERT_EQ(proxy->DropFrameByPid(vec), ERR_INVALID_VALUE);
}

/**
 * @tc.name: RegisterBufferClearListener Test
 * @tc.desc: RegisterBufferClearListener Test
 * @tc.type:FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSClientToRenderConnectionProxyTest, RegisterBufferClearListener, TestSize.Level1)
{
    NodeId id = 1;
    sptr<RSIBufferClearCallback> callback;
    proxy->RegisterBufferClearListener(id, callback);
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_NE(samgr, nullptr);
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    callback = iface_cast<RSIBufferClearCallback>(remoteObject);
    proxy->RegisterBufferClearListener(id, callback);
    ASSERT_NE(proxy->transactionDataIndex_, 5);
}

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
/**
 * @tc.name: RegisterCanvasCallbackTest
 * @tc.desc: RegisterCanvasCallback Test
 * @tc.type:FUNC
 */
HWTEST_F(RSClientToRenderConnectionProxyTest, RegisterCanvasCallbackTest, TestSize.Level1)
{
    ASSERT_NE(proxy, nullptr);
    sptr<RSICanvasSurfaceBufferCallback> callback = nullptr;
    proxy->RegisterCanvasCallback(callback);
    callback = new MockRSCanvasSurfaceBufferCallbackProxy(nullptr);
    proxy->RegisterCanvasCallback(callback);
    sptr<IRemoteObject> remoteObject = new IRemoteObjectMock();
    callback = new MockRSCanvasSurfaceBufferCallbackProxy(remoteObject);
    proxy->RegisterCanvasCallback(callback);
}

/**
 * @tc.name: SubmitCanvasPreAllocatedBufferTest
 * @tc.desc: SubmitCanvasPreAllocatedBuffer Test
 * @tc.type:FUNC
 */
HWTEST_F(RSClientToRenderConnectionProxyTest, SubmitCanvasPreAllocatedBufferTest, TestSize.Level1)
{
    ASSERT_NE(proxy, nullptr);
    auto ret = proxy->SubmitCanvasPreAllocatedBuffer(1, nullptr, 1);
    ASSERT_NE(ret, 0);
    auto buffer = SurfaceBufferUtils::CreateCanvasSurfaceBuffer(1, 100, 100);
    ret = proxy->SubmitCanvasPreAllocatedBuffer(1, buffer, 2);
    ASSERT_NE(ret, 0);
    buffer = SurfaceBuffer::Create();
    ret = proxy->SubmitCanvasPreAllocatedBuffer(1, buffer, 3);
    ASSERT_NE(ret, 0);
}
#endif

/**
 * @tc.name: SetLogicalCameraRotationCorrectionTest001
 * @tc.desc: SetLogicalCameraRotationCorrection test to write legal degree.
 * @tc.type:FUNC
 * @tc.require: issueICS2J8
 */
HWTEST_F(RSClientToRenderConnectionProxyTest, SetLogicalCameraRotationCorrection001, TestSize.Level1)
{
    ASSERT_NE(proxy, nullptr);
    ScreenId screenId = 0;
    ScreenRotation logicalCorrection = ScreenRotation::ROTATION_90;
    auto ret = proxy->SetLogicalCameraRotationCorrection(screenId, logicalCorrection);
    EXPECT_EQ(ret, 2);
}

/**
 * @tc.name: GetMaxGpuBufferSize001
 * @tc.desc: Test GetMaxGpuBufferSize with zero initial values
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToRenderConnectionProxyTest, GetMaxGpuBufferSize001, TestSize.Level1)
{
    ASSERT_NE(proxy, nullptr);
    uint32_t maxWidth = 0;
    uint32_t maxHeight = 0;
    int32_t ret = proxy->GetMaxGpuBufferSize(maxWidth, maxHeight);
    EXPECT_LE(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: GetMaxGpuBufferSize002
 * @tc.desc: Test GetMaxGpuBufferSize with preset values
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToRenderConnectionProxyTest, GetMaxGpuBufferSize002, TestSize.Level1)
{
    ASSERT_NE(proxy, nullptr);
    uint32_t maxWidth = 2048;
    uint32_t maxHeight = 2048;
    int32_t ret = proxy->GetMaxGpuBufferSize(maxWidth, maxHeight);
    EXPECT_LE(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: GetMaxGpuBufferSize003
 * @tc.desc: Test GetMaxGpuBufferSize with UINT32_MAX initial values
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToRenderConnectionProxyTest, GetMaxGpuBufferSize003, TestSize.Level1)
{
    ASSERT_NE(proxy, nullptr);
    uint32_t maxWidth = UINT32_MAX;
    uint32_t maxHeight = UINT32_MAX;
    int32_t ret = proxy->GetMaxGpuBufferSize(maxWidth, maxHeight);
    EXPECT_LE(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: GetMaxGpuBufferSize004
 * @tc.desc: Test GetMaxGpuBufferSize with different width and height
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToRenderConnectionProxyTest, GetMaxGpuBufferSize004, TestSize.Level1)
{
    ASSERT_NE(proxy, nullptr);
    uint32_t maxWidth = 4096;
    uint32_t maxHeight = 2160;
    int32_t ret = proxy->GetMaxGpuBufferSize(maxWidth, maxHeight);
    EXPECT_LE(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: RegisterFrameStabilityDetectionTest001
 * @tc.desc: Test RegisterFrameStabilityDetection with nullptr callback
 * @tc.type: FUNC
 * @tc.require: issues22734
 */
HWTEST_F(RSClientToRenderConnectionProxyTest, RegisterFrameStabilityDetectionTest001, TestSize.Level1)
{
    FrameStabilityTarget target =
        { .id = 100, .type = FrameStabilityTargetType::SCREEN };
    FrameStabilityConfig config = {
        .stableDuration = 1000,
        .changePercent = 0.1f
    };
    int32_t ret = proxy->RegisterFrameStabilityDetection(target, config, nullptr);
    EXPECT_EQ(ret, INVALID_ARGUMENTS);
}
} // namespace Rosen
} // namespace OHOS