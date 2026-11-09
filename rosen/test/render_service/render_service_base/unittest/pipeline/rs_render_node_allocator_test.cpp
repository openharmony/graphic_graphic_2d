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
#include <vector>
 
#include "gtest/gtest.h"

#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_render_node_allocator.h"
#include "pipeline/rs_render_node_gc.h"
#include "pipeline/rs_scb_node_memory_pool.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSRenderNodeAllocatorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderNodeAllocatorTest::SetUpTestCase() {}
void RSRenderNodeAllocatorTest::TearDownTestCase() {}
void RSRenderNodeAllocatorTest::SetUp() {}
void RSRenderNodeAllocatorTest::TearDown() {}

/**
 * @tc.name: CreateRSCanvasRenderNodeTest
 * @tc.desc: test CreateRSCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueICD9PG
 */
HWTEST_F(RSRenderNodeAllocatorTest, CreateRSCanvasRenderNodeTest, TestSize.Level1)
{
    auto& nodeAllocator = RSRenderNodeAllocator::Instance();
    auto node = nodeAllocator.CreateRSCanvasRenderNode(0);
    ASSERT_NE(node, nullptr);

    nodeAllocator.AddNodeToAllocator(node.get());
    auto newNode = nodeAllocator.CreateRSCanvasRenderNode(1);
    ASSERT_NE(newNode, nullptr);
}

/**
 * @tc.name: AddNodeToAllocatorTest
 * @tc.desc: test AddNodeToAllocator
 * @tc.type: FUNC
 * @tc.require: issueICD9PG
 */
HWTEST_F(RSRenderNodeAllocatorTest, AddNodeToAllocatorTest, TestSize.Level1)
{
    auto& nodeAllocator = RSRenderNodeAllocator::Instance();
    auto nodePtr = std::make_shared<RSRenderNode>(0);
    EXPECT_FALSE(nodeAllocator.AddNodeToAllocator(nodePtr.get()));

    auto canvasNodePtr = nodeAllocator.CreateRSCanvasRenderNode(0);
    EXPECT_TRUE(nodeAllocator.AddNodeToAllocator(canvasNodePtr.get()));

    auto newCanvasNodePtr = nodeAllocator.CreateRSCanvasRenderNode(1);
    for (int i = 0; i < 512; i++) {
        auto ptr = std::make_shared<RSCanvasRenderNode>(i);
        nodeAllocator.nodeAllocator_.push(ptr.get());
    }
    EXPECT_FALSE(nodeAllocator.AddNodeToAllocator(newCanvasNodePtr.get()));
}

/**
 * @tc.name: CreateRSCanvasRenderNodeTest002
 * @tc.desc: test CreateRSCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueICD9PG
 */
HWTEST_F(RSRenderNodeAllocatorTest, CreateRSCanvasRenderNodeTest002, TestSize.Level1)
{
    auto& nodeAllocator = RSRenderNodeAllocator::Instance();
    auto node = nodeAllocator.CreateRSCanvasRenderNode(0);
    ASSERT_NE(node, nullptr);

    nodeAllocator.AddNodeToAllocator(node.get());
    auto newNode = nodeAllocator.CreateRSCanvasRenderNode(1);
    ASSERT_NE(newNode, nullptr);
}

/**
 * @tc.name: CreateScbCanvasRenderNodeTest001
 * @tc.desc: test CreateScbCanvasRenderNode returns a valid node allocated from the SCB pool
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeAllocatorTest, CreateScbCanvasRenderNodeTest001, TestSize.Level1)
{
    auto& nodeAllocator = RSRenderNodeAllocator::Instance();
    auto node = nodeAllocator.CreateScbCanvasRenderNode(0);
    ASSERT_NE(node, nullptr);
    // Node should be allocated from the SCB contiguous memory pool
    EXPECT_TRUE(nodeAllocator.IsScbNodePool(node.get()));
}

/**
 * @tc.name: CreateScbCanvasRenderNodeTest002
 * @tc.desc: test CreateScbCanvasRenderNode reuses a node from the SCB free queue when available.
 *           After recycling a node via AddScbNodeToPool, the next create should reuse it
 *           (queue size decreases and pool free slot count stays the same).
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeAllocatorTest, CreateScbCanvasRenderNodeTest002, TestSize.Level1)
{
    auto& nodeAllocator = RSRenderNodeAllocator::Instance();
    // Create a node from the pool, then recycle it to the free queue
    auto node = nodeAllocator.CreateScbCanvasRenderNode(1001);
    ASSERT_NE(node, nullptr);
    EXPECT_TRUE(nodeAllocator.IsScbNodePool(node.get()));
    RSCanvasRenderNode* rawPtr = node.get();
    // Replace deleter with a no-op before recycling: AddScbNodeToPool destructs
    // the node, so the shared_ptr must not invoke NodeDestructor again on reset.
    node = std::shared_ptr<RSCanvasRenderNode>(node.get(), [](RSCanvasRenderNode*) {});
    // Recycle: this destructs the node and pushes the raw pointer to scbNodeFreeQueue_
    EXPECT_TRUE(nodeAllocator.AddScbNodeToPool(rawPtr));
    node.reset(); // no-op deleter won't fire, prevents double-destruct
    size_t queueBefore = nodeAllocator.GetScbFreeQueueSize();
    EXPECT_GT(queueBefore, 0u);
    // Next create should pull from the free queue
    auto reused = nodeAllocator.CreateScbCanvasRenderNode(1002);
    ASSERT_NE(reused, nullptr);
    EXPECT_TRUE(nodeAllocator.IsScbNodePool(reused.get()));
    EXPECT_EQ(nodeAllocator.GetScbFreeQueueSize(), queueBefore - 1);
    // The reused node should occupy the same memory slot
    EXPECT_EQ(reused.get(), rawPtr);
}

/**
 * @tc.name: CreateScbCanvasRenderNodeTest003
 * @tc.desc: test CreateScbCanvasRenderNode fallback to heap allocation when the pool is exhausted
 *           (mem == nullptr branch). Force pool exhaustion by draining free slots beyond MAX_SLOTS.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeAllocatorTest, CreateScbCanvasRenderNodeTest003, TestSize.Level1)
{
    auto& nodeAllocator = RSRenderNodeAllocator::Instance();
    auto& pool = RSScbNodeMemoryPool::Instance();
    // Drain the pool completely: allocate all available slots
    std::vector<void*> drained;
    while (pool.GetFreeSlotCount() > 0) {
        void* p = pool.Allocate();
        if (p == nullptr) {
            break;
        }
        drained.push_back(p);
    }
    // Now freeSlots_ is empty and Expand has hit MAX_SLOTS, so Allocate returns nullptr
    // and CreateScbCanvasRenderNode should fall back to heap new.
    // Note: if the pool hasn't reached MAX_SLOTS, Allocate would still expand.
    // To force the fallback, we drain until Allocate returns nullptr.
    while (true) {
        void* p = pool.Allocate();
        if (p == nullptr) {
            break;
        }
        drained.push_back(p);
    }
    auto node = nodeAllocator.CreateScbCanvasRenderNode(2001);
    ASSERT_NE(node, nullptr);
    // Heap fallback node is NOT owned by the SCB pool
    EXPECT_FALSE(nodeAllocator.IsScbNodePool(node.get()));
    // Cleanup: return drained slots to the pool
    for (void* p : drained) {
        pool.Deallocate(p);
    }
}

/**
 * @tc.name: AddScbNodeToPoolTest001
 * @tc.desc: test AddScbNodeToPool returns false for nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeAllocatorTest, AddScbNodeToPoolTest001, TestSize.Level1)
{
    auto& nodeAllocator = RSRenderNodeAllocator::Instance();
    EXPECT_FALSE(nodeAllocator.AddScbNodeToPool(nullptr));
}

/**
 * @tc.name: AddScbNodeToPoolTest002
 * @tc.desc: test AddScbNodeToPool returns false for a non-CANVAS node (wrong type)
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeAllocatorTest, AddScbNodeToPoolTest002, TestSize.Level1)
{
    auto& nodeAllocator = RSRenderNodeAllocator::Instance();
    auto baseNode = std::make_shared<RSRenderNode>(0);
    EXPECT_FALSE(nodeAllocator.AddScbNodeToPool(baseNode.get()));
}

/**
 * @tc.name: AddScbNodeToPoolTest003
 * @tc.desc: test AddScbNodeToPool returns false when the node is not from the SCB pool
 *           (Owns returns false). A heap-allocated canvas node is not pool-owned.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeAllocatorTest, AddScbNodeToPoolTest003, TestSize.Level1)
{
    auto& nodeAllocator = RSRenderNodeAllocator::Instance();
    auto heapNode = std::make_shared<RSCanvasRenderNode>(0);
    EXPECT_FALSE(nodeAllocator.IsScbNodePool(heapNode.get()));
    EXPECT_FALSE(nodeAllocator.AddScbNodeToPool(heapNode.get()));
}

/**
 * @tc.name: AddScbNodeToPoolTest004
 * @tc.desc: test AddScbNodeToPool returns true for a valid SCB-pool canvas node and
 *           increases the free queue size.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeAllocatorTest, AddScbNodeToPoolTest004, TestSize.Level1)
{
    auto& nodeAllocator = RSRenderNodeAllocator::Instance();
    auto node = nodeAllocator.CreateScbCanvasRenderNode(3001);
    ASSERT_NE(node, nullptr);
    EXPECT_TRUE(nodeAllocator.IsScbNodePool(node.get()));
    size_t queueBefore = nodeAllocator.GetScbFreeQueueSize();
    // Replace deleter with no-op: AddScbNodeToPool destructs the node itself
    RSCanvasRenderNode* raw = node.get();
    node = std::shared_ptr<RSCanvasRenderNode>(raw, [](RSCanvasRenderNode*) {});
    EXPECT_TRUE(nodeAllocator.AddScbNodeToPool(raw));
    EXPECT_EQ(nodeAllocator.GetScbFreeQueueSize(), queueBefore + 1);
    node.reset(); // no-op deleter won't fire, prevents double-destruct
}

/**
 * @tc.name: AddScbNodeToPoolTest005
 * @tc.desc: test AddScbNodeToPool returns false when the SCB free queue is full (>= limit).
 *           Fill the queue to its limit and verify the next recycle fails.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeAllocatorTest, AddScbNodeToPoolTest005, TestSize.Level1)
{
    auto& nodeAllocator = RSRenderNodeAllocator::Instance();
    // Drain any existing free queue entries first by creating nodes
    std::vector<std::shared_ptr<RSCanvasRenderNode>> holders;
    while (nodeAllocator.GetScbFreeQueueSize() > 0) {
        holders.push_back(nodeAllocator.CreateScbCanvasRenderNode(0));
    }
    // Now create and recycle nodes until the queue is full
    constexpr uint32_t queueLimit = 32; // SCB_NODE_FREE_QUEUE_LIMIT
    std::vector<RSCanvasRenderNode*> recycled;
    for (uint32_t i = 0; i < queueLimit; ++i) {
        auto node = nodeAllocator.CreateScbCanvasRenderNode(4000 + i);
        ASSERT_NE(node, nullptr);
        RSCanvasRenderNode* raw = node.get();
        node = std::shared_ptr<RSCanvasRenderNode>(raw, [](RSCanvasRenderNode*) {});
        ASSERT_TRUE(nodeAllocator.AddScbNodeToPool(raw));
        node.reset(); // no-op deleter won't fire, already destructed by AddScbNodeToPool
        recycled.push_back(raw);
    }
    EXPECT_EQ(nodeAllocator.GetScbFreeQueueSize(), queueLimit);
    // One more recycle should fail (queue full)
    auto extra = nodeAllocator.CreateScbCanvasRenderNode(4999);
    ASSERT_NE(extra, nullptr);
    EXPECT_FALSE(nodeAllocator.AddScbNodeToPool(extra.get()));
    // Cleanup: drain the queue by creating nodes (which pulls from the free queue)
    while (nodeAllocator.GetScbFreeQueueSize() > 0) {
        nodeAllocator.CreateScbCanvasRenderNode(0);
    }
}

/**
 * @tc.name: CreateScbCanvasRenderNodeMemClear001
 * @tc.desc: test CreateScbCanvasRenderNode takes the IsNodeMemClearEnable()==true branch and
 *           registers the node into RSRenderNodeGC::notOnTreeNodeMap_ via SetIsOnTheTree.
 *           IsNodeMemClearEnable() depends on build-time statics (NOT_BUILD_FOR_OHOS_SDK,
 *           IS_UNI_RENDER) plus runtime flags; the test forces the runtime flag and skips
 *           gracefully when the build has the feature compiled out.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeAllocatorTest, CreateScbCanvasRenderNodeMemClear001, TestSize.Level1)
{
    auto& nodeAllocator = RSRenderNodeAllocator::Instance();
    auto& nodeGC = RSRenderNodeGC::Instance();
    bool originalOffTree = nodeGC.IsNodeOffTreeMemReleaseEnabled();
    pid_t originalScbPid = nodeGC.GetScbPid();
    nodeGC.SetNodeOffTreeMemReleaseEnabled(true);
    // Ensure the node pid differs from scbPid_; SetIsOnTheTree early-returns on a match.
    nodeGC.SetScbPid(-1);

    constexpr pid_t pid = 11;
    NodeId id = (static_cast<NodeId>(pid) << 32) | 1;
    auto node = nodeAllocator.CreateScbCanvasRenderNode(id, {}, false);
    ASSERT_NE(node, nullptr);
    ASSERT_TRUE(nodeAllocator.IsScbNodePool(node.get()));

    if (node->IsNodeMemClearEnable()) {
        // Branch taken: the node must have been registered as not-on-tree.
        auto it = nodeGC.notOnTreeNodeMap_.find(pid);
        ASSERT_NE(it, nodeGC.notOnTreeNodeMap_.end());
        EXPECT_NE(it->second.find(id), it->second.end());
        // Cleanup the registration
        nodeGC.notOnTreeNodeMap_.erase(it);
    } else {
        // Build has the mem-clear feature compiled out (e.g. OHOS SDK build):
        // the SetIsOnTheTree branch is unreachable here.
        SUCCEED();
    }
    nodeGC.SetNodeOffTreeMemReleaseEnabled(originalOffTree);
    nodeGC.SetScbPid(originalScbPid);
}

/**
 * @tc.name: CreateRSCanvasRenderNodeMemClear001
 * @tc.desc: test CreateRSCanvasRenderNode (generic path) takes the IsNodeMemClearEnable()==true
 *           branch and registers the node via SetIsOnTheTree. Skips gracefully when the
 *           build compiles the mem-clear feature out.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeAllocatorTest, CreateRSCanvasRenderNodeMemClear001, TestSize.Level1)
{
    auto& nodeAllocator = RSRenderNodeAllocator::Instance();
    auto& nodeGC = RSRenderNodeGC::Instance();
    bool originalOffTree = nodeGC.IsNodeOffTreeMemReleaseEnabled();
    pid_t originalScbPid = nodeGC.GetScbPid();
    nodeGC.SetNodeOffTreeMemReleaseEnabled(true);
    nodeGC.SetScbPid(-1);

    constexpr pid_t pid = 12;
    NodeId id = (static_cast<NodeId>(pid) << 32) | 1;
    auto node = nodeAllocator.CreateRSCanvasRenderNode(id, {}, false);
    ASSERT_NE(node, nullptr);

    if (node->IsNodeMemClearEnable()) {
        auto it = nodeGC.notOnTreeNodeMap_.find(pid);
        ASSERT_NE(it, nodeGC.notOnTreeNodeMap_.end());
        EXPECT_NE(it->second.find(id), it->second.end());
        nodeGC.notOnTreeNodeMap_.erase(it);
    } else {
        SUCCEED();
    }
    nodeGC.SetNodeOffTreeMemReleaseEnabled(originalOffTree);
    nodeGC.SetScbPid(originalScbPid);
}

/**
 * @tc.name: IsScbNodePoolTest001
 * @tc.desc: test IsScbNodePool returns true for a SCB-pool node and false for a heap node
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeAllocatorTest, IsScbNodePoolTest001, TestSize.Level1)
{
    auto& nodeAllocator = RSRenderNodeAllocator::Instance();
    auto scbNode = nodeAllocator.CreateScbCanvasRenderNode(5001);
    ASSERT_NE(scbNode, nullptr);
    EXPECT_TRUE(nodeAllocator.IsScbNodePool(scbNode.get()));
    auto heapNode = std::make_shared<RSCanvasRenderNode>(0);
    EXPECT_FALSE(nodeAllocator.IsScbNodePool(heapNode.get()));
}

/**
 * @tc.name: IsScbNodePoolTest002
 * @tc.desc: test IsScbNodePool returns false for nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeAllocatorTest, IsScbNodePoolTest002, TestSize.Level1)
{
    auto& nodeAllocator = RSRenderNodeAllocator::Instance();
    EXPECT_FALSE(nodeAllocator.IsScbNodePool(nullptr));
}

/**
 * @tc.name: GetScbFreeQueueSizeTest001
 * @tc.desc: test GetScbFreeQueueSize reflects recycle and reuse operations
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeAllocatorTest, GetScbFreeQueueSizeTest001, TestSize.Level1)
{
    auto& nodeAllocator = RSRenderNodeAllocator::Instance();
    // Drain existing queue
    std::vector<std::shared_ptr<RSCanvasRenderNode>> holders;
    while (nodeAllocator.GetScbFreeQueueSize() > 0) {
        holders.push_back(nodeAllocator.CreateScbCanvasRenderNode(0));
    }
    EXPECT_EQ(nodeAllocator.GetScbFreeQueueSize(), 0u);
    // Recycle a node -> queue grows
    auto node = nodeAllocator.CreateScbCanvasRenderNode(6001);
    ASSERT_NE(node, nullptr);
    RSCanvasRenderNode* raw = node.get();
    node = std::shared_ptr<RSCanvasRenderNode>(raw, [](RSCanvasRenderNode*) {});
    EXPECT_TRUE(nodeAllocator.AddScbNodeToPool(raw));
    node.reset(); // no-op deleter won't fire
    EXPECT_EQ(nodeAllocator.GetScbFreeQueueSize(), 1u);
    // Next create pulls from the queue -> queue shrinks
    auto reused = nodeAllocator.CreateScbCanvasRenderNode(6002);
    ASSERT_NE(reused, nullptr);
    EXPECT_EQ(nodeAllocator.GetScbFreeQueueSize(), 0u);
}
} // namespace Rosen
} // namespace OHOS