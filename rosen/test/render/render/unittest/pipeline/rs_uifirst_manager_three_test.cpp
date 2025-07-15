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
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_uifirst_manager.h"
#include "rs_test_util.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::DrawableV2;

namespace OHOS::Rosen {
class RSUifirstManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline RectI DEFAULT_RECT = {0, 0, 10, 10};
    static inline uint8_t GROUPED_BY_ANIM = 1;
    static inline NodeId INVALID_NODEID = 0xFFFF;
    static inline RSMainThread* mainThread_;
    static inline RSUifirstManager& uifirstManager_ = RSUifirstManager::Instance();
    static inline Occlusion::Region DEFAULT_VISIBLE_REGION = Occlusion::Rect(0, 0, 10, 10);
    static inline RectI VISIBLE_DIRTY_REGION = {0, 0, 10, 10};
    static inline RectI INVISIBLE_DIRTY_REGION = {20, 20, 10, 10};
};

/**
 * @tc.name: UifirstStateChange
 * @tc.desc: Test UifirstStateChange, not enabled case.
 * @tc.type: FUNC
 * @tc.require: #ICLUN7
 */
HWTEST_F(RSUifirstManagerTest, UifirstStateChange, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    // not support cache type switch, just disable multithread cache
    surfaceNode->SetLastFrameUifirstFlag(MultiThreadCacheType::LEASH_WINDOW);
    auto currentFrameCacheType = MultiThreadCacheType::NONFOCUS_WINDOW;
    uifirstManager_.UifirstStateChange(*surfaceNode, currentFrameCacheType);
    ASSERT_EQ(surfaceNode->GetLastFrameUifirstFlag(), MultiThreadCacheType::NONE);
}
 
/**
 * @tc.name: CollectSkipSyncNode001
 * @tc.desc: Test CollectSkipSyncNode
 * @tc.type: FUNC
 * @tc.require: issueICLUN7
 */
HWTEST_F(RSUifirstManagerTest, CollectSkipSyncNode001, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> node = nullptr;
    bool res = uifirstManager_.CollectSkipSyncNode(node);
    EXPECT_FALSE(res);

    node = std::make_shared<RSRenderNode>(++RSTestUtil::id);
    DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(node);
    uifirstManager_.pendingPostCardNodes_.clear();
    res = uifirstManager_.CollectSkipSyncNode(node);
    auto renderNode = std::make_shared<RSSurfaceRenderNode>(1);
    uifirstManager_.pendingPostCardNodes_.insert(std::make_pair(1, renderNode));
    res = uifirstManager_.CollectSkipSyncNode(node);
    node->id_ = 1;
    res = uifirstManager_.CollectSkipSyncNode(node);
    node->id_ = 0;
    res = uifirstManager_.CollectSkipSyncNode(node);
    EXPECT_FALSE(res);

    uifirstManager_.entryViewNodeId_ = 1;
    uifirstManager_.negativeScreenNodeId_ = 1;
    node->instanceRootNodeId_ = 1;
    uifirstManager_.processingCardNodeSkipSync_.clear();
    uifirstManager_.processingCardNodeSkipSync_.insert(0);
    res = uifirstManager_.CollectSkipSyncNode(node);
    EXPECT_FALSE(res);

    node->uifirstRootNodeId_ = 1;
    uifirstManager_.processingCardNodeSkipSync_.insert(1);
    res = uifirstManager_.CollectSkipSyncNode(node);
    EXPECT_TRUE(res);

    uifirstManager_.entryViewNodeId_ = 0;
    uifirstManager_.processingNodePartialSync_.clear();
    uifirstManager_.processingNodePartialSync_.insert(0);
    res = uifirstManager_.CollectSkipSyncNode(node);
    EXPECT_FALSE(res);

    uifirstManager_.processingNodePartialSync_.insert(1);
    node->instanceRootNodeId_ = 1;
    res = uifirstManager_.CollectSkipSyncNode(node);
    EXPECT_TRUE(res);

    uifirstManager_.processingNodePartialSync_.clear();
    uifirstManager_.processingNodeSkipSync_.insert(1);
    res = uifirstManager_.CollectSkipSyncNode(node);
    EXPECT_TRUE(res);
    node->renderDrawable_ = nullptr;
}

/**
 * @tc.name: RestoreSkipSyncNode001
 * @tc.desc: Test RestoreSkipSyncNode
 * @tc.type: FUNC
 * @tc.require: issueICLUN7
 */
HWTEST_F(RSUifirstManagerTest, RestoreSkipSyncNode001, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(0);
    std::vector<std::shared_ptr<RSRenderNode>> renderNode;
    renderNode.push_back(node);

    uifirstManager_.pendingSyncForSkipBefore_.clear();
    uifirstManager_.pendingSyncForSkipBefore_.insert(std::make_pair(1, renderNode));
    uifirstManager_.RestoreSkipSyncNode();
    EXPECT_TRUE(uifirstManager_.pendingSyncForSkipBefore_.size() == 1);

    uifirstManager_.processingNodeSkipSync_.clear();
    uifirstManager_.processingNodeSkipSync_.insert(0);
    uifirstManager_.RestoreSkipSyncNode();
    EXPECT_TRUE(uifirstManager_.processingNodeSkipSync_.size() == 1);

    uifirstManager_.processingNodePartialSync_.clear();
    uifirstManager_.processingNodePartialSync_.insert(0);
    uifirstManager_.RestoreSkipSyncNode();
    EXPECT_TRUE(uifirstManager_.processingNodePartialSync_.size() == 1);

    uifirstManager_.processingCardNodeSkipSync_.clear();
    uifirstManager_.processingCardNodeSkipSync_.insert(0);
    uifirstManager_.RestoreSkipSyncNode();
    EXPECT_TRUE(uifirstManager_.processingCardNodeSkipSync_.size() == 1);

    uifirstManager_.processingCardNodeSkipSync_.clear();
    uifirstManager_.RestoreSkipSyncNode();
    EXPECT_TRUE(uifirstManager_.processingCardNodeSkipSync_.size() == 0);

    uifirstManager_.processingNodePartialSync_.clear();
    uifirstManager_.RestoreSkipSyncNode();
    EXPECT_TRUE(uifirstManager_.processingNodePartialSync_.size() == 0);

    uifirstManager_.processingNodeSkipSync_.clear();
    uifirstManager_.RestoreSkipSyncNode();
    EXPECT_TRUE(uifirstManager_.processingNodeSkipSync_.size() == 0);
}
 
/**
* @tc.name: UifirstStateChange
* @tc.desc: Test UifirstStateChange, last frame enabled, this frame disabled
* @tc.type: FUNC
* @tc.require: #ICLUN7
*/
HWTEST_F(RSUifirstManagerTest, UifirstStateChange004, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);

    surfaceNode->SetLastFrameUifirstFlag(MultiThreadCacheType::LEASH_WINDOW);
    auto currentFrameCacheType = MultiThreadCacheType::NONE;
    uifirstManager_.UifirstStateChange(*surfaceNode, currentFrameCacheType);
    ASSERT_EQ(surfaceNode->GetLastFrameUifirstFlag(), MultiThreadCacheType::NONE);
}

/**
* @tc.name: CheckIfAppWindowHasAnimation
* @tc.desc: Test CheckIfAppWindowHasAnimation
* @tc.type: FUNC
* @tc.require: #ICLUN7
*/
HWTEST_F(RSUifirstManagerTest, CheckIfAppWindowHasAnimation, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);

    surfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::ABILITY_COMPONENT_NODE);
    ASSERT_FALSE(uifirstManager_.CheckIfAppWindowHasAnimation(*surfaceNode));
}
 
 /**
  * @tc.name: CheckIfAppWindowHasAnimation
  * @tc.desc: Test CheckIfAppWindowHasAnimation, currentFrameEvent_ is empty or node is not leash/app window
  * @tc.type: FUNC
  * @tc.require: #ICLUN7
  */
 HWTEST_F(RSUifirstManagerTest, CheckIfAppWindowHasAnimation001, TestSize.Level1)
 {
     auto surfaceNode = RSTestUtil::CreateSurfaceNode();
     ASSERT_NE(surfaceNode, nullptr);
     ASSERT_FALSE(uifirstManager_.CheckIfAppWindowHasAnimation(*surfaceNode));
 
     RSUifirstManager::EventInfo event;
     uifirstManager_.currentFrameEvent_.push_back(event);
     surfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::ABILITY_COMPONENT_NODE);
     ASSERT_FALSE(uifirstManager_.CheckIfAppWindowHasAnimation(*surfaceNode));
     uifirstManager_.currentFrameEvent_.clear();
 }
 
/**
* @tc.name: CheckIfAppWindowHasAnimation
* @tc.desc: Test CheckIfAppWindowHasAnimation, app window
* @tc.type: FUNC
* @tc.require: #ICLUN7
*/
HWTEST_F(RSUifirstManagerTest, CheckIfAppWindowHasAnimation002, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);

    surfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    RSUifirstManager::EventInfo event;
    event.disableNodes.emplace(surfaceNode->GetId());
    uifirstManager_.currentFrameEvent_.push_back(event);
    ASSERT_TRUE(uifirstManager_.CheckIfAppWindowHasAnimation(*surfaceNode));
    uifirstManager_.currentFrameEvent_.clear();
}
 
/**
* @tc.name: CheckIfAppWindowHasAnimation
* @tc.desc: Test CheckIfAppWindowHasAnimation, leash window
* @tc.type: FUNC
* @tc.require: #ICLUN7
*/
HWTEST_F(RSUifirstManagerTest, CheckIfAppWindowHasAnimation003, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    auto childNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(childNode, nullptr);
    surfaceNode->AddChild(childNode);
    surfaceNode->GenerateFullChildrenList();

    surfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
    childNode->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    RSUifirstManager::EventInfo event;
    event.disableNodes.emplace(surfaceNode->GetId());
    uifirstManager_.currentFrameEvent_.push_back(event);
    ASSERT_TRUE(uifirstManager_.CheckIfAppWindowHasAnimation(*surfaceNode));
    uifirstManager_.currentFrameEvent_.clear();
}

/**
* @tc.name: ResetUifirstNode
* @tc.desc: Test ResetUifirstNode
* @tc.type: FUNC
* @tc.require: issueICLUN7
*/
HWTEST_F(RSUifirstManagerTest, ResetUifirstNode, TestSize.Level1)
{
    auto nodePtr = std::make_shared<RSSurfaceRenderNode>(1);
    nodePtr->stagingRenderParams_ = nullptr;
    uifirstManager_.ResetUifirstNode(nodePtr);
    EXPECT_TRUE(nodePtr);
}
 
/**
* @tc.name: NotifyUIStartingWindow
* @tc.desc: Test NotifyUIStartingWindow
* @tc.type: FUNC
* @tc.require: issueICLUN7
*/
HWTEST_F(RSUifirstManagerTest, NotifyUIStartingWindow, TestSize.Level1)
{
    NodeId id = 0;
    auto rsContext = std::make_shared<RSContext>();
    auto parentNode = std::make_shared<RSSurfaceRenderNode>(id, rsContext);
    EXPECT_TRUE(parentNode);
    parentNode->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
    uifirstManager_.NotifyUIStartingWindow(id, false);

    NodeId parentNodeId = parentNode->GetId();
    pid_t parentNodePid = ExtractPid(parentNodeId);
    mainThread_->GetContext().GetMutableNodeMap().renderNodeMap_[parentNodePid][parentNodeId] = parentNode;

    auto childNode = std::make_shared<RSSurfaceRenderNode>(1, rsContext);
    EXPECT_TRUE(childNode);
    childNode->SetSurfaceNodeType(RSSurfaceNodeType::STARTING_WINDOW_NODE);
    parentNode->AddChild(childNode);
    EXPECT_FALSE(childNode->IsWaitUifirstFirstFrame());
}
 
/**
* @tc.name: PostSubTaskAndPostReleaseCacheSurfaceSubTask001
* @tc.desc: Test PostSubTask And PostReleaseCacheSurfaceSubTask
* @tc.type: FUNC
* @tc.require: issueICLUN7
*/
HWTEST_F(RSUifirstManagerTest, PostSubTaskAndPostReleaseCacheSurfaceSubTask001, TestSize.Level1)
{
    NodeId id = 1;
    uifirstManager_.PostSubTask(id);
    uifirstManager_.PostReleaseCacheSurfaceSubTask(id);
    EXPECT_FALSE(uifirstManager_.subthreadProcessingNode_.empty());
    EXPECT_EQ(uifirstManager_.subthreadProcessingNode_.size(), 2);

    id = 4;
    uifirstManager_.PostSubTask(id);
    uifirstManager_.PostReleaseCacheSurfaceSubTask(id);
    EXPECT_FALSE(uifirstManager_.subthreadProcessingNode_.empty());

    id = 2;
    std::shared_ptr<const RSRenderNode> node = std::make_shared<const RSRenderNode>(id);
    auto adapter = std::make_shared<RSRenderNodeDrawable>(std::move(node));
    std::weak_ptr<RSRenderNodeDrawable> drawableAdapter = adapter;
    DrawableV2::RSRenderNodeDrawableAdapter::RenderNodeDrawableCache_.insert(std::make_pair(id, drawableAdapter));
    uifirstManager_.PostSubTask(id);
    EXPECT_FALSE(uifirstManager_.subthreadProcessingNode_.empty());

    id = 3;
    std::shared_ptr<const RSRenderNode> renderNode = std::make_shared<const RSRenderNode>(id);
    auto renderNodeDrawable = std::make_shared<RSRenderNodeDrawable>(std::move(renderNode));
    std::weak_ptr<RSRenderNodeDrawable> nodeDrawableAdapter = renderNodeDrawable;
    DrawableV2::RSRenderNodeDrawableAdapter::RenderNodeDrawableCache_.insert(std::make_pair(id, nodeDrawableAdapter));
    uifirstManager_.PostReleaseCacheSurfaceSubTask(id);
    EXPECT_FALSE(uifirstManager_.subthreadProcessingNode_.empty());
}

 /**
 * @tc.name: ResetUifirstNode
 * @tc.desc: Test ResetUifirstNode
 * @tc.type: FUNC
 * @tc.require: issueIBJQV8
 */
HWTEST_F(RSUifirstManagerTest, ResetUifirstNode, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> nodePtr = nullptr;
    uifirstManager_.ResetUifirstNode(nodePtr);
    nodePtr = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(nodePtr, nullptr);
    uifirstManager_.ResetUifirstNode(nodePtr);
    nodePtr->isOnTheTree_ = true;
    uifirstManager_.ResetUifirstNode(nodePtr);
}

/**
  * @tc.name: UifirstStateChange
  * @tc.desc: Test UifirstStateChange, last frame not enabled, this frame enabled
  * @tc.type: FUNC
  * @tc.require: #ICLUN7
  */
HWTEST_F(RSUifirstManagerTest, UifirstStateChange002, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    // not support cache type switch, just disable multithread cache
    surfaceNode->SetLastFrameUifirstFlag(MultiThreadCacheType::NONE);
    auto currentFrameCacheType = MultiThreadCacheType::LEASH_WINDOW;
    uifirstManager_.UifirstStateChange(*surfaceNode, currentFrameCacheType);
    ASSERT_EQ(surfaceNode->GetLastFrameUifirstFlag(), MultiThreadCacheType::LEASH_WINDOW);
}
  
/**
* @tc.name: UifirstStateChange
* @tc.desc: Test UifirstStateChange, last frame enabled, this frame enabled
* @tc.type: FUNC
* @tc.require: #ICLUN7
*/
HWTEST_F(RSUifirstManagerTest, UifirstStateChange003, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);

    surfaceNode->SetLastFrameUifirstFlag(MultiThreadCacheType::LEASH_WINDOW);
    auto currentFrameCacheType = MultiThreadCacheType::LEASH_WINDOW;
    uifirstManager_.UifirstStateChange(*surfaceNode, currentFrameCacheType);
    ASSERT_EQ(surfaceNode->GetLastFrameUifirstFlag(), MultiThreadCacheType::LEASH_WINDOW);
}

/**
 * @tc.name: NotifyUIStartingWindowTest
 * @tc.desc: Test NotifyUIStartingWindow
 * @tc.type: FUNC
 * @tc.require: issueICLUN7
 */
HWTEST_F(RSUifirstManagerTest, NotifyUIStartingWindowTest, TestSize.Level1)
{
    auto parentNode = RSTestUtil::CreateSurfaceNode();
    EXPECT_TRUE(parentNode);
    parentNode->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
    NodeId parentNodeId = parentNode->GetId();
    uifirstManager_.NotifyUIStartingWindow(parentNodeId, false);

    pid_t parentNodePid = ExtractPid(parentNodeId);
    mainThread_->GetContext().GetMutableNodeMap().renderNodeMap_[parentNodePid][parentNodeId] = parentNode;

    auto childNode = RSTestUtil::CreateSurfaceNode();
    EXPECT_TRUE(childNode);
    childNode->SetSurfaceNodeType(RSSurfaceNodeType::STARTING_WINDOW_NODE);
    parentNode->AddChild(childNode);
    parentNode->GenerateFullChildrenList();
    uifirstManager_.NotifyUIStartingWindow(parentNodeId, false);
    EXPECT_FALSE(childNode->IsWaitUifirstFirstFrame());

    uifirstManager_.NotifyUIStartingWindow(parentNodeId, true);
    EXPECT_TRUE(childNode->IsWaitUifirstFirstFrame());
}

/**
 * @tc.name: PostSubTaskAndPostReleaseCacheSurfaceSubTask001
 * @tc.desc: Test PostSubTask And PostReleaseCacheSurfaceSubTask
 * @tc.type: FUNC
 * @tc.require: issueICLUN7
 */
HWTEST_F(RSUifirstManagerTest, PostSubTaskAndPostReleaseCacheSurfaceSubTask001, TestSize.Level1)
{
    NodeId id = 1;
    uifirstManager_.PostSubTask(id);
    uifirstManager_.PostReleaseCacheSurfaceSubTask(id);
    EXPECT_FALSE(uifirstManager_.subthreadProcessingNode_.empty());
    EXPECT_EQ(uifirstManager_.subthreadProcessingNode_.size(), 2);

    id = 4;
    uifirstManager_.PostSubTask(id);
    uifirstManager_.PostReleaseCacheSurfaceSubTask(id);
    EXPECT_FALSE(uifirstManager_.subthreadProcessingNode_.empty());

    id = 3;
    std::shared_ptr<const RSRenderNode> renderNode = std::make_shared<const RSRenderNode>(id);
    auto renderNodeDrawable = std::make_shared<RSRenderNodeDrawable>(std::move(renderNode));
    std::weak_ptr<RSRenderNodeDrawable> nodeDrawableAdapter = renderNodeDrawable;
    DrawableV2::RSRenderNodeDrawableAdapter::RenderNodeDrawableCache_.insert(std::make_pair(id, nodeDrawableAdapter));
    uifirstManager_.PostReleaseCacheSurfaceSubTask(id);
    EXPECT_FALSE(uifirstManager_.subthreadProcessingNode_.empty());

    id = 2;
    std::shared_ptr<const RSRenderNode> node = std::make_shared<const RSRenderNode>(id);
    auto adapter = std::make_shared<RSRenderNodeDrawable>(std::move(node));
    std::weak_ptr<RSRenderNodeDrawable> drawableAdapter = adapter;
    DrawableV2::RSRenderNodeDrawableAdapter::RenderNodeDrawableCache_.insert(std::make_pair(id, drawableAdapter));
    uifirstManager_.PostSubTask(id);
    EXPECT_FALSE(uifirstManager_.subthreadProcessingNode_.empty());
}

/**
 * @tc.name: UpdateSkipSyncNode001
 * @tc.desc: Test UpdateSkipSyncNode
 * @tc.type: FUNC
 * @tc.require: issueICLUN7
 */
HWTEST_F(RSUifirstManagerTest, UpdateSkipSyncNode001, TestSize.Level1)
{
    uifirstManager_.UpdateSkipSyncNode();
    EXPECT_TRUE(uifirstManager_.mainThread_);

    uifirstManager_.mainThread_ = nullptr;
    uifirstManager_.UpdateSkipSyncNode();
    EXPECT_FALSE(uifirstManager_.mainThread_);
    uifirstManager_.mainThread_ = mainThread_;
    EXPECT_TRUE(uifirstManager_.mainThread_);

    uifirstManager_.subthreadProcessingNode_.clear();
    uifirstManager_.UpdateSkipSyncNode();
    EXPECT_TRUE(uifirstManager_.subthreadProcessingNode_.empty());

    NodeId nodeId = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(nodeId);
    auto pid = ExtractPid(nodeId);
    mainThread_->GetContext().GetMutableNodeMap().renderNodeMap_[pid][nodeId] = surfaceNode;
    uifirstManager_.UpdateSkipSyncNode();
}

/**
  * @tc.name: UpdateUifirstNodes
  * @tc.desc: Test UpdateUifirstNodes, with different nodes
  * @tc.type: FUNC
  * @tc.require: #ICLUN7
  */
HWTEST_F(RSUifirstManagerTest, UpdateUifirstNodes, TestSize.Level1)
{
    auto surfaceNode1 = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode1, nullptr);
    surfaceNode1->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
    surfaceNode1->isChildSupportUifirst_ = true;
    uifirstManager_.UpdateUifirstNodes(*surfaceNode1, true);

    auto surfaceNode3 = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode3, nullptr);
    surfaceNode3->SetSurfaceNodeType(RSSurfaceNodeType::ABILITY_COMPONENT_NODE);
    surfaceNode3->isChildSupportUifirst_ = true;
    uifirstManager_.UpdateUifirstNodes(*surfaceNode3, true);

    auto surfaceNode4 = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode4, nullptr);
    surfaceNode4->SetSurfaceNodeType(RSSurfaceNodeType::SCB_SCREEN_NODE);
    surfaceNode4->isChildSupportUifirst_ = true;
    uifirstManager_.UpdateUifirstNodes(*surfaceNode4, true);

    auto surfaceNode2 = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode2, nullptr);
    surfaceNode2->SetSurfaceNodeType(RSSurfaceNodeType::SELF_DRAWING_WINDOW_NODE);
    surfaceNode2->isChildSupportUifirst_ = true;
    uifirstManager_.UpdateUifirstNodes(*surfaceNode2, true);
}

/**
 * @tc.name: ClearSubthreadRes001
 * @tc.desc: Test ClearSubthreadRes
 * @tc.type: FUNC
 * @tc.require: issueICLUN7
 */
HWTEST_F(RSUifirstManagerTest, ClearSubthreadRes001, TestSize.Level1)
{
    uifirstManager_.ClearSubthreadRes();
    EXPECT_EQ(uifirstManager_.subthreadProcessingNode_.size(), 0);
    EXPECT_EQ(uifirstManager_.pendingSyncForSkipBefore_.size(), 0);
    EXPECT_EQ(uifirstManager_.noUifirstNodeFrameCount_, 1);

    uifirstManager_.ClearSubthreadRes();
    uifirstManager_.ClearSubthreadRes();
    EXPECT_EQ(uifirstManager_.noUifirstNodeFrameCount_, 3);

    uifirstManager_.ClearSubthreadRes();
    EXPECT_EQ(uifirstManager_.noUifirstNodeFrameCount_, 4);

    NodeId nodeId = 1;
    std::shared_ptr<const RSRenderNode> renderNodeDrawable = std::make_shared<const RSRenderNode>(1);
    auto adapter = std::make_shared<RSRenderNodeDrawable>(std::move(renderNodeDrawable));
    uifirstManager_.subthreadProcessingNode_.insert(std::make_pair(nodeId, adapter));
    uifirstManager_.ClearSubthreadRes();
    EXPECT_EQ(uifirstManager_.subthreadProcessingNode_.size(), 1);

    auto node = std::make_shared<RSRenderNode>(0);
    std::vector<std::shared_ptr<RSRenderNode>> renderNode;
    renderNode.push_back(node);
    uifirstManager_.pendingSyncForSkipBefore_.insert(std::make_pair(1, renderNode));
    uifirstManager_.ClearSubthreadRes();
    EXPECT_EQ(uifirstManager_.pendingSyncForSkipBefore_.size(), 1);
}

/**
 * @tc.name: IsInLeashWindowTree001
 * @tc.desc: Test IsInLeashWindowTree
 * @tc.type: FUNC
 * @tc.require: issueICLUN7
 */
HWTEST_F(RSUifirstManagerTest, IsInLeashWindowTree001, TestSize.Level1)
{
    RSSurfaceRenderNode node(0);
    NodeId instanceRootId = 0;
    bool res = uifirstManager_.IsInLeashWindowTree(node, instanceRootId);
    EXPECT_TRUE(res);

    node.instanceRootNodeId_ = 1;
    res = uifirstManager_.IsInLeashWindowTree(node, instanceRootId);
    EXPECT_FALSE(res);

    std::shared_ptr<RSRenderNode> renderNode = std::make_shared<RSRenderNode>(0);
    children.clear();
    children.push_back(renderNode);
    node.fullChildrenList_ = std::make_shared<std::vector<std::shared_ptr<RSRenderNode>>>(children);
    res = uifirstManager_.IsInLeashWindowTree(node, instanceRootId);
    EXPECT_FALSE(res);

    std::vector<std::shared_ptr<RSRenderNode>> children;
    std::shared_ptr<RSSurfaceRenderNode> surfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(0);
    surfaceRenderNode->instanceRootNodeId_ = 0;
    children.push_back(surfaceRenderNode);
    node.fullChildrenList_ = std::make_shared<std::vector<std::shared_ptr<RSRenderNode>>>(children);
    node.nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    res = uifirstManager_.IsInLeashWindowTree(node, instanceRootId);
    EXPECT_TRUE(res);
}
 
/**
* @tc.name: AddPendingPostNode001
* @tc.desc: Test AddPendingPostNode
* @tc.type: FUNC
* @tc.require: issueICLUN7
*/
HWTEST_F(RSUifirstManagerTest, AddPendingPostNode001, TestSize.Level1)
{
    NodeId id = 0;
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(0);
    MultiThreadCacheType currentFrameCacheType = MultiThreadCacheType::NONE;
    uifirstManager_.AddPendingPostNode(id, node, currentFrameCacheType);
    EXPECT_TRUE(node);

    id = 1;
    uifirstManager_.AddPendingPostNode(id, node, currentFrameCacheType);
    EXPECT_EQ(uifirstManager_.pendingResetNodes_.count(id), 0);

    currentFrameCacheType = MultiThreadCacheType::NONFOCUS_WINDOW;
    EXPECT_EQ(uifirstManager_.pendingResetNodes_.count(id), 0);
    uifirstManager_.AddPendingPostNode(id, node, currentFrameCacheType);
    EXPECT_EQ(uifirstManager_.pendingPostCardNodes_.size(), 0);

    std::shared_ptr<RSSurfaceRenderNode> surfaceRenderNode = nullptr;
    uifirstManager_.pendingPostCardNodes_.insert(std::make_pair(++id, surfaceRenderNode));
    uifirstManager_.AddPendingPostNode(id, node, currentFrameCacheType);
    EXPECT_EQ(uifirstManager_.pendingPostCardNodes_.size(), 2);
    EXPECT_EQ(uifirstManager_.pendingResetNodes_.count(id), 0);
    EXPECT_EQ(uifirstManager_.pendingResetNodes_.count(--id), 0);

    currentFrameCacheType = MultiThreadCacheType::ARKTS_CARD;
    uifirstManager_.pendingResetNodes_.insert(std::make_pair(id, renderNode));
    uifirstManager_.AddPendingPostNode(id, node, currentFrameCacheType);
    EXPECT_EQ(uifirstManager_.pendingPostCardNodes_.size(), 2);
    EXPECT_EQ(uifirstManager_.pendingResetNodes_.count(id), 1);

    currentFrameCacheType = MultiThreadCacheType::LEASH_WINDOW;
    std::shared_ptr<RSSurfaceRenderNode> renderNode = std::make_shared<RSSurfaceRenderNode>(0);
    renderNode->instanceRootNodeId_ = 1;
    uifirstManager_.pendingPostCardNodes_.insert(std::make_pair(--id, renderNode));
    uifirstManager_.AddPendingPostNode(id, node, currentFrameCacheType);
    EXPECT_EQ(uifirstManager_.pendingPostCardNodes_.size(), 1);
}
 
/**
* @tc.name: LeashWindowContainMainWindowAndStarting001
* @tc.desc: Test LeashWindowContainMainWindowAndStarting
* @tc.type: FUNC
* @tc.require: issueICLUN7
*/
HWTEST_F(RSUifirstManagerTest, LeashWindowContainMainWindowAndStarting001, TestSize.Level1)
{
    RSSurfaceRenderNode node(0);
    NodeId resId = uifirstManager_.LeashWindowContainMainWindowAndStarting(node);
    EXPECT_FALSE(resId);

    node.nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    resId = uifirstManager_.LeashWindowContainMainWindowAndStarting(node);
    EXPECT_FALSE(resId);

    std::shared_ptr<RSRenderNode> rsRenderNode = std::make_shared<RSRenderNode>(0);
    canvasRenderNode->children_.push_back(std::weak_ptr<RSRenderNode>(rsRenderNode));
    resId = uifirstManager_.LeashWindowContainMainWindowAndStarting(node);
    EXPECT_FALSE(resId);

    surfaceRenderNode->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    resId = uifirstManager_.LeashWindowContainMainWindowAndStarting(node);
    EXPECT_FALSE(resId);

    children.clear();
    node.fullChildrenList_ = std::make_shared<std::vector<std::shared_ptr<RSRenderNode>>>(children);
    resId = uifirstManager_.LeashWindowContainMainWindowAndStarting(node);
    EXPECT_FALSE(resId);

    std::vector<std::shared_ptr<RSRenderNode>> children;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(1);
    auto rsCanvasRenderNode = std::make_shared<RSCanvasRenderNode>(1);
    children.push_back(rsSurfaceRenderNode);
    children.push_back(rsCanvasRenderNode);
    node.fullChildrenList_ = std::make_shared<std::vector<std::shared_ptr<RSRenderNode>>>(children);
    rsCanvasRenderNode->stagingRenderParams_ = std::make_unique<RSRenderParams>(1);
    resId = uifirstManager_.LeashWindowContainMainWindowAndStarting(node);
    EXPECT_TRUE(resId);

    auto canvasRenderNode = std::make_shared<RSCanvasRenderNode>(2);
    children.push_back(canvasRenderNode);
    auto surfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(3);
    children.push_back(surfaceRenderNode);
    node.fullChildrenList_ = std::make_shared<std::vector<std::shared_ptr<RSRenderNode>>>(children);
    resId = uifirstManager_.LeashWindowContainMainWindowAndStarting(node);
    EXPECT_FALSE(resId);
}
}