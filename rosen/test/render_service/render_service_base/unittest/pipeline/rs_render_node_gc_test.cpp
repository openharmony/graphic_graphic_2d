/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include <atomic>
#include <thread>

#include "gtest/gtest.h"

#include "animation/rs_render_curve_animation.h"
#include "pipeline/rs_render_node_allocator.h"
#include "pipeline/rs_render_node_gc.h"
#include "drawable/rs_render_node_shadow_drawable.h"
#include "platform/common/rs_system_properties.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class ConcreteRSRenderNodeDrawableAdapter : public DrawableV2::RSRenderNodeDrawableAdapter {
public:
    explicit ConcreteRSRenderNodeDrawableAdapter(std::shared_ptr<const RSRenderNode> node)
        : RSRenderNodeDrawableAdapter(std::move(node))
    {}
    void Draw(Drawing::Canvas& canvas) {}
};

class RSRenderNodeGCTest : public testing::Test {
public:
    typedef void (*GcTask)(RSTaskMessage::RSTask, const std::string&, int64_t,
        AppExecFwk::EventQueue::Priority);
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
private:
    // just flag for test, HIGH for drawableReleaseFunc_, IMMEDIATE for imageReleaseFunc_
    GCLevel taskFlag_ = GCLevel::IDLE;
    void ClearResource();
    void ClearNodeBucket();
    void AddNodes(uint32_t nums, pid_t pid, NodeId startId,
        bool needInvalidNode, std::shared_ptr<RSBaseRenderNode> parent);
    void AddNodes(uint32_t nums, NodeId startId);
    void AddCanvasNodes(uint32_t nums, NodeId startId);
    void ClearOffTreeBucket();
    void ClearOffTreeBucketMap();
    void ClearBgBucket();
};

void RSRenderNodeGCTest::ClearResource()
{
    ClearOffTreeBucket();
    ClearOffTreeBucketMap();
    ClearNodeBucket();
    ClearBgBucket();
}

void RSRenderNodeGCTest::ClearNodeBucket()
{
    auto& nodeGC = RSRenderNodeGC::Instance();
    while (!nodeGC.nodeBucket_.empty()) {
        nodeGC.nodeBucket_.pop();
    }
}

void RSRenderNodeGCTest::ClearOffTreeBucket()
{
    auto& nodeGC = RSRenderNodeGC::Instance();
    while (!nodeGC.offTreeBucket_.empty()) {
        nodeGC.offTreeBucket_.pop();
    }
}

void RSRenderNodeGCTest::ClearOffTreeBucketMap()
{
    auto& nodeGC = RSRenderNodeGC::Instance();
    while (!nodeGC.offTreeBucketMap_.empty()) {
        nodeGC.offTreeBucketMap_.pop();
    }
}

void RSRenderNodeGCTest::ClearBgBucket()
{
    auto& nodeGC = RSRenderNodeGC::Instance();
    while (!nodeGC.nodeBgBucket_.empty()) {
        nodeGC.nodeBgBucket_.pop();
    }
    nodeGC.bgReleasePending_ = false;
}

void RSRenderNodeGCTest::AddNodes(uint32_t nums, pid_t pid, NodeId startId,
    bool needInvalidNode, std::shared_ptr<RSBaseRenderNode> parent)
{
    auto& nodeGC = RSRenderNodeGC::Instance();
    NodeId id;
    std::unordered_map<NodeId, std::shared_ptr<RSBaseRenderNode>> renderNodeMap;
    for (int i = 0; i < nums; i++) {
        id = startId + i;
        if (needInvalidNode && i % 2 == 0) { // 2 means half renderNode is invalid
            renderNodeMap[id] = nullptr;
        } else {
            renderNodeMap[id] = std::make_shared<RSBaseRenderNode>(id);
        }
        if (parent) {
            parent->AddChild(renderNodeMap[id]);
        }
    }
    nodeGC.AddToOffTreeNodeBucket(pid, renderNodeMap);
}

void RSRenderNodeGCTest::AddNodes(uint32_t nums, NodeId startId)
{
    auto& nodeGC = RSRenderNodeGC::Instance();
    for (int i = 0; i < nums; i++) {
        nodeGC.AddToOffTreeNodeBucket(std::make_shared<RSBaseRenderNode>(startId + i));
    }
}

void RSRenderNodeGCTest::AddCanvasNodes(uint32_t nums, NodeId startId)
{
    auto& nodeGC = RSRenderNodeGC::Instance();
    RSRenderNodeAllocator& nodeAllocator = RSRenderNodeAllocator::Instance();
    for (int i = 0; i < nums; i++) {
        const auto& renderNode = nodeAllocator.CreateRSCanvasRenderNode(startId + i);
        nodeGC.AddToOffTreeNodeBucket(renderNode);
        ConcreteRSRenderNodeDrawableAdapter::OnGenerate(renderNode);
    }
}

void RSRenderNodeGCTest::SetUpTestCase()
{
    GcTask testTask = [](RSTaskMessage::RSTask task,
                const std::string& str,
                int64_t value,
                AppExecFwk::EventQueue::Priority priority) {
        task();
    };
    RSRenderNodeGC::Instance().SetMainTask(testTask);
    RSRenderNodeGC::Instance().SetRenderTask(testTask);
}

void RSRenderNodeGCTest::TearDownTestCase() {}
void RSRenderNodeGCTest::SetUp()
{
    ClearResource();
}

void RSRenderNodeGCTest::TearDown()
{
    ClearResource();
}

/**
 * @tc.name: NodeDestructor001
 * @tc.desc: test results of NodeDestructor
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSRenderNodeGCTest, NodeDestructor001, TestSize.Level1)
{
    auto ptr = std::make_shared<RSRenderNode>(0);
    RSRenderNodeGC::NodeDestructor(ptr.get());
    EXPECT_TRUE(ptr != nullptr);
}

/**
 * @tc.name: NodeDestructorInner001
 * @tc.desc: test results of NodeDestructorInner
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSRenderNodeGCTest, NodeDestructorInner001, TestSize.Level1)
{
    RSRenderNodeGC& node = RSRenderNodeGC::Instance();
    node.NodeDestructorInner(nullptr);
    EXPECT_TRUE(node.nodeBucket_.size() == 0);

    auto ptr = std::make_shared<RSRenderNode>(0);
    node.NodeDestructorInner(ptr.get());
    EXPECT_TRUE(ptr != nullptr);

    node.NodeDestructorInner(ptr.get());
    EXPECT_TRUE(ptr != nullptr);
}

/**
 * @tc.name: AddNodeToBucket001
 * @tc.desc: test results of AddNodeToBucket
 * @tc.type: FUNC
 * @tc.require: issue19909
 */
HWTEST_F(RSRenderNodeGCTest, AddNodeToBucket001, TestSize.Level1)
{
    RSRenderNodeGC& gc = RSRenderNodeGC::Instance();
    auto ptr = std::make_shared<RSRenderNode>(0).get();

    // Test adding more than BUCKET_MAX_SIZE nodes
    constexpr int nodeNum = BUCKET_MAX_SIZE + 2;
    for (int i = 0; i < nodeNum; ++i) {
        gc.AddNodeToBucket(ptr);
    }
    auto nodeBucketSize = gc.nodeBucket_.size();
    EXPECT_TRUE(nodeBucketSize == 2);
}

/**
 * @tc.name: ReleaseNodeBucketTest001
 * @tc.desc: test results of ReleaseNodeBucket
 * @tc.type: FUNC
 * @tc.require: issueICD9PG
 */
HWTEST_F(RSRenderNodeGCTest, ReleaseNodeBucketTest001, TestSize.Level1)
{
    RSRenderNodeGC& gc = RSRenderNodeGC::Instance();
    RSRenderNodeAllocator& nodeAllocator = RSRenderNodeAllocator::Instance();
    auto ptr = nodeAllocator.CreateRSCanvasRenderNode(0);
    std::queue<std::vector<RSRenderNode*>> tempQueue;
    tempQueue.push({ptr.get()});
    gc.nodeBucket_.swap(tempQueue);
    gc.ReleaseNodeBucket();
    EXPECT_TRUE(gc.nodeBucket_.size() == 0);
}

/**
 * @tc.name: ReleaseNodeBucketTest002
 * @tc.desc: test results of ReleaseNodeBucket
 * @tc.type: FUNC
 * @tc.require:issue19909
 */
HWTEST_F(RSRenderNodeGCTest, ReleaseNodeBucketTest002, TestSize.Level1)
{
    RSRenderNodeGC& gc = RSRenderNodeGC::Instance();
    RSRenderNodeAllocator& nodeAllocator = RSRenderNodeAllocator::Instance();
    auto ptr = nodeAllocator.CreateRSCanvasRenderNode(0);
    std::queue<std::vector<RSRenderNode*>> tempQueue;
    tempQueue.push({ptr.get()});
    gc.nodeBucket_.swap(tempQueue);

    // Simulate the scenario where the VSync signal arrives
    gc.isEnable_.store(false);
    gc.ReleaseNodeBucket();
    EXPECT_TRUE(gc.nodeBucket_.size() == 1);
    gc.isEnable_.store(true);
}

/**
 * @tc.name: ReleaseNodeBucketTest003
 * @tc.desc: test results of ReleaseNodeBucket
 * @tc.type: FUNC
 * @tc.require: issueICD9PG
 */
HWTEST_F(RSRenderNodeGCTest, ReleaseNodeBucketTest003, TestSize.Level1)
{
    RSRenderNodeGC& gc = RSRenderNodeGC::Instance();
    // cannot happen
    gc.AddNodeToBucket(nullptr);
    gc.ReleaseNodeBucket();
    EXPECT_TRUE(gc.nodeBucket_.size() == 0);
}

/**
 * @tc.name: ReleaseNodeBucketTest004
 * @tc.desc: test results of ReleaseNodeBucket
 * @tc.type: FUNC
 * @tc.require: issueICD9PG
 */
HWTEST_F(RSRenderNodeGCTest, ReleaseNodeBucketTest004, TestSize.Level1)
{
    RSRenderNodeGC& gc = RSRenderNodeGC::Instance();
    // Test adding more than BUCKET_MAX_SIZE * GC_LEVEL_THR_HIGH nodes
    constexpr int nodeNum = BUCKET_MAX_SIZE * GC_LEVEL_THR_HIGH + 1;
    for (int i = 0; i < nodeNum; ++i) {
        auto ptr = new RSRenderNode(0);
        gc.AddNodeToBucket(ptr);
    }
    gc.ReleaseNodeBucket();
    EXPECT_TRUE(gc.nodeBucket_.size() == GC_LEVEL_THR_HIGH);
}

/**
 * @tc.name: ReleaseNodeMemory001
 * @tc.desc: test results of ReleaseNodeMemory
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSRenderNodeGCTest, ReleaseNodeMemory001, TestSize.Level1)
{
    RSRenderNodeGC& node = RSRenderNodeGC::Instance();
    std::queue<std::vector<RSRenderNode*>> tempQueue;
    node.nodeBucket_.swap(tempQueue);
    node.ReleaseNodeMemory();
    EXPECT_TRUE(node.nodeBucket_.size() == 0);

    auto ptrToNode = new RSRenderNode(0);
    node.NodeDestructorInner(ptrToNode);
    node.ReleaseNodeMemory();
    EXPECT_TRUE(node.nodeBucket_.size() == 0);

    constexpr int nodeNum = BUCKET_MAX_SIZE * GC_LEVEL_THR_IMMEDIATE + 1;
    for (int i = 0; i < nodeNum; ++i) {
        auto ptr = new RSRenderNode(i);
        node.AddNodeToBucket(ptr);
    }
    node.ReleaseNodeMemory();
    EXPECT_TRUE(node.nodeBucket_.size() == 0);
}

/**
 * @tc.name: ReleaseDrawableMemory001
 * @tc.desc: test results of ReleaseDrawableMemory
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSRenderNodeGCTest, ReleaseDrawableMemory001, TestSize.Level1)
{
    class ConcreteRSRenderNodeDrawableAdapter : public DrawableV2::RSRenderNodeDrawableAdapter {
    public:
        explicit ConcreteRSRenderNodeDrawableAdapter(std::shared_ptr<const RSRenderNode> node)
            : RSRenderNodeDrawableAdapter(std::move(node))
        {}
        void Draw(Drawing::Canvas& canvas) {}
    };
    RSRenderNodeGC& node = RSRenderNodeGC::Instance();
    node.ReleaseDrawableMemory();
    EXPECT_TRUE(node.drawableBucket_.size() == 0);

    std::shared_ptr<const RSRenderNode> otherNode = std::make_shared<const RSRenderNode>(0);
    DrawableV2::RSRenderNodeDrawableAdapter* ptrToNode = new ConcreteRSRenderNodeDrawableAdapter(otherNode);
    node.DrawableDestructorInner(ptrToNode);
    node.ReleaseDrawableMemory();
    EXPECT_TRUE(node.drawableBucket_.size() == 0);
}

/**
 * @tc.name: AddToOffTreeNodeBucket001
 * @tc.desc: test results of AddToOffTreeNodeBucket, while bucket queue is empty.
 * @tc.type: FUNC
 * @tc.require: issueIAF9XV
 */
HWTEST_F(RSRenderNodeGCTest, AddToOffTreeNodeBucket001, TestSize.Level1)
{
    RSRenderNodeGC& nodeGC = RSRenderNodeGC::Instance();
    NodeId id = 1;
    auto node = std::make_shared<RSBaseRenderNode>(id);
    nodeGC.AddToOffTreeNodeBucket(node);
    ASSERT_EQ(nodeGC.offTreeBucket_.size(), 1);
}

/**
 * @tc.name: AddToOffTreeNodeBucket002
 * @tc.desc: test results of AddToOffTreeNodeBucket, while bucket is full.
 * @tc.type: FUNC
 * @tc.require: issueIAF9XV
 */
HWTEST_F(RSRenderNodeGCTest, AddToOffTreeNodeBucket002, TestSize.Level1)
{
    RSRenderNodeGC& nodeGC = RSRenderNodeGC::Instance();
    NodeId id = 1;
    auto node = std::make_shared<RSBaseRenderNode>(id);
    nodeGC.offTreeBucket_.push(std::vector<std::shared_ptr<RSBaseRenderNode>>(OFF_TREE_BUCKET_MAX_SIZE, nullptr));
    nodeGC.AddToOffTreeNodeBucket(node);
    ASSERT_EQ(nodeGC.offTreeBucket_.size(), 2);
}

/**
 * @tc.name: ReleaseOffTreeNodeBucket001
 * @tc.desc: test results of ReleaseOffTreeNodeBucket001, expect node off tree and queue is empty
 * @tc.type: FUNC
 * @tc.require: issueIAF9XV
 */
HWTEST_F(RSRenderNodeGCTest, ReleaseOffTreeNodeBucket001, TestSize.Level1)
{
    RSRenderNodeGC& nodeGC = RSRenderNodeGC::Instance();
    NodeId id = 1;
    auto parent = std::make_shared<RSBaseRenderNode>(id);
    auto child = std::make_shared<RSBaseRenderNode>(++id);
    parent->AddChild(child);
    parent->GenerateFullChildrenList();
    parent->SetIsOnTheTree(true);
    child->SetIsOnTheTree(true);
    nodeGC.AddToOffTreeNodeBucket(parent);
    nodeGC.AddToOffTreeNodeBucket(child);
    ASSERT_EQ(parent->fullChildrenList_->size(), 1);
    nodeGC.ReleaseOffTreeNodeBucket();
    ASSERT_EQ(parent->fullChildrenList_->size(), 0);
    ASSERT_EQ(child->GetParent().lock(), nullptr);
}

/**
 * @tc.name: ReleaseOffTreeNodeBucket002
 * @tc.desc: test results of ReleaseOffTreeNodeBucket002, expect node off tree and queue is empty
 * @tc.type: FUNC
 * @tc.require: issueIAF9XV
 */
HWTEST_F(RSRenderNodeGCTest, ReleaseOffTreeNodeBucket002, TestSize.Level1)
{
    RSRenderNodeGC& nodeGC = RSRenderNodeGC::Instance();
    ClearOffTreeBucket();
    ClearOffTreeBucketMap();
    nodeGC.ReleaseOffTreeNodeBucket();
    EXPECT_EQ(nodeGC.offTreeBucket_.size(), 0);
    EXPECT_EQ(nodeGC.offTreeBucketMap_.size(), 0);
}

/**
 * @tc.name: ReleaseOffTreeNodeBucket003
 * @tc.desc: test results of ReleaseOffTreeNodeBucket003, expect node off tree and queue is empty
 * @tc.type: FUNC
 * @tc.require: issueIAF9XV
 */
HWTEST_F(RSRenderNodeGCTest, ReleaseOffTreeNodeBucket003, TestSize.Level1)
{
    RSRenderNodeGC& nodeGC = RSRenderNodeGC::Instance();
    {
        ClearOffTreeBucketMap();
        auto parent = std::make_shared<RSBaseRenderNode>(1);
        AddNodes(OFF_TREE_BUCKET_MAX_SIZE - 1, 1, 10000, false, parent);
        nodeGC.ReleaseOffTreeNodeBucket();
        EXPECT_EQ(nodeGC.offTreeBucketMap_.size(), 0);
    }
    {
        ClearOffTreeBucketMap();
        AddNodes(OFF_TREE_BUCKET_MAX_SIZE, 1, 10000, false, nullptr);
        nodeGC.ReleaseOffTreeNodeBucket();
        EXPECT_EQ(nodeGC.offTreeBucketMap_.size(), 0);
    }
    {
        ClearOffTreeBucketMap();
        AddNodes(OFF_TREE_BUCKET_MAX_SIZE + 100, 1, 10000, false, nullptr);
        nodeGC.ReleaseOffTreeNodeBucket();
        EXPECT_EQ(nodeGC.offTreeBucketMap_.size(), 1);
        EXPECT_EQ(nodeGC.offTreeBucketMap_.front().second.size(), 100);
    }
}

/**
 * @tc.name: ReleaseOffTreeNodeBucket004
 * @tc.desc: test results of ReleaseOffTreeNodeBucket004, expect node off tree and queue is empty
 * @tc.type: FUNC
 * @tc.require: issueIAF9XV
 */
HWTEST_F(RSRenderNodeGCTest, ReleaseOffTreeNodeBucket004, TestSize.Level1)
{
    RSRenderNodeGC& nodeGC = RSRenderNodeGC::Instance();
    ClearOffTreeBucketMap();
    AddNodes(OFF_TREE_BUCKET_MAX_SIZE, 1, 10000, true, nullptr);
    nodeGC.ReleaseOffTreeNodeBucket();
    EXPECT_EQ(nodeGC.offTreeBucketMap_.size(), 0);
}

/**
 * @tc.name: ReleaseOffTreeNodeBucket005
 * @tc.desc: test results of ReleaseOffTreeNodeBucket005, expect node off tree and queue is empty
 * @tc.type: FUNC
 * @tc.require: issueIAF9XV
 */
HWTEST_F(RSRenderNodeGCTest, ReleaseOffTreeNodeBucket005, TestSize.Level1)
{
    RSRenderNodeGC& nodeGC = RSRenderNodeGC::Instance();
    int32_t nodeNums =  OFF_TREE_BUCKET_MAX_SIZE;
    ClearOffTreeBucketMap();
    auto parent = std::make_shared<RSBaseRenderNode>(100);
    for (int i = 0; i < nodeNums; i++) {
        NodeId nodeId = 10000 + i;
        if (i % 2 == 0) {
            auto node = std::make_shared<RSBaseRenderNode>(nodeId);
            nodeGC.AddToOffTreeNodeBucket(node);
            if (i % 4 == 0) {
                parent->AddChild(node);
            }
        } else {
            nodeGC.AddToOffTreeNodeBucket(nullptr);
        }
    }
    nodeGC.ReleaseOffTreeNodeBucket();
    EXPECT_EQ(nodeGC.offTreeBucketMap_.size(), 0);
}

/**
 * @tc.name: ReleaseOffTreeNodeBucket006
 * @tc.desc: test ReleaseOffTreeNodeForBucketMap when isEnable_ is false
 * @tc.type: FUNC
 * @tc.require: issueIAF9XV
 */
HWTEST_F(RSRenderNodeGCTest, ReleaseOffTreeNodeBucket006, TestSize.Level1)
{
    RSRenderNodeGC& nodeGC = RSRenderNodeGC::Instance();
    ClearOffTreeBucketMap();
    constexpr uint32_t nodeNums = 100;
    AddNodes(nodeNums, 1, 10000, false, nullptr);
    
    EXPECT_EQ(nodeGC.offTreeBucketMap_.size(), 1);
    auto originalMapSize = nodeGC.offTreeBucketMap_.front().second.size();
    
    nodeGC.isEnable_.store(false);
    nodeGC.ReleaseOffTreeNodeBucket();
    
    EXPECT_EQ(nodeGC.offTreeBucketMap_.size(), 1);
    EXPECT_EQ(nodeGC.offTreeBucketMap_.front().second.size(), originalMapSize);
    
    nodeGC.isEnable_.store(true);
}

/**
 * @tc.name: ReleaseFromTree001
 * @tc.desc: test results of ReleaseFromTree, expect node off tree and queue is empty
 * @tc.type: FUNC
 * @tc.require: issueIAF9XV
 */
HWTEST_F(RSRenderNodeGCTest, ReleaseFromTree001, TestSize.Level1)
{
    RSRenderNodeGC& nodeGC = RSRenderNodeGC::Instance();
    {
        ClearOffTreeBucket();
        AddNodes(OFF_TREE_BUCKET_MAX_SIZE, 10000);
        nodeGC.ReleaseFromTree();
        EXPECT_EQ(nodeGC.offTreeBucket_.size(), 0);
    }
    {
        ClearOffTreeBucketMap();
        AddNodes(OFF_TREE_BUCKET_MAX_SIZE, 1, 10000, false, nullptr);
        nodeGC.ReleaseFromTree();
        EXPECT_EQ(nodeGC.offTreeBucketMap_.size(), 0);
    }
    {
        ClearOffTreeBucket();
        ClearOffTreeBucketMap();
        AddNodes(OFF_TREE_BUCKET_MAX_SIZE, 1, 10000, false, nullptr);
        AddNodes(OFF_TREE_BUCKET_MAX_SIZE, 20000);
        nodeGC.ReleaseFromTree();
        EXPECT_EQ(nodeGC.offTreeBucketMap_.size(), 0);
        EXPECT_EQ(nodeGC.offTreeBucket_.size(), 0);
    }
    EXPECT_EQ(nodeGC.offTreeBucket_.size(), 0);
    EXPECT_EQ(nodeGC.offTreeBucketMap_.size(), 0);
    nodeGC.ReleaseFromTree();
}

/**
 * @tc.name: ReleaseNodePidTest001
 * @tc.desc: ReleaseNodePid Test
 * @tc.type: FUNC
 * @tc.require: issueIAF9XV
 */
HWTEST_F(RSRenderNodeGCTest, ReleaseNodePidTest001, TestSize.Level1)
{
    RSRenderNodeGC &nodeGC = RSRenderNodeGC::Instance();
    pid_t pid = 5;
    nodeGC.backgroundPidSet_.insert(pid);
    nodeGC.ReleaseNodePid(pid);
    EXPECT_EQ(nodeGC.backgroundPidSet_.end(), nodeGC.backgroundPidSet_.find(pid));
}
 
/**
 * @tc.name: ReleaseNodeNotOnTreeTest001
 * @tc.desc: ReleaseNodeNotOnTree Test
 * @tc.type: FUNC
 * @tc.require: issueIAF9XV
 */
HWTEST_F(RSRenderNodeGCTest, ReleaseNodeNotOnTreeTest001, TestSize.Level1)
{
    RSRenderNodeGC &nodeGC = RSRenderNodeGC::Instance();
    pid_t pid = 5;
    auto node = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(node, nullptr);
    std::weak_ptr<RSBaseRenderNode> ptrnode = node->weak_from_this();
    nodeGC.notOnTreeNodeMap_[pid][node->GetId()] = ptrnode;
    nodeGC.ReleaseNodeNotOnTree(pid);
    EXPECT_EQ(nodeGC.notOnTreeNodeMap_.end(), nodeGC.notOnTreeNodeMap_.find(pid));
}
 
/**
 * @tc.name: ReleaseNodeMemNotOnTreeTest001
 * @tc.desc: ReleaseNodeMemNotOnTree Test
 * @tc.type: FUNC
 * @tc.require: issueIAF9XV
 */
HWTEST_F(RSRenderNodeGCTest, ReleaseNodeMemNotOnTreeTest001, TestSize.Level1)
{
    RSRenderNodeGC &nodeGC = RSRenderNodeGC::Instance();
    pid_t pid = 5;
    auto node = std::make_shared<RSCanvasRenderNode>(0);
    EXPECT_NE(node, nullptr);
    auto node2 = std::make_shared<RSCanvasRenderNode>(1);
    EXPECT_NE(node2, nullptr);
    std::weak_ptr<RSBaseRenderNode> ptrnode = node->weak_from_this();
    std::weak_ptr<RSBaseRenderNode> ptrnode2 = node2->weak_from_this();
    nodeGC.backgroundPidSet_.insert(pid);
    nodeGC.notOnTreeNodeMap_[pid][node->GetId()] = ptrnode;
    nodeGC.notOnTreeNodeMap_[pid][node2->GetId()] = ptrnode2;
    node2.reset();
    nodeGC.ReleaseNodeMemNotOnTree();
    ASSERT_TRUE(true);
}

/**
 * @tc.name: ReleaseNodeMemNotOnTreeTest002
 * @tc.desc: Test ReleaseNodeMemNotOnTree when isEnable_ is false
 * @tc.type: FUNC
 * @tc.require: issueIAF9XV
 */
HWTEST_F(RSRenderNodeGCTest, ReleaseNodeMemNotOnTreeTest002, TestSize.Level1)
{
    RSRenderNodeGC &nodeGC = RSRenderNodeGC::Instance();
    pid_t pid = 5;
    auto node = std::make_shared<RSCanvasRenderNode>(0);
    EXPECT_NE(node, nullptr);
    auto node2 = std::make_shared<RSCanvasRenderNode>(1);
    EXPECT_NE(node2, nullptr);
    std::weak_ptr<RSBaseRenderNode> ptrnode = node->weak_from_this();
    std::weak_ptr<RSBaseRenderNode> ptrnode2 = node2->weak_from_this();
    nodeGC.backgroundPidSet_.insert(pid);
    nodeGC.notOnTreeNodeMap_[pid][node->GetId()] = ptrnode;
    nodeGC.notOnTreeNodeMap_[pid][node2->GetId()] = ptrnode2;

    // Set isEnable_ to false to trigger early return in ReleaseNodeMemNotOnTree
    nodeGC.isEnable_.store(false);

    // Store original map size to verify it doesn't change
    auto originalMapSize = nodeGC.notOnTreeNodeMap_[pid].size();

    nodeGC.ReleaseNodeMemNotOnTree();

    // Verify that the map size hasn't changed due to early return
    EXPECT_EQ(nodeGC.notOnTreeNodeMap_[pid].size(), originalMapSize);
}

/**
 * @tc.name: ReleaseNodeMemNotOnTreeTest003
 * @tc.desc: Test ReleaseNodeMemNotOnTree when cnt exceeds NODE_MEM_RELEASE_LIMIT
 * @tc.type: FUNC
 * @tc.require: issueIAF9XV
 */
HWTEST_F(RSRenderNodeGCTest, ReleaseNodeMemNotOnTreeTest003, TestSize.Level1)
{
    RSRenderNodeGC &nodeGC = RSRenderNodeGC::Instance();
    pid_t pid = 5;

    // Add more than NODE_MEM_RELEASE_LIMIT nodes to trigger the cnt condition
    constexpr uint32_t nodeCount = NODE_MEM_RELEASE_LIMIT + 10;
    std::vector<std::shared_ptr<RSCanvasRenderNode>> nodes;

    // Create and add nodes
    for (uint32_t i = 0; i < nodeCount; ++i) {
        auto node = std::make_shared<RSCanvasRenderNode>(i);
        nodes.push_back(node);
        nodeGC.notOnTreeNodeMap_[pid][node->GetId()] = node->weak_from_this();
    }

    nodeGC.backgroundPidSet_.insert(pid);

    // Store original map size to verify it shrinks due to early return
    auto originalMapSize = nodeGC.notOnTreeNodeMap_[pid].size();

    nodeGC.isEnable_.store(true);
    // isEnable_ remains true (default), only cnt condition triggers early return
    nodeGC.ReleaseNodeMemNotOnTree();

    // Verify that the map size has reduced due to early return triggered by cnt limit
    // Since we return early after NODE_MEM_RELEASE_LIMIT iterations, some nodes should remain
    EXPECT_LT(nodeGC.notOnTreeNodeMap_[pid].size(), originalMapSize);
    EXPECT_GT(nodeGC.notOnTreeNodeMap_[pid].size(), 0);
}

/**
 * @tc.name: ReleaseNodeMemNotOnTreeTest004
 * @tc.desc: Test ReleaseNodeMemNotOnTree when cnt is below NODE_MEM_RELEASE_LIMIT and isEnable_ is true
 * @tc.type: FUNC
 * @tc.require: issueIAF9XV
 */
HWTEST_F(RSRenderNodeGCTest, ReleaseNodeMemNotOnTreeTest004, TestSize.Level1)
{
    RSRenderNodeGC &nodeGC = RSRenderNodeGC::Instance();
    pid_t pid = 5;

    // Add fewer than NODE_MEM_RELEASE_LIMIT nodes to ensure cnt condition doesn't trigger
    constexpr uint32_t nodeCount = 50; // Much less than 200
    std::vector<std::shared_ptr<RSCanvasRenderNode>> nodes;

    // Create and add nodes
    for (uint32_t i = 0; i < nodeCount; ++i) {
        auto node = std::make_shared<RSCanvasRenderNode>(i);
        nodes.push_back(node);
        nodeGC.notOnTreeNodeMap_[pid][node->GetId()] = node->weak_from_this();
    }

    nodeGC.backgroundPidSet_.insert(pid);

    // Set isEnable_ to true to ensure only cnt condition is evaluated
    nodeGC.isEnable_.store(true);

    nodeGC.ReleaseNodeMemNotOnTree();
    
    // Verify that all nodes are processed (map should be empty since cnt++ > NODE_MEM_RELEASE_LIMIT is false)
    EXPECT_EQ(nodeGC.notOnTreeNodeMap_[pid].size(), 0);
}

/**
 * @tc.name: SetAbilityState001
 * @tc.desc: SetAbilityState Test
 * @tc.type: FUNC
 * @tc.require: issues20579
 */
HWTEST_F(RSRenderNodeGCTest, SetAbilityState001, TestSize.Level1)
{
    RSRenderNodeGC &nodeGC = RSRenderNodeGC::Instance();
    pid_t pid = 5;
    nodeGC.backgroundPidSet_.insert(pid);
    nodeGC.scbPid_ = pid;
    nodeGC.SetAbilityState(pid, false);
    EXPECT_NE(nodeGC.backgroundPidSet_.end(), nodeGC.backgroundPidSet_.find(pid));

    nodeGC.scbPid_ = 6;
    EXPECT_NE(nodeGC.scbPid_, pid);
    nodeGC.SetAbilityState(pid, false);
    EXPECT_EQ(nodeGC.backgroundPidSet_.end(), nodeGC.backgroundPidSet_.find(pid));
}

/**
 * @tc.name: SetIsOnTheTree001
 * @tc.desc: SetIsOnTheTree Test
 * @tc.type: FUNC
 * @tc.require: issues20579
 */
HWTEST_F(RSRenderNodeGCTest, SetIsOnTheTree001, TestSize.Level1)
{
    RSRenderNodeGC &nodeGC = RSRenderNodeGC::Instance();
    nodeGC.scbPid_ = 1;
    NodeId nodeId = (2ull << 32) + 1;
    auto nodePid = ExtractPid(nodeId);
    EXPECT_NE(nodePid, nodeGC.scbPid_);

    bool isOnTree = false;
    RSRenderNodeAllocator& nodeAllocator = RSRenderNodeAllocator::Instance();
    auto ptr = nodeAllocator.CreateRSCanvasRenderNode(nodeId);

    nodeGC.SetIsOnTheTree(nodeId, ptr, isOnTree);
    EXPECT_EQ(nodeGC.notOnTreeNodeMap_[nodePid][nodeId].lock().get(), ptr.get());
}

/**
 * @tc.name: NodeOffTreeMemReleaseEnabledTest
 * @tc.desc: Verify the SetNodeOffTreeMemReleaseEnabled and IsNodeOffTreeMemReleaseEnabled functions
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeGCTest, NodeOffTreeMemReleaseEnabledTest, TestSize.Level1)
{
    RSRenderNodeGC::Instance().SetNodeOffTreeMemReleaseEnabled(true);
    ASSERT_EQ(RSRenderNodeGC::Instance().IsNodeOffTreeMemReleaseEnabled(), true);
    RSRenderNodeGC::Instance().SetNodeOffTreeMemReleaseEnabled(false);
    ASSERT_EQ(RSRenderNodeGC::Instance().IsNodeOffTreeMemReleaseEnabled(), false);
}

/**
 * @tc.name: ReleaseOffTreeNodeBucket007
 * @tc.desc: Cover branch: animationManager non-null in ReleaseOffTreeNodeForBucketMap
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeGCTest, ReleaseOffTreeNodeBucket007, TestSize.Level1)
{
    RSRenderNodeGC& nodeGC = RSRenderNodeGC::Instance();
    ClearOffTreeBucketMap();
    nodeGC.isEnable_ = true;

    pid_t pid = 1;
    NodeId nodeId = 10000;
    auto renderNode = std::make_shared<RSCanvasRenderNode>(nodeId);
    // Make animationManager_ non-null by adding an animation
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto animation = std::make_shared<RSRenderCurveAnimation>(1, 1, property, property1, property2);
    renderNode->AddAnimation(animation);
    ASSERT_NE(renderNode->GetAnimationManager(), nullptr);

    std::unordered_map<NodeId, std::shared_ptr<RSBaseRenderNode>> renderNodeMap;
    renderNodeMap[nodeId] = renderNode;
    nodeGC.AddToOffTreeNodeBucket(pid, renderNodeMap);
    nodeGC.ReleaseOffTreeNodeBucket();
    EXPECT_TRUE(true);
}

namespace {
constexpr size_t MAX_CHECK_SIZE = 50;
std::atomic<uint32_t> g_offTreeTaskCount(0);

RSRenderNodeGC::gcTask MakeCountingTask()
{
    return [](RSTaskMessage::RSTask task, const std::string& name, int64_t,
        AppExecFwk::EventQueue::Priority) {
        if (name == DELETE_NODE_OFF_TREE_TASK) {
            g_offTreeTaskCount++;
        }
        task();
    };
}

class MainTaskGuard {
public:
    explicit MainTaskGuard(RSRenderNodeGC::gcTask task) : original_(RSRenderNodeGC::Instance().mainTask_)
    {
        RSRenderNodeGC::Instance().SetMainTask(task);
    }

    ~MainTaskGuard()
    {
        RSRenderNodeGC::Instance().SetMainTask(original_);
    }

private:
    RSRenderNodeGC::gcTask original_;
};

void ClearNotOnTreeState()
{
    RSRenderNodeGC& nodeGC = RSRenderNodeGC::Instance();
    nodeGC.backgroundPidSet_.clear();
    nodeGC.notOnTreeNodeMap_.clear();
    nodeGC.isEnable_.store(true);
}
} // namespace

/**
 * @tc.name: CheckHasNodeNotOnTreeTest001
 * @tc.desc: Test CheckHasNodeNotOnTree returns false when loop finishes without hit,
 *           including empty set, background pid without node and non-background pid ignored
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeGCTest, CheckHasNodeNotOnTreeTest001, TestSize.Level1)
{
    RSRenderNodeGC& nodeGC = RSRenderNodeGC::Instance();
    ClearNotOnTreeState();
    EXPECT_FALSE(nodeGC.CheckHasNodeNotOnTree());

    pid_t backgroundPid = 5;
    pid_t foregroundPid = 6;
    auto node = std::make_shared<RSCanvasRenderNode>(0);
    nodeGC.backgroundPidSet_.insert(backgroundPid);
    nodeGC.notOnTreeNodeMap_[foregroundPid][node->GetId()] = node->weak_from_this();
    EXPECT_FALSE(nodeGC.CheckHasNodeNotOnTree());
    // mutex must be released after each return path
    EXPECT_TRUE(nodeGC.nodeNotOnTreeMutex_.try_lock());
    nodeGC.nodeNotOnTreeMutex_.unlock();
    ClearNotOnTreeState();
}

/**
 * @tc.name: CheckHasNodeNotOnTreeTest002
 * @tc.desc: Test CheckHasNodeNotOnTree returns true when a background pid has node not on tree
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeGCTest, CheckHasNodeNotOnTreeTest002, TestSize.Level1)
{
    RSRenderNodeGC& nodeGC = RSRenderNodeGC::Instance();
    ClearNotOnTreeState();
    pid_t noNodePid = 5;
    pid_t hasNodePid = 6;
    auto node = std::make_shared<RSCanvasRenderNode>(0);
    nodeGC.backgroundPidSet_.insert(noNodePid);
    nodeGC.backgroundPidSet_.insert(hasNodePid);
    nodeGC.notOnTreeNodeMap_[hasNodePid][node->GetId()] = node->weak_from_this();
    EXPECT_TRUE(nodeGC.CheckHasNodeNotOnTree());
    EXPECT_TRUE(nodeGC.nodeNotOnTreeMutex_.try_lock());
    nodeGC.nodeNotOnTreeMutex_.unlock();
    ClearNotOnTreeState();
}

/**
 * @tc.name: CheckHasNodeNotOnTreeTest003
 * @tc.desc: Test CheckHasNodeNotOnTree returns true when background pid count exceeds MAX_CHECK_SIZE
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeGCTest, CheckHasNodeNotOnTreeTest003, TestSize.Level1)
{
    RSRenderNodeGC& nodeGC = RSRenderNodeGC::Instance();
    ClearNotOnTreeState();
    for (size_t i = 0; i <= MAX_CHECK_SIZE; i++) {
        nodeGC.backgroundPidSet_.insert(static_cast<pid_t>(i));
    }
    EXPECT_TRUE(nodeGC.CheckHasNodeNotOnTree());
    EXPECT_TRUE(nodeGC.nodeNotOnTreeMutex_.try_lock());
    nodeGC.nodeNotOnTreeMutex_.unlock();
    ClearNotOnTreeState();
}

/**
 * @tc.name: CheckHasNodeNotOnTreeTest004
 * @tc.desc: Test CheckHasNodeNotOnTree returns true when nodeNotOnTreeMutex_ is held by another thread
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeGCTest, CheckHasNodeNotOnTreeTest004, TestSize.Level1)
{
    RSRenderNodeGC& nodeGC = RSRenderNodeGC::Instance();
    ClearNotOnTreeState();
    bool result = false;
    nodeGC.nodeNotOnTreeMutex_.lock();
    std::thread checkThread([&nodeGC, &result]() { result = nodeGC.CheckHasNodeNotOnTree(); });
    checkThread.join();
    nodeGC.nodeNotOnTreeMutex_.unlock();
    EXPECT_TRUE(result);
    ClearNotOnTreeState();
}

/**
 * @tc.name: ReleaseNodeMemoryOffTreeTest001
 * @tc.desc: Test ReleaseNodeMemory skips posting off-tree task when no node is pending release
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeGCTest, ReleaseNodeMemoryOffTreeTest001, TestSize.Level1)
{
    RSRenderNodeGC& nodeGC = RSRenderNodeGC::Instance();
    ClearNotOnTreeState();
    g_offTreeTaskCount.store(0);
    MainTaskGuard guard(MakeCountingTask());
    nodeGC.ReleaseNodeMemory(true);
    EXPECT_EQ(g_offTreeTaskCount.load(), 0);
    ClearNotOnTreeState();
}

/**
 * @tc.name: ReleaseNodeMemoryOffTreeTest002
 * @tc.desc: Test ReleaseNodeMemory posts and executes off-tree task when background node exists
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeGCTest, ReleaseNodeMemoryOffTreeTest002, TestSize.Level1)
{
    RSRenderNodeGC& nodeGC = RSRenderNodeGC::Instance();
    ClearNotOnTreeState();
    g_offTreeTaskCount.store(0);
    pid_t pid = 5;
    auto node = std::make_shared<RSCanvasRenderNode>(0);
    nodeGC.backgroundPidSet_.insert(pid);
    nodeGC.notOnTreeNodeMap_[pid][node->GetId()] = node->weak_from_this();
    {
        MainTaskGuard guard(MakeCountingTask());
        nodeGC.ReleaseNodeMemory(true);
    }
    EXPECT_EQ(g_offTreeTaskCount.load(), 1);
    EXPECT_EQ(nodeGC.notOnTreeNodeMap_.find(pid), nodeGC.notOnTreeNodeMap_.end());
    ClearNotOnTreeState();
}

/**
 * @tc.name: ReleaseNodeMemoryOffTreeTest003
 * @tc.desc: Test ReleaseNodeMemory releases off-tree node directly when mainTask_ is null
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeGCTest, ReleaseNodeMemoryOffTreeTest003, TestSize.Level1)
{
    RSRenderNodeGC& nodeGC = RSRenderNodeGC::Instance();
    ClearNotOnTreeState();
    pid_t pid = 5;
    auto node = std::make_shared<RSCanvasRenderNode>(0);
    nodeGC.backgroundPidSet_.insert(pid);
    nodeGC.notOnTreeNodeMap_[pid][node->GetId()] = node->weak_from_this();
    MainTaskGuard guard(nullptr);
    nodeGC.ReleaseNodeMemory(true);
    EXPECT_EQ(nodeGC.notOnTreeNodeMap_.find(pid), nodeGC.notOnTreeNodeMap_.end());
    ClearNotOnTreeState();
}

/**
 * @tc.name: AddNodeToBgBucket001
 * @tc.desc: test results of AddNodeToBgBucket
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeGCTest, AddNodeToBgBucket001, TestSize.Level1)
{
    RSRenderNodeGC& gc = RSRenderNodeGC::Instance();
    ClearBgBucket();
    RSRenderNodeAllocator& nodeAllocator = RSRenderNodeAllocator::Instance();

    // Add one node, bg bucket should have 1 bucket with 1 element
    auto ptr1 = nodeAllocator.CreateRSCanvasRenderNode(0);
    gc.AddNodeToBgBucket(ptr1.get());
    EXPECT_EQ(gc.nodeBgBucket_.size(), 1u);
    EXPECT_EQ(gc.nodeBgBucket_.front().size(), 1u);

    // Add BUCKET_MAX_SIZE - 1 more nodes, still 1 bucket
    for (int i = 1; i < BUCKET_MAX_SIZE; ++i) {
        auto ptr = nodeAllocator.CreateRSCanvasRenderNode(i);
        gc.AddNodeToBgBucket(ptr.get());
    }
    EXPECT_EQ(gc.nodeBgBucket_.size(), 1u);
    EXPECT_EQ(gc.nodeBgBucket_.front().size(), static_cast<size_t>(BUCKET_MAX_SIZE));

    // Add one more, should split into 2 buckets
    auto ptr2 = nodeAllocator.CreateRSCanvasRenderNode(BUCKET_MAX_SIZE);
    gc.AddNodeToBgBucket(ptr2.get());
    EXPECT_EQ(gc.nodeBgBucket_.size(), 2u);
    ClearBgBucket();
}

/**
 * @tc.name: ReleaseNodeOnBgThread001
 * @tc.desc: test ReleaseNodeOnBgThread with empty bg bucket returns directly
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeGCTest, ReleaseNodeOnBgThread001, TestSize.Level1)
{
    RSRenderNodeGC& gc = RSRenderNodeGC::Instance();
    ClearBgBucket();
    gc.ReleaseNodeOnBgThread();
    EXPECT_TRUE(gc.nodeBgBucket_.empty());
}

/**
 * @tc.name: ReleaseNodeOnBgThread002
 * @tc.desc: test ReleaseNodeOnBgThread drains all bg buckets
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeGCTest, ReleaseNodeOnBgThread002, TestSize.Level1)
{
    RSRenderNodeGC& gc = RSRenderNodeGC::Instance();
    RSRenderNodeAllocator& nodeAllocator = RSRenderNodeAllocator::Instance();
    ClearBgBucket();

    // Add multiple buckets
    auto ptr1 = nodeAllocator.CreateRSCanvasRenderNode(0);
    auto ptr2 = nodeAllocator.CreateRSCanvasRenderNode(1);
    gc.AddNodeToBgBucket(ptr1.get());
    // Force a new bucket by filling the first one
    for (int i = 1; i < BUCKET_MAX_SIZE; ++i) {
        auto ptr = nodeAllocator.CreateRSCanvasRenderNode(i + 100);
        gc.AddNodeToBgBucket(ptr.get());
    }
    gc.AddNodeToBgBucket(ptr2.get());
    ASSERT_EQ(gc.nodeBgBucket_.size(), 2u);

    gc.ReleaseNodeOnBgThread();
    EXPECT_TRUE(gc.nodeBgBucket_.empty());
}

/**
 * @tc.name: ReleaseNodeOnBgThread003
 * @tc.desc: test ReleaseNodeOnBgThread with nullptr nodes in bucket
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeGCTest, ReleaseNodeOnBgThread003, TestSize.Level1)
{
    RSRenderNodeGC& gc = RSRenderNodeGC::Instance();
    RSRenderNodeAllocator& nodeAllocator = RSRenderNodeAllocator::Instance();
    ClearBgBucket();

    auto ptr = nodeAllocator.CreateRSCanvasRenderNode(0);
    std::queue<std::vector<RSRenderNode*>> tempQueue;
    tempQueue.push({nullptr, ptr.get(), nullptr});
    gc.nodeBgBucket_.swap(tempQueue);
    ASSERT_EQ(gc.nodeBgBucket_.size(), 1u);

    gc.ReleaseNodeOnBgThread();
    EXPECT_TRUE(gc.nodeBgBucket_.empty());
}

/**
 * @tc.name: NodeDestructorInnerBgBucket001
 * @tc.desc: test NodeDestructorInner routes to bg bucket for non-animation node
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeGCTest, NodeDestructorInnerBgBucket001, TestSize.Level1)
{
    RSRenderNodeGC& gc = RSRenderNodeGC::Instance();
    ClearNodeBucket();
    ClearBgBucket();

    // RSRenderNode without animation -> MustReleaseOnMainThread returns false
    // -> routes to bg bucket when GetBgNodeReleaseEnabled is true (default)
    RSRenderNodeAllocator& nodeAllocator = RSRenderNodeAllocator::Instance();
    auto ptr = nodeAllocator.CreateRSCanvasRenderNode(0);
    gc.NodeDestructorInner(ptr.get());
    EXPECT_TRUE(gc.nodeBucket_.empty());
    EXPECT_EQ(gc.nodeBgBucket_.size(), 1u);
    ClearBgBucket();
}

/**
 * @tc.name: NodeDestructorInnerBgBucket002
 * @tc.desc: test NodeDestructorInner routes to main bucket for animation node
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeGCTest, NodeDestructorInnerBgBucket002, TestSize.Level1)
{
    RSRenderNodeGC& gc = RSRenderNodeGC::Instance();
    ClearNodeBucket();
    ClearBgBucket();

    // Node with animation -> MustReleaseOnMainThread returns true -> main bucket
    auto node = std::make_shared<RSCanvasRenderNode>(0);
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto animation = std::make_shared<RSRenderCurveAnimation>(1, 1, property, property1, property2);
    node->AddAnimation(animation);
    gc.NodeDestructorInner(node.get());
    EXPECT_FALSE(gc.nodeBucket_.empty());
    EXPECT_TRUE(gc.nodeBgBucket_.empty());
    ClearNodeBucket();
}

/**
 * @tc.name: ReleaseNodeMemoryBgBucket001
 * @tc.desc: test ReleaseNodeMemory with non-empty bg bucket posts background task
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeGCTest, ReleaseNodeMemoryBgBucket001, TestSize.Level1)
{
    RSRenderNodeGC& gc = RSRenderNodeGC::Instance();
    ClearNodeBucket();
    ClearBgBucket();
    RSRenderNodeAllocator& nodeAllocator = RSRenderNodeAllocator::Instance();

    // Add nodes to bg bucket
    auto ptr = nodeAllocator.CreateRSCanvasRenderNode(0);
    gc.AddNodeToBgBucket(ptr.get());
    ASSERT_FALSE(gc.nodeBgBucket_.empty());

    // ReleaseNodeMemory should post bg task when switch is on (default)
    gc.ReleaseNodeMemory();
    // After PostTask, bg bucket will be drained by background thread asynchronously
    // Verify no crash and main bucket was not affected
    EXPECT_TRUE(gc.nodeBucket_.empty());
}

/**
 * @tc.name: ReleaseNodeMemoryBgBucket002
 * @tc.desc: test ReleaseNodeMemory with empty bg bucket does not post task
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeGCTest, ReleaseNodeMemoryBgBucket002, TestSize.Level1)
{
    RSRenderNodeGC& gc = RSRenderNodeGC::Instance();
    ClearNodeBucket();
    ClearBgBucket();

    // bg bucket is empty, ReleaseNodeMemory should return early for bg path
    gc.ReleaseNodeMemory();
    EXPECT_TRUE(gc.nodeBgBucket_.empty());
    EXPECT_TRUE(gc.nodeBucket_.empty());
}

} // namespace Rosen
} // namespace OHOS