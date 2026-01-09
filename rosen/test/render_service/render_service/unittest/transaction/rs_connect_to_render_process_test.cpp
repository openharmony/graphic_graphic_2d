/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include <hdi_backend.h>
#include <iremote_stub.h>

#include "parameters.h"
#include "transaction/zidl/rs_connect_to_render_process_stub.h"
#include "transaction/rs_connect_to_render_process.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "transaction/rs_client_to_render_connection.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
RSRenderService renderService;
sptr<RSConnectToRenderProcessStub> g_connectionStub = nullptr;
}

class RSConnectToRenderProcessTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSConnectToRenderProcessTest::SetUpTestCase()
{
    auto runner = AppExecFwk::EventRunner::Create(true);
    auto handler = std::make_shared<AppExecFwk::EventHandler>(runner);
    auto renderPipeline = RSRenderPipeline::Create(handler, nullptr, nullptr, nullptr);
    OHOS::system::SetParameter("bootevent.samgr.ready", "false");
    renderService.Init();
    RSUniRenderThread::Instance().uniRenderEngine_ = nullptr;
    sptr<RSRenderPipelineAgent> renderPipelineAgent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    g_connectionStub = sptr<RSConnectToRenderProcess>::MakeSptr(renderPipelineAgent);
}
void RSConnectToRenderProcessTest::TearDownTestCase() {}
void RSConnectToRenderProcessTest::SetUp() {}
void RSConnectToRenderProcessTest::TearDown() {}

/**
 * @tc.name: CreateRenderConnectionTest
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issue#IAS6LQ
 */
HWTEST_F(RSConnectToRenderProcessTest, CreateRenderConnectionTest, TestSize.Level1)
{
    sptr<RSIConnectionToken> token = nullptr;
    sptr<RSIConnectionToken> token1 = new IRemoteStub<RSIConnectionToken>();
    ASSERT_FALSE(g_connectionStub->CreateRenderConnection(token));
    ASSERT_TRUE(g_connectionStub->CreateRenderConnection(token1));
}
} // namespace OHOS::Rosen
