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
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSOcclusionHandlerTest::SetUpTestCase() {}
void RSOcclusionHandlerTest::TearDownTestCase() {}
void RSOcclusionHandlerTest::SetUp() {}
void RSOcclusionHandlerTest::TearDown() {}

/*
 * @tc.name: UpdateOcclusionCullingStatus_001
 * @tc.desc: Test UpdateOcclusionCullingStatus
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOcclusionHandlerTest, UpdateOcclusionCullingStatus_001, TestSize.Level1)
{
    NodeId id(0);
    NodeId keyOcclusionNodeId(0);
    std::shared_ptr<RSOcclusionHandler> rsOcclusionHandler_ =
        std::make_shared<RSOcclusionHandler>(id, keyOcclusionNodeId);
    rsOcclusionHandler_->isOcclusionActive_ = true;
    rsOcclusionHandler_->UpdateOcclusionCullingStatus(false, keyOcclusionNodeId);
    ASSERT_FALSE(rsOcclusionHandler_->isOcclusionActive_);
    ASSERT_EQ(0, rsOcclusionHandler_->keyOcclusionNodeId_);

    OcclusionCullingParam::SetIntraAppControlsLevelOcclusionCullingEnable(true);
    keyOcclusionNodeId = 1;
    rsOcclusionHandler_->UpdateOcclusionCullingStatus(true, keyOcclusionNodeId);
    ASSERT_TRUE(rsOcclusionHandler_->isOcclusionActive_);
    ASSERT_EQ(1, rsOcclusionHandler_->keyOcclusionNodeId_);

    OcclusionCullingParam::SetIntraAppControlsLevelOcclusionCullingEnable(false);
    rsOcclusionHandler_->UpdateOcclusionCullingStatus(true, keyOcclusionNodeId);
    ASSERT_FALSE(rsOcclusionHandler_->isOcclusionActive_);
    ASSERT_EQ(1, rsOcclusionHandler_->keyOcclusionNodeId_);
}

/*
 * @tc.name: TerminateOcclusionProcessing_001
 * @tc.desc: Test TerminateOcclusionProcessing
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOcclusionHandlerTest, TerminateOcclusionProcessing_001, TestSize.Level1)
{
    NodeId id(0);
    NodeId keyOcclusionNodeId(0);
    std::shared_ptr<RSOcclusionHandler> rsOcclusionHandler_ =
        std::make_shared<RSOcclusionHandler>(id, keyOcclusionNodeId);
    rsOcclusionHandler_->TerminateOcclusionProcessing();
    ASSERT_FALSE(rsOcclusionHandler_->isOcclusionActive_);
    ASSERT_EQ(keyOcclusionNodeId, rsOcclusionHandler_->keyOcclusionNodeId_);
    int expectSize = 0;
    ASSERT_EQ(expectSize, rsOcclusionHandler_->subTreeSkipPrepareNodes_.size());
    ASSERT_EQ(expectSize, rsOcclusionHandler_->occlusionNodes_.size());
    ASSERT_EQ(nullptr, rsOcclusionHandler_->rootOcclusionNode_);
    ASSERT_EQ(nullptr, rsOcclusionHandler_->rootNode_.lock());
}

/*
 * @tc.name: CollectNode_001
 * @tc.desc: Test CollectNode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOcclusionHandlerTest, CollectNode_001, TestSize.Level1)
{
    NodeId nodeId(1);
    RSRenderNode node = RSRenderNode(nodeId);
    NodeId id(0);
    NodeId keyOcclusionNodeId(0);
    std::shared_ptr<RSOcclusionHandler> rsOcclusionHandler_ =
        std::make_shared<RSOcclusionHandler>(id, keyOcclusionNodeId);
    rsOcclusionHandler_->isOcclusionActive_ = false;
    rsOcclusionHandler_->CollectNode(node);
    ASSERT_EQ(rsOcclusionHandler_->rootOcclusionNode_, nullptr);
}

/*
 * @tc.name: CollectNode_002
 * @tc.desc: Test CollectNode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOcclusionHandlerTest, CollectNode_002, TestSize.Level1)
{
    NodeId nodeId(1);
    RSRenderNode node = RSRenderNode(nodeId);
    NodeId id(0);
    NodeId keyOcclusionNodeId(0);
    std::shared_ptr<RSOcclusionHandler> rsOcclusionHandler_ =
        std::make_shared<RSOcclusionHandler>(id, keyOcclusionNodeId);
    rsOcclusionHandler_->isOcclusionActive_ = true;
    rsOcclusionHandler_->CollectNode(node);
    ASSERT_EQ(rsOcclusionHandler_->rootOcclusionNode_, nullptr);
}

/*
 * @tc.name: CollectNode_003
 * @tc.desc: Test CollectNode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOcclusionHandlerTest, CollectNode_003, TestSize.Level1)
{
    NodeId nodeId(1);
    RSRenderNode node(nodeId);
    node.instanceRootNodeId_ = nodeId;
    NodeId id(0);
    NodeId keyOcclusionNodeId(0);
    std::shared_ptr<RSOcclusionHandler> rsOcclusionHandler_ =
        std::make_shared<RSOcclusionHandler>(id, keyOcclusionNodeId);
    rsOcclusionHandler_->isOcclusionActive_ = true;
    rsOcclusionHandler_->rootNodeId_ = nodeId;
    rsOcclusionHandler_->CollectNode(node);
    EXPECT_EQ(rsOcclusionHandler_->rootOcclusionNode_->id_, nodeId);
}

/*
 * @tc.name: CollectNode_004
 * @tc.desc: Test CollectNode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOcclusionHandlerTest, CollectNode_004, TestSize.Level1)
{
    NodeId firstNodeId(1);
    std::shared_ptr<RSRenderNode> nodePtr = std::make_shared<RSRenderNode>(firstNodeId);
    nodePtr->instanceRootNodeId_ = firstNodeId;
    NodeId id(0);
    NodeId keyOcclusionNodeId(0);
    std::shared_ptr<RSOcclusionHandler> rsOcclusionHandler_ =
        std::make_shared<RSOcclusionHandler>(id, keyOcclusionNodeId);
    rsOcclusionHandler_->isOcclusionActive_ = true;
    rsOcclusionHandler_->rootNodeId_ = firstNodeId;
    rsOcclusionHandler_->CollectNode(*nodePtr);
    NodeId secondNodeId(2);
    std::shared_ptr<RSRenderNode> nodePtr2 = std::make_shared<RSRenderNode>(secondNodeId);
    nodePtr2->parent_ = nodePtr;
    nodePtr2->instanceRootNodeId_ = firstNodeId;
    rsOcclusionHandler_->CollectNode(*nodePtr2);
    EXPECT_EQ(nodePtr2->GetId(), secondNodeId);
    EXPECT_NE(rsOcclusionHandler_->occlusionNodes_.find(nodePtr2->GetId()), rsOcclusionHandler_->occlusionNodes_.end());
    EXPECT_EQ(rsOcclusionHandler_->rootOcclusionNode_->id_, firstNodeId);
    EXPECT_EQ(rsOcclusionHandler_->rootOcclusionNode_->firstChild_->id_, secondNodeId);
}

/*
 * @tc.name: ProcessOffTreeNodes_001
 * @tc.desc: Test ProcessOffTreeNodes
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOcclusionHandlerTest, ProcessOffTreeNodes_001, TestSize.Level1)
{
    NodeId firstNodeId(1);
    std::shared_ptr<RSRenderNode> nodePtr = std::make_shared<RSRenderNode>(firstNodeId);
    NodeId id(0);
    NodeId keyOcclusionNodeId(0);
    std::shared_ptr<RSOcclusionHandler> rsOcclusionHandler_ =
        std::make_shared<RSOcclusionHandler>(id, keyOcclusionNodeId);
    rsOcclusionHandler_->isOcclusionActive_ = true;
    rsOcclusionHandler_->rootNodeId_ = firstNodeId;
    rsOcclusionHandler_->CollectNode(*nodePtr);
    NodeId secondNodeId(2);
    std::shared_ptr<RSRenderNode> nodePtr2 = std::make_shared<RSRenderNode>(secondNodeId);
    nodePtr2->parent_ = nodePtr;
    rsOcclusionHandler_->CollectNode(*nodePtr2);
    std::unordered_set<uint64_t> ids = {secondNodeId};
    rsOcclusionHandler_->ProcessOffTreeNodes(ids);
    EXPECT_EQ(rsOcclusionHandler_->occlusionNodes_.find(secondNodeId), rsOcclusionHandler_->occlusionNodes_.end());
    EXPECT_EQ(rsOcclusionHandler_->occlusionNodes_.size(), firstNodeId);
}

/*
 * @tc.name: CollectSubTree_001
 * @tc.desc: Test CollectSubTree
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOcclusionHandlerTest, CollectSubTree_001, TestSize.Level1)
{
    NodeId firstNodeId(1);
    std::shared_ptr<RSRenderNode> nodePtr = std::make_shared<RSRenderNode>(firstNodeId);
    nodePtr->instanceRootNodeId_ = firstNodeId;
    NodeId id(0);
    NodeId keyOcclusionNodeId(0);
    std::shared_ptr<RSOcclusionHandler> rsOcclusionHandler_ =
        std::make_shared<RSOcclusionHandler>(id, keyOcclusionNodeId);

    rsOcclusionHandler_->isOcclusionActive_ = false;
    rsOcclusionHandler_->CollectSubTree(*nodePtr, false);
    int expectSize = 0;
    EXPECT_EQ(rsOcclusionHandler_->occlusionNodes_.size(), expectSize);

    rsOcclusionHandler_->isOcclusionActive_ = true;
    rsOcclusionHandler_->CollectSubTree(*nodePtr, true);
    EXPECT_EQ(rsOcclusionHandler_->occlusionNodes_.size(), expectSize);

    nodePtr->instanceRootNodeId_ = 2;
    rsOcclusionHandler_->CollectSubTree(*nodePtr, false);
    EXPECT_EQ(rsOcclusionHandler_->occlusionNodes_.size(), expectSize);

    nodePtr->instanceRootNodeId_ = firstNodeId;
    rsOcclusionHandler_->isOcclusionActive_ = true;
    rsOcclusionHandler_->rootNodeId_ = firstNodeId;
    rsOcclusionHandler_->CollectNode(*nodePtr);
    NodeId secondNodeId(2);
    std::shared_ptr<RSRenderNode> nodePtr2 = std::make_shared<RSRenderNode>(secondNodeId);
    nodePtr2->parent_ = nodePtr;
    nodePtr2->instanceRootNodeId_ = firstNodeId;
    rsOcclusionHandler_->CollectSubTree(*nodePtr2, false);
    EXPECT_EQ(rsOcclusionHandler_->occlusionNodes_.size(), 2);
    EXPECT_EQ(rsOcclusionHandler_->occlusionNodes_.count(nodePtr2->id_), 1);

    auto it = rsOcclusionHandler_->occlusionNodes_.find(nodePtr2->id_);
    if (it != rsOcclusionHandler_->occlusionNodes_.end()) {
        it->second->isSubTreeIgnored_ = false;
        rsOcclusionHandler_->CalculateFrameOcclusion();
        rsOcclusionHandler_->CollectSubTree(*nodePtr2, false);
        EXPECT_NE(rsOcclusionHandler_->subTreeSkipPrepareNodes_.find(nodePtr2->id_),
                  rsOcclusionHandler_->subTreeSkipPrepareNodes_.end());
    }
}

/*
 * @tc.name: UpdateChildrenOutOfRectInfo_001
 * @tc.desc: Test UpdateChildrenOutOfRectInfo
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOcclusionHandlerTest, UpdateChildrenOutOfRectInfo_001, TestSize.Level1)
{
    NodeId nodeId(1);
    std::shared_ptr<RSRenderNode> nodePtr = std::make_shared<RSRenderNode>(nodeId);
    NodeId id(0);
    NodeId keyOcclusionNodeId(0);
    std::shared_ptr<RSOcclusionHandler> rsOcclusionHandler_ =
        std::make_shared<RSOcclusionHandler>(id, keyOcclusionNodeId);
    nodePtr->instanceRootNodeId_ = nodeId;
    rsOcclusionHandler_->isOcclusionActive_ = true;
    rsOcclusionHandler_->rootNodeId_ = nodeId;
    rsOcclusionHandler_->CollectNode(*nodePtr);
    int expectSize = 0;
    EXPECT_NE(rsOcclusionHandler_->occlusionNodes_.size(), expectSize);
    auto it = rsOcclusionHandler_->occlusionNodes_.find(nodePtr->id_);
    EXPECT_NE(rsOcclusionHandler_->occlusionNodes_.end(), it);

    rsOcclusionHandler_->UpdateChildrenOutOfRectInfo(*nodePtr);
    EXPECT_FALSE(it->second->hasChildrenOutOfRect_);
}

/*
 * @tc.name: UpdateSkippedSubTreeProp_001
 * @tc.desc: Test UpdateSkippedSubTreeProp
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOcclusionHandlerTest, UpdateSkippedSubTreeProp_001, TestSize.Level1)
{
    NodeId firstNodeId(1);
    std::shared_ptr<RSRenderNode> nodePtr = std::make_shared<RSRenderNode>(firstNodeId);
    NodeId id(0);
    NodeId keyOcclusionNodeId(0);
    std::shared_ptr<RSOcclusionHandler> rsOcclusionHandler_ =
        std::make_shared<RSOcclusionHandler>(id, keyOcclusionNodeId);
    nodePtr->instanceRootNodeId_ = firstNodeId;
    rsOcclusionHandler_->isOcclusionActive_ = true;
    rsOcclusionHandler_->rootNodeId_ = firstNodeId;
    rsOcclusionHandler_->CollectNode(*nodePtr);
    NodeId secondNodeId(2);
    std::shared_ptr<RSRenderNode> nodePtr2 = std::make_shared<RSRenderNode>(secondNodeId);
    nodePtr2->parent_ = nodePtr;
    nodePtr2->instanceRootNodeId_ = firstNodeId;
    rsOcclusionHandler_->CollectSubTree(*nodePtr2, false);
    int expectCount = 1;
    int expectSize = 0;
    EXPECT_EQ(rsOcclusionHandler_->occlusionNodes_.size(), secondNodeId);
    EXPECT_EQ(rsOcclusionHandler_->occlusionNodes_.count(nodePtr2->id_), expectCount);
    auto it = rsOcclusionHandler_->occlusionNodes_.find(nodePtr2->id_);
    it->second->isSubTreeIgnored_ = false;
    rsOcclusionHandler_->CalculateFrameOcclusion();
    rsOcclusionHandler_->CollectSubTree(*nodePtr2, false);
    EXPECT_NE(rsOcclusionHandler_->subTreeSkipPrepareNodes_.find(nodePtr2->id_),
              rsOcclusionHandler_->subTreeSkipPrepareNodes_.end());
    rsOcclusionHandler_->UpdateSkippedSubTreeProp();
    EXPECT_EQ(rsOcclusionHandler_->subTreeSkipPrepareNodes_.size(), expectSize);
}

/*
 * @tc.name: CalculateFrameOcclusion_001
 * @tc.desc: Test CalculateFrameOcclusion
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOcclusionHandlerTest, CalculateFrameOcclusion_001, TestSize.Level1)
{
    NodeId id(0);
    NodeId keyOcclusionNodeId(0);
    std::shared_ptr<RSOcclusionHandler> rsOcclusionHandler_ =
        std::make_shared<RSOcclusionHandler>(id, keyOcclusionNodeId);
    rsOcclusionHandler_->isOcclusionActive_ = true;
    rsOcclusionHandler_->CalculateFrameOcclusion();
    int expectSize = 0;
    EXPECT_EQ(rsOcclusionHandler_->culledNodes_.size(), expectSize);
}

/*
 * @tc.name: GetRootNodeId_001
 * @tc.desc: Test GetRootNodeId
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOcclusionHandlerTest, GetRootNodeId_001, TestSize.Level1)
{
    NodeId id(0);
    NodeId keyOcclusionNodeId(0);
    std::shared_ptr<RSOcclusionHandler> rsOcclusionHandler_ =
        std::make_shared<RSOcclusionHandler>(id, keyOcclusionNodeId);
    EXPECT_EQ(rsOcclusionHandler_->GetRootNodeId(), id);
}

/*
 * @tc.name: UpdateKeyOcclusionNodeId_001
 * @tc.desc: Test UpdateKeyOcclusionNodeId
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOcclusionHandlerTest, UpdateKeyOcclusionNodeId_001, TestSize.Level1)
{
    NodeId id(0);
    NodeId keyOcclusionNodeId(0);
    std::shared_ptr<RSOcclusionHandler> rsOcclusionHandler_ =
        std::make_shared<RSOcclusionHandler>(id, keyOcclusionNodeId);
    keyOcclusionNodeId = 1;
    rsOcclusionHandler_->UpdateKeyOcclusionNodeId(keyOcclusionNodeId);
    EXPECT_EQ(rsOcclusionHandler_->keyOcclusionNodeId_, keyOcclusionNodeId);
}

/*
 * @tc.name: IsOcclusionActive_001
 * @tc.desc: Test IsOcclusionActive
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOcclusionHandlerTest, IsOcclusionActive_001, TestSize.Level1)
{
    NodeId id(0);
    NodeId keyOcclusionNodeId(0);
    std::shared_ptr<RSOcclusionHandler> rsOcclusionHandler_ =
        std::make_shared<RSOcclusionHandler>(id, keyOcclusionNodeId);
    rsOcclusionHandler_->isOcclusionActive_ = true;
    EXPECT_EQ(rsOcclusionHandler_->IsOcclusionActive(), true);
}

}  // namespace OHOS::Rosen