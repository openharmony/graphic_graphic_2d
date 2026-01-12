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
void RSServiceToRenderConnectionStubTest::TearDownTestCase() {}
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
} // namespace OHOS::Rosen
