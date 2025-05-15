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

/*
 * @tc.name: UpdateOcclusionCullingStatus_001
 * @tc.desc: Test UpdateOcclusionCullingStatus
 * @tc.type: FUNC
 * @tc.require: issueIC2H2
 */
HWTEST_F(RSOcclusionHandlerTest, UpdateOcclusionCullingStatus_001, TestSize.Level1)
{
    std::shared_ptr<RSOcclusionHandler> rsOcclusionHandler =
        std::make_shared<RSOcclusionHandler>(id, keyOcclusionNodeId);
    rsOcclusionHandler->isOcclusionActive_ = true;
    rsOcclusionHandler->UpdateOcclusionCullingStatus(false, keyOcclusionNodeId);
    ASSERT_FALSE(rsOcclusionHandler->isOcclusionActive_);
    ASSERT_EQ(keyOcclusionNodeId, rsOcclusionHandler->keyOcclusionNodeId_);

    OcclusionCullingParam::SetIntraAppControlsLevelOcclusionCullingEnable(true);
    NodeId keyOcclusionNodeIdNew = 1;
    rsOcclusionHandler->UpdateOcclusionCullingStatus(true, keyOcclusionNodeIdNew);
    ASSERT_TRUE(rsOcclusionHandler->isOcclusionActive_);
    ASSERT_EQ(keyOcclusionNodeIdNew, rsOcclusionHandler->keyOcclusionNodeId_);

    OcclusionCullingParam::SetIntraAppControlsLevelOcclusionCullingEnable(false);
    rsOcclusionHandler->UpdateOcclusionCullingStatus(true, keyOcclusionNodeIdNew);
    ASSERT_FALSE(rsOcclusionHandler->isOcclusionActive_);
    ASSERT_EQ(keyOcclusionNodeIdNew, rsOcclusionHandler->keyOcclusionNodeId_);
}

/*
 * @tc.name: TerminateOcclusionProcessing_001
 * @tc.desc: Test TerminateOcclusionProcessing
 * @tc.type: FUNC
 * @tc.require: issueIC2H2
 */
HWTEST_F(RSOcclusionHandlerTest, TerminateOcclusionProcessing_001, TestSize.Level1)
{
    std::shared_ptr<RSOcclusionHandler> rsOcclusionHandler =
        std::make_shared<RSOcclusionHandler>(id, keyOcclusionNodeId);
    rsOcclusionHandler->TerminateOcclusionProcessing();
    ASSERT_FALSE(rsOcclusionHandler->isOcclusionActive_);
    ASSERT_EQ(keyOcclusionNodeId, rsOcclusionHandler->keyOcclusionNodeId_);
    int expectSize = 0;
    ASSERT_EQ(expectSize, rsOcclusionHandler->subTreeSkipPrepareNodes_.size());
    ASSERT_EQ(expectSize, rsOcclusionHandler->occlusionNodes_.size());
    ASSERT_EQ(nullptr, rsOcclusionHandler->rootOcclusionNode_);
    ASSERT_EQ(nullptr, rsOcclusionHandler->rootNode_.lock());
}

/*
 * @tc.name: CollectNode_001
 * @tc.desc: Test CollectNode when property isOcclusionActive_ is false
 * @tc.type: FUNC
 * @tc.require: issueIC2H2
 */
HWTEST_F(RSOcclusionHandlerTest, CollectNode_001, TestSize.Level1)
{
    RSRenderNode node = RSRenderNode(nodeId);
    std::shared_ptr<RSOcclusionHandler> rsOcclusionHandler =
        std::make_shared<RSOcclusionHandler>(id, keyOcclusionNodeId);
    rsOcclusionHandler->isOcclusionActive_ = false;
    rsOcclusionHandler->CollectNode(node);
    ASSERT_EQ(rsOcclusionHandler->rootOcclusionNode_, nullptr);
}

/*
 * @tc.name: CollectNode_002
 * @tc.desc: Test CollectNode when the node id and rootNodeId are not equal
 * @tc.type: FUNC
 * @tc.require: issueIC2H2
 */
HWTEST_F(RSOcclusionHandlerTest, CollectNode_002, TestSize.Level1)
{
    RSRenderNode node = RSRenderNode(nodeId);
    std::shared_ptr<RSOcclusionHandler> rsOcclusionHandler =
        std::make_shared<RSOcclusionHandler>(id, keyOcclusionNodeId);
    rsOcclusionHandler->isOcclusionActive_ = true;
    rsOcclusionHandler->CollectNode(node);
    ASSERT_EQ(rsOcclusionHandler->rootOcclusionNode_, nullptr);
}

/*
 * @tc.name: CollectNode_003
 * @tc.desc: Test CollectNode when node is rootNode
 * @tc.type: FUNC
 * @tc.require: issueIC2H2
 */
HWTEST_F(RSOcclusionHandlerTest, CollectNode_003, TestSize.Level1)
{
    RSRenderNode node(nodeId);
    node.instanceRootNodeId_ = nodeId;
    std::shared_ptr<RSOcclusionHandler> rsOcclusionHandler =
        std::make_shared<RSOcclusionHandler>(id, keyOcclusionNodeId);
    rsOcclusionHandler->isOcclusionActive_ = true;
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
HWTEST_F(RSOcclusionHandlerTest, CollectNode_004, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodePtr = std::make_shared<RSRenderNode>(firstNodeId);
    nodePtr->instanceRootNodeId_ = firstNodeId;
    std::shared_ptr<RSOcclusionHandler> rsOcclusionHandler =
        std::make_shared<RSOcclusionHandler>(id, keyOcclusionNodeId);
    rsOcclusionHandler->isOcclusionActive_ = true;
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
        std::make_shared<RSOcclusionHandler>(id, keyOcclusionNodeId);
    rsOcclusionHandler->isOcclusionActive_ = true;
    rsOcclusionHandler->rootNodeId_ = firstNodeId;
    rsOcclusionHandler->CollectNode(*nodePtr);
    std::shared_ptr<RSRenderNode> nodePtr2 = std::make_shared<RSRenderNode>(secondNodeId);
    nodePtr2->parent_ = nodePtr;
    rsOcclusionHandler->CollectNode(*nodePtr2);
    std::unordered_set<uint64_t> ids = {secondNodeId};
    rsOcclusionHandler->ProcessOffTreeNodes(ids);
    EXPECT_EQ(rsOcclusionHandler->occlusionNodes_.find(secondNodeId), rsOcclusionHandler->occlusionNodes_.end());
    EXPECT_EQ(rsOcclusionHandler->occlusionNodes_.size(), firstNodeId);
}

/*
 * @tc.name: CollectSubTree_001
 * @tc.desc: Test CollectSubTree when isOcclusionActive is false
 * @tc.type: FUNC
 * @tc.require: issueIC2H2
 */
HWTEST_F(RSOcclusionHandlerTest, CollectSubTree_001, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodePtr = std::make_shared<RSRenderNode>(firstNodeId);
    nodePtr->instanceRootNodeId_ = firstNodeId;
    std::shared_ptr<RSOcclusionHandler> rsOcclusionHandler =
        std::make_shared<RSOcclusionHandler>(id, keyOcclusionNodeId);
    // set isOcclusionActive_ flag to false, then occlusionNodes_ collection will be empty
    rsOcclusionHandler->isOcclusionActive_ = false;
    rsOcclusionHandler->CollectSubTree(*nodePtr, false);
    int expectSize = 0;
    EXPECT_EQ(rsOcclusionHandler->occlusionNodes_.size(), expectSize);
}

/*
 * @tc.name: CollectSubTree_002
 * @tc.desc: Test CollectSubTree where isSubTreeNeedPrepare is true
 * @tc.type: FUNC
 * @tc.require: issueIC2H2
 */
HWTEST_F(RSOcclusionHandlerTest, CollectSubTree_002, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodePtr = std::make_shared<RSRenderNode>(firstNodeId);
    nodePtr->instanceRootNodeId_ = firstNodeId;
    std::shared_ptr<RSOcclusionHandler> rsOcclusionHandler =
        std::make_shared<RSOcclusionHandler>(id, keyOcclusionNodeId);
    // set the second param to true, then occlusionNodes_ collection will be empty
    rsOcclusionHandler->isOcclusionActive_ = true;
    rsOcclusionHandler->CollectSubTree(*nodePtr, true);
    int expectSize = 0;
    EXPECT_EQ(rsOcclusionHandler->occlusionNodes_.size(), expectSize);
}

/*
 * @tc.name: CollectSubTree_003
 * @tc.desc: Test CollectSubTree in non-anomalous situations and the node and its subtree were correctly collected.
 * @tc.type: FUNC
 * @tc.require: issueIC2H2
 */
HWTEST_F(RSOcclusionHandlerTest, CollectSubTree_003, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodePtr = std::make_shared<RSRenderNode>(firstNodeId);
    nodePtr->instanceRootNodeId_ = firstNodeId;
    std::shared_ptr<RSOcclusionHandler> rsOcclusionHandler =
        std::make_shared<RSOcclusionHandler>(id, keyOcclusionNodeId);
    rsOcclusionHandler->rootNodeId_ = firstNodeId;
    rsOcclusionHandler->isOcclusionActive_ = true;
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
        std::make_shared<RSOcclusionHandler>(id, keyOcclusionNodeId);
    nodePtr->instanceRootNodeId_ = nodeId;
    rsOcclusionHandler->isOcclusionActive_ = true;
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
        std::make_shared<RSOcclusionHandler>(id, keyOcclusionNodeId);
    rsOcclusionHandler->isOcclusionActive_ = true;
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
        std::make_shared<RSOcclusionHandler>(id, keyOcclusionNodeId);
    nodePtr->instanceRootNodeId_ = firstNodeId;
    rsOcclusionHandler->isOcclusionActive_ = true;
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
        std::make_shared<RSOcclusionHandler>(id, keyOcclusionNodeId);
    rsOcclusionHandler->isOcclusionActive_ = true;
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
        std::make_shared<RSOcclusionHandler>(id, keyOcclusionNodeId);
    EXPECT_EQ(rsOcclusionHandler->GetRootNodeId(), id);
}

/*
 * @tc.name: UpdateKeyOcclusionNodeId_001
 * @tc.desc: Test UpdateKeyOcclusionNodeId
 * @tc.type: FUNC
 * @tc.require: issueIC2H2
 */
HWTEST_F(RSOcclusionHandlerTest, UpdateKeyOcclusionNodeId_001, TestSize.Level1)
{
    std::shared_ptr<RSOcclusionHandler> rsOcclusionHandler =
        std::make_shared<RSOcclusionHandler>(id, keyOcclusionNodeId);
    NodeId keyOcclusionNodeIdNew = 1;
    rsOcclusionHandler->UpdateKeyOcclusionNodeId(keyOcclusionNodeIdNew);
    EXPECT_EQ(rsOcclusionHandler->keyOcclusionNodeId_, keyOcclusionNodeIdNew);
}

/*
 * @tc.name: IsOcclusionActive_001
 * @tc.desc: Test IsOcclusionActive
 * @tc.type: FUNC
 * @tc.require: issueIC2H2
 */
HWTEST_F(RSOcclusionHandlerTest, IsOcclusionActive_001, TestSize.Level1)
{
    std::shared_ptr<RSOcclusionHandler> rsOcclusionHandler =
        std::make_shared<RSOcclusionHandler>(id, keyOcclusionNodeId);
    rsOcclusionHandler->isOcclusionActive_ = true;
    EXPECT_TRUE(rsOcclusionHandler->IsOcclusionActive());
}

}  // namespace OHOS::Rosen