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

#include <refbase.h>
#include <iremote_broker.h>

#include "parameters.h"
#include "rs_render_service_agent.h"
#include "rs_render_process_manager.h"
#include "rs_render_service.h"
#include "rs_render_to_composer_connection_proxy.h"
#include "rs_render_to_composer_connection.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "rs_render_composer_agent.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
RSRenderService renderService;
sptr<RSRenderServiceAgent> g_rsAgent = nullptr;
}

class RSRenderServiceAgentTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderServiceAgentTest::SetUpTestCase()
{
    OHOS::system::SetParameter("bootevent.samgr.ready", "false");
    g_rsAgent = sptr<RSRenderServiceAgent>::MakeSptr(renderService);
}

void RSRenderServiceAgentTest::TearDownTestCase() {}
void RSRenderServiceAgentTest::SetUp() {}
void RSRenderServiceAgentTest::TearDown() {}

/**
 * @tc.name: PostTaskImmediate001
 * @tc.desc: PostTaskImmediate Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderServiceAgentTest, PostTaskImmediate001, TestSize.Level1)
{
    auto renderService = sptr<RSRenderService>::MakeSptr();
    renderService->runner_ = AppExecFwk::EventRunner::Create(false);
    renderService->handler_ = std::make_shared<AppExecFwk::EventHandler>(renderService->runner_);
    sptr<RSRenderServiceAgent> renderServiceAgent = sptr<RSRenderServiceAgent>::MakeSptr(*renderService);
    ASSERT_NE(renderServiceAgent, nullptr);
    RSTaskMessage::RSTask task = []() -> void { return; };
    renderServiceAgent->PostTaskImmediate(task);
}

/**
 * @tc.name: PostTaskImmediate002
 * @tc.desc: PostTaskImmediate Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderServiceAgentTest, PostTaskImmediate002, TestSize.Level1)
{
    auto renderService = sptr<RSRenderService>::MakeSptr();
    renderService->runner_ = AppExecFwk::EventRunner::Create(false);
    renderService->handler_ = std::make_shared<AppExecFwk::EventHandler>(renderService->runner_);
    sptr<RSRenderServiceAgent> renderServiceAgent = sptr<RSRenderServiceAgent>::MakeSptr(*renderService);
    ASSERT_NE(renderServiceAgent, nullptr);
    RSTaskMessage::RSTask task = []() -> void { return; };
    renderServiceAgent->PostTaskImmediate(task);
}

/**
 * @tc.name: PostTaskImmediateInPlace001
 * @tc.desc: PostTaskImmediateInPlace Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderServiceAgentTest, PostTaskImmediateInPlace001, TestSize.Level1)
{
    auto renderService = sptr<RSRenderService>::MakeSptr();
    sptr<RSRenderServiceAgent> renderServiceAgent = sptr<RSRenderServiceAgent>::MakeSptr(*renderService);
    ASSERT_NE(renderServiceAgent, nullptr);
    RSTaskMessage::RSTask task = []() -> void { return; };
    renderServiceAgent->PostTaskImmediateInPlace(task);
}

/**
 * @tc.name: PostTaskImmediateInPlace002
 * @tc.desc: PostTaskImmediateInPlace Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderServiceAgentTest, PostTaskImmediateInPlace002, TestSize.Level1)
{
    auto renderService = sptr<RSRenderService>::MakeSptr();
    renderService->runner_ = AppExecFwk::EventRunner::Create(false);
    renderService->handler_ = std::make_shared<AppExecFwk::EventHandler>(renderService->runner_);
    sptr<RSRenderServiceAgent> renderServiceAgent = sptr<RSRenderServiceAgent>::MakeSptr(*renderService);
    ASSERT_NE(renderServiceAgent, nullptr);
    RSTaskMessage::RSTask task = []() -> void { return; };
    renderServiceAgent->PostTaskImmediateInPlace(task);
}

/**
 * @tc.name: PostSyncTaskImmediate001
 * @tc.desc: PostSyncTaskImmediate Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderServiceAgentTest, PostSyncTaskImmediate001, TestSize.Level1)
{
    auto renderService = sptr<RSRenderService>::MakeSptr();
    renderService->runner_ = AppExecFwk::EventRunner::Create(false);
    renderService->handler_ = std::make_shared<AppExecFwk::EventHandler>(renderService->runner_);
    sptr<RSRenderServiceAgent> renderServiceAgent = sptr<RSRenderServiceAgent>::MakeSptr(*renderService);
    ASSERT_NE(renderServiceAgent, nullptr);
    RSTaskMessage::RSTask task = []() -> void { return; };
    renderServiceAgent->PostSyncTaskImmediate(task);
}

/**
 * @tc.name: ProcessHgmFrameRateTest
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSRenderServiceAgentTest, ProcessHgmFrameRateTest, TestSize.Level1)
{
    int32_t timeStamp = 1;
    int32_t vsyncId = 1;
    sptr<HgmProcessToServiceInfo> processToServiceInfo = sptr<HgmProcessToServiceInfo>::MakeSptr();
    sptr<HgmServiceToProcessInfo> serviceToProcessInfo = sptr<HgmServiceToProcessInfo>::MakeSptr();
    g_rsAgent->ProcessHgmFrameRate(timeStamp, vsyncId, processToServiceInfo, serviceToProcessInfo);
    ASSERT_TRUE(g_rsAgent);
}
} // namespace OHOS::Rosen
