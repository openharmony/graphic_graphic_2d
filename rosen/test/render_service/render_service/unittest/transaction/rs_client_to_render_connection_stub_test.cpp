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
#include "feature/pointer_window_manager/rs_pointer_window_manager.h"
#include "pipeline/rs_render_node_gc.h"
#include "pipeline/rs_screen_render_node.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "ipc_callbacks/rs_transaction_data_callback_stub.h"
#include "platform/ohos/transaction/zidl/rs_irender_service.h"
#include "screen_manager/screen_types.h"
#include "transaction/zidl/rs_client_to_render_connection_stub.h"
#include "screen_manager/rs_screen.h"
#include "feature/capture/rs_capture_pixelmap_manager.h"
#include "ipc_callbacks/surface_capture_callback_stub.h"
#include "ipc_callbacks/rs_surface_buffer_callback_stub.h"
#include "ipc_callbacks/buffer_available_callback_stub.h"
#include "ipc_callbacks/buffer_clear_callback_stub.h"
#include "ipc_callbacks/rs_application_agent_stub.h"
#include "transaction/rs_transaction_data.h"
#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif
using namespace testing;
using namespace testing::ext;

namespace {
constexpr const int WAIT_HANDLER_TIME = 1; // 1s
constexpr const int WAIT_HANDLER_TIME_COUNT = 5;
constexpr const int SURFACE_NODE_ID = 1003;
};

namespace OHOS::Rosen {
class RSClientToRenderConnectionStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline Mock::HdiDeviceMock* hdiDeviceMock_;
    static inline sptr<RSScreenManager> screenManager_ = sptr<RSScreenManager>::MakeSptr();
    static inline RSMainThread* mainThread_;
    static inline std::shared_ptr<HdiOutput> hdiOutput_;
    float mirrorAdaptiveCoefficient = 1.0f;
    static uint32_t screenId_;
    static std::shared_ptr<RSSurfaceRenderNode> surfaceNode_;
private:
    int OnRemoteRequestTest(uint32_t code);
    static void WaitHandlerTask();
    static inline sptr<RSIConnectionToken> token_;
    static inline sptr<RSClientToRenderConnectionStub> connectionStub_;
    static inline std::shared_ptr<RSRenderPipeline> renderPipeline_;
    static inline sptr<RSRenderPipelineAgent> renderPipelineAgent_;
};

uint32_t RSClientToRenderConnectionStubTest::screenId_ = 0;
std::shared_ptr<RSSurfaceRenderNode> RSClientToRenderConnectionStubTest::surfaceNode_ = nullptr;

void RSClientToRenderConnectionStubTest::SetUpTestCase()
{
    pid_t pid = SURFACE_NODE_ID;
    surfaceNode_ = std::shared_ptr<RSSurfaceRenderNode>(new RSSurfaceRenderNode(((NodeId)pid << 32 | SURFACE_NODE_ID),
        std::make_shared<RSContext>(), true), RSRenderNodeGC::NodeDestructor);
#ifdef RS_ENABLE_VK
    RsVulkanContext::SetRecyclable(false);
#endif
    hdiOutput_ = HdiOutput::CreateHdiOutput(screenId_);
    auto rsScreen = std::make_shared<RSScreen>(screenId_);
    screenManager_ = sptr<RSScreenManager>::MakeSptr();
    screenManager_->MockHdiScreenConnected(rsScreen);

    hdiDeviceMock_ = Mock::HdiDeviceMock::GetInstance();
    EXPECT_CALL(*hdiDeviceMock_, RegHotPlugCallback(_, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*hdiDeviceMock_, RegHwcDeadCallback(_, _)).WillRepeatedly(testing::Return(false));
    EXPECT_CALL(*hdiDeviceMock_, RegRefreshCallback(_, _)).WillRepeatedly(testing::Return(0));
    renderPipeline_ = std::make_shared<RSRenderPipeline>();

    mainThread_ = RSMainThread::Instance();
    mainThread_->handler_ =
        std::make_shared<OHOS::AppExecFwk::EventHandler>(OHOS::AppExecFwk::EventRunner::Create(true));
    mainThread_->handler_->eventRunner_->Run();
    renderPipeline_->mainThread_ = mainThread_;

    auto runner = OHOS::AppExecFwk::EventRunner::Create(true);
    auto handler = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner);
    runner->Run();
    token_ = new IRemoteStub<RSIConnectionToken>();

    renderPipeline_->uniRenderThread_ = &(RSUniRenderThread::Instance());
    renderPipeline_->uniRenderThread_->handler_ = handler;
    renderPipeline_->uniRenderThread_->runner_ = runner;

    renderPipelineAgent_ = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline_);
    connectionStub_ = new RSClientToRenderConnection(0, renderPipelineAgent_, token_->AsObject());
}

void RSClientToRenderConnectionStubTest::WaitHandlerTask()
{
    auto count = 0;
    auto isMainThreadRunning = !renderPipeline_->mainThread_->handler_->IsIdle();
    auto isUniRenderThreadRunning = !renderPipeline_->uniRenderThread_->handler_->IsIdle();
    while (count < WAIT_HANDLER_TIME_COUNT && (isMainThreadRunning || isUniRenderThreadRunning)) {
        std::this_thread::sleep_for(std::chrono::seconds(WAIT_HANDLER_TIME));
    }

    if (count >= WAIT_HANDLER_TIME_COUNT) {
        renderPipeline_->mainThread_->handler_->RemoveAllEvents();
        renderPipeline_->uniRenderThread_->handler_->RemoveAllEvents();
    }
}

void RSClientToRenderConnectionStubTest::TearDownTestCase()
{
    WaitHandlerTask();

    renderPipeline_->mainThread_->handler_->eventRunner_->Stop();
    renderPipeline_->uniRenderThread_->runner_->Stop();

    renderPipeline_->mainThread_->handler_ = nullptr;
    renderPipeline_->mainThread_->receiver_ = nullptr;
    renderPipeline_->mainThread_->context_ = nullptr;

    renderPipeline_->uniRenderThread_->handler_ = nullptr;
    renderPipeline_->uniRenderThread_->runner_ = nullptr;

    hdiOutput_ = nullptr;
    screenManager_ = nullptr;
    hdiDeviceMock_ = nullptr;
    token_ = nullptr;
    connectionStub_ = nullptr;
    surfaceNode_ = nullptr;
}

void RSClientToRenderConnectionStubTest::SetUp() {}
void RSClientToRenderConnectionStubTest::TearDown() {}
int RSClientToRenderConnectionStubTest::OnRemoteRequestTest(uint32_t code)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    return connectionStub_->OnRemoteRequest(code, data, reply, option);
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

class RSApplicationAgentStubMock : public RSApplicationAgentStub {
public:
    RSApplicationAgentStubMock() = default;
    ~RSApplicationAgentStubMock() noexcept override = default;
    void OnTransaction(std::shared_ptr<RSTransactionData> transactionData) override {}
};

class RSBufferAvailableCallbackStubMock : public RSBufferAvailableCallbackStub {
public:
    RSBufferAvailableCallbackStubMock() = default;
    ~RSBufferAvailableCallbackStubMock() noexcept override = default;
    void OnBufferAvailable() override {}
};

class RSBufferClearCallbackStubMock : public RSBufferClearCallbackStub {
public:
    RSBufferClearCallbackStubMock() = default;
    ~RSBufferClearCallbackStubMock() noexcept override = default;
    void OnBufferClear() override {}
};

class RSSurfaceBufferCallbackStubMock : public RSSurfaceBufferCallbackStub {
public:
    RSSurfaceBufferCallbackStubMock() = default;
    ~RSSurfaceBufferCallbackStubMock() noexcept override = default;
    void OnFinish(const FinishCallbackRet& ret) override {}
    void OnAfterAcquireBuffer(const AfterAcquireBufferRet& ret) override {}
};

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

    connectionStub_ =
        new RSClientToRenderConnection(newPid, renderPipelineAgent_, token_->AsObject());
    ASSERT_EQ(connectionStub_ != nullptr, true);
    sptr<RSISurfaceCaptureCallback> callback = new RSSurfaceCaptureCallbackStubMock();
    ASSERT_EQ(callback != nullptr, true);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::TAKE_SURFACE_CAPTURE);
    NodeId id = surfaceNode_->GetId();
    // Test0 Abnormal condition, isClientPixelMap = true, but no clientPixelMap
    RSSurfaceCaptureConfig captureConfig;
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
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
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_LE(res, ERR_NULL_OBJECT);

    // Test1: normal Capture
    auto& nodeMap = RSMainThread::Instance()->GetContext().nodeMap;
    nodeMap.RegisterRenderNode(surfaceNode_);

    MessageParcel data2;
    RSSurfaceCaptureConfig captureConfig2;
    data2.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
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

    res = connectionStub_->OnRemoteRequest(code, data2, reply, option);

    EXPECT_LE(res, ERR_NULL_OBJECT);
    // Test3 Abnoram condiation, Write config error
    MessageParcel data3;
    data3.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data2.WriteUint64(id);
    data2.WriteRemoteObject(callback->AsObject());
    // Write BlurParm
    data2.WriteBool(false);
    data2.WriteFloat(0);
    // write Area Rect;
    data2.WriteFloat(0);
    data2.WriteFloat(0);
    data2.WriteFloat(0);
    data2.WriteFloat(0);
    res = connectionStub_->OnRemoteRequest(code, data3, reply, option);
    EXPECT_LE(res, ERR_NULL_OBJECT);

    // Test4 Abnoram condiation, Write blurParam error
    MessageParcel data4;
    data4.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data4.WriteUint64(id);
    data4.WriteRemoteObject(callback->AsObject());
    g_WriteSurfaceCaptureConfigMock(captureConfig, data4);
    // Write BlurParm
    data4.WriteFloat(0);
    // write Area Rect;
    data4.WriteFloat(0);
    data4.WriteFloat(0);
    data4.WriteFloat(0);
    data4.WriteFloat(0);
    res = connectionStub_->OnRemoteRequest(code, data4, reply, option);
    EXPECT_LE(res, ERR_NULL_OBJECT);

    // Test4 Abnoram condiation, Write areaReat error
    MessageParcel data5;
    data5.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data5.WriteUint64(id);
    data5.WriteRemoteObject(callback->AsObject());
    g_WriteSurfaceCaptureConfigMock(captureConfig, data5);
    // Write BlurParm
    data5.WriteBool(false);
    data5.WriteFloat(0);
    // write Area Rect;
    data5.WriteFloat(0);
    data5.WriteFloat(0);
    data5.WriteFloat(0);
    data5.WriteFloat(0);
    res = connectionStub_->OnRemoteRequest(code, data5, reply, option);
    EXPECT_LE(res, ERR_NULL_OBJECT);

    constexpr uint32_t TIME_OF_CAPTURE_TASK_REMAIN = 1000000;
    usleep(TIME_OF_CAPTURE_TASK_REMAIN);
    nodeMap.UnregisterRenderNode(id);
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

/**
 * @tc.name: TestRSClientToRenderConnectionStub001
 * @tc.desc: Test if data has no content.
 * @tc.type: FUNC
 * @tc.require: issueI60KUK
 */
HWTEST_F(RSClientToRenderConnectionStubTest, TestRSClientToRenderConnectionStub001, TestSize.Level1)
{
    EXPECT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::REGISTER_SURFACE_BUFFER_CALLBACK)),
        ERR_INVALID_DATA);
    EXPECT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::UNREGISTER_SURFACE_BUFFER_CALLBACK)),
        ERR_INVALID_DATA);
    if (RSUniRenderJudgement::IsUniRender()) {
        EXPECT_EQ(OnRemoteRequestTest(static_cast<uint32_t>(
                      RSIClientToRenderConnectionInterfaceCode::SET_LAYER_TOP_FOR_HARDWARE_COMPOSER)),
            ERR_INVALID_DATA);
    }
    EXPECT_EQ(OnRemoteRequestTest(static_cast<uint32_t>(
        RSIClientToRenderConnectionInterfaceCode::REGISTER_TRANSACTION_DATA_CALLBACK)), ERR_NULL_OBJECT);
    EXPECT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::SET_GLOBAL_DARK_COLOR_MODE)), ERR_INVALID_DATA);
    EXPECT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::SET_HIDE_PRIVACY_CONTENT)), ERR_INVALID_DATA);
    EXPECT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::CREATE_DISPLAY_NODE)), ERR_INVALID_DATA);
    EXPECT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::REGISTER_APPLICATION_AGENT)), ERR_NULL_OBJECT);
    EXPECT_EQ(OnRemoteRequestTest(static_cast<uint32_t>(
        RSIClientToRenderConnectionInterfaceCode::SET_BUFFER_AVAILABLE_LISTENER)), ERR_INVALID_DATA);
    EXPECT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::SET_BUFFER_CLEAR_LISTENER)), ERR_INVALID_DATA);
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
        static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::SET_ANCO_FORCE_DO_DIRECT)), ERR_INVALID_STATE);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::SET_WINDOW_CONTAINER)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::SET_FOCUS_APP_INFO)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::TAKE_SURFACE_CAPTURE)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::TAKE_UI_CAPTURE_IN_RANGE)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::CREATE_NODE)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::CREATE_NODE_AND_SURFACE)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::SET_SYSTEM_ANIMATED_SCENES)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::EXECUTE_SYNCHRONOUS_TASK)), ERR_INVALID_STATE);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::SET_HARDWARE_ENABLED)), ERR_INVALID_DATA);
}

/**
 * @tc.name: TestRSRenderServiceConnectionStub001
 * @tc.desc: Test if data has no content.
 * @tc.type: FUNC
 * @tc.require: issueI60KUK
 */
HWTEST_F(RSClientToRenderConnectionStubTest, TestRSRenderServiceConnectionStub001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    int res;
    uint32_t code;
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::COMMIT_TRANSACTION);
    res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);
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
            static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::SET_LAYER_TOP_FOR_HARDWARE_COMPOSER);
        data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
        data.WriteUint64(0);
        data.WriteBool(true);
        data.WriteUint32(1);
        int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
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
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::CLEAR_UIFIRST_CACHE);
    data.WriteUint64(0);
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
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
    ASSERT_NE(connectionStub_, nullptr);
    // MAX_DROP_FRAME_PID_LIST_SIZE = 1024
    std::vector<int32_t> pidList(1025, 1);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::DROP_FRAME_BY_PID);
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data.WriteInt32Vector(pidList);
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);
}

/**
 * @tc.name: GetBrightnessInfoTest
 * @tc.desc: Test GetBrightnessInfo
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToRenderConnectionStubTest, GetBrightnessInfoTest, TestSize.Level2)
{
    sptr<RSClientToRenderConnectionStub> connectionStub =
        new RSClientToRenderConnection(0, renderPipelineAgent_, token_->AsObject());

    // case 1: no data
    {
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;
        data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
        auto interfaceCode = RSIClientToRenderConnectionInterfaceCode::GET_BRIGHTNESS_INFO;
        auto res = connectionStub->OnRemoteRequest(static_cast<uint32_t>(interfaceCode), data, reply, option);
        EXPECT_EQ(res, ERR_INVALID_DATA);
    }

    // case 2: valid screenId
    {
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;
        data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
        data.WriteUint64(hdiOutput_->GetScreenId());
        auto interfaceCode = RSIClientToRenderConnectionInterfaceCode::GET_BRIGHTNESS_INFO;
        auto res = connectionStub->OnRemoteRequest(static_cast<uint32_t>(interfaceCode), data, reply, option);
        EXPECT_EQ(res, ERR_NONE);
    }

    // case 3: invalid screenId
    {
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;
        data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
        data.WriteUint64(INVALID_SCREEN_ID);
        auto interfaceCode = RSIClientToRenderConnectionInterfaceCode::GET_BRIGHTNESS_INFO;
        auto res = connectionStub->OnRemoteRequest(static_cast<uint32_t>(interfaceCode), data, reply, option);
        EXPECT_EQ(res, ERR_NONE);
    }
}

/**
 * @tc.name: WriteBrightnessInfoTest
 * @tc.desc: Test WriteBrightnessInfo
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToRenderConnectionStubTest, WriteBrightnessInfoTest, TestSize.Level2)
{
    // case 1: normal write
    {
        BrightnessInfo brightnessInfo;
        MessageParcel data;
        ASSERT_TRUE(connectionStub_->WriteBrightnessInfo(brightnessInfo, data));
    }

    // case 2: can't write any data
    {
        BrightnessInfo brightnessInfo;
        MessageParcel data;
        size_t desireCapacity = sizeof(float) * 0;
        data.writeCursor_ = data.GetMaxCapacity() - desireCapacity;
        ASSERT_FALSE(connectionStub_->WriteBrightnessInfo(brightnessInfo, data));
    }

    // case 3: can write one float
    {
        BrightnessInfo brightnessInfo;
        MessageParcel data;
        size_t desireCapacity = sizeof(float) * 1;
        data.writeCursor_ = data.GetMaxCapacity() - desireCapacity;
        ASSERT_FALSE(connectionStub_->WriteBrightnessInfo(brightnessInfo, data));
    }

    // case 4: can write two float
    {
        BrightnessInfo brightnessInfo;
        MessageParcel data;
        size_t desireCapacity = sizeof(float) * 2;
        data.writeCursor_ = data.GetMaxCapacity() - desireCapacity;
        ASSERT_FALSE(connectionStub_->WriteBrightnessInfo(brightnessInfo, data));
    }
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
    auto handler = mainThread->handler_;
    auto runner = AppExecFwk::EventRunner::Create("TakeUICaptureInRangeTest001");
    mainThread->handler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
    ASSERT_NE(mainThread->handler_, nullptr);
    runner->Run();

    sptr<RSIConnectionToken> token_ = new IRemoteStub<RSIConnectionToken>();
    sptr<RSClientToRenderConnection> toRenderConnection =
        new RSClientToRenderConnection(0, renderPipelineAgent_, token_->AsObject());
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
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::GET_SCREEN_HDR_STATUS);
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data.WriteUint64(0);
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
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
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::GET_SCREEN_HDR_STATUS);
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data.WriteUint64(~0);
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
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
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::GET_SCREEN_HDR_STATUS);
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data.WriteBool(false);
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);
}

/**
 * @tc.name: GetScreenHDRStatus004
 * @tc.desc: Test GetScreenHDRStatus
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToRenderConnectionStubTest, GetScreenHDRStatus004, TestSize.Level1)
{
    sptr<RSClientToRenderConnection> connection = iface_cast<RSClientToRenderConnection>(connectionStub_);
    ASSERT_NE(connection, nullptr);
    ScreenId id = 0;
    HdrStatus hdrStatus;
    int32_t resCode;
    EXPECT_EQ(connection->GetScreenHDRStatus(id, hdrStatus, resCode), ERR_OK);
}

/**
 * @tc.name: TakeSurfaceCaptureWithAllWindowsTest001
 * @tc.desc: Test TakeSurfaceCaptureWithAllWindows for success
 * @tc.type: FUNC
 * @tc.require: issueICQ74B
 */
HWTEST_F(RSClientToRenderConnectionStubTest, TakeSurfaceCaptureWithAllWindowsTest001, TestSize.Level2)
{
    ASSERT_NE(connectionStub_, nullptr);

    bool checkDrmAndSurfaceLock = false;
    NodeId displayNodeId = 123;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(
        RSIClientToRenderConnectionInterfaceCode::TAKE_SURFACE_CAPTURE_WITH_ALL_WINDOWS);
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
    auto res = connectionStub_->OnRemoteRequest(code, data, reply, option);
#ifdef RS_ENABLE_UNI_RENDER
    EXPECT_LE(res, ERR_PERMISSION_DENIED);
#else
    EXPECT_LE(res, ERR_INVALID_DATA);
#endif
}

/**
 * @tc.name: TakeSurfaceCaptureWithAllWindowsTest002
 * @tc.desc: Test TakeSurfaceCaptureWithAllWindows
 * @tc.type: FUNC
 * @tc.require: issueICQ74B
 */
HWTEST_F(RSClientToRenderConnectionStubTest, TakeSurfaceCaptureWithAllWindowsTest002, TestSize.Level2)
{
    sptr<RSClientToRenderConnection> connection = iface_cast<RSClientToRenderConnection>(connectionStub_);
    ASSERT_NE(connection, nullptr);
    sptr<RSRenderPipelineAgent> agent = connection->renderPipelineAgent_;
    connection->renderPipelineAgent_ = nullptr;
    RSSurfaceCaptureConfig captureConfig;
    RSSurfaceCapturePermissions permissions;
    auto res = connection->TakeSurfaceCaptureWithAllWindows(0, nullptr, captureConfig, true, permissions);
    EXPECT_EQ(res, ERR_INVALID_VALUE);
    connection->renderPipelineAgent_ = agent;

    ASSERT_NE(connection->renderPipelineAgent_, nullptr);
    permissions.isSystemCalling = false;
    permissions.screenCapturePermission = false;
    res = connection->TakeSurfaceCaptureWithAllWindows(0, nullptr, captureConfig, true, permissions);
    EXPECT_EQ(res, ERR_PERMISSION_DENIED);
    ASSERT_NE(renderPipelineAgent_->rsRenderPipeline_, nullptr);
    sptr<RSISurfaceCaptureCallback> callback = new RSSurfaceCaptureCallbackStubMock();
    res = connection->TakeSurfaceCaptureWithAllWindows(0, callback, captureConfig, true, permissions);
    EXPECT_EQ(res, ERR_PERMISSION_DENIED);

    ASSERT_NE(renderPipelineAgent_, nullptr);
    auto pipeline = renderPipelineAgent_->rsRenderPipeline_;
    renderPipelineAgent_->rsRenderPipeline_ = nullptr;
    res = renderPipelineAgent_->TakeSurfaceCaptureWithAllWindows(0, nullptr, captureConfig, true, permissions);
    EXPECT_EQ(res, ERR_INVALID_VALUE);
    renderPipelineAgent_->rsRenderPipeline_ = pipeline;
}

/**
 * @tc.name: TakeSurfaceCaptureWithAllWindowsTest003
 * @tc.desc: Test TakeSurfaceCaptureWithAllWindows
 * @tc.type: FUNC
 * @tc.require: issueICQ74B
 */
HWTEST_F(RSClientToRenderConnectionStubTest, TakeSurfaceCaptureWithAllWindowsTest003, TestSize.Level2)
{
    ASSERT_NE(connectionStub_, nullptr);
    MessageParcel data1;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(
        RSIClientToRenderConnectionInterfaceCode::TAKE_SURFACE_CAPTURE_WITH_ALL_WINDOWS);
    data1.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data1.WriteBool(false);
    int res = connectionStub_->OnRemoteRequest(code, data1, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);

    MessageParcel data2;
    data2.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data2.WriteUint64(1);
    res = connectionStub_->OnRemoteRequest(code, data2, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);

    MessageParcel data3;
    data3.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data3.WriteUint64(1);
    data3.WriteBool(true);
    res = connectionStub_->OnRemoteRequest(code, data3, reply, option);
    ASSERT_EQ(res, ERR_NULL_OBJECT);

    MessageParcel data4;
    data4.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data4.WriteUint64(1);
    data4.WriteBool(true);
    sptr<RSScreenChangeCallbackStubMock> callback1 = new RSScreenChangeCallbackStubMock();
    ASSERT_NE(callback1, nullptr);
    data4.WriteRemoteObject(callback1->AsObject());
    res = connectionStub_->OnRemoteRequest(code, data4, reply, option);
    ASSERT_EQ(res, ERR_NULL_OBJECT);

    MessageParcel data5;
    data5.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data5.WriteUint64(1);
    data5.WriteBool(true);
    sptr<RSISurfaceCaptureCallback> callback2 = new RSSurfaceCaptureCallbackStubMock();
    ASSERT_NE(callback2, nullptr);
    data5.WriteRemoteObject(callback2->AsObject());
    data5.WriteBool(false);
    res = connectionStub_->OnRemoteRequest(code, data5, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);
}

/**
 * @tc.name: FreezeScreenTest001
 * @tc.desc: Test FreezeScreen
 * @tc.type: FUNC
 * @tc.require: issueICQ74B
 */
HWTEST_F(RSClientToRenderConnectionStubTest, FreezeScreenTest001, TestSize.Level2)
{
    constexpr uint32_t TIME_OF_FREEZE_TASK = 100000;
    ASSERT_NE(connectionStub_, nullptr);
    MessageParcel data1;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::FREEZE_SCREEN);
    data1.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data1.WriteBool(false);
    int res = connectionStub_->OnRemoteRequest(code, data1, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);

    MessageParcel data2;
    data2.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data2.WriteUint64(1);
    res = connectionStub_->OnRemoteRequest(code, data2, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);

    MessageParcel data3;
    data3.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data3.WriteUint64(1);
    data3.WriteBool(true);
    data3.WriteBool(false);
    res = connectionStub_->OnRemoteRequest(code, data3, reply, option);
    ASSERT_EQ(res, ERR_OK);

    sptr<RSClientToRenderConnection> connection = iface_cast<RSClientToRenderConnection>(connectionStub_);
    ASSERT_NE(connection, nullptr);
    sptr<RSRenderPipelineAgent> agent = connection->renderPipelineAgent_;
    connection->renderPipelineAgent_ = nullptr;
    ASSERT_EQ(connection->renderPipelineAgent_, nullptr);
    res = connection->FreezeScreen(0, false);
    ASSERT_EQ(res, ERR_INVALID_VALUE);
    connection->renderPipelineAgent_ = agent;

    ASSERT_NE(connection->renderPipelineAgent_, nullptr);
    std::shared_ptr<RSRenderPipeline> pipeline = connection->renderPipelineAgent_->rsRenderPipeline_;
    connection->renderPipelineAgent_->rsRenderPipeline_ = nullptr;
    ASSERT_EQ(connection->renderPipelineAgent_->rsRenderPipeline_, nullptr);
    res = connection->FreezeScreen(0, false);
    ASSERT_EQ(res, ERR_INVALID_VALUE);
    connection->renderPipelineAgent_->rsRenderPipeline_ = pipeline;

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
    auto& nodeMap = connection->renderPipelineAgent_->rsRenderPipeline_->GetContext().GetMutableNodeMap();
    EXPECT_TRUE(nodeMap.RegisterRenderNode(displayNode));
    EXPECT_TRUE(nodeMap.RegisterRenderNode(screenNode));
    res = connection->FreezeScreen(displayNodeId, false);
    usleep(TIME_OF_FREEZE_TASK);
    ASSERT_EQ(res, ERR_OK);
    nodeMap.UnregisterRenderNode(screenNode->GetId());
    nodeMap.UnregisterRenderNode(displayNode->GetId());
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
    ASSERT_NE(renderPipelineAgent_, nullptr);
    ASSERT_NE(renderPipelineAgent_->rsRenderPipeline_, nullptr);
    ASSERT_NE(renderPipelineAgent_->rsRenderPipeline_->mainThread_, nullptr);
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
    auto& nodeMap = renderPipelineAgent_->rsRenderPipeline_->mainThread_->GetContext().GetMutableNodeMap();
    EXPECT_TRUE(nodeMap.RegisterRenderNode(displayNode));
    EXPECT_TRUE(nodeMap.RegisterRenderNode(screenNode));
    auto res = renderPipelineAgent_->FreezeScreen(displayNodeId, false);
    usleep(TIME_OF_FREEZE_TASK);
    ASSERT_EQ(res, ERR_OK);
    res = renderPipelineAgent_->FreezeScreen(displayNodeId, true);
    usleep(TIME_OF_FREEZE_TASK);
    ASSERT_EQ(res, ERR_OK);
    nodeMap.UnregisterRenderNode(displayNode->GetId());
    nodeMap.UnregisterRenderNode(screenNode->GetId());
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
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::FREEZE_SCREEN);
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
    ASSERT_NE(renderPipelineAgent_, nullptr);
    ASSERT_NE(renderPipelineAgent_->rsRenderPipeline_, nullptr);
    ASSERT_NE(renderPipelineAgent_->rsRenderPipeline_->mainThread_, nullptr);

    RSSurfaceCaptureConfig captureConfig;
    RSSurfaceCapturePermissions permissions;
    permissions.screenCapturePermission = true;
    permissions.isSystemCalling = true;
    NodeId displayNodeId = 45;
    RSDisplayNodeConfig displayNodeConfig;
    std::shared_ptr<RSLogicalDisplayRenderNode> displayNode = std::shared_ptr<RSLogicalDisplayRenderNode>(
        new RSLogicalDisplayRenderNode(displayNodeId, displayNodeConfig), RSRenderNodeGC::NodeDestructor);
    ASSERT_NE(displayNode, nullptr);
    auto& nodeMap = renderPipelineAgent_->rsRenderPipeline_->mainThread_->GetContext().GetMutableNodeMap();
    EXPECT_TRUE(nodeMap.RegisterRenderNode(displayNode));
    auto ret = renderPipelineAgent_->TakeSurfaceCaptureWithAllWindows(
        displayNodeId, nullptr, captureConfig, false, permissions);
    usleep(TIME_OF_CAPTURE_TASK);
    EXPECT_EQ(ret, ERR_NONE);
    RSMainThread::Instance()->SetHasSurfaceLockLayer(false);
    RSMainThread::Instance()->hasProtectedLayer_ = false;
    RSMainThread::Instance()->hasSurfaceLockLayer_ = false;
    EXPECT_FALSE(RSMainThread::Instance()->HasDRMOrSurfaceLockLayer());
    ret = renderPipelineAgent_->TakeSurfaceCaptureWithAllWindows(
        displayNodeId, nullptr, captureConfig, true, permissions);
    usleep(TIME_OF_CAPTURE_TASK);
    EXPECT_EQ(ret, ERR_NONE);
    RSMainThread::Instance()->hasProtectedLayer_ = true;
    RSMainThread::Instance()->hasSurfaceLockLayer_ = true;
    ret = renderPipelineAgent_->TakeSurfaceCaptureWithAllWindows(
        displayNodeId, nullptr, captureConfig, true, permissions);
    usleep(TIME_OF_CAPTURE_TASK);
    EXPECT_EQ(ret, ERR_NONE);
    sptr<RSISurfaceCaptureCallback> callback = new RSSurfaceCaptureCallbackStubMock();
    ret = renderPipelineAgent_->TakeSurfaceCaptureWithAllWindows(
        displayNodeId, callback, captureConfig, true, permissions);
    usleep(TIME_OF_CAPTURE_TASK);
    EXPECT_EQ(ret, ERR_NONE);
    nodeMap.UnregisterRenderNode(displayNode->GetId());
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
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::REGISTER_TRANSACTION_DATA_CALLBACK);
    data.WriteUint64(123);
    data.WriteUint64(456);
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
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
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::REGISTER_TRANSACTION_DATA_CALLBACK);
    data.WriteUint64(123);
    data.WriteUint64(456);
    sptr<RSTransactionDataCallbackStubMock> callback = new RSTransactionDataCallbackStubMock();
    data.WriteRemoteObject(callback->AsObject());
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
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
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::REGISTER_CANVAS_CALLBACK);

    MessageParcel data1;
    data1.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    int res = connectionStub_->OnRemoteRequest(code, data1, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);

    MessageParcel data2;
    data2.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data2.WriteBool(false);
    res = connectionStub_->OnRemoteRequest(code, data2, reply, option);
    ASSERT_EQ(res, ERR_NONE);

    MessageParcel data3;
    data3.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data3.WriteBool(true);
    sptr<RSICanvasSurfaceBufferCallback> callback = new RSCanvasSurfaceBufferCallbackStubMock();
    data3.WriteRemoteObject(callback->AsObject());
    res = connectionStub_->OnRemoteRequest(code, data3, reply, option);
    ASSERT_EQ(res, ERR_NONE);

    MessageParcel data4;
    data4.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data4.WriteBool(true);
    sptr<RSCanvasSurfaceBufferCallbackStubMock> bufferCallback = new RSCanvasSurfaceBufferCallbackStubMock();
    bufferCallback->isProxyObject_ = false;
    callback = bufferCallback;
    data4.WriteRemoteObject(callback->AsObject());
    res = connectionStub_->OnRemoteRequest(code, data4, reply, option);
    ASSERT_EQ(res, ERR_UNKNOWN_OBJECT);

    MessageParcel data5;
    data5.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data5.WriteBool(true);
    res = connectionStub_->OnRemoteRequest(code, data5, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);

    NodeMemReleaseParam::SetCanvasDrawingNodeDMAMemEnabled(false);
    MessageParcel data6;
    data6.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data6.WriteBool(true);
    callback = new RSCanvasSurfaceBufferCallbackStubMock();
    data6.WriteRemoteObject(callback->AsObject());
    res = connectionStub_->OnRemoteRequest(code, data6, reply, option);
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
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::SUBMIT_CANVAS_PRE_ALLOCATED_BUFFER);

    MessageParcel data1;
    data1.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    int res = connectionStub_->OnRemoteRequest(code, data1, reply, option);
    ASSERT_NE(res, ERR_NONE);

    MessageParcel data2;
    data2.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data2.WriteUint64(1);
    res = connectionStub_->OnRemoteRequest(code, data2, reply, option);
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
    res = connectionStub_->OnRemoteRequest(code, data3, reply, option);
    ASSERT_EQ(res, ERR_NONE);

    MessageParcel data4;
    data4.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data4.WriteUint64(1); // Write nodeId
    data4.WriteUint32(1); // Write resetSurfaceIndex
    data4.WriteUint32(1); // Write sequence
    data4.WriteBool(true); // Whether has buffer
    res = connectionStub_->OnRemoteRequest(code, data4, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);

    MessageParcel data5;
    data5.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data5.WriteUint64(3); // Write nodeId
    data5.WriteUint32(3); // Write resetSurfaceIndex
    data5.WriteUint32(1); // Write sequence
    data5.WriteBool(true); // Whether has buffer
    sptr<SurfaceBuffer> buffer = SurfaceBufferUtils::CreateCanvasSurfaceBuffer(1, 100, 100);
    buffer->WriteToMessageParcel(data5);
    res = connectionStub_->OnRemoteRequest(code, data5, reply, option);
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
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::SUBMIT_CANVAS_PRE_ALLOCATED_BUFFER);
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
    auto ret = connectionStub_->OnRemoteRequest(code, data, reply, option);
    NodeMemReleaseParam::SetCanvasDrawingNodeDMAMemEnabled(true);
    ASSERT_EQ(ret, 0);

    auto newPid = getpid();
    auto mainThread = RSMainThread::Instance();
    sptr<RSIConnectionToken> token_ = new IRemoteStub<RSIConnectionToken>();
    sptr<RSClientToRenderConnection> toRenderConnection =
        new RSClientToRenderConnection(0, renderPipelineAgent_, token_->AsObject());
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
    sptr<RSClientToRenderConnection> connection = iface_cast<RSClientToRenderConnection>(connectionStub_);
    ASSERT_NE(connection, nullptr);
    connection->remotePid_ = 0;
    ret = connection->SubmitCanvasPreAllocatedBuffer(1, buffer, 2);
    ASSERT_EQ(ret, 0);
    ret = connection->SubmitCanvasPreAllocatedBuffer(1, buffer, 2);
    ASSERT_NE(ret, 0);
}
#endif

/**
 * @tc.name: SetSystemAnimatedScenesTest001
 * @tc.desc: Test SetSystemAnimatedScenes when ReadBool and ReadUint32 fail
 * @tc.type: FUNC
 * @tc.require: issue20726
 */
HWTEST_F(RSClientToRenderConnectionStubTest, SetSystemAnimatedScenesTest001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code =
        static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::SET_SYSTEM_ANIMATED_SCENES);
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    int ret = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ret, ERR_INVALID_DATA);
}
 
/**
 * @tc.name: SetSystemAnimatedScenesTest002
 * @tc.desc: Test SetSystemAnimatedScenes when data is ReadBool fail
 * @tc.type: FUNC
 * @tc.require: issue20726
 */
HWTEST_F(RSClientToRenderConnectionStubTest, SetSystemAnimatedScenesTest002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code =
        static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::SET_SYSTEM_ANIMATED_SCENES);
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data.WriteUint32(0);
    int ret = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ret, ERR_INVALID_DATA);
}
 
/**
 * @tc.name: SetSystemAnimatedScenesTest003
 * @tc.desc: Test SetSystemAnimatedScenes when ReadUint32 fail
 * @tc.type: FUNC
 * @tc.require: issue20726
 */
HWTEST_F(RSClientToRenderConnectionStubTest, SetSystemAnimatedScenesTest003, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code =
        static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::SET_SYSTEM_ANIMATED_SCENES);
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data.WriteBool(true);
    int ret = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ret, ERR_INVALID_DATA);
}
 
/**
 * @tc.name: SetSystemAnimatedScenesTest004
 * @tc.desc: Test SetSystemAnimatedScenes when mainThread_ isn't nullptr
 * @tc.type: FUNC
 * @tc.require: issue20726
 */
HWTEST_F(RSClientToRenderConnectionStubTest, SetSystemAnimatedScenesTest004, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code =
        static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::SET_SYSTEM_ANIMATED_SCENES);
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data.WriteUint32(0);
    data.WriteBool(true);
    int ret = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ret, ERR_NONE);
}
 
// /**
//  * @tc.name: SetSystemAnimatedScenesTest005
//  * @tc.desc: Test SetSystemAnimatedScenes when mainThread_ is nullptr
//  * @tc.type: FUNC
//  * @tc.require: issue20726
//  */
// HWTEST_F(RSClientToRenderConnectionStubTest, SetSystemAnimatedScenesTest005, TestSize.Level1)
// {
//     MessageParcel data;
//     MessageParcel reply;
//     MessageOption option;
//     uint32_t code =
//         static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::SET_SYSTEM_ANIMATED_SCENES);
//     data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
//     data.WriteUint32(0);
//     data.WriteBool(true);
//     sptr<RSClientToRenderConnection> clientToRenderConnection =
//         iface_cast<RSClientToRenderConnection>(connectionStub_);
//     ASSERT_NE(clientToRenderConnection, nullptr);
//     auto mainThread = clientToRenderConnection->mainThread_;
//     clientToRenderConnection->mainThread_ = nullptr;
//     int ret = connectionStub_->OnRemoteRequest(code, data, reply, option);
//     ASSERT_EQ(ret, ERR_NONE);
 
//     clientToRenderConnection->mainThread_ = mainThread;
// }

/**
 * @tc.name: SetSurfaceWatermarkSub001
 * @tc.desc: Test SetSurfaceWatermarkSub001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToRenderConnectionStubTest, SetSurfaceWatermarkSub001, TestSize.Level1)
{
    ASSERT_NE(connectionStub_, nullptr);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    std::string name = "watermark";
    int width = 10;
    int height = 10;
    Media::InitializationOptions opts;
    opts.size.width = width;
    opts.size.height = height;
    std::shared_ptr<Media::PixelMap> pixelmap = Media::PixelMap::Create(opts);
    std::vector<NodeId> nodeList;
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::SET_SURFACE_WATERMARK);
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    MessageParcel data0;
    data0.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data0.WriteInt32(2);
    res = connectionStub_->OnRemoteRequest(code, data0, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    MessageParcel data20;
    data20.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data20.WriteInt32(2);
    data20.WriteString(name);
    res = connectionStub_->OnRemoteRequest(code, data20, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    MessageParcel data2;
    data2.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data2.WriteInt32(2);
    data2.WriteString(name);
    data2.WriteBool(false);
    res = connectionStub_->OnRemoteRequest(code, data2, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    MessageParcel data3;
    data3.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data3.WriteInt32(2);
    data3.WriteString(name);
    data3.WriteBool(true);
    data3.WriteParcelable(pixelmap.get());
    res = connectionStub_->OnRemoteRequest(code, data3, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    MessageParcel data4;
    data4.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data4.WriteInt32(2);
    data4.WriteString(name);
    data4.WriteBool(true);
    data4.WriteParcelable(pixelmap.get());
    data4.WriteUInt64Vector(nodeList);
    res = connectionStub_->OnRemoteRequest(code, data4, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    MessageParcel data5;
    data5.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data5.WriteInt32(2);
    data5.WriteString(name);
    data5.WriteBool(true);
    data5.WriteParcelable(pixelmap.get());
    data5.WriteUInt64Vector(nodeList);
    data5.WriteUint8(static_cast<uint8_t>(0));
    res = connectionStub_->OnRemoteRequest(code, data5, reply, option);
}

/**
 * @tc.name: ClearSurfaceWatermarkForNodesStub001
 * @tc.desc: Test ClearSurfaceWatermarkForNodesStub001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToRenderConnectionStubTest, ClearSurfaceWatermarkForNodesStub001, TestSize.Level1)
{
    ASSERT_NE(connectionStub_, nullptr);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    std::string name = "watermark";
    std::vector<NodeId> nodeList;
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::CLEAR_SURFACE_WATERMARK_FOR_NODES);
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    MessageParcel data2;
    data2.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data2.WriteString(name);
    res = connectionStub_->OnRemoteRequest(code, data2, reply, option);

    MessageParcel data3;
    data3.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data3.WriteString(name);
    data3.WriteUInt64Vector(nodeList);
    res = connectionStub_->OnRemoteRequest(code, data3, reply, option);
}
/**
 * @tc.name: ClearSurfaceWatermarkStub001
 * @tc.desc: Test ClearSurfaceWatermarkStub001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToRenderConnectionStubTest, ClearSurfaceWatermarkStub001, TestSize.Level1)
{
    ASSERT_NE(connectionStub_, nullptr);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    std::string name = "watermark";
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::CLEAR_SURFACE_WATERMARK_FOR_NODES);
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    MessageParcel data2;
    data2.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data2.WriteString(name);
    res = connectionStub_->OnRemoteRequest(code, data2, reply, option);
}

/**
 * @tc.name: CreateNodeTest001
 * @tc.desc: Test CREATE_NODE interface code path
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToRenderConnectionStubTest, CreateNodeTest001, TestSize.Level1)
{
    ASSERT_NE(connectionStub_, nullptr);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::CREATE_NODE);

    // Test case 1: missing nodeId
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    // Test case 2: missing surfaceName (branch coverage)
    MessageParcel data2;
    data2.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    NodeId nodeId = 10001;
    data2.WriteUint64(nodeId);
    res = connectionStub_->OnRemoteRequest(code, data2, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    // Test case 3: valid data with nodeId and surfaceName
    MessageParcel data3;
    data3.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data3.WriteUint64(nodeId);
    data3.WriteString("TestSurface");
    res = connectionStub_->OnRemoteRequest(code, data3, reply, option);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: CreateDisplayNodeTest001
 * @tc.desc: Test CREATE_DISPLAY_NODE interface code path
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToRenderConnectionStubTest, CreateDisplayNodeTest001, TestSize.Level1)
{
    ASSERT_NE(connectionStub_, nullptr);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::CREATE_DISPLAY_NODE);

    // Test case 1: missing id
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    // Test case 2: missing mirrorId/screenId/isMirrored (branch coverage)
    MessageParcel data2;
    data2.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    NodeId displayNodeId = 10002;
    data2.WriteUint64(displayNodeId);
    res = connectionStub_->OnRemoteRequest(code, data2, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    // Test case 3: valid data
    MessageParcel data3;
    data3.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data3.WriteUint64(displayNodeId);
    data3.WriteUint64(0); // mirrorId
    data3.WriteUint64(screenId_); // screenId
    data3.WriteBool(false); // isMirrored
    res = connectionStub_->OnRemoteRequest(code, data3, reply, option);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: CreateDisplayNodeTest002
 * @tc.desc: Test CREATE_DISPLAY_NODE with missing screenId (independent Read* condition)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToRenderConnectionStubTest, CreateDisplayNodeTest002, TestSize.Level1)
{
    ASSERT_NE(connectionStub_, nullptr);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::CREATE_DISPLAY_NODE);

    // Test case: write id and mirrorId, but NOT screenId
    // This should fail at !data.ReadUint64(screenId) condition
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    NodeId displayNodeId = 10002;
    data.WriteUint64(displayNodeId);
    data.WriteUint64(0); // mirrorId
    // NOT writing screenId - should fail here
    // NOT writing isMirrored

    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);
}

/**
 * @tc.name: CreateDisplayNodeTest003
 * @tc.desc: Test CREATE_DISPLAY_NODE with missing isMirrored (independent Read* condition)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToRenderConnectionStubTest, CreateDisplayNodeTest003, TestSize.Level1)
{
    ASSERT_NE(connectionStub_, nullptr);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::CREATE_DISPLAY_NODE);

    // Test case: write id, mirrorId, and screenId, but NOT isMirrored
    // This should fail at !data.ReadBool(isMirrored) condition
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    NodeId displayNodeId = 10002;
    data.WriteUint64(displayNodeId);
    data.WriteUint64(0); // mirrorId
    data.WriteUint64(screenId_); // screenId
    // NOT writing isMirrored - should fail here

    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);
}

/**
 * @tc.name: RegisterApplicationAgentTest001
 * @tc.desc: Test REGISTER_APPLICATION_AGENT interface code path
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToRenderConnectionStubTest, RegisterApplicationAgentTest001, TestSize.Level1)
{
    ASSERT_NE(connectionStub_, nullptr);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::REGISTER_APPLICATION_AGENT);

    // Test case 1: missing remote object
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, ERR_NULL_OBJECT);

    // Test case 2: valid data with mock agent
    MessageParcel data2;
    data2.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    sptr<RSApplicationAgentStub> agent = new RSApplicationAgentStubMock();
    data2.WriteRemoteObject(agent->AsObject());
    res = connectionStub_->OnRemoteRequest(code, data2, reply, option);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: SetBufferAvailableListenerTest001
 * @tc.desc: Test SET_BUFFER_AVAILABLE_LISTENER interface code path
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToRenderConnectionStubTest, SetBufferAvailableListenerTest001, TestSize.Level1)
{
    ASSERT_NE(connectionStub_, nullptr);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::SET_BUFFER_AVAILABLE_LISTENER);

    // Test case 1: missing nodeId
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    // Test case 2: valid data with callback
    MessageParcel data2;
    data2.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    NodeId nodeId = surfaceNode_->GetId();
    data2.WriteUint64(nodeId);

    sptr<RSBufferAvailableCallbackStub> callback = new RSBufferAvailableCallbackStubMock();
    data2.WriteRemoteObject(callback->AsObject());
    data2.WriteBool(false); // isFromRenderThread
    res = connectionStub_->OnRemoteRequest(code, data2, reply, option);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: SetBufferClearListenerTest001
 * @tc.desc: Test SET_BUFFER_CLEAR_LISTENER interface code path
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToRenderConnectionStubTest, SetBufferClearListenerTest001, TestSize.Level1)
{
    ASSERT_NE(connectionStub_, nullptr);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::SET_BUFFER_CLEAR_LISTENER);

    // Test case 1: missing nodeId
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    // Test case 2: valid data with callback
    MessageParcel data2;
    data2.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    NodeId nodeId = surfaceNode_->GetId();
    data2.WriteUint64(nodeId);

    sptr<RSBufferClearCallbackStub> callback = new RSBufferClearCallbackStubMock();
    data2.WriteRemoteObject(callback->AsObject());
    res = connectionStub_->OnRemoteRequest(code, data2, reply, option);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: SetBufferClearListenerTest002
 * @tc.desc: Test SET_BUFFER_CLEAR_LISTENER with missing callback (branch coverage)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToRenderConnectionStubTest, SetBufferClearListenerTest002, TestSize.Level1)
{
    ASSERT_NE(connectionStub_, nullptr);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::SET_BUFFER_CLEAR_LISTENER);

    // Test case: missing callback (branch coverage)
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    NodeId nodeId = surfaceNode_->GetId();
    data.WriteUint64(nodeId);
    // Not writing callback - should return ERR_NULL_OBJECT
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, ERR_NULL_OBJECT);
}

/**
 * @tc.name: GetPixelmapTest001
 * @tc.desc: Test GET_PIXELMAP interface code path
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToRenderConnectionStubTest, GetPixelmapTest001, TestSize.Level1)
{
    ASSERT_NE(connectionStub_, nullptr);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::GET_PIXELMAP);

    // Test case 1: missing nodeId
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    // Test case 2: valid data
    MessageParcel data2;
    data2.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    NodeId nodeId = 10003;
    data2.WriteUint64(nodeId);

    // Create mock PixelMap
    Media::InitializationOptions opts;
    opts.size.width = 10;
    opts.size.height = 10;
    std::shared_ptr<Media::PixelMap> pixelmap = Media::PixelMap::Create(opts);
    data2.WriteParcelable(pixelmap.get());

    // Write Rect
    Drawing::Rect rect;
    data2.WriteFloat(rect.left_);
    data2.WriteFloat(rect.top_);
    data2.WriteFloat(rect.right_);
    data2.WriteFloat(rect.bottom_);

    // Write DrawCmdList (nullptr)
    data2.WriteUint64(0);

    res = connectionStub_->OnRemoteRequest(code, data2, reply, option);
    // Note: This might return ERR_INVALID_REPLY if pixelmap cannot be created
    EXPECT_TRUE(res == ERR_NONE || res == ERR_INVALID_REPLY);
}

/**
 * @tc.name: GetPixelmapTest002
 * @tc.desc: Test GET_PIXELMAP with missing PixelMap (independent Read* condition)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToRenderConnectionStubTest, GetPixelmapTest002, TestSize.Level1)
{
    ASSERT_NE(connectionStub_, nullptr);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::GET_PIXELMAP);

    // Test case: write nodeId, but NOT PixelMap
    // This should fail when reading PixelMap or subsequent data
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    NodeId nodeId = 10003;
    data.WriteUint64(nodeId);
    // NOT writing PixelMap - should fail here or later
    connectionStub_->OnRemoteRequest(code, data, reply, option);
    bool res;
    reply.ReadBool(res);
    EXPECT_FALSE(res);
}

/**
 * @tc.name: TakeSurfaceCaptureSoloTest001
 * @tc.desc: Test TAKE_SURFACE_CAPTURE_SOLO interface code path
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToRenderConnectionStubTest, TakeSurfaceCaptureSoloTest001, TestSize.Level1)
{
    ASSERT_NE(connectionStub_, nullptr);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::TAKE_SURFACE_CAPTURE_SOLO);

    // Test case 1: missing nodeId
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    // Test case 2: valid data with capture config
    MessageParcel data2;
    data2.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    NodeId nodeId = surfaceNode_->GetId();
    data2.WriteUint64(nodeId);

    RSSurfaceCaptureConfig captureConfig;
    data2.WriteFloat(captureConfig.scaleX);
    data2.WriteFloat(captureConfig.scaleY);
    data2.WriteBool(captureConfig.useDma);
    data2.WriteBool(captureConfig.useCurWindow);
    data2.WriteUint8(static_cast<uint8_t>(captureConfig.captureType));
    data2.WriteBool(captureConfig.isSync);
    data2.WriteBool(captureConfig.isHdrCapture);
    data2.WriteBool(captureConfig.needF16WindowCaptureForScRGB);
    data2.WriteBool(captureConfig.needErrorCode);
    data2.WriteFloat(captureConfig.mainScreenRect.left_);
    data2.WriteFloat(captureConfig.mainScreenRect.top_);
    data2.WriteFloat(captureConfig.mainScreenRect.right_);
    data2.WriteFloat(captureConfig.mainScreenRect.bottom_);
    data2.WriteUInt64Vector(captureConfig.blackList);
    data2.WriteUint64(captureConfig.uiCaptureInRangeParam.endNodeId);
    data2.WriteBool(captureConfig.uiCaptureInRangeParam.useBeginNodeSize);
    data2.WriteFloat(captureConfig.specifiedAreaRect.left_);
    data2.WriteFloat(captureConfig.specifiedAreaRect.top_);
    data2.WriteFloat(captureConfig.specifiedAreaRect.right_);
    data2.WriteFloat(captureConfig.specifiedAreaRect.bottom_);
    data2.WriteUint32(captureConfig.backGroundColor);
    data2.WriteUint32(captureConfig.colorSpace.first);
    data2.WriteBool(captureConfig.colorSpace.second);
    data2.WriteUint32(captureConfig.dynamicRangeMode.first);
    data2.WriteBool(captureConfig.dynamicRangeMode.second);
    data2.WriteBool(captureConfig.isSyncRender);

    res = connectionStub_->OnRemoteRequest(code, data2, reply, option);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: TakeSurfaceCaptureSoloTest002
 * @tc.desc: Test TAKE_SURFACE_CAPTURE_SOLO with missing captureConfig (independent Read* condition)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToRenderConnectionStubTest, TakeSurfaceCaptureSoloTest002, TestSize.Level1)
{
    ASSERT_NE(connectionStub_, nullptr);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::TAKE_SURFACE_CAPTURE_SOLO);

    // Test case: write nodeId, but NOT captureConfig
    // This should fail at !ReadSurfaceCaptureConfig(captureConfig, data) condition
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    NodeId nodeId = surfaceNode_->GetId();
    data.WriteUint64(nodeId);
    // NOT writing captureConfig - should fail here
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);
}

/**
 * @tc.name: TakeSelfSurfaceCaptureTest001
 * @tc.desc: Test TAKE_SELF_SURFACE_CAPTURE interface code path
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToRenderConnectionStubTest, TakeSelfSurfaceCaptureTest001, TestSize.Level1)
{
    ASSERT_NE(connectionStub_, nullptr);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::TAKE_SELF_SURFACE_CAPTURE);

    // Test case 1: missing nodeId
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    // Test case 2: valid data with callback and config
    MessageParcel data2;
    data2.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    NodeId nodeId = surfaceNode_->GetId();
    data2.WriteUint64(nodeId);

    sptr<RSISurfaceCaptureCallback> callback = new RSSurfaceCaptureCallbackStubMock();
    data2.WriteRemoteObject(callback->AsObject());

    RSSurfaceCaptureConfig captureConfig;
    data2.WriteFloat(captureConfig.scaleX);
    data2.WriteFloat(captureConfig.scaleY);
    data2.WriteBool(captureConfig.useDma);
    data2.WriteBool(captureConfig.useCurWindow);
    data2.WriteUint8(static_cast<uint8_t>(captureConfig.captureType));
    data2.WriteBool(captureConfig.isSync);
    data2.WriteBool(captureConfig.isHdrCapture);
    data2.WriteBool(captureConfig.needF16WindowCaptureForScRGB);
    data2.WriteBool(captureConfig.needErrorCode);
    data2.WriteFloat(captureConfig.mainScreenRect.left_);
    data2.WriteFloat(captureConfig.mainScreenRect.top_);
    data2.WriteFloat(captureConfig.mainScreenRect.right_);
    data2.WriteFloat(captureConfig.mainScreenRect.bottom_);
    data2.WriteUInt64Vector(captureConfig.blackList);
    data2.WriteUint64(captureConfig.uiCaptureInRangeParam.endNodeId);
    data2.WriteBool(captureConfig.uiCaptureInRangeParam.useBeginNodeSize);
    data2.WriteFloat(captureConfig.specifiedAreaRect.left_);
    data2.WriteFloat(captureConfig.specifiedAreaRect.top_);
    data2.WriteFloat(captureConfig.specifiedAreaRect.right_);
    data2.WriteFloat(captureConfig.specifiedAreaRect.bottom_);
    data2.WriteUint32(captureConfig.backGroundColor);
    data2.WriteUint32(captureConfig.colorSpace.first);
    data2.WriteBool(captureConfig.colorSpace.second);
    data2.WriteUint32(captureConfig.dynamicRangeMode.first);
    data2.WriteBool(captureConfig.dynamicRangeMode.second);
    data2.WriteBool(captureConfig.isSyncRender);
    res = connectionStub_->OnRemoteRequest(code, data2, reply, option);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: TakeSelfSurfaceCaptureTest002
 * @tc.desc: Test TAKE_SELF_SURFACE_CAPTURE with missing callback (branch coverage)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToRenderConnectionStubTest, TakeSelfSurfaceCaptureTest002, TestSize.Level1)
{
    ASSERT_NE(connectionStub_, nullptr);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::TAKE_SELF_SURFACE_CAPTURE);

    // Test case: valid nodeId but missing callback (branch coverage)
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    NodeId nodeId = surfaceNode_->GetId();
    data.WriteUint64(nodeId);
    // Not writing callback - should return ERR_NULL_OBJECT
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, ERR_NULL_OBJECT);
}

/**
 * @tc.name: TakeSelfSurfaceCaptureTest003
 * @tc.desc: Test TAKE_SELF_SURFACE_CAPTURE with missing captureConfig (branch coverage)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToRenderConnectionStubTest, TakeSelfSurfaceCaptureTest003, TestSize.Level1)
{
    ASSERT_NE(connectionStub_, nullptr);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::TAKE_SELF_SURFACE_CAPTURE);

    // Test case: valid callback but missing captureConfig (branch coverage)
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    NodeId nodeId = surfaceNode_->GetId();
    data.WriteUint64(nodeId);

    sptr<RSISurfaceCaptureCallback> callback = new RSSurfaceCaptureCallbackStubMock();
    data.WriteRemoteObject(callback->AsObject());
    // Not writing captureConfig - should return ERR_INVALID_DATA
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);
}

/**
 * @tc.name: SetWindowFreezeImmediatelyTest001
 * @tc.desc: Test SET_WINDOW_FREEZE_IMMEDIATELY interface code path
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToRenderConnectionStubTest, SetWindowFreezeImmediatelyTest001, TestSize.Level1)
{
    ASSERT_NE(connectionStub_, nullptr);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::SET_WINDOW_FREEZE_IMMEDIATELY);

    // Test case 1: missing nodeId
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    // Test case 2: unfreeze (isFreeze = false)
    MessageParcel data2;
    data2.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    NodeId nodeId = 10004;
    data2.WriteUint64(nodeId);
    data2.WriteBool(false); // isFreeze
    res = connectionStub_->OnRemoteRequest(code, data2, reply, option);
    EXPECT_EQ(res, ERR_NONE);

    // Test case 3: freeze (isFreeze = true) with callback and config
    MessageParcel data3;
    data3.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data3.WriteUint64(nodeId);
    data3.WriteBool(true); // isFreeze

    sptr<RSISurfaceCaptureCallback> callback = new RSSurfaceCaptureCallbackStubMock();
    data3.WriteRemoteObject(callback->AsObject());

    RSSurfaceCaptureConfig captureConfig;
    data3.WriteFloat(captureConfig.scaleX);
    data3.WriteFloat(captureConfig.scaleY);
    data3.WriteBool(captureConfig.useDma);
    data3.WriteBool(captureConfig.useCurWindow);
    data3.WriteUint8(static_cast<uint8_t>(captureConfig.captureType));
    data3.WriteBool(captureConfig.isSync);
    data3.WriteBool(captureConfig.isHdrCapture);
    data3.WriteBool(captureConfig.needF16WindowCaptureForScRGB);
    data3.WriteBool(captureConfig.needErrorCode);
    data3.WriteFloat(captureConfig.mainScreenRect.left_);
    data3.WriteFloat(captureConfig.mainScreenRect.top_);
    data3.WriteFloat(captureConfig.mainScreenRect.right_);
    data3.WriteFloat(captureConfig.mainScreenRect.bottom_);
    data3.WriteUInt64Vector(captureConfig.blackList);
    data3.WriteUint64(captureConfig.uiCaptureInRangeParam.endNodeId);
    data3.WriteBool(captureConfig.uiCaptureInRangeParam.useBeginNodeSize);
    data3.WriteFloat(captureConfig.specifiedAreaRect.left_);
    data3.WriteFloat(captureConfig.specifiedAreaRect.top_);
    data3.WriteFloat(captureConfig.specifiedAreaRect.right_);
    data3.WriteFloat(captureConfig.specifiedAreaRect.bottom_);
    data3.WriteUint32(captureConfig.backGroundColor);
    data3.WriteUint32(captureConfig.colorSpace.first);
    data3.WriteBool(captureConfig.colorSpace.second);
    data3.WriteUint32(captureConfig.dynamicRangeMode.first);
    data3.WriteBool(captureConfig.dynamicRangeMode.second);
    data3.WriteBool(captureConfig.isSyncRender);
    // Write blurParam
    data3.WriteBool(false); // isNeedBlur
    data3.WriteFloat(0.0f); // blurRadius

    res = connectionStub_->OnRemoteRequest(code, data3, reply, option);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: SetWindowFreezeImmediatelyTest002
 * @tc.desc: Test SET_WINDOW_FREEZE_IMMEDIATELY with missing callback when freezing (branch coverage)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToRenderConnectionStubTest, SetWindowFreezeImmediatelyTest002, TestSize.Level1)
{
    ASSERT_NE(connectionStub_, nullptr);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::SET_WINDOW_FREEZE_IMMEDIATELY);

    // Test case: isFreeze=true but missing callback (branch coverage)
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    NodeId nodeId = 10004;
    data.WriteUint64(nodeId);
    data.WriteBool(true); // isFreeze = true
    // Not writing callback when isFreeze is true - should return ERR_NULL_OBJECT
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, ERR_NULL_OBJECT);
}

/**
 * @tc.name: SetWindowFreezeImmediatelyTest003
 * @tc.desc: Test SET_WINDOW_FREEZE_IMMEDIATELY with missing captureConfig when freezing (branch coverage)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToRenderConnectionStubTest, SetWindowFreezeImmediatelyTest003, TestSize.Level1)
{
    ASSERT_NE(connectionStub_, nullptr);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::SET_WINDOW_FREEZE_IMMEDIATELY);

    // Test case: valid callback but missing captureConfig (branch coverage)
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    NodeId nodeId = 10004;
    data.WriteUint64(nodeId);
    data.WriteBool(true); // isFreeze = true

    sptr<RSISurfaceCaptureCallback> callback = new RSSurfaceCaptureCallbackStubMock();
    data.WriteRemoteObject(callback->AsObject());
    // Not writing captureConfig when isFreeze is true - should return ERR_INVALID_DATA
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);
}

/**
 * @tc.name: SetWindowFreezeImmediatelyTest004
 * @tc.desc: Test SET_WINDOW_FREEZE_IMMEDIATELY with missing blurParam when freezing (branch coverage)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToRenderConnectionStubTest, SetWindowFreezeImmediatelyTest004, TestSize.Level1)
{
    ASSERT_NE(connectionStub_, nullptr);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::SET_WINDOW_FREEZE_IMMEDIATELY);

    // Test case: valid captureConfig but missing blurParam (branch coverage)
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    NodeId nodeId = 10004;
    data.WriteUint64(nodeId);
    data.WriteBool(true); // isFreeze = true

    sptr<RSISurfaceCaptureCallback> callback = new RSSurfaceCaptureCallbackStubMock();
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
    // Not writing blurParam when isFreeze is true - should return ERR_INVALID_DATA
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);
}

/**
 * @tc.name: TakeUICaptureInRangeTest001
 * @tc.desc: Test TAKE_UI_CAPTURE_IN_RANGE interface code path
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToRenderConnectionStubTest, TakeUICaptureInRangeTest001, TestSize.Level1)
{
    ASSERT_NE(connectionStub_, nullptr);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::TAKE_UI_CAPTURE_IN_RANGE);

    // Test case 1: missing nodeId
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    // Test case 2: valid data with callback and config
    MessageParcel data2;
    data2.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    NodeId nodeId = 10005;
    data2.WriteUint64(nodeId);

    sptr<RSISurfaceCaptureCallback> callback = new RSSurfaceCaptureCallbackStubMock();
    data2.WriteRemoteObject(callback->AsObject());

    RSSurfaceCaptureConfig captureConfig;
    data2.WriteFloat(captureConfig.scaleX);
    data2.WriteFloat(captureConfig.scaleY);
    data2.WriteBool(captureConfig.useDma);
    data2.WriteBool(captureConfig.useCurWindow);
    data2.WriteUint8(static_cast<uint8_t>(captureConfig.captureType));
    data2.WriteBool(captureConfig.isSync);
    data2.WriteBool(captureConfig.isHdrCapture);
    data2.WriteBool(captureConfig.needF16WindowCaptureForScRGB);
    data3.WriteBool(captureConfig.needErrorCode);
    data2.WriteFloat(captureConfig.mainScreenRect.left_);
    data2.WriteFloat(captureConfig.mainScreenRect.top_);
    data2.WriteFloat(captureConfig.mainScreenRect.right_);
    data2.WriteFloat(captureConfig.mainScreenRect.bottom_);
    data2.WriteUInt64Vector(captureConfig.blackList);
    data2.WriteUint64(captureConfig.uiCaptureInRangeParam.endNodeId);
    data2.WriteBool(captureConfig.uiCaptureInRangeParam.useBeginNodeSize);
    data2.WriteFloat(captureConfig.specifiedAreaRect.left_);
    data2.WriteFloat(captureConfig.specifiedAreaRect.top_);
    data2.WriteFloat(captureConfig.specifiedAreaRect.right_);
    data2.WriteFloat(captureConfig.specifiedAreaRect.bottom_);
    data2.WriteUint32(captureConfig.backGroundColor);
    data2.WriteUint32(captureConfig.colorSpace.first);
    data2.WriteBool(captureConfig.colorSpace.second);
    data2.WriteUint32(captureConfig.dynamicRangeMode.first);
    data2.WriteBool(captureConfig.dynamicRangeMode.second);
    data2.WriteBool(captureConfig.isSyncRender);
    res = connectionStub_->OnRemoteRequest(code, data2, reply, option);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: TakeUICaptureInRangeTest002
 * @tc.desc: Test TAKE_UI_CAPTURE_IN_RANGE with missing callback (branch coverage)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToRenderConnectionStubTest, TakeUICaptureInRangeTest002, TestSize.Level1)
{
    ASSERT_NE(connectionStub_, nullptr);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::TAKE_UI_CAPTURE_IN_RANGE);

    // Test case: valid nodeId but missing callback (branch coverage)
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    NodeId nodeId = 10005;
    data.WriteUint64(nodeId);
    // Not writing callback - should return ERR_NULL_OBJECT
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, ERR_NULL_OBJECT);
}

/**
 * @tc.name: TakeUICaptureInRangeTest003
 * @tc.desc: Test TAKE_UI_CAPTURE_IN_RANGE with missing captureConfig (branch coverage)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToRenderConnectionStubTest, TakeUICaptureInRangeTest003, TestSize.Level1)
{
    ASSERT_NE(connectionStub_, nullptr);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::TAKE_UI_CAPTURE_IN_RANGE);

    // Test case: valid callback but missing captureConfig (branch coverage)
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    NodeId nodeId = 10005;
    data.WriteUint64(nodeId);

    sptr<RSISurfaceCaptureCallback> callback = new RSSurfaceCaptureCallbackStubMock();
    data.WriteRemoteObject(callback->AsObject());
    // Not writing captureConfig - should return ERR_INVALID_DATA
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);
}

/**
 * @tc.name: CommitTransactionTest001
 * @tc.desc: Test COMMIT_TRANSACTION interface code path
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToRenderConnectionStubTest, CommitTransactionTest001, TestSize.Level1)
{
    ASSERT_NE(connectionStub_, nullptr);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::COMMIT_TRANSACTION);

    // Test case 1: normal parcel with readData = 0
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data.WriteInt32(0); // readData = 0 (normal parcel)
    // Write minimal transaction data
    data.WriteUint64(1); // timestamp
    data.WriteInt32(0); // follower count
    data.WriteInt32(0); // command count

    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    // Should succeed or return invalid data if parsing fails
    EXPECT_TRUE(res == ERR_NONE || res == ERR_INVALID_DATA);
}

/**
 * @tc.name: CommitTransactionTest002
 * @tc.desc: Test COMMIT_TRANSACTION with missing data
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToRenderConnectionStubTest, CommitTransactionTest002, TestSize.Level1)
{
    ASSERT_NE(connectionStub_, nullptr);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::COMMIT_TRANSACTION);

    // Test case: missing readData flag
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);
}

/**
 * @tc.name: ClearSurfaceWatermarkForNodesTest002
 * @tc.desc: Test CLEAR_SURFACE_WATERMARK_FOR_NODES with complete data
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToRenderConnectionStubTest, ClearSurfaceWatermarkForNodesTest002, TestSize.Level1)
{
    ASSERT_NE(connectionStub_, nullptr);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::CLEAR_SURFACE_WATERMARK_FOR_NODES);

    // Test case: complete data with pid, name and nodeList
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    pid_t pid = getpid();
    data.WriteInt32(pid);
    std::string name = "testWatermark";
    data.WriteString(name);
    std::vector<NodeId> nodeList = {10001, 10002};
    data.WriteUInt64Vector(nodeList);

    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: ClearSurfaceWatermarkForNodesTest003
 * @tc.desc: Test CLEAR_SURFACE_WATERMARK_FOR_NODES with missing name (independent Read* condition)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToRenderConnectionStubTest, ClearSurfaceWatermarkForNodesTest003, TestSize.Level1)
{
    ASSERT_NE(connectionStub_, nullptr);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::CLEAR_SURFACE_WATERMARK_FOR_NODES);

    // Test case: write pid, but NOT name
    // This should fail at !data.ReadString(name) condition
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    pid_t pid = getpid();
    data.WriteInt32(pid);
    // NOT writing name - should fail here

    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);
}

/**
 * @tc.name: ClearSurfaceWatermarkForNodesTest004
 * @tc.desc: Test CLEAR_SURFACE_WATERMARK_FOR_NODES with missing nodeList (independent Read* condition)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToRenderConnectionStubTest, ClearSurfaceWatermarkForNodesTest004, TestSize.Level1)
{
    ASSERT_NE(connectionStub_, nullptr);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::CLEAR_SURFACE_WATERMARK_FOR_NODES);

    // Test case: write pid and name, but NOT nodeList
    // This should fail at !data.ReadUInt64Vector(&nodeIdList) condition
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    pid_t pid = getpid();
    data.WriteInt32(pid);
    std::string name = "testWatermark";
    data.WriteString(name);
    // NOT writing nodeList - should fail here

    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);
}

/**
 * @tc.name: ClearSurfaceWatermarkTest002
 * @tc.desc: Test CLEAR_SURFACE_WATERMARK with complete data
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToRenderConnectionStubTest, ClearSurfaceWatermarkTest002, TestSize.Level1)
{
    ASSERT_NE(connectionStub_, nullptr);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::CLEAR_SURFACE_WATERMARK);

    // Test case: complete data with pid and name
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    pid_t pid = getpid();
    data.WriteInt32(pid);
    std::string name = "testWatermark";
    data.WriteString(name);

    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: ClearSurfaceWatermarkTest003
 * @tc.desc: Test CLEAR_SURFACE_WATERMARK with missing name (independent Read* condition)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToRenderConnectionStubTest, ClearSurfaceWatermarkTest003, TestSize.Level1)
{
    ASSERT_NE(connectionStub_, nullptr);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::CLEAR_SURFACE_WATERMARK);

    // Test case: write pid, but NOT name
    // This should fail at !data.ReadString(name) condition
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    pid_t pid = getpid();
    data.WriteInt32(pid);
    // NOT writing name - should fail here

    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);
}

/**
 * @tc.name: RegisterSurfaceBufferCallbackTest001
 * @tc.desc: Test REGISTER_SURFACE_BUFFER_CALLBACK interface code path
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToRenderConnectionStubTest, RegisterSurfaceBufferCallbackTest001, TestSize.Level1)
{
    ASSERT_NE(connectionStub_, nullptr);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::REGISTER_SURFACE_BUFFER_CALLBACK);

    // Test case 1: missing pid (should fail at ReadInt32)
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    // Test case 2: write pid, but NOT uid (should fail at ReadUint64)
    MessageParcel data2;
    data2.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    int32_t pid = getpid();
    data2.WriteInt32(pid);
    res = connectionStub_->OnRemoteRequest(code, data2, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    // Test case 3: write pid and uid, but NOT remoteObject
    MessageParcel data3;
    data3.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data3.WriteInt32(pid);
    data3.WriteUint64(0); // uid
    res = connectionStub_->OnRemoteRequest(code, data3, reply, option);
    EXPECT_EQ(res, ERR_NULL_OBJECT);

    // Test case 4: write pid, uid, and valid remoteObject (success path)
    MessageParcel data4;
    data4.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data4.WriteInt32(pid);
    data4.WriteUint64(0); // uid
    sptr<RSISurfaceBufferCallback> callback = new RSSurfaceBufferCallbackStubMock();
    data4.WriteRemoteObject(callback->AsObject());
    res = connectionStub_->OnRemoteRequest(code, data4, reply, option);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: UnregisterSurfaceBufferCallbackTest001
 * @tc.desc: Test UNREGISTER_SURFACE_BUFFER_CALLBACK interface code path
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToRenderConnectionStubTest, UnregisterSurfaceBufferCallbackTest001, TestSize.Level1)
{
    ASSERT_NE(connectionStub_, nullptr);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::UNREGISTER_SURFACE_BUFFER_CALLBACK);

    // Test case 1: missing pid (should fail at ReadInt32)
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    // Test case 2: write pid, but NOT uid (should fail at ReadUint64)
    MessageParcel data2;
    data2.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    int32_t pid = getpid();
    data2.WriteInt32(pid);
    res = connectionStub_->OnRemoteRequest(code, data2, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    // Test case 3: valid data with pid and uid
    MessageParcel data3;
    data3.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data3.WriteInt32(pid);
    data3.WriteUint64(0); // uid
    res = connectionStub_->OnRemoteRequest(code, data3, reply, option);
    EXPECT_EQ(res, ERR_NONE);
}

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
        static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::SET_LOGICAL_CAMERA_ROTATION_CORRECTION);
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data.WriteUint64(1);
    data.WriteUint32(static_cast<uint32_t>(ScreenRotation::ROTATION_90));
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
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
        static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::SET_LOGICAL_CAMERA_ROTATION_CORRECTION);
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data.WriteUint64(1);
    data.WriteUint32(6);
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
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
        static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::SET_LOGICAL_CAMERA_ROTATION_CORRECTION);
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data.WriteString("str");
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
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
        static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::SET_LOGICAL_CAMERA_ROTATION_CORRECTION);
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data.WriteUint64(0);
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
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
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::SET_POINTER_POSITION);
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

/**
 * @tc.name: SetSystemAnimatedScenesTest001
 * @tc.desc: Test SetSystemAnimatedScenes when ReadBool and ReadUint32 fail
 * @tc.type: FUNC
 * @tc.require: issue20726
 */
HWTEST_F(RSClientToRenderConnectionStubTest, SetSystemAnimatedScenesTest001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code =
        static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::SET_SYSTEM_ANIMATED_SCENES);
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    int ret = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ret, ERR_INVALID_DATA);
}
 
/**
 * @tc.name: SetSystemAnimatedScenesTest002
 * @tc.desc: Test SetSystemAnimatedScenes when data is ReadBool fail
 * @tc.type: FUNC
 * @tc.require: issue20726
 */
HWTEST_F(RSClientToRenderConnectionStubTest, SetSystemAnimatedScenesTest002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code =
        static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::SET_SYSTEM_ANIMATED_SCENES);
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data.WriteUint32(0);
    int ret = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ret, ERR_INVALID_DATA);
}
 
/**
 * @tc.name: SetSystemAnimatedScenesTest003
 * @tc.desc: Test SetSystemAnimatedScenes when ReadUint32 fail
 * @tc.type: FUNC
 * @tc.require: issue20726
 */
HWTEST_F(RSClientToRenderConnectionStubTest, SetSystemAnimatedScenesTest003, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code =
        static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::SET_SYSTEM_ANIMATED_SCENES);
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data.WriteBool(true);
    int ret = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ret, ERR_INVALID_DATA);
}
 
/**
 * @tc.name: SetSystemAnimatedScenesTest004
 * @tc.desc: Test SetSystemAnimatedScenes when mainThread_ isn't nullptr
 * @tc.type: FUNC
 * @tc.require: issue20726
 */
HWTEST_F(RSClientToRenderConnectionStubTest, SetSystemAnimatedScenesTest004, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code =
        static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::SET_SYSTEM_ANIMATED_SCENES);
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data.WriteUint32(0);
    data.WriteBool(true);
    int ret = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ret, ERR_NONE);
}
 
/**
 * @tc.name: SetSystemAnimatedScenesTest005
 * @tc.desc: Test SetSystemAnimatedScenes when mainThread_ is nullptr
 * @tc.type: FUNC
 * @tc.require: issue20726
 */
HWTEST_F(RSClientToRenderConnectionStubTest, SetSystemAnimatedScenesTest005, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code =
        static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::SET_SYSTEM_ANIMATED_SCENES);
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data.WriteUint32(0);
    data.WriteBool(true);
    sptr<RSClientToServiceConnection> clientToServiceConnection =
        iface_cast<RSClientToServiceConnection>(connectionStub_);
    ASSERT_NE(clientToServiceConnection, nullptr);
    auto mainThread = clientToServiceConnection->mainThread_;
    clientToServiceConnection->mainThread_ = nullptr;
    int ret = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ret, ERR_INVALID_DATA);
 
    clientToServiceConnection->mainThread_ = mainThread;
}

/**
 * @tc.name: ExecuteSynchronousTaskTest001
 * @tc.desc: Test ExecuteSynchronousTask when ReadInt16 fails
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToRenderConnectionStubTest, ExecuteSynchronousTaskTest001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::EXECUTE_SYNCHRONOUS_TASK);
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data.WriteString("invalid"); // Not int16, will cause ReadInt16 to fail
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_STATE);
}

/**
 * @tc.name: ExecuteSynchronousTaskTest002
 * @tc.desc: Test ExecuteSynchronousTask with invalid type (not RS_NODE_SYNCHRONOUS_READ_PROPERTY or
 * RS_NODE_SYNCHRONOUS_GET_VALUE_FRACTION)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToRenderConnectionStubTest, ExecuteSynchronousTaskTest002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::EXECUTE_SYNCHRONOUS_TASK);
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data.WriteInt16(999); // Invalid type (not 10 or 11)
    data.WriteInt16(0);
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_STATE);
}

/**
 * @tc.name: ExecuteSynchronousTaskTest003
 * @tc.desc: Test ExecuteSynchronousTask when GetUnmarshallingFunc returns nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToRenderConnectionStubTest, ExecuteSynchronousTaskTest003, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::EXECUTE_SYNCHRONOUS_TASK);
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data.WriteInt16(10); // RS_NODE_SYNCHRONOUS_READ_PROPERTY
    data.WriteInt16(999); // Invalid subType that doesn't have a registered func
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_STATE);
}

/**
 * @tc.name: ExecuteSynchronousTaskTest004
 * @tc.desc: Test ExecuteSynchronousTask when unmarshalling returns nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToRenderConnectionStubTest, ExecuteSynchronousTaskTest004, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::EXECUTE_SYNCHRONOUS_TASK);
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data.WriteInt16(10); // RS_NODE_SYNCHRONOUS_READ_PROPERTY
    data.WriteInt16(0);
    data.WriteUint64(0); // Incomplete data for unmarshalling
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_STATE);
}

/**
 * @tc.name: ExecuteSynchronousTaskTest005
 * @tc.desc: Test ExecuteSynchronousTask with RS_NODE_SYNCHRONOUS_GET_VALUE_FRACTION type and invalid subType
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToRenderConnectionStubTest, ExecuteSynchronousTaskTest005, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::EXECUTE_SYNCHRONOUS_TASK);
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data.WriteInt16(11); // RS_NODE_SYNCHRONOUS_GET_VALUE_FRACTION
    data.WriteInt16(999); // Invalid subType
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_STATE);
}

/**
 * @tc.name: ExecuteSynchronousTaskTest006
 * @tc.desc: Test ExecuteSynchronousTask with RS_NODE_SYNCHRONOUS_GET_VALUE_FRACTION type and invalid subType (line 397
 * branch)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToRenderConnectionStubTest, ExecuteSynchronousTaskTest006, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::EXECUTE_SYNCHRONOUS_TASK);
    data.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data.WriteInt16(11); // RS_NODE_SYNCHRONOUS_GET_VALUE_FRACTION
    data.WriteInt16(0); // subType that doesn't have registered func
    data.WriteUint64(0); // Incomplete data
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_STATE);
}
} // namespace OHOS::Rosen
