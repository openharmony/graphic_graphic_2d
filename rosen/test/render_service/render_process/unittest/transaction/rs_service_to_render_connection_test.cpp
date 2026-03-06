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

#include <iremote_broker.h>

#include "irs_render_to_composer_connection.h"
#include "parameters.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/render_thread/rs_render_engine.h"
#include "rs_render_process_manager.h"
#include "rs_render_service.h"
#include "rs_render_to_composer_connection_proxy.h"
#include "rs_render_single_process_manager.h"
#include "transaction/rs_service_to_render_connection.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
constexpr const int WAIT_HANDLER_TIME = 1; // 1S
constexpr const int WAIT_HANDLER_TIME_COUNT = 5;

std::shared_ptr<RSRenderPipeline> renderPipeline = nullptr;
sptr<RSServiceToRenderConnection> g_rsConn = nullptr;
}

class RSServiceToRenderConnectionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static void WaitHandlerTask();
};

void RSServiceToRenderConnectionTest::SetUpTestCase()
{
    renderPipeline = std::make_shared<RSRenderPipeline>();
    renderPipeline->imageEnhanceManager_ = std::make_shared<ImageEnhanceManager>();

    auto runner1 = AppExecFwk::EventRunner::Create(true);
    auto handler1 = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner1);
    auto mainThread = RSMainThread::Instance();
    mainThread->hwcContext_ =
        std::make_shared<RSHwcContext>(HWCParam::GetSourceTuningForAppMap(), HWCParam::GetSolidColorLayerMap());
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
    g_rsConn = sptr<RSServiceToRenderConnection>::MakeSptr(renderPipelineAgent);
}

void RSServiceToRenderConnectionTest::TearDownTestCase()
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

    g_rsConn = nullptr;
}

void RSServiceToRenderConnectionTest::SetUp() {}
void RSServiceToRenderConnectionTest::TearDown() {}

void RSServiceToRenderConnectionTest::WaitHandlerTask()
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
 * @tc.name: HgmForceUpdateTaskTest
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionTest, HgmForceUpdateTaskTest, TestSize.Level1)
{
    g_rsConn->HgmForceUpdateTask(false, "");
    g_rsConn->HgmForceUpdateTask(true, "");
    ASSERT_TRUE(g_rsConn);
}

/**
 * @tc.name: GetRealtimeRefreshRateTest
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSServiceToRenderConnectionTest, GetRealtimeRefreshRateTest, TestSize.Level1)
{
    ASSERT_TRUE(g_rsConn);
    auto result = g_rsConn->GetRealtimeRefreshRate(INVALID_SCREEN_ID);
    EXPECT_GE(result, 0);
}


/**
 * @tc.name: SetBehindWindowFilterEnabledTest
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionTest, SetBehindWindowFilterEnabledTest, TestSize.Level1)
{
    bool enabled = true;
    bool enabled1 = false;
    g_rsConn->SetBehindWindowFilterEnabled(enabled);
    g_rsConn->SetBehindWindowFilterEnabled(enabled1);
    ASSERT_TRUE(g_rsConn);
}

/**
 * @tc.name: GetBehindWindowFilterEnabledTest
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSServiceToRenderConnectionTest, GetBehindWindowFilterEnabledTest, TestSize.Level1)
{
    bool enabled = true;
    bool enabled1 = false;
    g_rsConn->GetBehindWindowFilterEnabled(enabled);
    g_rsConn->GetBehindWindowFilterEnabled(enabled1);
    ASSERT_TRUE(g_rsConn);
}

/**
 * @tc.name: SetShowRefreshRateEnabledTest
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSServiceToRenderConnectionTest, SetShowRefreshRateEnabledTest, TestSize.Level1)
{
    bool enabled = true;
    bool enabled1 = false;
    int32_t type = 1;
    g_rsConn->SetShowRefreshRateEnabled(enabled, type);
    g_rsConn->SetShowRefreshRateEnabled(enabled1, type);
    ASSERT_TRUE(g_rsConn);
}

/**
 * @tc.name: GetShowRefreshRateEnabledTest
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSServiceToRenderConnectionTest, GetShowRefreshRateEnabledTest, TestSize.Level1)
{
    bool enabled = true;
    bool enabled1 = false;
    g_rsConn->GetShowRefreshRateEnabled(enabled);
    g_rsConn->GetShowRefreshRateEnabled(enabled1);
    ASSERT_TRUE(g_rsConn);
}

/**
 * @tc.name: NotifyPackageEventTest
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSServiceToRenderConnectionTest, NotifyPackageEventTest, TestSize.Level1)
{
    uint32_t listSize1 = 0;
    std::vector<std::string> package1;
    uint32_t listSize2 = 2;
    std::vector<std::string> package2 = {"package1", "package2"};
    g_rsConn->NotifyPackageEvent(listSize1, package1);
    g_rsConn->NotifyPackageEvent(listSize2, package2);
    ASSERT_TRUE(g_rsConn);
}
} // namespace OHOS::Rosen
