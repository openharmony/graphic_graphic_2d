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
#include "rs_render_composer_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
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
}
void RSRenderServiceAgentTest::TearDownTestCase()
{
    g_rsAgent = nullptr;
}
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
 * @tc.name: PostTaskImmediateInPlace001
 * @tc.desc: PostTaskImmediateInPlace Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderServiceAgentTest, PostTaskImmediateInPlace001, TestSize.Level1)
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
 * @tc.name: ScheduleTask001
 * @tc.desc: ScheduleTask Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderServiceAgentTest, ScheduleTask001, TestSize.Level1)
{
    auto renderService = sptr<RSRenderService>::MakeSptr();
    renderService->runner_ = AppExecFwk::EventRunner::Create(false);
    renderService->handler_ = std::make_shared<AppExecFwk::EventHandler>(renderService->runner_);
    sptr<RRenderServiceAgent> renderServiceAgent = sptr<RSRenderServiceAgent>::MakeSptr(*renderService);
    auto task = []() {};
    auto future = renderServiceAgent->ScheduleTask(std::move(task));
    ASSERT_EQ(future.valid(), true);
}

/**
 * @tc.name: HandleGameSceneChanged001
 * @tc.desc: HandleGameSceneChanged Test with RsGameFrameHandler nullptr.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderServiceAgentTest, HandleGameSceneChanged001, TestSize.Level1)
{
    auto renderService = sptr<RSRenderService>::MakeSptr();
    auto vsyncManager = sptr<RSVsyncManager>::MakeSptr();
    auto screenManager = sptr<RSScreenManager>::MakeSptr();
    ASSERT_NE(renderService, nullptr);
    ASSERT_NE(vsyncManager, nullptr);
    ASSERT_NE(screenManager, nullptr);
    vsyncManager->init(screenManager);
    renderService->vsyncManager_ = vsyncManager;
    sptr<RSRenderServiceAgent> renderServiceAgent = sptr<RSRenderServiceAgent>::MakeSptr(*renderService);
    ASSERT_NE(renderServiceAgent, nullptr);
    auto& handler = renderService->GetGameFrameHandler();
    ASSERT_EQ(handler, nullptr);
    renderServiceAgent->HandleGameSceneChanged();
}

/**
 * @tc.name: HandleGameSceneChanged002
 * @tc.desc: HandleGameSceneChanged Test with RsGameFrameHandler.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderServiceAgentTest, HandleGameSceneChanged002, TestSize.Level1)
{
    auto renderService = sptr<RSRenderService>::MakeSptr();
    auto vsyncManager = sptr<RSVsyncManager>::MakeSptr();
    auto screenManager = sptr<RSScreenManager>::MakeSptr();
    ASSERT_NE(renderService, nullptr);
    ASSERT_NE(vsyncManager, nullptr);
    ASSERT_NE(screenManager, nullptr);
    vsyncManager->init(screenManager);
    renderService->vsyncManager_ = vsyncManager;
    sptr<RSRenderServiceAgent> renderServiceAgent = sptr<RSRenderServiceAgent>::MakeSptr(*renderService);
    ASSERT_NE(renderServiceAgent, nullptr);
    renderService->InitGameFrameHandler();
    auto& handler = renderService->GetGameFrameHandler();
    ASSERT_NE(handler, nullptr);
    renderServiceAgent->HandleGameSceneChanged();
}

/**
 * @tc.name: GetProcessInfo001
 * @tc.desc: GetProcessInfo Test with null vsyncToken
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderServiceAgentTest, GetProcessInfo001, TestSize.Level1)
{
    auto renderService = sptr<RSRenderService>::MakeSptr();
    ASSERT_NE(renderService, nullptr);
    auto runner = AppExecFwk::EventRunner::Create(false);
    renderService->handler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
    renderService->screenManager_ = sptr<RSScreenManager>::MakeSptr();
    renderService->vsyncManager_ = sptr<RSVsyncManager>::MakeSptr();
    renderService->vsyncManager_->init(renderService->screenManager_);
    renderService->rsRenderComposerManager_ = std::make_shared<RSRenderComposerManager>(renderService->handler_);
    sptr<RSRenderServiceAgent> renderServiceAgent = sptr<RSRenderServiceAgent>::MakeSptr(*renderService);
    ASSERT_NE(renderServiceAgent, nullptr);
    ScreenId screenId = 0;
    sptr<IRemoteObject> vsyncToken = nullptr;
    auto result = renderServiceAgent->GetProcessInfo(screenId, vsyncToken);
    EXPECT_EQ(result.first, nullptr);
    EXPECT_NE(result.second, nullptr);
}

/**
 * @tc.name: RegisterHgmProcessCallback001
 * @tc.desc: RegisterHgmProcessCallback Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderServiceAgentTest, RegisterHgmProcessCallback001, TestSize.Level1)
{
    auto renderService = sptr<RSRenderService>::MakeSptr();
    ASSERT_NE(renderService, nullptr);
    sptr<RSRenderServiceAgent> renderServiceAgent = sptr<RSRenderServiceAgent>::MakeSptr(*renderService);
    ASSERT_NE(renderServiceAgent, nullptr);
    HgmProcessCallback callback = [](uint64_t, uint64_t,
        const sptr<HgmProcessToServiceInfo>&, const sptr<HgmServiceToProcessInfo>&) -> void {};
    renderServiceAgent->RegisterHgmProcessCallback(callback);
    renderServiceAgent->RegisterHgmProcessCallback(nullptr);
}
} // namespace OHOS::Rosen
