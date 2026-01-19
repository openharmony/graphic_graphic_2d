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
#include "ipc_callbacks/rs_canvas_surface_buffer_callback_stub.h"
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
#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif
using namespace testing;
using namespace testing::ext;

namespace {
constexpr const int WAIT_HANDLER_TIME = 1; // 1s
constexpr const int WAIT_HANDLER_TIME_COUNT = 5;
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
std::shared_ptr<RSSurfaceRenderNode> RSClientToRenderConnectionStubTest::surfaceNode_ =
    std::shared_ptr<RSSurfaceRenderNode>(new RSSurfaceRenderNode(10003, std::make_shared<RSContext>(), true),
    RSRenderNodeGC::NodeDestructor);

void RSClientToRenderConnectionStubTest::SetUpTestCase()
{
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

    renderPipeline_->uniRenderThread_->handler_ = nullptr;
    renderPipeline_->uniRenderThread_->runner_ = nullptr;

    hdiOutput_ = nullptr;
    screenManager_ = nullptr;
    hdiDeviceMock_ = nullptr;
    token_ = nullptr;
    connectionStub_ = nullptr;
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
        Media::PixelMap* pixelmap, Media::PixelMap* pixelmapHDR = nullptr) override {};
};

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
class RSCanvasSurfaceBufferCallbackStubMock : public RSCanvasSurfaceBufferCallbackStub {
public:
    RSCanvasSurfaceBufferCallbackStubMock() = default;
    virtual ~RSCanvasSurfaceBufferCallbackStubMock() = default;
    void OnCanvasSurfaceBufferChanged(NodeId nodeId, sptr<SurfaceBuffer> buffer, uint32_t resetSurfaceIndex) override {}
    bool IsProxyObject() const override
    {
        return true;
    }
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
    data.WriteUInt64Vector(captureConfig.blackList);
    data.WriteUint32(captureConfig.backGroundColor);
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

    res = connectionStub_->OnRemoteRequest(code, data2, reply, option);

    ASSERT_EQ(res, ERR_INVALID_DATA);
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
        static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::TAKE_SURFACE_CAPTURE)), ERR_NULL_OBJECT);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::TAKE_UI_CAPTURE_IN_RANGE)), ERR_NULL_OBJECT);
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
    auto res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_LE(res, ERR_PERMISSION_DENIED);
}

// /**
//  * @tc.name: TakeSurfaceCaptureWithAllWindowsTest002
//  * @tc.desc: Test TakeSurfaceCaptureWithAllWindows for task execution
//  * @tc.type: FUNC
//  * @tc.require: issueICQ74B
//  */
// HWTEST_F(RSClientToRenderConnectionStubTest, TakeSurfaceCaptureWithAllWindowsTest002, TestSize.Level2)
// {
//     constexpr uint32_t TIME_OF_CAPTURE_TASK = 100000;
//     auto handler = RSMainThread::Instance()->handler_;
//     auto runner_ = AppExecFwk::EventRunner::Create("TaskSurfaceCaptureWithAllWindowsTest002");
//     ASSERT_NE(runner_, nullptr);
//     RSMainThread::Instance()->handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
//     ASSERT_NE(RSMainThread::Instance()->handler_, nullptr);
//     runner_->Run();
//     sptr<RSClientToRenderConnection> connection = iface_cast<RSClientToRenderConnection>(connectionStub_);
//     ASSERT_NE(connection, nullptr);
//     auto mainThread = connection->mainThread_;
//     connection->mainThread_ = nullptr;
//     RSSurfaceCaptureConfig captureConfig;
//     RSSurfaceCapturePermissions permissions;
//     auto ret = connection->TakeSurfaceCaptureWithAllWindows(0, nullptr, captureConfig, false, permissions);
//     EXPECT_EQ(ret, ERR_PERMISSION_DENIED);
//     sptr<RSISurfaceCaptureCallback> callback = new RSSurfaceCaptureCallbackStubMock();
//     ASSERT_NE(callback, nullptr);
//     ret = connection->TakeSurfaceCaptureWithAllWindows(0, callback, captureConfig, false, permissions);
//     EXPECT_EQ(ret, ERR_PERMISSION_DENIED);

//     ASSERT_NE(mainThread, nullptr);
//     connection->mainThread_ = mainThread;
//     permissions.screenCapturePermission = false;
//     permissions.isSystemCalling = false;
//     ret = connection->TakeSurfaceCaptureWithAllWindows(0, nullptr, captureConfig, false, permissions);
//     EXPECT_EQ(ret, ERR_PERMISSION_DENIED);
//     permissions.screenCapturePermission = true;
//     permissions.isSystemCalling = false;
//     ret = connection->TakeSurfaceCaptureWithAllWindows(0, callback, captureConfig, false, permissions);
//     EXPECT_EQ(ret, ERR_PERMISSION_DENIED);

//     permissions.screenCapturePermission = true;
//     permissions.isSystemCalling = true;
//     ret = connection->TakeSurfaceCaptureWithAllWindows(0, nullptr, captureConfig, false, permissions);
//     usleep(TIME_OF_CAPTURE_TASK);
//     EXPECT_EQ(ret, ERR_NONE);
//     ret = connection->TakeSurfaceCaptureWithAllWindows(0, callback, captureConfig, false, permissions);
//     usleep(TIME_OF_CAPTURE_TASK);
//     EXPECT_EQ(ret, ERR_NONE);

//     ASSERT_NE(connection->mainThread_, nullptr);
//     NodeId displayNodeId = 111;
//     RSDisplayNodeConfig displayNodeConfig;
//     std::shared_ptr<RSLogicalDisplayRenderNode> displayNode = std::shared_ptr<RSLogicalDisplayRenderNode>(
//         new RSLogicalDisplayRenderNode(displayNodeId, displayNodeConfig), RSRenderNodeGC::NodeDestructor);
//     ASSERT_NE(displayNode, nullptr);
//     auto& nodeMap = connection->mainThread_->GetContext().GetMutableNodeMap();
//     EXPECT_TRUE(nodeMap.RegisterRenderNode(displayNode));
//     ret = connection->TakeSurfaceCaptureWithAllWindows(displayNodeId, nullptr, captureConfig, false, permissions);
//     usleep(TIME_OF_CAPTURE_TASK);
//     EXPECT_EQ(ret, ERR_NONE);
//     ret = connection->TakeSurfaceCaptureWithAllWindows(displayNodeId, callback, captureConfig, false, permissions);
//     usleep(TIME_OF_CAPTURE_TASK);
//     EXPECT_EQ(ret, ERR_NONE);
//     ret = connection->TakeSurfaceCaptureWithAllWindows(displayNodeId, callback, captureConfig, false, permissions);
//     usleep(TIME_OF_CAPTURE_TASK);
//     EXPECT_EQ(ret, ERR_NONE);

//     RSMainThread::Instance()->handler_ = handler;
// }

/**
 * @tc.name: TakeSurfaceCaptureWithAllWindowsTest003
 * @tc.desc: Test TakeSurfaceCaptureWithAllWindows for failures
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

// /**
//  * @tc.name: FreezeScreenTest001
//  * @tc.desc: Test FreezeScreen to freeze or unfreeze screen
//  * @tc.type: FUNC
//  * @tc.require: issueICS2J8
//  */
// HWTEST_F(RSClientToRenderConnectionStubTest, FreezeScreenTest001, TestSize.Level2)
// {
//     constexpr uint32_t TIME_OF_FREEZE_TASK = 100000;
//     ASSERT_NE(connectionStub_, nullptr);
//     MessageParcel data1;
//     MessageParcel reply;
//     MessageOption option;
//     uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::FREEZE_SCREEN);
//     data1.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
//     data1.WriteBool(false);
//     int res = connectionStub_->OnRemoteRequest(code, data1, reply, option);
//     ASSERT_EQ(res, ERR_INVALID_DATA);

//     MessageParcel data2;
//     data2.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
//     data2.WriteUint64(1);
//     res = connectionStub_->OnRemoteRequest(code, data2, reply, option);
//     ASSERT_EQ(res, ERR_INVALID_DATA);

//     MessageParcel data3;
//     data3.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
//     data3.WriteUint64(1);
//     data3.WriteBool(true);
//     res = connectionStub_->OnRemoteRequest(code, data3, reply, option);
//     ASSERT_EQ(res, ERR_OK);

//     auto handler = RSMainThread::Instance()->handler_;
//     auto runner_ = AppExecFwk::EventRunner::Create("TaskSurfaceCaptureWithAllWindowsTest002");
//     ASSERT_NE(runner_, nullptr);
//     RSMainThread::Instance()->handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
//     ASSERT_NE(RSMainThread::Instance()->handler_, nullptr);
//     runner_->Run();
//     sptr<RSClientToRenderConnection> connection = iface_cast<RSClientToRenderConnection>(connectionStub_);
//     ASSERT_NE(connection, nullptr);
//     auto mainThread = connection->mainThread_;
//     connection->mainThread_ = nullptr;
//     res = connection->FreezeScreen(0, false);
//     ASSERT_EQ(res, ERR_INVALID_OPERATION);

//     ASSERT_NE(mainThread, nullptr);
//     connection->mainThread_ = mainThread;
//     res = connection->FreezeScreen(0, false);
//     usleep(TIME_OF_FREEZE_TASK);
//     ASSERT_EQ(res, ERR_OK);

//     NodeId displayNodeId = 1123;
//     NodeId screenNodeId = 4456;
//     ScreenId screenId = 1;
//     RSDisplayNodeConfig displayNodeConfig;
//     std::shared_ptr<RSLogicalDisplayRenderNode> displayNode = std::shared_ptr<RSLogicalDisplayRenderNode>(
//         new RSLogicalDisplayRenderNode(displayNodeId, displayNodeConfig), RSRenderNodeGC::NodeDestructor);
//     ASSERT_NE(displayNode, nullptr);
//     std::shared_ptr<RSScreenRenderNode> screenNode = std::shared_ptr<RSScreenRenderNode>(
//         new RSScreenRenderNode(screenNodeId, screenId), RSRenderNodeGC::NodeDestructor);
//     ASSERT_NE(screenNode, nullptr);
//     displayNode->SetParent(screenNode);
//     auto& nodeMap = connection->mainThread_->GetContext().GetMutableNodeMap();
//     EXPECT_TRUE(nodeMap.RegisterRenderNode(displayNode));
//     EXPECT_TRUE(nodeMap.RegisterRenderNode(screenNode));
//     res = connection->FreezeScreen(displayNodeId, false);
//     usleep(TIME_OF_FREEZE_TASK);
//     ASSERT_EQ(res, ERR_OK);

//     RSMainThread::Instance()->handler_ = handler;
// }

// /**
//  * @tc.name: FreezeScreenTest002
//  * @tc.desc: Test FreezeScreen to freeze or unfreeze screen
//  * @tc.type: FUNC
//  * @tc.require: issueICUQ08
//  */
// HWTEST_F(RSClientToRenderConnectionStubTest, FreezeScreenTest002, TestSize.Level2)
// {
//     constexpr uint32_t TIME_OF_FREEZE_TASK = 100000;
//     ASSERT_NE(connectionStub_, nullptr);
//     auto handler = RSMainThread::Instance()->handler_;
//     auto runner_ = AppExecFwk::EventRunner::Create("FreezeScreenTest002");
//     ASSERT_NE(runner_, nullptr);
//     RSMainThread::Instance()->handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
//     ASSERT_NE(RSMainThread::Instance()->handler_, nullptr);
//     runner_->Run();
//     sptr<RSClientToRenderConnection> connection = iface_cast<RSClientToRenderConnection>(connectionStub_);
//     ASSERT_NE(connection, nullptr);
//     NodeId displayNodeId = 896;
//     NodeId screenNodeId = 54;
//     ScreenId screenId = 1;
//     RSDisplayNodeConfig displayNodeConfig;
//     std::shared_ptr<RSLogicalDisplayRenderNode> displayNode = std::shared_ptr<RSLogicalDisplayRenderNode>(
//         new RSLogicalDisplayRenderNode(displayNodeId, displayNodeConfig), RSRenderNodeGC::NodeDestructor);
//     ASSERT_NE(displayNode, nullptr);
//     std::shared_ptr<RSScreenRenderNode> screenNode = std::shared_ptr<RSScreenRenderNode>(
//         new RSScreenRenderNode(screenNodeId, screenId), RSRenderNodeGC::NodeDestructor);
//     ASSERT_NE(screenNode, nullptr);
//     displayNode->SetParent(screenNode);
//     auto& nodeMap = connection->mainThread_->GetContext().GetMutableNodeMap();
//     EXPECT_TRUE(nodeMap.RegisterRenderNode(displayNode));
//     EXPECT_TRUE(nodeMap.RegisterRenderNode(screenNode));
//     auto res = connection->FreezeScreen(displayNodeId, false);
//     usleep(TIME_OF_FREEZE_TASK);
//     ASSERT_EQ(res, ERR_OK);
//     res = connection->FreezeScreen(displayNodeId, true);
//     usleep(TIME_OF_FREEZE_TASK);
//     ASSERT_EQ(res, ERR_OK);

//     RSMainThread::Instance()->handler_ = handler;
// }

// /**
//  * @tc.name: TakeSurfaceCaptureWithAllWindowsTest004
//  * @tc.desc: Test TakeSurfaceCaptureWithAllWindows for drm/surfacelock
//  * @tc.type: FUNC
//  * @tc.require: issueICUQ08
//  */
// HWTEST_F(RSClientToRenderConnectionStubTest, TakeSurfaceCaptureWithAllWindowsTest004, TestSize.Level2)
// {
//     constexpr uint32_t TIME_OF_CAPTURE_TASK = 100000;
//     auto handler = RSMainThread::Instance()->handler_;
//     auto runner_ = AppExecFwk::EventRunner::Create("TaskSurfaceCaptureWithAllWindowsTest004");
//     ASSERT_NE(runner_, nullptr);
//     RSMainThread::Instance()->handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
//     ASSERT_NE(RSMainThread::Instance()->handler_, nullptr);
//     runner_->Run();
//     sptr<RSClientToRenderConnection> connection = iface_cast<RSClientToRenderConnection>(connectionStub_);
//     ASSERT_NE(connection, nullptr);

//     RSSurfaceCaptureConfig captureConfig;
//     RSSurfaceCapturePermissions permissions;
//     permissions.screenCapturePermission = true;
//     permissions.isSystemCalling = true;
//     NodeId displayNodeId = 45;
//     RSDisplayNodeConfig displayNodeConfig;
//     std::shared_ptr<RSLogicalDisplayRenderNode> displayNode = std::shared_ptr<RSLogicalDisplayRenderNode>(
//         new RSLogicalDisplayRenderNode(displayNodeId, displayNodeConfig), RSRenderNodeGC::NodeDestructor);
//     ASSERT_NE(displayNode, nullptr);
//     auto& nodeMap = connection->mainThread_->GetContext().GetMutableNodeMap();
//     EXPECT_TRUE(nodeMap.RegisterRenderNode(displayNode));
//     auto ret = connection->TakeSurfaceCaptureWithAllWindows(displayNodeId, nullptr, captureConfig, false, permissions);
//     usleep(TIME_OF_CAPTURE_TASK);
//     EXPECT_EQ(ret, ERR_NONE);
//     RSMainThread::Instance()->SetHasSurfaceLockLayer(false);
//     RSMainThread::Instance()->hasProtectedLayer_ = false;
//     RSMainThread::Instance()->hasSurfaceLockLayer_ = false;
//     EXPECT_FALSE(RSMainThread::Instance()->HasDRMOrSurfaceLockLayer());
//     ret = connection->TakeSurfaceCaptureWithAllWindows(displayNodeId, nullptr, captureConfig, true, permissions);
//     usleep(TIME_OF_CAPTURE_TASK);
//     EXPECT_EQ(ret, ERR_NONE);
//     RSMainThread::Instance()->hasProtectedLayer_ = true;
//     RSMainThread::Instance()->hasSurfaceLockLayer_ = true;
//     ret = connection->TakeSurfaceCaptureWithAllWindows(displayNodeId, nullptr, captureConfig, true, permissions);
//     usleep(TIME_OF_CAPTURE_TASK);
//     EXPECT_EQ(ret, ERR_NONE);
//     sptr<RSISurfaceCaptureCallback> callback = new RSSurfaceCaptureCallbackStubMock();
//     ret = connection->TakeSurfaceCaptureWithAllWindows(displayNodeId, callback, captureConfig, true, permissions);
//     usleep(TIME_OF_CAPTURE_TASK);
//     EXPECT_EQ(ret, ERR_NONE);

//     RSMainThread::Instance()->handler_ = handler;
// }

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

    // Scenario 1: Cannot read enableReadRemoteObject - should fail with ERR_INVALID_DATA
    MessageParcel data1;
    data1.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    int res = toRenderConnectionStub_->OnRemoteRequest(code, data1, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);

    // Scenario 2: enableReadRemoteObject = false, callback will be nullptr
    MessageParcel data2;
    data2.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data2.WriteBool(false);
    res = toRenderConnectionStub_->OnRemoteRequest(code, data2, reply, option);
    ASSERT_EQ(res, ERR_NONE);

    // Scenario 3: enableReadRemoteObject = true with valid callback
    MessageParcel data3;
    data3.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    data3.WriteBool(true);
    sptr<RSICanvasSurfaceBufferCallback> callback = new RSCanvasSurfaceBufferCallbackStubMock();
    data3.WriteRemoteObject(callback->AsObject());
    res = toRenderConnectionStub_->OnRemoteRequest(code, data3, reply, option);
    ASSERT_EQ(res, ERR_NONE);
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
} // namespace OHOS::Rosen
