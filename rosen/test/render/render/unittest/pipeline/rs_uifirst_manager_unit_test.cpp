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

#include "gtest/gtest.h"
#include "pipeline/rs_uifirst_manager.h"
#include "rs_test_util.h"
#include "pipeline/rs_canvas_render_node.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::DrawableV2;

namespace OHOS::Rosen {
class RsUifirstManagerTest : public testing::Test {
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

void RsUifirstManagerTest::SetUpTestCase()
{
    mainThread_ = RSMainThread::Instance();
    if (mainThread_) {
        uifirstManager_.mainThread_ = mainThread_;
    }
    RSTestUtil::InitRenderNodeGC();
}

void RsUifirstManagerTest::TearDownTestCase()
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

    uifirstManager_.subthreadProcessDoneNode_.clear();
    uifirstManager_.markForceUpdateByUifirst_.clear();
    uifirstManager_.pendingPostNodes_.clear();
    uifirstManager_.pendingPostCardNodes_.clear();
    uifirstManager_.pendingResetNodes_.clear();
}
void RsUifirstManagerTest::SetUp() {}

void RsUifirstManagerTest::TearDown() {}

/**
 * @tc.name: ProcessForceUpdateNode01
 * @tc.desc: Test ProcessForceUpdateNode, input invalid nodeid
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RsUifirstManagerTest, ProcessForceUpdateNode01, TestSize.Level1)
{
    ASSERT_NE(mainThread_, nullptr);
    uifirstManager_.pendingForceUpdateNode_.push_back(INVALID_NODEID);
    uifirstManager_.ProcessForceUpdateNode();
}

/**
 * @tc.name: ProcessForceUpdateNode02
 * @tc.desc: Test ProcessForceUpdateNode, when parent node is surface node
 * @tc.type: FUNC
 * @tc.require: issue1IAOJHQ
 */
HWTEST_F(RsUifirstManagerTest, ProcessForceUpdateNode02, TestSize.Level1)
{
    ASSERT_NE(mainThread_, nullptr);
    auto parentNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(parentNode, nullptr);
    auto childNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(childNode, nullptr);
    parentNode->AddChild(childNode);
    parentNode->GenerateFullChildrenList();
    mainThread_->context_->nodeMap.RegisterRenderNode(parentNode);
    uifirstManager_.pendingForceUpdateNode_.push_back(parentNode->GetId());
    uifirstManager_.ProcessForceUpdateNode();
}

/**
 * @tc.name: ProcessDoneNode
 * @tc.desc: Test ProcessDoneNode, subthreadProcessDoneNode_ is expected to be empty
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RsUifirstManagerTest, ProcessDoneNode, TestSize.Level1)
{
    uifirstManager_.subthreadProcessDoneNode_.push_back(INVALID_NODEID);
    uifirstManager_.ProcessDoneNode();
    ASSERT_TRUE(uifirstManager_.subthreadProcessDoneNode_.empty());
}

/**
* @tc.name: ProcessDoneNode01
* @tc.desc: Test ProcessDoneNode
* @tc.type: FUNC
* @tc.require: issue1IAOJHQ
*/
HWTEST_F(RsUifirstManagerTest, ProcessDoneNode01, TestSize.Level1)
{
    NodeId id = 1;
    uifirstManager_.capturedNodes_.push_back(id);
    uifirstManager_.ProcessDoneNode();
    EXPECT_TRUE(uifirstManager_.capturedNodes_.empty());
    
    auto surfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(id);
    auto adapter = DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceRenderNode);
    uifirstManager_.subthreadProcessingNode_.insert(std::make_pair(id, adapter));
    uifirstManager_.capturedNodes_.push_back(id);
    uifirstManager_.ProcessDoneNode();
    EXPECT_FALSE(uifirstManager_.subthreadProcessingNode_.empty());

    uifirstManager_.pendingResetNodes_.insert(std::make_pair(id, surfaceRenderNode));
    uifirstManager_.capturedNodes_.push_back(id);
    uifirstManager_.ProcessDoneNode();
    EXPECT_FALSE(uifirstManager_.pendingResetNodes_.empty());

    std::shared_ptr<RSRenderNodeDrawable> renderNodeDrawable = nullptr;
    uifirstManager_.subthreadProcessingNode_.insert(std::make_pair(++id, renderNodeDrawable));
    uifirstManager_.ProcessDoneNode();
    EXPECT_FALSE(uifirstManager_.subthreadProcessingNode_.empty());
}

/**
 * @tc.name: CheckVisibleDirtyRegionIsEmpty
 * @tc.desc: Test CheckVisibleDirtyRegionIsEmpty, node has no child
 * @tc.type: FUNC
 * @tc.require: #I9UNQP
 */
HWTEST_F(RsUifirstManagerTest, CheckVisibleDirtyRegionIsEmpty01, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    ASSERT_FALSE(uifirstManager_.CheckVisibleDirtyRegionIsEmpty(surfaceNode));
}

/**
 * @tc.name: CheckVisibleDirtyRegionIsEmpty
 * @tc.desc: Test CheckVisibleDirtyRegionIsEmpty, child has empty dirty region
 * @tc.type: FUNC
 * @tc.require: #I9UNQP
 */
HWTEST_F(RsUifirstManagerTest, CheckVisibleDirtyRegionIsEmpty02, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    auto childNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(childNode, nullptr);
    surfaceNode->AddChild(childNode);
    surfaceNode->GenerateFullChildrenList();

    ASSERT_FALSE(uifirstManager_.CheckVisibleDirtyRegionIsEmpty(surfaceNode));
}

/**
 * @tc.name: CheckVisibleDirtyRegionIsEmpty
 * @tc.desc: Test CheckVisibleDirtyRegionIsEmpty, child has visible/invisible dirty region
 * @tc.type: FUNC
 * @tc.require: #I9UNQP
 */
HWTEST_F(RsUifirstManagerTest, CheckVisibleDirtyRegionIsEmpty03, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(0);
    std::shared_ptr<RSRenderNode> renderNode = std::make_shared<RSRenderNode>(0);
    std::vector<std::shared_ptr<RSRenderNode>> children;
    children.push_back(renderNode);
    node->fullChildrenList_ = std::make_shared<std::vector<std::shared_ptr<RSRenderNode>>>(children);
    bool res = uifirstManager_.CheckVisibleDirtyRegionIsEmpty(node);
    EXPECT_FALSE(res);

    std::shared_ptr<RSSurfaceRenderNode> surfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(0);
    surfaceRenderNode->dirtyManager_ = std::make_shared<RSDirtyRegionManager>();
    children.clear();
    children.push_back(surfaceRenderNode);
    node->fullChildrenList_ = std::make_shared<std::vector<std::shared_ptr<RSRenderNode>>>(children);
    res = uifirstManager_.CheckVisibleDirtyRegionIsEmpty(node);
    EXPECT_TRUE(node->visibleRegion_.IsEmpty());
    EXPECT_FALSE(res);

    Occlusion::Rect rect(1, 2, 3, 4);
    Occlusion::Rect bound(1, 2, 3, 4);
    surfaceRenderNode->visibleRegion_.rects_.push_back(rect);
    surfaceRenderNode->visibleRegion_.bound_ = bound;
    res = uifirstManager_.CheckVisibleDirtyRegionIsEmpty(node);
    EXPECT_FALSE(res);

    surfaceRenderNode->GetDirtyManager()->SetCurrentFrameDirtyRect(RectI(1, 2, 3, 4));
    surfaceRenderNode->SetUIFirstIsPurge(true);
    res = uifirstManager_.CheckVisibleDirtyRegionIsEmpty(node);
    EXPECT_FALSE(res);

    surfaceRenderNode->SetUIFirstIsPurge(false);
    res = uifirstManager_.CheckVisibleDirtyRegionIsEmpty(node);
    EXPECT_FALSE(surfaceRenderNode->GetUIFirstIsPurge());
    EXPECT_FALSE(res);
}

/**
 * @tc.name: ProcessTreeStateChange
 * @tc.desc: Test ProcessTreeStateChange, early return case
 * @tc.type: FUNC
 * @tc.require: #I9UNQP
 */
HWTEST_F(RsUifirstManagerTest, ProcessTreeStateChange, TestSize.Level1)
{
    auto surfaceNode1 = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode1, nullptr);
    surfaceNode1->SetIsOnTheTree(true);
    uifirstManager_.ProcessTreeStateChange(*surfaceNode1);

    auto surfaceNode2 = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode2, nullptr);
    surfaceNode1->SetIsOnTheTree(false);
    surfaceNode1->SetIsNodeToBeCaptured(true);
    uifirstManager_.ProcessTreeStateChange(*surfaceNode2);
}

/**
 * @tc.name: SetUifirstNodeEnableParam01
 * @tc.desc: Test SetUifirstNodeEnableParam, when node is leash window type
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RsUifirstManagerTest, SetUifirstNodeEnableParam01, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
    uifirstManager_.SetUifirstNodeEnableParam(*surfaceNode, MultiThreadCacheType::NONE);
}

/**
 * @tc.name: SetUifirstNodeEnableParam02
 * @tc.desc: Test SetUifirstNodeEnableParam, with APP window child and FOREGROUND child
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RsUifirstManagerTest, SetUifirstNodeEnableParam02, TestSize.Level1)
{
    auto parentNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(parentNode, nullptr);
    parentNode->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
    // create different children nodes
    NodeId id = 1;
    auto childNode1 = std::make_shared<RSSurfaceRenderNode>(id);
    auto childNode2 = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(childNode2, nullptr);
    childNode2->SetSurfaceNodeType(RSSurfaceNodeType::FOREGROUND_SURFACE);
    auto childNode3 = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(childNode3, nullptr);
    childNode3->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    parentNode->AddChild(childNode1);
    parentNode->AddChild(childNode2);
    parentNode->AddChild(childNode3);
    parentNode->GenerateFullChildrenList();
    uifirstManager_.SetUifirstNodeEnableParam(*parentNode, MultiThreadCacheType::LEASH_WINDOW);
    // child2 uifirstParentFlag_ expected to be false
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(childNode2->GetStagingRenderParams().get());
    ASSERT_NE(surfaceParams, nullptr);
    ASSERT_FALSE(surfaceParams->GetParentUifirstNodeEnableParam());
    // child3 uifirstParentFlag_ expected to be true
    surfaceParams = static_cast<RSSurfaceRenderParams*>(childNode3->GetStagingRenderParams().get());
    ASSERT_NE(surfaceParams, nullptr);
    ASSERT_TRUE(surfaceParams->GetParentUifirstNodeEnableParam());
}

/**
 * @tc.name: MergeOldDirty01
 * @tc.desc: Test MergeOldDirty, preSurfaceCacheContentStatic_ is false, no dirty region
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RsUifirstManagerTest, MergeOldDirty01, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceNode->GetStagingRenderParams().get());
    ASSERT_NE(surfaceParams, nullptr);
    surfaceParams->preSurfaceCacheContentStatic_ = false;
    surfaceNode->oldDirty_ = RsUifirstManagerTest::DEFAULT_RECT;
    uifirstManager_.MergeOldDirty(*surfaceNode);
    if (surfaceNode->GetDirtyManager()) {
        ASSERT_TRUE(surfaceNode->GetDirtyManager()->GetCurrentFrameDirtyRegion().IsEmpty());
    }
}

/**
 * @tc.name: MergeOldDirty02
 * @tc.desc: Test MergeOldDirty, preSurfaceCacheContentStatic_ is true, dirty region is not empty
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RsUifirstManagerTest, MergeOldDirty02, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceNode->GetStagingRenderParams().get());
    ASSERT_NE(surfaceParams, nullptr);
    surfaceParams->preSurfaceCacheContentStatic_ = true;
    surfaceNode->oldDirty_ = RsUifirstManagerTest::DEFAULT_RECT;
    uifirstManager_.MergeOldDirty(*surfaceNode);
    if (surfaceNode->GetDirtyManager()) {
        ASSERT_FALSE(surfaceNode->GetDirtyManager()->GetCurrentFrameDirtyRegion().IsEmpty());
    }
}

/**
 * @tc.name: MergeOldDirty03
 * @tc.desc: Test MergeOldDirty, merge children dirty region
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RsUifirstManagerTest, MergeOldDirty03, TestSize.Level1)
{
    auto parentNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(parentNode, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceNode->GetStagingRenderParams().get());
    ASSERT_NE(surfaceParams, nullptr);
    surfaceParams->preSurfaceCacheContentStatic_ = true;
    parentNode->AddChild(surfaceNode);
    // add different children nodes
    auto childNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(childNode, nullptr);
    childNode->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    childNode->oldDirty_= RsUifirstManagerTest::DEFAULT_RECT;
    surfaceNode->AddChild(childNode);
    surfaceNode->GenerateFullChildrenList();
    uifirstManager_.MergeOldDirty(*surfaceNode);
    if (childNode->GetDirtyManager()) {
        ASSERT_FALSE(childNode->GetDirtyManager()->GetCurrentFrameDirtyRegion().IsEmpty());
    }
}

/**
 * @tc.name: RenderGroupUpdate01
 * @tc.desc: Test RenderGroupUpdate, when parent node is nullptr
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RsUifirstManagerTest, RenderGroupUpdate01, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    auto surfaceDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
    uifirstManager_.RenderGroupUpdate(surfaceDrawable);
}

/**
 * @tc.name: RenderGroupUpdate02
 * @tc.desc: Test RenderGroupUpdate, when parent node is display node
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RsUifirstManagerTest, RenderGroupUpdate02, TestSize.Level1)
{
    NodeId id = 1;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    ASSERT_NE(displayNode, nullptr);
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(++id);
    ASSERT_NE(surfaceNode, nullptr);
    displayNode->AddChild(surfaceNode);
    auto surfaceDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
    uifirstManager_.RenderGroupUpdate(surfaceDrawable);
}

/**
 * @tc.name: RenderGroupUpdate03
 * @tc.desc: Test RenderGroupUpdate, when parent node is surface node
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RsUifirstManagerTest, RenderGroupUpdate03, TestSize.Level1)
{
    auto parentNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(parentNode, nullptr);
    auto childNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(childNode, nullptr);
    parentNode->AddChild(childNode);
    parentNode->nodeGroupType_ = RsUifirstManagerTest::GROUPED_BY_ANIM;
    auto surfaceDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(childNode));
    uifirstManager_.RenderGroupUpdate(surfaceDrawable);
}

/**
 * @tc.name: UpdateChildrenDirtyRect
 * @tc.desc: Test UpdateChildrenDirtyRect if node is not leash window, return early.
 * @tc.type: FUNC
 * @tc.require: #I9UNQP
 */
HWTEST_F(RsUifirstManagerTest, UpdateChildrenDirtyRect01, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::STARTING_WINDOW_NODE);
    uifirstManager_.UpdateChildrenDirtyRect(*surfaceNode);

    auto param = static_cast<RSSurfaceRenderParams*>(surfaceNode->stagingRenderParams_.get());
    ASSERT_TRUE(param->GetUifirstChildrenDirtyRectParam().IsEmpty());
}

/**
 * @tc.name: UpdateChildrenDirtyRect
 * @tc.desc: Test UpdateChildrenDirtyRect if node is not leash window, return early.
 * @tc.type: FUNC
 * @tc.require: #I9UNQP
 */
HWTEST_F(RsUifirstManagerTest, UpdateChildrenDirtyRect02, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
    auto child1 = RSTestUtil::CreateSurfaceNode();
    child1->SetSurfaceNodeType(RSSurfaceNodeType::FOREGROUND_SURFACE);
    auto child2 = RSTestUtil::CreateSurfaceNode();
    child2->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    child2->oldDirtyInSurface_ = DEFAULT_RECT;
    surfaceNode->AddChild(child1);
    surfaceNode->AddChild(child2);
    surfaceNode->GenerateFullChildrenList();
    uifirstManager_.UpdateChildrenDirtyRect(*surfaceNode);

    auto param = static_cast<RSSurfaceRenderParams*>(surfaceNode->stagingRenderParams_.get());
    ASSERT_EQ(param->GetUifirstChildrenDirtyRectParam(), DEFAULT_RECT);
}

/**
 * @tc.name: UifirstStateChange
 * @tc.desc: Test UifirstStateChange, not enabled case.
 * @tc.type: FUNC
 * @tc.require: #I9UNQP
 */
HWTEST_F(RsUifirstManagerTest, UifirstStateChange, TestSize.Level1)
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
 * @tc.name: UifirstStateChange
 * @tc.desc: Test UifirstStateChange, last frame not enabled, this frame enabled
 * @tc.type: FUNC
 * @tc.require: #I9UNQP
 */
HWTEST_F(RsUifirstManagerTest, UifirstStateChange02, TestSize.Level1)
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
 * @tc.require: #I9UNQP
 */
HWTEST_F(RsUifirstManagerTest, UifirstStateChange03, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);

    surfaceNode->SetLastFrameUifirstFlag(MultiThreadCacheType::LEASH_WINDOW);
    auto currentFrameCacheType = MultiThreadCacheType::LEASH_WINDOW;
    uifirstManager_.UifirstStateChange(*surfaceNode, currentFrameCacheType);
    ASSERT_EQ(surfaceNode->GetLastFrameUifirstFlag(), MultiThreadCacheType::LEASH_WINDOW);
}

/**
 * @tc.name: UifirstStateChange
 * @tc.desc: Test UifirstStateChange, last frame enabled, this frame disabled
 * @tc.type: FUNC
 * @tc.require: #I9UNQP
 */
HWTEST_F(RsUifirstManagerTest, UifirstStateChange04, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);

    surfaceNode->SetLastFrameUifirstFlag(MultiThreadCacheType::LEASH_WINDOW);
    auto currentFrameCacheType = MultiThreadCacheType::NONE;
    uifirstManager_.UifirstStateChange(*surfaceNode, currentFrameCacheType);
    ASSERT_EQ(surfaceNode->GetLastFrameUifirstFlag(), MultiThreadCacheType::NONE);
}

/**
 @tc.name: CollectSkipSyncBuffer01
 @tc.desc: Test CollectSkipSyncBuffer
 @tc.type: FUNC
 @tc.require: issue1IAMKU9
 */
HWTEST_F(RsUifirstManagerTest, CollectSkipSyncBuffer01, TestSize.Level1)
{
    NodeId id = 1;
    std::function<void()> task = {};
    std::vector<std::function<void()>> tasks;
    tasks.push_back(task);
    uifirstManager_.CollectSkipSyncBuffer(tasks, id);
    ASSERT_NE(tasks.empty(), true);
}

/**
 * @tc.name: ResetUifirstNode
 * @tc.desc: Test ResetUifirstNode
 * @tc.type: FUNC
 * @tc.require: issue1IADDL3
 */
HWTEST_F(RsUifirstManagerTest, ResetUifirstNode, TestSize.Level1)
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
 * @tc.require: issue1IADDL3
 */
HWTEST_F(RsUifirstManagerTest, NotifyUIStartingWindow, TestSize.Level1)
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
 * @tc.name: PostSubTaskAndPostReleaseCacheSurfaceSubTask01
 * @tc.desc: Test PostSubTask And PostReleaseCacheSurfaceSubTask
 * @tc.type: FUNC
 * @tc.require: issue1IADDL3
 */
HWTEST_F(RsUifirstManagerTest, PostSubTaskAndPostReleaseCacheSurfaceSubTask01, TestSize.Level1)
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
 * @tc.name: CheckIfAppWindowHasAnimation
 * @tc.desc: Test CheckIfAppWindowHasAnimation
 * @tc.type: FUNC
 * @tc.require: #I9UNQP
 */
HWTEST_F(RsUifirstManagerTest, CheckIfAppWindowHasAnimation, TestSize.Level1)
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
 * @tc.require: #I9UNQP
 */
HWTEST_F(RsUifirstManagerTest, CheckIfAppWindowHasAnimation01, TestSize.Level1)
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
 * @tc.require: #I9UNQP
 */
HWTEST_F(RsUifirstManagerTest, CheckIfAppWindowHasAnimation02, TestSize.Level1)
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
 * @tc.require: #I9UNQP
 */
HWTEST_F(RsUifirstManagerTest, CheckIfAppWindowHasAnimation03, TestSize.Level1)
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
 * @tc.name: UpdateUifirstNodes
 * @tc.desc: Test UpdateUifirstNodes, with different nodes
 * @tc.type: FUNC
 * @tc.require: #I9UNQP
 */
HWTEST_F(RsUifirstManagerTest, UpdateUifirstNodes, TestSize.Level1)
{
    auto surfaceNode1 = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode1, nullptr);
    surfaceNode1->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
    surfaceNode1->isChildSupportUifirst_ = true;
    uifirstManager_.UpdateUifirstNodes(*surfaceNode1, true);

    auto surfaceNode2 = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode2, nullptr);
    surfaceNode2->SetSurfaceNodeType(RSSurfaceNodeType::SELF_DRAWING_WINDOW_NODE);
    surfaceNode2->isChildSupportUifirst_ = true;
    uifirstManager_.UpdateUifirstNodes(*surfaceNode2, true);

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
}

/**
 @tc.name: ReleaseSkipSyncBuffer01
 @tc.desc: Test ReleaseSkipSyncBuffer
 @tc.type: FUNC
 @tc.require: issue1IAMKU9
 */
HWTEST_F(RsUifirstManagerTest, ReleaseSkipSyncBuffer01, TestSize.Level1)
{
    std::function<void()> task = {};
    std::vector<std::function<void()>> tasks;
    tasks.push_back(task);
    uifirstManager_.ReleaseSkipSyncBuffer(tasks);
    ASSERT_NE(tasks.empty(), true);
}

/**
 * @tc.name: UpdateSkipSyncNode01
 * @tc.desc: Test UpdateSkipSyncNode
 * @tc.type: FUNC
 * @tc.require: issue1IADDL3
 */
HWTEST_F(RsUifirstManagerTest, UpdateSkipSyncNode01, TestSize.Level1)
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
}

/**
 * @tc.name: ConvertPendingNodeToDrawable01
 * @tc.desc: Test ConvertPendingNodeToDrawable
 * @tc.type: FUNC
 * @tc.require: issue1IADDL3
 */
HWTEST_F(RsUifirstManagerTest, ConvertPendingNodeToDrawable01, TestSize.Level1)
{
    uifirstManager_.useDmaBuffer_ = false;
    uifirstManager_.ConvertPendingNodeToDrawable();
    EXPECT_FALSE(uifirstManager_.useDmaBuffer_);

    uifirstManager_.useDmaBuffer_ = true;
    uifirstManager_.ConvertPendingNodeToDrawable();
    EXPECT_TRUE(uifirstManager_.useDmaBuffer_);

    uifirstManager_.pendingPostNodes_.clear();
    std::shared_ptr<RSSurfaceRenderNode> node = nullptr;
    uifirstManager_.pendingPostNodes_.insert(std::make_pair(0, node));
    uifirstManager_.ConvertPendingNodeToDrawable();
    EXPECT_FALSE(uifirstManager_.pendingPostNodes_.empty());

    uifirstManager_.pendingPostNodes_.clear();
    auto renderNode = std::make_shared<RSSurfaceRenderNode>(0);
    uifirstManager_.pendingPostNodes_.insert(std::make_pair(0, renderNode));
    uifirstManager_.ConvertPendingNodeToDrawable();
    EXPECT_FALSE(uifirstManager_.pendingPostNodes_.empty());
}

/**
 * @tc.name: CollectSkipSyncNode01
 * @tc.desc: Test CollectSkipSyncNode
 * @tc.type: FUNC
 * @tc.require: issue1IADDL3
 */
HWTEST_F(RsUifirstManagerTest, CollectSkipSyncNode01, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> node = nullptr;
    bool res = uifirstManager_.CollectSkipSyncNode(node);
    EXPECT_FALSE(res);
    EXPECT_TRUE(uifirstManager_.pendingPostNodes_.size() == 1);

    node = std::make_shared<RSRenderNode>(1);
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
 * @tc.name: RestoreSkipSyncNode01
 * @tc.desc: Test RestoreSkipSyncNode
 * @tc.type: FUNC
 * @tc.require: issue1IADDL3
 */
HWTEST_F(RsUifirstManagerTest, RestoreSkipSyncNode01, TestSize.Level1)
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
 * @tc.name: AddPendingPostNode01
 * @tc.desc: Test AddPendingPostNode
 * @tc.type: FUNC
 * @tc.require: issue1IADDL3
 */
HWTEST_F(RsUifirstManagerTest, AddPendingPostNode01, TestSize.Level1)
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
 * @tc.name: LeashWindowContainMainWindowAndStarting01
 * @tc.desc: Test LeashWindowContainMainWindowAndStarting
 * @tc.type: FUNC
 * @tc.require: issue1IADDL3
 */
HWTEST_F(RsUifirstManagerTest, LeashWindowContainMainWindowAndStarting01, TestSize.Level1)
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
 * @tc.name: AddPendingResetNode01
 * @tc.desc: Test AddPendingResetNode
 * @tc.type: FUNC
 * @tc.require: issue1IADDL3
 */
HWTEST_F(RsUifirstManagerTest, AddPendingResetNode01, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(0);
    uifirstManager_.AddPendingResetNode(0, node);
    EXPECT_TRUE(node);

    uifirstManager_.AddPendingResetNode(1, node);
    EXPECT_TRUE(node);
}

/**
 * @tc.name: GetNodeStatus01
 * @tc.desc: Test GetNodeStatus
 * @tc.type: FUNC
 * @tc.require: issue1IADDL3
 */
HWTEST_F(RsUifirstManagerTest, GetNodeStatus01, TestSize.Level1)
{
    CacheProcessStatus status = uifirstManager_.GetNodeStatus(0);
    EXPECT_EQ(status, CacheProcessStatus::UNKNOWN);

    status = uifirstManager_.GetNodeStatus(2);
    EXPECT_EQ(status, CacheProcessStatus::UNKNOWN);
}

/**
 * @tc.name: ClearSubthreadRes01
 * @tc.desc: Test ClearSubthreadRes
 * @tc.type: FUNC
 * @tc.require: issue1IADDL3
 */
HWTEST_F(RsUifirstManagerTest, ClearSubthreadRes01, TestSize.Level1)
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
 * @tc.name: SetNodePriorty01
 * @tc.desc: Test SetNodePriorty
 * @tc.type: FUNC
 * @tc.require: issue1IAOJHQ
 */
HWTEST_F(RsUifirstManagerTest, SetNodePriorty01, TestSize.Level1)
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
 * @tc.name: IsInLeashWindowTree01
 * @tc.desc: Test IsInLeashWindowTree
 * @tc.type: FUNC
 * @tc.require: issue1IADDL3
 */
HWTEST_F(RsUifirstManagerTest, IsInLeashWindowTree01, TestSize.Level1)
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
 * @tc.name: AddReuseNode01
 * @tc.desc: Test AddReuseNode
 * @tc.type: FUNC
 * @tc.require: issue1IADDL3
 */
HWTEST_F(RsUifirstManagerTest, AddReuseNode01, TestSize.Level1)
{
    uifirstManager_.AddReuseNode(0);
    EXPECT_TRUE(uifirstManager_.reuseNodes_.empty());

    uifirstManager_.AddReuseNode(1);
    EXPECT_FALSE(uifirstManager_.reuseNodes_.empty());
}

/**
 * @tc.name: OnProcessEventComplete01
 * @tc.desc: Test OnProcessEventComplete
 * @tc.type: FUNC
 * @tc.require: issue1IADDL3
 */
HWTEST_F(RsUifirstManagerTest, OnProcessEventComplete01, TestSize.Level1)
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
 * @tc.name: EventDisableLeashWindowCache01
 * @tc.desc: Test EventDisableLeashWindowCache
 * @tc.type: FUNC
 * @tc.require: issue1IADDL3
 */
HWTEST_F(RsUifirstManagerTest, EventDisableLeashWindowCache01, TestSize.Level1)
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
 * @tc.name: PrepareCurrentFrameEvent01
 * @tc.desc: Test PrepareCurrentFrameEvent
 * @tc.type: FUNC
 * @tc.require: issue1IADDL3
 */
HWTEST_F(RsUifirstManagerTest, PrepareCurrentFrameEvent01, TestSize.Level1)
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
 * @tc.name: OnProcessAnimateScene01
 * @tc.desc: Test OnProcessAnimateScene
 * @tc.type: FUNC
 * @tc.require: issue1IADDL3
 */
HWTEST_F(RsUifirstManagerTest, OnProcessAnimateScene01, TestSize.Level1)
{
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

/**
 * @tc.name: NodeIsInCardWhiteList01
 * @tc.desc: Test NodeIsInCardWhiteList
 * @tc.type: FUNC
 * @tc.require: issue1IADDL3
 */
HWTEST_F(RsUifirstManagerTest, NodeIsInCardWhiteList01, TestSize.Level1)
{
    RSRenderNode node(0);
    bool res = uifirstManager_.NodeIsInCardWhiteList(node);
    EXPECT_FALSE(res);
    EXPECT_FALSE(uifirstManager_.entryViewNodeId_);
    EXPECT_FALSE(uifirstManager_.negativeScreenNodeId_);

    uifirstManager_.negativeScreenNodeId_ = 1;
    res = uifirstManager_.NodeIsInCardWhiteList(node);
    EXPECT_FALSE(res);

    uifirstManager_.entryViewNodeId_ = 1;
    res = uifirstManager_.NodeIsInCardWhiteList(node);
    EXPECT_FALSE(res);

    node.instanceRootNodeId_ = 1;
    res = uifirstManager_.NodeIsInCardWhiteList(node);
    EXPECT_TRUE(res);

    uifirstManager_.entryViewNodeId_ = 2;
    node.instanceRootNodeId_ = 2;
    res = uifirstManager_.NodeIsInCardWhiteList(node);
    EXPECT_TRUE(res);

    node.instanceRootNodeId_ = 3;
    res = uifirstManager_.NodeIsInCardWhiteList(node);
    EXPECT_FALSE(res);
}

/**
 * @tc.name: IsCardSkipFirstWaitScene01
 * @tc.desc: Test IsCardSkipFirstWaitScene
 * @tc.type: FUNC
 * @tc.require: issue1IADDL3
 */
HWTEST_F(RsUifirstManagerTest, IsCardSkipFirstWaitScene01, TestSize.Level1)
{
    std::string scene = "";
    int32_t appPid = 1;
    bool res = uifirstManager_.IsCardSkipFirstWaitScene(scene, appPid);
    EXPECT_FALSE(res);

    appPid = 0;
    res = uifirstManager_.IsCardSkipFirstWaitScene(scene, appPid);
    EXPECT_FALSE(res);

    scene = "INTO_HOME_ANI"; // for test
    res = uifirstManager_.IsCardSkipFirstWaitScene(scene, appPid);
    EXPECT_TRUE(res);
}

/**
 * @tc.name: EventsCanSkipFirstWait01
 * @tc.desc: Test EventsCanSkipFirstWait
 * @tc.type: FUNC
 * @tc.require: issue1IADDL3
 */
HWTEST_F(RsUifirstManagerTest, EventsCanSkipFirstWait01, TestSize.Level1)
{
    std::vector<RSUifirstManager::EventInfo> events;
    bool res = uifirstManager_.EventsCanSkipFirstWait(events);
    EXPECT_FALSE(res);

    RSUifirstManager::EventInfo info;
    info.sceneId = "";
    info.appPid = 0;
    events.push_back(info);
    res = uifirstManager_.EventsCanSkipFirstWait(events);
    EXPECT_FALSE(res);

    info.sceneId = "INTO_HOME_ANI"; // for test
    events.push_back(info);
    res = uifirstManager_.EventsCanSkipFirstWait(events);
    EXPECT_TRUE(res);
}

/**
 * @tc.name: IsScreenshotAnimation01
 * @tc.desc: Test IsScreenshotAnimation
 * @tc.type: FUNC
 * @tc.require: issue1IADDL3
 */
HWTEST_F(RsUifirstManagerTest, IsScreenshotAnimation01, TestSize.Level1)
{
    EXPECT_FALSE(uifirstManager_.currentFrameEvent_.empty());
    bool res = uifirstManager_.IsScreenshotAnimation();
    EXPECT_FALSE(res);

    RSUifirstManager::EventInfo info;
    info.sceneId = "SCREENSHOT_SCALE_ANIMATION"; // for test
    uifirstManager_.currentFrameEvent_.push_back(info);
    res = uifirstManager_.IsScreenshotAnimation();
    EXPECT_TRUE(res);
}

/**
 * @tc.name: UpdateUifirstNodes01
 * @tc.desc: Test UpdateUifirstNodes
 * @tc.type: FUNC
 * @tc.require: issue1IADDL3
 */
HWTEST_F(RsUifirstManagerTest, UpdateUifirstNodes01, TestSize.Level1)
{
    RSSurfaceRenderNode node(0);
    bool ancestorNodeHasAnimation = true;
    uifirstManager_.UpdateUifirstNodes(node, ancestorNodeHasAnimation);
    EXPECT_TRUE(node.GetUifirstSupportFlag());
    EXPECT_FALSE(uifirstManager_.isUiFirstOn_);

    uifirstManager_.isUiFirstOn_ = true;
    uifirstManager_.UpdateUifirstNodes(node, ancestorNodeHasAnimation);
    EXPECT_TRUE(uifirstManager_.isUiFirstOn_);

    node.isChildSupportUifirst_ = false;
    uifirstManager_.UpdateUifirstNodes(node, ancestorNodeHasAnimation);
    EXPECT_TRUE(uifirstManager_.isUiFirstOn_);
}

/**
 * @tc.name: UpdateUIFirstNodeUseDma01
 * @tc.desc: Test UpdateUIFirstNodeUseDma
 * @tc.type: FUNC
 * @tc.require: issue1IADDL3
 */
HWTEST_F(RsUifirstManagerTest, UpdateUIFirstNodeUseDma01, TestSize.Level1)
{
    RSSurfaceRenderNode node(0);
    std::vector<RectI> rects;
    uifirstManager_.UpdateUIFirstNodeUseDma(node, rects);
    EXPECT_FALSE(uifirstManager_.GetUseDmaBuffer(node.GetName()));
}

/**
 * @tc.name: DoPurgePendingPostNodes01
 * @tc.desc: Test DoPurgePendingPostNodes
 * @tc.type: FUNC
 * @tc.require: issue1IAOJHQ
 */
HWTEST_F(RsUifirstManagerTest, DoPurgePendingPostNodes01, TestSize.Level1)
{
    std::unordered_map<NodeId, std::shared_ptr<RSSurfaceRenderNode>> pendingNode;
    NodeId nodeId = 1;
    auto surfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(0);
    pendingNode.insert(std::make_pair(nodeId, surfaceRenderNode));
    uifirstManager_.subthreadProcessingNode_.clear();
    uifirstManager_.DoPurgePendingPostNodes(pendingNode);
    EXPECT_FALSE(pendingNode.empty());

    auto node = std::make_shared<RSSurfaceRenderNode>(nodeId);
    auto adapter = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(node));
    adapter->renderParams_ = std::make_unique<RSSurfaceRenderParams>(0);
    surfaceRenderNode->lastFrameUifirstFlag_ = MultiThreadCacheType::ARKTS_CARD;
    uifirstManager_.subthreadProcessingNode_.insert(std::make_pair(nodeId, adapter));
    uifirstManager_.DoPurgePendingPostNodes(pendingNode);
    EXPECT_FALSE(pendingNode.empty());

    uifirstManager_.DoPurgePendingPostNodes(pendingNode);
    EXPECT_FALSE(pendingNode.empty());

    adapter->hasHdrPresent_ = true;
    uifirstManager_.subthreadProcessingNode_.insert(std::make_pair(nodeId, adapter));
    uifirstManager_.DoPurgePendingPostNodes(pendingNode);
    EXPECT_FALSE(pendingNode.empty());
}

/**
 * @tc.name: GetUiFirstMode
 * @tc.desc: Test GetUiFirstMode
 * @tc.type: FUNC
 * @tc.require: issue1IANPC2
 */
HWTEST_F(RsUifirstManagerTest, GetUiFirstMode, TestSize.Level1)
{
    if (RSMainThread::Instance()->GetDeviceType() == DeviceType::PHONE) {
        auto type = uifirstManager_.GetUiFirstMode();
        EXPECT_EQ(type, UiFirstModeType::SINGLE_WINDOW_MODE);
    }

    if (RSMainThread::Instance()->GetDeviceType() == DeviceType::PC) {
        auto type = uifirstManager_.GetUiFirstMode();
        EXPECT_EQ(type, UiFirstModeType::MULTI_WINDOW_MODE);
    }

    if (RSMainThread::Instance()->GetDeviceType() == DeviceType::TABLET) {
        uifirstManager_.SetFreeMultiWindowStatus(false);
        auto type = uifirstManager_.GetUiFirstMode();
        EXPECT_EQ(type, UiFirstModeType::SINGLE_WINDOW_MODE);

        uifirstManager_.SetFreeMultiWindowStatus(true);
        type = uifirstManager_.GetUiFirstMode();
        EXPECT_EQ(type, UiFirstModeType::MULTI_WINDOW_MODE);
    }
}

/**
 * @tc.name: UpdateUifirstNodes02
 * @tc.desc: Test UpdateUifirstNodes
 * @tc.type: FUNC
 * @tc.require: issue1IAVLLE
 */
HWTEST_F(RsUifirstManagerTest, UpdateUifirstNodes02, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    if (RSMainThread::Instance()->GetDeviceType() != DeviceType::PC) {
        return;
    }

    surfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
    surfaceNode->isChildSupportUifirst_ = true;
    surfaceNode->firstLevelNodeId_ = surfaceNode->GetId();
    surfaceNode->forceUIFirst_ = true;
    surfaceNode->hasSharedTransitionNode_ = false;
    surfaceNode->hasFilter_ = false;
    surfaceNode->lastFrameUifirstFlag_ = MultiThreadCacheType::NONE;
    uifirstManager_.rotationChanged_ = false;
    uifirstManager_.UpdateUifirstNodes(*surfaceNode, true);
    auto param = static_cast<RSSurfaceRenderParams*>(surfaceNode->stagingRenderParams_.get());
    EXPECT_TRUE(param->GetUifirstNodeEnableParam() == MultiThreadCacheType::NONE);

    surfaceNode->lastFrameUifirstFlag_ = MultiThreadCacheType::NONFOCUS_WINDOW;
    uifirstManager_.UpdateUifirstNodes(*surfaceNode, true);
    EXPECT_TRUE(param->GetUifirstNodeEnableParam() == MultiThreadCacheType::NONFOCUS_WINDOW);
}
}