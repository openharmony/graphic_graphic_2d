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

namespace OHOS::Rosen {

class RSClientToServiceConnectionStubTestExt : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline Mock::HdiDeviceMock* hdiDeviceMock_;
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

uint32_t RSClientToServiceConnectionStubTestExt::screenId_ = 0;
sptr<RSIConnectionToken> RSClientToServiceConnectionStubTestExt::token_ = new IRemoteStub<RSIConnectionToken>();
sptr<RSClientToServiceConnectionStub> RSClientToServiceConnectionStubTestExt::connectionStub_ =
    new RSClientToServiceConnection(
        0, nullptr, RSMainThread::Instance(), CreateOrGetScreenManager(), token_->AsObject(), nullptr);
std::shared_ptr<RSSurfaceRenderNode> RSClientToServiceConnectionStubTestExt::surfaceNode_ =
    std::shared_ptr<RSSurfaceRenderNode>(new RSSurfaceRenderNode(10003, std::make_shared<RSContext>(), true),
    RSRenderNodeGC::NodeDestructor);

void RSClientToServiceConnectionStubTestExt::SetUpTestCase()
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
    mainThread_ = RSMainThread::Instance();
    mainThread_->runner_ = OHOS::AppExecFwk::EventRunner::Create(true);
    mainThread_->handler_ = std::make_shared<OHOS::AppExecFwk::EventHandler>(mainThread_->runner_);
    mainThread_->runner_->Run();
}

void RSClientToServiceConnectionStubTestExt::TearDownTestCase()
{
    mainThread_->runner_->Stop();
    hdiOutput_ = nullptr;
    screenManager_ = nullptr;
    hdiDeviceMock_ = nullptr;
    token_ = nullptr;
    connectionStub_ = nullptr;
}

void RSClientToServiceConnectionStubTestExt::SetUp() {}
void RSClientToServiceConnectionStubTestExt::TearDown() {}

/**
 * @tc.name: SetSurfaceWatermarkSub001
 * @tc.desc: Test SetSurfaceWatermarkSub001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTestExt, SetSurfaceWatermarkSub001, TestSize.Level1)
{
#if defined(RS_ENABLE_UNI_RENDER)
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
#endif
}

/**
 * @tc.name: ClearSurfaceWatermarkForNodesStub001
 * @tc.desc: Test ClearSurfaceWatermarkForNodesStub001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientToServiceConnectionStubTestExt, ClearSurfaceWatermarkForNodesStub001, TestSize.Level1)
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
HWTEST_F(RSClientToServiceConnectionStubTestExt, ClearSurfaceWatermarkStub001, TestSize.Level1)
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
}