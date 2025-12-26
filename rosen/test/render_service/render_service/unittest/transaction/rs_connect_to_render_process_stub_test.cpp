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

#include "gtest/gtest.h"
#include <hdi_backend>
#include <iremote_broker.h>
#include <iremote_stub.h>

#include "parameters.h"
#include "/transaction/zidl/rs_connect_to_render_process_stub.h"
#include "render_server/rs_render_service.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
RSRenderService renderService;
static inline sptr<RSConnectionToRenderProcessStub> connectionStub_ = nullptr;
}
class RSConnectToRenderProcessStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSConnectToRenderProcessStubTest::SetUpTestCase()
{
    OHOS::system::SetParameter("bootevent.samgr.ready", "false");
    renderService.Init();
    RSUniRenderThread::Instance().uniRenderEngine_ = nullptr;
    connectionStub_ = sptr<RSConnectionToRenderProcessStub>::MakeSptr();
}
void RSConnectToRenderProcessStubTest::TearDownTestCase() {}
void RSConnectToRenderProcessStubTest::SetUp() {}
void RSConnectToRenderProcessStubTest::TearDown() {}

/**
 * @tc.name: TestRSConnectionToRenderProcessStub001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issue#IAS6LQ
 */
HWTEST_F(RSConnectToRenderProcessStubTest, TestRSConnectionToRenderProcessStub001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(RSIConnectToRenderProcess::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(
        RSIConnectToRenderProcessInterfaceCode::FORK_CONNECTION);
    sptr<RSIConnectionToken> token1 = new IRemoteStub<RSIConnectionToken>();
    connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_TRUE(connectionStub_);
}
} // namespace OHOS::Rosen
