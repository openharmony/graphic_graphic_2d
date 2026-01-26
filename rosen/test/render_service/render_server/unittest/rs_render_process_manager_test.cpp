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

#include "parameters.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "render_process/transaction/rs_service_to_render_connection.h"
#include "render_server/transaction/rs_render_to_service_connection.h"
#include "rs_composer_to_render_connection.h"
#include "rs_render_composer_agent.h"
#include "rs_render_composer_manager.h"
#include "rs_render_single_process_manager.h"
#include "rs_render_to_composer_connection.h"
#include "screen_manager/screen_types.h"
#include "transaction/rs_client_to_render_connection.h"
#include "transaction/rs_connect_to_render_process.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
class RSSingleRenderProcessManagerMock : public RSRenderProcessManager {
public:
    explicit RSSingleRenderProcessManagerMock(RSRenderService& renderService_)
        : RSRenderProcessManager(renderService_)
    {
        // step2: Create renderPipeline and Following Connections
        auto renderServiceAgent = sptr<RSRenderServiceAgent>::MakeSptr(renderService_);
        auto renderProcessManagerAgent =
            sptr<RSRenderProcessManagerAgent>::MakeSptr(renderService_.renderProcessManager_);
        auto screenManagerAgent = sptr<RSScreenManagerAgent>::MakeSptr(renderService_.screenManager_);
        renderToServiceConnection_ = sptr<RSRenderToServiceConnection>::MakeSptr(renderServiceAgent,
            renderProcessManagerAgent, screenManagerAgent);
        renderService_.renderPipeline_ = RSRenderPipeline::Create(renderService_.handler_,
            nullptr, renderToServiceConnection_, nullptr);

        auto mainThread = RSMainThread::Instance();
        renderService_.renderPipeline_->mainThread_ = mainThread;
        mainThread->RegisterScreenSwitchFinishCallback(renderToServiceConnection_);

        renderService_.renderPipeline_->uniRenderThread_ = &(RSUniRenderThread::Instance());

        auto renderPipelineAgent = sptr<RSRenderPipelineAgent>::MakeSptr(renderService_.renderPipeline_);
        serviceToRenderConnection_ = sptr<RSServiceToRenderConnection>::MakeSptr(renderPipelineAgent);
        composerToRenderConnection_ = sptr<RSComposerToRenderConnection>::MakeSptr();

        // step3:
        connectToRenderConnection_ = sptr<RSConnectToRenderProcess>::MakeSptr(renderPipelineAgent);

        // Cancel Thread
        renderService_.renderPipeline_->uniRenderThread_->handler_ = nullptr;
        renderService_.renderPipeline_->uniRenderThread_->runner_ = nullptr;
        renderService_.renderPipeline_->mainThread_->handler_ = nullptr;
    }

    ~RSSingleRenderProcessManagerMock() noexcept override = default;

    sptr<IRemoteObject> OnScreenConnected(ScreenId screenId_,
        const std::shared_ptr<HdiOutput>& output, const sptr<RSScreenProperty>& property) override
    {
        auto composerConn = renderService_.rsRenderComposerManager_->GetRSComposerConnection(property->GetScreenId());
        renderService_.renderPipeline_->OnScreenConnected(property, composerConn, composerToRenderConnection_,
            renderService_.rsVsyncManagerAgent_, output);
        return connectToRenderConnection_->AsObject();
    }

    void OnScreenDisconnected(ScreenId id) override
    {
        renderService_.renderPipeline_->OnScreenDisconnected(id);
    }

    void OnScreenPropertyChanged(
        ScreenId id, ScreenPropertyType type, const sptr<ScreenPropertyBase>& property) override
    {
        renderService_.renderPipeline_->OnScreenPropertyChanged(id, type, property);
    }

    void OnScreenRefresh(ScreenId id) override
    {
        renderService_.renderPipeline_->OnScreenRefresh(id);
    }

    void OnVirtualScreenConnected(ScreenId id, ScreenId associatedScreenId,
        const sptr<RSScreenProperty>& property) override
    {
        renderService_.renderPipeline_->OnScreenConnected(property, nullptr, nullptr, nullptr, nullptr);
    }

    void OnVirtualScreenDisconnected(ScreenId id) override
    {
        renderService_.renderPipeline_->OnScreenDisconnected(id);
    }

    sptr<RSIServiceToRenderConnection> GetServiceToRenderConn(ScreenId screenId) const override
    {
        return serviceToRenderConnection_;
    }

    std::vector<sptr<RSIServiceToRenderConnection>> GetServiceToRenderConns() const override
    {
        return { serviceToRenderConnection_ };
    }

    sptr<RSIConnectToRenderProcess> GetConnectToRenderConnection(ScreenId screenId) const override
    {
        return connectToRenderConnection_;
    }
    sptr<RSIServiceToRenderConnection> serviceToRenderConnection_ = nullptr;
    sptr<IRSComposerToRenderConnection> composerToRenderConnection_ = nullptr;
    sptr<RSIRenderToServiceConnection> renderToServiceConnection_ = nullptr;
    sptr<RSIConnectToRenderProcess> connectToRenderConnection_ = nullptr;
};
} // namespace

class RSRenderProcessManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static uint32_t screenId_;
private:
    static inline RSRenderService renderService_;
};

uint32_t RSRenderProcessManagerTest::screenId_ = 0;

void RSRenderProcessManagerTest::SetUpTestCase()
{
    auto runner = OHOS::AppExecFwk::EventRunner::Create(true);
    renderService_.handler_ = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner);
    renderService_.renderProcessManager_ = sptr<RSSingleRenderProcessManagerMock>::MakeSptr(renderService_);
    renderService_.rsRenderComposerManager_ = std::make_shared<RSRenderComposerManager>(
        renderService_.handler_, nullptr);
}
void RSRenderProcessManagerTest::TearDownTestCase()
{
    RSUniRenderThread::Instance().uniRenderEngine_->GetRenderContext()->drGPUContext_ = nullptr;
    RSUniRenderThread::Instance().uniRenderEngine_ = nullptr;
}
void RSRenderProcessManagerTest::SetUp() {}
void RSRenderProcessManagerTest::TearDown() {}

/**
 * @tc.name: OnVBlankIdleTest
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSRenderProcessManagerTest, OnVBlankIdleTest, TestSize.Level1)
{
    uint64_t ns = 0;
    renderService_.renderProcessManager_->OnVBlankIdle(screenId_, ns);
    ASSERT_TRUE(renderService_.renderProcessManager_);
}

/**
 * @tc.name: OnActiveScreenIdChangedTest
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSRenderProcessManagerTest, OnActiveScreenIdChangedTest, TestSize.Level1)
{
    renderService_.renderProcessManager_->OnActiveScreenIdChanged(screenId_);
    ASSERT_TRUE(renderService_.renderProcessManager_);
}

/**
 * @tc.name: OnHwcRestoredTest
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSRenderProcessManagerTest, OnHwcRestoredTest, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(screenId_);
    output->Init();
    sptr<RSScreenProperty> property = sptr<RSScreenProperty>::MakeSptr();
    renderService_.renderProcessManager_->OnHwcRestored(screenId_, output, property);
    ASSERT_TRUE(renderService_.renderProcessManager_);
}

/**
 * @tc.name: OnHwcDeadTest
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSRenderProcessManagerTest, OnHwcDeadTest, TestSize.Level1)
{
    renderService_.renderProcessManager_->OnHwcDead(screenId_);
    ASSERT_TRUE(renderService_.renderProcessManager_);
}

/**
 * @tc.name: OnHwcEventTest
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSRenderProcessManagerTest, OnHwcEventTest, TestSize.Level1)
{
    uint32_t deviceId = 100;
    uint32_t eventId = 100;
    std::vector<int32_t> eventData;
    renderService_.renderProcessManager_->OnHwcEvent(deviceId, eventId, eventData);
    ASSERT_TRUE(renderService_.renderProcessManager_);
}

/**
 * @tc.name: OnScreenBacklightChangedTest
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSRenderProcessManagerTest, OnScreenBacklightChangedTest, TestSize.Level1)
{
    uint32_t level = 100;
    renderService_.renderProcessManager_->OnScreenBacklightChanged(screenId_, level);
    ASSERT_TRUE(renderService_.renderProcessManager_);
}

/**
 * @tc.name: OnGlobalBlacklistChangedTest
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSRenderProcessManagerTest, OnGlobalBlacklistChangedTest, TestSize.Level1)
{
    std::unordered_set<NodeId> globalBlackList;
    renderService_.renderProcessManager_->OnGlobalBlacklistChanged(globalBlackList);
    ASSERT_TRUE(renderService_.renderProcessManager_);
}
} // namespace OHOS::Rosen
