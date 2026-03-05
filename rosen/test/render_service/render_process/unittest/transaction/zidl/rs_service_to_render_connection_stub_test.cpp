/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <hilog/log.h>
#include <iostream>
#include <iservice_registry.h>
#include <message_parcel.h>
#include <securec.h>
#include <system_ability_definition.h>
#include <unistd.h>

#include "ipc_callbacks/rs_occlusion_change_callback_stub.h"
#include "ipc_callbacks/rs_surface_occlusion_change_callback_stub.h"
#include "parameters.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/render_thread/rs_render_engine.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "rs_render_service.h"
#include "transaction/rs_service_to_render_connection.h"
#include "transaction/zidl/rs_service_to_render_connection_stub.h"
#include "transaction/zidl/rs_iservice_to_render_connection_ipc_interface_code.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
constexpr const size_t PARCEL_MAX_CAPACITY = 2000 * 1024;

class MockRSBrightnessInfoChangeCallback : public IRemoteProxy<RSIBrightnessInfoChangeCallback> {
public:
    MockRSBrightnessInfoChangeCallback() : IRemoteProxy<RSIBrightnessInfoChangeCallback>(nullptr) {}
    virtual ~MockRSBrightnessInfoChangeCallback() noexcept = default;

    void OnBrightnessInfoChange(ScreenId screenId, const BrightnessInfo& brightnessInfo) override {}
};

sptr<RSServiceToRenderConnectionStub> g_connectionStub = nullptr;
}

class RSServiceToRenderConnectionStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

class RSIOcclusionChangeCallbackMock : public RSOcclusionChangeCallbackStub {
public:
    RSIOcclusionChangeCallbackMock() = default;
    virtual ~RSIOcclusionChangeCallbackMock() = default;
    void OnOcclusionVisibleChanged(std::shared_ptr<RSOcclusionData> occlusionData) override {};
};

class RSSurfaceOcclusionChangeCallbackStubMock : public RSSurfaceOcclusionChangeCallbackStub {
public:
    RSSurfaceOcclusionChangeCallbackStubMock() = default;
    virtual ~RSSurfaceOcclusionChangeCallbackStubMock() = default;
    void OnSurfaceOcclusionVisibleChanged(float visibleAreaRatio) override {};
};

void RSServiceToRenderConnectionStubTest::SetUpTestCase()
{
    auto renderPipeline = std::make_shared<RSRenderPipeline>();
    renderPipeline->imageEnhanceManager_ = std::make_shared<ImageEnhanceManager>();

    auto runner1 = AppExecFwk::EventRunner::Create(true);
    auto handler1 = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner1);
    auto mainThread = RSMainThread::Instance();
    renderPipeline->mainThread_ = mainThread;
    renderPipeline->mainThread_->handler_ = handler1;

    renderPipeline->uniRenderThread_ = &(RSUniRenderThread::Instance());
    auto runner2 = AppExecFwk::EventRunner::Create(true);
    renderPipeline->uniRenderThread_->runner_ = runner2;
    renderPipeline->uniRenderThread_->handler_ = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner2);

    auto renderPipelineAgent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    renderPipeline->uniRenderThread_->uniRenderEngine_ = std::make_shared<OHOS::Rosen::RSRenderEngine>();
    renderPipeline->uniRenderThread_->uniRenderEngine_->renderContext_ = OHOS::Rosen::RenderContext::Create();
    g_connectionStub = sptr<RSServiceToRenderConnection>::MakeSptr(renderPipelineAgent);
}

void RSServiceToRenderConnectionStubTest::TearDownTestCase()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(110));
}

void RSServiceToRenderConnectionStubTest::SetUp() {}
void RSServiceToRenderConnectionStubTest::TearDown() {}

static void SetLeftSize(Parcel& parcel, uint32_t leftSize)
{
    parcel.SetMaxCapacity(PARCEL_MAX_CAPACITY);
    size_t useSize = PARCEL_MAX_CAPACITY - leftSize;
    size_t writeInt32Count = useSize / 4;
    size_t writeBoolCount = useSize % 4;

    for (size_t i = 0; i < writeInt32Count; i++) {
        parcel.WriteInt32(0);
    }

    for (size_t j = 0; j < writeBoolCount; j++) {
        parcel.WriteBoolUnaligned(false);
    }
}

/**
 * @tc.name: TestRSServiceToRenderConnectionStub001
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, TestRSServiceToRenderConnectionStub001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(
        RSIServiceToRenderConnectionInterfaceCode::GET_REALTIME_REFRESH_RATE);
    uint64_t screenId = 1;
    data.WriteUint64(screenId);
    g_connectionStub->OnRemoteRequest(code, data, reply, option);
    ASSERT_TRUE(g_connectionStub);
}

/**
 * @tc.name: TestRSServiceToRenderConnectionStub002
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, TestRSServiceToRenderConnectionStub002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(
        RSIServiceToRenderConnectionInterfaceCode::SET_SHOW_REFRESH_RATE_ENABLED);
    bool enabled = true;
    int32_t type = 1;
    data.WriteBool(enabled);
    data.WriteInt32(type);
    g_connectionStub->OnRemoteRequest(code, data, reply, option);
    ASSERT_TRUE(g_connectionStub);
}

/**
 * @tc.name: TestRSServiceToRenderConnectionStub003
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, TestRSServiceToRenderConnectionStub003, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(
        RSIServiceToRenderConnectionInterfaceCode::GET_SHOW_REFRESH_RATE_ENABLED);
    g_connectionStub->OnRemoteRequest(code, data, reply, option);
    ASSERT_TRUE(g_connectionStub);
}

// /**
//  * @tc.name: SetGpuCrcDirtyEnabledPidList001
//  * @tc.desc: Test SetGpuCrcDirtyEnabledPidList when data is invalid
//  * @tc.type: FUNC
//  * @tc.require: issueIBRN69
//  */
// HWTEST_F(RSServiceToRenderConnectionStubTest, SetGpuCrcDirtyEnabledPidList001, TestSize.Level1)
// {
//     MessageParcel data;
//     MessageParcel reply;
//     MessageOption option;
//     option.SetFlags(MessageOption::TF_ASYNC);
//     if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
//         return;
//     }
//     data.WriteInt32(-1);
//     uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::SET_GPU_CRC_DIRTY_ENABLED_PIDLIST);
//     auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
//     ASSERT_EQ(ret, ERR_INVALID_DATA);
// }

// /**
//  * @tc.name: SetGpuCrcDirtyEnabledPidList002
//  * @tc.desc: Test SetGpuCrcDirtyEnabledPidList when data is valid
//  * @tc.type: FUNC
//  * @tc.require: issueIBRN69
//  */
// HWTEST_F(RSServiceToRenderConnectionStubTest, SetGpuCrcDirtyEnabledPidList002, TestSize.Level1)
// {
//     MessageParcel data;
//     MessageParcel reply;
//     MessageOption option;
//     option.SetFlags(MessageOption::TF_ASYNC);
//     if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
//         return;
//     }
//     uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::SET_GPU_CRC_DIRTY_ENABLED_PIDLIST);
//     std::vector<int32_t> pidList;
//     data.WriteInt32Vector(pidList);
//     auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
//     ASSERT_EQ(ret, ERR_NONE);
// }

/**
 * @tc.name: HgmForceUpdateTaskTest
 * @tc.desc: Test HgmForceUpdateTask
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, HgmForceUpdateTaskTest, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::HGM_FORCE_UPDATE_TASK);
    MessageParcel reply;
    MessageOption option;

    MessageParcel data;
    data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor());
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_DATA);

    MessageParcel data2;
    data2.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor());
    data2.WriteBool(true);
    ret = g_connectionStub->OnRemoteRequest(code, data2, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_DATA);

    MessageParcel data3;
    data3.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor());
    data3.WriteBool(true);
    data3.WriteString("test");
    ret = g_connectionStub->OnRemoteRequest(code, data3, reply, option);
    EXPECT_EQ(ret, ERR_NONE);
}

/**
 * @tc.name: GetShowRefreshRateEnabled001
 * @tc.desc: Test GetShowRefreshRateEnabled
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, GetShowRefreshRateEnabled001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::GET_SHOW_REFRESH_RATE_ENABLED);
    g_connectionStub->OnRemoteRequest(code, data, reply, option);
}

/**
 * @tc.name: GetShowRefreshRateEnabled002
 * @tc.desc: Test GetShowRefreshRateEnabled when reply.WriteBool fails.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, GetShowRefreshRateEnabled002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::GET_SHOW_REFRESH_RATE_ENABLED);
    // Set reply capacity to 0 to make WriteBool fail
    SetLeftSize(reply, 0);
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_REPLY);
}

/**
 * @tc.name: SetShowRefreshRateEnabled001
 * @tc.desc: Test SetShowRefreshRateEnabled when ReadBool fails
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, SetShowRefreshRateEnabled001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::SET_SHOW_REFRESH_RATE_ENABLED);
    // Not writing enabled or type, causing ReadBool to fail
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name: SetShowRefreshRateEnabled002
 * @tc.desc: Test SetShowRefreshRateEnabled when ReadInt32 fails
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, SetShowRefreshRateEnabled002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::SET_SHOW_REFRESH_RATE_ENABLED);
    bool enabled = true;
    data.WriteBool(enabled);
    // Not writing type, causing ReadInt32 to fail
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name: SetShowRefreshRateEnabled003
 * @tc.desc: Test SetShowRefreshRateEnabled with enabled=true, type=0
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, SetShowRefreshRateEnabled003, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::SET_SHOW_REFRESH_RATE_ENABLED);
    bool enabled = true;
    int32_t type = 0;
    data.WriteBool(enabled);
    data.WriteInt32(type);
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_NONE);
}

/**
 * @tc.name: GetRealtimeRefreshRate001
 * @tc.desc: Test GetRealtimeRefreshRate when ReadUint64 fails
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, GetRealtimeRefreshRate001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::GET_REALTIME_REFRESH_RATE);
    // Not writing screenId, causing ReadUint64 to fail
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name: GetRealtimeRefreshRate002
 * @tc.desc: Test GetRealtimeRefreshRate when reply.WriteUint32 fails.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, GetRealtimeRefreshRate002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::GET_REALTIME_REFRESH_RATE);
    uint64_t screenId = 100;
    data.WriteUint64(screenId);
    // Set reply capacity to 0 to make WriteUint32 fail
    SetLeftSize(reply, 0);
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_REPLY);
}

/**
 * @tc.name: GetRealtimeRefreshRate003
 * @tc.desc: Test GetRealtimeRefreshRate with valid screenId
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, GetRealtimeRefreshRate003, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::GET_REALTIME_REFRESH_RATE);
    uint64_t screenId = 100;
    data.WriteUint64(screenId);
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_NONE);

    // Verify the response contains a valid refresh rate
    uint32_t refreshRate = 0;
    EXPECT_TRUE(reply.ReadUint32(refreshRate));
}

/**
 * @tc.name: SetBrightnessInfoChangeCallbackTest
 * @tc.desc: Test SetBrightnessInfoChangeCallback
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, SetBrightnessInfoChangeCallbackTest, TestSize.Level2)
{
    // case 1: no data
    {
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;
        data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
        auto interfaceCode = RSIServiceToRenderConnectionInterfaceCode::SET_BRIGHTNESS_INFO_CHANGE_CALLBACK;
        auto res = g_connectionStub->OnRemoteRequest(static_cast<uint32_t>(interfaceCode), data, reply, option);
        EXPECT_EQ(res, ERR_INVALID_STATE);
    }

    // case 2: remoteObject null
    {
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;
        data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
        data.WriteInt32(getpid());
        data.WriteRemoteObject(nullptr);
        auto interfaceCode = RSIServiceToRenderConnectionInterfaceCode::SET_BRIGHTNESS_INFO_CHANGE_CALLBACK;
        auto res = g_connectionStub->OnRemoteRequest(static_cast<uint32_t>(interfaceCode), data, reply, option);
        EXPECT_EQ(res, ERR_INVALID_STATE);
    }

    // case 3: remoteObject not null
    {
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;
        data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
        data.WriteInt32(getpid());
        MockRSBrightnessInfoChangeCallback callback;
        data.WriteRemoteObject(callback.AsObject());
        auto interfaceCode = RSIServiceToRenderConnectionInterfaceCode::SET_BRIGHTNESS_INFO_CHANGE_CALLBACK;
        auto res = g_connectionStub->OnRemoteRequest(static_cast<uint32_t>(interfaceCode), data, reply, option);
        EXPECT_EQ(res, ERR_INVALID_STATE);
    }
}

/**
 * @tc.name: GetBehindWindowFilterEnabled001
 * @tc.desc: Test GetBehindWindowFilterEnabled
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, GetBehindWindowFilterEnabled001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::GET_BEHIND_WINDOW_FILTER_ENABLED);
    g_connectionStub->OnRemoteRequest(code, data, reply, option);
}

/**
 * @tc.name: GetBehindWindowFilterEnabled002
 * @tc.desc: Test GetBehindWindowFilterEnabled when reply.WriteBool fails.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, GetBehindWindowFilterEnabled002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::GET_BEHIND_WINDOW_FILTER_ENABLED);
    // Set reply capacity to 0 to make WriteBool fail
    SetLeftSize(reply, 0);
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_REPLY);
}

/**
 * @tc.name: SetBehindWindowFilterEnabled001
 * @tc.desc: Test SetBehindWindowFilterEnabled when ReadBool fails
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, SetBehindWindowFilterEnabled001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::SET_BEHIND_WINDOW_FILTER_ENABLED);
    // Not writing enabled or type, causing ReadBool to fail
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name: SetBehindWindowFilterEnabled002
 * @tc.desc: Test SetBehindWindowFilterEnabled when ReadInt32 fails
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, SetBehindWindowFilterEnabled002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::SET_BEHIND_WINDOW_FILTER_ENABLED);
    bool enabled = true;
    data.WriteBool(enabled);
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_NONE);
}
} // namespace OHOS::Rosen
