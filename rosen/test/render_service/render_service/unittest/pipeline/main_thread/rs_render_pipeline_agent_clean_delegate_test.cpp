/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>

#include <iremote_stub.h>
#include <memory>

#include "command/rs_delegate_composite_command.h"
#include "common/rs_common_def.h"
#include "consumer_surface.h"
#include "feature/delegate_composite/rs_delegate_composite_callback_manager.h"
#include "iconsumer_surface.h"
#include "ipc_callbacks/rs_delegate_composite_callback.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_surface_render_node.h"
#include "rs_render_pipeline.h"
#include "rs_render_pipeline_agent.h"
#include "transaction/rs_transaction_data.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
constexpr uint64_t CLEAN_TEST_DEFAULT_ID = 10;

class RSRenderPipelineAgentCleanDelegateTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    static inline RSMainThread* mainThread_ = nullptr;
};

void RSRenderPipelineAgentCleanDelegateTest::SetUpTestCase()
{
    mainThread_ = RSMainThread::Instance();
    ASSERT_NE(mainThread_, nullptr);
    mainThread_->handler_ =
        std::make_shared<OHOS::AppExecFwk::EventHandler>(OHOS::AppExecFwk::EventRunner::Create(true));
    mainThread_->handler_->eventRunner_->Run();
    ASSERT_NE(mainThread_->context_, nullptr);
}

void RSRenderPipelineAgentCleanDelegateTest::TearDownTestCase()
{
    if (mainThread_ != nullptr && mainThread_->handler_ != nullptr) {
        mainThread_->handler_->RemoveAllEvents();
        mainThread_->handler_->eventRunner_->Stop();
        mainThread_->handler_->eventRunner_ = nullptr;
        mainThread_->handler_ = nullptr;
    }
    mainThread_ = nullptr;
}

#if !defined(ROSEN_CROSS_PLATFORM) && defined(RS_ENABLE_DELEGATE_COMPOSITE)
namespace {
constexpr pid_t AGENT_CLEAN_SENDING_PID = 78001;
constexpr pid_t AGENT_CLEAN_SENDING_TID = 78002;
constexpr pid_t AGENT_CLEAN_CLIENT_PID = 78003;
constexpr uint64_t AGENT_CLEAN_SRC_ID = 780000;
constexpr uint64_t AGENT_CLEAN_SEQ_NUM = 781000;
constexpr uint32_t AGENT_CLEAN_BUFFER_SEQ = 7811;
} // namespace

class MockAgentBufferReleaseCallback : public IRemoteStub<RSISurfaceNodeBufferReleaseCallback> {
public:
    MockAgentBufferReleaseCallback() = default;
    ~MockAgentBufferReleaseCallback() noexcept override = default;
    GSError OnBufferComplete(std::queue<OnCompletedRet>& queue) override
    {
        callCount++;
        while (!queue.empty()) {
            received.push_back(queue.front());
            queue.pop();
        }
        return GSERROR_OK;
    }
    uint32_t callCount = 0;
    std::vector<OnCompletedRet> received;
};

static void PrepareAgentCleanCommands(NodeId nodeId, uint64_t index)
{
    auto& manager = RsDelegateCompositeCallbackManager::GetInstance();
    manager.AddSurfaceTransactionCmdInfo(
        AGENT_CLEAN_SRC_ID, AGENT_CLEAN_SEQ_NUM + index, AGENT_CLEAN_SENDING_PID, AGENT_CLEAN_SENDING_TID);
    auto data = std::make_unique<RSTransactionData>();
    data->SetSendingPid(AGENT_CLEAN_SENDING_PID);
    data->SetSendingTid(AGENT_CLEAN_SENDING_TID);
    data->SetIndex(index);
    RSTransactionConfig config;
    config.nodeId = nodeId;
    config.transactionInfo.bufferInfo.sequence = AGENT_CLEAN_BUFFER_SEQ;
    std::vector<RSTransactionConfig> configs = { config };
    auto& payload = data->GetPayload();
    payload.emplace_back(nodeId, FollowType::NONE,
        std::make_unique<TransactionBufferCommand>(configs, nodeId));
    payload.emplace_back(nodeId, FollowType::NONE,
        std::make_unique<TransactionBufferCommand>(RectF(15.0f, 16.0f, 17.0f, 18.0f), true, nodeId));
    manager.PrepareDelegateCompositeCommand(data);
}

/**
 * @tc.name: CleanDelegateModeBufferCmd_NullPipeline_ReturnFalse
 * @tc.desc: Verify CleanDelegateModeBufferCmd returns false when pipeline is null.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPipelineAgentCleanDelegateTest, CleanDelegateModeBufferCmd_NullPipeline_ReturnFalse, TestSize.Level1)
{
    std::shared_ptr<RSRenderPipeline> renderPipeline = nullptr;
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);
    EXPECT_FALSE(agent->CleanDelegateModeBufferCmd(CLEAN_TEST_DEFAULT_ID, 0));
}

/**
 * @tc.name: CleanDelegateModeBufferCmd_NodeMissing_ReturnTrue
 * @tc.desc: Verify CleanDelegateModeBufferCmd returns true and the main thread task
 *           exits safely when the node is not registered.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPipelineAgentCleanDelegateTest, CleanDelegateModeBufferCmd_NodeMissing_ReturnTrue, TestSize.Level1)
{
    auto renderPipeline = std::make_shared<RSRenderPipeline>();
    renderPipeline->mainThread_ = mainThread_;
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);
    EXPECT_TRUE(agent->CleanDelegateModeBufferCmd(999999999, AGENT_CLEAN_SENDING_PID));
}

/**
 * @tc.name: CleanDelegateModeBufferCmd_DelegateModeOff_KeepsBufferCmd
 * @tc.desc: Verify the main thread task skips cleaning when node delegate mode is off,
 *           the pending buffer command survives and still notifies release callback.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPipelineAgentCleanDelegateTest,
    CleanDelegateModeBufferCmd_DelegateModeOff_KeepsBufferCmd, TestSize.Level1)
{
    auto renderPipeline = std::make_shared<RSRenderPipeline>();
    renderPipeline->mainThread_ = mainThread_;
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    NodeId nodeId = (static_cast<NodeId>(AGENT_CLEAN_CLIENT_PID) << 32) | 91;
    auto node = std::make_shared<RSSurfaceRenderNode>(nodeId, std::make_shared<RSContext>());
    auto handler = node->GetMutableRSSurfaceHandler();
    ASSERT_NE(handler, nullptr);
    handler->SetConsumer(IConsumerSurface::Create("AgentDelegateOffSurface"));
    auto& mainNodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
    ASSERT_TRUE(mainNodeMap.RegisterRenderNode(node));

    PrepareAgentCleanCommands(nodeId, 1);
    auto& manager = RsDelegateCompositeCallbackManager::GetInstance();
    sptr<MockAgentBufferReleaseCallback> listener = new MockAgentBufferReleaseCallback();
    manager.RegisterSurfaceNodeBufferReleaseListener(AGENT_CLEAN_CLIENT_PID, listener);

    // delegate mode off: task returns without cleaning
    EXPECT_TRUE(agent->CleanDelegateModeBufferCmd(nodeId, AGENT_CLEAN_SENDING_PID));
    // enable delegate mode for the following process to make flush failure observable
    node->SetDelegateMode(true);
    while (manager.ProcessDelegateCompositeCommand(RSMainThread::Instance()->GetContext())) {}

    // rect command survived, buffer command survived as well and flushed with failure
    EXPECT_EQ(node->GetDelegateSrcRect(), Vector4f(
        RectF(15.0f, 16.0f, 17.0f, 18.0f).GetTop(), RectF(15.0f, 16.0f, 17.0f, 18.0f).GetLeft(),
        RectF(15.0f, 16.0f, 17.0f, 18.0f).GetWidth(), RectF(15.0f, 16.0f, 17.0f, 18.0f).GetHeight()));
    EXPECT_EQ(listener->callCount, 1);

    manager.NotifyCurrentSurfaceNodeBufferReleaseCallback();
    manager.UnRegisterSurfaceNodeBufferReleaseListener(AGENT_CLEAN_CLIENT_PID);
    manager.RemoveAllListenerbyPid(AGENT_CLEAN_SENDING_PID);
    mainNodeMap.UnregisterRenderNode(nodeId);
}

/**
 * @tc.name: CleanDelegateModeBufferCmd_DelegateModeOn_CleansBufferCmd
 * @tc.desc: Verify the main thread task cleans pending TRANSACTION_BUFFER commands of the
 *           delegate node so window resize is no longer blocked by stale buffer commands.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPipelineAgentCleanDelegateTest,
    CleanDelegateModeBufferCmd_DelegateModeOn_CleansBufferCmd, TestSize.Level1)
{
    auto renderPipeline = std::make_shared<RSRenderPipeline>();
    renderPipeline->mainThread_ = mainThread_;
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);

    NodeId nodeId = (static_cast<NodeId>(AGENT_CLEAN_CLIENT_PID) << 32) | 92;
    auto node = std::make_shared<RSSurfaceRenderNode>(nodeId, std::make_shared<RSContext>());
    auto handler = node->GetMutableRSSurfaceHandler();
    ASSERT_NE(handler, nullptr);
    handler->SetConsumer(IConsumerSurface::Create("AgentDelegateOnSurface"));
    node->SetDelegateMode(true);
    auto& mainNodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
    ASSERT_TRUE(mainNodeMap.RegisterRenderNode(node));

    PrepareAgentCleanCommands(nodeId, 2);
    auto& manager = RsDelegateCompositeCallbackManager::GetInstance();
    sptr<MockAgentBufferReleaseCallback> listener = new MockAgentBufferReleaseCallback();
    manager.RegisterSurfaceNodeBufferReleaseListener(AGENT_CLEAN_CLIENT_PID, listener);

    EXPECT_TRUE(agent->CleanDelegateModeBufferCmd(nodeId, AGENT_CLEAN_SENDING_PID));
    while (manager.ProcessDelegateCompositeCommand(RSMainThread::Instance()->GetContext())) {}

    // rect command survives while buffer command of the node is cleaned: no release callback
    EXPECT_EQ(node->GetDelegateSrcRect(), Vector4f(
        RectF(15.0f, 16.0f, 17.0f, 18.0f).GetTop(), RectF(15.0f, 16.0f, 17.0f, 18.0f).GetLeft(),
        RectF(15.0f, 16.0f, 17.0f, 18.0f).GetWidth(), RectF(15.0f, 16.0f, 17.0f, 18.0f).GetHeight()));
    EXPECT_EQ(listener->callCount, 0);

    manager.NotifyCurrentSurfaceNodeBufferReleaseCallback();
    manager.UnRegisterSurfaceNodeBufferReleaseListener(AGENT_CLEAN_CLIENT_PID);
    manager.RemoveAllListenerbyPid(AGENT_CLEAN_SENDING_PID);
    mainNodeMap.UnregisterRenderNode(nodeId);
}
#else
/**
 * @tc.name: CleanDelegateModeBufferCmd_FeatureDisabled_ReturnFalse
 * @tc.desc: Verify CleanDelegateModeBufferCmd returns false when delegate composite is disabled.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPipelineAgentCleanDelegateTest,
    CleanDelegateModeBufferCmd_FeatureDisabled_ReturnFalse, TestSize.Level1)
{
    auto renderPipeline = std::make_shared<RSRenderPipeline>();
    renderPipeline->mainThread_ = mainThread_;
    sptr<RSRenderPipelineAgent> agent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    ASSERT_NE(agent, nullptr);
    EXPECT_FALSE(agent->CleanDelegateModeBufferCmd(CLEAN_TEST_DEFAULT_ID, 0));
}
#endif
} // namespace Rosen
} // namespace OHOS
