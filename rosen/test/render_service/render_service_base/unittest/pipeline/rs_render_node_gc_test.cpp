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
#include "gtest/gtest.h"

#include "pipeline/rs_render_node_allocator.h"
#include "pipeline/rs_render_node_gc.h"
#include "drawable/rs_render_node_shadow_drawable.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSRenderNodeGCTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderNodeGCTest::SetUpTestCase() {}
void RSRenderNodeGCTest::TearDownTestCase() {}
void RSRenderNodeGCTest::SetUp() {}
void RSRenderNodeGCTest::TearDown()
{
    auto& nodeGC = RSRenderNodeGC::Instance();
    while (nodeGC.offTreeBucket_.size() > 0) {
        nodeGC.offTreeBucket_.pop();
    }
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
    EXPECT_TRUE(node.nodeBucket_.size() == 1);

    auto ptr = std::make_shared<RSRenderNode>(0);
    node.NodeDestructorInner(ptr.get());
    EXPECT_TRUE(ptr != nullptr);

    node.NodeDestructorInner(ptr.get());
    EXPECT_TRUE(ptr != nullptr);
}

/**
 * @tc.name: ReleaseNodeBucketTest
 * @tc.desc: test results of ReleaseNodeBucket
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeGCTest, ReleaseNodeBucketTest, TestSize.Level1)
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
 * @tc.name: ReleaseOffTreeNodeBucket
 * @tc.desc: test results of ReleaseOffTreeNodeBucket, expect node off tree and queue is empty
 * @tc.type: FUNC
 * @tc.require: issueIAF9XV
 */
HWTEST_F(RSRenderNodeGCTest, ReleaseOffTreeNodeBucket, TestSize.Level1)
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
} // namespace Rosen
} // namespace OHOS
