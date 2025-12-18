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

#include <gtest/gtest.h>

#include "parameters.h"
#include "transaction/zidl/rs_irender_to_service_connection_stub.h"
#include "rs_render_process_manager_agent.h"
#include "rs_render_service_agent.h"
#include "transaction/zidl/rs_irender_to_service_connection_ipc_interface_code.h"
#include "transaction/rs_render_to_service_connection.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"

using namespace testing;
using namespace testing::ext;


namespace OHOS::Rosen {
namespace {
RSRenderService renderService;
static inline sptr<RSRenderToServiceConnectionStub> connectionStub_ = nullptr;
}

class RSRenderToServiceConnectionStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderToServiceConnectionStubTest::SetUpTestCase()
{
    OHOS::system::SetParameter("bootevent.samgr.ready", false);
    renderservice.Init();
    RSUniRenderThread::Instance().uniRenderEngine_ = nullptr;
    sptr<RSRenderServiceAgent> rsRenderServiceAgent = sptr<RSRenderServiceAgent>::MakeSptr(renderservice);
    sptr<RSRenderProcessManagerAgent> renderProcessManagerAgent =
        sptr<RSRenderProcessManagerAgent>::MakeSptr(renderService.renderProcessManager_);
    connectionStub_ = sptr<RSRenderToServiceConnection>::MakeSptr(renderServiceAgent, renderProcessManagerAgent);
}
void RSRenderToServiceConnectionStubTest::TearDownTestCase() {}
void RSRenderToServiceConnectionStubTest::SetUp() {}
void RSRenderToServiceConnectionStubTest::TearDown() {}

/**
 * @tc.name: TestRSRenderToServiceConnectionStub001
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSRenderToServiceConnectionStubTest, TestRSRenderToServiceConnectionStub001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(
        RSIRenderToServiceConnectionInterfaceCode::NOTIFY_PROCESS_FRAME_RATE);
    int32_t sizes = 0;
    uint64_t timestamp = 1;
    uint64_t vsyncId = 1;
    data.WriteInt32(sizes);
    data.WriteUint64_t(timestamp);
    data.WriteUint64_t(vsyncId);
    for (uint64_t i = 0; i < sizes; ++i) {
        data.WriteUint64(i);
    }
    connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_TRUE(connectionStub_);
}
} // namespace OHOS::Rosen
