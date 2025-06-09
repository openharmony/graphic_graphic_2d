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
#include "feature/occlusion_culling/rs_occlusion_handler.h"
#include "feature/occlusion_culling/rs_occlusion_node.h"
#include "feature_cfg/graphic_feature_param_manager.h"

#include "pipeline/rs_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class RSOcclusionHandlerTest : public testing::Test {
public:
    static const NodeId id;
    static const NodeId keyOcclusionNodeId;
    static const NodeId nodeId;
    static const NodeId firstNodeId;
    static const NodeId secondNodeId;
    static const NodeId thirdNodeId;
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSOcclusionHandlerTest::SetUpTestCase() {}
void RSOcclusionHandlerTest::TearDownTestCase() {}
void RSOcclusionHandlerTest::SetUp() {}
void RSOcclusionHandlerTest::TearDown() {}

const NodeId RSOcclusionHandlerTest::id = 0;
const NodeId RSOcclusionHandlerTest::keyOcclusionNodeId = 0;
const NodeId RSOcclusionHandlerTest::nodeId = 1;
const NodeId RSOcclusionHandlerTest::firstNodeId = 1;
const NodeId RSOcclusionHandlerTest::secondNodeId = 2;
const NodeId RSOcclusionHandlerTest::thirdNodeId = 3;

/*
 * @tc.name: CollectNode_001
 * @tc.desc: Test CollectNode when the node id and rootNodeId are not equal
 * @tc.type: FUNC
 * @tc.require: issueIC2H2
 */
HWTEST_F(RSOcclusionHandlerTest, CollectNode_001, TestSize.Level1)
{
    RSRenderNode node = RSRenderNode(nodeId);
    std::shared_ptr<RSOcclusionHandler> rsOcclusionHandler =
        std::make_shared<RSOcclusionHandler>(id);
    rsOcclusionHandler->CollectNode(node);
    ASSERT_EQ(rsOcclusionHandler->rootOcclusionNode_, nullptr);
}

/*
 * @tc.name: CollectNode_003
 * @tc.desc: Test CollectNode when node is rootNode
 * @tc.type: FUNC
 * @tc.require: issueIC2H2
 */
HWTEST_F(RSOcclusionHandlerTest, CollectNode_002, TestSize.Level1)
{
    RSRenderNode node(nodeId);
    node.instanceRootNodeId_ = nodeId;
    std::shared_ptr<RSOcclusionHandler> rsOcclusionHandler =
        std::make_shared<RSOcclusionHandler>(id);
    rsOcclusionHandler->rootNodeId_ = nodeId;
    rsOcclusionHandler->CollectNode(node);
    ASSERT_NE(rsOcclusionHandler->rootOcclusionNode_, nullptr);
    EXPECT_EQ(rsOcclusionHandler->rootOcclusionNode_->id_, nodeId);
}

/*
 * @tc.name: CollectNode_004
 * @tc.desc: Test CollectNode in non-anomalous situations.
 * @tc.type: FUNC
 * @tc.require: issueIC2H2
 */
HWTEST_F(RSOcclusionHandlerTest, CollectNode_003, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodePtr = std::make_shared<RSRenderNode>(firstNodeId);
    nodePtr->instanceRootNodeId_ = firstNodeId;
    std::shared_ptr<RSOcclusionHandler> rsOcclusionHandler =
        std::make_shared<RSOcclusionHandler>(id);
    rsOcclusionHandler->rootNodeId_ = firstNodeId;
    rsOcclusionHandler->CollectNode(*nodePtr);
    std::shared_ptr<RSRenderNode> nodePtr2 = std::make_shared<RSRenderNode>(secondNodeId);
    nodePtr2->parent_ = nodePtr;
    nodePtr2->instanceRootNodeId_ = firstNodeId;
    rsOcclusionHandler->CollectNode(*nodePtr2);
    EXPECT_EQ(nodePtr2->GetId(), secondNodeId);
    EXPECT_NE(rsOcclusionHandler->occlusionNodes_.find(nodePtr2->GetId()), rsOcclusionHandler->occlusionNodes_.end());
    ASSERT_NE(rsOcclusionHandler->rootOcclusionNode_, nullptr);
    EXPECT_EQ(rsOcclusionHandler->rootOcclusionNode_->id_, firstNodeId);
    EXPECT_EQ(rsOcclusionHandler->rootOcclusionNode_->firstChild_->id_, secondNodeId);
    std::shared_ptr<RSRenderNode> nodePtr3 = std::make_shared<RSRenderNode>(thirdNodeId);
    std::shared_ptr<OcclusionNode> occlusionNode3 =
        std::make_shared<OcclusionNode>(thirdNodeId, RSRenderNodeType::CANVAS_NODE);
    nodePtr3->parent_ = nodePtr2;
    nodePtr3->instanceRootNodeId_ = firstNodeId;
    rsOcclusionHandler->occlusionNodes_[secondNodeId] = nullptr;
    rsOcclusionHandler->occlusionNodes_[thirdNodeId] = occlusionNode3;
    rsOcclusionHandler->CollectNode(*nodePtr3);
    EXPECT_EQ(rsOcclusionHandler->occlusionNodes_.find(nodePtr3->GetId()), rsOcclusionHandler->occlusionNodes_.end());
    rsOcclusionHandler->CollectNode(*nodePtr3);
    EXPECT_EQ(rsOcclusionHandler->occlusionNodes_.find(nodePtr3->GetId()), rsOcclusionHandler->occlusionNodes_.end());
}

/*
 * @tc.name: ProcessOffTreeNodes_001
 * @tc.desc: Test ProcessOffTreeNodes
 * @tc.type: FUNC
 * @tc.require: issueIC2H2
 */
HWTEST_F(RSOcclusionHandlerTest, ProcessOffTreeNodes_001, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodePtr = std::make_shared<RSRenderNode>(firstNodeId);
    std::shared_ptr<RSOcclusionHandler> rsOcclusionHandler =
        std::make_shared<RSOcclusionHandler>(id);
    rsOcclusionHandler->rootNodeId_ = firstNodeId;
    rsOcclusionHandler->CollectNode(*nodePtr);
    std::shared_ptr<RSRenderNode> nodePtr2 = std::make_shared<RSRenderNode>(secondNodeId);
    nodePtr2->parent_ = nodePtr;
    rsOcclusionHandler->CollectNode(*nodePtr2);
    std::unordered_set<uint64_t> ids = {secondNodeId};
    std::shared_ptr<OcclusionNode> keyOcclusionNode = std::make_shared<OcclusionNode>(keyOcclusionNodeId,
        RSRenderNodeType::ROOT_NODE);
    rsOcclusionHandler->occlusionNodes_[keyOcclusionNodeId] = keyOcclusionNode;
    rsOcclusionHandler->ProcessOffTreeNodes(ids);
    EXPECT_EQ(rsOcclusionHandler->occlusionNodes_.find(secondNodeId), rsOcclusionHandler->occlusionNodes_.end());
    int expectSize = 2;
    EXPECT_EQ(rsOcclusionHandler->occlusionNodes_.size(), expectSize);
}

/*
 * @tc.name: CollectSubTree_001
 * @tc.desc: Test CollectSubTree where isSubTreeNeedPrepare is true
 * @tc.type: FUNC
 * @tc.require: issueIC2H2
 */
HWTEST_F(RSOcclusionHandlerTest, CollectSubTree_001, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodePtr = std::make_shared<RSRenderNode>(firstNodeId);
    nodePtr->instanceRootNodeId_ = firstNodeId;
    std::shared_ptr<RSOcclusionHandler> rsOcclusionHandler =
        std::make_shared<RSOcclusionHandler>(id);
    // set the second param to true, then occlusionNodes_ collection will be empty
    rsOcclusionHandler->CollectSubTree(*nodePtr, true);
    int expectSize = 0;
    EXPECT_EQ(rsOcclusionHandler->occlusionNodes_.size(), expectSize);
}

/*
 * @tc.name: CollectSubTree_002
 * @tc.desc: Test CollectSubTree in non-anomalous situations and the node and its subtree were correctly collected.
 * @tc.type: FUNC
 * @tc.require: issueIC2H2
 */
HWTEST_F(RSOcclusionHandlerTest, CollectSubTree_002, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodePtr = std::make_shared<RSRenderNode>(firstNodeId);
    nodePtr->instanceRootNodeId_ = firstNodeId;
    std::shared_ptr<RSOcclusionHandler> rsOcclusionHandler =
        std::make_shared<RSOcclusionHandler>(id);
    rsOcclusionHandler->rootNodeId_ = firstNodeId;
    rsOcclusionHandler->CollectNode(*nodePtr);
    std::shared_ptr<RSRenderNode> nodePtr2 = std::make_shared<RSRenderNode>(secondNodeId);
    nodePtr2->parent_ = nodePtr;
    rsOcclusionHandler->CollectSubTree(*nodePtr2, false);
    // the node and its subtree were correctly collected.
    int expectSize = 2;
    EXPECT_EQ(rsOcclusionHandler->occlusionNodes_.size(), expectSize);
    int expectCount = 1;
    EXPECT_EQ(rsOcclusionHandler->occlusionNodes_.count(nodePtr2->id_), expectCount);
}


/*
 * @tc.name: UpdateChildrenOutOfRectInfo_001
 * @tc.desc: Test UpdateChildrenOutOfRectInfo
 * @tc.type: FUNC
 * @tc.require: issueIC2H2
 */
HWTEST_F(RSOcclusionHandlerTest, UpdateChildrenOutOfRectInfo_001, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodePtr = std::make_shared<RSRenderNode>(nodeId);
    std::shared_ptr<RSOcclusionHandler> rsOcclusionHandler =
        std::make_shared<RSOcclusionHandler>(id);
    nodePtr->instanceRootNodeId_ = nodeId;
    rsOcclusionHandler->rootNodeId_ = nodeId;
    rsOcclusionHandler->CollectNode(*nodePtr);
    int expectSize = 0;
    EXPECT_NE(rsOcclusionHandler->occlusionNodes_.size(), expectSize);
    auto it = rsOcclusionHandler->occlusionNodes_.find(nodePtr->id_);
    ASSERT_NE(rsOcclusionHandler->occlusionNodes_.end(), it);

    rsOcclusionHandler->UpdateChildrenOutOfRectInfo(*nodePtr);
    EXPECT_FALSE(it->second->hasChildrenOutOfRect_);
}

/*
 * @tc.name: UpdateSkippedSubTreeProp_001
 * @tc.desc: Test UpdateSkippedSubTreeProp when isSubTreeIgnored_ is true
 * @tc.type: FUNC
 * @tc.require: issueIC2H2
 */
HWTEST_F(RSOcclusionHandlerTest, UpdateSkippedSubTreeProp_001, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodePtr = std::make_shared<RSRenderNode>(firstNodeId);
    std::shared_ptr<RSOcclusionHandler> rsOcclusionHandler =
        std::make_shared<RSOcclusionHandler>(id);
    rsOcclusionHandler->rootNodeId_ = firstNodeId;
    rsOcclusionHandler->CollectNode(*nodePtr);
    std::shared_ptr<RSRenderNode> nodePtr2 = std::make_shared<RSRenderNode>(secondNodeId);
    nodePtr2->parent_ = nodePtr;
    nodePtr2->instanceRootNodeId_ = firstNodeId;
    rsOcclusionHandler->CollectSubTree(*nodePtr2, false);
    auto it = rsOcclusionHandler->occlusionNodes_.find(nodePtr2->id_);
    ASSERT_NE(rsOcclusionHandler->occlusionNodes_.end(), it);
    it->second->isSubTreeIgnored_ = true;
    rsOcclusionHandler->CollectSubTree(*nodePtr2, false);
    // when isSubTreeIgnored_ is true then subTreeSkipPrepareNodes_ is empty after method CollectSubTree
    EXPECT_TRUE(rsOcclusionHandler->subTreeSkipPrepareNodes_.empty());
    rsOcclusionHandler->UpdateSkippedSubTreeProp();
    EXPECT_TRUE(rsOcclusionHandler->subTreeSkipPrepareNodes_.empty());
}

/*
 * @tc.name: UpdateSkippedSubTreeProp_002
 * @tc.desc: Test UpdateSkippedSubTreeProp when isSubTreeIgnored_ is false
 * @tc.type: FUNC
 * @tc.require: issueIC2H2
 */
HWTEST_F(RSOcclusionHandlerTest, UpdateSkippedSubTreeProp_002, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodePtr = std::make_shared<RSRenderNode>(firstNodeId);
    std::shared_ptr<RSOcclusionHandler> rsOcclusionHandler =
        std::make_shared<RSOcclusionHandler>(id);
    nodePtr->instanceRootNodeId_ = firstNodeId;
    rsOcclusionHandler->rootNodeId_ = firstNodeId;
    rsOcclusionHandler->CollectNode(*nodePtr);
    std::shared_ptr<RSRenderNode> nodePtr2 = std::make_shared<RSRenderNode>(secondNodeId);
    nodePtr2->parent_ = nodePtr;
    nodePtr2->instanceRootNodeId_ = firstNodeId;
    rsOcclusionHandler->CollectSubTree(*nodePtr2, false);
    auto it = rsOcclusionHandler->occlusionNodes_.find(nodePtr2->id_);
    ASSERT_NE(it, rsOcclusionHandler->occlusionNodes_.end());
    it->second->isSubTreeIgnored_ = false;
    rsOcclusionHandler->CollectSubTree(*nodePtr2, false);
    // when isSubTreeIgnored_ is false then subTreeSkipPrepareNodes_ is not empty after method CollectSubTree
    EXPECT_FALSE(rsOcclusionHandler->subTreeSkipPrepareNodes_.empty());
    rsOcclusionHandler->UpdateSkippedSubTreeProp();
    EXPECT_TRUE(rsOcclusionHandler->subTreeSkipPrepareNodes_.empty());
}

/*
 * @tc.name: CalculateFrameOcclusion_001
 * @tc.desc: Test CalculateFrameOcclusion
 * @tc.type: FUNC
 * @tc.require: issueIC2H2
 */
HWTEST_F(RSOcclusionHandlerTest, CalculateFrameOcclusion_001, TestSize.Level1)
{
    std::shared_ptr<RSOcclusionHandler> rsOcclusionHandler =
        std::make_shared<RSOcclusionHandler>(id);
    rsOcclusionHandler->CalculateFrameOcclusion();
    int expectSize = 0;
    EXPECT_EQ(rsOcclusionHandler->culledNodes_.size(), expectSize);
}

/*
 * @tc.name: GetRootNodeId_001
 * @tc.desc: Test GetRootNodeId
 * @tc.type: FUNC
 * @tc.require: issueIC2H2
 */
HWTEST_F(RSOcclusionHandlerTest, GetRootNodeId_001, TestSize.Level1)
{
    std::shared_ptr<RSOcclusionHandler> rsOcclusionHandler =
        std::make_shared<RSOcclusionHandler>(id);
    EXPECT_EQ(rsOcclusionHandler->GetRootNodeId(), id);
}
}  // namespace OHOS::Rosen