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
constexpr const int WAIT_HANDLER_TIME = 1; // 1S
constexpr const int WAIT_HANDLER_TIME_COUNT = 5;

class MockRSBrightnessInfoChangeCallback : public IRemoteProxy<RSIBrightnessInfoChangeCallback> {
public:
    MockRSBrightnessInfoChangeCallback() : IRemoteProxy<RSIBrightnessInfoChangeCallback>(nullptr) {}
    virtual ~MockRSBrightnessInfoChangeCallback() noexcept = default;

    void OnBrightnessInfoChange(ScreenId screenId, const BrightnessInfo& brightnessInfo) override {}
};

std::shared_ptr<RSRenderPipeline> renderPipeline = nullptr;
sptr<RSServiceToRenderConnectionStub> g_connectionStub = nullptr;
}

class RSServiceToRenderConnectionStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static void WaitHandlerTask();
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
    renderPipeline = std::make_shared<RSRenderPipeline>();

    auto runner1 = AppExecFwk::EventRunner::Create(true);
    auto handler1 = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner1);
    auto mainThread = RSMainThread::Instance();
    renderPipeline->mainThread_ = mainThread;
    renderPipeline->mainThread_->handler_ = handler1;
    runner1->Run();

    renderPipeline->uniRenderThread_ = &(RSUniRenderThread::Instance());
    auto runner2 = AppExecFwk::EventRunner::Create(true);
    renderPipeline->uniRenderThread_->runner_ = runner2;
    renderPipeline->uniRenderThread_->handler_ = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner2);
    runner2->Run();

    auto renderPipelineAgent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    renderPipeline->uniRenderThread_->uniRenderEngine_ = std::make_shared<OHOS::Rosen::RSRenderEngine>();
    renderPipeline->uniRenderThread_->uniRenderEngine_->renderContext_ = OHOS::Rosen::RenderContext::Create();
    g_connectionStub = sptr<RSServiceToRenderConnection>::MakeSptr(renderPipelineAgent);
}

void RSServiceToRenderConnectionStubTest::TearDownTestCase()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(110));

    WaitHandlerTask();
    renderPipeline->mainThread_->handler_->eventRunner_->Stop();
    renderPipeline->uniRenderThread_->runner_->Stop();

    renderPipeline->mainThread_->handler_ = nullptr;
    renderPipeline->mainThread_->receiver_ = nullptr;
    renderPipeline->mainThread_->renderEngine_ = nullptr;

    renderPipeline->uniRenderThread_->handler_ = nullptr;
    renderPipeline->uniRenderThread_->runner_ = nullptr;
    renderPipeline->uniRenderThread_->uniRenderEngine_->renderContext_ = nullptr;
    renderPipeline->uniRenderThread_->uniRenderEngine_ = nullptr;
    renderPipeline->uniRenderThread_ = nullptr;
    renderPipeline = nullptr;

    g_connectionStub = nullptr;
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

void RSServiceToRenderConnectionStubTest::WaitHandlerTask()
{
    auto count = 0;
    auto isMainThreadRunning = !renderPipeline->mainThread_->handler_->IsIdle();
    auto isUniRenderThreadRunning = !renderPipeline->uniRenderThread_->handler_->IsIdle();
    while (count < WAIT_HANDLER_TIME_COUNT && (isMainThreadRunning || isUniRenderThreadRunning)) {
        std::this_thread::sleep_for(std::chrono::seconds(WAIT_HANDLER_TIME));
    }
    if (count >= WAIT_HANDLER_TIME_COUNT) {
        renderPipeline->mainThread_->handler_->RemoveAllEvents();
        renderPipeline->uniRenderThread_->handler_->RemoveAllEvents();
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
 * @tc.name: ReportJankStats001
 * @tc.desc: Test ReportJankStats interface code
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, ReportJankStats001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::REPORT_JANK_STATS);
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_NONE);
}

/**
 * @tc.name: ReportEventResponse001
 * @tc.desc: Test ReportEventResponse when ReadDataBaseRs fails
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, ReportEventResponse001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::REPORT_EVENT_RESPONSE);
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name: ReportEventResponse002
 * @tc.desc: Test ReportEventResponse with valid data
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, ReportEventResponse002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::REPORT_EVENT_RESPONSE);
    data.WriteInt32(100);
    data.WriteInt32(1);
    data.WriteInt32(1);
    data.WriteInt64(1000);
    data.WriteInt64(2000);
    data.WriteInt64(3000);
    data.WriteBool(false);
    data.WriteString("scene1");
    data.WriteString("1.0");
    data.WriteString("bundle");
    data.WriteString("process");
    data.WriteString("ability");
    data.WriteString("page");
    data.WriteString("source");
    data.WriteString("note");
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_NONE);
}

/**
 * @tc.name: ReportEventComplete001
 * @tc.desc: Test ReportEventComplete when ReadDataBaseRs fails
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, ReportEventComplete001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::REPORT_EVENT_COMPLETE);
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name: ReportEventComplete002
 * @tc.desc: Test ReportEventComplete with valid data
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, ReportEventComplete002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::REPORT_EVENT_COMPLETE);
    data.WriteInt32(100);
    data.WriteInt32(1);
    data.WriteInt32(1);
    data.WriteInt64(1000);
    data.WriteInt64(2000);
    data.WriteInt64(3000);
    data.WriteBool(false);
    data.WriteString("scene1");
    data.WriteString("1.0");
    data.WriteString("bundle");
    data.WriteString("process");
    data.WriteString("ability");
    data.WriteString("page");
    data.WriteString("source");
    data.WriteString("note");
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_NONE);
}

/**
 * @tc.name: ReportEventJankFrame001
 * @tc.desc: Test ReportEventJankFrame when ReadDataBaseRs fails
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, ReportEventJankFrame001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::REPORT_EVENT_JANK_FRAME);
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name: ReportEventJankFrame002
 * @tc.desc: Test ReportEventJankFrame with valid data
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, ReportEventJankFrame002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::REPORT_EVENT_JANK_FRAME);
    data.WriteInt32(100);
    data.WriteInt32(1);
    data.WriteInt32(1);
    data.WriteInt64(1000);
    data.WriteInt64(2000);
    data.WriteInt64(3000);
    data.WriteBool(false);
    data.WriteString("scene1");
    data.WriteString("1.0");
    data.WriteString("bundle");
    data.WriteString("process");
    data.WriteString("ability");
    data.WriteString("page");
    data.WriteString("source");
    data.WriteString("note");
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_NONE);
}

/**
 * @tc.name: ReportRsSceneJankStart001
 * @tc.desc: Test ReportRsSceneJankStart when ReadAppInfo fails
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, ReportRsSceneJankStart001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::REPORT_RS_SCENE_JANK_START);
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name: ReportRsSceneJankStart002
 * @tc.desc: Test ReportRsSceneJankStart with valid data
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, ReportRsSceneJankStart002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::REPORT_RS_SCENE_JANK_START);
    data.WriteInt64(1000);
    data.WriteInt64(2000);
    data.WriteInt32(100);
    data.WriteString("1.0");
    data.WriteInt32(1);
    data.WriteString("bundle");
    data.WriteString("process");
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_NONE);
}

/**
 * @tc.name: ReportRsSceneJankEnd001
 * @tc.desc: Test ReportRsSceneJankEnd when ReadAppInfo fails
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, ReportRsSceneJankEnd001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::REPORT_RS_SCENE_JANK_END);
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name: ReportRsSceneJankEnd002
 * @tc.desc: Test ReportRsSceneJankEnd with valid data
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, ReportRsSceneJankEnd002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::REPORT_RS_SCENE_JANK_END);
    data.WriteInt64(1000);
    data.WriteInt64(2000);
    data.WriteInt32(100);
    data.WriteString("1.0");
    data.WriteInt32(1);
    data.WriteString("bundle");
    data.WriteString("process");
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_NONE);
}

/**
 * @tc.name: AvcodecVideoStart001
 * @tc.desc: Test AvcodecVideoStart when reading data fails
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, AvcodecVideoStart001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::AVCODEC_VIDEO_START);
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name: AvcodecVideoStart002
 * @tc.desc: Test AvcodecVideoStart when reply.WriteInt32 fails
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, AvcodecVideoStart002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::AVCODEC_VIDEO_START);
    std::vector<uint64_t> uniqueIdList = {1000, 2000};
    std::vector<std::string> surfaceNameList = {"surface1", "surface2"};
    uint32_t fps = 60;
    uint64_t reportTime = 1000;
    data.WriteUInt64Vector(uniqueIdList);
    data.WriteStringVector(surfaceNameList);
    data.WriteUint32(fps);
    data.WriteUint64(reportTime);
    SetLeftSize(reply, 0);
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_REPLY);
}

/**
 * @tc.name: AvcodecVideoStart003
 * @tc.desc: Test AvcodecVideoStart with valid data
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, AvcodecVideoStart003, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::AVCODEC_VIDEO_START);
    std::vector<uint64_t> uniqueIdList = {1000, 2000};
    std::vector<std::string> surfaceNameList = {"surface1", "surface2"};
    uint32_t fps = 60;
    uint64_t reportTime = 1000;
    data.WriteUInt64Vector(uniqueIdList);
    data.WriteStringVector(surfaceNameList);
    data.WriteUint32(fps);
    data.WriteUint64(reportTime);
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_NONE);
}

/**
 * @tc.name: AvcodecVideoStop001
 * @tc.desc: Test AvcodecVideoStop when reading data fails
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, AvcodecVideoStop001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::AVCODEC_VIDEO_STOP);
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name: AvcodecVideoStop002
 * @tc.desc: Test AvcodecVideoStop when reply.WriteInt32 fails
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, AvcodecVideoStop002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::AVCODEC_VIDEO_STOP);
    std::vector<uint64_t> uniqueIdList = {1000, 2000};
    std::vector<std::string> surfaceNameList = {"surface1", "surface2"};
    uint32_t fps = 60;
    data.WriteUInt64Vector(uniqueIdList);
    data.WriteStringVector(surfaceNameList);
    data.WriteUint32(fps);
    SetLeftSize(reply, 0);
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_REPLY);
}

/**
 * @tc.name: AvcodecVideoStop003
 * @tc.desc: Test AvcodecVideoStop with valid data
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, AvcodecVideoStop003, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::AVCODEC_VIDEO_STOP);
    std::vector<uint64_t> uniqueIdList = {1000, 2000};
    std::vector<std::string> surfaceNameList = {"surface1", "surface2"};
    uint32_t fps = 60;
    data.WriteUInt64Vector(uniqueIdList);
    data.WriteStringVector(surfaceNameList);
    data.WriteUint32(fps);
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_NONE);
}

/**
 * @tc.name: AvcodecVideoGet001
 * @tc.desc: Test AvcodecVideoGet when reading data fails
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, AvcodecVideoGet001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::AVCODEC_VIDEO_GET);
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name: AvcodecVideoGet002
 * @tc.desc: Test AvcodecVideoGet when reply.WriteInt32 fails
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, AvcodecVideoGet002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::AVCODEC_VIDEO_GET);
    uint64_t uniqueId = 1000;
    data.WriteUint64(uniqueId);
    SetLeftSize(reply, 0);
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_REPLY);
}

/**
 * @tc.name: AvcodecVideoGet003
 * @tc.desc: Test AvcodecVideoGet with valid data
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, AvcodecVideoGet003, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::AVCODEC_VIDEO_GET);
    uint64_t uniqueId = 1000;
    data.WriteUint64(uniqueId);
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_NONE);
}

/**
 * @tc.name: AvcodecVideoGetRecent001
 * @tc.desc: Test AvcodecVideoGetRecent when reply.WriteInt32 fails
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, AvcodecVideoGetRecent001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::AVCODEC_VIDEO_GET_RECENT);
    SetLeftSize(reply, 0);
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_REPLY);
}

/**
 * @tc.name: AvcodecVideoGetRecent002
 * @tc.desc: Test AvcodecVideoGetRecent with valid data
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, AvcodecVideoGetRecent002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::AVCODEC_VIDEO_GET_RECENT);
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_NONE);
}

/**
 * @tc.name: GetMemoryGraphics001
 * @tc.desc: Test GetMemoryGraphics when reply.WriteUint64 fails
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, GetMemoryGraphics001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::GET_MEMORY_GRAPHICS);
    SetLeftSize(reply, 0);
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_REPLY);
}

/**
 * @tc.name: GetMemoryGraphics002
 * @tc.desc: Test GetMemoryGraphics with valid data
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, GetMemoryGraphics002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::GET_MEMORY_GRAPHICS);
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_NONE);
}

/**
 * @tc.name: GetTotalAppMemSize001
 * @tc.desc: Test GetTotalAppMemSize when reply.WriteFloat fails
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, GetTotalAppMemSize001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::GET_TOTAL_APP_MEM_SIZE);
    SetLeftSize(reply, 0);
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_REPLY);
}

/**
 * @tc.name: GetTotalAppMemSize002
 * @tc.desc: Test GetTotalAppMemSize with valid data
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, GetTotalAppMemSize002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::GET_TOTAL_APP_MEM_SIZE);
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_NONE);
}

/**
 * @tc.name: GetMemoryGraphic001
 * @tc.desc: Test GetMemoryGraphic when reading pid fails
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, GetMemoryGraphic001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::GET_MEMORY_GRAPHIC);
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name: GetMemoryGraphic002
 * @tc.desc: Test GetMemoryGraphic when reply.WriteParcelable fails
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, GetMemoryGraphic002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::GET_MEMORY_GRAPHIC);
    int32_t pid = 100;
    data.WriteInt32(pid);
    SetLeftSize(reply, 0);
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_REPLY);
}

/**
 * @tc.name: GetMemoryGraphic003
 * @tc.desc: Test GetMemoryGraphic with valid data
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, GetMemoryGraphic003, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::GET_MEMORY_GRAPHIC);
    int32_t pid = 100;
    data.WriteInt32(pid);
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_NONE);
}

/**
 * @tc.name: GetPixelMapByProcessId001
 * @tc.desc: Test GetPixelMapByProcessId when ReadUint64 fails
 * @tc.type: FUNC
 * @tc.require: issueI8V6MD
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, GetPixelMapByProcessId001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::GET_PIXELMAP_BY_PROCESSID);
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name: GetPixelMapByProcessId002
 * @tc.desc: Test GetPixelMapByProcessId when reply.WriteInt32 fails
 * @tc.type: FUNC
 * @tc.require: issueI8V6MD
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, GetPixelMapByProcessId002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::GET_PIXELMAP_BY_PROCESSID);
    uint64_t pid = 12345;
    data.WriteUint64(pid);
    SetLeftSize(reply, 0);
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_REPLY);
}

/**
 * @tc.name: GetPixelMapByProcessId003
 * @tc.desc: Test GetPixelMapByProcessId with valid pid
 * @tc.type: FUNC
 * @tc.require: issueI8V6MD
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, GetPixelMapByProcessId003, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::GET_PIXELMAP_BY_PROCESSID);
    uint64_t pid = 12345;
    data.WriteUint64(pid);
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_REPLY);
}

/**
 * @tc.name: ShowWatermark001
 * @tc.desc: Test ShowWatermark when ReadBool fails
 * @tc.type: FUNC
 * @tc.require: issueI8V6MD
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, ShowWatermark001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::SHOW_WATERMARK);
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name: ShowWatermark002
 * @tc.desc: Test ShowWatermark with watermarkImg nullptr
 * @tc.type: FUNC
 * @tc.require: issueI8V6MD
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, ShowWatermark002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::SHOW_WATERMARK);
    data.WriteParcelable(nullptr);
    bool isShow = true;
    data.WriteBool(isShow);
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name: ShowWatermark003
 * @tc.desc: Test ShowWatermark with valid PixelMap and isShow=true
 * @tc.type: FUNC
 * @tc.require: issueI8V6MD
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, ShowWatermark003, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::SHOW_WATERMARK);
    
    Media::InitializationOptions opts;
    opts.size.width = 100;
    opts.size.height = 100;
    auto pixelMap = Media::PixelMap::Create(opts);
    ASSERT_NE(pixelMap, nullptr);
    data.WriteParcelable(pixelMap.get());
    bool isShow = true;
    data.WriteBool(isShow);
    
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_NONE);
}

/**
 * @tc.name: ShowWatermark004
 * @tc.desc: Test ShowWatermark with valid PixelMap and isShow=false
 * @tc.type: FUNC
 * @tc.require: issueI8V6MD
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, ShowWatermark004, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::SHOW_WATERMARK);
    
    Media::InitializationOptions opts;
    opts.size.width = 100;
    opts.size.height = 100;
    auto pixelMap = Media::PixelMap::Create(opts);
    ASSERT_NE(pixelMap, nullptr);
    data.WriteParcelable(pixelMap.get());
    bool isShow = false;
    data.WriteBool(isShow);
    
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_NONE);
}

/**
 * @tc.name: GetSurfaceRootNode001
 * @tc.desc: Test GetSurfaceRootNode when ReadUint64 fails
 * @tc.type: FUNC
 * @tc.require: issueI8V6MD
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, GetSurfaceRootNode001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::GET_SURFACE_ROOT_NODE);
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name: GetSurfaceRootNode002
 * @tc.desc: Test GetSurfaceRootNode when reply.WriteUint64 fails
 * @tc.type: FUNC
 * @tc.require: issueI8V6MD
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, GetSurfaceRootNode002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::GET_SURFACE_ROOT_NODE);
    uint64_t windowNodeId = 12345;
    data.WriteUint64(windowNodeId);
    SetLeftSize(reply, 0);
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_REPLY);
}

/**
 * @tc.name: GetSurfaceRootNode003
 * @tc.desc: Test GetSurfaceRootNode with valid windowNodeId
 * @tc.type: FUNC
 * @tc.require: issueI8V6MD
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, GetSurfaceRootNode003, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::GET_SURFACE_ROOT_NODE);
    uint64_t windowNodeId = 12345;
    data.WriteUint64(windowNodeId);
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_NONE);
}

/**
 * @tc.name: SetWatermark001
 * @tc.desc: Test SetWatermark when watermark feature disabled
 * @tc.type: FUNC
 * @tc.require: issueI8V6MD
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, SetWatermark001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::SET_WATERMARK);
    pid_t callingPid = 12345;
    std::string name = "test_watermark";
    data.WriteInt32(callingPid);
    data.WriteString(name);
    
    Media::InitializationOptions opts;
    opts.size.width = 100;
    opts.size.height = 100;
    auto pixelMap = Media::PixelMap::Create(opts);
    ASSERT_NE(pixelMap, nullptr);
    data.WriteParcelable(pixelMap.get());
    
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_NONE);
}

/**
 * @tc.name: SetWatermark002
 * @tc.desc: Test SetWatermark when ReadInt32 fails
 * @tc.type: FUNC
 * @tc.require: issueI8V6MD
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, SetWatermark002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::SET_WATERMARK);
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: SetWatermark003
 * @tc.desc: Test SetWatermark when ReadString fails
 * @tc.type: FUNC
 * @tc.require: issueI8V6MD
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, SetWatermark003, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::SET_WATERMARK);
    pid_t callingPid = 12345;
    data.WriteInt32(callingPid);
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name: SetWatermark004
 * @tc.desc: Test SetWatermark when watermark is nullptr
 * @tc.type: FUNC
 * @tc.require: issueI8V6MD
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, SetWatermark004, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::SET_WATERMARK);
    pid_t callingPid = 12345;
    std::string name = "test_watermark";
    data.WriteInt32(callingPid);
    data.WriteString(name);
    data.WriteParcelable(nullptr);
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_NULL_OBJECT);
}

/**
 * @tc.name: RegisterSelfDrawingNodeRectChangeCallback001
 * @tc.desc: Test RegisterSelfDrawingNodeRectChangeCallback when ReadInt32(remotePid) fails
 * @tc.type: FUNC
 * @tc.require: issueI8V6MD
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, RegisterSelfDrawingNodeRectChangeCallback001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(
        RSIServiceToRenderConnectionInterfaceCode::REGISTER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK);
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_REPLY);
}

/**
 * @tc.name: RegisterSelfDrawingNodeRectChangeCallback002
 * @tc.desc: Test RegisterSelfDrawingNodeRectChangeCallback when ReadUint32(size) fails
 * @tc.type: FUNC
 * @tc.require: issueI8V6MD
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, RegisterSelfDrawingNodeRectChangeCallback002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(
        RSIServiceToRenderConnectionInterfaceCode::REGISTER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK);
    pid_t remotePid = 12345;
    data.WriteInt32(remotePid);
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_REPLY);
}

/**
 * @tc.name: RegisterSelfDrawingNodeRectChangeCallback003
 * @tc.desc: Test RegisterSelfDrawingNodeRectChangeCallback when size exceeds MAX_PID_SIZE_NUMBER
 * @tc.type: FUNC
 * @tc.require: issueI8V6MD
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, RegisterSelfDrawingNodeRectChangeCallback003, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(
        RSIServiceToRenderConnectionInterfaceCode::REGISTER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK);
    pid_t remotePid = 12345;
    data.WriteInt32(remotePid);
    uint32_t size = 200000;
    data.WriteUint32(size);
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_NONE);
}

/**
 * @tc.name: RegisterSelfDrawingNodeRectChangeCallback004
 * @tc.desc: Test RegisterSelfDrawingNodeRectChangeCallback when ReadInt32(pid) fails in loop
 * @tc.type: FUNC
 * @tc.require: issueI8V6MD
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, RegisterSelfDrawingNodeRectChangeCallback004, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(
        RSIServiceToRenderConnectionInterfaceCode::REGISTER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK);
    pid_t remotePid = 12345;
    data.WriteInt32(remotePid);
    uint32_t size = 1;
    data.WriteUint32(size);
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_REPLY);
}

/**
 * @tc.name: RegisterSelfDrawingNodeRectChangeCallback005
 * @tc.desc: Test RegisterSelfDrawingNodeRectChangeCallback when ReadInt32(constraint) fails
 * @tc.type: FUNC
 * @tc.require: issueI8V6MD
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, RegisterSelfDrawingNodeRectChangeCallback005, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(
        RSIServiceToRenderConnectionInterfaceCode::REGISTER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK);
    pid_t remotePid = 12345;
    data.WriteInt32(remotePid);
    uint32_t size = 0;
    data.WriteUint32(size);
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_REPLY);
}

/**
 * @tc.name: RegisterSelfDrawingNodeRectChangeCallback006
 * @tc.desc: Test RegisterSelfDrawingNodeRectChangeCallback when remoteObject is nullptr
 * @tc.type: FUNC
 * @tc.require: issueI8V6MD
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, RegisterSelfDrawingNodeRectChangeCallback006, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(
        RSIServiceToRenderConnectionInterfaceCode::REGISTER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK);
    pid_t remotePid = 12345;
    data.WriteInt32(remotePid);
    uint32_t size = 0;
    data.WriteUint32(size);
    data.WriteInt32(0);
    data.WriteInt32(0);
    data.WriteInt32(1000);
    data.WriteInt32(1000);
    data.WriteRemoteObject(nullptr);
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_NULL_OBJECT);
}

/**
 * @tc.name: UnRegisterSelfDrawingNodeRectChangeCallback001
 * @tc.desc: Test UnRegisterSelfDrawingNodeRectChangeCallback when ReadInt32 fails
 * @tc.type: FUNC
 * @tc.require: issueI8V6MD
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, UnRegisterSelfDrawingNodeRectChangeCallback001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(
        RSIServiceToRenderConnectionInterfaceCode::UNREGISTER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK);
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_REPLY);
}

/**
 * @tc.name: UnRegisterSelfDrawingNodeRectChangeCallback002
 * @tc.desc: Test UnRegisterSelfDrawingNodeRectChangeCallback when reply.WriteInt32 fails
 * @tc.type: FUNC
 * @tc.require: issueI8V6MD
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, UnRegisterSelfDrawingNodeRectChangeCallback002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(
        RSIServiceToRenderConnectionInterfaceCode::UNREGISTER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK);
    pid_t remotePid = 12345;
    data.WriteInt32(remotePid);
    SetLeftSize(reply, 0);
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_REPLY);
}

/**
 * @tc.name: UnRegisterSelfDrawingNodeRectChangeCallback003
 * @tc.desc: Test UnRegisterSelfDrawingNodeRectChangeCallback with valid remotePid
 * @tc.type: FUNC
 * @tc.require: issueI8V6MD
 */
HWTEST_F(RSServiceToRenderConnectionStubTest, UnRegisterSelfDrawingNodeRectChangeCallback003, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    ASSERT_TRUE(data.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor()));
    uint32_t code = static_cast<uint32_t>(
        RSIServiceToRenderConnectionInterfaceCode::UNREGISTER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK);
    pid_t remotePid = 12345;
    data.WriteInt32(remotePid);
    auto ret = g_connectionStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_NONE);
}
} // namespace OHOS::Rosen
