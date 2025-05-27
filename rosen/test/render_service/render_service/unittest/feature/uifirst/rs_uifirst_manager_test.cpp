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
#include "pipeline/rs_test_util.h"
#include "pipeline/rs_canvas_render_node.h"

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

    uifirstManager_.subthreadProcessDoneNode_.clear();
    uifirstManager_.markForceUpdateByUifirst_.clear();
    uifirstManager_.pendingPostNodes_.clear();
    uifirstManager_.pendingPostCardNodes_.clear();
    uifirstManager_.pendingResetNodes_.clear();

    mainThread->context_->globalRootRenderNode_->renderDrawable_ = nullptr;
    mainThread->context_->globalRootRenderNode_ = nullptr;
}
void RSUifirstManagerTest::SetUp() {}

void RSUifirstManagerTest::TearDown() {}

/**
 * @tc.name: GetSurfaceDrawableByID
 * @tc.desc: Test GetSurfaceDrawableByID
 * @tc.type: FUNC
 * @tc.require: #IBEL7U
 */
HWTEST_F(RSUifirstManagerTest, GetSurfaceDrawableByID, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    auto adapter = DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode);
    uifirstManager_.subthreadProcessingNode_.insert(std::make_pair(surfaceNode->GetId(), adapter));
    auto drawable = uifirstManager_.GetSurfaceDrawableByID(surfaceNode->GetId());
    ASSERT_NE(drawable, nullptr);

    NodeId id = 100;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(id);
    adapter = DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(canvasNode);
    uifirstManager_.subthreadProcessingNode_.insert(std::make_pair(canvasNode->GetId(), adapter));
    drawable = uifirstManager_.GetSurfaceDrawableByID(canvasNode->GetId());
    ASSERT_EQ(drawable, nullptr);
    uifirstManager_.subthreadProcessingNode_.clear();
}

/**
 * @tc.name: SetUifirstNodeEnableParam001
 * @tc.desc: Test SetUifirstNodeEnableParam, when node is leash window type
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSUifirstManagerTest, SetUifirstNodeEnableParam001, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
    uifirstManager_.SetUifirstNodeEnableParam(*surfaceNode, MultiThreadCacheType::NONE);
}

/**
 * @tc.name: SetUifirstNodeEnableParam002
 * @tc.desc: Test SetUifirstNodeEnableParam, with APP window child and FOREGROUND child
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSUifirstManagerTest, SetUifirstNodeEnableParam002, TestSize.Level1)
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
 * @tc.name: MergeOldDirty001
 * @tc.desc: Test MergeOldDirty
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSUifirstManagerTest, MergeOldDirty001, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->oldDirty_ = RSUifirstManagerTest::DEFAULT_RECT;
    auto surfaceDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
    ASSERT_NE(surfaceDrawable, nullptr);

    uifirstManager_.mainThread_ = nullptr;
    uifirstManager_.MergeOldDirty(surfaceNode->GetId());
    if (surfaceDrawable->GetSyncDirtyManager()) {
        ASSERT_TRUE(surfaceDrawable->GetSyncDirtyManager()->GetCurrentFrameDirtyRegion().IsEmpty());
    }

    uifirstManager_.mainThread_ = mainThread_;
    uifirstManager_.MergeOldDirty(surfaceNode->GetId());
    if (surfaceDrawable->GetSyncDirtyManager()) {
        ASSERT_TRUE(surfaceDrawable->GetSyncDirtyManager()->GetCurrentFrameDirtyRegion().IsEmpty());
    }

    mainThread_->context_->nodeMap.RegisterRenderNode(surfaceNode);
    surfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    uifirstManager_.MergeOldDirty(surfaceNode->GetId());
    if (surfaceDrawable->GetSyncDirtyManager()) {
        ASSERT_TRUE(surfaceDrawable->GetSyncDirtyManager()->GetCurrentFrameDirtyRegion().IsEmpty());
    }
}

/**
 * @tc.name: MergeOldDirty002
 * @tc.desc: Test MergeOldDirty, merge children dirty region
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSUifirstManagerTest, MergeOldDirty002, TestSize.Level1)
{
    auto parentNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(parentNode, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    parentNode->AddChild(surfaceNode);
    // add different children nodes
    auto childNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(childNode, nullptr);
    auto childDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(childNode));
    ASSERT_NE(childDrawable, nullptr);
    childNode->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    childNode->oldDirty_= RSUifirstManagerTest::DEFAULT_RECT;
    surfaceNode->AddChild(childNode);
    surfaceNode->GenerateFullChildrenList();
    uifirstManager_.mainThread_ = mainThread_;
    mainThread_->context_->nodeMap.RegisterRenderNode(surfaceNode);
    uifirstManager_.MergeOldDirty(surfaceNode->GetId());
    if (childDrawable->GetSyncDirtyManager()) {
        ASSERT_TRUE(childDrawable->GetSyncDirtyManager()->GetCurrentFrameDirtyRegion().IsEmpty());
    }
}

/**
 * @tc.name: RenderGroupUpdate001
 * @tc.desc: Test RenderGroupUpdate, when parent node is nullptr
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSUifirstManagerTest, RenderGroupUpdate001, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    auto surfaceDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
    uifirstManager_.RenderGroupUpdate(surfaceDrawable);
}

/**
 * @tc.name: RenderGroupUpdate002
 * @tc.desc: Test RenderGroupUpdate, when parent node is display node
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSUifirstManagerTest, RenderGroupUpdate002, TestSize.Level1)
{
    NodeId id = 1;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    ASSERT_NE(displayNode, nullptr);
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(++id);
    ASSERT_NE(surfaceNode, nullptr);
    displayNode->AddChild(surfaceNode);
    mainThread_->context_->nodeMap.RegisterRenderNode(surfaceNode);
    auto surfaceDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(surfaceNode->GetRenderDrawable());
    uifirstManager_.RenderGroupUpdate(surfaceDrawable);
}

/**
 * @tc.name: RenderGroupUpdate003
 * @tc.desc: Test RenderGroupUpdate, when parent node is surface node
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSUifirstManagerTest, RenderGroupUpdate003, TestSize.Level1)
{
    auto parentNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(parentNode, nullptr);
    auto childNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(childNode, nullptr);
    parentNode->AddChild(childNode);
    parentNode->nodeGroupType_ = RSUifirstManagerTest::GROUPED_BY_ANIM;
    auto surfaceDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(childNode));
    uifirstManager_.RenderGroupUpdate(surfaceDrawable);
}

/**
 * @tc.name: ProcessForceUpdateNode001
 * @tc.desc: Test ProcessForceUpdateNode, input invalid nodeid
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSUifirstManagerTest, ProcessForceUpdateNode001, TestSize.Level1)
{
    ASSERT_NE(mainThread_, nullptr);
    uifirstManager_.pendingForceUpdateNode_.push_back(INVALID_NODEID);
    uifirstManager_.ProcessForceUpdateNode();
}

/**
 * @tc.name: ProcessForceUpdateNode002
 * @tc.desc: Test ProcessForceUpdateNode, when parent node is surface node
 * @tc.type: FUNC
 * @tc.require: issueIAOJHQ
 */
HWTEST_F(RSUifirstManagerTest, ProcessForceUpdateNode002, TestSize.Level1)
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
HWTEST_F(RSUifirstManagerTest, ProcessDoneNode, TestSize.Level1)
{
    uifirstManager_.subthreadProcessDoneNode_.push_back(INVALID_NODEID);
    uifirstManager_.ProcessDoneNode();
    ASSERT_TRUE(uifirstManager_.subthreadProcessDoneNode_.empty());
}

/**
* @tc.name: ProcessDoneNode001
* @tc.desc: Test ProcessDoneNode
* @tc.type: FUNC
* @tc.require: issueIAOJHQ
*/
HWTEST_F(RSUifirstManagerTest, ProcessDoneNode001, TestSize.Level1)
{
    NodeId id = 1;
    auto surfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(id);
    auto adapter = DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceRenderNode);
    uifirstManager_.subthreadProcessingNode_.insert(std::make_pair(id, adapter));
    uifirstManager_.ProcessDoneNode();
    EXPECT_FALSE(uifirstManager_.subthreadProcessingNode_.empty());

    uifirstManager_.pendingResetNodes_.insert(std::make_pair(id, surfaceRenderNode));
    uifirstManager_.ProcessDoneNode();
    EXPECT_FALSE(uifirstManager_.pendingResetNodes_.empty());

    std::shared_ptr<RSRenderNodeDrawable> renderNodeDrawable = nullptr;
    uifirstManager_.subthreadProcessingNode_.insert(std::make_pair(++id, renderNodeDrawable));
    uifirstManager_.ProcessDoneNode();
    EXPECT_FALSE(uifirstManager_.subthreadProcessingNode_.empty());
}

/**
 * @tc.name: CurSurfaceHasVisibleDirtyRegion
 * @tc.desc: Test CurSurfaceHasVisibleDirtyRegion, node has no child
 * @tc.type: FUNC
 * @tc.require: issueIBJQV8
 */
HWTEST_F(RSUifirstManagerTest, CurSurfaceHasVisibleDirtyRegion, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    ASSERT_FALSE(uifirstManager_.CurSurfaceHasVisibleDirtyRegion(surfaceNode));
    Occlusion::Region region({50, 50, 100, 100});
    surfaceNode->SetVisibleRegion(region);
    ASSERT_FALSE(uifirstManager_.CurSurfaceHasVisibleDirtyRegion(surfaceNode));

    auto surfaceDrawable = surfaceNode->GetRenderDrawable();
    surfaceDrawable->GetSyncDirtyManager()->SetUifirstFrameDirtyRect(RectI{0, 0, 100, 100});
    ASSERT_TRUE(uifirstManager_.CurSurfaceHasVisibleDirtyRegion(surfaceNode));
}

/**
 * @tc.name: CheckVisibleDirtyRegionIsEmpty
 * @tc.desc: Test CheckVisibleDirtyRegionIsEmpty, node has no child
 * @tc.type: FUNC
 * @tc.require: #I9UNQP
 */
HWTEST_F(RSUifirstManagerTest, CheckVisibleDirtyRegionIsEmpty001, TestSize.Level1)
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
HWTEST_F(RSUifirstManagerTest, CheckVisibleDirtyRegionIsEmpty002, TestSize.Level1)
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
HWTEST_F(RSUifirstManagerTest, CheckVisibleDirtyRegionIsEmpty003, TestSize.Level1)
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
 * @tc.name: SyncHDRDisplayParam
 * @tc.desc: Test SyncHDRDisplayParam
 * @tc.type: FUNC
 * @tc.require: #IB8HZA
 */
HWTEST_F(RSUifirstManagerTest, SyncHDRDisplayParam, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    auto surfaceDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
    ASSERT_NE(surfaceDrawable, nullptr);
    surfaceDrawable->SetTargetColorGamut(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    NodeId id = 10;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    auto surfaceRenderParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable->renderParams_.get());
    surfaceRenderParams->SetAncestorDisplayNode(displayNode);
    auto colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
    uifirstManager_.SyncHDRDisplayParam(surfaceDrawable, colorGamut);
}
/**
 * @tc.name: ProcessTreeStateChange
 * @tc.desc: Test ProcessTreeStateChange, early return case
 * @tc.type: FUNC
 * @tc.require: #I9UNQP
 */
HWTEST_F(RSUifirstManagerTest, ProcessTreeStateChange, TestSize.Level1)
{
    auto surfaceNode1 = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode1, nullptr);
    surfaceNode1->SetIsOnTheTree(true);
    uifirstManager_.ProcessTreeStateChange(*surfaceNode1);
}

/**
 * @tc.name: UpdateChildrenDirtyRect
 * @tc.desc: Test UpdateChildrenDirtyRect if node is not leash window, return early.
 * @tc.type: FUNC
 * @tc.require: #I9UNQP
 */
HWTEST_F(RSUifirstManagerTest, UpdateChildrenDirtyRect001, TestSize.Level1)
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
HWTEST_F(RSUifirstManagerTest, UpdateChildrenDirtyRect002, TestSize.Level1)
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
 * @tc.name: UifirstStateChange
 * @tc.desc: Test UifirstStateChange, last frame not enabled, this frame enabled
 * @tc.type: FUNC
 * @tc.require: #I9UNQP
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
 * @tc.require: #I9UNQP
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
 * @tc.name: UifirstStateChange
 * @tc.desc: Test UifirstStateChange, last frame enabled, this frame disabled
 * @tc.type: FUNC
 * @tc.require: #I9UNQP
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
 * @tc.require: #I9UNQP
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
 * @tc.require: #I9UNQP
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
 * @tc.require: #I9UNQP
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
 * @tc.require: #I9UNQP
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
 * @tc.name: UpdateUifirstNodes
 * @tc.desc: Test UpdateUifirstNodes, with deviceType is Phone
 * @tc.type: FUNC
 * @tc.require: #IBOBU1
 */
HWTEST_F(RSUifirstManagerTest, UpdateUifirstNodesPhone001, TestSize.Level1)
{
    uifirstManager_.isUiFirstOn_ = true;
    auto surfaceNode1 = RSTestUtil::CreateSurfaceNode();
    surfaceNode1->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
    surfaceNode1->firstLevelNodeId_ = surfaceNode1->GetId();
    // 1. surfaceNode1 only has animation.
    uifirstManager_.UpdateUifirstNodes(*surfaceNode1, true);
    ASSERT_EQ(surfaceNode1->lastFrameUifirstFlag_, MultiThreadCacheType::LEASH_WINDOW);
    // 2. surfaceNode1 not has animation.
    uifirstManager_.UpdateUifirstNodes(*surfaceNode1, false);
    ASSERT_EQ(surfaceNode1->lastFrameUifirstFlag_, MultiThreadCacheType::NONE);
    // 3. surfaceNode1 has animation and filter.
    surfaceNode1->SetHasFilter(true);
    uifirstManager_.UpdateUifirstNodes(*surfaceNode1, true);
    ASSERT_EQ(surfaceNode1->lastFrameUifirstFlag_, MultiThreadCacheType::NONE);
    // 4. surfaceNode1 has animation, filter and rotation.
    uifirstManager_.rotationChanged_ = true;
    uifirstManager_.UpdateUifirstNodes(*surfaceNode1, true);
    ASSERT_EQ(surfaceNode1->lastFrameUifirstFlag_, MultiThreadCacheType::NONE);

    // 5. surfaceNode2 has animation, WindowType is scb.
    auto surfaceNode2 = RSTestUtil::CreateSurfaceNode();
    surfaceNode2->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
    surfaceNode2->firstLevelNodeId_ = surfaceNode2->GetId();
    uifirstManager_.rotationChanged_ = false;
    const_cast<SurfaceWindowType&>(surfaceNode2->surfaceWindowType_) = SurfaceWindowType::SYSTEM_SCB_WINDOW;
    uifirstManager_.UpdateUifirstNodes(*surfaceNode2, true);
    ASSERT_EQ(surfaceNode2->lastFrameUifirstFlag_, MultiThreadCacheType::NONE);

    // 6. surfaceNode3 has animation, nodeType is self_drawing.
    auto surfaceNode3 = RSTestUtil::CreateSurfaceNode();
    surfaceNode3->SetSurfaceNodeType(RSSurfaceNodeType::SELF_DRAWING_NODE);
    surfaceNode3->firstLevelNodeId_ = surfaceNode3->GetId();
    uifirstManager_.UpdateUifirstNodes(*surfaceNode3, true);
    ASSERT_EQ(surfaceNode3->lastFrameUifirstFlag_, MultiThreadCacheType::NONE);
}

/**
 * @tc.name: UpdateUifirstNodes
 * @tc.desc: Test UpdateUifirstNodes, with deviceType is Phone
 * @tc.type: FUNC
 * @tc.require: #IBOBU1
 */
HWTEST_F(RSUifirstManagerTest, UpdateUifirstNodesPhone002, TestSize.Level1)
{
    // 1. RecentTaskScene
    // 2. surfaceNode1 not has animation, has scale, children is surfaceNdoe2
    auto surfaceNode1 = RSTestUtil::CreateSurfaceNode();
    surfaceNode1->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
    surfaceNode1->firstLevelNodeId_ = surfaceNode1->GetId();
    uifirstManager_.isRecentTaskScene_ = true;
    surfaceNode1->SetIsScale(true);
    auto surfaceNode2 = RSTestUtil::CreateSurfaceNode();
    surfaceNode1->UpdateChildSubSurfaceNodes(surfaceNode2, true);
    uifirstManager_.UpdateUifirstNodes(*surfaceNode1, false);
    ASSERT_EQ(surfaceNode1->lastFrameUifirstFlag_, MultiThreadCacheType::LEASH_WINDOW);
    uifirstManager_.isRecentTaskScene_ = false;

    // 3. cardNode
    RSDisplayNodeConfig displayConfig;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(10, displayConfig);
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = ++RSTestUtil::id;
    surfaceConfig.name = "ArkTSCardNode";
    auto surfaceNode3 = RSTestUtil::CreateSurfaceNode(surfaceConfig);
    surfaceNode3->SetSurfaceNodeType(RSSurfaceNodeType::ABILITY_COMPONENT_NODE);
    surfaceNode3->firstLevelNodeId_ = surfaceNode3->GetId();
    surfaceNode3->SetAncestorDisplayNode(displayNode);
    uifirstManager_.entryViewNodeId_ = 1;
    uifirstManager_.negativeScreenNodeId_ = 1;
    surfaceNode3->instanceRootNodeId_ = 1;
    surfaceNode3->shouldPaint_ = true;
    uifirstManager_.UpdateUifirstNodes(*surfaceNode3, false);
    ASSERT_EQ(surfaceNode3->lastFrameUifirstFlag_, MultiThreadCacheType::ARKTS_CARD);
}

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
 * @tc.name: NotifyUIStartingWindow
 * @tc.desc: Test NotifyUIStartingWindow
 * @tc.type: FUNC
 * @tc.require: issueIADDL3
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

    status = uifirstManager_.GetNodeStatus(2);
    EXPECT_EQ(status, CacheProcessStatus::UNKNOWN);
}

/**
 * @tc.name: AddReuseNode001
 * @tc.desc: Test AddReuseNode
 * @tc.type: FUNC
 * @tc.require: issueIADDL3
 */
HWTEST_F(RSUifirstManagerTest, AddReuseNode001, TestSize.Level1)
{
    uifirstManager_.AddReuseNode(0);
    EXPECT_TRUE(uifirstManager_.reuseNodes_.empty());

    uifirstManager_.AddReuseNode(1);
    EXPECT_FALSE(uifirstManager_.reuseNodes_.empty());
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
 * @tc.name: NodeIsInCardWhiteList001
 * @tc.desc: Test NodeIsInCardWhiteList
 * @tc.type: FUNC
 * @tc.require: issueIADDL3
 */
HWTEST_F(RSUifirstManagerTest, NodeIsInCardWhiteList001, TestSize.Level1)
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
 * @tc.name: IsCardSkipFirstWaitScene001
 * @tc.desc: Test IsCardSkipFirstWaitScene
 * @tc.type: FUNC
 * @tc.require: issueIADDL3
 */
HWTEST_F(RSUifirstManagerTest, IsCardSkipFirstWaitScene001, TestSize.Level1)
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
 * @tc.name: EventsCanSkipFirstWait001
 * @tc.desc: Test EventsCanSkipFirstWait
 * @tc.type: FUNC
 * @tc.require: issueIADDL3
 */
HWTEST_F(RSUifirstManagerTest, EventsCanSkipFirstWait001, TestSize.Level1)
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
 * @tc.name: UpdateUifirstNodes001
 * @tc.desc: Test UpdateUifirstNodes
 * @tc.type: FUNC
 * @tc.require: issueIADDL3
 */
HWTEST_F(RSUifirstManagerTest, UpdateUifirstNodes001, TestSize.Level1)
{
    RSSurfaceRenderNode node(0);
    bool ancestorNodeHasAnimation = true;
    uifirstManager_.UpdateUifirstNodes(node, ancestorNodeHasAnimation);
    EXPECT_TRUE(node.GetUifirstSupportFlag());

    uifirstManager_.isUiFirstOn_ = true;
    uifirstManager_.UpdateUifirstNodes(node, ancestorNodeHasAnimation);
    EXPECT_TRUE(uifirstManager_.isUiFirstOn_);

    node.isChildSupportUifirst_ = false;
    uifirstManager_.UpdateUifirstNodes(node, ancestorNodeHasAnimation);
    EXPECT_TRUE(uifirstManager_.isUiFirstOn_);
}

/**
 * @tc.name: DoPurgePendingPostNodes001
 * @tc.desc: Test DoPurgePendingPostNodes
 * @tc.type: FUNC
 * @tc.require: issueIAOJHQ
 */
HWTEST_F(RSUifirstManagerTest, DoPurgePendingPostNodes001, TestSize.Level1)
{
    std::unordered_map<NodeId, std::shared_ptr<RSSurfaceRenderNode>> pendingNode;
    NodeId nodeId = 1;
    auto surfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(nodeId);
    auto adapter = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceRenderNode));
    pendingNode.insert(std::make_pair(nodeId, surfaceRenderNode));
    uifirstManager_.subthreadProcessingNode_.clear();
    uifirstManager_.subthreadProcessingNode_.insert(std::make_pair(nodeId, adapter));
    uifirstManager_.DoPurgePendingPostNodes(pendingNode);
    EXPECT_FALSE(pendingNode.empty());

    nodeId = 2;
    auto node = std::make_shared<RSSurfaceRenderNode>(nodeId);
    adapter = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(node));
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
    uifirstManager_.subthreadProcessingNode_.clear();
}

/**
 * @tc.name: GetUiFirstMode001
 * @tc.desc: Test GetUiFirstMode for phone
 * @tc.type: FUNC
 * @tc.require: issueIB31K8
 */
HWTEST_F(RSUifirstManagerTest, GetUiFirstMode001, TestSize.Level1)
{
    auto type = uifirstManager_.GetUiFirstMode();
    if (uifirstManager_.uifirstType_ == UiFirstCcmType::SINGLE) {
        EXPECT_EQ(type, UiFirstModeType::SINGLE_WINDOW_MODE);
    }
}

/**
 * @tc.name: GetUiFirstMode002
 * @tc.desc: Test GetUiFirstMode for pc
 * @tc.type: FUNC
 * @tc.require: issueIB31K8
 */
HWTEST_F(RSUifirstManagerTest, GetUiFirstMode002, TestSize.Level1)
{
    auto type = uifirstManager_.GetUiFirstMode();
    if (uifirstManager_.uifirstType_ == UiFirstCcmType::MULTI) {
        EXPECT_EQ(type, UiFirstModeType::MULTI_WINDOW_MODE);
    }
}

/**
 * @tc.name: GetUiFirstMode003
 * @tc.desc: Test GetUiFirstMode for tablet while free multi-window off
 * @tc.type: FUNC
 * @tc.require: issueIB31K8
 */
HWTEST_F(RSUifirstManagerTest, GetUiFirstMode003, TestSize.Level1)
{
    if (uifirstManager_.uifirstType_ != UiFirstCcmType::HYBRID) {
        return;
    }

    uifirstManager_.SetFreeMultiWindowStatus(false);
    auto type = uifirstManager_.GetUiFirstMode();
    EXPECT_EQ(type, UiFirstModeType::SINGLE_WINDOW_MODE);
}

/**
 * @tc.name: GetUiFirstMode004
 * @tc.desc: Test GetUiFirstMode for tablet while free multi-window on
 * @tc.type: FUNC
 * @tc.require: issueIB31K8
 */
HWTEST_F(RSUifirstManagerTest, GetUiFirstMode004, TestSize.Level1)
{
    if (uifirstManager_.uifirstType_ != UiFirstCcmType::HYBRID) {
        return;
    }

    uifirstManager_.SetFreeMultiWindowStatus(true);
    auto type = uifirstManager_.GetUiFirstMode();
    EXPECT_EQ(type, UiFirstModeType::MULTI_WINDOW_MODE);
}

/**
 * @tc.name: UpdateUifirstNodes002
 * @tc.desc: Test UpdateUifirstNodes
 * @tc.type: FUNC
 * @tc.require: issueIAVLLE
 */
HWTEST_F(RSUifirstManagerTest, UpdateUifirstNodes002, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    if (uifirstManager_.uifirstType_ != UiFirstCcmType::MULTI) {
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

/**
@tc.name: IsSubTreeNeedPrepareForSnapshot
@tc.desc: Test IsSubTreeNeedPrepareForSnapshot in recents.
@tc.type: FUNC
@tc.require: #IB7WHH
*/
HWTEST_F(RSUifirstManagerTest, IsSubTreeNeedPrepareForSnapshot, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    uifirstManager_.OnProcessAnimateScene(SystemAnimatedScenes::ENTER_RECENTS);
    bool isOccluded = uifirstManager_.IsSubTreeNeedPrepareForSnapshot(*surfaceNode);
    ASSERT_EQ(isOccluded, false);
}

/**
@tc.name: CheckAndWaitPreFirstLevelDrawableNotify
@tc.desc: Test CheckAndWaitPreFirstLevelDrawableNotify
@tc.type: FUNC
@tc.require: #IBHZJA
*/
HWTEST_F(RSUifirstManagerTest, CheckAndWaitPreFirstLevelDrawableNotify, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    auto renderParams = surfaceNode->GetRenderParams().get();
    bool res = RSUiFirstProcessStateCheckerHelper::CheckAndWaitPreFirstLevelDrawableNotify(*renderParams);
    ASSERT_TRUE(res);

    auto leashSurfaceNode = RSTestUtil::CreateSurfaceNode();
    renderParams->SetUiFirstRootNode(leashSurfaceNode->GetId());
    res = RSUiFirstProcessStateCheckerHelper::CheckAndWaitPreFirstLevelDrawableNotify(*renderParams);
    ASSERT_TRUE(res);
}

/**
@tc.name: IsCurFirstLevelMatch
@tc.desc: Test IsCurFirstLevelMatch
@tc.type: FUNC
@tc.require: #IBJQV8
*/
HWTEST_F(RSUifirstManagerTest, IsCurFirstLevelMatch, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    auto renderParams = surfaceNode->GetRenderParams().get();
    bool res = RSUiFirstProcessStateCheckerHelper::IsCurFirstLevelMatch(*renderParams);
    ASSERT_EQ(res, true);
}

/**
@tc.name: IsCurFirstLevelMatch
@tc.desc: Test IsCurFirstLevelMatch
@tc.type: FUNC
@tc.require: #IBJQV8
*/
HWTEST_F(RSUifirstManagerTest, CheckHwcChildrenType, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    SurfaceHwcNodeType enabledType = SurfaceHwcNodeType::DEFAULT_HWC_ROSENWEB;
    uifirstManager_.CheckHwcChildrenType(*surfaceNode, enabledType);
    
    surfaceNode->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    enabledType = SurfaceHwcNodeType::DEFAULT_HWC_VIDEO;
    uifirstManager_.CheckHwcChildrenType(*surfaceNode, enabledType);
    ASSERT_EQ(surfaceNode->childHardwareEnabledNodes_.size(), 0);

    surfaceNode->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    uifirstManager_.CheckHwcChildrenType(*surfaceNode, enabledType);
}
}