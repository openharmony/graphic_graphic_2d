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

#include "binder_invoker.h"
#include "gtest/gtest.h"
#include "accesstoken_kit.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"
#include "limit_number.h"
#include "mock_hdi_device.h"
#include "sandbox_utils.h"
#include "common/rs_special_layer_manager.h"
#include "ipc_callbacks/rs_frame_rate_linker_expected_fps_update_callback_stub.h"
#include "ipc_callbacks/rs_iframe_rate_linker_expected_fps_update_callback_ipc_interface_code.h"
#include "ipc_callbacks/rs_iframe_rate_linker_expected_fps_update_callback.h"
#include "ipc_callbacks/screen_change_callback_stub.h"
#include "ipc_callbacks/brightness_info_change_callback_stub.h"
#include "ipc_skeleton.h"
#include "ipc_thread_skeleton.h"
#include "pipeline/render_thread/rs_composer_adapter.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "render_server/transaction/rs_client_to_service_connection.h"
#include "pipeline/rs_logical_display_render_node.h"
#include "pipeline/rs_render_node_gc.h"
#include "pipeline/rs_screen_render_node.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "pipeline/render_thread/rs_render_engine.h"
#include "ipc_callbacks/rs_transaction_data_callback_stub.h"
#include "platform/ohos/transaction/zidl/rs_irender_service.h"
#include "screen_manager/screen_types.h"
#include "render_server/transaction/zidl/rs_client_to_service_connection_stub.h"
#include "screen_manager/rs_screen.h"
#include "feature/capture/rs_capture_pixelmap_manager.h"
#include "feature/color_picker/rs_color_picker_thread.h"
#include "render_process/transaction/rs_service_to_render_connection.h"
#include "render_server/transaction/rs_render_to_service_connection.h"
#include "render_service/composer/composer_client/connection/rs_composer_to_render_connection.h"
#include "transaction/rs_connect_to_render_process.h"
#include "gfx/fps_info/rs_surface_fps_manager.h"
#include "ipc_callbacks/surface_capture_callback_stub.h"
#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif
#include "file_ex.h"
using namespace testing;
using namespace testing::ext;
using namespace OHOS::Security::AccessToken;

namespace {
constexpr const int DEFAULT_WIDTH = 2160;
constexpr const int DEFAULT_HEIGHT = 1080;
constexpr const int INVALID_PIDLIST_SIZE = 200;
constexpr const int WAIT_HANDLER_TIME = 1; // 1S
constexpr const int WAIT_HANDLER_TIME_COUNT = 5;
constexpr const int DELAY_TEAR_DOWN_TESE_CASE = 110;
// constexpr const size_t PARCEL_MAX_CAPACITY = 2000 * 1024;

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
    RSSingleRenderProcessManagerMock(RSRenderService& renderService) : RSRenderProcessManager(renderService)
    {
        // step1: Create renderPipeline and following connections
        auto renderServiceAgent = sptr<RSRenderServiceAgent>::MakeSptr(renderService);
        auto renderProcessManagerAgent =
            sptr<RSRenderProcessManagerAgent>::MakeSptr(renderService.renderProcessManager_);
        auto screenManagerAgent = sptr<RSScreenManagerAgent>::MakeSptr(renderService.screenManager_);

        renderToServiceConnection_ = sptr<RSRenderToServiceConnection>::MakeSptr(
            renderServiceAgent, renderProcessManagerAgent, screenManagerAgent);
        renderService.renderPipeline_ = std::make_shared<RSRenderPipeline>();
        
        auto mainThread = RSMainThread::Instance();
        mainThread->RegisterScreenSwitchFinishCallback(renderToServiceConnection_);
        renderService.renderPipeline_->mainThread_ = mainThread;
        renderService.renderPipeline_->mainThread_->handler_ = renderService.handler_;

        renderService_.renderPipeline_->uniRenderThread_ = &(RSUniRenderThread::Instance());
        auto runner = OHOS::AppExecFwk::EventRunner::Create(true);
        renderService_.renderPipeline_->uniRenderThread_->runner_ = runner;
        renderService_.renderPipeline_->uniRenderThread_->handler_ =
            std::make_shared<OHOS::AppExecFwk::EventHandler>(runner);
        renderService_.renderPipeline_->uniRenderThread_->runner_ ->Run();

        auto renderPipelineAgent = sptr<RSRenderPipelineAgent>::MakeSptr(renderService_.renderPipeline_);
        serviceToRenderConnection_ = sptr<RSServiceToRenderConnection>::MakeSptr(renderPipelineAgent);
        composerToRenderConnection_ = sptr<RSComposerToRenderConnection>::MakeSptr();

        renderService_.renderPipeline_->uniRenderThread_->uniRenderEngine_ =
            std::make_shared<OHOS::Rosen::RSRenderEngine>();
        renderService_.renderPipeline_->uniRenderThread_->uniRenderEngine_->renderContext_ =
            OHOS::Rosen::RenderContext::Create();
        // step2:
        connectToRenderConnection_ = sptr<RSConnectToRenderProcess>::MakeSptr(renderPipelineAgent);
    }

    ~RSSingleRenderProcessManagerMock() noexcept override = default;

    sptr<IRemoteObject> OnScreenConnected(ScreenId screenId,
        const std::shared_ptr<HdiOutput>& output, const sptr<RSScreenProperty>& property)
    {
        auto composerConn = renderService_.rsRenderComposerManager_->GetRSComposerConnection(property->GetScreenId());
        renderService_.renderPipeline_->OnScreenConnected(property, composerConn, composerToRenderConnection_, output);
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
        renderService_.renderPipeline_->OnScreenConnected(property, nullptr, nullptr, nullptr);
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

class MockNativeToken {
public:
    explicit MockNativeToken(const std::string& process);
    ~MockNativeToken()
    {
        SetSelfTokenID(selfToken_);
    }
private:
    uint64_t selfToken_;
    AccessTokenID GetNativeTokenIdFromProcess(const std::string& process);
};

MockNativeToken::MockNativeToken(const std::string& process)
{
    selfToken_ = GetSelfTokenID();
    uint32_t targetTokenId = GetNativeTokenIdFromProcess(process);
    SetSelfTokenID(targetTokenId);
}

AccessTokenID MockNativeToken::GetNativeTokenIdFromProcess(const std::string &process)
{
    std::string dumpInfo;
    AtmToolsParamInfo info;
    info.processName = process;
    AccessTokenKit::DumpTokenInfo(info, dumpInfo);
    size_t pos = dumpInfo.find("\"tokenID\": ");
    if (pos == std::string::npos) {
        return 0;
    }
    pos += std::string("\"tokenID\": ").length();
    std::string numStr;
    while (pos < dumpInfo.length() && std::isdigit(dumpInfo[pos])) {
        numStr += dumpInfo[pos];
        ++pos;
    }

    std::istringstream iss(numStr);
    AccessTokenID tokenID;
    iss >> tokenID;
    return tokenID;
}
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
    static inline sptr<RSClientToServiceConnection> connectionStub_;
    static inline sptr<RSRenderServiceAgent> renderServiceAgent_;
    static inline sptr<RSRenderProcessManagerAgent> renderProcessManagerAgent_;
    static inline RSRenderService renderService_;
    static inline sptr<RSScreenManagerAgent> screenManagerAgent_;
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
    if (auto mgr = HgmCore::Instance().GetFrameRateMgr()) {
        auto func = [](bool forceUpdate, ScreenId activeScreenId) {};
        auto rsDistributor = sptr<VSyncDistributor>::MakeSptr(nullptr, "rs");
        auto appDistributor = sptr<VSyncDistributor>::MakeSptr(nullptr, "app");
        auto runner2 = OHOS::AppExecFwk::EventRunner::Create(true);
        auto handler = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner2);
        renderService_.hgmContext_ = std::make_shared<HgmContext>(handler, mgr, func, appDistributor, rsDistributor);
    }
    runner->Run();
    renderService_.renderProcessManager_ = sptr<RSSingleRenderProcessManagerMock>::MakeSptr(renderService_);
    
    renderServiceAgent_ = sptr<RSRenderServiceAgent>::MakeSptr(renderService_);
    renderProcessManagerAgent_ = sptr<RSRenderProcessManagerAgent>::MakeSptr(renderService_.renderProcessManager_);
    screenManagerAgent_ = new RSScreenManagerAgent(screenManager_);

    renderService_.vsyncManager_ = sptr<RSVsyncManager>::MakeSptr();
    renderService_.vsyncManager_->init(screenManager_);
    renderService_.rsRenderComposerManager_ = std::make_shared<RSRenderComposerManager>(renderService_.handler_);
    token_ = new OHOS::IRemoteStub<OHOS::Rosen::RSIConnectionToken>();
    connectionStub_ =
        sptr<RSClientToServiceConnection>::MakeSptr(0, renderServiceAgent_,
            renderProcessManagerAgent_, screenManagerAgent_, token_->AsObject(),
            renderService_.vsyncManager_->GetVsyncManagerAgent());
}

void RSClientToServiceConnectionStubTest::TearDownTestCase()
{
    auto* connection = static_cast<RSClientToServiceConnection*>(connectionStub_.GetRefPtr());
    auto renderServiceAgent = connection->renderServiceAgent_;
    connection->renderServiceAgent_ = nullptr;
    connection->CleanAll(true);
    connection->renderServiceAgent_ = renderServiceAgent;
    auto hgmContext = connection->hgmContext_;
    if (connection->hgmContext_) {
        connection->hgmContext_ = nullptr;
    }
    connection->CleanAll(true);

    std::this_thread::sleep_for(std::chrono::milliseconds(DELAY_TEAR_DOWN_TESE_CASE));
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
    renderService_.renderPipeline_->uniRenderThread_->uniRenderEngine_->renderContext_ = nullptr;
    renderService_.renderPipeline_->uniRenderThread_->uniRenderEngine_ = nullptr;
    renderService_.renderPipeline_->uniRenderThread_ = nullptr;
    renderService_.renderPipeline_ = nullptr;
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

    sptr<RSClientToServiceConnection>::MakeSptr(
        0, nullptr, nullptr, nullptr, nullptr, renderService_.vsyncManager_->GetVsyncManagerAgent());
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
        static_cast<uint32_t>(
            RSIClientToServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_SURFACE)), ERR_INVALID_DATA);
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
        static_cast<uint32_t>(
            RSIClientToServiceConnectionInterfaceCode::SET_REFRESH_RATE_MODE)), ERR_INVALID_DATA);
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
        static_cast<uint32_t>(
            RSIClientToServiceConnectionInterfaceCode::NOTIFY_LIGHT_FACTOR_STATUS)), ERR_INVALID_DATA);
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
    ASSERT_EQ(OnRemoteRequestTest(
         static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_TOTAL_APP_MEM_SIZE)), ERR_NONE);
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
HWTEST_F(RSClientToServiceConnectionStubTest, TestRSRenderServiceConnectionStub018, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_CHANGE_CALLBACK);
    sptr<RSScreenChangeCallbackStubMock> callback = new RSScreenChangeCallbackStubMock();
    data.WriteRemoteObject(callback->AsObject());
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, NO_ERROR);
}

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
    uint32_t code = static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::GET_SCREEN_CURRENT_REFRESH_RATE);
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
    uint32_t code = static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_REFRESH_RATES);
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
 * @tc.name: TestGetRefreshInfoByPidAndUniqueId001
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, TestGetRefreshInfoByPidAndUniqueId001, TestSize.Level1)
{
    {
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;
        uint32_t code =
            static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_REFRESH_INFO_BY_PID_AND_UNIQUEID);
        int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
        ASSERT_EQ(res, ERR_INVALID_STATE);
    }

    {
        EXPECT_EQ(OnRemoteRequestTest(static_cast<uint32_t>(
                      RSIClientToServiceConnectionInterfaceCode::GET_REFRESH_INFO_BY_PID_AND_UNIQUEID)),
            ERR_INVALID_DATA);
    }

    {
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;
        data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
        data.WriteInt32(1000);
        uint32_t code =
            static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_REFRESH_INFO_BY_PID_AND_UNIQUEID);
        int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
        ASSERT_EQ(res, ERR_INVALID_DATA);
    }

    {
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;
        data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
        data.WriteInt32(1000);
        data.WriteUint64(0);
        uint32_t code =
            static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_REFRESH_INFO_BY_PID_AND_UNIQUEID);
        int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
        ASSERT_EQ(res, ERR_OK);
    }
}

/**
 * @tc.name: TestGetRefreshInfoByPidAndUniqueId002
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, TestGetRefreshInfoByPidAndUniqueId002, TestSize.Level1)
{
    pid_t newPid = 1003;
    NodeId nodeId = (static_cast<uint64_t>(newPid) << 32) | 0x00001031;
    sptr<RSClientToServiceConnection> clientToServiceConnection =
        iface_cast<RSClientToServiceConnection>(connectionStub_);
    ASSERT_NE(clientToServiceConnection, nullptr);
    std::string res = "";
    clientToServiceConnection->GetRefreshInfoByPidAndUniqueId(newPid, 0, res);
    ASSERT_EQ(res, "");

    RSSurfaceFpsManager& surfaceFpsManager = RSSurfaceFpsManager::GetInstance();
    std::string name = "surfacefps0";
    uint64_t uniqueId = 1000;
    surfaceFpsManager.RegisterSurfaceFps(nodeId, name, uniqueId);
    clientToServiceConnection->GetRefreshInfoByPidAndUniqueId(newPid, uniqueId, res);
    ASSERT_NE(res, "");

    surfaceFpsManager.UnregisterSurfaceFps(nodeId);
}

/**
 * @tc.name: TestGetRefreshInfoByPidAndUniqueId003
 * @tc.desc: Test Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, TestGetRefreshInfoByPidAndUniqueId003, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data.WriteInt32(1000);
    data.WriteUint64(0);
    uint32_t code =
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_REFRESH_INFO_BY_PID_AND_UNIQUEID);
    reply.writable_ = false;
    reply.data_ = nullptr;
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_REPLY);
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
    uint32_t code = static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::NOTIFY_WINDOW_EXPECTED_BY_WINDOW_ID);
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
    uint32_t code = static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::NOTIFY_WINDOW_EXPECTED_BY_WINDOW_ID);
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
 * @tc.name: OnRemoteRequest_CreateVirtualScreen_Test001
 * @tc.desc: OnRemoteRequest_CreateVirtualScreen_Test001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, OnRemoteRequest_CreateVirtualScreen_Test001, TestSize.Level1)
{
    MockNativeToken mockToken("foundation");
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::CREATE_VIRTUAL_SCREEN);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    ASSERT_EQ(ERR_INVALID_DATA, connectionStub_->OnRemoteRequest(code, data, reply, option));

    auto* connection = static_cast<RSClientToServiceConnection*>(connectionStub_.GetRefPtr());
    ASSERT_NE(connection, nullptr);
    auto screenManagerAgent = connection->screenManagerAgent_;
    ASSERT_NE(screenManagerAgent, nullptr);
    connection->screenManagerAgent_ = nullptr;
    connection->CreateVirtualScreen("test", 0, 0, nullptr, INVALID_SCREEN_ID, 0);
    connection->CleanVirtualScreens();
    connection->screenManagerAgent_ = screenManagerAgent;
    auto screenManager = connection->screenManagerAgent_->screenManager_;
    connection->screenManagerAgent_->screenManager_ = nullptr;
    connection->CreateVirtualScreen("test", 0, 0, nullptr, INVALID_SCREEN_ID, 0);
    connection->screenManagerAgent_->screenManager_ = screenManager;
    connection->CleanVirtualScreens();
}

/**
 * @tc.name: OnRemoteRequest_CreateVirtualScreen_Test002
 * @tc.desc: OnRemoteRequest_CreateVirtualScreen_Test002
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, OnRemoteRequest_CreateVirtualScreen_Test002, TestSize.Level1)
{
    MockNativeToken mockToken("foundation");
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::CREATE_VIRTUAL_SCREEN);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data.WriteString("name");
    ASSERT_EQ(ERR_INVALID_DATA, connectionStub_->OnRemoteRequest(code, data, reply, option));
}

/**
 * @tc.name: OnRemoteRequest_CreateVirtualScreen_Test003
 * @tc.desc: OnRemoteRequest_CreateVirtualScreen_Test003
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, OnRemoteRequest_CreateVirtualScreen_Test003, TestSize.Level1)
{
    MockNativeToken mockToken("foundation");
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::CREATE_VIRTUAL_SCREEN);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data.WriteString("name");
    data.WriteUint32(1920);
    ASSERT_EQ(ERR_INVALID_DATA, connectionStub_->OnRemoteRequest(code, data, reply, option));
}

/**
 * @tc.name: OnRemoteRequest_CreateVirtualScreen_Test004
 * @tc.desc: OnRemoteRequest_CreateVirtualScreen_Test004
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, OnRemoteRequest_CreateVirtualScreen_Test004, TestSize.Level1)
{
    MockNativeToken mockToken("foundation");
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::CREATE_VIRTUAL_SCREEN);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data.WriteString("name");
    data.WriteUint32(1920);
    data.WriteUint32(1080);
    ASSERT_EQ(ERR_INVALID_DATA, connectionStub_->OnRemoteRequest(code, data, reply, option));
}

/**
 * @tc.name: OnRemoteRequest_CreateVirtualScreen_Test005
 * @tc.desc: OnRemoteRequest_CreateVirtualScreen_Test005
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, OnRemoteRequest_CreateVirtualScreen_Test005, TestSize.Level1)
{
    MockNativeToken mockToken("foundation");
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::CREATE_VIRTUAL_SCREEN);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data.WriteString("name");
    data.WriteUint32(1920);
    data.WriteUint32(1080);
    data.WriteBool(false);
    ASSERT_EQ(ERR_INVALID_DATA, connectionStub_->OnRemoteRequest(code, data, reply, option));
}

/**
 * @tc.name: OnRemoteRequest_CreateVirtualScreen_Test006
 * @tc.desc: OnRemoteRequest_CreateVirtualScreen_Test006
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, OnRemoteRequest_CreateVirtualScreen_Test006, TestSize.Level1)
{
    MockNativeToken mockToken("foundation");
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::CREATE_VIRTUAL_SCREEN);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data.WriteString("name");
    data.WriteUint32(1920);
    data.WriteUint32(1080);
    data.WriteBool(false);
    data.WriteUint64(64);
    ASSERT_EQ(ERR_INVALID_DATA, connectionStub_->OnRemoteRequest(code, data, reply, option));
}

/**
 * @tc.name: OnRemoteRequest_CreateVirtualScreen_Test007
 * @tc.desc: OnRemoteRequest_CreateVirtualScreen_Test007
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, OnRemoteRequest_CreateVirtualScreen_Test007, TestSize.Level1)
{
    MockNativeToken mockToken("foundation");
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::CREATE_VIRTUAL_SCREEN);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data.WriteString("name");
    data.WriteUint32(1920);
    data.WriteUint32(1080);
    data.WriteBool(false);
    data.WriteUint64(64);
    data.WriteUint32(32);
    data.WriteUInt32Vector({static_cast<uint32_t>(1000)});
    ASSERT_EQ(ERR_INVALID_DATA, connectionStub_->OnRemoteRequest(code, data, reply, option));
}

/**
 * @tc.name: OnRemoteRequest_CreateVirtualScreen_Test008
 * @tc.desc: OnRemoteRequest_CreateVirtualScreen_Test008
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, OnRemoteRequest_CreateVirtualScreen_Test008, TestSize.Level1)
{
    MockNativeToken mockToken("foundation");
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::CREATE_VIRTUAL_SCREEN);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data.WriteString("name");
    data.WriteUint32(1920);
    data.WriteUint32(1080);
    data.WriteBool(false);
    data.WriteUint64(64);
    data.WriteUint32(32);
    data.WriteUInt64Vector({64});
    ASSERT_EQ(ERR_NONE, connectionStub_->OnRemoteRequest(code, data, reply, option));
}

/**
 * @tc.name: OnRemoteRequest_CreateVirtualScreen_Test009
 * @tc.desc: OnRemoteRequest_CreateVirtualScreen_Test009
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, OnRemoteRequest_CreateVirtualScreen_Test009, TestSize.Level1)
{
    MockNativeToken mockToken("foundation");
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::CREATE_VIRTUAL_SCREEN);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data.WriteString("name");
    data.WriteUint32(1920);
    data.WriteUint32(1080);
    data.WriteBool(true);
    ASSERT_EQ(ERR_INVALID_DATA, connectionStub_->OnRemoteRequest(code, data, reply, option));
}

/**
 * @tc.name: OnRemoteRequest_CreateVirtualScreen_Test010
 * @tc.desc: OnRemoteRequest_CreateVirtualScreen_Test010
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, OnRemoteRequest_CreateVirtualScreen_Test010, TestSize.Level1)
{
    MockNativeToken mockToken("foundation");
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::CREATE_VIRTUAL_SCREEN);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data.WriteString("name");
    data.WriteUint32(1920);
    data.WriteUint32(1080);
    data.WriteBool(true);
    sptr<RSScreenChangeCallbackStubMock> callback = new RSScreenChangeCallbackStubMock();
    data.WriteRemoteObject(callback->AsObject());
    ASSERT_EQ(ERR_INVALID_DATA, connectionStub_->OnRemoteRequest(code, data, reply, option));
}

/**
 * @tc.name: OnRemoteRequest_SetVirtualScreenResolution_Test001
 * @tc.desc: OnRemoteRequest_SetVirtualScreenResolution_Test001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, OnRemoteRequest_SetVirtualScreenResolution_Test001, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_RESOLUTION);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    ASSERT_EQ(ERR_INVALID_DATA, connectionStub_->OnRemoteRequest(code, data, reply, option));

    auto* connection = static_cast<RSClientToServiceConnection*>(connectionStub_.GetRefPtr());
    auto screenManagerAgent = connection->screenManagerAgent_;
    connection->screenManagerAgent_ = nullptr;
    connection->SetVirtualScreenResolution(INVALID_SCREEN_ID, 0, 0);
    connection->screenManagerAgent_ = screenManagerAgent;
}

/**
 * @tc.name: OnRemoteRequest_SetVirtualScreenResolution_Test002
 * @tc.desc: OnRemoteRequest_SetVirtualScreenResolution_Test002
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, OnRemoteRequest_SetVirtualScreenResolution_Test002, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_RESOLUTION);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data.WriteUint64(64);
    ASSERT_EQ(ERR_INVALID_DATA, connectionStub_->OnRemoteRequest(code, data, reply, option));
}

/**
 * @tc.name: OnRemoteRequest_SetVirtualScreenResolution_Test003
 * @tc.desc: OnRemoteRequest_SetVirtualScreenResolution_Test003
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, OnRemoteRequest_SetVirtualScreenResolution_Test003, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_RESOLUTION);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data.WriteUint64(64);
    data.WriteUint32(32);
    ASSERT_EQ(ERR_INVALID_DATA, connectionStub_->OnRemoteRequest(code, data, reply, option));
}

/**
 * @tc.name: OnRemoteRequest_SetVirtualScreenResolution_Test004
 * @tc.desc: OnRemoteRequest_SetVirtualScreenResolution_Test004
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, OnRemoteRequest_SetVirtualScreenResolution_Test004, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_RESOLUTION);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data.WriteUint64(64);
    data.WriteUint32(1920);
    data.WriteUint32(1080);
    ASSERT_EQ(ERR_NONE, connectionStub_->OnRemoteRequest(code, data, reply, option));
}

/**
 * @tc.name: OnRemoteRequest_GetScreenBacklight_Test001
 * @tc.desc: OnRemoteRequest_GetScreenBacklight_Test001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, OnRemoteRequest_GetScreenBacklight_Test001, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_SCREEN_BACK_LIGHT);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    ASSERT_EQ(ERR_INVALID_DATA, connectionStub_->OnRemoteRequest(code, data, reply, option));
}

/**
 * @tc.name: OnRemoteRequest_GetScreenBacklight_Test002
 * @tc.desc: OnRemoteRequest_GetScreenBacklight_Test002
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, OnRemoteRequest_GetScreenBacklight_Test002, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_SCREEN_BACK_LIGHT);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data.WriteUint64(64);
    ASSERT_EQ(ERR_NONE, connectionStub_->OnRemoteRequest(code, data, reply, option));
}

/**
 * @tc.name: OnRemoteRequest_GetScreenSupportedMetaDataKeys_Test001
 * @tc.desc: OnRemoteRequest_GetScreenSupportedMetaDataKeys_Test001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, OnRemoteRequest_GetScreenSupportedMetaDataKeys_Test001, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_METADATAKEYS);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    ASSERT_EQ(ERR_INVALID_DATA, connectionStub_->OnRemoteRequest(code, data, reply, option));
}

/**
 * @tc.name: OnRemoteRequest_GetScreenSupportedMetaDataKeys_Test002
 * @tc.desc: OnRemoteRequest_GetScreenSupportedMetaDataKeys_Test002
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, OnRemoteRequest_GetScreenSupportedMetaDataKeys_Test002, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_METADATAKEYS);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data.WriteUint64(64);
    ASSERT_EQ(ERR_UNKNOWN_REASON, connectionStub_->OnRemoteRequest(code, data, reply, option));
}

/**
 * @tc.name: OnRemoteRequest_ResizeVirtualScreen_Test001
 * @tc.desc: OnRemoteRequest_ResizeVirtualScreen_Test001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, OnRemoteRequest_ResizeVirtualScreen_Test001, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::RESIZE_VIRTUAL_SCREEN);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    ASSERT_EQ(ERR_INVALID_DATA, connectionStub_->OnRemoteRequest(code, data, reply, option));
}

/**
 * @tc.name: OnRemoteRequest_ResizeVirtualScreen_Test002
 * @tc.desc: OnRemoteRequest_ResizeVirtualScreen_Test002
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, OnRemoteRequest_ResizeVirtualScreen_Test002, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::RESIZE_VIRTUAL_SCREEN);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data.WriteUint64(64);
    ASSERT_EQ(ERR_INVALID_DATA, connectionStub_->OnRemoteRequest(code, data, reply, option));
}

/**
 * @tc.name: OnRemoteRequest_ResizeVirtualScreen_Test003
 * @tc.desc: OnRemoteRequest_ResizeVirtualScreen_Test003
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, OnRemoteRequest_ResizeVirtualScreen_Test003, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::RESIZE_VIRTUAL_SCREEN);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data.WriteUint64(64);
    data.WriteUint32(32);
    ASSERT_EQ(ERR_INVALID_DATA, connectionStub_->OnRemoteRequest(code, data, reply, option));
}

/**
 * @tc.name: OnRemoteRequest_ResizeVirtualScreen_Test004
 * @tc.desc: OnRemoteRequest_ResizeVirtualScreen_Test004
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, OnRemoteRequest_ResizeVirtualScreen_Test004, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::RESIZE_VIRTUAL_SCREEN);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data.WriteUint64(64);
    data.WriteUint32(1920);
    data.WriteUint32(1080);
    ASSERT_EQ(ERR_NONE, connectionStub_->OnRemoteRequest(code, data, reply, option));
}

/**
 * @tc.name: GetPanelPowerStatus001
 * @tc.desc: Test GetPanelPowerStatus
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, GetPanelPowerStatus001, TestSize.Level1)
{
    constexpr uint64_t SCREEN_ID = 0;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_PANEL_POWER_STATUS);
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data.WriteUint64(SCREEN_ID);
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, ERR_OK);
}

/**
 * @tc.name: GetPanelPowerStatus002
 * @tc.desc: Test GetPanelPowerStatus with empty data condition
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, GetPanelPowerStatus002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_PANEL_POWER_STATUS);
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);
}

/**
 * @tc.name: GetPanelPowerStatus003
 * @tc.desc: Test GetPanelPowerStatus with fail reply
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, GetPanelPowerStatus003, TestSize.Level1)
{
    constexpr uint64_t SCREEN_ID = 0;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_PANEL_POWER_STATUS);
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data.WriteUint64(SCREEN_ID);
    reply.writable_ = false;
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, ERR_INVALID_REPLY);
}

/**
 * @tc.name: GetPanelPowerStatus004
 * @tc.desc: Test inner API (clientToServiceConnection::GetPanelPowerStatus)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, GetPanelPowerStatus004, TestSize.Level1)
{
    constexpr uint64_t SCREEN_ID = 0;

    sptr<RSClientToServiceConnection> clientToServiceConnection =
        iface_cast<RSClientToServiceConnection>(connectionStub_);
    ASSERT_NE(clientToServiceConnection, nullptr);

    // empty screen manager
    PanelPowerStatus status = PanelPowerStatus::INVALID_PANEL_POWER_STATUS;
    auto screenManagerAgent = clientToServiceConnection->screenManagerAgent_;
    clientToServiceConnection->screenManagerAgent_ = nullptr;
    int ret = clientToServiceConnection->GetPanelPowerStatus(SCREEN_ID, status);
    EXPECT_EQ(ret, ERR_INVALID_OPERATION);

    // restore screenManagerAgent
    clientToServiceConnection->screenManagerAgent_ = screenManagerAgent;
    ASSERT_NE(clientToServiceConnection->screenManagerAgent_, nullptr);
}

/**
 * @tc.name: OnRemoteRequest_SetScreenSkipFrameInterval_Test001
 * @tc.desc: OnRemoteRequest_SetScreenSkipFrameInterval_Test001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, OnRemoteRequest_SetScreenSkipFrameInterval_Test001, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_SKIP_FRAME_INTERVAL);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    ASSERT_EQ(ERR_INVALID_DATA, connectionStub_->OnRemoteRequest(code, data, reply, option));
}

/**
 * @tc.name: OnRemoteRequest_SetScreenSkipFrameInterval_Test002
 * @tc.desc: OnRemoteRequest_SetScreenSkipFrameInterval_Test002
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, OnRemoteRequest_SetScreenSkipFrameInterval_Test002, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_SKIP_FRAME_INTERVAL);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data.WriteUint64(64);
    ASSERT_EQ(ERR_INVALID_DATA, connectionStub_->OnRemoteRequest(code, data, reply, option));
}

/**
 * @tc.name: OnRemoteRequest_SetScreenSkipFrameInterval_Test003
 * @tc.desc: OnRemoteRequest_SetScreenSkipFrameInterval_Test003
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, OnRemoteRequest_SetScreenSkipFrameInterval_Test003, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_SKIP_FRAME_INTERVAL);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data.WriteUint64(64);
    data.WriteUint32(32);
    ASSERT_EQ(ERR_NONE, connectionStub_->OnRemoteRequest(code, data, reply, option));
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
 * @tc.name: AvcodecVideoGetTest001
 * @tc.desc: Test AvcodecVideoGet
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, AvcodecVideoGetTest001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::AVCODEC_VIDEO_GET);
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    uint64_t uniqueId = 1;
    data.WriteUint64(uniqueId);
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_NONE);
}
 
/**
 * @tc.name: AvcodecVideoGetRecentTest001
 * @tc.desc: Test AvcodecVideoGetRecent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, AvcodecVideoGetRecentTest001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::AVCODEC_VIDEO_GET_RECENT);
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
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
    uint32_t code = static_cast<uint32_t>(
            RSIClientToServiceConnectionInterfaceCode::REGISTER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK);
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

    auto* connection = static_cast<RSClientToServiceConnection*>(connectionStub_.GetRefPtr());
    auto screenManagerAgent = connection->screenManagerAgent_;
    ASSERT_NE(screenManagerAgent, nullptr);
    auto renderServiceAgent = connection->renderServiceAgent_;
    ASSERT_NE(renderServiceAgent, nullptr);
    connection->screenManagerAgent_ = nullptr;
    connection->SetScreenPowerStatus(INVALID_SCREEN_ID, ScreenPowerStatus::INVALID_POWER_STATUS);
    connection->screenManagerAgent_ = screenManagerAgent;
    connection->renderServiceAgent_ = nullptr;
    connection->SetScreenPowerStatus(INVALID_SCREEN_ID, ScreenPowerStatus::INVALID_POWER_STATUS);
    connection->renderServiceAgent_ = renderServiceAgent;
}

/**
 * @tc.name: SetRogScreenResolutionTest001
 * @tc.desc: Test SetRogScreenResolution
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, SetRogScreenResolutionTest001, TestSize.Level1)
{
    constexpr uint32_t FOUNDATION_UID = 5523;

    // To run this unittest with foundation permissions
    // create a invoker with specified foundation Uid_
    BinderInvoker *invoker = new BinderInvoker();
    invoker->status_ = IRemoteInvoker::ACTIVE_INVOKER;
    invoker->callerUid_ = FOUNDATION_UID;
    IPCThreadSkeleton *current = IPCThreadSkeleton::GetCurrent();
    current->invokers_[IRemoteObject::IF_PROT_DEFAULT] = invoker;
    ASSERT_EQ(OHOS::IPCSkeleton::GetCallingUid(), FOUNDATION_UID);

    constexpr uint64_t SCREEN_ID = 0;
    uint32_t width{1920};
    uint32_t height{1080};
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_ROG_SCREEN_RESOLUTION);
    
    // case 1: entire pipeline
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data.WriteUint64(SCREEN_ID);
    data.WriteUint32(width);
    data.WriteUint32(height);
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_OK);

    // case 2: Read parcel failed
    MessageParcel data2;
    MessageParcel reply2;
    data2.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data2.WriteUint64(SCREEN_ID);
    res = connectionStub_->OnRemoteRequest(code, data2, reply2, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);

    // case 3: Write parcel failed
    MessageParcel data3;
    MessageParcel reply3;
    data3.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data3.WriteUint64(SCREEN_ID);
    data3.WriteUint32(width);
    data3.WriteUint32(height);
    reply3.writable_ = false;
    res = connectionStub_->OnRemoteRequest(code, data3, reply3, option);
    ASSERT_EQ(res, ERR_INVALID_REPLY);

    // remove the invoker
    current->invokers_[IRemoteObject::IF_PROT_DEFAULT] = nullptr;
    delete invoker;
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

/**
 * @tc.name: SetBrightnessInfoChangeCallbackTest
 * @tc.desc: Test SetBrightnessInfoChangeCallback
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSClientToServiceConnectionStubTest, SetBrightnessInfoChangeCallbackTest, TestSize.Level2)
{
    // case 1: no data
    {
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;
        data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
        auto code = RSIClientToServiceConnectionInterfaceCode::SET_BRIGHTNESS_INFO_CHANGE_CALLBACK;
        auto res = connectionStub_->OnRemoteRequest(static_cast<uint32_t>(code), data, reply, option);
        EXPECT_EQ(res, ERR_INVALID_DATA);
    }

    // case 2: readRemoteObject false
    {
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;
        data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
        data.WriteBool(false);
        auto code = RSIClientToServiceConnectionInterfaceCode::SET_BRIGHTNESS_INFO_CHANGE_CALLBACK;
        auto res = connectionStub_->OnRemoteRequest(static_cast<uint32_t>(code), data, reply, option);
        EXPECT_EQ(res, ERR_NONE);
    }

    // case 3: remoteObject null
    {
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;
        data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
        data.WriteBool(true);
        data.WriteRemoteObject(nullptr);
        auto code = RSIClientToServiceConnectionInterfaceCode::SET_BRIGHTNESS_INFO_CHANGE_CALLBACK;
        auto res = connectionStub_->OnRemoteRequest(static_cast<uint32_t>(code), data, reply, option);
        EXPECT_EQ(res, ERR_NONE);
    }

    // case 4: remoteObject not null
    {
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;
        data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
        data.WriteBool(true);
        MockRSBrightnessInfoChangeCallback callback;
        data.WriteRemoteObject(callback.AsObject());
        auto code = RSIClientToServiceConnectionInterfaceCode::SET_BRIGHTNESS_INFO_CHANGE_CALLBACK;
        auto res = connectionStub_->OnRemoteRequest(static_cast<uint32_t>(code), data, reply, option);
        EXPECT_EQ(res, ERR_NONE);
    }
}

/**
 * @tc.name: GetPixelFormatTest001
 * @tc.desc: Test GetPixelFormat
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSClientToServiceConnectionStubTest, GetPixelFormatTest001, TestSize.Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    auto code = RSIClientToServiceConnectionInterfaceCode::GET_PIXEL_FORMAT;
    auto res = connectionStub_->OnRemoteRequest(static_cast<uint32_t>(code), data, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);
}

/**
 * @tc.name: GetPixelFormatTest003
 * @tc.desc: Test GetPixelFormat
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSClientToServiceConnectionStubTest, GetPixelFormatTest003, TestSize.Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data.WriteUint64(screenId_);
    auto code = RSIClientToServiceConnectionInterfaceCode::GET_PIXEL_FORMAT;
    auto res = connectionStub_->OnRemoteRequest(static_cast<uint32_t>(code), data, reply, option);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: SetPixelFormatTest001
 * @tc.desc: Test SetPixelFormat
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSClientToServiceConnectionStubTest, SetPixelFormatTest001, TestSize.Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    auto code = RSIClientToServiceConnectionInterfaceCode::SET_PIXEL_FORMAT;
    auto res = connectionStub_->OnRemoteRequest(static_cast<uint32_t>(code), data, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);
}

/**
 * @tc.name: SetPixelFormatTest003
 * @tc.desc: Test SetPixelFormat
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSClientToServiceConnectionStubTest, SetPixelFormatTest003, TestSize.Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data.WriteUint64(screenId_);
    data.WriteBool(false);
    auto code = RSIClientToServiceConnectionInterfaceCode::SET_PIXEL_FORMAT;
    auto res = connectionStub_->OnRemoteRequest(static_cast<uint32_t>(code), data, reply, option);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: SetPixelFormatTest004
 * @tc.desc: Test SetPixelFormat
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSClientToServiceConnectionStubTest, SetPixelFormatTest004, TestSize.Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data.WriteUint64(screenId_);
    data.WriteInt32(static_cast<int32_t>(GraphicPixelFormat::GRAPHIC_PIXEL_FMT_RGBA_8888));
    auto code = RSIClientToServiceConnectionInterfaceCode::SET_PIXEL_FORMAT;
    auto res = connectionStub_->OnRemoteRequest(static_cast<uint32_t>(code), data, reply, option);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: GetScreenHDRFormat001
 * @tc.desc: Test GetScreenHDRFormat
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSClientToServiceConnectionStubTest, GetScreenHDRFormat001, TestSize.Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    auto code = RSIClientToServiceConnectionInterfaceCode::GET_SCREEN_HDR_FORMAT;
    auto res = connectionStub_->OnRemoteRequest(static_cast<uint32_t>(code), data, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);
}

/**
 * @tc.name: GetScreenHDRFormat002
 * @tc.desc: Test GetScreenHDRFormat
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSClientToServiceConnectionStubTest, GetScreenHDRFormat002, TestSize.Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data.WriteUint64(screenId_);
    auto code = RSIClientToServiceConnectionInterfaceCode::GET_SCREEN_HDR_FORMAT;
    auto res = connectionStub_->OnRemoteRequest(static_cast<uint32_t>(code), data, reply, option);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: SetScreenCorrectionTest001
 * @tc.desc: Test SetScreenCorrection when ReadUint64 failed
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, SetScreenCorrectionTest001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_CORRECTION);
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data.WriteString("str");
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);
}

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

/**
 * @tc.name: NotifyHgmConfigEventTest
 * @tc.desc: Test NotifyHgmConfigEvent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, NotifyHgmConfigEventTest, TestSize.Level1)
{
    ASSERT_NE(connectionStub_, nullptr);
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::NOTIFY_HGMCONFIG_EVENT);
    std::string eventName = "test";
    MessageParcel reply;
    MessageOption option;

    MessageParcel data;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    auto res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    MessageParcel data2;
    data2.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data2.WriteString(eventName);
    res = connectionStub_->OnRemoteRequest(code, data2, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    MessageParcel data3;
    data3.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data3.WriteString(eventName);
    data3.WriteBool(true);
    res = connectionStub_->OnRemoteRequest(code, data3, reply, option);
    EXPECT_EQ(res, ERR_OK);

    auto orgHgmContext = renderService_.hgmContext_;
    renderService_.hgmContext_ = nullptr;
    MessageParcel data4;
    data4.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data4.WriteString(eventName);
    data4.WriteBool(true);
    res = connectionStub_->OnRemoteRequest(code, data4, reply, option);
    EXPECT_EQ(res, ERR_OK);
    renderService_.hgmContext_ = orgHgmContext;
}

/**
 * @tc.name: ShowWatermarkTest
 * @tc.desc: Test ShowWatermark
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, ShowWatermarkTest, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SHOW_WATERMARK);
    bool isShow = true;
    int width = 10;
    int height = 10;
    Media::InitializationOptions opts;
    opts.size.width = width;
    opts.size.height = height;
    std::shared_ptr<Media::PixelMap> pixelmap = Media::PixelMap::Create(opts);
    EXPECT_NE(pixelmap, nullptr);
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data.WriteParcelable(pixelmap.get());
    data.WriteBool(isShow);
    connectionStub_->OnRemoteRequest(code, data, reply, option);
}

/**
 * @tc.name: SetDualScreenStateTest001
 * @tc.desc: Test SetDualScreenState
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, SetDualScreenStateTest001, TestSize.Level1)
{
    uint32_t interfaceCode = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_DUAL_SCREEN_STATE);

    // case 1: only write descriptor
    {
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;
        data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
        auto res = connectionStub_->OnRemoteRequest(static_cast<uint32_t>(interfaceCode), data, reply, option);
        EXPECT_EQ(res, ERR_INVALID_DATA);
    }

    // case 2: only write descriptor and screen id
    {
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;
        data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
        data.WriteUint64(hdiOutput_->GetScreenId());
        auto res = connectionStub_->OnRemoteRequest(static_cast<uint32_t>(interfaceCode), data, reply, option);
        EXPECT_EQ(res, ERR_INVALID_DATA);
    }

    // case 3: write invalid dual screen status
    {
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;
        data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
        data.WriteUint64(hdiOutput_->GetScreenId());
        data.WriteUint64(static_cast<uint64_t>(DualScreenStatus::DUAL_SCREEN_STATUS_BUTT));
        auto res = connectionStub_->OnRemoteRequest(static_cast<uint32_t>(interfaceCode), data, reply, option);
        EXPECT_EQ(res, ERR_INVALID_DATA);
    }

    // case 4: write valid data, but error occurs in reply
    {
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;
        data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
        data.WriteUint64(hdiOutput_->GetScreenId());
        data.WriteUint64(static_cast<uint64_t>(DualScreenStatus::DUAL_SCREEN_ENTER));
        reply.writable_ = false;
        auto res = connectionStub_->OnRemoteRequest(static_cast<uint32_t>(interfaceCode), data, reply, option);
        EXPECT_EQ(res, ERR_INVALID_REPLY);
    }

    // case 5: write valid data
    {
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;
        data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
        data.WriteUint64(hdiOutput_->GetScreenId());
        data.WriteUint64(static_cast<uint64_t>(DualScreenStatus::DUAL_SCREEN_ENTER));
        auto res = connectionStub_->OnRemoteRequest(static_cast<uint32_t>(interfaceCode), data, reply, option);
        EXPECT_EQ(res, ERR_OK);
    }

    // case 6: empty screen manager
    {
        ScreenId id = hdiOutput_->GetScreenId();
        sptr<RSClientToServiceConnection> connection = iface_cast<RSClientToServiceConnection>(connectionStub_);
        ASSERT_NE(connection, nullptr);
        auto screenManagerAgent = connection->screenManagerAgent_;
        connection->screenManagerAgent_ = nullptr;
        int32_t res = connection->SetDualScreenState(id, DualScreenStatus::DUAL_SCREEN_ENTER);
        connection->screenManagerAgent_ = screenManagerAgent;
        EXPECT_EQ(res, StatusCode::SCREEN_NOT_FOUND);
    }
}

/**
 * @tc.name: AddVirtualScreenWhiteListTest001
 * @tc.desc: Test AddVirtualScreenWhiteList
 * @tc.type: FUNC
 * @tc.require: issue21114
 */
HWTEST_F(RSClientToServiceConnectionStubTest, AddVirtualScreenWhiteListTest001, TestSize.Level2)
{
    ASSERT_NE(connectionStub_, nullptr);
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::ADD_VIRTUAL_SCREEN_WHITELIST);
    ScreenId id = INVALID_SCREEN_ID;
    std::vector<NodeId> whiteList;

    MessageParcel data1;
    data1.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    auto res = connectionStub_->OnRemoteRequest(code, data1, reply, option);
    EXPECT_NE(res, ERR_NONE);

    MessageParcel data2;
    data2.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data2.WriteUint64(id);
    data2.WriteUInt64Vector(whiteList);
    res = connectionStub_->OnRemoteRequest(code, data2, reply, option);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: AddVirtualScreenWhiteListTest002
 * @tc.desc: Test AddVirtualScreenWhiteList
 * @tc.type: FUNC
 * @tc.require: issue21114
 */
HWTEST_F(RSClientToServiceConnectionStubTest, AddVirtualScreenWhiteListTest002, TestSize.Level2)
{
    ASSERT_NE(connectionStub_, nullptr);
    sptr<RSClientToServiceConnection> connection = iface_cast<RSClientToServiceConnection>(connectionStub_);
    ASSERT_NE(connection, nullptr);
    ASSERT_NE(connection->screenManagerAgent_, nullptr);

    std::vector<NodeId> whiteList;
    int32_t repCode = 0;
    auto res = connection->AddVirtualScreenWhiteList(INVALID_SCREEN_ID, whiteList, repCode);
    ASSERT_EQ(res, ERR_OK);

    auto screenManagerAgent = connection->screenManagerAgent_;
    connection->screenManagerAgent_ = nullptr;
    res = connection->AddVirtualScreenWhiteList(INVALID_SCREEN_ID, whiteList, repCode);
    EXPECT_EQ(res, ERR_INVALID_VALUE);

    // restore screenManagerAgent
    connection->screenManagerAgent_ = screenManagerAgent;
    ASSERT_NE(connection->screenManagerAgent_, nullptr);
}

/**
 * @tc.name: SetScreenActiveModeTest001
 * @tc.desc: Test SetScreenActiveMode
 * @tc.type: FUNC
 * @tc.require: issue21114
 */
HWTEST_F(RSClientToServiceConnectionStubTest, SetScreenActiveModeTest001, TestSize.Level2)
{
    ASSERT_NE(connectionStub_, nullptr);
    sptr<RSClientToServiceConnection> connection = iface_cast<RSClientToServiceConnection>(connectionStub_);
    ASSERT_NE(connection, nullptr);
    ASSERT_NE(connection->screenManagerAgent_, nullptr);

    ScreenId screenId = INVALID_SCREEN_ID;
    uint32_t modeId = 0;
    auto res = connection->SetScreenActiveMode(screenId, modeId);
    ASSERT_EQ(res, StatusCode::SCREEN_NOT_FOUND);

    auto screenManagerAgent = connection->screenManagerAgent_;
    connection->screenManagerAgent_ = nullptr;
    res = connection->SetScreenActiveMode(screenId, modeId);
    EXPECT_EQ(res, StatusCode::SCREEN_NOT_FOUND);

    // restore screenManagerAgent
    connection->screenManagerAgent_ = screenManagerAgent;
    ASSERT_NE(connection->screenManagerAgent_, nullptr);
}

/**
 * @tc.name: SetScreenChangeCallbackTest001
 * @tc.desc: Test SetScreenChangeCallback
 * @tc.type: FUNC
 * @tc.require: issue21114
 */
HWTEST_F(RSClientToServiceConnectionStubTest, SetScreenChangeCallbackTest001, TestSize.Level2)
{
    ASSERT_NE(connectionStub_, nullptr);
    sptr<RSClientToServiceConnection> connection = iface_cast<RSClientToServiceConnection>(connectionStub_);
    ASSERT_NE(connection, nullptr);
    ASSERT_NE(connection->screenManagerAgent_, nullptr);

    auto res = connection->SetScreenChangeCallback(nullptr);
    ASSERT_EQ(res, 0);

    auto screenManagerAgent = connection->screenManagerAgent_;
    connection->screenManagerAgent_ = nullptr;
    res = connection->SetScreenChangeCallback(nullptr);
    EXPECT_EQ(res, StatusCode::SCREEN_NOT_FOUND);

    // restore screenManagerAgent
    connection->screenManagerAgent_ = screenManagerAgent;
    ASSERT_NE(connection->screenManagerAgent_, nullptr);
}

/**
 * @tc.name: SetPhysicalScreenResolutionTest001
 * @tc.desc: Test SetPhysicalScreenResolution
 * @tc.type: FUNC
 * @tc.require: issue21114
 */
HWTEST_F(RSClientToServiceConnectionStubTest, SetPhysicalScreenResolutionTest001, TestSize.Level2)
{
    ASSERT_NE(connectionStub_, nullptr);
    sptr<RSClientToServiceConnection> connection = iface_cast<RSClientToServiceConnection>(connectionStub_);
    ASSERT_NE(connection, nullptr);
    ASSERT_NE(connection->screenManagerAgent_, nullptr);

    ScreenId screenId = INVALID_SCREEN_ID;
    uint32_t width = 0;
    uint32_t height = 0;
    auto res = connection->SetPhysicalScreenResolution(screenId, width, height);
    ASSERT_EQ(res, SCREEN_NOT_FOUND);

    auto screenManagerAgent = connection->screenManagerAgent_;
    connection->screenManagerAgent_ = nullptr;
    res = connection->SetPhysicalScreenResolution(screenId, width, height);
    EXPECT_EQ(res, SCREEN_NOT_FOUND);

    // restore screenManagerAgent
    connection->screenManagerAgent_ = screenManagerAgent;
    ASSERT_NE(connection->screenManagerAgent_, nullptr);
}

/**
 * @tc.name: RemoveVirtualScreenWhiteList001
 * @tc.desc: Test RemoveVirtualScreenWhiteList
 * @tc.type: FUNC
 * @tc.require: issue21114
 */
HWTEST_F(RSClientToServiceConnectionStubTest, RemoveVirtualScreenWhiteList001, TestSize.Level2)
{
    ASSERT_NE(connectionStub_, nullptr);
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::REMOVE_VIRTUAL_SCREEN_WHITELIST);
    ScreenId id = INVALID_SCREEN_ID;
    std::vector<NodeId> whiteList;

    MessageParcel data1;
    data1.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    auto res = connectionStub_->OnRemoteRequest(code, data1, reply, option);
    EXPECT_NE(res, ERR_NONE);

    MessageParcel data2;
    data2.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data2.WriteUint64(id);
    data2.WriteUInt64Vector(whiteList);
    res = connectionStub_->OnRemoteRequest(code, data2, reply, option);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: SetVirtualScreenBlackListTest001
 * @tc.desc: Test SetVirtualScreenBlackList
 * @tc.type: FUNC
 * @tc.require: issue21114
 */
HWTEST_F(RSClientToServiceConnectionStubTest, SetVirtualScreenBlackListTest001, TestSize.Level2)
{
    ASSERT_NE(connectionStub_, nullptr);
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_BLACKLIST);
    ScreenId id = INVALID_SCREEN_ID;
    std::vector<NodeId> blackList;

    MessageParcel data1;
    data1.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    auto res = connectionStub_->OnRemoteRequest(code, data1, reply, option);
    EXPECT_NE(res, ERR_NONE);

    MessageParcel data2;
    data2.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data2.WriteUint64(id);
    data2.WriteUInt64Vector(blackList);
    res = connectionStub_->OnRemoteRequest(code, data2, reply, option);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: SetVirtualScreenBlackListTest002
 * @tc.desc: Test SetVirtualScreenBlackList when screenManagerAgent_ is nullptr
 * @tc.type: FUNC
 * @tc.require: issue21114
 */
HWTEST_F(RSClientToServiceConnectionStubTest, SetVirtualScreenBlackListTest002, TestSize.Level2)
{
    ASSERT_NE(connectionStub_, nullptr);
    sptr<RSClientToServiceConnection> connection = iface_cast<RSClientToServiceConnection>(connectionStub_);
    ASSERT_NE(connection, nullptr);
    ASSERT_NE(connection->screenManagerAgent_, nullptr);

    std::vector<NodeId> blackList;
    auto res = connection->SetVirtualScreenBlackList(INVALID_SCREEN_ID, blackList);
    ASSERT_EQ(res, ERR_OK);

    auto screenManagerAgent = connection->screenManagerAgent_;
    connection->screenManagerAgent_ = nullptr;
    res = connection->SetVirtualScreenBlackList(INVALID_SCREEN_ID, blackList);
    EXPECT_EQ(res, StatusCode::SCREEN_NOT_FOUND);

    // restore screenManagerAgent
    connection->screenManagerAgent_ = screenManagerAgent;
    ASSERT_NE(connection->screenManagerAgent_, nullptr);
}

/**
 * @tc.name: GetDefaultScreenIdTest002
 * @tc.desc: Test GetDefaultScreenId when screenManagerAgent_ is nullptr
 * @tc.type: FUNC
 * @tc.require: issue21114
 */
HWTEST_F(RSClientToServiceConnectionStubTest, GetDefaultScreenIdTest002, TestSize.Level2)
{
    ASSERT_NE(connectionStub_, nullptr);
    sptr<RSClientToServiceConnection> connection = iface_cast<RSClientToServiceConnection>(connectionStub_);
    ASSERT_NE(connection, nullptr);
    ASSERT_NE(connection->screenManagerAgent_, nullptr);

    ScreenId screenId = INVALID_SCREEN_ID;
    auto res = connection->GetDefaultScreenId(screenId);
    ASSERT_EQ(res, ERR_OK);

    auto screenManagerAgent = connection->screenManagerAgent_;
    connection->screenManagerAgent_ = nullptr;
    res = connection->GetDefaultScreenId(screenId);
    EXPECT_EQ(res, ERR_INVALID_OPERATION);

    // restore screenManagerAgent
    connection->screenManagerAgent_ = screenManagerAgent;
    ASSERT_NE(connection->screenManagerAgent_, nullptr);
}

/**
 * @tc.name: CreateVirtualScreenTest001
 * @tc.desc: Test CreateVirtualScreen when screenManagerAgent_ is nullptr
 * @tc.type: FUNC
 * @tc.require: issue21114
 */
HWTEST_F(RSClientToServiceConnectionStubTest, CreateVirtualScreenTest001, TestSize.Level2)
{
    ASSERT_NE(connectionStub_, nullptr);
    sptr<RSClientToServiceConnection> connection = iface_cast<RSClientToServiceConnection>(connectionStub_);
    ASSERT_NE(connection, nullptr);
    ASSERT_NE(connection->screenManagerAgent_, nullptr);

    string name;
    uint32_t width = 0;
    uint32_t height = 0;
    ScreenId associatedScreenId = 0;
    int32_t flags = 0;
    std::vector<NodeId> whiteList;

    auto res = connection->CreateVirtualScreen(name, width, height, nullptr, associatedScreenId, flags, whiteList);
    ASSERT_NE(res, 0);
    width = 65537;
    height = 65537;
    res = connection->CreateVirtualScreen(name, width, height, nullptr, associatedScreenId, flags, whiteList);
    ASSERT_EQ(res, INVALID_SCREEN_ID);

    auto screenManagerAgent = connection->screenManagerAgent_;
    connection->screenManagerAgent_ = nullptr;
    res = connection->CreateVirtualScreen(name, width, height, nullptr, associatedScreenId, flags, whiteList);
    EXPECT_EQ(res, INVALID_SCREEN_ID);

    // restore screenManagerAgent
    connection->screenManagerAgent_ = screenManagerAgent;
    ASSERT_NE(connection->screenManagerAgent_, nullptr);
}

/**
 * @tc.name: AddVirtualScreenBlackListTest001
 * @tc.desc: Test AddVirtualScreenBlackList when screenManagerAgent_ is nullptr
 * @tc.type: FUNC
 * @tc.require: issue21114
 */
HWTEST_F(RSClientToServiceConnectionStubTest, AddVirtualScreenBlackListTest001, TestSize.Level2)
{
    ASSERT_NE(connectionStub_, nullptr);
    sptr<RSClientToServiceConnection> connection = iface_cast<RSClientToServiceConnection>(connectionStub_);
    ASSERT_NE(connection, nullptr);
    ASSERT_NE(connection->screenManagerAgent_, nullptr);

    std::vector<NodeId> blackList;
    int32_t repCode = 0;
    auto res = connection->AddVirtualScreenBlackList(INVALID_SCREEN_ID, blackList, repCode);
    ASSERT_EQ(res, ERR_OK);

    auto screenManagerAgent = connection->screenManagerAgent_;
    connection->screenManagerAgent_ = nullptr;
    res = connection->AddVirtualScreenBlackList(INVALID_SCREEN_ID, blackList, repCode);
    EXPECT_EQ(res, ERR_INVALID_VALUE);

    // restore screenManagerAgent
    connection->screenManagerAgent_ = screenManagerAgent;
    ASSERT_NE(connection->screenManagerAgent_, nullptr);
}

/**
 * @tc.name: RemoveVirtualScreenBlackListTest001
 * @tc.desc: Test RemoveVirtualScreenBlackList when screenManagerAgent_ is nullptr
 * @tc.type: FUNC
 * @tc.require: issue21114
 */
HWTEST_F(RSClientToServiceConnectionStubTest, RemoveVirtualScreenBlackListTest001, TestSize.Level2)
{
    ASSERT_NE(connectionStub_, nullptr);
    sptr<RSClientToServiceConnection> connection = iface_cast<RSClientToServiceConnection>(connectionStub_);
    ASSERT_NE(connection, nullptr);
    ASSERT_NE(connection->screenManagerAgent_, nullptr);

    std::vector<NodeId> blackList;
    int32_t repCode = 0;
    auto res = connection->RemoveVirtualScreenBlackList(INVALID_SCREEN_ID, blackList, repCode);
    ASSERT_EQ(res, ERR_OK);

    auto screenManagerAgent = connection->screenManagerAgent_;
    connection->screenManagerAgent_ = nullptr;
    res = connection->RemoveVirtualScreenBlackList(INVALID_SCREEN_ID, blackList, repCode);
    EXPECT_EQ(res, ERR_INVALID_VALUE);

    // restore screenManagerAgent
    connection->screenManagerAgent_ = screenManagerAgent;
    ASSERT_NE(connection->screenManagerAgent_, nullptr);
}

/**
 * @tc.name: SetVirtualScreenTypeBlackListTest001
 * @tc.desc: Test SetVirtualScreenTypeBlackList
 * @tc.type: FUNC
 * @tc.require: issue20886
 */
HWTEST_F(RSClientToServiceConnectionStubTest, SetVirtualScreenTypeBlackListTest001, TestSize.Level2)
{
    ASSERT_NE(connectionStub_, nullptr);
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_TYPE_BLACKLIST);
    ScreenId id = INVALID_SCREEN_ID;
    std::vector<NodeType> typeBlackListVector;

    MessageParcel data1;
    data1.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    auto res = connectionStub_->OnRemoteRequest(code, data1, reply, option);
    EXPECT_NE(res, ERR_NONE);

    MessageParcel data2;
    data2.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data2.WriteUint64(id);
    res = connectionStub_->OnRemoteRequest(code, data2, reply, option);
    EXPECT_EQ(res, ERR_NONE);

    MessageParcel data3;
    data3.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data3.WriteUint64(id);
    data3.WriteUInt8Vector(typeBlackListVector);
    res = connectionStub_->OnRemoteRequest(code, data3, reply, option);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: SetVirtualScreenTypeBlackListTest002
 * @tc.desc: Test SetVirtualScreenTypeBlackList when screenManagerAgent_ of connection is nullptr
 * @tc.type: FUNC
 * @tc.require: issue20886
 */
HWTEST_F(RSClientToServiceConnectionStubTest, SetVirtualScreenTypeBlackListTest002, TestSize.Level2)
{
    ASSERT_NE(connectionStub_, nullptr);
    sptr<RSClientToServiceConnection> connection = iface_cast<RSClientToServiceConnection>(connectionStub_);
    ASSERT_NE(connection, nullptr);

    ScreenId screenId = INVALID_SCREEN_ID;
    std::vector<NodeType> typeBlackListVector;
    int32_t repCode;

    auto screenManagerAgent = connection->screenManagerAgent_;
    connection->screenManagerAgent_ = nullptr;
    auto res = connection->SetVirtualScreenTypeBlackList(screenId, typeBlackListVector, repCode);
    ASSERT_EQ(res, ERR_INVALID_VALUE);

    // restore screenManagerAgent
    connection->screenManagerAgent_ = screenManagerAgent;
    ASSERT_NE(connection->screenManagerAgent_, nullptr);
}

/**
 * @tc.name: SetVirtualScreenTypeBlackListTest003
 * @tc.desc: Test SetVirtualScreenTypeBlackList when typeBlackListVector is empty
 * @tc.type: FUNC
 * @tc.require: issue20886
 */
HWTEST_F(RSClientToServiceConnectionStubTest, SetVirtualScreenTypeBlackListTest003, TestSize.Level2)
{
    ASSERT_NE(connectionStub_, nullptr);
    sptr<RSClientToServiceConnection> connection = iface_cast<RSClientToServiceConnection>(connectionStub_);
    ASSERT_NE(connection, nullptr);

    ScreenId screenId = INVALID_SCREEN_ID;
    std::vector<NodeType> typeBlackListVector;
    int32_t repCode;
    auto res = connection->SetVirtualScreenTypeBlackList(screenId, typeBlackListVector, repCode);
    ASSERT_EQ(res, ERR_OK);
}

/**
 * @tc.name: SetVirtualScreenTypeBlackListTest004
 * @tc.desc: Test SetVirtualScreenTypeBlackList when typeBlackListVector size is over MAX_SPECIAL_LAYER_NUM
 * @tc.type: FUNC
 * @tc.require: issue20886
 */
HWTEST_F(RSClientToServiceConnectionStubTest, SetVirtualScreenTypeBlackListTest004, TestSize.Level2)
{
    ASSERT_NE(connectionStub_, nullptr);
    sptr<RSClientToServiceConnection> connection = iface_cast<RSClientToServiceConnection>(connectionStub_);
    ASSERT_NE(connection, nullptr);

    ScreenId screenId = INVALID_SCREEN_ID;
    std::vector<NodeType> typeBlackListVector(MAX_SPECIAL_LAYER_NUM + 1);
    int32_t repCode;
    auto res = connection->SetVirtualScreenTypeBlackList(screenId, typeBlackListVector, repCode);
    ASSERT_EQ(res, ERR_OK);
}

/**
 * @tc.name: NotifyAppStrategyConfigChangeEvent001
 * @tc.desc: Test NotifyAppStrategyConfigChangeEvent001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, NotifyAppStrategyConfigChangeEvent001, TestSize.Level2)
{
    ASSERT_NE(connectionStub_, nullptr);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::NOTIFY_APP_STRATEGY_CONFIG_CHANGE_EVENT);
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    auto res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    std::string pkgName = "testpkg";
    uint32_t listSize = 1;
    uint32_t errListSize = 100;
    std::string key = "testkey";
    std::string value = "testvalue";

    MessageParcel data2;
    data2.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data2.WriteString(pkgName);
    res = connectionStub_->OnRemoteRequest(code, data2, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    MessageParcel data3;
    data3.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data3.WriteString(pkgName);
    data3.WriteUint32(errListSize);
    res = connectionStub_->OnRemoteRequest(code, data3, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    MessageParcel data5;
    data5.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data5.WriteString(pkgName);
    data5.WriteUint32(listSize);
    data5.WriteString(key);
    res = connectionStub_->OnRemoteRequest(code, data5, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    MessageParcel data6;
    data6.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data6.WriteString(pkgName);
    data6.WriteUint32(listSize);
    data6.WriteString(key);
    data6.WriteString(value);
    res = connectionStub_->OnRemoteRequest(code, data6, reply, option);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: NotifySoftVsyncRateDiscountEvent001
 * @tc.desc: Test NotifySoftVsyncRateDiscountEvent001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, NotifySoftVsyncRateDiscountEvent001, TestSize.Level2)
{
    ASSERT_NE(connectionStub_, nullptr);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::NOTIFY_SOFT_VSYNC_RATE_DISCOUNT_EVENT);
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    auto res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    uint32_t pid = 0;
    std::string name = "test";
    uint32_t rateDiscount = 0;

    MessageParcel data2;
    data2.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data2.WriteUint32(pid);
    res = connectionStub_->OnRemoteRequest(code, data2, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    MessageParcel data3;
    data3.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data3.WriteUint32(pid);
    data3.WriteString(name);
    res = connectionStub_->OnRemoteRequest(code, data3, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    MessageParcel data4;
    data4.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data4.WriteUint32(pid);
    data4.WriteString(name);
    data4.WriteUint32(rateDiscount);
    connectionStub_->OnRemoteRequest(code, data4, reply, option);
}

/**
 * @tc.name: SetWindowExpectedRefreshRate001
 * @tc.desc: Test SetWindowExpectedRefreshRate001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, SetWindowExpectedRefreshRate001, TestSize.Level2)
{
    ASSERT_NE(connectionStub_, nullptr);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::NOTIFY_WINDOW_EXPECTED_BY_WINDOW_ID);
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    auto res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);
    uint32_t mapSize = 1;
    uint32_t errMapSize = 200;
    uint64_t windowId = 0;
    std::string eventName = "testEvent";
    bool eventStatus = false;
    MessageParcel data2;
    data2.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data2.WriteUint32(errMapSize);
    res = connectionStub_->OnRemoteRequest(code, data2, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    MessageParcel data3;
    data3.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data3.WriteUint32(mapSize);
    res = connectionStub_->OnRemoteRequest(code, data3, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    MessageParcel data4;
    data4.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data4.WriteUint32(mapSize);
    data4.WriteUint64(windowId);
    res = connectionStub_->OnRemoteRequest(code, data4, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    MessageParcel data5;
    data5.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data5.WriteUint32(mapSize);
    data5.WriteUint64(windowId);
    data5.WriteString(eventName);
    res = connectionStub_->OnRemoteRequest(code, data5, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    MessageParcel data6;
    data6.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data6.WriteUint32(mapSize);
    data6.WriteUint64(windowId);
    data6.WriteString(eventName);
    data6.WriteBool(eventStatus);
    res = connectionStub_->OnRemoteRequest(code, data6, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);
}

/**
 * @tc.name: SetWindowExpectedRefreshRate002
 * @tc.desc: Test SetWindowExpectedRefreshRate002
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, SetWindowExpectedRefreshRate002, TestSize.Level2)
{
    ASSERT_NE(connectionStub_, nullptr);
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::NOTIFY_WINDOW_EXPECTED_BY_WINDOW_ID);

    uint32_t mapSize = 1;
    uint64_t windowId = 0;
    std::string eventName = "testEvent";
    bool eventStatus = false;
    uint32_t minRefreshRate = 60;
    uint32_t maxRefreshRate = 120;
    std::string description = "test";

    MessageParcel data7;
    data7.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data7.WriteUint32(mapSize);
    data7.WriteUint64(windowId);
    data7.WriteString(eventName);
    data7.WriteBool(eventStatus);
    data7.WriteUint32(minRefreshRate);
    auto res = connectionStub_->OnRemoteRequest(code, data7, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    MessageParcel data8;
    data8.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data8.WriteUint32(mapSize);
    data8.WriteUint64(windowId);
    data8.WriteString(eventName);
    data8.WriteBool(eventStatus);
    data8.WriteUint32(minRefreshRate);
    data8.WriteUint32(maxRefreshRate);
    res = connectionStub_->OnRemoteRequest(code, data8, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    MessageParcel data9;
    data9.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data9.WriteUint32(mapSize);
    data9.WriteUint64(windowId);
    data9.WriteString(eventName);
    data9.WriteBool(eventStatus);
    data9.WriteUint32(minRefreshRate);
    data9.WriteUint32(maxRefreshRate);
    data9.WriteString(description);
    res = connectionStub_->OnRemoteRequest(code, data9, reply, option);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: SetWindowExpectedRefreshRate003
 * @tc.desc: Test SetWindowExpectedRefreshRate003
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, SetWindowExpectedRefreshRate003, TestSize.Level2)
{
    ASSERT_NE(connectionStub_, nullptr);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::NOTIFY_WINDOW_EXPECTED_BY_VSYNC_NAME);
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    auto res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    uint32_t mapSize = 1;
    uint32_t errMapSize = 200;
    std::string vsyncName = "testVsync";
    std::string eventName = "testEvent";
    bool eventStatus = false;

    MessageParcel data2;
    data2.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data2.WriteUint32(errMapSize);
    res = connectionStub_->OnRemoteRequest(code, data2, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    MessageParcel data3;
    data3.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data3.WriteUint32(mapSize);
    res = connectionStub_->OnRemoteRequest(code, data3, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    MessageParcel data4;
    data4.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data4.WriteUint32(mapSize);
    data4.WriteString(vsyncName);
    res = connectionStub_->OnRemoteRequest(code, data4, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    MessageParcel data5;
    data5.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data5.WriteUint32(mapSize);
    data5.WriteString(vsyncName);
    data5.WriteString(eventName);
    res = connectionStub_->OnRemoteRequest(code, data5, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    MessageParcel data6;
    data6.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data6.WriteUint32(mapSize);
    data6.WriteString(vsyncName);
    data6.WriteString(eventName);
    data6.WriteBool(eventStatus);
    res = connectionStub_->OnRemoteRequest(code, data6, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);
}

/**
 * @tc.name: SetWindowExpectedRefreshRate004
 * @tc.desc: Test SetWindowExpectedRefreshRate004
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, SetWindowExpectedRefreshRate004, TestSize.Level2)
{
    ASSERT_NE(connectionStub_, nullptr);
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::NOTIFY_WINDOW_EXPECTED_BY_VSYNC_NAME);
    
    uint32_t mapSize = 1;
    std::string vsyncName = "testVsync";
    std::string eventName = "testEvent";
    bool eventStatus = false;
    uint32_t minRefreshRate = 60;
    uint32_t maxRefreshRate = 120;
    std::string description = "test";

    MessageParcel data7;
    data7.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data7.WriteUint32(mapSize);
    data7.WriteString(vsyncName);
    data7.WriteString(eventName);
    data7.WriteBool(eventStatus);
    data7.WriteUint32(minRefreshRate);
    auto res = connectionStub_->OnRemoteRequest(code, data7, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    MessageParcel data8;
    data8.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data8.WriteUint32(mapSize);
    data8.WriteString(vsyncName);
    data8.WriteString(eventName);
    data8.WriteBool(eventStatus);
    data8.WriteUint32(minRefreshRate);
    data8.WriteUint32(maxRefreshRate);
    res = connectionStub_->OnRemoteRequest(code, data8, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    MessageParcel data9;
    data9.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data9.WriteUint32(mapSize);
    data9.WriteString(vsyncName);
    data9.WriteString(eventName);
    data9.WriteBool(eventStatus);
    data9.WriteUint32(minRefreshRate);
    data9.WriteUint32(maxRefreshRate);
    data9.WriteString(description);
    res = connectionStub_->OnRemoteRequest(code, data9, reply, option);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: SetRefreshRateMode
 * @tc.desc: Test SetRefreshRateMode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, SetRefreshRateMode, TestSize.Level2)
{
    ASSERT_NE(connectionStub_, nullptr);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::SET_REFRESH_RATE_MODE);

    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    auto res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    int32_t mode = 0;
    MessageParcel data2;
    data2.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data2.WriteInt32(mode);
    res = connectionStub_->OnRemoteRequest(code, data2, reply, option);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: SyncFrameRateRange001
 * @tc.desc: Test SyncFrameRateRange001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, SyncFrameRateRange001, TestSize.Level2)
{
    ASSERT_NE(connectionStub_, nullptr);
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::SYNC_FRAME_RATE_RANGE);
    MessageParcel data;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    auto res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    uint64_t frameRateLinkerId = static_cast<uint64_t>(getpid()) << 32;
    MessageParcel data2;
    data2.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data2.WriteInt64(frameRateLinkerId);
    res = connectionStub_->OnRemoteRequest(code, data2, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    uint32_t min{0};
    uint32_t max{0};
    uint32_t preferred{0};
    MessageParcel data3;
    data3.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data3.WriteUint64(frameRateLinkerId);
    data3.WriteUint32(min);
    res = connectionStub_->OnRemoteRequest(code, data3, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    MessageParcel data4;
    data4.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data4.WriteUint64(frameRateLinkerId);
    data4.WriteUint32(min);
    data4.WriteUint32(max);
    res = connectionStub_->OnRemoteRequest(code, data4, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    MessageParcel data5;
    data5.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data5.WriteUint64(frameRateLinkerId);
    data5.WriteUint32(min);
    data5.WriteUint32(max);
    data5.WriteUint32(preferred);
    res = connectionStub_->OnRemoteRequest(code, data5, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    MessageParcel data6;
    frameRateLinkerId = 0;
    data6.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data6.WriteUint64(frameRateLinkerId);
    res = connectionStub_->OnRemoteRequest(code, data6, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);
}

/**
 * @tc.name: SyncFrameRateRange002
 * @tc.desc: Test SyncFrameRateRange002
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, SyncFrameRateRange002, TestSize.Level2)
{
    ASSERT_NE(connectionStub_, nullptr);
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::SYNC_FRAME_RATE_RANGE);

    uint64_t frameRateLinkerId = static_cast<uint64_t>(getpid()) << 32;
    uint32_t min{0};
    uint32_t max{0};
    uint32_t preferred{0};
    uint32_t type{0};
    int32_t animatorExpectedFrameRate{0};
    MessageParcel data6;
    data6.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data6.WriteUint64(frameRateLinkerId);
    data6.WriteUint32(min);
    data6.WriteUint32(max);
    data6.WriteUint32(preferred);
    data6.WriteUint32(type);
    auto res = connectionStub_->OnRemoteRequest(code, data6, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    MessageParcel data7;
    data7.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data7.WriteUint64(frameRateLinkerId);
    data7.WriteUint32(min);
    data7.WriteUint32(max);
    data7.WriteUint32(preferred);
    data7.WriteUint32(type);
    data7.WriteInt32(animatorExpectedFrameRate);
    res = connectionStub_->OnRemoteRequest(code, data7, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);
}

/**
 * @tc.name: NotifyTouchEventTest001
 * @tc.desc: Test NotifyTouchEvent when mainThread_ is nullptr
 * @tc.type: FUNC
 * @tc.require: issue20726
 */
HWTEST_F(RSClientToServiceConnectionStubTest, NotifyTouchEventTest001, TestSize.Level1)
{
    MessageParcel data1;
    MessageParcel reply;
    MessageOption option;
    int32_t touchStatus{0};
    int32_t touchCount{0};
    int32_t sourceType = 2;
    uint32_t code =
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::NOTIFY_TOUCH_EVENT);

    data1.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data1.WriteInt32(touchStatus);
    data1.WriteInt32(touchCount);
    data1.WriteInt32(sourceType);
    int ret = connectionStub_->OnRemoteRequest(code, data1, reply, option);
    ASSERT_EQ(ret, ERR_NONE);

    MessageParcel data2;
    data2.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data2.WriteInt32(touchStatus);
    data2.WriteInt32(touchCount);
    sourceType = -1;
    data2.WriteInt32(sourceType);
    connectionStub_->OnRemoteRequest(code, data2, reply, option);
}

/**
 * @tc.name: RegisterSharedTypefaceTest001
 * @tc.desc: Test RegisterSharedTypeface
 * @tc.type: FUNC
 * @tc.require: issue20726
 */
HWTEST_F(RSClientToServiceConnectionStubTest, RegisterSharedTypefaceTest001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    uint32_t interfaceCode = static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::REGISTER_SHARED_TYPEFACE);

    Drawing::SharedTypeface sharedTypeface;
    sharedTypeface.fd_ = open("/system/fonts/HarmonyOS_Sans_SC.ttf", O_RDONLY);
    EXPECT_EQ(RSMarshallingHelper::Marshalling(data, sharedTypeface), true);

    auto res = connectionStub_->OnRemoteRequest(static_cast<uint32_t>(interfaceCode), data, reply, option);
    ASSERT_EQ(res, ERR_OK);
}

/**
 * @tc.name: RegisterTypefaceTest001
 * @tc.desc: test register typeface and unregister typeface
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, RegisterTypefaceTest001, TestSize.Level1)
{
    ASSERT_NE(connectionStub_, nullptr);
    auto tf = Drawing::Typeface::MakeDefault();
    uint64_t uniqueId = 1;
    EXPECT_TRUE(connectionStub_->RegisterTypeface(uniqueId, tf));
    EXPECT_TRUE(connectionStub_->UnRegisterTypeface(uniqueId));
    EXPECT_TRUE(connectionStub_->UnRegisterTypeface(0));
}

/**
 * @tc.name: RegisterTypefaceTest002
 * @tc.desc: test register shared typeface and unregister shared typeface
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, RegisterTypefaceTest002, TestSize.Level1)
{
    ASSERT_NE(connectionStub_, nullptr);
    std::vector<char> content;
    LoadBufferFromFile("/system/fonts/Roboto-Regular.ttf", content);
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromAshmem(reinterpret_cast<const uint8_t*>(content.data()), content.size(), 0, "test");
    ASSERT_NE(typeface, nullptr);
    int32_t needUpdate;
    pid_t pid = getpid();
    Drawing::SharedTypeface sharedTypeface;
    sharedTypeface.id_ = (static_cast<uint64_t>(pid) << 32) | static_cast<uint64_t>(typeface->GetHash());
    sharedTypeface.size_ = typeface->GetSize();
    sharedTypeface.fd_ = typeface->GetFd();
    EXPECT_NE(
        connectionStub_->RegisterTypeface(sharedTypeface, needUpdate), -1);
    EXPECT_TRUE(connectionStub_->UnRegisterTypeface(typeface->GetHash()));
}

/**
 * @tc.name: SetWatermarkTest003
 * @tc.desc: Test SetWatermark normal case
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, SetWatermarkTest001, TestSize.Level1)
{
    std::string name = "test_watermark";
    int width = 10;
    int height = 10;
    Media::InitializationOptions opts;
    opts.size.width = width;
    opts.size.height = height;
    std::shared_ptr<Media::PixelMap> pixelmap = Media::PixelMap::Create(opts);
    EXPECT_NE(pixelmap, nullptr);
    bool success = true;

    auto res = connectionStub_->SetWatermark(name, pixelmap, success);
    // Should succeed with valid connectionStub_
    EXPECT_EQ(res, ERR_OK);
}

/**
 * @tc.name: ShowWatermarkTest003
 * @tc.desc: Test ShowWatermark normal case
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, ShowWatermarkTest001, TestSize.Level1)
{
    bool isShow = true;
    int width = 10;
    int height = 10;
    Media::InitializationOptions opts;
    opts.size.width = width;
    opts.size.height = height;
    std::shared_ptr<Media::PixelMap> pixelmap = Media::PixelMap::Create(opts);
    EXPECT_NE(pixelmap, nullptr);

    // Should succeed with valid connectionStub_ without crash
    connectionStub_->ShowWatermark(pixelmap, isShow);
}

/**
 * @tc.name: GetSurfaceRootNodeIdTest001
 * @tc.desc: Test GetSurfaceRootNodeId with empty serviceToRenderConns
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, GetSurfaceRootNodeIdTest001, TestSize.Level1)
{
    NodeId windowNodeId = 0;
    connectionStub_->GetSurfaceRootNodeId(windowNodeId);
    // Should return without setting windowNodeId
    EXPECT_EQ(windowNodeId, 0);
}

/**
 * @tc.name: GetRefreshInfoTest001
 * @tc.desc: Test GetRefreshInfo with renderServiceAgent_ is nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, GetRefreshInfoTest001, TestSize.Level1)
{
    pid_t pid = 1001;
    std::string enable;

    connectionStub_->GetRefreshInfo(pid, enable);
    EXPECT_TRUE(enable.empty());
}

/**
 * @tc.name: GetRefreshInfoToSPTest001
 * @tc.desc: Test GetRefreshInfoToSP with renderServiceAgent_ is nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, GetRefreshInfoToSPTest001, TestSize.Level1)
{
    NodeId id = 1001;
    std::string enable;

    connectionStub_->GetRefreshInfoToSP(id, enable);
    EXPECT_TRUE(enable.empty());
}

/**
 * @tc.name: GetConnectionTest
 * @tc.desc: Test GetConnectionTest with param is
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, GetConnectionTest, TestSize.Level1)
{
    auto connection =
        sptr<RSClientToServiceConnection>::MakeSptr(0, nullptr,
            renderProcessManagerAgent_, nullptr, nullptr,
            renderService_.vsyncManager_->GetVsyncManagerAgent());
    EXPECT_EQ(connection != nullptr, true);
}

/**
 * @tc.name: ATC_NotifyRefreshRateEvent
 * @tc.desc: Test ATC_NotifyRefreshRateEvent
 * @tc.type: FUNC
 * @tc.require: issue20726
 */
HWTEST_F(RSClientToServiceConnectionStubTest, ATC_NotifyRefreshRateEvent, TestSize.Level1)
{
    ASSERT_NE(connectionStub_, nullptr);
    EventInfo eventInfo;
    eventInfo.eventName = "GPU_FREQ_PREF";
    connectionStub_->NotifyRefreshRateEvent(eventInfo);
    ASSERT_EQ(eventInfo.eventName, "GPU_FREQ_PREF");
}

/**
 * @tc.name: testnullptrCase001
 * @tc.desc: Test testnullptrCase
 * @tc.type: FUNC
 * @tc.require: issue20726
 */
HWTEST_F(RSClientToServiceConnectionStubTest, testnullptrCase001, TestSize.Level1)
{
    auto connection = sptr<RSClientToServiceConnection>::MakeSptr(0, renderServiceAgent_,
            renderProcessManagerAgent_, screenManagerAgent_, nullptr,
            renderService_.vsyncManager_->GetVsyncManagerAgent());
    ASSERT_NE(connection, nullptr);

    auto renderProcessManagerAgent = connection->renderProcessManagerAgent_;
    auto hgmContext = connection->hgmContext_;
    auto screenManagerAgent = connection->screenManagerAgent_;
    auto vsyncManagerAgent = connection->vsyncManagerAgent_;
    auto renderServiceAgent = connection->renderServiceAgent_;

    connection->renderProcessManagerAgent_ = nullptr;
    // test GetPixelMapByProcessId
    std::vector<PixelMapInfo> pixelMapInfoVector = {};
    int32_t repCode = 0;
    connection->GetPixelMapByProcessId(pixelMapInfoVector, 0, repCode);
    // test CreatePixelMapFromSurface
    {
        Rect srcRect;
        std::shared_ptr<Media::PixelMap> pixelMap = nullptr;
        connection->CreatePixelMapFromSurface(nullptr, srcRect, pixelMap, false);
    }
    // test SetWatermark
    std::string name("");
    bool success = false;
    connection->SetWatermark(name, nullptr, success);
    connection->screenManagerAgent_ = nullptr;
    // test GetDefaultScreenId
    uint64_t screenId = 0;
    connection->GetDefaultScreenId(screenId);
    // test GetAllScreenIds
    connection->GetAllScreenIds();
    // test SetVirtualScreenBlackList
    std::vector<NodeId> virtualScreenBlackList = {};
    connection->SetVirtualScreenBlackList(INVALID_SCREEN_ID, virtualScreenBlackList);
    // test AddVirtualScreenBlackList
    connection->AddVirtualScreenBlackList(INVALID_SCREEN_ID, virtualScreenBlackList, repCode);
    // test RemoveVirtualScreenBlackList
    connection->RemoveVirtualScreenBlackList(INVALID_SCREEN_ID, virtualScreenBlackList, repCode);
    // test AddVirtualScreenWhiteList
    std::vector<NodeId> whiteList = {};
    connection->AddVirtualScreenWhiteList(INVALID_SCREEN_ID, whiteList, repCode);
    // test RemoveVirtualScreenWhiteList
    connection->RemoveVirtualScreenWhiteList(INVALID_SCREEN_ID, whiteList, repCode);
    // test SetVirtualScreenSecurityExemptionList
    std::vector<NodeId> securityExemptionList = {};
    connection->SetVirtualScreenSecurityExemptionList(INVALID_SCREEN_ID, securityExemptionList);
    // test SetScreenSecurityMask
    connection->SetScreenSecurityMask(INVALID_SCREEN_ID, nullptr);
    // test SetMirrorScreenVisibleRect
    Rect mainScreenRect;
    connection->SetMirrorScreenVisibleRect(INVALID_SCREEN_ID, mainScreenRect, false);
    // test SetCastScreenEnableSkipWindow
    connection->SetCastScreenEnableSkipWindow(INVALID_SCREEN_ID, false);
    // test SetVirtualScreenSurface
    connection->SetVirtualScreenSurface(INVALID_SCREEN_ID, nullptr);
    // test RemoveVirtualScreen
    connection->RemoveVirtualScreen(INVALID_SCREEN_ID);
    // test SetScreenChangeCallback
    connection->SetScreenChangeCallback(nullptr);
    // test SetScreenSwitchingNotifyCallback
    connection->SetScreenSwitchingNotifyCallback(nullptr);
    // test SetBrightnessInfoChangeCallback
    connection->SetBrightnessInfoChangeCallback(nullptr);
    // test SetScreenActiveMode
    connection->SetScreenActiveMode(INVALID_SCREEN_ID, 0);
    // test SetScreenRefreshRate
    connection->hgmContext_ = nullptr;
    connection->SetScreenRefreshRate(INVALID_SCREEN_ID, 0, 0);
    // test SetRefreshRateMode
    connection->SetRefreshRateMode(0);
    // test SyncFrameRateRange
    FrameRateLinkerId frameRateLinkerId = 0;
    FrameRateRange range;
    connection->SyncFrameRateRange(frameRateLinkerId, range, 0);
    connection->renderServiceAgent_ = nullptr;
    connection->SyncFrameRateRange(frameRateLinkerId, range, 0);
    connection->hgmContext_ = hgmContext;
    connection->SyncFrameRateRange(frameRateLinkerId, range, 0);
    connection->hgmContext_ = nullptr;
    // test UnregisterFrameRateLinker
    connection->UnregisterFrameRateLinker(frameRateLinkerId);
    // test GetScreenCurrentRefreshRate
    connection->GetScreenCurrentRefreshRate(INVALID_SCREEN_ID);
    // test GetScreenSupportedRefreshRates
    connection->GetScreenSupportedRefreshRates(INVALID_SCREEN_ID);
    // test SetShowRefreshRateEnabled
    connection->SetShowRefreshRateEnabled(false, 0);
    // test GetRealtimeRefreshRate
    connection->GetRealtimeRefreshRate(INVALID_SCREEN_ID);
    connection->screenManagerAgent_ = screenManagerAgent;
    connection->GetRealtimeRefreshRate(INVALID_SCREEN_ID);
    connection->renderProcessManagerAgent_ = renderProcessManagerAgent;
    connection->GetRealtimeRefreshRate(INVALID_SCREEN_ID);
    connection->hgmContext_ = hgmContext;
    connection->GetRealtimeRefreshRate(INVALID_SCREEN_ID);
    connection->screenManagerAgent_ = nullptr;
    connection->GetRealtimeRefreshRate(INVALID_SCREEN_ID);
    connection->renderProcessManagerAgent_ = nullptr;
    connection->hgmContext_ = nullptr;
    // test GetRefreshInfo
    std::string GetRefreshInfoEnable = "";
    connection->GetRefreshInfo(0, GetRefreshInfoEnable);
    // test GetRefreshInfoToSP
    connection->GetRefreshInfoToSP(INVALID_SCREEN_ID, GetRefreshInfoEnable);
    // test GetRefreshInfoByPidAndUniqueId
    connection->GetRefreshInfoByPidAndUniqueId(0, 0, GetRefreshInfoEnable);
    // test GetCurrentRefreshRateMode
    connection->GetCurrentRefreshRateMode();
    // test SetPhysicalScreenResolution
    connection->SetPhysicalScreenResolution(INVALID_SCREEN_ID, 0, 0);
    connection->screenManagerAgent_ = screenManagerAgent;
    connection->SetPhysicalScreenResolution(INVALID_SCREEN_ID, 0, 0);
    connection->screenManagerAgent_ = nullptr;
    // test SetRogScreenResolution
    connection->SetRogScreenResolution(INVALID_SCREEN_ID, 0, 0);
    // test GetRogScreenResolution
    uint32_t width = 0;
    uint32_t height = 0;
    connection->GetRogScreenResolution(INVALID_SCREEN_ID, width, height);
    // test MarkPowerOffNeedProcessOneFrame
    connection->MarkPowerOffNeedProcessOneFrame();
    // test RepaintEverything
    connection->RepaintEverything();
    // test ForceRefreshOneFrameWithNextVSync
    connection->renderProcessManagerAgent_ = renderProcessManagerAgent;
    ASSERT_NE(connection->renderProcessManagerAgent_, nullptr);
    connection->ForceRefreshOneFrameWithNextVSync();
    connection->renderProcessManagerAgent_ = nullptr;
    // test DisablePowerOffRenderControl
    connection->DisablePowerOffRenderControl(INVALID_SCREEN_ID);
    // test SetScreenPowerStatus
    connection->SetScreenPowerStatus(INVALID_SCREEN_ID, ScreenPowerStatus::INVALID_POWER_STATUS);
    connection->screenManagerAgent_ = screenManagerAgent;
    connection->SetScreenPowerStatus(INVALID_SCREEN_ID, ScreenPowerStatus::INVALID_POWER_STATUS);
    connection->screenManagerAgent_ = nullptr;
    connection->renderServiceAgent_ = renderServiceAgent;
    connection->SetScreenPowerStatus(INVALID_SCREEN_ID, ScreenPowerStatus::INVALID_POWER_STATUS);
    connection->renderServiceAgent_ = nullptr;
    // test GetVirtualScreenResolution
    connection->GetVirtualScreenResolution(INVALID_SCREEN_ID);
    connection->screenManagerAgent_ = screenManagerAgent;
    connection->GetVirtualScreenResolution(INVALID_SCREEN_ID);
    connection->screenManagerAgent_ = nullptr;
    // test GetTotalAppMemSize
    float cpuMemSize = 0.0;
    float gpuMemSize = 0.0;
    connection->GetTotalAppMemSize(cpuMemSize, gpuMemSize);
    // test GetMemoryGraphic
    MemoryGraphic memoryGraphic;
    connection->GetMemoryGraphic(0, memoryGraphic);
    // test GetMemoryGraphics
    std::vector<MemoryGraphic> memoryGraphics = {};
    connection->GetMemoryGraphics(memoryGraphics);
    // test GetScreenSupportedModes
    connection->GetScreenSupportedModes(INVALID_SCREEN_ID);
    // test GetScreenCapability
    connection->GetScreenCapability(INVALID_SCREEN_ID);
    // test GetScreenPowerStatus
    uint32_t status = 0;
    connection->GetScreenPowerStatus(INVALID_SCREEN_ID, status);
    // test GetScreenData
    connection->GetScreenData(INVALID_SCREEN_ID);
    // test GetScreenBacklight
    int32_t level = 0;
    connection->GetScreenBacklight(INVALID_SCREEN_ID, level);
    // test SetScreenBacklight
    connection->SetScreenBacklight(INVALID_SCREEN_ID, level);
    // test GetScreenSupportedColorGamuts
    std::vector<ScreenColorGamut> gamutMode = {};
    connection->GetScreenSupportedColorGamuts(INVALID_SCREEN_ID, gamutMode);
    // test GetScreenSupportedMetaDataKeys
    std::vector<ScreenHDRMetadataKey> keys = {};
    connection->GetScreenSupportedMetaDataKeys(INVALID_SCREEN_ID, keys);
    // test GetScreenColorGamut and SetScreenColorGamut
    ScreenColorGamut screenColorGamut;
    connection->SetScreenColorGamut(INVALID_SCREEN_ID, 0);
    connection->GetScreenColorGamut(INVALID_SCREEN_ID, screenColorGamut);

    // restore
    connection->renderProcessManagerAgent_ = renderProcessManagerAgent;
    connection->hgmContext_ = hgmContext;
    connection->screenManagerAgent_ = screenManagerAgent;
    connection->vsyncManagerAgent_ = vsyncManagerAgent;
    connection->renderServiceAgent_ = renderServiceAgent;
}

/**
 * @tc.name: testnullptrCase002
 * @tc.desc: Test testnullptrCase
 * @tc.type: FUNC
 * @tc.require: issue20726
 */
HWTEST_F(RSClientToServiceConnectionStubTest, testnullptrCase002, TestSize.Level1)
{
    auto connection = sptr<RSClientToServiceConnection>::MakeSptr(0, renderServiceAgent_,
            renderProcessManagerAgent_, screenManagerAgent_, nullptr,
            renderService_.vsyncManager_->GetVsyncManagerAgent());
    ASSERT_NE(connection, nullptr);

    auto renderProcessManagerAgent = connection->renderProcessManagerAgent_;
    auto hgmContext = connection->hgmContext_;
    auto screenManagerAgent = connection->screenManagerAgent_;
    auto vsyncManagerAgent = connection->vsyncManagerAgent_;
    auto renderServiceAgent = connection->renderServiceAgent_;

    connection->screenManagerAgent_ = screenManagerAgent;
    connection->SetScreenColorGamut(INVALID_SCREEN_ID, 0);
    connection->GetScreenColorGamut(INVALID_SCREEN_ID, screenColorGamut);
    connection->screenManagerAgent_ = nullptr;
    // test SetScreenGamutMap
    ScreenGamutMap gamutMapMode = ScreenGamutMap::GAMUT_MAP_CONSTANT;
    connection->SetScreenGamutMap(INVALID_SCREEN_ID, gamutMapMode);
    // test SetScreenCorrection
    ScreenRotation screenRotation = ScreenRotation::ROTATION_0;
    connection->SetScreenCorrection(INVALID_SCREEN_ID, screenRotation);
    // test SetVirtualMirrorScreenCanvasRotation and SetVirtualScreenAutoRotation
    connection->SetVirtualMirrorScreenCanvasRotation(INVALID_SCREEN_ID, false);
    connection->SetVirtualScreenAutoRotation(INVALID_SCREEN_ID, false);
    connection->screenManagerAgent_ = screenManagerAgent;
    connection->SetVirtualMirrorScreenCanvasRotation(INVALID_SCREEN_ID, false);
    connection->SetVirtualScreenAutoRotation(INVALID_SCREEN_ID, false);
    connection->screenManagerAgent_ = nullptr;
    // test SetVirtualMirrorScreenScaleMode
    ScreenScaleMode scaleMode = ScreenScaleMode::FILL_MODE;
    connection->SetVirtualMirrorScreenScaleMode(INVALID_SCREEN_ID, scaleMode);
    // test GetScreenGamutMap
    connection->GetScreenGamutMap(INVALID_SCREEN_ID, gamutMapMode);
    // test GetScreenHDRCapability
    RSScreenHDRCapability screenHdrCapability;
    connection->GetScreenHDRCapability(INVALID_SCREEN_ID, screenHdrCapability);
    // test GetPixelFormat and SetPixelFormat
    GraphicPixelFormat pixelFormat = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_CLUT8;
    int32_t pixelFormatResCode = 0;
    connection->SetPixelFormat(INVALID_SCREEN_ID, pixelFormat, pixelFormatResCode);
    connection->GetPixelFormat(INVALID_SCREEN_ID, pixelFormat, pixelFormatResCode);
    // test GetScreenSupportedHDRFormats
    std::vector<ScreenHDRFormat> hdrFormats = {};
    int32_t hdrFormatsResCode = 0;
    connection->GetScreenSupportedHDRFormats(INVALID_SCREEN_ID, hdrFormats, hdrFormatsResCode, nullptr);
    // test GetScreenHDRFormat
    ScreenHDRFormat hdrFormat;
    int32_t hdrFormatResCode = 0;
    connection->GetScreenHDRFormat(INVALID_SCREEN_ID, hdrFormat, hdrFormatResCode);
    // test GetScreenSupportedColorSpaces
    std::vector<GraphicCM_ColorSpaceType> colorSpaces;
    int32_t colorSpacesResCode = 0;
    connection->GetScreenSupportedColorSpaces(INVALID_SCREEN_ID, colorSpaces, colorSpacesResCode);
    // test GetScreenColorSpace and SetScreenColorSpace
    GraphicCM_ColorSpaceType colorSpace = GraphicCM_ColorSpaceType::GRAPHIC_CM_COLORSPACE_NONE;
    connection->SetScreenColorSpace(INVALID_SCREEN_ID, colorSpace, colorSpacesResCode);
    connection->GetScreenColorSpace(INVALID_SCREEN_ID, colorSpace, colorSpacesResCode);
    // test GetScreenType
    RSScreenType screenType;
    connection->GetScreenType(INVALID_SCREEN_ID, screenType);
    // test RegisterTypeface
    std::shared_ptr<Drawing::Typeface> typeface = nullptr;
    connection->RegisterTypeface(0, typeface);
    // test UnRegisterTypeface
    connection->UnRegisterTypeface(0);
    // test GetDisplayIdentificationData
    uint8_t outPort = 0;
    std::vector<uint8_t> edidData = {};
    connection->GetDisplayIdentificationData(INVALID_SCREEN_ID, outPort, edidData);
    // test SetVirtualScreenRefreshRate and SetScreenActiveRect
    int32_t resCode = 0;
    uint32_t actualRefreshRate = 0;
    connection->SetVirtualScreenRefreshRate(0, 0, actualRefreshRate, resCode);
    Rect activeRect;
    uint32_t setScreenActiveRectRepCode = 0;
    connection->SetScreenActiveRect(INVALID_SCREEN_ID, activeRect, setScreenActiveRectRepCode);
    connection->screenManagerAgent_ = screenManagerAgent;
    connection->SetVirtualScreenRefreshRate(0, 0, actualRefreshRate, resCode);
    connection->SetScreenActiveRect(INVALID_SCREEN_ID, activeRect, setScreenActiveRectRepCode);
    connection->screenManagerAgent_ = nullptr;
    // teset SetScreenOffset
    connection->SetScreenOffset(INVALID_SCREEN_ID, 0, 0);
    // test SetScreenFrameGravity
    connection->SetScreenFrameGravity(INVALID_SCREEN_ID, 0);
    // test RegisterHgmConfigChangeCallback
    connection->RegisterHgmConfigChangeCallback(nullptr);
    // test RegisterHgmRefreshRateModeChangeCallback
    connection->RegisterHgmRefreshRateModeChangeCallback(nullptr);
    // test RegisterHgmRefreshRateUpdateCallback
    connection->RegisterHgmRefreshRateUpdateCallback(nullptr);
    // test RegisterFrameRateLinkerExpectedFpsUpdateCallback
    connection->RegisterFrameRateLinkerExpectedFpsUpdateCallback(0, nullptr);
    // test ShowWatermark
    std::shared_ptr<Media::PixelMap> watermarkImg = nullptr;
    connection->ShowWatermark(watermarkImg, false);
    connection->renderProcessManagerAgent_ = renderProcessManagerAgent;
    connection->ShowWatermark(watermarkImg, false);
    connection->renderProcessManagerAgent_ = nullptr;
    // test ResizeVirtualScreen
    connection->ResizeVirtualScreen(INVALID_SCREEN_ID, 0, 0);
    // test ReportJankStats
    connection->ReportJankStats();
    // test NotifyLightFactorStatus
    connection->NotifyLightFactorStatus(0);
    // test NotifyAppStrategyConfigChangeEvent
    const std::string pkgName = "";
    const std::vector<std::pair<std::string, std::string>> newConfig = {};
    connection->NotifyAppStrategyConfigChangeEvent(pkgName, 0, newConfig);
    // test NotifyRefreshRateEvent
    EventInfo eventInfo;
    connection->NotifyRefreshRateEvent(eventInfo);
    // tese SetWindowExpectedRefreshRate
    const std::unordered_map<uint64_t, EventInfo> eventInfos = {};
    connection->SetWindowExpectedRefreshRate(eventInfos);
    // test SetWindowExpectedRefreshRate
    std::unordered_map<std::string, EventInfo> refreshRateEventInfos = {};
    connection->SetWindowExpectedRefreshRate(refreshRateEventInfos);
    // test NotifySoftVsyncRateDiscountEvent
    connection->NotifySoftVsyncRateDiscountEvent(0, pkgName, 0);
    // test NotifyTouchEvent

    connection->vsyncManagerAgent_ = nullptr;
    connection->NotifyTouchEvent(0, 0, 0);
    connection->vsyncManagerAgent_ = vsyncManagerAgent;
    // test NotifyDynamicModeEvent
    connection->NotifyDynamicModeEvent(false);
    // test NotifyHgmConfigEvent
    connection->NotifyHgmConfigEvent(pkgName, false);
    // test NotifyXComponentExpectedFrameRate
    connection->NotifyXComponentExpectedFrameRate(pkgName, 0);
    // test ReportEventResponse and ReportEventComplete and ReportEventJankFrame
    DataBaseRs responseInfo;
    connection->ReportEventResponse(responseInfo);
    connection->ReportEventComplete(responseInfo);
    connection->ReportEventJankFrame(responseInfo);
    connection->renderProcessManagerAgent_ = renderProcessManagerAgent;
    connection->ReportEventResponse(responseInfo);
    connection->ReportEventComplete(responseInfo);
    connection->ReportEventJankFrame(responseInfo);
    connection->renderProcessManagerAgent_ = nullptr;
    // test ReportRsSceneJankStart and ReportRsSceneJankEnd
    AppInfo jankInfo;
    connection->ReportRsSceneJankStart(jankInfo);
    connection->ReportRsSceneJankEnd(jankInfo);
    connection->renderProcessManagerAgent_ = renderProcessManagerAgent;
    connection->ReportRsSceneJankStart(jankInfo);
    connection->ReportRsSceneJankEnd(jankInfo);
    connection->renderProcessManagerAgent_ = nullptr;
    // test ReportGameStateData
    GameStateData reportGameStateData;
    connection->ReportGameStateData(reportGameStateData);
    connection->renderProcessManagerAgent_ = renderProcessManagerAgent;
    connection->ReportGameStateData(reportGameStateData);
    connection->renderProcessManagerAgent_ = nullptr;
    // test SetCacheEnabledForRotation
    connection->SetCacheEnabledForRotation(false);
    // test GetActiveDirtyRegionInfo
    connection->GetActiveDirtyRegionInfo();
    // test GetHdrOnDuration and SetVmaCacheStatus and SetCurtainScreenUsingStatus
    connection->renderProcessManagerAgent_ = renderProcessManagerAgent;
    ASSERT_NE(connection->renderProcessManagerAgent_, nullptr);
    int64_t hdrOnDuration = 0;
    connection->GetHdrOnDuration(hdrOnDuration);
    connection->SetVmaCacheStatus(false);
    connection->SetCurtainScreenUsingStatus(false);
    connection->renderProcessManagerAgent_ = nullptr;
    // test SetGpuCrcDirtyEnabledPidList and
    std::vector<int32_t> pidList = {};
    connection->SetGpuCrcDirtyEnabledPidList(pidList);
    // test RegisterUIExtensionCallback
    connection->RegisterUIExtensionCallback(0, nullptr);
    connection->renderProcessManagerAgent_ = renderProcessManagerAgent;
    ASSERT_NE(connection->renderProcessManagerAgent_, nullptr);
    connection->RegisterUIExtensionCallback(0, nullptr);
    connection->renderProcessManagerAgent_ = nullptr;
    // test SetVirtualScreenStatus
    bool setVirtualScreenStatusRet = false;
    connection->SetVirtualScreenStatus(
        INVALID_SCREEN_ID, VirtualScreenStatus::VIRTUAL_SCREEN_INVALID_STATUS, setVirtualScreenStatusRet);
    // test SetLayerTop and SetForceRefresh
    connection->SetLayerTop(pkgName, false);
    connection->renderProcessManagerAgent_ = renderProcessManagerAgent;
    ASSERT_NE(connection->renderProcessManagerAgent_, nullptr);
    connection->SetLayerTop(pkgName, false);
    connection->SetForceRefresh(pkgName, false);
    connection->renderProcessManagerAgent_ = nullptr;
    // test SetFreeMultiWindowStatus and SetColorFollow
    connection->SetFreeMultiWindowStatus(false);
    connection->SetColorFollow(pkgName, false);
    connection->renderProcessManagerAgent_ = renderProcessManagerAgent;
    ASSERT_NE(connection->renderProcessManagerAgent_, nullptr);
    connection->SetFreeMultiWindowStatus(false);
    connection->SetColorFollow(pkgName, false);
    connection->renderProcessManagerAgent_ = nullptr;
    // test NotifyScreenSwitched
    connection->NotifyScreenSwitched();
    connection->screenManagerAgent_ = screenManagerAgent;
    ASSERT_NE(connection->screenManagerAgent_, nullptr);
    connection->NotifyScreenSwitched();
    connection->screenManagerAgent_ = nullptr;
    // test RegisterSelfDrawingNodeRectChangeCallback and UnRegisterSelfDrawingNodeRectChangeCallback
    RectConstraint constraint;
    connection->RegisterSelfDrawingNodeRectChangeCallback(constraint, nullptr);
    connection->UnRegisterSelfDrawingNodeRectChangeCallback();
    // test NotifyPageName
    connection->NotifyPageName(pkgName, pkgName, false);
    connection->hgmContext_ = hgmContext;
    ASSERT_NE(connection->hgmContext_, nullptr);
    connection->NotifyPageName(pkgName, pkgName, false);
    connection->hgmContext_ = nullptr;
    // test AvcodecVideoStart and AvcodecVideoStop and AvcodecVideoGet and AvcodecVideoGetRecent
    std::vector<uint64_t> uniqueIdList = {};
    std::vector<std::string> surfaceNameList = {};
    connection->AvcodecVideoStart(uniqueIdList, surfaceNameList, 0, 0);
    connection->AvcodecVideoStop(uniqueIdList, surfaceNameList, 0);
    connection->AvcodecVideoGet(0);
    connection->AvcodecVideoGetRecent();
    // test SetBehindWindowFilterEnabled and GetBehindWindowFilterEnabled
    connection->SetBehindWindowFilterEnabled(false);
    bool getBehindWindowFilterEnabledRet = false;
    connection->GetBehindWindowFilterEnabled(getBehindWindowFilterEnabledRet);
    connection->renderProcessManagerAgent_ = renderProcessManagerAgent;
    ASSERT_NE(connection->renderProcessManagerAgent_, nullptr);
    connection->SetBehindWindowFilterEnabled(false);
    connection->GetBehindWindowFilterEnabled(getBehindWindowFilterEnabledRet);
    connection->renderProcessManagerAgent_ = nullptr;
    // test GetPidGpuMemoryInMB
    float gpuMemInMB = 0.0;
    connection->GetPidGpuMemoryInMB(0, gpuMemInMB);

    // restore
    connection->renderProcessManagerAgent_ = renderProcessManagerAgent;
    connection->hgmContext_ = hgmContext;
    connection->screenManagerAgent_ = screenManagerAgent;
    connection->vsyncManagerAgent_ = vsyncManagerAgent;
    connection->renderServiceAgent_ = renderServiceAgent;
}
} // namespace OHOS::Rosen
