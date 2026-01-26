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
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "rs_render_service.h"
#include "transaction/rs_service_to_render_connection.h"
#include "transaction/zidl/rs_service_to_render_connection_stub.h"
#include "transaction/zidl/rs_iservice_to_render_connection_ipc_interface_code.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
class MockRSBrightnessInfoChangeCallback : public IRemoteProxy<RSIBrightnessInfoChangeCallback> {
public:
    explicit MockRSBrightnessInfoChangeCallback() : IRemoteProxy<RSIBrightnessInfoChangeCallback>(nullptr) {};
    virtual ~MockRSBrightnessInfoChangeCallback() noexcept = default;

    void OnBrightnessInfoChange(ScreenId screenId, const BrightnessInfo& brightnessInfo) override {}
};

RSRenderService renderService;
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
    auto runner = AppExecFwk::EventRunner::Create(false);
    auto handler = std::make_shared<AppExecFwk::EventHandler>(runner);
    auto renderPipeline = RSRenderPipeline::Create(handler, nullptr, nullptr, nullptr);
    OHOS::system::SetParameter("bootevent.samgr.ready", "false");
    RSUniRenderThread::Instance().uniRenderEngine_ = nullptr;
    sptr<RSRenderPipelineAgent> renderPipelineAgent = new RSRenderPipelineAgent(renderPipeline);
    g_connectionStub = sptr<RSServiceToRenderConnection>::MakeSptr(renderPipelineAgent);
}

void RSServiceToRenderConnectionStubTest::TearDownTestCase()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(110));
}

void RSServiceToRenderConnectionStubTest::SetUp() {}
void RSServiceToRenderConnectionStubTest::TearDown() {}

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

/**
 * @tc.name: SetGpuCrcDirtyEnabledPidList001
 * @tc.desc: Test SetGpuCrcDirtyEnabledPidList when data is invalid
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, SetGpuCrcDirtyEnabledPidList001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
        return;
    }
    data.WriteInt32(-1);
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::SET_GPU_CRC_DIRTY_ENABLED_PIDLIST);
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ret, ERR_INVALID_DATA);
}
 
/**
 * @tc.name: SetGpuCrcDirtyEnabledPidList002
 * @tc.desc: Test SetGpuCrcDirtyEnabledPidList when data is valid
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, SetGpuCrcDirtyEnabledPidList002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::SET_GPU_CRC_DIRTY_ENABLED_PIDLIST);
    std::vector<int32_t> pidList;
    data.WriteInt32Vector(pidList);
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ret, ERR_NONE);
}
 
/**
 * @tc.name: RegisterOcclusionChangeCallback001
 * @tc.desc: Test SetGpuCrcDirtyEnabledPidList when ReadInt32 fail
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, RegisterOcclusionChangeCallback001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::REGISTER_OCCLUSION_CHANGE_CALLBACK);
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ret, ERR_INVALID_DATA);
}
 
/**
 * @tc.name: RegisterOcclusionChangeCallback002
 * @tc.desc: Test RegisterOcclusionChangeCallback when ReadRemoteObject fail
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, RegisterOcclusionChangeCallback002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
        return;
    }
    data.WriteInt32(0);
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::REGISTER_OCCLUSION_CHANGE_CALLBACK);
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ret, ERR_NULL_OBJECT);
}
 
/**
 * @tc.name: RegisterOcclusionChangeCallback003
 * @tc.desc: Test RegisterOcclusionChangeCallback when data is valid
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, RegisterOcclusionChangeCallback003, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
        return;
    }
    data.WriteInt32(0);
    sptr<RSIOcclusionChangeCallbackMock> callback =
        new RSIOcclusionChangeCallbackMock();
    data.WriteRemoteObject(callback->AsObject());
    uint32_t code = static_cast<uint32_t>(
        RSIServiceToRenderConnectionInterfaceCode::REGISTER_OCCLUSION_CHANGE_CALLBACK);
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ret, ERR_NONE);
}
 
/**
 * @tc.name: RegisterSurfaceOcclusionChangeCallback001
 * @tc.desc: Test RegisterOcclusionChangeCallback when ReadUint64 fail
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, RegisterSurfaceOcclusionChangeCallback001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
        return;
    }
    uint32_t code = static_cast<uint32_t>(
        RSIServiceToRenderConnectionInterfaceCode::REGISTER_SURFACE_OCCLUSION_CHANGE_CALLBACK);
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ret, ERR_INVALID_DATA);
}
 
/**
 * @tc.name: RegisterSurfaceOcclusionChangeCallback002
 * @tc.desc: Test RegisterOcclusionChangeCallback when ReadInt32 fail
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, RegisterSurfaceOcclusionChangeCallback002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
        return;
    }
    data.WriteUint64(0);
    uint32_t code = static_cast<uint32_t>(
        RSIServiceToRenderConnectionInterfaceCode::REGISTER_SURFACE_OCCLUSION_CHANGE_CALLBACK);
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ret, ERR_INVALID_DATA);
}
 
/**
 * @tc.name: RegisterSurfaceOcclusionChangeCallback003
 * @tc.desc: Test RegisterOcclusionChangeCallback when ReadRemoteObject fail
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, RegisterSurfaceOcclusionChangeCallback003, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
        return;
    }
    data.WriteUint64(0);
    data.WriteInt32(0);
    uint32_t code = static_cast<uint32_t>(
        RSIServiceToRenderConnectionInterfaceCode::REGISTER_SURFACE_OCCLUSION_CHANGE_CALLBACK);
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ret, ERR_NULL_OBJECT);
}
 
/**
 * @tc.name: RegisterSurfaceOcclusionChangeCallback004
 * @tc.desc: Test RegisterOcclusionChangeCallback when data is valid
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, RegisterSurfaceOcclusionChangeCallback004, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
        return;
    }
    data.WriteUint64(0);
    data.WriteInt32(0);
    sptr<RSSurfaceOcclusionChangeCallbackStubMock> callback = new RSSurfaceOcclusionChangeCallbackStubMock();
    data.WriteRemoteObject(callback->AsObject());
    uint32_t code = static_cast<uint32_t>(
        RSIServiceToRenderConnectionInterfaceCode::REGISTER_SURFACE_OCCLUSION_CHANGE_CALLBACK);
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ret, ERR_NONE);
}
 
/**
 * @tc.name: UnRegisterSurfaceOcclusionChangeCallback001
 * @tc.desc: Test UnRegisterSurfaceOcclusionChangeCallback when ReadUint64 fail
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, UnRegisterSurfaceOcclusionChangeCallback001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
        return;
    }
    uint32_t code = static_cast<uint32_t>(
        RSIServiceToRenderConnectionInterfaceCode::UNREGISTER_SURFACE_OCCLUSION_CHANGE_CALLBACK);
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ret, ERR_INVALID_DATA);
}
 
/**
 * @tc.name: UnRegisterSurfaceOcclusionChangeCallback002
 * @tc.desc: Test UnRegisterSurfaceOcclusionChangeCallback when data is valid
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, UnRegisterSurfaceOcclusionChangeCallback002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor())) {
        return;
    }
    data.WriteUint64(0);
    uint32_t code = static_cast<uint32_t>(
        RSIServiceToRenderConnectionInterfaceCode::UNREGISTER_SURFACE_OCCLUSION_CHANGE_CALLBACK);
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ret, ERR_NONE);
}


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
} // namespace OHOS::Rosen
