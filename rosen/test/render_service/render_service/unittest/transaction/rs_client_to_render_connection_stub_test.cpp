/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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

#include <iremote_stub.h>
#include <message_option.h>
#include <message_parcel.h>
#include <parameters.h>

#include "gtest/gtest.h"
#include "limit_number.h"
#include "mock_hdi_device.h"
#include "sandbox_utils.h"
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
#include "feature_cfg/feature_param/performance_feature/node_mem_release_param.h"
#include "ipc_callbacks/rs_canvas_surface_buffer_callback_stub.h"
#include "platform/ohos/backend/surface_buffer_utils.h"
#endif
#include "ipc_callbacks/rs_frame_rate_linker_expected_fps_update_callback_stub.h"
#include "ipc_callbacks/rs_iframe_rate_linker_expected_fps_update_callback_ipc_interface_code.h"
#include "ipc_callbacks/rs_iframe_rate_linker_expected_fps_update_callback.h"
#include "ipc_callbacks/screen_change_callback_stub.h"
#include "ipc_callbacks/brightness_info_change_callback_stub.h"
#include "pipeline/render_thread/rs_composer_adapter.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "transaction/rs_client_to_render_connection.h"
#include "pipeline/rs_logical_display_render_node.h"
#include "pipeline/rs_pointer_window_manager.h"
#include "pipeline/rs_render_node_gc.h"
#include "pipeline/rs_screen_render_node.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "ipc_callbacks/rs_transaction_data_callback_stub.h"
#include "rs_irender_service.h"
#include "screen_manager/screen_types.h"
#include "transaction/zidl/rs_client_to_render_connection_stub.h"
#include "screen_manager/rs_screen.h"
#include "feature/capture/rs_capture_pixelmap_manager.h"
#include "ipc_callbacks/surface_capture_callback_stub.h"
#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif
using namespace testing;
using namespace testing::ext;

namespace {
    const int DEFAULT_WIDTH = 2160;
    const int DEFAULT_HEIGHT = 1080;
};

namespace OHOS::Rosen {
namespace {
class MockRSBrightnessInfoChangeCallback : public IRemoteProxy<RSIBrightnessInfoChangeCallback> {
public:
    explicit MockRSBrightnessInfoChangeCallback() : IRemoteProxy<RSIBrightnessInfoChangeCallback>(nullptr) {};
    virtual ~MockRSBrightnessInfoChangeCallback() noexcept = default;

    void OnBrightnessInfoChange(ScreenId screenId, const BrightnessInfo& brightnessInfo) override {}
};
} // namespace
class RSClientToRenderConnectionStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    void CreateComposerAdapterWithScreenInfo(uint32_t width = 2560, uint32_t height = 1080,
        ScreenColorGamut colorGamut = ScreenColorGamut::COLOR_GAMUT_SRGB,
        ScreenState state = ScreenState::UNKNOWN,
        ScreenRotation rotation = ScreenRotation::ROTATION_0);
    static inline Mock::HdiDeviceMock* hdiDeviceMock_;
    static inline std::unique_ptr<RSComposerAdapter> composerAdapter_;
    static inline sptr<RSScreenManager> screenManager_;
    static inline RSMainThread* mainThread_;
    static inline std::shared_ptr<HdiOutput> hdiOutput_;
    int32_t offsetX = 0; //screenOffset on x axis equals to 0
    int32_t offsetY = 0; //screenOffset on y axis equals to 0
    float mirrorAdaptiveCoefficient = 1.0f;
    static uint32_t screenId_;
    static std::shared_ptr<RSSurfaceRenderNode> surfaceNode_;
private:
    int OnRemoteRequestTest(uint32_t code);
    static sptr<RSIConnectionToken> token_;
    static sptr<RSClientToRenderConnectionStub> toRenderConnectionStub_;
};

uint32_t RSClientToRenderConnectionStubTest::screenId_ = 0;
sptr<RSIConnectionToken> RSClientToRenderConnectionStubTest::token_ = new IRemoteStub<RSIConnectionToken>();
sptr<RSClientToRenderConnectionStub> RSClientToRenderConnectionStubTest::toRenderConnectionStub_ =
    new RSClientToRenderConnection(
        0, nullptr, RSMainThread::Instance(), CreateOrGetScreenManager(), token_->AsObject(), nullptr);
std::shared_ptr<RSSurfaceRenderNode> RSClientToRenderConnectionStubTest::surfaceNode_ =
    std::shared_ptr<RSSurfaceRenderNode>(new RSSurfaceRenderNode(10003, std::make_shared<RSContext>(), true),
    RSRenderNodeGC::NodeDestructor);

void RSClientToRenderConnectionStubTest::SetUpTestCase()
{
#ifdef RS_ENABLE_VK
    RsVulkanContext::SetRecyclable(false);
#endif
    hdiOutput_ = HdiOutput::CreateHdiOutput(screenId_);
    auto rsScreen = std::make_shared<RSScreen>(hdiOutput_);
    screenManager_ = CreateOrGetScreenManager();
    screenManager_->MockHdiScreenConnected(rsScreen);
    hdiDeviceMock_ = Mock::HdiDeviceMock::GetInstance();
    EXPECT_CALL(*hdiDeviceMock_, RegHotPlugCallback(_, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*hdiDeviceMock_, RegHwcDeadCallback(_, _)).WillRepeatedly(testing::Return(false));
    EXPECT_CALL(*hdiDeviceMock_, RegRefreshCallback(_, _)).WillRepeatedly(testing::Return(0));
    mainThread_ = new RSMainThread();
    mainThread_->runner_ = OHOS::AppExecFwk::EventRunner::Create(true);
    mainThread_->handler_ = std::make_shared<OHOS::AppExecFwk::EventHandler>(mainThread_->runner_);
}

class RSSurfaceCaptureCallbackStubMock : public RSSurfaceCaptureCallbackStub {
public:
    RSSurfaceCaptureCallbackStubMock() = default;
    virtual ~RSSurfaceCaptureCallbackStubMock() = default;
    void OnSurfaceCapture(NodeId id, const RSSurfaceCaptureConfig& captureConfig,
        Media::PixelMap* pixelmap, CaptureError captureErrorCode = CaptureError::CAPTURE_OK,
        Media::PixelMap* pixelmapHDR = nullptr) override {};
};

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
class RSCanvasSurfaceBufferCallbackStubMock : public RSCanvasSurfaceBufferCallbackStub {
public:
    RSCanvasSurfaceBufferCallbackStubMock() = default;
    virtual ~RSCanvasSurfaceBufferCallbackStubMock() = default;
    void OnCanvasSurfaceBufferChanged(NodeId nodeId, sptr<SurfaceBuffer> buffer, uint32_t resetSurfaceIndex) override {}
    bool IsProxyObject() const override
    {
        return isProxyObject_;
    }

    bool isProxyObject_ = true;
};
#endif

void g_WriteSurfaceCaptureConfigMock(RSSurfaceCaptureConfig& captureConfig, MessageParcel& data)
{
    data.WriteFloat(captureConfig.scaleX);
    data.WriteFloat(captureConfig.scaleY);
    data.WriteBool(captureConfig.useDma);
    data.WriteBool(captureConfig.useCurWindow);
    data.WriteUint8(static_cast<uint8_t>(captureConfig.captureType));
    data.WriteBool(captureConfig.isSync);
    data.WriteFloat(captureConfig.mainScreenRect.left_);
    data.WriteFloat(captureConfig.mainScreenRect.top_);
    data.WriteFloat(captureConfig.mainScreenRect.right_);
    data.WriteFloat(captureConfig.mainScreenRect.bottom_);
    data.WriteUint64(captureConfig.uiCaptureInRangeParam.endNodeId);
    data.WriteBool(captureConfig.uiCaptureInRangeParam.useBeginNodeSize);
    data.WriteFloat(captureConfig.specifiedAreaRect.left_);
    data.WriteFloat(captureConfig.specifiedAreaRect.top_);
    data.WriteFloat(captureConfig.specifiedAreaRect.right_);
    data.WriteFloat(captureConfig.specifiedAreaRect.bottom_);
    data.WriteUInt64Vector(captureConfig.blackList);
    data.WriteUint32(captureConfig.backGroundColor);
    data.WriteUint32(captureConfig.colorSpace.first);
    data.WriteBool(captureConfig.colorSpace.second);
    data.WriteUint32(captureConfig.dynamicRangeMode.first);
    data.WriteBool(captureConfig.dynamicRangeMode.second);
    data.WriteBool(captureConfig.isSyncRender);
}

/**
 * @tc.name: NotifySurfaceCaptureRemoteTest001
 * @tc.desc: NotifySurfaceCaptureRemoteTest001.
 * @tc.type: FUNC
 * @tc.require: issueI60KUK
 */
HWTEST_F(RSClientToRenderConnectionStubTest, NotifySurfaceCaptureRemoteTest001, TestSize.Level1)
{
    auto newPid = getpid();
    auto screenManagerPtr = RSScreenManager::GetInstance();
    auto mainThread = RSMainThread::Instance();

    sptr<RSIConnectionToken> token_ = new IRemoteStub<RSIConnectionToken>();
    sptr<RSClientToRenderConnectionStub> toRenderConnectionStub_ =
        new RSClientToRenderConnection(newPid, nullptr, mainThread, screenManagerPtr, token_->AsObject(), nullptr);
    ASSERT_EQ(toRenderConnectionStub_ != nullptr, true);
    sptr<RSISurfaceCaptureCallback> callback = new RSSurfaceCaptureCallbackStubMock();
    ASSERT_EQ(callback != nullptr, true);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::TAKE_SURFACE_CAPTURE);
    NodeId id = surfaceNode_->GetId();
    // Test0 Abnormal condition, isClientPixelMap = true, but no clientPixelMap
    RSSurfaceCaptureConfig captureConfig;
    data.WriteUint64(id);
    data.WriteRemoteObject(callback->AsObject());
    g_WriteSurfaceCaptureConfigMock(captureConfig, data);
    // Write BlurParm
    data.WriteBool(false);
    data.WriteFloat(0);
    // write Area Rect;
    data.WriteFloat(0);
    data.WriteFloat(0);
    data.WriteFloat(0);
    data.WriteFloat(0);
    int res = toRenderConnectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);

    // Test1: normal Capture
    auto& nodeMap = RSMainThread::Instance()->GetContext().nodeMap;
    nodeMap.RegisterRenderNode(surfaceNode_);

    MessageParcel data2;
    RSSurfaceCaptureConfig captureConfig2;
    data2.WriteUint64(id);
    data2.WriteRemoteObject(callback->AsObject());
    g_WriteSurfaceCaptureConfigMock(captureConfig2, data2);
    // Write BlurParm
    data2.WriteBool(false);
    data2.WriteFloat(0);
    // write Area Rect;
    data2.WriteFloat(0);
    data2.WriteFloat(0);
    data2.WriteFloat(0);
    data2.WriteFloat(0);

    res = toRenderConnectionStub_->OnRemoteRequest(code, data2, reply, option);

    ASSERT_EQ(res, ERR_INVALID_DATA);
    constexpr uint32_t TIME_OF_CAPTURE_TASK_REMAIN = 1000000;
    usleep(TIME_OF_CAPTURE_TASK_REMAIN);
    nodeMap.UnregisterRenderNode(id);
}

void RSClientToRenderConnectionStubTest::TearDownTestCase()
{
    hdiOutput_ = nullptr;
    composerAdapter_ = nullptr;
    screenManager_ = nullptr;
    hdiDeviceMock_ = nullptr;
    token_ = nullptr;
    toRenderConnectionStub_ = nullptr;
}

void RSClientToRenderConnectionStubTest::SetUp()
{
    CreateComposerAdapterWithScreenInfo(DEFAULT_WIDTH, DEFAULT_HEIGHT,
        ScreenColorGamut::COLOR_GAMUT_SRGB, ScreenState::UNKNOWN, ScreenRotation::ROTATION_0);

    auto mainThread = RSMainThread::Instance();
    if (!mainThread) {
        return;
    }
    mainThread->runner_ = AppExecFwk::EventRunner::Create("RSClientToRenderConnectionStubTest");
    if (!mainThread->runner_) {
        return;
    }
    mainThread->handler_ = std::make_shared<AppExecFwk::EventHandler>(mainThread->runner_);
    mainThread->runner_->Run();
}
void RSClientToRenderConnectionStubTest::TearDown() {}
int RSClientToRenderConnectionStubTest::OnRemoteRequestTest(uint32_t code)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    return toRenderConnectionStub_->OnRemoteRequest(code, data, reply, option);
}

class RSScreenChangeCallbackStubMock : public RSScreenChangeCallbackStub {
public:
    RSScreenChangeCallbackStubMock() = default;
    virtual ~RSScreenChangeCallbackStubMock() = default;
    void OnScreenChanged(ScreenId id, ScreenEvent event,
        ScreenChangeReason reason, sptr<IRemoteObject> obj = nullptr) override {};
};


class RSFrameRateLinkerExpectedFpsUpdateCallbackStubMock : public RSFrameRateLinkerExpectedFpsUpdateCallbackStub {
public:
    RSFrameRateLinkerExpectedFpsUpdateCallbackStubMock() = default;
    virtual ~RSFrameRateLinkerExpectedFpsUpdateCallbackStubMock() = default;
    void OnFrameRateLinkerExpectedFpsUpdate(pid_t dstPid,
        const std::string& xcomponentId, int32_t expectedFps) override {};
};

void RSClientToRenderConnectionStubTest::CreateComposerAdapterWithScreenInfo(uint32_t width, uint32_t height,
    ScreenColorGamut colorGamut, ScreenState state, ScreenRotation rotation)
{
    auto info = screenManager_->QueryScreenInfo(screenId_);
    info.phyWidth = width;
    info.phyHeight = height;
    info.colorGamut = colorGamut;
    info.width = width;
    info.height = height;
    info.state = state;
    info.rotation = rotation;
    composerAdapter_ = std::make_unique<RSComposerAdapter>();
    composerAdapter_->Init(info, offsetX, offsetY, mirrorAdaptiveCoefficient, nullptr);
    composerAdapter_->SetHdiBackendDevice(hdiDeviceMock_);
}

/**
 * @tc.name: TestRSClientToRenderConnectionStub001
 * @tc.desc: Test if data has no content.
 * @tc.type: FUNC
 * @tc.require: issueI60KUK
 */
HWTEST_F(RSClientToRenderConnectionStubTest, TestRSClientToRenderConnectionStub001, TestSize.Level1)
{
    EXPECT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_SURFACE_BUFFER_CALLBACK)),
        ERR_INVALID_DATA);
    EXPECT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::UNREGISTER_SURFACE_BUFFER_CALLBACK)),
        ERR_INVALID_DATA);
    if (RSUniRenderJudgement::IsUniRender()) {
        EXPECT_EQ(OnRemoteRequestTest(static_cast<uint32_t>(
                      RSIRenderServiceConnectionInterfaceCode::SET_LAYER_TOP_FOR_HARDWARE_COMPOSER)),
            ERR_INVALID_DATA);
    }
    EXPECT_EQ(OnRemoteRequestTest(static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::REGISTER_TRANSACTION_DATA_CALLBACK)), ERR_NULL_OBJECT);
}

/**
 * @tc.name: TestRSClientToRenderConnectionStub002
 * @tc.desc: Test screen related transaction, with non empty data.
 * @tc.type: FUNC
 * @tc.require: issueI60KUK
 */
HWTEST_F(RSClientToRenderConnectionStubTest, TestRSClientToRenderConnectionStub002, TestSize.Level1)
{
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_ANCO_FORCE_DO_DIRECT)), ERR_INVALID_STATE);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_WINDOW_CONTAINER)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_FOCUS_APP_INFO)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::TAKE_SURFACE_CAPTURE)), ERR_NULL_OBJECT);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::TAKE_UI_CAPTURE_IN_RANGE)), ERR_NULL_OBJECT);
}

/**
 * @tc.name: SetLayerTopForHWCTest
 * @tc.desc: Test SetLayerTopForHWC
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToRenderConnectionStubTest, SetLayerTopForHWCTest, TestSize.Level1)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;
        uint32_t code =
            static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_LAYER_TOP_FOR_HARDWARE_COMPOSER);
        data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
        data.WriteUint64(0);
        data.WriteBool(true);
        data.WriteUint32(1);
        int res = toRenderConnectionStub_->OnRemoteRequest(code, data, reply, option);
        ASSERT_EQ(res, ERR_NONE);
    }
}

/**
 * @tc.name: ClearUifirstCacheTest
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require: issueICK4SM
 */
HWTEST_F(RSClientToRenderConnectionStubTest, ClearUifirstCacheTest, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CLEAR_UIFIRST_CACHE);
    data.WriteUint64(0);
    int res = toRenderConnectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_STATE);
}

/**
 * @tc.name: DropFrameByPid
 * @tc.desc: Test DropFrameByPid
 * @tc.type: FUNC
 * @tc.require: issueICQ74B
 */
HWTEST_F(RSClientToRenderConnectionStubTest, DropFrameByPid001, TestSize.Level2)
{
    ASSERT_NE(toRenderConnectionStub_, nullptr);
    // MAX_DROP_FRAME_PID_LIST_SIZE = 1024
    std::vector<int32_t> pidList(1025, 1);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::DROP_FRAME_BY_PID);
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data.WriteInt32Vector(pidList);
    int res = toRenderConnectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);
}

/**
 * @tc.name: TakeUICaptureInRangeTest001
 * @tc.desc: Test TakeUICaptureInRange under different node states
 * @tc.type: FUNC
 * @tc.require: issue21623
 */
HWTEST_F(RSClientToRenderConnectionStubTest, TakeUICaptureInRangeTest001, TestSize.Level2)
{
    constexpr uint32_t TIME_OF_CAPTURE_TASK = 100000;
    auto mainThread = RSMainThread::Instance();
    auto runner = mainThread->runner_;
    auto handler = mainThread->handler_;
    mainThread->runner_ = AppExecFwk::EventRunner::Create("TakeUICaptureInRangeTest001");
    ASSERT_NE(mainThread->runner_, nullptr);
    mainThread->handler_ = std::make_shared<AppExecFwk::EventHandler>(mainThread->runner_);
    ASSERT_NE(mainThread->handler_, nullptr);
    mainThread->runner_->Run();

    auto screenManagerPtr = RSScreenManager::GetInstance();
    sptr<RSIConnectionToken> token_ = new IRemoteStub<RSIConnectionToken>();
    sptr<RSClientToRenderConnection> toRenderConnection =
        new RSClientToRenderConnection(getpid(), nullptr, mainThread, screenManagerPtr, token_->AsObject(), nullptr);
    ASSERT_NE(toRenderConnection, nullptr);
    sptr<RSISurfaceCaptureCallback> callback = new RSSurfaceCaptureCallbackStubMock();
    ASSERT_NE(callback, nullptr);
    RSSurfaceCaptureConfig config;
    config.isSync = false;
    RSSurfaceCapturePermissions permissions;
    permissions.isSystemCalling = true;
    auto& nodeMap = mainThread->GetContext().GetMutableNodeMap();
    NodeId id{std::numeric_limits<NodeId>::max()};
    std::shared_ptr<RSRenderNode> node = std::make_shared<RSRenderNode>(id);
    nodeMap.RegisterRenderNode(node);
    node->isOnTheTree_ = true;
    node->dirtyStatus_ = RSRenderNode::NodeDirty::CLEAN;
    node->renderProperties_.isDirty_ = false;
    node->isSubTreeDirty_ = false;
    toRenderConnection->TakeUICaptureInRange(id, callback, config, permissions);
    usleep(TIME_OF_CAPTURE_TASK);
    node->isSubTreeDirty_ = true;
    toRenderConnection->TakeUICaptureInRange(id, callback, config, permissions);
    usleep(TIME_OF_CAPTURE_TASK);
    node->renderProperties_.isDirty_ = true;
    toRenderConnection->TakeUICaptureInRange(id, callback, config, permissions);
    usleep(TIME_OF_CAPTURE_TASK);
    node->isOnTheTree_ = false;
    toRenderConnection->TakeUICaptureInRange(id, callback, config, permissions);
    usleep(TIME_OF_CAPTURE_TASK);
    nodeMap.UnregisterRenderNode(id);
    toRenderConnection->TakeUICaptureInRange(id, callback, config, permissions);
    usleep(TIME_OF_CAPTURE_TASK);
    EXPECT_EQ(nodeMap.GetRenderNode<RSRenderNode>(id), nullptr);

    mainThread->runner_ = runner;
    mainThread->handler_ = handler;
}

/**
 * @tc.name: GetScreenHDRStatus001
 * @tc.desc: Test GetScreenHDRStatus
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToRenderConnectionStubTest, GetScreenHDRStatus001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_HDR_STATUS);
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data.WriteUint64(0);
    int res = toRenderConnectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_OK);
}

/**
 * @tc.name: GetScreenHDRStatus002
 * @tc.desc: Test GetScreenHDRStatus
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToRenderConnectionStubTest, GetScreenHDRStatus002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_HDR_STATUS);
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data.WriteUint64(~0);
    int res = toRenderConnectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_OK);
}

/**
 * @tc.name: GetScreenHDRStatus003
 * @tc.desc: Test GetScreenHDRStatus
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToRenderConnectionStubTest, GetScreenHDRStatus003, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_HDR_STATUS);
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data.WriteBool(false);
    int res = toRenderConnectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);
}

/**
 * @tc.name: TakeSurfaceCaptureWithAllWindowsTest001
 * @tc.desc: Test TakeSurfaceCaptureWithAllWindows for success
 * @tc.type: FUNC
 * @tc.require: issueICQ74B
 */
HWTEST_F(RSClientToRenderConnectionStubTest, TakeSurfaceCaptureWithAllWindowsTest001, TestSize.Level2)
{
    ASSERT_NE(toRenderConnectionStub_, nullptr);

    bool checkDrmAndSurfaceLock = false;
    NodeId displayNodeId = 123;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::TAKE_SURFACE_CAPTURE_WITH_ALL_WINDOWS);
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data.WriteUint64(displayNodeId);
    data.WriteBool(checkDrmAndSurfaceLock);
    sptr<RSISurfaceCaptureCallback> callback = new RSSurfaceCaptureCallbackStubMock();
    ASSERT_EQ(callback != nullptr, true);
    data.WriteRemoteObject(callback->AsObject());
    RSSurfaceCaptureConfig captureConfig;
    data.WriteFloat(captureConfig.scaleX);
    data.WriteFloat(captureConfig.scaleY);
    data.WriteBool(captureConfig.useDma);
    data.WriteBool(captureConfig.useCurWindow);
    data.WriteUint8(static_cast<uint8_t>(captureConfig.captureType));
    data.WriteBool(captureConfig.isSync);
    data.WriteBool(captureConfig.isHdrCapture);
    data.WriteBool(captureConfig.needF16WindowCaptureForScRGB);
    data.WriteBool(captureConfig.needErrorCode);
    data.WriteFloat(captureConfig.mainScreenRect.left_);
    data.WriteFloat(captureConfig.mainScreenRect.top_);
    data.WriteFloat(captureConfig.mainScreenRect.right_);
    data.WriteFloat(captureConfig.mainScreenRect.bottom_);
    data.WriteUInt64Vector(captureConfig.blackList);
    data.WriteUint64(captureConfig.uiCaptureInRangeParam.endNodeId);
    data.WriteBool(captureConfig.uiCaptureInRangeParam.useBeginNodeSize);
    data.WriteFloat(captureConfig.specifiedAreaRect.left_);
    data.WriteFloat(captureConfig.specifiedAreaRect.top_);
    data.WriteFloat(captureConfig.specifiedAreaRect.right_);
    data.WriteFloat(captureConfig.specifiedAreaRect.bottom_);
    data.WriteUint32(captureConfig.backGroundColor);
    data.WriteUint32(captureConfig.colorSpace.first);
    data.WriteBool(captureConfig.colorSpace.second);
    data.WriteUint32(captureConfig.dynamicRangeMode.first);
    data.WriteBool(captureConfig.dynamicRangeMode.second);
    data.WriteBool(captureConfig.isSyncRender);
    auto res = toRenderConnectionStub_->OnRemoteRequest(code, data, reply, option);
#ifdef RS_ENABLE_UNI_RENDER
    EXPECT_LE(res, ERR_PERMISSION_DENIED);
#else
    EXPECT_LE(res, ERR_INVALID_DATA);
#endif
}

/**
 * @tc.name: TakeSurfaceCaptureWithAllWindowsTest002
 * @tc.desc: Test TakeSurfaceCaptureWithAllWindows for task execution
 * @tc.type: FUNC
 * @tc.require: issueICQ74B
 */
HWTEST_F(RSClientToRenderConnectionStubTest, TakeSurfaceCaptureWithAllWindowsTest002, TestSize.Level2)
{
    constexpr uint32_t TIME_OF_CAPTURE_TASK = 100000;
    auto runner = RSMainThread::Instance()->runner_;
    auto handler = RSMainThread::Instance()->handler_;
    RSMainThread::Instance()->runner_ = AppExecFwk::EventRunner::Create("TaskSurfaceCaptureWithAllWindowsTest002");
    ASSERT_NE(RSMainThread::Instance()->runner_, nullptr);
    RSMainThread::Instance()->handler_ = std::make_shared<AppExecFwk::EventHandler>(RSMainThread::Instance()->runner_);
    ASSERT_NE(RSMainThread::Instance()->handler_, nullptr);
    RSMainThread::Instance()->runner_->Run();
    sptr<RSClientToRenderConnection> connection = iface_cast<RSClientToRenderConnection>(toRenderConnectionStub_);
    ASSERT_NE(connection, nullptr);
    auto mainThread = connection->mainThread_;
    connection->mainThread_ = nullptr;
    RSSurfaceCaptureConfig captureConfig;
    RSSurfaceCapturePermissions permissions;
    auto ret = connection->TakeSurfaceCaptureWithAllWindows(0, nullptr, captureConfig, false, permissions);
    EXPECT_EQ(ret, ERR_PERMISSION_DENIED);
    sptr<RSISurfaceCaptureCallback> callback = new RSSurfaceCaptureCallbackStubMock();
    ASSERT_NE(callback, nullptr);
    ret = connection->TakeSurfaceCaptureWithAllWindows(0, callback, captureConfig, false, permissions);
    EXPECT_EQ(ret, ERR_PERMISSION_DENIED);

    ASSERT_NE(mainThread, nullptr);
    connection->mainThread_ = mainThread;
    permissions.screenCapturePermission = false;
    permissions.isSystemCalling = false;
    ret = connection->TakeSurfaceCaptureWithAllWindows(0, nullptr, captureConfig, false, permissions);
    EXPECT_EQ(ret, ERR_PERMISSION_DENIED);
    permissions.screenCapturePermission = true;
    permissions.isSystemCalling = false;
    ret = connection->TakeSurfaceCaptureWithAllWindows(0, callback, captureConfig, false, permissions);
    EXPECT_EQ(ret, ERR_PERMISSION_DENIED);

    permissions.screenCapturePermission = true;
    permissions.isSystemCalling = true;
    ret = connection->TakeSurfaceCaptureWithAllWindows(0, nullptr, captureConfig, false, permissions);
    usleep(TIME_OF_CAPTURE_TASK);
    EXPECT_EQ(ret, ERR_NONE);
    ret = connection->TakeSurfaceCaptureWithAllWindows(0, callback, captureConfig, false, permissions);
    usleep(TIME_OF_CAPTURE_TASK);
    EXPECT_EQ(ret, ERR_NONE);

    ASSERT_NE(connection->mainThread_, nullptr);
    NodeId displayNodeId = 111;
    RSDisplayNodeConfig displayNodeConfig;
    std::shared_ptr<RSLogicalDisplayRenderNode> displayNode = std::shared_ptr<RSLogicalDisplayRenderNode>(
        new RSLogicalDisplayRenderNode(displayNodeId, displayNodeConfig), RSRenderNodeGC::NodeDestructor);
    ASSERT_NE(displayNode, nullptr);
    auto& nodeMap = connection->mainThread_->GetContext().GetMutableNodeMap();
    EXPECT_TRUE(nodeMap.RegisterRenderNode(displayNode));
    ret = connection->TakeSurfaceCaptureWithAllWindows(displayNodeId, nullptr, captureConfig, false, permissions);
    usleep(TIME_OF_CAPTURE_TASK);
    EXPECT_EQ(ret, ERR_NONE);
    ret = connection->TakeSurfaceCaptureWithAllWindows(displayNodeId, callback, captureConfig, false, permissions);
    usleep(TIME_OF_CAPTURE_TASK);
    EXPECT_EQ(ret, ERR_NONE);
    ret = connection->TakeSurfaceCaptureWithAllWindows(displayNodeId, callback, captureConfig, false, permissions);
    usleep(TIME_OF_CAPTURE_TASK);
    EXPECT_EQ(ret, ERR_NONE);

    RSMainThread::Instance()->runner_ = runner;
    RSMainThread::Instance()->handler_ = handler;
}

/**
 * @tc.name: TakeSurfaceCaptureWithAllWindowsTest003
 * @tc.desc: Test TakeSurfaceCaptureWithAllWindows for failures
 * @tc.type: FUNC
 * @tc.require: issueICQ74B
 */
HWTEST_F(RSClientToRenderConnectionStubTest, TakeSurfaceCaptureWithAllWindowsTest003, TestSize.Level2)
{
    ASSERT_NE(toRenderConnectionStub_, nullptr);
    MessageParcel data1;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::TAKE_SURFACE_CAPTURE_WITH_ALL_WINDOWS);
    data1.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data1.WriteBool(false);
    int res = toRenderConnectionStub_->OnRemoteRequest(code, data1, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);

    MessageParcel data2;
    data2.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data2.WriteUint64(1);
    res = toRenderConnectionStub_->OnRemoteRequest(code, data2, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);

    MessageParcel data3;
    data3.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data3.WriteUint64(1);
    data3.WriteBool(true);
    res = toRenderConnectionStub_->OnRemoteRequest(code, data3, reply, option);
    ASSERT_EQ(res, ERR_NULL_OBJECT);

    MessageParcel data4;
    data4.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data4.WriteUint64(1);
    data4.WriteBool(true);
    sptr<RSScreenChangeCallbackStubMock> callback1 = new RSScreenChangeCallbackStubMock();
    ASSERT_NE(callback1, nullptr);
    data4.WriteRemoteObject(callback1->AsObject());
    res = toRenderConnectionStub_->OnRemoteRequest(code, data4, reply, option);
    ASSERT_EQ(res, ERR_NULL_OBJECT);

    MessageParcel data5;
    data5.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data5.WriteUint64(1);
    data5.WriteBool(true);
    sptr<RSISurfaceCaptureCallback> callback2 = new RSSurfaceCaptureCallbackStubMock();
    ASSERT_NE(callback2, nullptr);
    data5.WriteRemoteObject(callback2->AsObject());
    data5.WriteBool(false);
    res = toRenderConnectionStub_->OnRemoteRequest(code, data5, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);
}

/**
 * @tc.name: FreezeScreenTest001
 * @tc.desc: Test FreezeScreen to freeze or unfreeze screen
 * @tc.type: FUNC
 * @tc.require: issueICS2J8
 */
HWTEST_F(RSClientToRenderConnectionStubTest, FreezeScreenTest001, TestSize.Level2)
{
    constexpr uint32_t TIME_OF_FREEZE_TASK = 100000;
    ASSERT_NE(toRenderConnectionStub_, nullptr);
    MessageParcel data1;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::FREEZE_SCREEN);
    data1.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data1.WriteBool(false);
    int res = toRenderConnectionStub_->OnRemoteRequest(code, data1, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);

    MessageParcel data2;
    data2.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data2.WriteUint64(1);
    res = toRenderConnectionStub_->OnRemoteRequest(code, data2, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);

    MessageParcel data3;
    data3.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data3.WriteUint64(1);
    data3.WriteBool(true);
    data3.WriteBool(false);
    res = toRenderConnectionStub_->OnRemoteRequest(code, data3, reply, option);
    ASSERT_EQ(res, ERR_OK);

    auto runner = RSMainThread::Instance()->runner_;
    auto handler = RSMainThread::Instance()->handler_;
    RSMainThread::Instance()->runner_ = AppExecFwk::EventRunner::Create("TaskSurfaceCaptureWithAllWindowsTest002");
    ASSERT_NE(RSMainThread::Instance()->runner_, nullptr);
    RSMainThread::Instance()->handler_ = std::make_shared<AppExecFwk::EventHandler>(RSMainThread::Instance()->runner_);
    ASSERT_NE(RSMainThread::Instance()->handler_, nullptr);
    RSMainThread::Instance()->runner_->Run();
    sptr<RSClientToRenderConnection> connection = iface_cast<RSClientToRenderConnection>(toRenderConnectionStub_);
    ASSERT_NE(connection, nullptr);
    auto mainThread = connection->mainThread_;
    connection->mainThread_ = nullptr;
    res = connection->FreezeScreen(0, false);
    ASSERT_EQ(res, ERR_INVALID_OPERATION);

    ASSERT_NE(mainThread, nullptr);
    connection->mainThread_ = mainThread;
    res = connection->FreezeScreen(0, false, false);
    usleep(TIME_OF_FREEZE_TASK);
    ASSERT_EQ(res, ERR_OK);

    NodeId displayNodeId = 1123;
    NodeId screenNodeId = 4456;
    ScreenId screenId = 1;
    RSDisplayNodeConfig displayNodeConfig;
    std::shared_ptr<RSLogicalDisplayRenderNode> displayNode = std::shared_ptr<RSLogicalDisplayRenderNode>(
        new RSLogicalDisplayRenderNode(displayNodeId, displayNodeConfig), RSRenderNodeGC::NodeDestructor);
    ASSERT_NE(displayNode, nullptr);
    std::shared_ptr<RSScreenRenderNode> screenNode = std::shared_ptr<RSScreenRenderNode>(
        new RSScreenRenderNode(screenNodeId, screenId), RSRenderNodeGC::NodeDestructor);
    ASSERT_NE(screenNode, nullptr);
    displayNode->SetParent(screenNode);
    auto& nodeMap = connection->mainThread_->GetContext().GetMutableNodeMap();
    EXPECT_TRUE(nodeMap.RegisterRenderNode(displayNode));
    EXPECT_TRUE(nodeMap.RegisterRenderNode(screenNode));
    res = connection->FreezeScreen(displayNodeId, false);
    usleep(TIME_OF_FREEZE_TASK);
    ASSERT_EQ(res, ERR_OK);

    RSMainThread::Instance()->runner_ = runner;
    RSMainThread::Instance()->handler_ = handler;
}

/**
 * @tc.name: FreezeScreenTest002
 * @tc.desc: Test FreezeScreen to freeze or unfreeze screen
 * @tc.type: FUNC
 * @tc.require: issueICUQ08
 */
HWTEST_F(RSClientToRenderConnectionStubTest, FreezeScreenTest002, TestSize.Level2)
{
    constexpr uint32_t TIME_OF_FREEZE_TASK = 100000;
    ASSERT_NE(toRenderConnectionStub_, nullptr);
    auto runner = RSMainThread::Instance()->runner_;
    auto handler = RSMainThread::Instance()->handler_;
    RSMainThread::Instance()->runner_ = AppExecFwk::EventRunner::Create("FreezeScreenTest002");
    ASSERT_NE(RSMainThread::Instance()->runner_, nullptr);
    RSMainThread::Instance()->handler_ = std::make_shared<AppExecFwk::EventHandler>(RSMainThread::Instance()->runner_);
    ASSERT_NE(RSMainThread::Instance()->handler_, nullptr);
    RSMainThread::Instance()->runner_->Run();
    sptr<RSClientToRenderConnection> connection = iface_cast<RSClientToRenderConnection>(toRenderConnectionStub_);
    ASSERT_NE(connection, nullptr);
    NodeId displayNodeId = 896;
    NodeId screenNodeId = 54;
    ScreenId screenId = 1;
    RSDisplayNodeConfig displayNodeConfig;
    std::shared_ptr<RSLogicalDisplayRenderNode> displayNode = std::shared_ptr<RSLogicalDisplayRenderNode>(
        new RSLogicalDisplayRenderNode(displayNodeId, displayNodeConfig), RSRenderNodeGC::NodeDestructor);
    ASSERT_NE(displayNode, nullptr);
    std::shared_ptr<RSScreenRenderNode> screenNode = std::shared_ptr<RSScreenRenderNode>(
        new RSScreenRenderNode(screenNodeId, screenId), RSRenderNodeGC::NodeDestructor);
    ASSERT_NE(screenNode, nullptr);
    displayNode->SetParent(screenNode);
    auto& nodeMap = connection->mainThread_->GetContext().GetMutableNodeMap();
    EXPECT_TRUE(nodeMap.RegisterRenderNode(displayNode));
    EXPECT_TRUE(nodeMap.RegisterRenderNode(screenNode));
    auto res = connection->FreezeScreen(displayNodeId, false);
    usleep(TIME_OF_FREEZE_TASK);
    ASSERT_EQ(res, ERR_OK);
    res = connection->FreezeScreen(displayNodeId, true);
    usleep(TIME_OF_FREEZE_TASK);
    ASSERT_EQ(res, ERR_OK);

    RSMainThread::Instance()->runner_ = runner;
    RSMainThread::Instance()->handler_ = handler;
}

/**
 * @tc.name: FreezeScreenTest003
 * @tc.desc: Test FreezeScreen sync
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToRenderConnectionStubTest, FreezeScreenTest003, TestSize.Level2)
{
    ASSERT_NE(toRenderConnectionStub_, nullptr);
    MessageParcel data1;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::FREEZE_SCREEN);
    data1.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data1.WriteUint64(1);
    data1.WriteBool(true);
    int res = toRenderConnectionStub_->OnRemoteRequest(code, data1, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);

    MessageParcel data2;
    data2.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data2.WriteUint64(1);
    data2.WriteBool(true);
    data2.WriteBool(true);
    res = toRenderConnectionStub_->OnRemoteRequest(code, data2, reply, option);
    ASSERT_EQ(res, ERR_OK);
}

/**
 * @tc.name: TakeSurfaceCaptureWithAllWindowsTest004
 * @tc.desc: Test TakeSurfaceCaptureWithAllWindows for drm/surfacelock
 * @tc.type: FUNC
 * @tc.require: issueICUQ08
 */
HWTEST_F(RSClientToRenderConnectionStubTest, TakeSurfaceCaptureWithAllWindowsTest004, TestSize.Level2)
{
    constexpr uint32_t TIME_OF_CAPTURE_TASK = 100000;
    auto runner = RSMainThread::Instance()->runner_;
    auto handler = RSMainThread::Instance()->handler_;
    RSMainThread::Instance()->runner_ = AppExecFwk::EventRunner::Create("TaskSurfaceCaptureWithAllWindowsTest004");
    ASSERT_NE(RSMainThread::Instance()->runner_, nullptr);
    RSMainThread::Instance()->handler_ = std::make_shared<AppExecFwk::EventHandler>(RSMainThread::Instance()->runner_);
    ASSERT_NE(RSMainThread::Instance()->handler_, nullptr);
    RSMainThread::Instance()->runner_->Run();
    sptr<RSClientToRenderConnection> connection = iface_cast<RSClientToRenderConnection>(toRenderConnectionStub_);
    ASSERT_NE(connection, nullptr);

    RSSurfaceCaptureConfig captureConfig;
    RSSurfaceCapturePermissions permissions;
    permissions.screenCapturePermission = true;
    permissions.isSystemCalling = true;
    NodeId displayNodeId = 45;
    RSDisplayNodeConfig displayNodeConfig;
    std::shared_ptr<RSLogicalDisplayRenderNode> displayNode = std::shared_ptr<RSLogicalDisplayRenderNode>(
        new RSLogicalDisplayRenderNode(displayNodeId, displayNodeConfig), RSRenderNodeGC::NodeDestructor);
    ASSERT_NE(displayNode, nullptr);
    auto& nodeMap = connection->mainThread_->GetContext().GetMutableNodeMap();
    EXPECT_TRUE(nodeMap.RegisterRenderNode(displayNode));
    auto ret = connection->TakeSurfaceCaptureWithAllWindows(displayNodeId, nullptr, captureConfig, false, permissions);
    usleep(TIME_OF_CAPTURE_TASK);
    EXPECT_EQ(ret, ERR_NONE);
    RSMainThread::Instance()->SetHasSurfaceLockLayer(false);
    RSMainThread::Instance()->hasProtectedLayer_ = false;
    RSMainThread::Instance()->hasSurfaceLockLayer_ = false;
    EXPECT_FALSE(RSMainThread::Instance()->HasDRMOrSurfaceLockLayer());
    ret = connection->TakeSurfaceCaptureWithAllWindows(displayNodeId, nullptr, captureConfig, true, permissions);
    usleep(TIME_OF_CAPTURE_TASK);
    EXPECT_EQ(ret, ERR_NONE);
    RSMainThread::Instance()->hasProtectedLayer_ = true;
    RSMainThread::Instance()->hasSurfaceLockLayer_ = true;
    ret = connection->TakeSurfaceCaptureWithAllWindows(displayNodeId, nullptr, captureConfig, true, permissions);
    usleep(TIME_OF_CAPTURE_TASK);
    EXPECT_EQ(ret, ERR_NONE);
    sptr<RSISurfaceCaptureCallback> callback = new RSSurfaceCaptureCallbackStubMock();
    ret = connection->TakeSurfaceCaptureWithAllWindows(displayNodeId, callback, captureConfig, true, permissions);
    usleep(TIME_OF_CAPTURE_TASK);
    EXPECT_EQ(ret, ERR_NONE);

    RSMainThread::Instance()->runner_ = runner;
    RSMainThread::Instance()->handler_ = handler;
}

/**
 * @tc.name: TestRSClientToRenderConnectionStub003
 * @tc.desc: Test callback is null
 * @tc.type: FUNC
 * @tc.require: issueI60KUK
 */
HWTEST_F(RSClientToRenderConnectionStubTest, TestRSClientToRenderConnectionStub003, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_TRANSACTION_DATA_CALLBACK);
    data.WriteUint64(123);
    data.WriteUint64(456);
    int res = toRenderConnectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_NULL_OBJECT);
}

class RSTransactionDataCallbackStubMock : public RSTransactionDataCallbackStub {
public:
    RSTransactionDataCallbackStubMock() = default;
    virtual ~RSTransactionDataCallbackStubMock() = default;
    void OnAfterProcess(uint64_t token, uint64_t timeStamp) override {};
};

/**
 * @tc.name: TestRSClientToRenderConnectionStub004
 * @tc.desc: Test callback exit
 * @tc.type: FUNC
 * @tc.require: issueI60KUK
 */
HWTEST_F(RSClientToRenderConnectionStubTest, TestRSClientToRenderConnectionStub004, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_TRANSACTION_DATA_CALLBACK);
    data.WriteUint64(123);
    data.WriteUint64(456);
    sptr<RSTransactionDataCallbackStubMock> callback = new RSTransactionDataCallbackStubMock();
    data.WriteRemoteObject(callback->AsObject());
    int res = toRenderConnectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, NO_ERROR);
}

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
/**
 * @tc.name: RegisterCanvasCallbackTest
 * @tc.desc: Test REGISTER_CANVAS_CALLBACK with various scenarios
 * @tc.type: FUNC
 * @tc.require: issue
 */
HWTEST_F(RSClientToRenderConnectionStubTest, RegisterCanvasCallbackTest, TestSize.Level1)
{
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_CANVAS_CALLBACK);

    MessageParcel data1;
    data1.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    int res = toRenderConnectionStub_->OnRemoteRequest(code, data1, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);

    MessageParcel data2;
    data2.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data2.WriteBool(false);
    res = toRenderConnectionStub_->OnRemoteRequest(code, data2, reply, option);
    ASSERT_EQ(res, ERR_NONE);

    MessageParcel data3;
    data3.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data3.WriteBool(true);
    sptr<RSICanvasSurfaceBufferCallback> callback = new RSCanvasSurfaceBufferCallbackStubMock();
    data3.WriteRemoteObject(callback->AsObject());
    res = toRenderConnectionStub_->OnRemoteRequest(code, data3, reply, option);
    ASSERT_EQ(res, ERR_NONE);

    MessageParcel data4;
    data4.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data4.WriteBool(true);
    sptr<RSCanvasSurfaceBufferCallbackStubMock> bufferCallback = new RSCanvasSurfaceBufferCallbackStubMock();
    bufferCallback->isProxyObject_ = false;
    callback = bufferCallback;
    data4.WriteRemoteObject(callback->AsObject());
    res = toRenderConnectionStub_->OnRemoteRequest(code, data4, reply, option);
    ASSERT_EQ(res, ERR_UNKNOWN_OBJECT);

    MessageParcel data5;
    data5.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data5.WriteBool(true);
    res = toRenderConnectionStub_->OnRemoteRequest(code, data5, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);

    NodeMemReleaseParam::SetCanvasDrawingNodeDMAMemEnabled(false);
    MessageParcel data6;
    data6.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data6.WriteBool(true);
    callback = new RSCanvasSurfaceBufferCallbackStubMock();
    data6.WriteRemoteObject(callback->AsObject());
    res = toRenderConnectionStub_->OnRemoteRequest(code, data6, reply, option);
    NodeMemReleaseParam::SetCanvasDrawingNodeDMAMemEnabled(true);
    ASSERT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: SubmitCanvasPreAllocatedBufferTest001
 * @tc.desc: Test SUBMIT_CANVAS_PRE_ALLOCATED_BUFFER with various scenarios
 * @tc.type: FUNC
 * @tc.require: issue
 */
HWTEST_F(RSClientToRenderConnectionStubTest, SubmitCanvasPreAllocatedBufferTest001, TestSize.Level1)
{
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SUBMIT_CANVAS_PRE_ALLOCATED_BUFFER);

    MessageParcel data1;
    data1.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    int res = toRenderConnectionStub_->OnRemoteRequest(code, data1, reply, option);
    ASSERT_NE(res, ERR_NONE);

    MessageParcel data2;
    data2.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data2.WriteUint64(1);
    res = toRenderConnectionStub_->OnRemoteRequest(code, data2, reply, option);
    ASSERT_NE(res, ERR_NONE);

    RSMainThread::Instance()->runner_ = AppExecFwk::EventRunner::Create("SubmitCanvasPreAllocatedBuffer001");
    ASSERT_NE(RSMainThread::Instance()->runner_, nullptr);
    RSMainThread::Instance()->handler_ = std::make_shared<AppExecFwk::EventHandler>(RSMainThread::Instance()->runner_);
    ASSERT_NE(RSMainThread::Instance()->handler_, nullptr);
    RSMainThread::Instance()->runner_->Run();
    MessageParcel data3;
    data3.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data3.WriteUint64(1); // Write nodeId
    data3.WriteUint32(1); // Write resetSurfaceIndex
    data3.WriteUint32(1); // Write sequence
    data3.WriteBool(false); // Whether has buffer
    res = toRenderConnectionStub_->OnRemoteRequest(code, data3, reply, option);
    ASSERT_EQ(res, ERR_NONE);

    MessageParcel data4;
    data4.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data4.WriteUint64(1); // Write nodeId
    data4.WriteUint32(1); // Write resetSurfaceIndex
    data4.WriteUint32(1); // Write sequence
    data4.WriteBool(true); // Whether has buffer
    res = toRenderConnectionStub_->OnRemoteRequest(code, data4, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);

    MessageParcel data5;
    data5.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data5.WriteUint64(3); // Write nodeId
    data5.WriteUint32(3); // Write resetSurfaceIndex
    data5.WriteUint32(1); // Write sequence
    data5.WriteBool(true); // Whether has buffer
    sptr<SurfaceBuffer> buffer = SurfaceBufferUtils::CreateCanvasSurfaceBuffer(1, 100, 100);
    buffer->WriteToMessageParcel(data5);
    res = toRenderConnectionStub_->OnRemoteRequest(code, data5, reply, option);
    ASSERT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: SubmitCanvasPreAllocatedBufferTest002
 * @tc.desc: Test func SubmitCanvasPreAllocatedBuffer
 * @tc.type: FUNC
 * @tc.require: issue
 */
HWTEST_F(RSClientToRenderConnectionStubTest, SubmitCanvasPreAllocatedBufferTest002, TestSize.Level1)
{
    NodeMemReleaseParam::SetCanvasDrawingNodeDMAMemEnabled(false);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SUBMIT_CANVAS_PRE_ALLOCATED_BUFFER);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data.WriteUint64(3); // Write nodeId
    data.WriteUint32(3); // Write resetSurfaceIndex
    data.WriteUint32(1); // Write sequence
    data.WriteBool(true); // Whether has buffer
    auto buffer = SurfaceBufferUtils::CreateCanvasSurfaceBuffer(1, 100, 100);
    buffer->WriteToMessageParcel(data);
    auto ret = toRenderConnectionStub_->OnRemoteRequest(code, data, reply, option);
    NodeMemReleaseParam::SetCanvasDrawingNodeDMAMemEnabled(true);
    ASSERT_EQ(ret, 0);

    auto newPid = getpid();
    auto screenManagerPtr = RSScreenManager::GetInstance();
    auto mainThread = RSMainThread::Instance();
    sptr<RSIConnectionToken> token_ = new IRemoteStub<RSIConnectionToken>();
    sptr<RSClientToRenderConnection> toRenderConnection =
        new RSClientToRenderConnection(newPid, nullptr, mainThread, screenManagerPtr, token_->AsObject(), nullptr);
    ASSERT_EQ(toRenderConnection != nullptr, true);
    toRenderConnection->mainThread_ = nullptr;
    buffer = SurfaceBuffer::Create();
    ret = toRenderConnection->SubmitCanvasPreAllocatedBuffer(1, buffer, 1);
    ASSERT_NE(ret, 0);
    toRenderConnection->mainThread_ = mainThread;
    toRenderConnection->remotePid_ = 1;
    ret = toRenderConnection->SubmitCanvasPreAllocatedBuffer(1, buffer, 1);
    ASSERT_NE(ret, 0);

    RSMainThread::Instance()->runner_ = AppExecFwk::EventRunner::Create("SubmitCanvasPreAllocatedBuffer002");
    ASSERT_NE(RSMainThread::Instance()->runner_, nullptr);
    RSMainThread::Instance()->handler_ = std::make_shared<AppExecFwk::EventHandler>(RSMainThread::Instance()->runner_);
    ASSERT_NE(RSMainThread::Instance()->handler_, nullptr);
    RSMainThread::Instance()->runner_->Run();
    sptr<RSClientToRenderConnection> connection = iface_cast<RSClientToRenderConnection>(toRenderConnectionStub_);
    ASSERT_NE(connection, nullptr);
    connection->remotePid_ = 0;
    ret = connection->SubmitCanvasPreAllocatedBuffer(1, buffer, 2);
    ASSERT_EQ(ret, 0);
    ret = connection->SubmitCanvasPreAllocatedBuffer(1, buffer, 2);
    ASSERT_NE(ret, 0);
}
#endif

/**
 * @tc.name: SetLogicalCameraRotationCorrection001
 * @tc.desc: Test SetLogicalCameraRotationCorrection
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToRenderConnectionStubTest, SetLogicalCameraRotationCorrection001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code =
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_LOGICAL_CAMERA_ROTATION_CORRECTION);
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data.WriteUint64(1);
    data.WriteUint32(static_cast<uint32_t>(ScreenRotation::ROTATION_90));
    int res = toRenderConnectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_OK);
}

/**
 * @tc.name: SetLogicalCameraRotationCorrection002
 * @tc.desc: Test SetLogicalCameraRotationCorrection
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToRenderConnectionStubTest, SetLogicalCameraRotationCorrection002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code =
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_LOGICAL_CAMERA_ROTATION_CORRECTION);
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data.WriteUint64(1);
    data.WriteUint32(6);
    int res = toRenderConnectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);
}

/**
 * @tc.name: SetLogicalCameraRotationCorrection003
 * @tc.desc: Test SetLogicalCameraRotationCorrection when ReadUint64 failed
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToRenderConnectionStubTest, SetLogicalCameraRotationCorrection003, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code =
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_LOGICAL_CAMERA_ROTATION_CORRECTION);
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data.WriteString("str");
    int res = toRenderConnectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);
}

/**
 * @tc.name: SetLogicalCameraRotationCorrection004
 * @tc.desc: Test SetLogicalCameraRotationCorrection when screenRotation is invalid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToRenderConnectionStubTest, SetLogicalCameraRotationCorrection004, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code =
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_LOGICAL_CAMERA_ROTATION_CORRECTION);
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data.WriteUint64(0);
    int res = toRenderConnectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);
}

/**
 * @tc.name: SetHwcNodeBounds
 * @tc.desc: Test SetHwcNodeBounds when isPointerEnableHwc_ is true or false
 * @tc.type: FUNC
 * @tc.require: issue22079
 */
HWTEST_F(RSClientToRenderConnectionStubTest, SetHwcNodeBounds, TestSize.Level1)
{
    ASSERT_NE(toRenderConnectionStub_, nullptr);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_POINTER_POSITION);
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data.WriteUint64(0);
    data.WriteFloat(0);
    int res = toRenderConnectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_NONE);
    auto& rsPointerWindowManager = RSPointerWindowManager::Instance();
    rsPointerWindowManager.SetIsPointerEnableHwc(false);
    res = toRenderConnectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_NONE);
    rsPointerWindowManager.SetIsPointerEnableHwc(true);
}
} // namespace OHOS::Rosen
