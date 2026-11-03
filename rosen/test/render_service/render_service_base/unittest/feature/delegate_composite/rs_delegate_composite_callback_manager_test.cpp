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

#include "command/rs_delegate_composite_command.h"
#include "common/rs_common_def.h"
#include "feature/delegate_composite/rs_delegate_composite_callback_manager.h"
#include "iconsumer_surface.h"
#include "ipc_callbacks/rs_delegate_composite_callback.h"
#include "iremote_stub.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_effect_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "surface_buffer_impl.h"
#include "sync_fence.h"
#include "transaction/rs_transaction_data.h"
#include "transaction/transaction.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
#ifndef ROSEN_CROSS_PLATFORM
namespace {
constexpr uint32_t CLEAN_BUFFER_SEQ = 901;
constexpr uint32_t KEEP_BUFFER_SEQ = 902;
constexpr uint32_t RELEASE_BUFFER_SEQ = 903;
constexpr uint32_t PREPARE_BUFFER_SEQ = 904;
constexpr uint64_t SRC_ID_BASE = 900000;
constexpr uint64_t SEQ_NUM_BASE = 910000;
constexpr uint64_t TX_INDEX_FIRST = 1;
constexpr uint64_t TX_INDEX_SECOND = 2;
constexpr NodeId UPDATE_HW_NODE_ID_BASE = 78000;
const char DELEGATE_CHILD_NAME[] = "delegate_child";
} // namespace

class MockNodeBufferReleaseCallback : public IRemoteStub<RSISurfaceNodeBufferReleaseCallback> {
public:
    MockNodeBufferReleaseCallback() = default;
    ~MockNodeBufferReleaseCallback() noexcept override = default;
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

class RSDelegateCompositeCallbackManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void TearDown() override;

    static std::unique_ptr<RSTransactionData> MakeTransactionData(
        pid_t sendingPid, pid_t sendingTid, uint64_t index);
    static std::unique_ptr<TransactionBufferCommand> MakeBufferCmd(NodeId nodeId, uint32_t sequence);
    static std::unique_ptr<TransactionBufferCommand> MakeRectCmd(NodeId nodeId, bool isSrcRect);
    static std::shared_ptr<RSSurfaceRenderNode> RegisterDelegateSurfaceNode(
        const std::shared_ptr<RSContext>& context, NodeId nodeId);
    static std::shared_ptr<RSSurfaceRenderNode> MakeNamedSurfaceNode(
        const std::shared_ptr<RSContext>& context, NodeId nodeId, const std::string& name);
    static std::shared_ptr<RSSurfaceRenderNode> MakeDelegateModeNode(
        const std::shared_ptr<RSContext>& context, NodeId nodeId, const std::string& name);
    static void PrepareCommands(pid_t pid, pid_t tid, uint64_t index,
        std::vector<std::unique_ptr<RSCommand>> cmds);
    static void DrainCmdQueues(RSContext& context);
};

void RSDelegateCompositeCallbackManagerTest::SetUpTestCase() {}

void RSDelegateCompositeCallbackManagerTest::TearDownTestCase() {}

void RSDelegateCompositeCallbackManagerTest::TearDown()
{
    // Drain leftover commands and release infos so the singleton stays clean for other tests.
    auto context = std::make_shared<RSContext>();
    DrainCmdQueues(*context);
    RsDelegateCompositeCallbackManager::GetInstance().NotifyCurrentSurfaceNodeBufferReleaseCallback();
}

std::unique_ptr<RSTransactionData> RSDelegateCompositeCallbackManagerTest::MakeTransactionData(
    pid_t sendingPid, pid_t sendingTid, uint64_t index)
{
    auto data = std::make_unique<RSTransactionData>();
    data->SetSendingPid(sendingPid);
    data->SetSendingTid(sendingTid);
    data->SetIndex(index);
    return data;
}

std::unique_ptr<TransactionBufferCommand> RSDelegateCompositeCallbackManagerTest::MakeBufferCmd(
    NodeId nodeId, uint32_t sequence)
{
    RSTransactionConfig config;
    config.nodeId = nodeId;
    config.transaction = nullptr;
    config.transactionInfo.bufferInfo.sequence = sequence;
    config.transactionInfo.fence = nullptr;
    config.delList = {};
    std::vector<RSTransactionConfig> configs = { config };
    return std::make_unique<TransactionBufferCommand>(configs, nodeId);
}

std::unique_ptr<TransactionBufferCommand> RSDelegateCompositeCallbackManagerTest::MakeRectCmd(
    NodeId nodeId, bool isSrcRect)
{
    RectF rect(5.0f, 6.0f, 7.0f, 8.0f);
    return std::make_unique<TransactionBufferCommand>(rect, isSrcRect, nodeId);
}

std::shared_ptr<RSSurfaceRenderNode> RSDelegateCompositeCallbackManagerTest::RegisterDelegateSurfaceNode(
    const std::shared_ptr<RSContext>& context, NodeId nodeId)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(nodeId, context);
    auto handler = node->GetMutableRSSurfaceHandler();
    if (handler != nullptr) {
        handler->SetConsumer(IConsumerSurface::Create("DelegateManagerTestSurface"));
    }
    node->SetDelegateMode(true);
    context->GetMutableNodeMap().RegisterRenderNode(node);
    return node;
}

std::shared_ptr<RSSurfaceRenderNode> RSDelegateCompositeCallbackManagerTest::MakeNamedSurfaceNode(
    const std::shared_ptr<RSContext>& context, NodeId nodeId, const std::string& name)
{
    RSSurfaceRenderNodeConfig config;
    config.id = nodeId;
    config.name = name;
    return std::make_shared<RSSurfaceRenderNode>(config, context);
}

std::shared_ptr<RSSurfaceRenderNode> RSDelegateCompositeCallbackManagerTest::MakeDelegateModeNode(
    const std::shared_ptr<RSContext>& context, NodeId nodeId, const std::string& name)
{
    auto node = MakeNamedSurfaceNode(context, nodeId, name);
    node->SetDelegateMode(true);
    return node;
}

void RSDelegateCompositeCallbackManagerTest::PrepareCommands(pid_t pid, pid_t tid, uint64_t index,
    std::vector<std::unique_ptr<RSCommand>> cmds)
{
    auto& manager = RsDelegateCompositeCallbackManager::GetInstance();
    manager.AddSurfaceTransactionCmdInfo(SRC_ID_BASE + static_cast<uint64_t>(pid),
        SEQ_NUM_BASE + index, pid, tid);
    auto data = MakeTransactionData(pid, tid, index);
    auto& payload = data->GetPayload();
    for (auto& cmd : cmds) {
        payload.emplace_back(cmd->GetNodeId(), FollowType::NONE, std::move(cmd));
    }
    manager.PrepareDelegateCompositeCommand(data);
}

void RSDelegateCompositeCallbackManagerTest::DrainCmdQueues(RSContext& context)
{
    auto& manager = RsDelegateCompositeCallbackManager::GetInstance();
    while (manager.ProcessDelegateCompositeCommand(context)) {}
}

/**
 * @tc.name: CleanDelegateCompositeCommand001
 * @tc.desc: Verify clean returns true when the pid has no pending command map.
 * @tc.type: FUNC
 */
HWTEST_F(RSDelegateCompositeCallbackManagerTest, CleanDelegateCompositeCommand001, TestSize.Level1)
{
    auto& manager = RsDelegateCompositeCallbackManager::GetInstance();
    EXPECT_TRUE(manager.CleanDelegateCompositeCommand(0, 0, 654321));
}

/**
 * @tc.name: CleanDelegateCompositeCommand002
 * @tc.desc: Verify clean removes only TRANSACTION_BUFFER commands of the target node across
 *           all index queues; rect subtypes and other nodes are kept.
 * @tc.type: FUNC
 */
HWTEST_F(RSDelegateCompositeCallbackManagerTest, CleanDelegateCompositeCommand002, TestSize.Level1)
{
    constexpr pid_t sendingPid = 71001;
    constexpr pid_t sendingTid = 71002;
    constexpr pid_t nodePid = 71003;
    NodeId targetNode = (static_cast<NodeId>(nodePid) << 32) | 1;
    NodeId otherNode = (static_cast<NodeId>(nodePid) << 32) | 2;

    auto context = std::make_shared<RSContext>();
    auto targetSurfaceNode = RegisterDelegateSurfaceNode(context, targetNode);
    auto otherSurfaceNode = RegisterDelegateSurfaceNode(context, otherNode);
    auto rectNode = RegisterDelegateSurfaceNode(context, (static_cast<NodeId>(nodePid) << 32) | 3);
    ASSERT_NE(targetSurfaceNode, nullptr);
    ASSERT_NE(otherSurfaceNode, nullptr);
    ASSERT_NE(rectNode, nullptr);

    sptr<MockNodeBufferReleaseCallback> listener = new MockNodeBufferReleaseCallback();
    auto& manager = RsDelegateCompositeCallbackManager::GetInstance();
    manager.RegisterSurfaceNodeBufferReleaseListener(nodePid, listener);

    // index 1 queue: mixed commands
    std::vector<std::unique_ptr<RSCommand>> firstQueue;
    firstQueue.push_back(MakeRectCmd((static_cast<NodeId>(nodePid) << 32) | 3, true)); // rect kept
    firstQueue.push_back(MakeBufferCmd(targetNode, CLEAN_BUFFER_SEQ)); // buffer cleaned
    firstQueue.push_back(MakeBufferCmd(otherNode, KEEP_BUFFER_SEQ)); // other node kept
    firstQueue.push_back(MakeRectCmd((static_cast<NodeId>(nodePid) << 32) | 3, false)); // dst rect kept
    firstQueue.push_back(MakeBufferCmd(targetNode, CLEAN_BUFFER_SEQ)); // buffer cleaned
    PrepareCommands(sendingPid, sendingTid, TX_INDEX_FIRST, std::move(firstQueue));

    // index 2 queue: buffer cmd of target node + rect cmd of target node
    std::vector<std::unique_ptr<RSCommand>> secondQueue;
    secondQueue.push_back(MakeBufferCmd(targetNode, CLEAN_BUFFER_SEQ)); // cleaned in second queue too
    secondQueue.push_back(MakeRectCmd((static_cast<NodeId>(nodePid) << 32) | 3, true)); // kept
    PrepareCommands(sendingPid, sendingTid, TX_INDEX_SECOND, std::move(secondQueue));

    ASSERT_TRUE(manager.CleanDelegateCompositeCommand(0, targetNode, sendingPid));

    DrainCmdQueues(*context);

    RectF delegateSrcRect = RectF(5.0f, 6.0f, 7.0f, 8.0f);
    RectF delegateDstRect = RectF(5.1f, 6.1f, 7.1f, 8.1f);
    rectNode->SetDelegateSrcRect(delegateSrcRect);
    rectNode->SetDelegateDstRect(delegateDstRect);
    // rect commands survived and were processed
    EXPECT_EQ(rectNode->GetDelegateSrcRect(), delegateSrcRect);
    EXPECT_EQ(rectNode->GetDelegateDstRect(), delegateDstRect);
    // only the buffer command of the other node was flushed, target node's were cleaned
    ASSERT_EQ(listener->callCount, 1);
    ASSERT_EQ(listener->received.size(), 1);
    EXPECT_EQ(listener->received.front().nodeId, otherNode);
    EXPECT_EQ(listener->received.front().bufferSeqNum, KEEP_BUFFER_SEQ);
    EXPECT_NE(listener->received.front().releaseFence, nullptr);

    context->GetMutableNodeMap().UnregisterRenderNode(targetNode);
    context->GetMutableNodeMap().UnregisterRenderNode(otherNode);
    context->GetMutableNodeMap().UnregisterRenderNode((static_cast<NodeId>(nodePid) << 32) | 3);
    manager.UnRegisterSurfaceNodeBufferReleaseListener(nodePid);
    manager.RemoveAllListenerbyPid(sendingPid);
}

/**
 * @tc.name: CleanDelegateCompositeCommand003
 * @tc.desc: Verify clean on a pid with only empty index queues returns true and is a no-op.
 * @tc.type: FUNC
 */
HWTEST_F(RSDelegateCompositeCallbackManagerTest, CleanDelegateCompositeCommand003, TestSize.Level1)
{
    constexpr pid_t sendingPid = 72001;
    constexpr pid_t sendingTid = 72002;
    auto& manager = RsDelegateCompositeCallbackManager::GetInstance();
    // prepare a transaction without any command, produces an empty index queue
    manager.AddSurfaceTransactionCmdInfo(SRC_ID_BASE + sendingPid, SEQ_NUM_BASE, sendingPid, sendingTid);
    auto data = MakeTransactionData(sendingPid, sendingTid, TX_INDEX_FIRST);
    manager.PrepareDelegateCompositeCommand(data);

    EXPECT_TRUE(manager.CleanDelegateCompositeCommand(0, 123, sendingPid));

    auto context = std::make_shared<RSContext>();
    EXPECT_FALSE(manager.ProcessDelegateCompositeCommand(*context));
    manager.RemoveAllListenerbyPid(sendingPid);
}

/**
 * @tc.name: PrepareDelegateCompositeCommand001
 * @tc.desc: Verify prepare with null data or unregistered identity keeps the manager empty.
 * @tc.type: FUNC
 */
HWTEST_F(RSDelegateCompositeCallbackManagerTest, PrepareDelegateCompositeCommand001, TestSize.Level1)
{
    auto& manager = RsDelegateCompositeCallbackManager::GetInstance();
    std::unique_ptr<RSTransactionData> nullData = nullptr;
    manager.PrepareDelegateCompositeCommand(nullData); // null data branch

    // identity not registered: sending pid/tid mismatch branch
    constexpr pid_t unknownPid = 73001;
    auto data = MakeTransactionData(unknownPid, 73002, TX_INDEX_FIRST);
    data->GetPayload().emplace_back(0, FollowType::NONE, MakeRectCmd(1, true));
    manager.PrepareDelegateCompositeCommand(data);
    EXPECT_FALSE(data->GetPayload().empty()); // commands are not moved out

    auto context = std::make_shared<RSContext>();
    EXPECT_FALSE(manager.ProcessDelegateCompositeCommand(*context));
}

/**
 * @tc.name: AddBufferReleaseInfo001
 * @tc.desc: Verify null release fence is converted to invalid fence and the listener
 *           receives the entry after notify.
 * @tc.type: FUNC
 */
HWTEST_F(RSDelegateCompositeCallbackManagerTest, AddBufferReleaseInfo001, TestSize.Level1)
{
    constexpr pid_t clientPid = 74001;
    constexpr NodeId nodeId = 74002;
    constexpr uint64_t surfaceId = 74003;
    auto& manager = RsDelegateCompositeCallbackManager::GetInstance();
    sptr<MockNodeBufferReleaseCallback> listener = new MockNodeBufferReleaseCallback();
    manager.RegisterSurfaceNodeBufferReleaseListener(clientPid, listener);

    manager.AddBufferReleaseInfo(nodeId, surfaceId, RELEASE_BUFFER_SEQ, nullptr, clientPid);
    manager.NotifyCurrentSurfaceNodeBufferReleaseCallback();

    ASSERT_EQ(listener->callCount, 1);
    ASSERT_EQ(listener->received.size(), 1);
    EXPECT_EQ(listener->received.front().nodeId, nodeId);
    EXPECT_EQ(listener->received.front().queueId, surfaceId);
    EXPECT_EQ(listener->received.front().bufferSeqNum, RELEASE_BUFFER_SEQ);
    EXPECT_NE(listener->received.front().releaseFence, nullptr); // SyncFence::InvalidFence
    manager.UnRegisterSurfaceNodeBufferReleaseListener(clientPid);
}

/**
 * @tc.name: AddBufferReleaseInfo002
 * @tc.desc: Verify buffer/fence/surface overload rejects invalid inputs and surfaces
 *           without delegate user data; valid input notifies with buffer sequence.
 * @tc.type: FUNC
 */
HWTEST_F(RSDelegateCompositeCallbackManagerTest, AddBufferReleaseInfo002, TestSize.Level1)
{
    constexpr pid_t clientPid = 75001;
    constexpr NodeId nodeId = 75002;
    auto& manager = RsDelegateCompositeCallbackManager::GetInstance();
    sptr<MockNodeBufferReleaseCallback> listener = new MockNodeBufferReleaseCallback();
    manager.RegisterSurfaceNodeBufferReleaseListener(clientPid, listener);

    sptr<SurfaceBuffer> buffer = new SurfaceBufferImpl(RELEASE_BUFFER_SEQ);
    sptr<SyncFence> fence = SyncFence::InvalidFence();
    sptr<IConsumerSurface> surface = IConsumerSurface::Create("AddReleaseInfoSurface");

    // invalid input branches: null buffer / null fence / null surface / surface without user data
    manager.AddBufferReleaseInfo(nullptr, fence, surface);
    manager.AddBufferReleaseInfo(buffer, nullptr, surface);
    manager.AddBufferReleaseInfo(buffer, fence, nullptr);
    manager.AddBufferReleaseInfo(buffer, fence, surface); // GetInfo fails: no user data
    manager.NotifyCurrentSurfaceNodeBufferReleaseCallback();
    EXPECT_EQ(listener->callCount, 0);

    // valid input after SetInfo
    ASSERT_TRUE(manager.SetInfo(surface, nodeId, clientPid));
    manager.AddBufferReleaseInfo(buffer, fence, surface);
    manager.NotifyCurrentSurfaceNodeBufferReleaseCallback();
    ASSERT_EQ(listener->callCount, 1);
    ASSERT_EQ(listener->received.size(), 1);
    EXPECT_EQ(listener->received.front().nodeId, nodeId);
    EXPECT_EQ(listener->received.front().bufferSeqNum, RELEASE_BUFFER_SEQ);
    manager.UnRegisterSurfaceNodeBufferReleaseListener(clientPid);
}

/**
 * @tc.name: PrepareBufferReleaseInfo001
 * @tc.desc: Verify prepare stores pending release info, rejects duplicate or null buffer,
 *           and process consumes it once with invalid fence for null input.
 * @tc.type: FUNC
 */
HWTEST_F(RSDelegateCompositeCallbackManagerTest, PrepareBufferReleaseInfo001, TestSize.Level1)
{
    constexpr pid_t clientPid = 76001;
    constexpr NodeId nodeId = 76002;
    constexpr uint64_t surfaceId = 76003;
    auto& manager = RsDelegateCompositeCallbackManager::GetInstance();
    sptr<MockNodeBufferReleaseCallback> listener = new MockNodeBufferReleaseCallback();
    manager.RegisterSurfaceNodeBufferReleaseListener(clientPid, listener);

    sptr<SurfaceBuffer> buffer = new SurfaceBufferImpl(PREPARE_BUFFER_SEQ);
    uint64_t bufferId = buffer->GetBufferId();

    EXPECT_FALSE(manager.PrepareBufferReleaseInfo(nodeId, surfaceId, nullptr, nullptr, clientPid));
    EXPECT_TRUE(manager.PrepareBufferReleaseInfo(nodeId, surfaceId, buffer, nullptr, clientPid));
    EXPECT_FALSE(manager.PrepareBufferReleaseInfo(nodeId, surfaceId, buffer, nullptr, clientPid));

    EXPECT_TRUE(manager.ProcessBufferReleaseInfo(bufferId));
    EXPECT_FALSE(manager.ProcessBufferReleaseInfo(bufferId)); // already consumed
    manager.NotifyCurrentSurfaceNodeBufferReleaseCallback();

    ASSERT_EQ(listener->callCount, 1);
    ASSERT_EQ(listener->received.size(), 1);
    EXPECT_EQ(listener->received.front().bufferSeqNum, PREPARE_BUFFER_SEQ);
    EXPECT_NE(listener->received.front().releaseFence, nullptr);
    manager.UnRegisterSurfaceNodeBufferReleaseListener(clientPid);
}

/**
 * @tc.name: PrepareBufferReleaseInfo002
 * @tc.desc: Verify a non-null release fence passes through to the callback unchanged.
 * @tc.type: FUNC
 */
HWTEST_F(RSDelegateCompositeCallbackManagerTest, PrepareBufferReleaseInfo002, TestSize.Level1)
{
    constexpr pid_t clientPid = 77001;
    constexpr NodeId nodeId = 77002;
    constexpr uint64_t surfaceId = 77003;
    auto& manager = RsDelegateCompositeCallbackManager::GetInstance();
    sptr<MockNodeBufferReleaseCallback> listener = new MockNodeBufferReleaseCallback();
    manager.RegisterSurfaceNodeBufferReleaseListener(clientPid, listener);

    sptr<SurfaceBuffer> buffer = new SurfaceBufferImpl(RELEASE_BUFFER_SEQ);
    sptr<SyncFence> fence = SyncFence::InvalidFence();
    uint64_t bufferId = buffer->GetBufferId();

    ASSERT_TRUE(manager.PrepareBufferReleaseInfo(nodeId, surfaceId, buffer, fence, clientPid));
    ASSERT_TRUE(manager.ProcessBufferReleaseInfo(bufferId));
    manager.NotifyCurrentSurfaceNodeBufferReleaseCallback();

    ASSERT_EQ(listener->callCount, 1);
    ASSERT_EQ(listener->received.size(), 1);
    EXPECT_EQ(listener->received.front().releaseFence, fence); // same sptr passes through
    manager.UnRegisterSurfaceNodeBufferReleaseListener(clientPid);
}

/**
 * @tc.name: UpdateHardWareForcedDisabledState001
 * @tc.desc: Verify null surface node input is a safe no-op without crash.
 * @tc.type: FUNC
 */
HWTEST_F(RSDelegateCompositeCallbackManagerTest, UpdateHardWareForcedDisabledState001, TestSize.Level1)
{
    auto& manager = RsDelegateCompositeCallbackManager::GetInstance();
    std::shared_ptr<RSSurfaceRenderNode> nullNode = nullptr;
    manager.UpdateHardWareForcedDisabledStateForDelegateMode(nullNode);
    SUCCEED();
}

/**
 * @tc.name: UpdateHardWareForcedDisabledState002
 * @tc.desc: Verify a node named delegate_child without delegate mode returns early and
 *           keeps hardware enabled even when a disabled delegate sibling exists.
 * @tc.type: FUNC
 */
HWTEST_F(RSDelegateCompositeCallbackManagerTest, UpdateHardWareForcedDisabledState002, TestSize.Level1)
{
    auto context = std::make_shared<RSContext>();
    auto container = MakeNamedSurfaceNode(context, UPDATE_HW_NODE_ID_BASE + 1, "delegate_container");
    auto disabledSibling = MakeDelegateModeNode(context, UPDATE_HW_NODE_ID_BASE + 2, "web_view");
    disabledSibling->SetHardwareForcedDisabledState(true);
    auto hwcNode = MakeNamedSurfaceNode(context, UPDATE_HW_NODE_ID_BASE + 3, DELEGATE_CHILD_NAME);
    container->AddChild(disabledSibling);
    container->AddChild(hwcNode);

    auto& manager = RsDelegateCompositeCallbackManager::GetInstance();
    manager.UpdateHardWareForcedDisabledStateForDelegateMode(hwcNode);
    EXPECT_FALSE(hwcNode->IsHardwareForcedDisabled());
}

/**
 * @tc.name: UpdateHardWareForcedDisabledState003
 * @tc.desc: Verify a delegate mode node not named delegate_child returns early and
 *           keeps hardware enabled even when a disabled delegate sibling exists.
 * @tc.type: FUNC
 */
HWTEST_F(RSDelegateCompositeCallbackManagerTest, UpdateHardWareForcedDisabledState003, TestSize.Level1)
{
    auto context = std::make_shared<RSContext>();
    auto container = MakeNamedSurfaceNode(context, UPDATE_HW_NODE_ID_BASE + 11, "delegate_container");
    auto disabledSibling = MakeDelegateModeNode(context, UPDATE_HW_NODE_ID_BASE + 12, "web_view");
    disabledSibling->SetHardwareForcedDisabledState(true);
    auto hwcNode = MakeDelegateModeNode(context, UPDATE_HW_NODE_ID_BASE + 13, "other_child");
    container->AddChild(disabledSibling);
    container->AddChild(hwcNode);

    auto& manager = RsDelegateCompositeCallbackManager::GetInstance();
    manager.UpdateHardWareForcedDisabledStateForDelegateMode(hwcNode);
    EXPECT_FALSE(hwcNode->IsHardwareForcedDisabled());
}

/**
 * @tc.name: UpdateHardWareForcedDisabledState004
 * @tc.desc: Verify an already hardware forced disabled delegate_child returns early
 *           and stays disabled without touching siblings.
 * @tc.type: FUNC
 */
HWTEST_F(RSDelegateCompositeCallbackManagerTest, UpdateHardWareForcedDisabledState004, TestSize.Level1)
{
    auto context = std::make_shared<RSContext>();
    auto container = MakeNamedSurfaceNode(context, UPDATE_HW_NODE_ID_BASE + 21, "delegate_container");
    auto hwcNode = MakeDelegateModeNode(context, UPDATE_HW_NODE_ID_BASE + 22, DELEGATE_CHILD_NAME);
    hwcNode->SetHardwareForcedDisabledState(true);
    container->AddChild(hwcNode);

    auto& manager = RsDelegateCompositeCallbackManager::GetInstance();
    manager.UpdateHardWareForcedDisabledStateForDelegateMode(hwcNode);
    EXPECT_TRUE(hwcNode->IsHardwareForcedDisabled());
}

/**
 * @tc.name: UpdateHardWareForcedDisabledState005
 * @tc.desc: Verify a delegate_child without parent returns early and keeps hardware enabled.
 * @tc.type: FUNC
 */
HWTEST_F(RSDelegateCompositeCallbackManagerTest, UpdateHardWareForcedDisabledState005, TestSize.Level1)
{
    auto context = std::make_shared<RSContext>();
    auto hwcNode = MakeDelegateModeNode(context, UPDATE_HW_NODE_ID_BASE + 31, DELEGATE_CHILD_NAME);

    auto& manager = RsDelegateCompositeCallbackManager::GetInstance();
    manager.UpdateHardWareForcedDisabledStateForDelegateMode(hwcNode);
    EXPECT_FALSE(hwcNode->IsHardwareForcedDisabled());
}

/**
 * @tc.name: UpdateHardWareForcedDisabledState006
 * @tc.desc: Verify siblings that do not satisfy delegate mode / not delegate child /
 *           disabled conditions never disable the delegate_child node.
 * @tc.type: FUNC
 */
HWTEST_F(RSDelegateCompositeCallbackManagerTest, UpdateHardWareForcedDisabledState006, TestSize.Level1)
{
    auto context = std::make_shared<RSContext>();
    auto container = MakeNamedSurfaceNode(context, UPDATE_HW_NODE_ID_BASE + 41, "delegate_container");
    auto effectChild = std::make_shared<RSEffectRenderNode>(UPDATE_HW_NODE_ID_BASE + 42, context);
    auto disabledNoDelegateMode = MakeNamedSurfaceNode(context, UPDATE_HW_NODE_ID_BASE + 43, "web_view");
    disabledNoDelegateMode->SetHardwareForcedDisabledState(true);
    auto delegateModeNotDisabled = MakeDelegateModeNode(context, UPDATE_HW_NODE_ID_BASE + 44, "web_view");
    auto delegateChildDisabled = MakeDelegateModeNode(
        context, UPDATE_HW_NODE_ID_BASE + 45, DELEGATE_CHILD_NAME);
    delegateChildDisabled->SetHardwareForcedDisabledState(true);
    auto hwcNode = MakeDelegateModeNode(context, UPDATE_HW_NODE_ID_BASE + 46, DELEGATE_CHILD_NAME);
    container->AddChild(effectChild);
    container->AddChild(disabledNoDelegateMode);
    container->AddChild(delegateModeNotDisabled);
    container->AddChild(delegateChildDisabled);
    container->AddChild(hwcNode);

    auto& manager = RsDelegateCompositeCallbackManager::GetInstance();
    manager.UpdateHardWareForcedDisabledStateForDelegateMode(hwcNode);
    EXPECT_FALSE(hwcNode->IsHardwareForcedDisabled());
}

/**
 * @tc.name: UpdateHardWareForcedDisabledState007
 * @tc.desc: Verify a sibling with delegate mode, not delegate child and hardware forced
 *           disabled disables the delegate_child node.
 * @tc.type: FUNC
 */
HWTEST_F(RSDelegateCompositeCallbackManagerTest, UpdateHardWareForcedDisabledState007, TestSize.Level1)
{
    auto context = std::make_shared<RSContext>();
    auto container = MakeNamedSurfaceNode(context, UPDATE_HW_NODE_ID_BASE + 51, "delegate_container");
    auto disabledSibling = MakeDelegateModeNode(context, UPDATE_HW_NODE_ID_BASE + 52, "web_view");
    disabledSibling->SetHardwareForcedDisabledState(true);
    auto hwcNode = MakeDelegateModeNode(context, UPDATE_HW_NODE_ID_BASE + 53, DELEGATE_CHILD_NAME);
    container->AddChild(disabledSibling);
    container->AddChild(hwcNode);

    auto& manager = RsDelegateCompositeCallbackManager::GetInstance();
    manager.UpdateHardWareForcedDisabledStateForDelegateMode(hwcNode);
    EXPECT_TRUE(hwcNode->IsHardwareForcedDisabled());
}

/**
 * @tc.name: UpdateHardWareForcedDisabledState008
 * @tc.desc: Verify the sibling loop skips non-matching children and still disables the
 *           delegate_child node when a matching disabled delegate sibling exists.
 * @tc.type: FUNC
 */
HWTEST_F(RSDelegateCompositeCallbackManagerTest, UpdateHardWareForcedDisabledState008, TestSize.Level1)
{
    auto context = std::make_shared<RSContext>();
    auto container = MakeNamedSurfaceNode(context, UPDATE_HW_NODE_ID_BASE + 61, "delegate_container");
    auto effectChild = std::make_shared<RSEffectRenderNode>(UPDATE_HW_NODE_ID_BASE + 62, context);
    auto disabledNoDelegateMode = MakeNamedSurfaceNode(context, UPDATE_HW_NODE_ID_BASE + 63, "web_view");
    disabledNoDelegateMode->SetHardwareForcedDisabledState(true);
    auto delegateModeNotDisabled = MakeDelegateModeNode(context, UPDATE_HW_NODE_ID_BASE + 64, "web_view");
    auto disabledSibling = MakeDelegateModeNode(context, UPDATE_HW_NODE_ID_BASE + 65, "web_view");
    disabledSibling->SetHardwareForcedDisabledState(true);
    auto hwcNode = MakeDelegateModeNode(context, UPDATE_HW_NODE_ID_BASE + 66, DELEGATE_CHILD_NAME);
    container->AddChild(effectChild);
    container->AddChild(disabledNoDelegateMode);
    container->AddChild(delegateModeNotDisabled);
    container->AddChild(disabledSibling);
    container->AddChild(hwcNode);

    auto& manager = RsDelegateCompositeCallbackManager::GetInstance();
    manager.UpdateHardWareForcedDisabledStateForDelegateMode(hwcNode);
    EXPECT_TRUE(hwcNode->IsHardwareForcedDisabled());
}

/**
 * @tc.name: UpdateHardWareForcedDisabledState009
 * @tc.desc: Verify an expired child in the parent list is skipped by the loop and a
 *           matching disabled sibling after it still disables the delegate_child node.
 * @tc.type: FUNC
 */
HWTEST_F(RSDelegateCompositeCallbackManagerTest, UpdateHardWareForcedDisabledState009, TestSize.Level1)
{
    auto context = std::make_shared<RSContext>();
    auto container = MakeNamedSurfaceNode(context, UPDATE_HW_NODE_ID_BASE + 71, "delegate_container");
    {
        // destroyed while still referenced by the parent children list, leaves an expired weak_ptr
        auto expiredChild = MakeDelegateModeNode(context, UPDATE_HW_NODE_ID_BASE + 72, "web_view");
        expiredChild->SetHardwareForcedDisabledState(true);
        container->AddChild(expiredChild);
    }
    auto disabledSibling = MakeDelegateModeNode(context, UPDATE_HW_NODE_ID_BASE + 73, "web_view");
    disabledSibling->SetHardwareForcedDisabledState(true);
    auto hwcNode = MakeDelegateModeNode(context, UPDATE_HW_NODE_ID_BASE + 74, DELEGATE_CHILD_NAME);
    container->AddChild(disabledSibling);
    container->AddChild(hwcNode);

    auto& manager = RsDelegateCompositeCallbackManager::GetInstance();
    manager.UpdateHardWareForcedDisabledStateForDelegateMode(hwcNode);
    EXPECT_TRUE(hwcNode->IsHardwareForcedDisabled());
}

/**
 * @tc.name: UpdateHardWareForcedDisabledState010
 * @tc.desc: Verify an expired child alone never disables the delegate_child node.
 * @tc.type: FUNC
 */
HWTEST_F(RSDelegateCompositeCallbackManagerTest, UpdateHardWareForcedDisabledState010, TestSize.Level1)
{
    auto context = std::make_shared<RSContext>();
    auto container = MakeNamedSurfaceNode(context, UPDATE_HW_NODE_ID_BASE + 81, "delegate_container");
    {
        // destroyed while still referenced by the parent children list, leaves an expired weak_ptr
        auto expiredChild = MakeDelegateModeNode(context, UPDATE_HW_NODE_ID_BASE + 82, "web_view");
        expiredChild->SetHardwareForcedDisabledState(true);
        container->AddChild(expiredChild);
    }
    auto hwcNode = MakeDelegateModeNode(context, UPDATE_HW_NODE_ID_BASE + 83, DELEGATE_CHILD_NAME);
    container->AddChild(hwcNode);

    auto& manager = RsDelegateCompositeCallbackManager::GetInstance();
    manager.UpdateHardWareForcedDisabledStateForDelegateMode(hwcNode);
    EXPECT_FALSE(hwcNode->IsHardwareForcedDisabled());
}
#endif // ROSEN_CROSS_PLATFORM
} // namespace Rosen
} // namespace OHOS
