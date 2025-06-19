/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "feature/uifirst/rs_uifirst_manager.h"
#include "gtest/gtest.h"
#include "pipeline/main_thread/rs_uni_render_visitor.h"
#include "pipeline/rs_test_util.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_surface_render_node.h"

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

void RSUifirstManagerTest::SetUpTestCase()
{
    mainThread_ = RSMainThread::Instance();
    if (mainThread_) {
        uifirstManager_.mainThread_ = mainThread_;
    }
    RSTestUtil::InitRenderNodeGC();
}

void RSUifirstManagerTest::TearDownTestCase()
{
    auto mainThread = RSMainThread::Instance();
    if (!mainThread || !mainThread->context_) {
        return;
    }
    auto& renderNodeMap = mainThread->context_->GetMutableNodeMap();
    renderNodeMap.renderNodeMap_.clear();
    renderNodeMap.surfaceNodeMap_.clear();
    renderNodeMap.residentSurfaceNodeMap_.clear();
    renderNodeMap.displayNodeMap_.clear();
    renderNodeMap.canvasDrawingNodeMap_.clear();
    renderNodeMap.uiExtensionSurfaceNodes_.clear();

    uifirstManager_.subthreadProcessingNode_.clear();
    uifirstManager_.subthreadProcessDoneNode_.clear();
    uifirstManager_.markForceUpdateByUifirst_.clear();
    uifirstManager_.pendingPostNodes_.clear();
    uifirstManager_.pendingPostCardNodes_.clear();
    uifirstManager_.pendingResetNodes_.clear();
    uifirstManager_.pendingResetWindowCachedNodes_.clear();

    mainThread->context_->globalRootRenderNode_->renderDrawable_ = nullptr;
    mainThread->context_->globalRootRenderNode_ = nullptr;
}
void RSUifirstManagerTest::SetUp() {}

void RSUifirstManagerTest::TearDown() {}

/**
 * @tc.name: UpdateUifirstNodes
 * @tc.desc: Test UpdateUifirstNodes, with deviceType is PC
 * @tc.type: FUNC
 * @tc.require: #IBOBU1
 */
HWTEST_F(RSUifirstManagerTest, UpdateUifirstNodesPC, TestSize.Level1)
{
    uifirstManager_.uifirstType_ = UiFirstCcmType::MULTI;
    auto surfaceNode1 = RSTestUtil::CreateSurfaceNode();
    surfaceNode1->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
    surfaceNode1->firstLevelNodeId_ = surfaceNode1->GetId();
    // 1. surfaceNode1 is focus window, not has animation and filter and rotation.
    mainThread_->focusNodeId_ = surfaceNode1->GetId();
    uifirstManager_.UpdateUifirstNodes(*surfaceNode1, false);
    ASSERT_EQ(surfaceNode1->lastFrameUifirstFlag_, MultiThreadCacheType::NONE);
    // 2. surfaceNode1 is focus window, has animation. first frame
    uifirstManager_.UpdateUifirstNodes(*surfaceNode1, true);
    ASSERT_EQ(surfaceNode1->lastFrameUifirstFlag_, MultiThreadCacheType::NONE);
    // 3. second frame
    uifirstManager_.UpdateUifirstNodes(*surfaceNode1, true);
    ASSERT_EQ(surfaceNode1->lastFrameUifirstFlag_, MultiThreadCacheType::NONFOCUS_WINDOW);
    // 4. surfaceNode1 is focus window, has animation and transparent, not has filter.
    surfaceNode1->hasTransparentSurface_ = true;
    uifirstManager_.UpdateUifirstNodes(*surfaceNode1, true);
    ASSERT_EQ(surfaceNode1->lastFrameUifirstFlag_, MultiThreadCacheType::NONFOCUS_WINDOW);
    // 5. surfaceNode1 is focus window, has animation and filter, not has transparent.
    surfaceNode1->hasTransparentSurface_ = false;
    surfaceNode1->SetHasFilter(true);
    uifirstManager_.UpdateUifirstNodes(*surfaceNode1, true);
    ASSERT_EQ(surfaceNode1->lastFrameUifirstFlag_, MultiThreadCacheType::NONE);
    mainThread_->focusNodeId_ = 0;

    auto surfaceNode2 = RSTestUtil::CreateSurfaceNode();
    surfaceNode2->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
    surfaceNode2->firstLevelNodeId_ = surfaceNode2->GetId();
    // 5. surfaceNode2 is not focus window, not has filter and transparent. first frame
    uifirstManager_.UpdateUifirstNodes(*surfaceNode2, false);
    ASSERT_EQ(surfaceNode2->lastFrameUifirstFlag_, MultiThreadCacheType::NONE);
    // 6. second frame
    uifirstManager_.UpdateUifirstNodes(*surfaceNode2, false);
    ASSERT_EQ(surfaceNode2->lastFrameUifirstFlag_, MultiThreadCacheType::NONFOCUS_WINDOW);
    // 7. surfaceNode2 is not focus window, has transparent.
    surfaceNode2->hasTransparentSurface_ = true;
    uifirstManager_.UpdateUifirstNodes(*surfaceNode2, false);
    ASSERT_EQ(surfaceNode2->lastFrameUifirstFlag_, MultiThreadCacheType::NONFOCUS_WINDOW);
    // 8. surfaceNode2 is not focus window, has filter and transparent.
    surfaceNode2->SetHasFilter(true);
    uifirstManager_.UpdateUifirstNodes(*surfaceNode2, true);
    ASSERT_EQ(surfaceNode2->lastFrameUifirstFlag_, MultiThreadCacheType::NONE);
    // 9. surfaceNode2 is not focus window, not has filter and transparent, has display rotation.
    surfaceNode2->hasTransparentSurface_ = false;
    surfaceNode2->SetHasFilter(false);
    uifirstManager_.rotationChanged_ = true;
    uifirstManager_.UpdateUifirstNodes(*surfaceNode2, true);
    ASSERT_EQ(surfaceNode2->lastFrameUifirstFlag_, MultiThreadCacheType::NONE);
    uifirstManager_.uifirstType_ = UiFirstCcmType::SINGLE;
    uifirstManager_.isUiFirstOn_ = false;
    uifirstManager_.rotationChanged_ = false;
}

/**
 * @tc.name: UpdateUifirstNodesPC
 * @tc.desc: Test UpdateUifirstNodes, with deviceType is PC
 * @tc.type: FUNC
 * @tc.require: #IC1LJK
 */
HWTEST_F(RSUifirstManagerTest, UpdateUifirstNodesPC_001, TestSize.Level1)
{
    // save uifirstmanager_ states and restore at the end
    auto uifirstType = uifirstManager_.uifirstType_;
    auto isUiFirstOn = uifirstManager_.isUiFirstOn_;
    auto rotationChanged = uifirstManager_.rotationChanged_;

    // Given: build uifirst state
    uifirstManager_.uifirstType_ = UiFirstCcmType::MULTI;
    uifirstManager_.isUiFirstOn_ = true;
    uifirstManager_.rotationChanged_ = false;

    // Given: build rstree CanvasNode->SurfaceNode(leashwindow) and visitor
    auto leashWindowNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(leashWindowNode, nullptr);
    leashWindowNode->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
    leashWindowNode->firstLevelNodeId_ = leashWindowNode->GetId();
    leashWindowNode->SetNeedCollectHwcNode(true); // force to prepare subtree
    NodeId canvasNodeId = static_cast<NodeId>(0);
    auto rsCanvasRenderNode = std::make_shared<RSCanvasRenderNode>(canvasNodeId);
    ASSERT_NE(rsCanvasRenderNode, nullptr);
    rsCanvasRenderNode->stagingRenderParams_ = std::make_unique<RSRenderParams>(canvasNodeId);
    auto children = std::vector<std::shared_ptr<RSRenderNode>>();
    children.push_back(leashWindowNode);
    rsCanvasRenderNode->fullChildrenList_ = std::make_shared<std::vector<std::shared_ptr<RSRenderNode>>>(children);
    auto visitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(visitor, nullptr);
    visitor->ancestorNodeHasAnimation_ = true;
    visitor->curDisplayDirtyManager_ = std::make_shared<RSDirtyRegionManager>();
    NodeId displayNodeId = 2;
    RSDisplayNodeConfig config;
    visitor->curDisplayNode_ = std::make_shared<RSDisplayRenderNode>(displayNodeId, config);

    // case01: shouldpaint of parent node is false
    rsCanvasRenderNode->shouldPaint_ = false;
    leashWindowNode->shouldPaint_ = true;
    leashWindowNode->SetSkipDraw(false);
    visitor->QuickPrepareCanvasRenderNode(*rsCanvasRenderNode);
    ASSERT_EQ(leashWindowNode->lastFrameUifirstFlag_, MultiThreadCacheType::NONE);
    ASSERT_EQ(leashWindowNode->GetSubThreadAssignable(), false);

    // case02: shouldpaint of parent node is true but shouldpaint of leashwindow is false
    rsCanvasRenderNode->shouldPaint_ = true;
    leashWindowNode->shouldPaint_ = false;
    leashWindowNode->SetSkipDraw(false);
    visitor->QuickPrepareCanvasRenderNode(*rsCanvasRenderNode);
    ASSERT_EQ(leashWindowNode->lastFrameUifirstFlag_, MultiThreadCacheType::NONE);
    ASSERT_EQ(leashWindowNode->GetSubThreadAssignable(), false);

    // case03: shouldpaint of parent node and leashwindow is true, but leashwindow skipdraw is true
    rsCanvasRenderNode->shouldPaint_ = true;
    leashWindowNode->shouldPaint_ = true;
    leashWindowNode->SetSkipDraw(true);
    visitor->QuickPrepareCanvasRenderNode(*rsCanvasRenderNode);
    ASSERT_EQ(leashWindowNode->lastFrameUifirstFlag_, MultiThreadCacheType::NONE);
    ASSERT_EQ(leashWindowNode->GetSubThreadAssignable(), false);

    // case04: shouldpaint of parent node and leashwindow is true, but leashwindow skipdraw is false
    rsCanvasRenderNode->shouldPaint_ = true;
    leashWindowNode->shouldPaint_ = true;
    leashWindowNode->SetSkipDraw(false);
    visitor->QuickPrepareCanvasRenderNode(*rsCanvasRenderNode);
    ASSERT_EQ(leashWindowNode->lastFrameUifirstFlag_, MultiThreadCacheType::NONE);
    ASSERT_EQ(leashWindowNode->GetSubThreadAssignable(), true);

    // case05: shouldpaint of parent node and leashwindow is true, but leashwindow skipdraw is false
    rsCanvasRenderNode->shouldPaint_ = true;
    leashWindowNode->shouldPaint_ = true;
    leashWindowNode->SetSkipDraw(false);
    visitor->QuickPrepareCanvasRenderNode(*rsCanvasRenderNode);
    ASSERT_EQ(leashWindowNode->lastFrameUifirstFlag_, MultiThreadCacheType::NONFOCUS_WINDOW);
    ASSERT_EQ(leashWindowNode->GetSubThreadAssignable(), true);

    // restore uifirstManager_ states
    uifirstManager_.uifirstType_ = uifirstType;
    uifirstManager_.isUiFirstOn_ = isUiFirstOn;
    uifirstManager_.rotationChanged_ = rotationChanged;
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
 * @tc.name: NotifyUIStartingWindowTest
 * @tc.desc: Test NotifyUIStartingWindow
 * @tc.type: FUNC
 * @tc.require: issueIADDL3
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
 * @tc.require: issueIADDL3
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
 * @tc.name: UpdateSkipSyncNode001
 * @tc.desc: Test UpdateSkipSyncNode
 * @tc.type: FUNC
 * @tc.require: issueIADDL3
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

    NodeId nodeId = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(nodeId);
    auto pid = ExtractPid(nodeId);
    mainThread_->GetContext().GetMutableNodeMap().renderNodeMap_[pid][nodeId] = surfaceNode;
    uifirstManager_.UpdateSkipSyncNode();

    uifirstManager_.subthreadProcessingNode_.clear();
    uifirstManager_.UpdateSkipSyncNode();
    EXPECT_TRUE(uifirstManager_.subthreadProcessingNode_.empty());
}

/**
 * @tc.name: CollectSkipSyncNode001
 * @tc.desc: Test CollectSkipSyncNode
 * @tc.type: FUNC
 * @tc.require: issueIADDL3
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
 * @tc.require: issueIADDL3
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
 * @tc.name: ClearSubthreadRes001
 * @tc.desc: Test ClearSubthreadRes
 * @tc.type: FUNC
 * @tc.require: issueIADDL3
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

    auto node = std::make_shared<RSRenderNode>(0);
    std::vector<std::shared_ptr<RSRenderNode>> renderNode;
    renderNode.push_back(node);
    uifirstManager_.pendingSyncForSkipBefore_.insert(std::make_pair(1, renderNode));
    uifirstManager_.ClearSubthreadRes();
    EXPECT_EQ(uifirstManager_.pendingSyncForSkipBefore_.size(), 1);

    NodeId nodeId = 1;
    std::shared_ptr<const RSRenderNode> renderNodeDrawable = std::make_shared<const RSRenderNode>(1);
    auto adapter = std::make_shared<RSRenderNodeDrawable>(std::move(renderNodeDrawable));
    uifirstManager_.subthreadProcessingNode_.insert(std::make_pair(nodeId, adapter));
    uifirstManager_.ClearSubthreadRes();
    EXPECT_EQ(uifirstManager_.subthreadProcessingNode_.size(), 1);
}

/**
 * @tc.name: SetNodePriorty001
 * @tc.desc: Test SetNodePriorty
 * @tc.type: FUNC
 * @tc.require: issueIAOJHQ
 */
HWTEST_F(RSUifirstManagerTest, SetNodePriorty001, TestSize.Level1)
{
    uifirstManager_.subthreadProcessingNode_.clear();
    std::list<NodeId> result;
    NodeId nodeId = 1;
    auto surfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(nodeId);
    DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceRenderNode);
    std::unordered_map<NodeId, std::shared_ptr<RSSurfaceRenderNode>> pendingNode;
    pendingNode.insert(std::make_pair(nodeId, surfaceRenderNode));
    uifirstManager_.SetNodePriorty(result, pendingNode);
    EXPECT_EQ(pendingNode.size(), 1);

    RSMainThread::Instance()->focusLeashWindowId_ = 1;
    uifirstManager_.SetNodePriorty(result, pendingNode);
    EXPECT_TRUE(RSMainThread::Instance()->GetFocusLeashWindowId());
}

/**
 * @tc.name: IsInLeashWindowTree001
 * @tc.desc: Test IsInLeashWindowTree
 * @tc.type: FUNC
 * @tc.require: issueIADDL3
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

    std::vector<std::shared_ptr<RSRenderNode>> children;
    std::shared_ptr<RSSurfaceRenderNode> surfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(0);
    surfaceRenderNode->instanceRootNodeId_ = 0;
    children.push_back(surfaceRenderNode);
    node.fullChildrenList_ = std::make_shared<std::vector<std::shared_ptr<RSRenderNode>>>(children);
    node.nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    res = uifirstManager_.IsInLeashWindowTree(node, instanceRootId);
    EXPECT_TRUE(res);

    std::shared_ptr<RSRenderNode> renderNode = std::make_shared<RSRenderNode>(0);
    children.clear();
    children.push_back(renderNode);
    node.fullChildrenList_ = std::make_shared<std::vector<std::shared_ptr<RSRenderNode>>>(children);
    res = uifirstManager_.IsInLeashWindowTree(node, instanceRootId);
    EXPECT_FALSE(res);
}

/**
 * @tc.name: AddPendingPostNode001
 * @tc.desc: Test AddPendingPostNode
 * @tc.type: FUNC
 * @tc.require: issueIADDL3
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

    currentFrameCacheType = MultiThreadCacheType::LEASH_WINDOW;
    std::shared_ptr<RSSurfaceRenderNode> renderNode = std::make_shared<RSSurfaceRenderNode>(0);
    renderNode->instanceRootNodeId_ = 1;
    uifirstManager_.pendingPostCardNodes_.insert(std::make_pair(--id, renderNode));
    uifirstManager_.AddPendingPostNode(id, node, currentFrameCacheType);
    EXPECT_EQ(uifirstManager_.pendingPostCardNodes_.size(), 1);

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
}

/**
 * @tc.name: LeashWindowContainMainWindowAndStarting001
 * @tc.desc: Test LeashWindowContainMainWindowAndStarting
 * @tc.type: FUNC
 * @tc.require: issueIADDL3
 */
HWTEST_F(RSUifirstManagerTest, LeashWindowContainMainWindowAndStarting001, TestSize.Level1)
{
    RSSurfaceRenderNode node(0);
    NodeId resId = uifirstManager_.LeashWindowContainMainWindowAndStarting(node);
    EXPECT_FALSE(resId);

    node.nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
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
}

/**
 * @tc.name: AddPendingResetNode001
 * @tc.desc: Test AddPendingResetNode
 * @tc.type: FUNC
 * @tc.require: issueIADDL3
 */
HWTEST_F(RSUifirstManagerTest, AddPendingResetNode001, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(0);
    uifirstManager_.AddPendingResetNode(0, node);
    EXPECT_TRUE(node);

    uifirstManager_.AddPendingResetNode(1, node);
    EXPECT_TRUE(node);
}

/**
 * @tc.name: GetNodeStatus001
 * @tc.desc: Test GetNodeStatus
 * @tc.type: FUNC
 * @tc.require: issueIADDL3
 */
HWTEST_F(RSUifirstManagerTest, GetNodeStatus001, TestSize.Level1)
{
    CacheProcessStatus status = uifirstManager_.GetNodeStatus(0);
    EXPECT_EQ(status, CacheProcessStatus::UNKNOWN);

    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    auto surfaceDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
    uifirstManager_.subthreadProcessingNode_.clear();
    uifirstManager_.subthreadProcessingNode_.insert(std::make_pair(surfaceNode->GetId(), surfaceDrawable));
    surfaceDrawable->GetRsSubThreadCache().SetCacheSurfaceProcessedStatus(CacheProcessStatus::DOING);
    EXPECT_EQ(uifirstManager_.GetNodeStatus(surfaceNode->GetId()), CacheProcessStatus::DOING);
}

/**
 * @tc.name: OnProcessEventComplete001
 * @tc.desc: Test OnProcessEventComplete
 * @tc.type: FUNC
 * @tc.require: issueIADDL3
 */
HWTEST_F(RSUifirstManagerTest, OnProcessEventComplete001, TestSize.Level1)
{
    DataBaseRs info;
    EXPECT_TRUE(uifirstManager_.globalFrameEvent_.empty());
    RSUifirstManager::EventInfo eventInfo;
    uifirstManager_.globalFrameEvent_.push_back(eventInfo);
    uifirstManager_.OnProcessEventComplete(info);
    EXPECT_FALSE(uifirstManager_.globalFrameEvent_.empty());

    info.sceneId = 1;
    uifirstManager_.OnProcessEventComplete(info);
    EXPECT_FALSE(uifirstManager_.globalFrameEvent_.empty());

    info.uniqueId = 1;
    uifirstManager_.OnProcessEventComplete(info);
    EXPECT_FALSE(uifirstManager_.globalFrameEvent_.empty());
}

/**
 * @tc.name: EventDisableLeashWindowCache001
 * @tc.desc: Test EventDisableLeashWindowCache
 * @tc.type: FUNC
 * @tc.require: issueIADDL3
 */
HWTEST_F(RSUifirstManagerTest, EventDisableLeashWindowCache001, TestSize.Level1)
{
    uifirstManager_.globalFrameEvent_.clear();
    EXPECT_TRUE(uifirstManager_.globalFrameEvent_.empty());
    RSUifirstManager::EventInfo eventInfo;
    uifirstManager_.globalFrameEvent_.push_back(eventInfo);
    RSUifirstManager::EventInfo info;
    uifirstManager_.EventDisableLeashWindowCache(0, info);
    EXPECT_FALSE(uifirstManager_.globalFrameEvent_.empty());

    info.sceneId = 1;
    uifirstManager_.EventDisableLeashWindowCache(0, info);
    EXPECT_FALSE(uifirstManager_.globalFrameEvent_.empty());

    info.uniqueId = 1;
    uifirstManager_.EventDisableLeashWindowCache(0, info);
    EXPECT_FALSE(uifirstManager_.globalFrameEvent_.empty());
}

static inline int64_t GetCurSysTime()
{
    auto curTime = std::chrono::system_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(curTime).count();
}

/**
 * @tc.name: PrepareCurrentFrameEvent001
 * @tc.desc: Test PrepareCurrentFrameEvent
 * @tc.type: FUNC
 * @tc.require: issueIADDL3
 */
HWTEST_F(RSUifirstManagerTest, PrepareCurrentFrameEvent001, TestSize.Level1)
{
    int64_t curSysTime = GetCurSysTime();
    uifirstManager_.mainThread_ = nullptr;
    uifirstManager_.globalFrameEvent_.clear();
    uifirstManager_.PrepareCurrentFrameEvent();
    EXPECT_TRUE(uifirstManager_.mainThread_);
    EXPECT_TRUE(uifirstManager_.globalFrameEvent_.empty());

    uifirstManager_.entryViewNodeId_ = 1;
    uifirstManager_.PrepareCurrentFrameEvent();
    EXPECT_FALSE(uifirstManager_.entryViewNodeId_);

    uifirstManager_.negativeScreenNodeId_ = 1;
    uifirstManager_.PrepareCurrentFrameEvent();
    EXPECT_FALSE(uifirstManager_.negativeScreenNodeId_);

    RSUifirstManager::EventInfo eventInfo;
    uifirstManager_.globalFrameEvent_.push_back(eventInfo);
    uifirstManager_.PrepareCurrentFrameEvent();
    EXPECT_TRUE(uifirstManager_.globalFrameEvent_.empty());

    eventInfo.stopTime = 1;
    uifirstManager_.globalFrameEvent_.push_back(eventInfo);
    uifirstManager_.PrepareCurrentFrameEvent();
    EXPECT_TRUE(uifirstManager_.globalFrameEvent_.empty());

    eventInfo.stopTime = curSysTime;
    uifirstManager_.globalFrameEvent_.push_back(eventInfo);
    uifirstManager_.PrepareCurrentFrameEvent();
    EXPECT_TRUE(uifirstManager_.globalFrameEvent_.empty());

    eventInfo.startTime = curSysTime;
    uifirstManager_.globalFrameEvent_.push_back(eventInfo);
    uifirstManager_.PrepareCurrentFrameEvent();
    EXPECT_FALSE(uifirstManager_.globalFrameEvent_.empty());
}

/**
 * @tc.name: OnProcessAnimateScene001
 * @tc.desc: Test OnProcessAnimateScene
 * @tc.type: FUNC
 * @tc.require: issueIADDL3
 */
HWTEST_F(RSUifirstManagerTest, OnProcessAnimateScene001, TestSize.Level1)
{
    uifirstManager_.isRecentTaskScene_ = false;
    SystemAnimatedScenes systemAnimatedScene = SystemAnimatedScenes::APPEAR_MISSION_CENTER;
    uifirstManager_.OnProcessAnimateScene(systemAnimatedScene);
    EXPECT_FALSE(uifirstManager_.isRecentTaskScene_);

    systemAnimatedScene = SystemAnimatedScenes::ENTER_RECENTS;
    uifirstManager_.OnProcessAnimateScene(systemAnimatedScene);
    EXPECT_TRUE(uifirstManager_.isRecentTaskScene_);

    uifirstManager_.OnProcessAnimateScene(systemAnimatedScene);
    EXPECT_TRUE(uifirstManager_.isRecentTaskScene_);

    systemAnimatedScene = SystemAnimatedScenes::EXIT_RECENTS;
    uifirstManager_.OnProcessAnimateScene(systemAnimatedScene);
    EXPECT_FALSE(uifirstManager_.isRecentTaskScene_);

    uifirstManager_.OnProcessAnimateScene(systemAnimatedScene);
    EXPECT_FALSE(uifirstManager_.isRecentTaskScene_);
}
}