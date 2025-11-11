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
#include "ipc_callbacks/rs_frame_rate_linker_expected_fps_update_callback_stub.h"
#include "ipc_callbacks/rs_iframe_rate_linker_expected_fps_update_callback_ipc_interface_code.h"
#include "ipc_callbacks/rs_iframe_rate_linker_expected_fps_update_callback.h"
#include "ipc_callbacks/screen_change_callback_stub.h"
#include "ipc_callbacks/brightness_info_change_callback_stub.h"
#include "pipeline/render_thread/rs_composer_adapter.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/main_thread/rs_client_to_service_connection.h"
#include "pipeline/rs_logical_display_render_node.h"
#include "pipeline/rs_render_node_gc.h"
#include "pipeline/rs_screen_render_node.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "ipc_callbacks/rs_transaction_data_callback_stub.h"
#include "rs_irender_service.h"
#include "screen_manager/screen_types.h"
#include "transaction/rs_client_to_service_connection_stub.h"
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
    const int INVALID_PIDLIST_SIZE = 200;
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
    static sptr<RSClientToServiceConnectionStub> connectionStub_;
};

uint32_t RSClientToServiceConnectionStubTest::screenId_ = 0;
sptr<RSIConnectionToken> RSClientToServiceConnectionStubTest::token_ = new IRemoteStub<RSIConnectionToken>();
sptr<RSClientToServiceConnectionStub> RSClientToServiceConnectionStubTest::connectionStub_ = new RSClientToServiceConnection(
    0, nullptr, RSMainThread::Instance(), CreateOrGetScreenManager(), token_->AsObject(), nullptr);
std::shared_ptr<RSSurfaceRenderNode> RSClientToServiceConnectionStubTest::surfaceNode_ =
    std::shared_ptr<RSSurfaceRenderNode>(new RSSurfaceRenderNode(10003, std::make_shared<RSContext>(), true),
    RSRenderNodeGC::NodeDestructor);

void RSClientToServiceConnectionStubTest::SetUpTestCase()
{
#ifdef RS_ENABLE_VK
    RsVulkanContext::SetRecyclable(false);
#endif
    hdiOutput_ = HdiOutput::CreateHdiOutput(screenId_);
    auto rsScreen = std::make_shared<impl::RSScreen>(screenId_, true, hdiOutput_, nullptr);
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

void RSClientToServiceConnectionStubTest::TearDownTestCase()
{
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
    RSHardwareThread::Instance().Start();

    auto mainThread = RSMainThread::Instance();
    if (!mainThread) {
        return;
    }
    mainThread->runner_ = AppExecFwk::EventRunner::Create("RSClientToServiceConnectionStubTest");
    if (!mainThread->runner_) {
        return;
    }
    mainThread->handler_ = std::make_shared<AppExecFwk::EventHandler>(mainThread->runner_);
    mainThread->runner_->Run();
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

class RSScreenChangeCallbackStubMock : public RSScreenChangeCallbackStub {
public:
    RSScreenChangeCallbackStubMock() = default;
    virtual ~RSScreenChangeCallbackStubMock() = default;
    void OnScreenChanged(ScreenId id, ScreenEvent event,
        ScreenChangeReason reason) override {};
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
 * @tc.name: TestRSRenderServiceConnectionStub001
 * @tc.desc: Test if data has no content.
 * @tc.type: FUNC
 * @tc.require: issueI60KUK
 */
HWTEST_F(RSClientToServiceConnectionStubTest, TestRSRenderServiceConnectionStub001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    int res;
    uint32_t code;

    code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::COMMIT_TRANSACTION);
    res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);
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
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_DEFAULT_SCREEN_ID)), ERR_NONE);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_ACTIVE_SCREEN_ID)), ERR_NONE);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_ALL_SCREEN_IDS)), ERR_NONE);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_CHANGE_CALLBACK)), ERR_NULL_OBJECT);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_ACTIVE_MODE)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_REFRESH_RATE)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_CURRENT_REFRESH_RATE)),
        ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_REFRESH_RATES)),
        ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_POWER_STATUS)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_BACK_LIGHT)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_ACTIVE_MODE)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_CORRECTION)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_TYPE)), ERR_INVALID_DATA);
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
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_SURFACE)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_BLACKLIST)),
        ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(static_cast<uint32_t>(
                  RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_SECURITY_EXEMPTION_LIST)),
        ERR_INVALID_STATE);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_CAST_SCREEN_ENABLE_SKIP_WINDOW)),
        ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REMOVE_VIRTUAL_SCREEN)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::DISABLE_RENDER_CONTROL_SCREEN)),
        ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_VIRTUAL_SCREEN_RESOLUTION)),
        ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_MIRROR_SCREEN_CANVAS_ROTATION)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_AUTO_ROTATION)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_MIRROR_SCREEN_SCALE_MODE)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_USING_STATUS)),
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
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_REFRESH_RATE_MODE)), ERR_INVALID_DATA);
    EXPECT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_CURRENT_REFRESH_RATE_MODE)),
        ERR_NONE);
    EXPECT_EQ(OnRemoteRequestTest(static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::GET_SHOW_REFRESH_RATE_ENABLED)), ERR_NONE);
    EXPECT_EQ(OnRemoteRequestTest(static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::SET_SHOW_REFRESH_RATE_ENABLED)), ERR_INVALID_DATA);
    EXPECT_EQ(OnRemoteRequestTest(static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::GET_REALTIME_REFRESH_RATE)), ERR_INVALID_DATA);
#ifdef RS_ENABLE_UNI_RENDER
    EXPECT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_REFRESH_INFO)), ERR_INVALID_DATA);
#else
    EXPECT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_REFRESH_INFO)), ERR_INVALID_DATA);
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
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_GLOBAL_DARK_COLOR_MODE)), ERR_INVALID_DATA);
    EXPECT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NEED_REGISTER_TYPEFACE)), ERR_INVALID_DATA);
    EXPECT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_REFRESH_RATE)),
        ERR_INVALID_DATA);
    EXPECT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_WATERMARK)), ERR_INVALID_DATA);
    EXPECT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_HIDE_PRIVACY_CONTENT)), ERR_INVALID_DATA);
    EXPECT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_LIGHT_FACTOR_STATUS)), ERR_INVALID_DATA);
    EXPECT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VMA_CACHE_STATUS)), ERR_INVALID_DATA);
    EXPECT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_STATUS)), ERR_INVALID_DATA);
    EXPECT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_DISPLAY_NODE)), ERR_INVALID_DATA);
    EXPECT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_LAYER_TOP)), ERR_INVALID_STATE);
    EXPECT_EQ(OnRemoteRequestTest(
                  static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::ADD_VIRTUAL_SCREEN_BLACKLIST)),
        ERR_INVALID_DATA);
    EXPECT_EQ(OnRemoteRequestTest(
                  static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REMOVE_VIRTUAL_SCREEN_BLACKLIST)),
        ERR_INVALID_DATA);
    EXPECT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_FORCE_REFRESH)), ERR_INVALID_STATE);
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
        RSIRenderServiceConnectionInterfaceCode::REGISTER_OCCLUSION_CHANGE_CALLBACK)), ERR_NULL_OBJECT);
    // Unable to access IPC due to lack of permissions.
    EXPECT_EQ(OnRemoteRequestTest(static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::REGISTER_SURFACE_OCCLUSION_CHANGE_CALLBACK)), ERR_INVALID_DATA);
    // Unable to access IPC due to lack of permissions.
    EXPECT_EQ(OnRemoteRequestTest(static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::UNREGISTER_SURFACE_OCCLUSION_CHANGE_CALLBACK)), ERR_INVALID_DATA);
    EXPECT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_HGM_CFG_CALLBACK)), ERR_NULL_OBJECT);
    EXPECT_EQ(OnRemoteRequestTest(static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::REFRESH_RATE_MODE_CHANGE_CALLBACK)), ERR_NULL_OBJECT);
    EXPECT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REFRESH_RATE_UPDATE_CALLBACK)),
        ERR_INVALID_DATA);
    EXPECT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::ON_FIRST_FRAME_COMMIT)),
        ERR_INVALID_DATA);
    EXPECT_EQ(OnRemoteRequestTest(static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::REGISTER_UIEXTENSION_CALLBACK)), ERR_INVALID_DATA);
    EXPECT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_APPLICATION_AGENT)), ERR_NULL_OBJECT);
    EXPECT_EQ(OnRemoteRequestTest(static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::SET_BUFFER_AVAILABLE_LISTENER)), ERR_INVALID_DATA);
    EXPECT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_BUFFER_CLEAR_LISTENER)), ERR_INVALID_DATA);
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
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_UNI_RENDER_ENABLED)), ERR_NONE);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_NODE)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_NODE_AND_SURFACE)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::MARK_POWER_OFF_NEED_PROCESS_ONE_FRAME)), ERR_NONE);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_VSYNC_CONNECTION)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::CREATE_PIXEL_MAP_FROM_SURFACE)), ERR_NULL_OBJECT);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_APP_WINDOW_NUM)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SYSTEM_ANIMATED_SCENES)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::EXECUTE_SYNCHRONOUS_TASK)), ERR_INVALID_STATE);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_TOUCH_EVENT)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_HARDWARE_ENABLED)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::SET_ROTATION_CACHE_ENABLED)), IPC_STUB_INVALID_DATA_ERR);
#ifdef TP_FEATURE_ENABLE
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_TP_FEATURE_CONFIG)), ERR_INVALID_STATE);
#endif
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_TYPEFACE)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::UNREGISTER_TYPEFACE)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_CURTAIN_SCREEN_USING_STATUS)),
        ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_ACTIVE_DIRTY_REGION_INFO)), ERR_NONE);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_GLOBAL_DIRTY_REGION_INFO)), ERR_NONE);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_LAYER_COMPOSE_INFO)), ERR_NONE);
    ASSERT_EQ(OnRemoteRequestTest(static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::GET_HARDWARE_COMPOSE_DISABLED_REASON_INFO)), ERR_NONE);
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
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_MEMORY_GRAPHIC)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_MEMORY_GRAPHICS)), ERR_INVALID_REPLY);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_TOTAL_APP_MEM_SIZE)), ERR_NONE);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_JANK_STATS)), ERR_NONE);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_EVENT_RESPONSE)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_EVENT_COMPLETE)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_EVENT_JANK_FRAME)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_EVENT_GAMESTATE)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
                  static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::UNREGISTER_FRAME_RATE_LINKER)),
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
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_FREE_MULTI_WINDOW_STATUS)),
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
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_BACK_LIGHT)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_DATA)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_GAMUTS)),
        ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_METADATAKEYS)),
        ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_GAMUT)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_GAMUT)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_GAMUT_MAP)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_GAMUT_MAP)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_PIXEL_FORMAT)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_PIXEL_FORMAT)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_HDR_FORMATS)),
        ERR_INVALID_DATA);
    if (RSUniRenderJudgement::IsUniRender()) {
        ASSERT_EQ(OnRemoteRequestTest(static_cast<uint32_t>(
                      RSIRenderServiceConnectionInterfaceCode::REGISTER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK)),
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
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_HDR_FORMAT)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_HDR_FORMAT)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_COLORSPACES)),
        ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_COLORSPACE)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_COLORSPACE)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_SKIP_FRAME_INTERVAL)),
        ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::RESIZE_VIRTUAL_SCREEN)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_PACKAGE_EVENT)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_APP_STRATEGY_CONFIG_CHANGE_EVENT)),
        ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_REFRESH_RATE_EVENT)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_WINDOW_EXPECTED_BY_WINDOW_ID)),
        ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_WINDOW_EXPECTED_BY_VSYNC_NAME)),
        ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_SOFT_VSYNC_EVENT)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_PIXELMAP_BY_PROCESSID)), ERR_NONE);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_SOFT_VSYNC_RATE_DISCOUNT_EVENT)),
        ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_XCOMPONENT_EXPECTED_FRAMERATE)),
        ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::REGISTER_FRAME_RATE_LINKER_EXPECTED_FPS_CALLBACK)),
        ERR_INVALID_DATA);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_SCREEN_SWITCHED);
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, NO_ERROR);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::DISABLE_RENDER_CONTROL_SCREEN);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::DISABLE_RENDER_CONTROL_SCREEN);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::ADD_VIRTUAL_SCREEN_BLACKLIST);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REMOVE_VIRTUAL_SCREEN_BLACKLIST);
    data.WriteUint64(1);
    data.WriteUInt64Vector({1});
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, NO_ERROR);
}

/**
 * @tc.name: TestRSRenderServiceConnectionStub018
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSClientToServiceConnectionStubTest, TestRSRenderServiceConnectionStub018, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_CHANGE_CALLBACK);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_ACTIVE_MODE);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_REFRESH_RATE);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::UNREGISTER_FRAME_RATE_LINKER);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::UNREGISTER_FRAME_RATE_LINKER);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_CURRENT_REFRESH_RATE);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_REFRESH_RATES);
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
        RSIRenderServiceConnectionInterfaceCode::REGISTER_FRAME_RATE_LINKER_EXPECTED_FPS_CALLBACK);
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
        RSIRenderServiceConnectionInterfaceCode::REGISTER_FRAME_RATE_LINKER_EXPECTED_FPS_CALLBACK);
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
        RSIRenderServiceConnectionInterfaceCode::REGISTER_FRAME_RATE_LINKER_EXPECTED_FPS_CALLBACK);
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
        RSIRenderServiceConnectionInterfaceCode::REGISTER_FRAME_RATE_LINKER_EXPECTED_FPS_CALLBACK);
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
        RSIRenderServiceConnectionInterfaceCode::NOTIFY_XCOMPONENT_EXPECTED_FRAMERATE);
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
        RSIRenderServiceConnectionInterfaceCode::NOTIFY_XCOMPONENT_EXPECTED_FRAMERATE);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_FORCE_REFRESH);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_WINDOW_EXPECTED_BY_WINDOW_ID);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_WINDOW_EXPECTED_BY_WINDOW_ID);
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
        RSIRenderServiceConnectionInterfaceCode::NOTIFY_WINDOW_EXPECTED_BY_VSYNC_NAME);
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
        RSIRenderServiceConnectionInterfaceCode::NOTIFY_WINDOW_EXPECTED_BY_VSYNC_NAME);
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
        RSIRenderServiceConnectionInterfaceCode::GET_PID_GPU_MEMORY_IN_MB);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::AVCODEC_VIDEO_START);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::AVCODEC_VIDEO_START);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::AVCODEC_VIDEO_START);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::AVCODEC_VIDEO_START);

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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::AVCODEC_VIDEO_START);

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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::AVCODEC_VIDEO_STOP);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::AVCODEC_VIDEO_STOP);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::AVCODEC_VIDEO_STOP);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::AVCODEC_VIDEO_STOP);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::PROFILER_SERVICE_OPEN_FILE);

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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::PROFILER_SERVICE_POPULATE_FILES);

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
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_POWER_STATUS);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_POWER_STATUS);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_ROG_SCREEN_RESOLUTION);
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
 * @tc.name: GetRogScreenResolutionTest001
 * @tc.desc: Test GetRogScreenResolution
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, GetRogScreenResolutionTest001, TestSize.Level1)
{
    constexpr uint64_t SCREEN_ID = 0;
    int32_t status{0};
    uint32_t width{0};
    uint32_t height{0};
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_ROG_SCREEN_RESOLUTION);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data.WriteUint64(SCREEN_ID);

    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_OK);
    ASSERT_EQ(reply.ReadInt32(status), static_cast<bool>(true));
    ASSERT_EQ(reply.ReadUint32(width), static_cast<bool>(true));
    ASSERT_EQ(reply.ReadUint32(height), static_cast<bool>(true));
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_GAMUT_MAP);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_GAMUT_MAP);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_GAMUT_MAP);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_GAMUT_MAP);
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
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, SetBrightnessInfoChangeCallbackTest, TestSize.Level2)
{
    sptr<RSClientToServiceConnectionStub> connectionStub =
        new RSClientToServiceConnection(0, nullptr, mainThread_, CreateOrGetScreenManager(), token_->AsObject(), nullptr);
    ASSERT_NE(connectionStub, nullptr);

    // case 1: no data
    {
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;
        data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
        auto interfaceCode = RSIRenderServiceConnectionInterfaceCode::SET_BRIGHTNESS_INFO_CHANGE_CALLBACK;
        auto res = connectionStub->OnRemoteRequest(static_cast<uint32_t>(interfaceCode), data, reply, option);
        EXPECT_EQ(res, ERR_INVALID_DATA);
    }

    // case 2: readRemoteObject false
    {
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;
        data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
        data.WriteBool(false);
        auto interfaceCode = RSIRenderServiceConnectionInterfaceCode::SET_BRIGHTNESS_INFO_CHANGE_CALLBACK;
        auto res = connectionStub->OnRemoteRequest(static_cast<uint32_t>(interfaceCode), data, reply, option);
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
        auto interfaceCode = RSIRenderServiceConnectionInterfaceCode::SET_BRIGHTNESS_INFO_CHANGE_CALLBACK;
        auto res = connectionStub->OnRemoteRequest(static_cast<uint32_t>(interfaceCode), data, reply, option);
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
        auto interfaceCode = RSIRenderServiceConnectionInterfaceCode::SET_BRIGHTNESS_INFO_CHANGE_CALLBACK;
        auto res = connectionStub->OnRemoteRequest(static_cast<uint32_t>(interfaceCode), data, reply, option);
        EXPECT_EQ(res, ERR_NONE);
    }
}

/**
 * @tc.name: GetBrightnessInfoTest
 * @tc.desc: Test GetBrightnessInfo
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, GetBrightnessInfoTest, TestSize.Level2)
{
    sptr<RSClientToServiceConnectionStub> connectionStub =
        new RSClientToServiceConnection(0, nullptr, mainThread_, screenManager_, token_->AsObject(), nullptr);

    // case 1: no data
    {
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;
        data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
        auto interfaceCode = RSIRenderServiceConnectionInterfaceCode::GET_BRIGHTNESS_INFO;
        auto res = connectionStub->OnRemoteRequest(static_cast<uint32_t>(interfaceCode), data, reply, option);
        EXPECT_EQ(res, ERR_INVALID_DATA);
    }

    // case 2: valid screenId
    {
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;
        data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
        data.WriteUint64(hdiOutput_->GetScreenId());
        auto interfaceCode = RSIRenderServiceConnectionInterfaceCode::GET_BRIGHTNESS_INFO;
        auto res = connectionStub->OnRemoteRequest(static_cast<uint32_t>(interfaceCode), data, reply, option);
        EXPECT_EQ(res, ERR_NONE);
    }

    // case 3: invalid screenId
    {
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;
        data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
        data.WriteUint64(INVALID_SCREEN_ID);
        auto interfaceCode = RSIRenderServiceConnectionInterfaceCode::GET_BRIGHTNESS_INFO;
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
HWTEST_F(RSClientToServiceConnectionStubTest, WriteBrightnessInfoTest, TestSize.Level2)
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_CORRECTION);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_CORRECTION);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_CORRECTION);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_CORRECTION);
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
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_GPU_CRC_DIRTY_ENABLED_PIDLIST);
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
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_GPU_CRC_DIRTY_ENABLED_PIDLIST);
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
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_GPU_CRC_DIRTY_ENABLED_PIDLIST);
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
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_GPU_CRC_DIRTY_ENABLED_PIDLIST);
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    std::vector<int32_t> pidList;
    data.WriteInt32Vector(pidList);
    sptr<RSClientToServiceConnection> connection = iface_cast<RSClientToServiceConnection>(connectionStub_);
    ASSERT_NE(connection, nullptr);
    auto mainThread = connection->mainThread_;
    connection->mainThread_ = nullptr;
    int ret = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ret, ERR_NONE);

    connection->mainThread_ = mainThread;
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
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_GPU_CRC_DIRTY_ENABLED_PIDLIST);
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    std::vector<int32_t> pidList(INVALID_PIDLIST_SIZE, 0);
    data.WriteInt32Vector(pidList);
    sptr<RSClientToServiceConnection> connection = iface_cast<RSClientToServiceConnection>(connectionStub_);
    ASSERT_NE(connection, nullptr);
    auto mainThread = connection->mainThread_;
    connection->mainThread_ = nullptr;
    int ret = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ret, ERR_NONE);

    connection->mainThread_ = mainThread;
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
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_OPTIMIZE_CANVAS_DIRTY_ENABLED_PIDLIST);
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
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_OPTIMIZE_CANVAS_DIRTY_ENABLED_PIDLIST);
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
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_OPTIMIZE_CANVAS_DIRTY_ENABLED_PIDLIST);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_STATUS);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_STATUS);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_STATUS);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_STATUS);
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data.WriteUint64(0);
    data.WriteUint32(static_cast<uint32_t>(VirtualScreenStatus::VIRTUAL_SCREEN_INVALID_STATUS));
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_OK);
}

/**
 * @tc.name: SetSurfaceWatermarkSub001
 * @tc.desc: Test SetSurfaceWatermarkSub001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, SetSurfaceWatermarkSub001, TestSize.Level1)
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SURFACE_WATERMARK);
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    MessageParcel data0;
    data0.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data0.WriteInt32(2);
    res = connectionStub_->OnRemoteRequest(code, data0, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    MessageParcel data20;
    data20.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data20.WriteInt32(2);
    data20.WriteString(name);
    res = connectionStub_->OnRemoteRequest(code, data20, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    MessageParcel data2;
    data2.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data2.WriteInt32(2);
    data2.WriteString(name);
    data2.WriteBool(false);
    res = connectionStub_->OnRemoteRequest(code, data2, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    MessageParcel data3;
    data3.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data3.WriteInt32(2);
    data3.WriteString(name);
    data3.WriteBool(true);
    data3.WriteParcelable(pixelmap.get());
    res = connectionStub_->OnRemoteRequest(code, data3, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    MessageParcel data4;
    data4.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data4.WriteInt32(2);
    data4.WriteString(name);
    data4.WriteBool(true);
    data4.WriteParcelable(pixelmap.get());
    data4.WriteUInt64Vector(nodeList);
    res = connectionStub_->OnRemoteRequest(code, data4, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    MessageParcel data5;
    data5.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
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
HWTEST_F(RSClientToServiceConnectionStubTest, ClearSurfaceWatermarkForNodesStub001, TestSize.Level1)
{
    ASSERT_NE(connectionStub_, nullptr);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    std::string name = "watermark";
    std::vector<NodeId> nodeList;
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CLEAR_SURFACE_WATERMARK_FOR_NODES);
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    MessageParcel data2;
    data2.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data2.WriteString(name);
    res = connectionStub_->OnRemoteRequest(code, data2, reply, option);

    MessageParcel data3;
    data3.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
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
HWTEST_F(RSClientToServiceConnectionStubTest, ClearSurfaceWatermarkStub001, TestSize.Level1)
{
    ASSERT_NE(connectionStub_, nullptr);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    std::string name = "watermark";
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CLEAR_SURFACE_WATERMARK_FOR_NODES);
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    MessageParcel data2;
    data2.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data2.WriteString(name);
    res = connectionStub_->OnRemoteRequest(code, data2, reply, option);
}

} // namespace OHOS::Rosen
