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
#include "common/rs_background_thread.h"
#include <iremote_stub.h>
#include <message_option.h>
#include <message_parcel.h>
#include <parameters.h>

#include "gtest/gtest.h"
#include "limit_number.h"
#include "mock_hdi_device.h"
#include "sandbox_utils.h"
#include "ipc_callbacks/rs_frame_rate_linker_expected_fps_update_callback_stub.h"
#include "ipc_callbacks/rs_iframe_rate_linker_expected_fps_update_callback_ipc_interface_code.h"
#include "ipc_callbacks/rs_iframe_rate_linker_expected_fps_update_callback.h"
#include "ipc_callbacks/screen_change_callback_stub.h"
#include "ipc_callbacks/brightness_info_change_callback_stub.h"
#include "pipeline/render_thread/rs_composer_adapter.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "render_server/transaction/rs_client_to_service_connection.h"
#include "pipeline/rs_logical_display_render_node.h"
#include "pipeline/rs_render_node_gc.h"
#include "pipeline/rs_screen_render_node.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "ipc_callbacks/rs_transaction_data_callback_stub.h"
#include "platform/ohos/transaction/zidl/rs_irender_service.h"
#include "screen_manager/screen_types.h"
#include "render_server/transaction/zidl/rs_client_to_service_connection_stub.h"
#include "screen_manager/rs_screen.h"
#include "feature/capture/rs_capture_pixelmap_manager.h"
#include "feature/colorpicker/rs_color_picker_thread.h"
#include "render_process/transaction/rs_service_to_render_connection.h"
#include "render_server/transaction/rs_render_to_service_connection.h"
#include "render_service/composer/composer_client/connection/rs_composer_to_render_connection.h"
#include "transaction/rs_connect_to_render_process.h"
#include "ipc_callbacks/surface_capture_callback_stub.h"
#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif
using namespace testing;
using namespace testing::ext;

namespace {
constexpr const int DEFAULT_WIDTH = 2160;
constexpr const int DEFAULT_HEIGHT = 1080;
constexpr const int INVALID_PIDLIST_SIZE = 200;
constexpr const int WAIT_HANDLER_TIME = 1; // 1S
constexpr const int WAIT_HANDLER_TIME_COUNT = 5;
};

namespace OHOS::Rosen {
namespace {
class MockRSBrightnessInfoChangeCallback : public IRemoteProxy<RSIBrightnessInfoChangeCallback> {
public:
    explicit MockRSBrightnessInfoChangeCallback() : IRemoteProxy<RSIBrightnessInfoChangeCallback>(nullptr) {};
    virtual ~MockRSBrightnessInfoChangeCallback() noexcept = default;

    void OnBrightnessInfoChange(ScreenId screenId, const BrightnessInfo& brightnessInfo) override {}
};
class RSSingleRenderProcessManagerMock : public RSRenderProcessManager {

public:
    RSSingleRenderProcessManagerMock(RSRenderService& renderService) :
        RSRenderProcessManager(renderService)
    {
        // step1: Create renderPipeline and following connections
        auto renderServiceAgent = sptr<RSRenderServiceAgent>::MakeSptr(renderService);
        auto renderProcessManagerAgent =
            sptr<RSRenderProcessManagerAgent>::MakeSptr(renderService.renderProcessManager_);
        auto screenManagerAgent = sptr<RSScreenManagerAgent>::MakeSptr(renderService.screenManager_);

        renderToServiceConnection_ =
            sptr<RSRenderToServiceConnection>::MakeSptr(renderServiceAgent, renderProcessManagerAgent, screenManagerAgent);
        renderService.renderPipeline_ = std::make_shared<RSRenderPipeline>();
        
        auto mainThread = RSMainThread::Instance();
        mainThread->RegisterScreenSwitchFinishCallback(renderToServiceConnection_);
        renderService.renderPipeline_->mainThread_ = mainThread;
        renderService.renderPipeline_->mainThread_->handler_ = renderService.handler_;

        renderService_.renderPipeline_->uniRenderThread_ = &(RSUniRenderThread::Instance());
        auto runner = OHOS::AppExecFwk::EventRunner::Create(true);
        renderService_.renderPipeline_->uniRenderThread_->runner_ = runner;
        renderService_.renderPipeline_->uniRenderThread_->handler_ = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner);
        renderService_.renderPipeline_->uniRenderThread_->runner_ ->Run();

        auto renderPipelineAgent = sptr<RSRenderPipelineAgent>::MakeSptr(renderService_.renderPipeline_);
        serviceToRenderConnection_ = sptr<RSServiceToRenderConnection>::MakeSptr(renderPipelineAgent);
        composerToRenderConnection_ = sptr<RSComposerToRenderConnection>::MakeSptr();

        // step3:
        connectToRenderConnection_ = sptr<RSConnectToRenderProcess>::MakeSptr(renderPipelineAgent);
    }

    ~RSSingleRenderProcessManagerMock() noexcept override = default;

    sptr<IRemoteObject> OnScreenConnected(ScreenId screenId,
        const std::shared_ptr<HdiOutput>& output, const sptr<RSScreenProperty>& property)
    {
        auto composerConn = renderService_.rsRenderComposerManager_->GetRSComposerConnection(property->GetScreenId());
        renderService_.renderPipeline_->OnScreenConnected(property, composerConn, composerToRenderConnection_,
            renderService_.rsVsyncManagerAgent_, output);
        return connectToRenderConnection_->AsObject();
    }

    void OnScreenDisconnected(ScreenId id)
    {
        renderService_.renderPipeline_->OnScreenDisconnected(id);
    }

    void OnScreenPropertyChanged(
        ScreenId id, ScreenPropertyType type, const sptr<ScreenPropertyBase>& property)
    {
        renderService_.renderPipeline_->OnScreenPropertyChanged(id, type, property);
    }
    
    void OnScreenRefresh(ScreenId id)
    {
        renderService_.renderPipeline_->OnScreenRefresh(id);
    }

    void OnVirtualScreenConnected(ScreenId id, ScreenId associatedScreenId,
        const sptr<RSScreenProperty>& property)
    {
        renderService_.renderPipeline_->OnScreenConnected(property, nullptr, nullptr, nullptr, nullptr);
    }

    void OnVirtualScreenDisconnected(ScreenId id)
    {
        renderService_.renderPipeline_->OnScreenDisconnected(id);
    }

    sptr<RSIServiceToRenderConnection> GetServiceToRenderConn(ScreenId screenId) const override
    {
        return serviceToRenderConnection_;
    }

    std::vector<sptr<RSIServiceToRenderConnection>> GetServiceToRenderConns() const override
    {
        return { serviceToRenderConnection_ };
    }

    sptr<RSIConnectToRenderProcess> GetConnectToRenderConnection(ScreenId screenId) const override
    {
        return connectToRenderConnection_;
    }
    sptr<RSIServiceToRenderConnection> serviceToRenderConnection_ = nullptr;
    sptr<IRSComposerToRenderConnection> composerToRenderConnection_ = nullptr;
    sptr<RSIRenderToServiceConnection> renderToServiceConnection_ = nullptr;
    sptr<RSIConnectToRenderProcess> connectToRenderConnection_ = nullptr;
};
} // namespace

class RSClientToServiceConnectionStubTest : public testing::Test {
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
    static inline sptr<RSScreenManager> screenManager_ = sptr<RSScreenManager>::MakeSptr();
    static inline std::shared_ptr<HdiOutput> hdiOutput_;
    float mirrorAdaptiveCoefficient = 1.0f;
    static uint32_t screenId_;
    static std::shared_ptr<RSSurfaceRenderNode> surfaceNode_;
private:
    int OnRemoteRequestTest(uint32_t code);
    static void WaitHandlerTask();
    static inline sptr<RSIConnectionToken> token_;
    static inline sptr<RSClientToServiceConnectionStub> connectionStub_;
    static inline sptr<RSRenderServiceAgent> renderServiceAgent_;
    static inline sptr<RSRenderProcessManagerAgent> renderProcessManagerAgent_;
    static inline RSRenderService renderService_;
    static inline sptr<RSScreenManagerAgent> screenManagerAgent_;
    static inline sptr<VSyncDistributor> appVSyncDistributor_;
};

uint32_t RSClientToServiceConnectionStubTest::screenId_ = 0;
std::shared_ptr<RSSurfaceRenderNode> RSClientToServiceConnectionStubTest::surfaceNode_ =
    std::shared_ptr<RSSurfaceRenderNode>(new RSSurfaceRenderNode(10003, std::make_shared<RSContext>(), true),
    RSRenderNodeGC::NodeDestructor);

void RSClientToServiceConnectionStubTest::SetUpTestCase()
{
#ifdef RS_ENABLE_VK
    RsVulkanContext::SetRecyclable(false);
#endif
    hdiOutput_ = HdiOutput::CreateHdiOutput(screenId_);
    auto rsScreen = std::make_shared<RSScreen>(screenId_);
    screenManager_->MockHdiScreenConnected(rsScreen);
    hdiDeviceMock_ = Mock::HdiDeviceMock::GetInstance();
    EXPECT_CALL(*hdiDeviceMock_, RegHotPlugCallback(_, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*hdiDeviceMock_, RegHwcDeadCallback(_, _)).WillRepeatedly(testing::Return(false));
    EXPECT_CALL(*hdiDeviceMock_, RegRefreshCallback(_, _)).WillRepeatedly(testing::Return(0));
    auto runner = OHOS::AppExecFwk::EventRunner::Create(true);
    renderService_.handler_ = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner);
    runner->Run();
    renderService_.renderProcessManager_ = sptr<RSSingleRenderProcessManagerMock>::MakeSptr(renderService_);
    
    renderServiceAgent_ = sptr<RSRenderServiceAgent>::MakeSptr(renderService_);
    renderProcessManagerAgent_ = sptr<RSRenderProcessManagerAgent>::MakeSptr(renderService_.renderProcessManager_);
    screenManagerAgent_ = new RSScreenManagerAgent(screenManager_);

    token_ = new OHOS::IRemoteStub<OHOS::Rosen::RSIConnectionToken>();
    connectionStub_ =
        sptr<RSClientToServiceConnection>::MakeSptr(0, renderServiceAgent_,
            renderProcessManagerAgent_, screenManagerAgent_, token_->AsObject(), nullptr);
}

void RSClientToServiceConnectionStubTest::TearDownTestCase()
{
#if defined(RS_ENABLE_UNI_RENDER) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    RSBackgroundThread::Instance().gpuContext_ = nullptr;
    RSBackgroundThread::Instance().renderContext_ = nullptr;
#endif
    RSBackgroundThread::Instance().runner_->Stop();
    RSBackgroundThread::Instance().runner_ = nullptr;
    RSBackgroundThread::Instance().handler_ = nullptr;
#if defined(RS_ENABLE_UNI_RENDER) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    RSColorPickerThread::Instance().gpuContext_ = nullptr;
    RSColorPickerThread::Instance().renderContext_ = nullptr;
#endif
    RSColorPickerThread::Instance().runner_->Stop();
    RSColorPickerThread::Instance().runner_ = nullptr;
    RSColorPickerThread::Instance().handler_ = nullptr;

    WaitHandlerTask();

    renderService_.handler_->eventRunner_->Stop();
    renderService_.renderPipeline_->mainThread_->handler_->eventRunner_->Stop();
    renderService_.renderPipeline_->uniRenderThread_->runner_->Stop();

    renderService_.renderPipeline_->mainThread_->handler_ = nullptr;
    renderService_.renderPipeline_->mainThread_->receiver_ = nullptr;
    renderService_.renderPipeline_->mainThread_->renderEngine_ = nullptr;

    renderService_.renderPipeline_->uniRenderThread_->handler_ = nullptr;
    renderService_.renderPipeline_->uniRenderThread_->runner_ = nullptr;
    renderService_.renderPipeline_->uniRenderThread_->uniRenderEngine_ = nullptr;
    renderService_.renderPipeline_->uniRenderThread_ = nullptr;
    renderService_.renderPipeline_ = nullptr;
    renderService_.rsVSyncDistributor_ = nullptr;
    hdiOutput_ = nullptr;
    composerAdapter_ = nullptr;
    screenManager_ = nullptr;
    hdiDeviceMock_ = nullptr;
    token_ = nullptr;
    connectionStub_ = nullptr;
}

void RSClientToServiceConnectionStubTest::SetUp()
{
    CreateComposerAdapterWithScreenInfo(DEFAULT_WIDTH, DEFAULT_HEIGHT,
        ScreenColorGamut::COLOR_GAMUT_SRGB, ScreenState::UNKNOWN, ScreenRotation::ROTATION_0);
    // RSHardwareThread::Instance().Start();

    auto mainThread = RSMainThread::Instance();
    if (!mainThread) {
        return;
    }
    auto runner_ = AppExecFwk::EventRunner::Create("RSClientToServiceConnectionStubTest");
    if (!runner_) {
        return;
    }
    mainThread->handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
    runner_->Run();
}
void RSClientToServiceConnectionStubTest::TearDown() {}
int RSClientToServiceConnectionStubTest::OnRemoteRequestTest(uint32_t code)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    return connectionStub_->OnRemoteRequest(code, data, reply, option);
}

void RSClientToServiceConnectionStubTest::WaitHandlerTask()
{
    auto count = 0;
    auto isMainThreadRunning = !renderService_.renderPipeline_->mainThread_->handler_->IsIdle();
    auto isUniRenderThreadRunning = !renderService_.renderPipeline_->uniRenderThread_->handler_->IsIdle();
    while (count < WAIT_HANDLER_TIME_COUNT && (isMainThreadRunning || isUniRenderThreadRunning)) {
        std::this_thread::sleep_for(std::chrono::seconds(WAIT_HANDLER_TIME));
    }

    if (count >= WAIT_HANDLER_TIME_COUNT) {
        renderService_.renderPipeline_->mainThread_->handler_->RemoveAllEvents();
        renderService_.renderPipeline_->uniRenderThread_->handler_->RemoveAllEvents();
    }
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

void RSClientToServiceConnectionStubTest::CreateComposerAdapterWithScreenInfo(uint32_t width, uint32_t height,
    ScreenColorGamut colorGamut, ScreenState state, ScreenRotation rotation)
{
    ScreenInfo info;
    info.phyWidth = width;
    info.phyHeight = height;
    info.colorGamut = colorGamut;
    info.width = width;
    info.height = height;
    info.state = state;
    info.rotation = rotation;
    composerAdapter_ = std::make_unique<RSComposerAdapter>();
    composerAdapter_->Init(info, mirrorAdaptiveCoefficient, nullptr, nullptr);
    composerAdapter_->SetHdiBackendDevice(hdiDeviceMock_);
}

/**
 * @tc.name: TestRSRenderServiceConnectionStub002
 * @tc.desc: Test screen related transaction, with non empty data.
 * @tc.type: FUNC
 * @tc.require: issueI60KUK
 */
HWTEST_F(RSClientToServiceConnectionStubTest, TestRSRenderServiceConnectionStub002, TestSize.Level1)
{
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_DEFAULT_SCREEN_ID)), ERR_NONE);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_ACTIVE_SCREEN_ID)), ERR_NONE);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_ALL_SCREEN_IDS)), ERR_NONE);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_CHANGE_CALLBACK)), ERR_NULL_OBJECT);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_ACTIVE_MODE)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_REFRESH_RATE)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_SCREEN_CURRENT_REFRESH_RATE)),
        ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_REFRESH_RATES)),
        ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_POWER_STATUS)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_BACK_LIGHT)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_SCREEN_ACTIVE_MODE)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_CORRECTION)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_SCREEN_TYPE)), ERR_INVALID_DATA);
}

/**
 * @tc.name: TestRSRenderServiceConnectionStub003
 * @tc.desc: Test virtual screen related transaction, with non empty data.
 * @tc.type: FUNC
 * @tc.require: issueI60KUK
 */
HWTEST_F(RSClientToServiceConnectionStubTest, TestRSRenderServiceConnectionStub003, TestSize.Level1)
{
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_SURFACE)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_BLACKLIST)),
        ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(static_cast<uint32_t>(
                  RSIClientToServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_SECURITY_EXEMPTION_LIST)),
        ERR_INVALID_STATE);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_CAST_SCREEN_ENABLE_SKIP_WINDOW)),
        ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::REMOVE_VIRTUAL_SCREEN)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::DISABLE_RENDER_CONTROL_SCREEN)),
        ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_VIRTUAL_SCREEN_RESOLUTION)),
        ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::SET_VIRTUAL_MIRROR_SCREEN_CANVAS_ROTATION)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_AUTO_ROTATION)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::SET_VIRTUAL_MIRROR_SCREEN_SCALE_MODE)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_USING_STATUS)),
        ERR_INVALID_DATA);
}

/**
 * @tc.name: TestRSRenderServiceConnectionStub004
 * @tc.desc: Test refresh rate related transaction, with non empty data.
 * @tc.type: FUNC
 * @tc.require: issueI60KUK
 */
HWTEST_F(RSClientToServiceConnectionStubTest, TestRSRenderServiceConnectionStub004, TestSize.Level1)
{
    EXPECT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_REFRESH_RATE_MODE)), ERR_INVALID_DATA);
    EXPECT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_CURRENT_REFRESH_RATE_MODE)),
        ERR_NONE);
    EXPECT_EQ(OnRemoteRequestTest(static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::GET_SHOW_REFRESH_RATE_ENABLED)), ERR_NONE);
    EXPECT_EQ(OnRemoteRequestTest(static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::SET_SHOW_REFRESH_RATE_ENABLED)), ERR_INVALID_DATA);
    EXPECT_EQ(OnRemoteRequestTest(static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::GET_REALTIME_REFRESH_RATE)), ERR_INVALID_DATA);
#ifdef RS_ENABLE_UNI_RENDER
    EXPECT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_REFRESH_INFO)), ERR_INVALID_DATA);
#else
    EXPECT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_REFRESH_INFO)), ERR_INVALID_DATA);
#endif
}

/**
 * @tc.name: TestRSRenderServiceConnectionStub005
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, TestRSRenderServiceConnectionStub005, TestSize.Level1)
{
    EXPECT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::NEED_REGISTER_TYPEFACE)), ERR_INVALID_DATA);
    EXPECT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_REFRESH_RATE)),
        ERR_INVALID_DATA);
    EXPECT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_WATERMARK)), ERR_INVALID_DATA);
    EXPECT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::NOTIFY_LIGHT_FACTOR_STATUS)), ERR_INVALID_DATA);
    EXPECT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_VMA_CACHE_STATUS)), ERR_INVALID_DATA);
    EXPECT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_STATUS)), ERR_INVALID_DATA);
    EXPECT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_LAYER_TOP)), ERR_INVALID_STATE);
    EXPECT_EQ(OnRemoteRequestTest(
                  static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::ADD_VIRTUAL_SCREEN_BLACKLIST)),
        ERR_INVALID_DATA);
    EXPECT_EQ(OnRemoteRequestTest(
                  static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::REMOVE_VIRTUAL_SCREEN_BLACKLIST)),
        ERR_INVALID_DATA);
    EXPECT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_FORCE_REFRESH)), ERR_INVALID_STATE);
}

/**
 * @tc.name: TestRSRenderServiceConnectionStub006
 * @tc.desc: Test register/unregister callback related transaction, with non empty data.
 * @tc.type: FUNC
 * @tc.require: issueI60KUK
 */
HWTEST_F(RSClientToServiceConnectionStubTest, TestRSRenderServiceConnectionStub006, TestSize.Level1)
{
    EXPECT_EQ(OnRemoteRequestTest(static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::REGISTER_OCCLUSION_CHANGE_CALLBACK)), ERR_NULL_OBJECT);
    // Unable to access IPC due to lack of permissions.
    EXPECT_EQ(OnRemoteRequestTest(static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::REGISTER_SURFACE_OCCLUSION_CHANGE_CALLBACK)), ERR_INVALID_DATA);
    // Unable to access IPC due to lack of permissions.
    EXPECT_EQ(OnRemoteRequestTest(static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::UNREGISTER_SURFACE_OCCLUSION_CHANGE_CALLBACK)), ERR_INVALID_DATA);
    EXPECT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::REGISTER_HGM_CFG_CALLBACK)), ERR_NULL_OBJECT);
    EXPECT_EQ(OnRemoteRequestTest(static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::REFRESH_RATE_MODE_CHANGE_CALLBACK)), ERR_NULL_OBJECT);
    EXPECT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::REFRESH_RATE_UPDATE_CALLBACK)),
        ERR_INVALID_DATA);
    EXPECT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::ON_FIRST_FRAME_COMMIT)),
        ERR_INVALID_DATA);
    EXPECT_EQ(OnRemoteRequestTest(static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::REGISTER_UIEXTENSION_CALLBACK)), ERR_INVALID_DATA);
}

/**
 * @tc.name: TestRSRenderServiceConnectionStub007
 * @tc.desc: Test render pipeline related transaction (node/dirty region/hwc etc.), with non empty data.
 * @tc.type: FUNC
 * @tc.require: issueI60KUK
 */
HWTEST_F(RSClientToServiceConnectionStubTest, TestRSRenderServiceConnectionStub007, TestSize.Level1)
{
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_UNI_RENDER_ENABLED)), ERR_NONE);
    ASSERT_EQ(OnRemoteRequestTest(static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::MARK_POWER_OFF_NEED_PROCESS_ONE_FRAME)), ERR_NONE);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::CREATE_VSYNC_CONNECTION)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::CREATE_PIXEL_MAP_FROM_SURFACE)), ERR_NULL_OBJECT);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::NOTIFY_TOUCH_EVENT)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::SET_ROTATION_CACHE_ENABLED)), IPC_STUB_INVALID_DATA_ERR);
#ifdef TP_FEATURE_ENABLE
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_TP_FEATURE_CONFIG)), ERR_INVALID_STATE);
#endif
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::REGISTER_TYPEFACE)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::UNREGISTER_TYPEFACE)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::REGISTER_SHARED_TYPEFACE)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_CURTAIN_SCREEN_USING_STATUS)),
        ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_ACTIVE_DIRTY_REGION_INFO)), ERR_NONE);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_GLOBAL_DIRTY_REGION_INFO)), ERR_NONE);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_LAYER_COMPOSE_INFO)), ERR_NONE);
    ASSERT_EQ(OnRemoteRequestTest(static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::GET_HARDWARE_COMPOSE_DISABLED_REASON_INFO)), ERR_NONE);
}

/**
 * @tc.name: TestRSRenderServiceConnectionStub008
 * @tc.desc: Test performance/memory related ipc, with non empty data.
 * @tc.type: FUNC
 * @tc.require: issueI60KUK
 */
HWTEST_F(RSClientToServiceConnectionStubTest, TestRSRenderServiceConnectionStub008, TestSize.Level1)
{
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_MEMORY_GRAPHIC)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_MEMORY_GRAPHICS)), ERR_INVALID_REPLY);
    // ASSERT_EQ(OnRemoteRequestTest(
    //     static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_TOTAL_APP_MEM_SIZE)), ERR_NONE);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::REPORT_JANK_STATS)), ERR_NONE);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::REPORT_EVENT_RESPONSE)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::REPORT_EVENT_COMPLETE)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::REPORT_EVENT_JANK_FRAME)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::REPORT_EVENT_GAMESTATE)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
                  static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::UNREGISTER_FRAME_RATE_LINKER)),
        ERR_INVALID_DATA);
}

/**
 * @tc.name: TestRSRenderServiceConnectionStub009
 * @tc.desc: Test if the code not exists.
 * @tc.type: FUNC
 * @tc.require: issueIABHAX
 */
HWTEST_F(RSClientToServiceConnectionStubTest, TestRSRenderServiceConnectionStub009, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    int res = connectionStub_->OnRemoteRequest(-1, data, reply, option);
    ASSERT_EQ(res, IPC_STUB_UNKNOW_TRANS_ERR);
}

/**
 * @tc.name: TestRSRenderServiceConnectionStub010
 * @tc.desc: Test if the code not exists.
 * @tc.type: FUNC
 * @tc.require: issueIB31K8
 */
HWTEST_F(RSClientToServiceConnectionStubTest, TestRSRenderServiceConnectionStub010, TestSize.Level1)
{
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_FREE_MULTI_WINDOW_STATUS)),
        ERR_INVALID_DATA);
}

/**
 * @tc.name: TestRSRenderServiceConnectionStub011
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, TestRSRenderServiceConnectionStub011, TestSize.Level1)
{
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_SCREEN_BACK_LIGHT)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_SCREEN_DATA)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_GAMUTS)),
        ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_METADATAKEYS)),
        ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_SCREEN_GAMUT)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_GAMUT)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_GAMUT_MAP)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_SCREEN_GAMUT_MAP)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_PIXEL_FORMAT)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_PIXEL_FORMAT)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_HDR_FORMATS)),
        ERR_INVALID_DATA);
    if (RSUniRenderJudgement::IsUniRender()) {
        ASSERT_EQ(OnRemoteRequestTest(static_cast<uint32_t>(
                      RSIClientToServiceConnectionInterfaceCode::REGISTER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK)),
            ERR_NULL_OBJECT);
    }
}

/**
 * @tc.name: TestRSRenderServiceConnectionStub012
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, TestRSRenderServiceConnectionStub012, TestSize.Level1)
{
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_SCREEN_HDR_FORMAT)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_HDR_FORMAT)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_COLORSPACES)),
        ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_SCREEN_COLORSPACE)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_COLORSPACE)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_SKIP_FRAME_INTERVAL)),
        ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::RESIZE_VIRTUAL_SCREEN)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::NOTIFY_PACKAGE_EVENT)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::NOTIFY_APP_STRATEGY_CONFIG_CHANGE_EVENT)),
        ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::NOTIFY_REFRESH_RATE_EVENT)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::NOTIFY_WINDOW_EXPECTED_BY_WINDOW_ID)),
        ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::NOTIFY_WINDOW_EXPECTED_BY_VSYNC_NAME)),
        ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::NOTIFY_SOFT_VSYNC_EVENT)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::NOTIFY_SOFT_VSYNC_RATE_DISCOUNT_EVENT)),
        ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::NOTIFY_XCOMPONENT_EXPECTED_FRAMERATE)),
        ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::REGISTER_FRAME_RATE_LINKER_EXPECTED_FPS_CALLBACK)),
        ERR_INVALID_DATA);
    // ASSERT_EQ(OnRemoteRequestTest(
    //     static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_PIXELMAP_BY_PROCESSID)), ERR_NONE);
    ASSERT_EQ(OnRemoteRequestTest(static_cast<uint32_t>(
        RSIFrameRateLinkerExpectedFpsUpdateCallbackInterfaceCode::ON_FRAME_RATE_LINKER_EXPECTED_FPS_UPDATE)),
        ERR_INVALID_DATA);
}

/**
 * @tc.name: TestRSRenderServiceConnectionStub013
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require: issueIBJFIK
 */
 HWTEST_F(RSClientToServiceConnectionStubTest, TestRSRenderServiceConnectionStub013, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::NOTIFY_SCREEN_SWITCHED);
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);
}

/**
 * @tc.name: TestRSRenderServiceConnectionStub014
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSClientToServiceConnectionStubTest, TestRSRenderServiceConnectionStub014, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::DISABLE_RENDER_CONTROL_SCREEN);
    data.WriteUint64(1);
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, NO_ERROR);
}

/**
 * @tc.name: TestRSRenderServiceConnectionStub015
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSClientToServiceConnectionStubTest, TestRSRenderServiceConnectionStub015, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::DISABLE_RENDER_CONTROL_SCREEN);
    data.WriteUint64(1);
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, NO_ERROR);
}

/**
 * @tc.name: TestRSRenderServiceConnectionStub016
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSClientToServiceConnectionStubTest, TestRSRenderServiceConnectionStub016, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::ADD_VIRTUAL_SCREEN_BLACKLIST);
    data.WriteUint64(1);
    data.WriteUInt64Vector({1});
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, NO_ERROR);
}

/**
 * @tc.name: TestRSRenderServiceConnectionStub017
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSClientToServiceConnectionStubTest, TestRSRenderServiceConnectionStub017, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::REMOVE_VIRTUAL_SCREEN_BLACKLIST);
    data.WriteUint64(1);
    data.WriteUInt64Vector({1});
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, NO_ERROR);
}

// /**
//  * @tc.name: TestRSRenderServiceConnectionStub018
//  * @tc.desc: Test
//  * @tc.type: FUNC
//  * @tc.require: issueIBRN69
//  */
// HWTEST_F(RSClientToServiceConnectionStubTest, TestRSRenderServiceConnectionStub018, TestSize.Level1)
// {
//     MessageParcel data;
//     MessageParcel reply;
//     MessageOption option;
//     data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
//     uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_CHANGE_CALLBACK);
//     sptr<RSScreenChangeCallbackStubMock> callback = new RSScreenChangeCallbackStubMock();
//     data.WriteRemoteObject(callback->AsObject());
//     int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
//     ASSERT_EQ(res, NO_ERROR);
// }

/**
 * @tc.name: TestRSRenderServiceConnectionStub019
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSClientToServiceConnectionStubTest, TestRSRenderServiceConnectionStub019, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_ACTIVE_MODE);
    data.WriteUint64(0);
    data.WriteUint32(1);
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, NO_ERROR);
    // for reply write fail branch
    MessageParcel reply1;
    MessageParcel data1;
    data1.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data1.WriteUint64(0);
    data1.WriteUint32(1);
    reply1.writable_ = false;
    reply1.data_ = nullptr;
    res = connectionStub_->OnRemoteRequest(code, data1, reply1, option);
    ASSERT_EQ(res, ERR_INVALID_REPLY);
}

/**
 * @tc.name: TestRSRenderServiceConnectionStub020
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSClientToServiceConnectionStubTest, TestRSRenderServiceConnectionStub020, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_REFRESH_RATE);
    data.WriteUint64(0);
    data.WriteUint32(1);
    data.WriteUint32(0);
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, NO_ERROR);
}

/**
 * @tc.name: TestRSRenderServiceConnectionStub021
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSClientToServiceConnectionStubTest, TestRSRenderServiceConnectionStub021, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::UNREGISTER_FRAME_RATE_LINKER);
    data.WriteUint64(0);
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);
}

/**
 * @tc.name: TestRSRenderServiceConnectionStub022
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSClientToServiceConnectionStubTest, TestRSRenderServiceConnectionStub022, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::UNREGISTER_FRAME_RATE_LINKER);
    pid_t pid = GetRealPid();
    uint32_t curId = 1;
    NodeId id = ((NodeId)pid << 32) | curId;
    data.WriteUint64(id);
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, NO_ERROR);
}

/**
 * @tc.name: TestRSRenderServiceConnectionStub023
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSClientToServiceConnectionStubTest, TestRSRenderServiceConnectionStub023, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_SCREEN_CURRENT_REFRESH_RATE);
    data.WriteUint64(1);
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, NO_ERROR);
}

/**
 * @tc.name: TestRSRenderServiceConnectionStub024
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSClientToServiceConnectionStubTest, TestRSRenderServiceConnectionStub024, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_REFRESH_RATES);
    data.WriteUint64(1);
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, NO_ERROR);
}

/**
 * @tc.name: TestRSRenderServiceConnectionStub029
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSClientToServiceConnectionStubTest, TestRSRenderServiceConnectionStub029, TestSize.Level1)
{
    int32_t dstPid = 1;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    uint32_t code = static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::REGISTER_FRAME_RATE_LINKER_EXPECTED_FPS_CALLBACK);
    sptr<RSFrameRateLinkerExpectedFpsUpdateCallbackStubMock> callback =
        new RSFrameRateLinkerExpectedFpsUpdateCallbackStubMock();

    data.WriteInt32(dstPid);
    data.WriteBool(true);
    data.WriteRemoteObject(callback->AsObject());
    int32_t res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, NO_ERROR);
}

/**
 * @tc.name: TestRSRenderServiceConnectionStub031
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSClientToServiceConnectionStubTest, TestRSRenderServiceConnectionStub031, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    uint32_t code = static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::REGISTER_FRAME_RATE_LINKER_EXPECTED_FPS_CALLBACK);
    sptr<RSFrameRateLinkerExpectedFpsUpdateCallbackStubMock> callback =
        new RSFrameRateLinkerExpectedFpsUpdateCallbackStubMock();
    data.WriteBool(true);
    int32_t res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_NE(res, NO_ERROR);
}

/**
 * @tc.name: TestRSRenderServiceConnectionStub032
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSClientToServiceConnectionStubTest, TestRSRenderServiceConnectionStub032, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    uint32_t code = static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::REGISTER_FRAME_RATE_LINKER_EXPECTED_FPS_CALLBACK);
    sptr<RSFrameRateLinkerExpectedFpsUpdateCallbackStubMock> callback =
        new RSFrameRateLinkerExpectedFpsUpdateCallbackStubMock();
    data.WriteInt32(0);
    data.WriteBool(true);
    data.WriteRemoteObject(callback->AsObject());
    int32_t res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, NO_ERROR);
}

/**
 * @tc.name: TestRSRenderServiceConnectionStub033
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSClientToServiceConnectionStubTest, TestRSRenderServiceConnectionStub033, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    uint32_t code = static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::REGISTER_FRAME_RATE_LINKER_EXPECTED_FPS_CALLBACK);
    sptr<RSFrameRateLinkerExpectedFpsUpdateCallbackStubMock> callback =
        new RSFrameRateLinkerExpectedFpsUpdateCallbackStubMock();

    data.WriteInt32(0);
    data.WriteBool(false);
    data.WriteRemoteObject(callback->AsObject());
    int32_t res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, NO_ERROR);
}

/**
 * @tc.name: TestRSRenderServiceConnectionStub036
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSClientToServiceConnectionStubTest, TestRSRenderServiceConnectionStub036, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIFrameRateLinkerExpectedFpsUpdateCallback::GetDescriptor());
    uint32_t code = static_cast<uint32_t>(
        RSIFrameRateLinkerExpectedFpsUpdateCallbackInterfaceCode::ON_FRAME_RATE_LINKER_EXPECTED_FPS_UPDATE);
    sptr<RSFrameRateLinkerExpectedFpsUpdateCallbackStubMock> callback =
        new RSFrameRateLinkerExpectedFpsUpdateCallbackStubMock();

    data.WriteInt32(0);
    data.WriteString("");
    data.WriteInt32(0);
    int32_t res = callback->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, NO_ERROR);
}

/**
 * @tc.name: TestRSRenderServiceConnectionStub037
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSClientToServiceConnectionStubTest, TestRSRenderServiceConnectionStub037, TestSize.Level1)
{
    int32_t dstPid = 1;
    int32_t expectedFps = 60;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIFrameRateLinkerExpectedFpsUpdateCallback::GetDescriptor());
    uint32_t code = static_cast<uint32_t>(
        RSIFrameRateLinkerExpectedFpsUpdateCallbackInterfaceCode::ON_FRAME_RATE_LINKER_EXPECTED_FPS_UPDATE);
    sptr<RSFrameRateLinkerExpectedFpsUpdateCallbackStubMock> callback =
        new RSFrameRateLinkerExpectedFpsUpdateCallbackStubMock();

    data.WriteInt32(dstPid);
    data.WriteString("xcomponentId");
    data.WriteInt32(expectedFps);
    int32_t res = callback->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, NO_ERROR);
}

/**
 * @tc.name: TestRSRenderServiceConnectionStub034
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSClientToServiceConnectionStubTest, TestRSRenderServiceConnectionStub034, TestSize.Level1)
{
    int32_t dstPid = 1;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    uint32_t code = static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::NOTIFY_XCOMPONENT_EXPECTED_FRAMERATE);
    sptr<RSScreenChangeCallbackStubMock> callback = new RSScreenChangeCallbackStubMock();

    data.WriteString("xcomponent");
    data.WriteInt32(dstPid);
    int32_t res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, NO_ERROR);
}

/**
 * @tc.name: TestRSRenderServiceConnectionStub035
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSClientToServiceConnectionStubTest, TestRSRenderServiceConnectionStub035, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    uint32_t code = static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::NOTIFY_XCOMPONENT_EXPECTED_FRAMERATE);
    sptr<RSScreenChangeCallbackStubMock> callback = new RSScreenChangeCallbackStubMock();

    data.WriteString("xcomponent");
    int32_t res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_NE(res, NO_ERROR);
}

/**
 * @tc.name: TestRSRenderServiceConnectionStub030
 * @tc.desc: Test ForceRefresh
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSClientToServiceConnectionStubTest, TestRSRenderServiceConnectionStub030, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_FORCE_REFRESH);
    data.WriteString("surface01");
    data.WriteBool(true);
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_STATE);
}

/**
 * @tc.name: NotifyWindowExpectedByWindowIDTest001
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, NotifyWindowExpectedByWindowIDTest001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::NOTIFY_WINDOW_EXPECTED_BY_WINDOW_ID);
    data.WriteUint32(120);
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);
}
 
/**
 * @tc.name: NotifyWindowExpectedByWindowIDTest002
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, NotifyWindowExpectedByWindowIDTest002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::NOTIFY_WINDOW_EXPECTED_BY_WINDOW_ID);
    data.WriteUint32(1);
    data.WriteUint64(0);
    EventInfo eventInfo = {"VOTER1", true, 60, 120, "SCENE1"};
    eventInfo.Serialize(data);
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_NONE);
}
 
/**
 * @tc.name: NotifyWindowExpectedByVsyncNameTest001
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, NotifyWindowExpectedByVsyncNameTest001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    uint32_t code = static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::NOTIFY_WINDOW_EXPECTED_BY_VSYNC_NAME);
    data.WriteUint32(120);
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);
}

/**
 * @tc.name: NotifyWindowExpectedByVsyncNameTest002
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, NotifyWindowExpectedByVsyncNameTest002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    uint32_t code = static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::NOTIFY_WINDOW_EXPECTED_BY_VSYNC_NAME);
    data.WriteUint32(1);
    data.WriteString("vsync1");
    EventInfo eventInfo = {"VOTER1", true, 60, 120, "SCENE1"};
    eventInfo.Serialize(data);
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: GetPidGpuMemoryInMBTest001
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require:issuesICE0QR
 */
HWTEST_F(RSClientToServiceConnectionStubTest, GetPidGpuMemoryInMBTest001, TestSize.Level1)
{
    MessageParcel data;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    uint32_t code = static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::GET_PID_GPU_MEMORY_IN_MB);
    data.WriteUint32(1001);
    MessageParcel reply;
    MessageOption option;
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);
}

/**
 * @tc.name: AvcodecVideoStartTest001
 * @tc.desc: Test AvcodecVideoStart
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, AvcodecVideoStartTest001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::AVCODEC_VIDEO_START);
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);
}

/**
 * @tc.name: AvcodecVideoStartTest002
 * @tc.desc: Test AvcodecVideoStart
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, AvcodecVideoStartTest002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::AVCODEC_VIDEO_START);
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data.WriteUint64(123);
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);
}

/**
 * @tc.name: AvcodecVideoStartTest003
 * @tc.desc: Test AvcodecVideoStart
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, AvcodecVideoStartTest003, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::AVCODEC_VIDEO_START);
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data.WriteUint64(123);
    data.WriteString("surfaceName");
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);
}

/**
 * @tc.name: AvcodecVideoStartTest004
 * @tc.desc: Test AvcodecVideoStart
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, AvcodecVideoStartTest004, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::AVCODEC_VIDEO_START);

    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data.WriteUint64(123);
    data.WriteString("surfaceName");
    data.WriteUint32(60);
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);
}

/**
 * @tc.name: AvcodecVideoStartTest005
 * @tc.desc: Test AvcodecVideoStart
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, AvcodecVideoStartTest005, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::AVCODEC_VIDEO_START);

    std::vector<uint64_t> uniqueIdList = {1};
    std::vector<std::string> surfaceNameList = {"surface1"};
    uint32_t fps = 120;
    uint64_t reportTime = 16;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data.WriteUInt64Vector(uniqueIdList);
    data.WriteStringVector(surfaceNameList);
    data.WriteUint32(fps);
    data.WriteUint64(reportTime);
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: AvcodecVideoStopTest001
 * @tc.desc: Test AvcodecVideoStop
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, AvcodecVideoStopTest001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::AVCODEC_VIDEO_STOP);
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);
}

/**
 * @tc.name: AvcodecVideoStopTest002
 * @tc.desc: Test AvcodecVideoStop
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, AvcodecVideoStopTest002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::AVCODEC_VIDEO_STOP);
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data.WriteUint64(123);
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);
}

/**
 * @tc.name: AvcodecVideoStopTest003
 * @tc.desc: Test AvcodecVideoStop
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, AvcodecVideoStopTest003, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::AVCODEC_VIDEO_STOP);
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data.WriteUint64(123);
    data.WriteString("surfaceName");
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);
}

/**
 * @tc.name: AvcodecVideoStopTest004
 * @tc.desc: Test AvcodecVideoStop
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, AvcodecVideoStopTest004, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::AVCODEC_VIDEO_STOP);
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    std::vector<uint64_t> uniqueIdList = {1};
    std::vector<std::string> surfaceNameList = {"surface1"};
    uint32_t fps = 120;
    data.WriteUInt64Vector(uniqueIdList);
    data.WriteStringVector(surfaceNameList);
    data.WriteUint32(fps);
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: ProfilerServiceOpenFileTest
 * @tc.desc: ProfilerServiceOpenFileTest
 * @tc.type: FUNC
 * @tc.require: issuesIC98WU
 */
HWTEST_F(RSClientToServiceConnectionStubTest, ProfilerServiceOpenFileTest, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::PROFILER_SERVICE_OPEN_FILE);

    HrpServiceDirInfo dirInfo{HrpServiceDir::HRP_SERVICE_DIR_COMMON, "subdir", "subdir2"};
    data.WriteUint32((uint32_t)dirInfo.baseDirType);
    data.WriteString(dirInfo.subDir);
    data.WriteString(dirInfo.subDir2);
    data.WriteString("no_such_filename");
    data.WriteInt32(0);
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, NO_ERROR);
}

/**
 * @tc.name: ProfilerServicePopulateFilesTest
 * @tc.desc: ProfilerServicePopulateFilesTest
 * @tc.type: FUNC
 * @tc.require: issuesIC98WU
 */
HWTEST_F(RSClientToServiceConnectionStubTest, ProfilerServicePopulateFilesTest, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::PROFILER_SERVICE_POPULATE_FILES);

    HrpServiceDirInfo dirInfo{HrpServiceDir::HRP_SERVICE_DIR_COMMON, "never_exist_dir", "never_exist_dir2"};

    data.WriteUint32((uint32_t)dirInfo.baseDirType);
    data.WriteString(dirInfo.subDir);
    data.WriteString(dirInfo.subDir2);
    data.WriteUint32(0);
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, NO_ERROR);
}

/**
 * @tc.name: RegisterSelfDrawingNodeRectChangeCallbackTest002
 * @tc.desc: Test RegisterSelfDrawingNodeRectChangeCallback
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, RegisterSelfDrawingNodeRectChangeCallbackTest002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code =
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::REGISTER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK);
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    uint32_t size = UINT32_MAX;
    data.WriteUint32(size);
    int pid = 0;
    data.WriteInt32(pid);
    data.WriteInt32(0);
    data.WriteInt32(0);
    data.WriteInt32(0);
    data.WriteInt32(0);
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: SetScreenPowerStatusTest001
 * @tc.desc: Test SetScreenPowerStatus when status is invalid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, SetScreenPowerStatusTest001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_POWER_STATUS);
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data.WriteUint64(0);
    data.WriteUint32(static_cast<uint32_t>(ScreenPowerStatus::INVALID_POWER_STATUS) + 1);
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);
}

/**
 * @tc.name: SetScreenPowerStatusTest002
 * @tc.desc: Test SetScreenPowerStatus when status is valid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, SetScreenPowerStatusTest002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_POWER_STATUS);
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data.WriteUint64(0);
    data.WriteUint32(static_cast<uint32_t>(ScreenPowerStatus::INVALID_POWER_STATUS));
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_OK);
}

/**
 * @tc.name: SetRogScreenResolutionTest001
 * @tc.desc: Test SetRogScreenResolution
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, SetRogScreenResolutionTest001, TestSize.Level1)
{
    constexpr uint64_t SCREEN_ID = 0;
    uint32_t width{1920};
    uint32_t height{1080};
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_ROG_SCREEN_RESOLUTION);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data.WriteUint64(SCREEN_ID);
    data.WriteUint32(width);
    data.WriteUint32(height);
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    // Authorization failed
    ASSERT_EQ(res, ERR_INVALID_STATE);
}

/**
 * @tc.name: SetScreenGamutMapTest001
 * @tc.desc: Test SetScreenGamutMap when ReadUint64 failed
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, SetScreenGamutMapTest001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_GAMUT_MAP);
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data.WriteString("str");
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);
}

/**
 * @tc.name: SetScreenGamutMapTest002
 * @tc.desc: Test SetScreenGamutMap when mode is invalid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, SetScreenGamutMapTest002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_GAMUT_MAP);
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data.WriteUint64(0);
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);
}

/**
 * @tc.name: SetScreenGamutMapTest003
 * @tc.desc: Test SetScreenGamutMap when mode is invalid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, SetScreenGamutMapTest003, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_GAMUT_MAP);
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data.WriteUint64(0);
    data.WriteUint32(static_cast<uint32_t>(ScreenGamutMap::GAMUT_MAP_HDR_EXTENSION) + 1);
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);
}

/**
 * @tc.name: SetScreenGamutMapTest004
 * @tc.desc: Test SetScreenGamutMap success
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, SetScreenGamutMapTest004, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_GAMUT_MAP);
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data.WriteUint64(0);
    data.WriteUint32(static_cast<uint32_t>(ScreenGamutMap::GAMUT_MAP_HDR_EXTENSION));
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_OK);
}

// /**
//  * @tc.name: SetBrightnessInfoChangeCallbackTest
//  * @tc.desc: Test SetBrightnessInfoChangeCallback
//  * @tc.type: FUNC
//  * @tc.require:
//  */
// HWTEST_F(RSClientToServiceConnectionStubTest, SetBrightnessInfoChangeCallbackTest, TestSize.Level2)
// {
//     sptr<RSClientToServiceConnectionStub> connectionStub =
//         new RSClientToServiceConnection(0, renderServiceAgent_, renderProcessManagerAgent_,
//             screenManagerAgent_, token_->AsObject(), nullptr);
//     ASSERT_NE(connectionStub, nullptr);

//     // case 1: no data
//     {
//         MessageParcel data;
//         MessageParcel reply;
//         MessageOption option;
//         data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
//         auto interfaceCode = RSIClientToServiceConnectionInterfaceCode::SET_BRIGHTNESS_INFO_CHANGE_CALLBACK;
//         auto res = connectionStub->OnRemoteRequest(static_cast<uint32_t>(interfaceCode), data, reply, option);
//         EXPECT_EQ(res, ERR_INVALID_DATA);
//     }

//     // case 2: readRemoteObject false
//     {
//         MessageParcel data;
//         MessageParcel reply;
//         MessageOption option;
//         data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
//         data.WriteBool(false);
//         auto interfaceCode = RSIClientToServiceConnectionInterfaceCode::SET_BRIGHTNESS_INFO_CHANGE_CALLBACK;
//         auto res = connectionStub->OnRemoteRequest(static_cast<uint32_t>(interfaceCode), data, reply, option);
//         EXPECT_EQ(res, ERR_NONE);
//     }

//     // case 3: remoteObject null
//     {
//         MessageParcel data;
//         MessageParcel reply;
//         MessageOption option;
//         data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
//         data.WriteBool(true);
//         data.WriteRemoteObject(nullptr);
//         auto interfaceCode = RSIClientToServiceConnectionInterfaceCode::SET_BRIGHTNESS_INFO_CHANGE_CALLBACK;
//         auto res = connectionStub->OnRemoteRequest(static_cast<uint32_t>(interfaceCode), data, reply, option);
//         EXPECT_EQ(res, ERR_NONE);
//     }

//     // case 4: remoteObject not null
//     {
//         MessageParcel data;
//         MessageParcel reply;
//         MessageOption option;
//         data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
//         data.WriteBool(true);
//         MockRSBrightnessInfoChangeCallback callback;
//         data.WriteRemoteObject(callback.AsObject());
//         auto interfaceCode = RSIClientToServiceConnectionInterfaceCode::SET_BRIGHTNESS_INFO_CHANGE_CALLBACK;
//         auto res = connectionStub->OnRemoteRequest(static_cast<uint32_t>(interfaceCode), data, reply, option);
//         EXPECT_EQ(res, ERR_NONE);
//     }
// }

// /**
//  * @tc.name: SetScreenCorrectionTest001
//  * @tc.desc: Test SetScreenCorrection when ReadUint64 failed
//  * @tc.type: FUNC
//  * @tc.require:
//  */
// HWTEST_F(RSClientToServiceConnectionStubTest, SetScreenCorrectionTest001, TestSize.Level1)
// {
//     MessageParcel data;
//     MessageParcel reply;
//     MessageOption option;
//     uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_CORRECTION);
//     data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
//     data.WriteString("str");
//     int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
//     ASSERT_EQ(res, ERR_INVALID_DATA);
// }

/**
 * @tc.name: SetScreenCorrectionTest002
 * @tc.desc: Test SetScreenCorrection when screenRotation is invalid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, SetScreenCorrectionTest002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_CORRECTION);
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data.WriteUint64(0);
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);
}

/**
 * @tc.name: SetScreenCorrectionTest003
 * @tc.desc: Test SetScreenCorrection when screenRotation is invalid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, SetScreenCorrectionTest003, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_CORRECTION);
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data.WriteUint64(0);
    data.WriteUint32(static_cast<uint32_t>(ScreenRotation::INVALID_SCREEN_ROTATION) + 1);
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);
}

/**
 * @tc.name: SetScreenCorrectionTest004
 * @tc.desc: Test SetScreenCorrection success
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, SetScreenCorrectionTest004, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_CORRECTION);
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data.WriteUint64(0);
    data.WriteUint32(static_cast<uint32_t>(ScreenRotation::INVALID_SCREEN_ROTATION));
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_OK);
}

/**
 * @tc.name: SetGpuCrcDirtyEnabledPidListTest001
 * @tc.desc: Test SetGpuCrcDirtyEnabledPidList when data is invalid
 * @tc.type: FUNC
 * @tc.require: issueIICR2M7
 */
HWTEST_F(RSClientToServiceConnectionStubTest, SetGpuCrcDirtyEnabledPidListTest001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code =
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_GPU_CRC_DIRTY_ENABLED_PIDLIST);
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data.WriteInt32(-1);
    int ret = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ret, ERR_INVALID_REPLY);
}

/**
 * @tc.name: SetGpuCrcDirtyEnabledPidListTest002
 * @tc.desc: Test SetGpuCrcDirtyEnabledPidList when data is valid
 * @tc.type: FUNC
 * @tc.require: issueIICR2M7
 */
HWTEST_F(RSClientToServiceConnectionStubTest, SetGpuCrcDirtyEnabledPidListTest002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code =
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_GPU_CRC_DIRTY_ENABLED_PIDLIST);
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    std::vector<int32_t> pidList;
    data.WriteInt32Vector(pidList);
    int ret = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ret, ERR_NONE);
}

/**
 * @tc.name: SetGpuCrcDirtyEnabledPidListTest003
 * @tc.desc: Test SetGpuCrcDirtyEnabledPidList when pidlist is invalid
 * @tc.type: FUNC
 * @tc.require: issueIICR2M7
 */
HWTEST_F(RSClientToServiceConnectionStubTest, SetGpuCrcDirtyEnabledPidListTest003, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code =
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_GPU_CRC_DIRTY_ENABLED_PIDLIST);
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    std::vector<int32_t> pidList(INVALID_PIDLIST_SIZE, 0);
    data.WriteInt32Vector(pidList);
    int ret = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ret, ERR_NONE);
}


/**
 * @tc.name: SetGpuCrcDirtyEnabledPidListTest004
 * @tc.desc: Test SetGpuCrcDirtyEnabledPidList when mainThread_ is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIICR2M7
 */
HWTEST_F(RSClientToServiceConnectionStubTest, SetGpuCrcDirtyEnabledPidListTest004, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code =
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_GPU_CRC_DIRTY_ENABLED_PIDLIST);
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    std::vector<int32_t> pidList;
    data.WriteInt32Vector(pidList);
    sptr<RSClientToServiceConnection> connection = iface_cast<RSClientToServiceConnection>(connectionStub_);
    ASSERT_NE(connection, nullptr);
    int ret = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ret, ERR_NONE);
}

/**
 * @tc.name: SetGpuCrcDirtyEnabledPidListTest005
 * @tc.desc: Test SetGpuCrcDirtyEnabledPidList when pidlist is invalid and mainThread_ is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIICR2M7
 */
HWTEST_F(RSClientToServiceConnectionStubTest, SetGpuCrcDirtyEnabledPidListTest005, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code =
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_GPU_CRC_DIRTY_ENABLED_PIDLIST);
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    std::vector<int32_t> pidList(INVALID_PIDLIST_SIZE, 0);
    data.WriteInt32Vector(pidList);
    sptr<RSClientToServiceConnection> connection = iface_cast<RSClientToServiceConnection>(connectionStub_);
    ASSERT_NE(connection, nullptr);
    int ret = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ret, ERR_NONE);
}

/**
 * @tc.name: SetOptimizeCanvasDirtyPidListTest001
 * @tc.desc: Test SetOptimizeCanvasDirtyPidList when data is invalid
 * @tc.type: FUNC
 * @tc.require: issueICSPON
 */
HWTEST_F(RSClientToServiceConnectionStubTest, SetOptimizeCanvasDirtyPidListTest001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code =
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_OPTIMIZE_CANVAS_DIRTY_ENABLED_PIDLIST);
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data.WriteInt32(-1);
    int ret = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ret, ERR_INVALID_REPLY);
}

/**
 * @tc.name: SetOptimizeCanvasDirtyPidListTest002
 * @tc.desc: Test SetOptimizeCanvasDirtyPidList when data is valid
 * @tc.type: FUNC
 * @tc.require: issueICSPON
 */
HWTEST_F(RSClientToServiceConnectionStubTest, SetOptimizeCanvasDirtyPidListTest002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code =
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_OPTIMIZE_CANVAS_DIRTY_ENABLED_PIDLIST);
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    std::vector<int32_t> pidList = {1};
    data.WriteInt32Vector(pidList);
    int ret = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: SetOptimizeCanvasDirtyPidListTest003
 * @tc.desc: Test SetOptimizeCanvasDirtyPidList when pidlist is invalid
 * @tc.type: FUNC
 * @tc.require: issueICSPON
 */
HWTEST_F(RSClientToServiceConnectionStubTest, SetOptimizeCanvasDirtyPidListTest003, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code =
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_OPTIMIZE_CANVAS_DIRTY_ENABLED_PIDLIST);
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    std::vector<int32_t> pidList(INVALID_PIDLIST_SIZE, 0);
    data.WriteInt32Vector(pidList);
    int ret = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: SetVirtualScreenStatusTest001
 * @tc.desc: Test SetVirtualScreenStatus when ReadUint64 failed
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, SetVirtualScreenStatusTest001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_STATUS);
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data.WriteString("str");
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);
}

/**
 * @tc.name: SetVirtualScreenStatusTest002
 * @tc.desc: Test SetVirtualScreenStatus when mode is invalid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, SetVirtualScreenStatusTest002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_STATUS);
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data.WriteUint64(0);
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);
}

/**
 * @tc.name: SetVirtualScreenStatusTest003
 * @tc.desc: Test SetVirtualScreenStatus when mode is invalid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, SetVirtualScreenStatusTest003, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_STATUS);
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data.WriteUint64(0);
    data.WriteUint32(static_cast<uint32_t>(VirtualScreenStatus::VIRTUAL_SCREEN_INVALID_STATUS) + 1);
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);
}

/**
 * @tc.name: SetVirtualScreenStatusTest004
 * @tc.desc: Test SetVirtualScreenStatus success
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, SetVirtualScreenStatusTest004, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_STATUS);
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data.WriteUint64(0);
    data.WriteUint32(static_cast<uint32_t>(VirtualScreenStatus::VIRTUAL_SCREEN_INVALID_STATUS));
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_OK);
}
} // namespace OHOS::Rosen
