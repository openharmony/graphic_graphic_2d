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

#include "binder_invoker.h"
#include "gtest/gtest.h"
#include "accesstoken_kit.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"
#include "limit_number.h"
#include "mock_hdi_device.h"
#include "sandbox_utils.h"
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
#include "ipc_callbacks/rs_transaction_data_callback_stub.h"
#include "rs_irender_service.h"
#include "screen_manager/screen_types.h"
#include "render_server/transaction/zidl/rs_client_to_service_connection_stub.h"
#include "screen_manager/rs_screen.h"
#include "feature/capture/rs_capture_pixelmap_manager.h"
#include "ipc_callbacks/surface_capture_callback_stub.h"
#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif
using namespace testing;
using namespace testing::ext;
using namespace OHOS::Security::AccessToken;

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
sptr<RSClientToServiceConnectionStub> RSClientToServiceConnectionStubTest::connectionStub_ =
    new RSClientToServiceConnection(
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
    // RSHardwareThread::Instance().Start();

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
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_SHARED_TYPEFACE)), ERR_INVALID_DATA);
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
 * @tc.name: TestRSClientToServiceConnectionStub031
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, TestRSClientToServiceConnectionStub031, TestSize.Level1)
{
    {
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;
        uint32_t code =
            static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_REFRESH_INFO_BY_PID_AND_UNIQUEID);
        int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
        ASSERT_EQ(res, ERR_INVALID_STATE);
    }

    {
        EXPECT_EQ(OnRemoteRequestTest(static_cast<uint32_t>(
                      RSIRenderServiceConnectionInterfaceCode::GET_REFRESH_INFO_BY_PID_AND_UNIQUEID)),
            ERR_INVALID_DATA);
    }

    {
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;
        data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
        data.WriteInt32(1000);
        uint32_t code =
            static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_REFRESH_INFO_BY_PID_AND_UNIQUEID);
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
            static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_REFRESH_INFO_BY_PID_AND_UNIQUEID);
        int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
        ASSERT_EQ(res, ERR_OK);
    }
}

/**
 * @tc.name: TestRSClientToServiceConnectionStub032
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, TestRSClientToServiceConnectionStub032, TestSize.Level1)
{
    pid_t newPid = 1002;
    NodeId nodeId = (static_cast<uint64_t>(newPid) << 32) | 0x00001031;
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
    RSSurfaceRenderNodeConfig config = {
        .id = nodeId, .name = "xcompentsurface", .nodeType = RSSurfaceNodeType::SELF_DRAWING_NODE
    };
    auto node = std::make_shared<RSSurfaceRenderNode>(config);
    nodeMap.surfaceNodeMap_[nodeId] = node;

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code =
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_REFRESH_INFO_BY_PID_AND_UNIQUEID);
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data.WriteInt32(newPid);
    data.WriteUint64(0);
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    nodeMap.surfaceNodeMap_.erase(nodeId);
    ASSERT_EQ(res, ERR_OK);
}

/**
 * @tc.name: TestRSClientToServiceConnectionStub033
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, TestRSClientToServiceConnectionStub033, TestSize.Level1)
{
    pid_t newPid = 1002;
    NodeId nodeId = (static_cast<uint64_t>(newPid) << 32) | 0x00001031;
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
    RSSurfaceRenderNodeConfig config = {
        .id = nodeId, .name = "xcompentsurface", .nodeType = RSSurfaceNodeType::SELF_DRAWING_NODE
    };
    auto node = std::make_shared<RSSurfaceRenderNode>(config);
    nodeMap.surfaceNodeMap_[nodeId] = node;

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code =
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_REFRESH_INFO_BY_PID_AND_UNIQUEID);
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data.WriteInt32(newPid);
    data.WriteUint64(1000);
    int res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    nodeMap.surfaceNodeMap_.erase(nodeId);
    ASSERT_EQ(res, ERR_OK);
}

/**
 * @tc.name: TestRSClientToServiceConnectionStub034
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, TestRSClientToServiceConnectionStub034, TestSize.Level1)
{
    pid_t newPid = 1003;
    NodeId nodeId = (static_cast<uint64_t>(newPid) << 32) | 0x00001031;
    sptr<RSClientToServiceConnectionStub> clientToServiceConnectionStub = new RSClientToServiceConnection(
        0, nullptr, mainThread_, CreateOrGetScreenManager(), token_->AsObject(), nullptr);
    ASSERT_NE(clientToServiceConnectionStub, nullptr);
    sptr<RSClientToServiceConnection> clientToServiceConnection =
        iface_cast<RSClientToServiceConnection>(clientToServiceConnectionStub);
    ASSERT_NE(clientToServiceConnection, nullptr);
    auto mainThread = clientToServiceConnection->mainThread_;
    clientToServiceConnection->mainThread_ = nullptr;
    std::string result = "";
    int res = clientToServiceConnection->GetRefreshInfoByPidAndUniqueId(newPid, 0, result);
    ASSERT_EQ(res, ERR_INVALID_VALUE);

    auto& nodeMap = mainThread->GetContext().GetMutableNodeMap();
    clientToServiceConnection->mainThread_ = mainThread;
    RSSurfaceRenderNodeConfig config = {
        .id = nodeId, .name = "xcompentsurface", .nodeType = RSSurfaceNodeType::SELF_DRAWING_NODE
    };
    auto node = std::make_shared<RSSurfaceRenderNode>(config);
    nodeMap.surfaceNodeMap_[nodeId] = node;
    auto originRenderType = RSUniRenderJudgement::uniRenderEnabledType_;
    RSUniRenderJudgement::uniRenderEnabledType_ = UniRenderEnabledType::UNI_RENDER_DISABLED;
    if (!RSUniRenderJudgement::IsUniRender()) {
        auto screenMgr = clientToServiceConnection->screenManager_;
        clientToServiceConnection->screenManager_ = nullptr;
        result = "";
        res = clientToServiceConnection->GetRefreshInfoByPidAndUniqueId(newPid, 0, result);
        ASSERT_EQ(res, ERR_OK);

        clientToServiceConnection->screenManager_ = screenMgr;
        result = "";
        res = clientToServiceConnection->GetRefreshInfoByPidAndUniqueId(newPid, 0, result);
        ASSERT_EQ(res, ERR_OK);
    }
    RSUniRenderJudgement::uniRenderEnabledType_ = originRenderType;
    nodeMap.surfaceNodeMap_.erase(nodeId);
}

/**
 * @tc.name: TestRSClientToServiceConnectionStub035
 * @tc.desc: Test Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, TestRSClientToServiceConnectionStub035, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    data.WriteInt32(1000);
    data.WriteUint64(0);
    uint32_t code =
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_REFRESH_INFO_BY_PID_AND_UNIQUEID);
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
 * @tc.name: OnRemoteRequest_CreateVirtualScreen_Test001
 * @tc.desc: OnRemoteRequest_CreateVirtualScreen_Test001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, OnRemoteRequest_CreateVirtualScreen_Test001, TestSize.Level1)
{
    MockNativeToken mockToken("foundation");
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_VIRTUAL_SCREEN);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    ASSERT_EQ(ERR_INVALID_DATA, connectionStub_->OnRemoteRequest(code, data, reply, option));
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_VIRTUAL_SCREEN);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_VIRTUAL_SCREEN);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_VIRTUAL_SCREEN);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_VIRTUAL_SCREEN);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_VIRTUAL_SCREEN);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_VIRTUAL_SCREEN);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_VIRTUAL_SCREEN);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_VIRTUAL_SCREEN);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_VIRTUAL_SCREEN);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_RESOLUTION);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    ASSERT_EQ(ERR_INVALID_DATA, connectionStub_->OnRemoteRequest(code, data, reply, option));
}

/**
 * @tc.name: OnRemoteRequest_SetVirtualScreenResolution_Test002
 * @tc.desc: OnRemoteRequest_SetVirtualScreenResolution_Test002
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, OnRemoteRequest_SetVirtualScreenResolution_Test002, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_RESOLUTION);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_RESOLUTION);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_RESOLUTION);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_BACK_LIGHT);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_BACK_LIGHT);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_METADATAKEYS);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_METADATAKEYS);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::RESIZE_VIRTUAL_SCREEN);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::RESIZE_VIRTUAL_SCREEN);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::RESIZE_VIRTUAL_SCREEN);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::RESIZE_VIRTUAL_SCREEN);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_PANEL_POWER_STATUS);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_PANEL_POWER_STATUS);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_PANEL_POWER_STATUS);
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

    sptr<RSClientToServiceConnectionStub> clientToServiceConnectionStub = new RSClientToServiceConnection(
        0, nullptr, mainThread_, CreateOrGetScreenManager(), token_->AsObject(), nullptr);
    ASSERT_NE(clientToServiceConnectionStub, nullptr);

    sptr<RSClientToServiceConnection> clientToServiceConnection =
        iface_cast<RSClientToServiceConnection>(clientToServiceConnectionStub);
    ASSERT_NE(clientToServiceConnection, nullptr);

    // empty screen manager
    PanelPowerStatus status = PanelPowerStatus::INVALID_PANEL_POWER_STATUS;
    auto screenMgr = clientToServiceConnection->screenManager_;
    clientToServiceConnection->screenManager_ = nullptr;
    int ret = clientToServiceConnection->GetPanelPowerStatus(SCREEN_ID, status);
    EXPECT_EQ(ret, ERR_INVALID_OPERATION);
}

/**
 * @tc.name: OnRemoteRequest_SetScreenSkipFrameInterval_Test001
 * @tc.desc: OnRemoteRequest_SetScreenSkipFrameInterval_Test001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTest, OnRemoteRequest_SetScreenSkipFrameInterval_Test001, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_SKIP_FRAME_INTERVAL);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_SKIP_FRAME_INTERVAL);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_SKIP_FRAME_INTERVAL);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::AVCODEC_VIDEO_GET);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::AVCODEC_VIDEO_GET_RECENT);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_ROG_SCREEN_RESOLUTION);
    
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
    sptr<RSClientToServiceConnectionStub> connectionStub = new RSClientToServiceConnection(
        0, nullptr, mainThread_, CreateOrGetScreenManager(), token_->AsObject(), nullptr);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SHOW_WATERMARK);
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
    sptr<RSClientToServiceConnectionStub> connectionStub =
        new RSClientToServiceConnection(0, nullptr, mainThread_, screenManager_, token_->AsObject(), nullptr);
    uint32_t interfaceCode = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_DUAL_SCREEN_STATE);

    // case 1: only write descriptor
    {
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;
        data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
        auto res = connectionStub->OnRemoteRequest(static_cast<uint32_t>(interfaceCode), data, reply, option);
        EXPECT_EQ(res, ERR_INVALID_DATA);
    }

    // case 2: only write descriptor and screen id
    {
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;
        data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
        data.WriteUint64(hdiOutput_->GetScreenId());
        auto res = connectionStub->OnRemoteRequest(static_cast<uint32_t>(interfaceCode), data, reply, option);
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
        auto res = connectionStub->OnRemoteRequest(static_cast<uint32_t>(interfaceCode), data, reply, option);
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
        auto res = connectionStub->OnRemoteRequest(static_cast<uint32_t>(interfaceCode), data, reply, option);
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
        auto res = connectionStub->OnRemoteRequest(static_cast<uint32_t>(interfaceCode), data, reply, option);
        EXPECT_EQ(res, ERR_OK);
    }

    // case 6: empty screen manager
    {
        ScreenId id = hdiOutput_->GetScreenId();
        sptr<RSClientToServiceConnection> connection = iface_cast<RSClientToServiceConnection>(connectionStub);
        ASSERT_NE(connection, nullptr);
        auto screenManager = connection->screenManager_;
        connection->screenManager_ = nullptr;
        int32_t res = connection->SetDualScreenState(id, DualScreenStatus::DUAL_SCREEN_ENTER);
        connection->screenManager_ = screenManager;
        EXPECT_EQ(res, StatusCode::SCREEN_MANAGER_NULL);
    }
}

/**
 * @tc.name: SetSystemAnimatedScenesTest001
 * @tc.desc: Test SetSystemAnimatedScenes when ReadBool and ReadUint32 fail
 * @tc.type: FUNC
 * @tc.require: issue20726
 */
HWTEST_F(RSClientToServiceConnectionStubTest, SetSystemAnimatedScenesTest001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code =
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SYSTEM_ANIMATED_SCENES);
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    int ret = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ret, ERR_INVALID_DATA);
}
 
/**
 * @tc.name: SetSystemAnimatedScenesTest002
 * @tc.desc: Test SetSystemAnimatedScenes when data is ReadBool fail
 * @tc.type: FUNC
 * @tc.require: issue20726
 */
HWTEST_F(RSClientToServiceConnectionStubTest, SetSystemAnimatedScenesTest002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code =
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SYSTEM_ANIMATED_SCENES);
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
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
HWTEST_F(RSClientToServiceConnectionStubTest, SetSystemAnimatedScenesTest003, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code =
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SYSTEM_ANIMATED_SCENES);
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
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
HWTEST_F(RSClientToServiceConnectionStubTest, SetSystemAnimatedScenesTest004, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code =
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SYSTEM_ANIMATED_SCENES);
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
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
HWTEST_F(RSClientToServiceConnectionStubTest, SetSystemAnimatedScenesTest005, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code =
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SYSTEM_ANIMATED_SCENES);
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
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
 * @tc.name: AddVirtualScreenWhiteList001
 * @tc.desc: Test AddVirtualScreenWhiteList
 * @tc.type: FUNC
 * @tc.require: issue21114
 */
HWTEST_F(RSClientToServiceConnectionStubTest, AddVirtualScreenWhiteList001, TestSize.Level2)
{
    ASSERT_NE(connectionStub_, nullptr);
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::ADD_VIRTUAL_SCREEN_WHITELIST);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REMOVE_VIRTUAL_SCREEN_WHITELIST);
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
 * @tc.name: SetVirtualScreenBlackList001
 * @tc.desc: Test SetVirtualScreenBlackList
 * @tc.type: FUNC
 * @tc.require: issue21114
 */
HWTEST_F(RSClientToServiceConnectionStubTest, SetVirtualScreenBlackList001, TestSize.Level2)
{
    ASSERT_NE(connectionStub_, nullptr);
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_BLACKLIST);
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
 * @tc.name: SetVirtualScreenTypeBlackList001
 * @tc.desc: Test SetVirtualScreenTypeBlackList001
 * @tc.type: FUNC
 * @tc.require: issue20886
 */
HWTEST_F(RSClientToServiceConnectionStubTest, SetVirtualScreenTypeBlackList001, TestSize.Level2)
{
    ASSERT_NE(connectionStub_, nullptr);
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_TYPE_BLACKLIST);
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
        RSIRenderServiceConnectionInterfaceCode::NOTIFY_APP_STRATEGY_CONFIG_CHANGE_EVENT);
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
        RSIRenderServiceConnectionInterfaceCode::NOTIFY_SOFT_VSYNC_RATE_DISCOUNT_EVENT);
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
        RSIRenderServiceConnectionInterfaceCode::NOTIFY_WINDOW_EXPECTED_BY_WINDOW_ID);
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
        RSIRenderServiceConnectionInterfaceCode::NOTIFY_WINDOW_EXPECTED_BY_WINDOW_ID);

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
        RSIRenderServiceConnectionInterfaceCode::NOTIFY_WINDOW_EXPECTED_BY_VSYNC_NAME);
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
        RSIRenderServiceConnectionInterfaceCode::NOTIFY_WINDOW_EXPECTED_BY_VSYNC_NAME);
    
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
        RSIRenderServiceConnectionInterfaceCode::SET_REFRESH_RATE_MODE);

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
        RSIRenderServiceConnectionInterfaceCode::SYNC_FRAME_RATE_RANGE);
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
        RSIRenderServiceConnectionInterfaceCode::SYNC_FRAME_RATE_RANGE);

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
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_TOUCH_EVENT);

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
    sptr<RSClientToServiceConnectionStub> connectionStub =
        new RSClientToServiceConnection(0, nullptr, mainThread_, screenManager_, token_->AsObject(), nullptr);
    uint32_t interfaceCode = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_SHARED_TYPEFACE);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());

    Drawing::SharedTypeface sharedTypeface;
    sharedTypeface.fd_ = open("/system/fonts/HarmonyOS_Sans_SC.ttf", O_RDONLY);
    EXPECT_EQ(RSMarshallingHelper::Marshalling(data, sharedTypeface), true);

    auto res = connectionStub->OnRemoteRequest(static_cast<uint32_t>(interfaceCode), data, reply, option);
    ASSERT_EQ(res, ERR_OK);
}
} // namespace OHOS::Rosen
