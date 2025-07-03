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
 * @tc.name: CheckVisibleDirtyRegionIsEmpty001
 * @tc.desc: Test special case of CheckVisibleDirtyRegionIsEmpty
 * @tc.type: FUNC
 * @tc.require: #I9UNQP
 */
HWTEST_F(RSUifirstManagerTest, CheckVisibleDirtyRegionIsEmpty001, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);

    uifirstManager_.SetUiFirstType((int)UiFirstCcmType::SINGLE);
    ASSERT_FALSE(uifirstManager_.CheckVisibleDirtyRegionIsEmpty(surfaceNode));

    uifirstManager_.SetUiFirstType((int)UiFirstCcmType::MULTI);

    // lockscreen to launcher anim
    surfaceNode->SetGlobalAlpha(0.f);
    surfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
    RSMainThread::Instance()->SetSystemAnimatedScenes(SystemAnimatedScenes::LOCKSCREEN_TO_LAUNCHER);
    ASSERT_TRUE(uifirstManager_.CheckVisibleDirtyRegionIsEmpty(surfaceNode));

    // uifirst content dirty
    surfaceNode->SetGlobalAlpha(1.f);
    surfaceNode->uifirstContentDirty_ = true;
    RSMainThread::Instance()->SetSystemAnimatedScenes(SystemAnimatedScenes::OTHERS);
    ASSERT_FALSE(uifirstManager_.CheckVisibleDirtyRegionIsEmpty(surfaceNode));
}

/**
 * @tc.name: CheckVisibleDirtyRegionIsEmpty002
 * @tc.desc: Test if surface has visible dirty region
 * @tc.type: FUNC
 * @tc.require: #I9UNQP
 */
HWTEST_F(RSUifirstManagerTest, CheckVisibleDirtyRegionIsEmpty002, TestSize.Level1)
{
    uifirstManager_.SetUiFirstType((int)UiFirstCcmType::MULTI);

    auto emptyRegion = Occlusion::Region();
    auto leashWindow = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(leashWindow, nullptr);
    leashWindow->SetGlobalAlpha(1.f);
    leashWindow->uifirstContentDirty_ = false;
    leashWindow->SetVisibleRegion(emptyRegion);

    auto appWindow = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(appWindow, nullptr);
    leashWindow->AddChild(appWindow);
    leashWindow->GenerateFullChildrenList();
    leashWindow->UpdateChildSubSurfaceNodes(appWindow, true);

    // surface invisible
    appWindow->SetVisibleRegion(emptyRegion);
    ASSERT_TRUE(uifirstManager_.CheckVisibleDirtyRegionIsEmpty(leashWindow));

    auto visibleRegion = Occlusion::Region({ 100, 100, 1440, 1000 });
    appWindow->SetVisibleRegion(visibleRegion);

    auto surfaceDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(appWindow->GetRenderDrawable());
    ASSERT_NE(surfaceDrawable, nullptr);
    auto dirtyManager = surfaceDrawable->GetSyncDirtyManager();
    ASSERT_NE(dirtyManager, nullptr);

    appWindow->renderDrawable_ = nullptr;
    // drawable is null
    ASSERT_FALSE(uifirstManager_.CheckVisibleDirtyRegionIsEmpty(leashWindow));
    appWindow->renderDrawable_ = surfaceDrawable;

    surfaceDrawable->syncDirtyManager_ = nullptr;
    // dirty manager is null
    ASSERT_FALSE(uifirstManager_.CheckVisibleDirtyRegionIsEmpty(leashWindow));
    surfaceDrawable->syncDirtyManager_ = dirtyManager;

    dirtyManager->SetUifirstFrameDirtyRect({ 0, 0, 0, 0 });
    // empty dirty region
    ASSERT_TRUE(uifirstManager_.CheckVisibleDirtyRegionIsEmpty(leashWindow));

    dirtyManager->SetUifirstFrameDirtyRect({ 1500, 200, 200, 200 });
    // no visible dirty region
    ASSERT_TRUE(uifirstManager_.CheckVisibleDirtyRegionIsEmpty(leashWindow));

    dirtyManager->SetUifirstFrameDirtyRect({ 200, 200, 200, 200 });
    // has visible dirty region
    ASSERT_FALSE(uifirstManager_.CheckVisibleDirtyRegionIsEmpty(leashWindow));
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
    surfaceDrawable->GetRsSubThreadCache().SetTargetColorGamut(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
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

    auto surfaceNode2 = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode2, nullptr);
    surfaceNode2->SetIsOnTheTree(false);
    surfaceNode2->SetIsNodeToBeCaptured(true);
    uifirstManager_.ProcessTreeStateChange(*surfaceNode2);
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
 * @tc.name: UifirstStateChange
 * @tc.desc: Test UifirstStateChange, clear pendingNodeBehindWindow_
 * @tc.type: FUNC
 * @tc.require: issueICIXW6
 */
HWTEST_F(RSUifirstManagerTest, UifirstStateChange005, TestSize.Level1)
{
    uifirstManager_.pendingNodeBehindWindow_.clear();
    NodeId nodeId = 0;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(nodeId);
    ASSERT_NE(surfaceNode, nullptr);
    uifirstManager_.pendingNodeBehindWindow_[nodeId] = RSUifirstManager::NodeDataBehindWindow{
        .curTime = uifirstManager_.GetMainThreadVsyncTime(),
        .isFirst = true
    };

    surfaceNode->SetLastFrameUifirstFlag(MultiThreadCacheType::NONFOCUS_WINDOW);
    auto currentFrameCacheType = MultiThreadCacheType::NONE;
    uifirstManager_.UifirstStateChange(*surfaceNode, currentFrameCacheType);
    ASSERT_EQ(uifirstManager_.pendingNodeBehindWindow_.size(), 0);

    surfaceNode->SetLastFrameUifirstFlag(MultiThreadCacheType::NONFOCUS_WINDOW);
    uifirstManager_.UifirstStateChange(*surfaceNode, currentFrameCacheType);
    ASSERT_EQ(uifirstManager_.pendingNodeBehindWindow_.size(), 0);

    uifirstManager_.pendingNodeBehindWindow_.clear();
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
    uifirstManager_.SetUiFirstType((int)UiFirstCcmType::SINGLE);
    auto surfaceNode1 = RSTestUtil::CreateSurfaceNode();
    surfaceNode1->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
    surfaceNode1->firstLevelNodeId_ = surfaceNode1->GetId();
    surfaceNode1->SetSubThreadAssignable(true);
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
    uifirstManager_.SetUiFirstType((int)UiFirstCcmType::SINGLE);
    surfaceNode1->SetIsScale(true);
    auto surfaceNode2 = RSTestUtil::CreateSurfaceNode();
    surfaceNode1->UpdateChildSubSurfaceNodes(surfaceNode2, true);
    surfaceNode1->SetSubThreadAssignable(true);
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
    surfaceNode3->SetSubThreadAssignable(true);
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
 * @tc.name: AddPendingNodeBehindWindow001
 * @tc.desc: Test AddPendingNodeBehindWindow
 * @tc.type: FUNC
 * @tc.require: issueICIXW6
 */
HWTEST_F(RSUifirstManagerTest, AddPendingNodeBehindWindow001, TestSize.Level1)
{
    NodeId id = 1;
    uifirstManager_.pendingNodeBehindWindow_.clear();

    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id);
    EXPECT_NE(node, nullptr);

    MultiThreadCacheType currentFrameCacheType = MultiThreadCacheType::NONE;
    uifirstManager_.AddPendingNodeBehindWindow(id, node, currentFrameCacheType);
    EXPECT_EQ(uifirstManager_.pendingNodeBehindWindow_.size(), 0);

    system::SetParameter("rosen.ui.first.behindwindow.enabled", "0");
    uifirstManager_.AddPendingNodeBehindWindow(id, node, currentFrameCacheType);
    EXPECT_EQ(uifirstManager_.pendingNodeBehindWindow_.size(), 0);

    currentFrameCacheType = MultiThreadCacheType::NONFOCUS_WINDOW;
    uifirstManager_.AddPendingNodeBehindWindow(id, node, currentFrameCacheType);
    EXPECT_EQ(uifirstManager_.pendingNodeBehindWindow_.size(), 0);

    system::SetParameter("rosen.ui.first.behindwindow.enabled", "1");
    currentFrameCacheType = MultiThreadCacheType::NONFOCUS_WINDOW;
    uifirstManager_.AddPendingNodeBehindWindow(id, node, currentFrameCacheType);
    EXPECT_EQ(uifirstManager_.pendingNodeBehindWindow_.size(), 1);

    uifirstManager_.AddPendingNodeBehindWindow(id, node, currentFrameCacheType);
    EXPECT_EQ(uifirstManager_.pendingNodeBehindWindow_.size(), 1);

    uifirstManager_.pendingNodeBehindWindow_.clear();
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

    adapter->GetRsSubThreadCache().hasHdrPresent_ = true;
    uifirstManager_.subthreadProcessingNode_.insert(std::make_pair(nodeId, adapter));
    uifirstManager_.DoPurgePendingPostNodes(pendingNode);
    EXPECT_FALSE(pendingNode.empty());
    uifirstManager_.subthreadProcessingNode_.clear();
}

/**
 * @tc.name: DoPurgePendingPostNodes002
 * @tc.desc: Test DoPurgePendingPostNodes
 * @tc.type: FUNC
 * @tc.require: issueICIXW6
 */
HWTEST_F(RSUifirstManagerTest, DoPurgePendingPostNodes002, TestSize.Level1)
{
    uifirstManager_.pendingNodeBehindWindow_.clear();
    uifirstManager_.SetPurgeEnable(true);
    auto emptyRegion = Occlusion::Region();
    auto visibleRegion = Occlusion::Region({50, 50, 100, 100});
    std::unordered_map<NodeId, std::shared_ptr<RSSurfaceRenderNode>> pendingNode;
    NodeId nodeId = 1;
    auto surfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(nodeId);
    auto adapter = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceRenderNode));
    pendingNode.insert(std::make_pair(nodeId, surfaceRenderNode));
    uifirstManager_.subthreadProcessingNode_.clear();
    uifirstManager_.subthreadProcessingNode_.insert(std::make_pair(nodeId, adapter));
    adapter->GetRsSubThreadCache().cacheSurface_ = std::make_shared<Drawing::Surface>();
    adapter->GetRsSubThreadCache().isCacheValid_ = true;
    uifirstManager_.UpdateCompletedSurface(surfaceRenderNode->GetId());

    surfaceRenderNode->SetGlobalAlpha(1.f);
    surfaceRenderNode->uifirstContentDirty_ = false;
    surfaceRenderNode->SetVisibleRegion(emptyRegion);
    surfaceRenderNode->SetVisibleRegionBehindWindow(emptyRegion);
    auto appWindow = RSTestUtil::CreateSurfaceNode();
    appWindow->SetVisibleRegion(visibleRegion);
    appWindow->SetVisibleRegionBehindWindow(emptyRegion);
    surfaceRenderNode->AddChild(appWindow);
    surfaceRenderNode->GenerateFullChildrenList();
    surfaceRenderNode->UpdateChildSubSurfaceNodes(appWindow, true);
    uifirstManager_.pendingNodeBehindWindow_[nodeId] = RSUifirstManager::NodeDataBehindWindow{
        .curTime = -30,
        .isFirst = false
    };
    uifirstManager_.DoPurgePendingPostNodes(pendingNode);
    EXPECT_FALSE(pendingNode.empty());

    surfaceRenderNode->SetLastFrameUifirstFlag(MultiThreadCacheType::NONFOCUS_WINDOW);
    uifirstManager_.DoPurgePendingPostNodes(pendingNode);
    EXPECT_FALSE(pendingNode.empty());

    uifirstManager_.subthreadProcessingNode_.clear();
    uifirstManager_.pendingNodeBehindWindow_.clear();
}

/**
 * @tc.name: NeedPurgeByBehindWindow001
 * @tc.desc: Test NeedPurgeByBehindWindow
 * @tc.type: FUNC
 * @tc.require: issueICIXW6
 */
HWTEST_F(RSUifirstManagerTest, NeedPurgeByBehindWindow001, TestSize.Level1)
{
    auto emptyRegion = Occlusion::Region();
    auto visibleRegion = Occlusion::Region({50, 50, 100, 100});
    uifirstManager_.SetPurgeEnable(true);
    bool hasTexture = true;
    NodeId id = 0;
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);

    surfaceNode->SetGlobalAlpha(1.f);
    surfaceNode->uifirstContentDirty_ = false;
    surfaceNode->SetLastFrameUifirstFlag(MultiThreadCacheType::NONFOCUS_WINDOW);
    surfaceNode->SetVisibleRegion(emptyRegion);
    surfaceNode->SetVisibleRegionBehindWindow(emptyRegion);

    auto appWindow = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(appWindow, nullptr);
    appWindow->SetVisibleRegion(visibleRegion);
    appWindow->SetVisibleRegionBehindWindow(emptyRegion);
    surfaceNode->AddChild(appWindow);
    surfaceNode->GenerateFullChildrenList();
    surfaceNode->UpdateChildSubSurfaceNodes(appWindow, true);

    uifirstManager_.SetUiFirstType((int)UiFirstCcmType::SINGLE);
    ASSERT_FALSE(uifirstManager_.NeedPurgeByBehindWindow(id, hasTexture, surfaceNode));

    uifirstManager_.SetUiFirstType((int)UiFirstCcmType::MULTI);
    system::SetParameter("rosen.ui.first.behindwindow.enabled", "0");
    ASSERT_FALSE(uifirstManager_.NeedPurgeByBehindWindow(id, hasTexture, surfaceNode));

    system::SetParameter("rosen.ui.first.behindwindow.enabled", "1");
    ASSERT_TRUE(uifirstManager_.NeedPurgeByBehindWindow(id, hasTexture, surfaceNode));

    uifirstManager_.SetUiFirstType((int)UiFirstCcmType::SINGLE);
}

/**
 * @tc.name: HandlePurgeBehindWindow001
 * @tc.desc: Test HandlePurgeBehindWindow
 * @tc.type: FUNC
 * @tc.require: issueICIXW6
 */
HWTEST_F(RSUifirstManagerTest, HandlePurgeBehindWindow001, TestSize.Level1)
{
    uifirstManager_.pendingNodeBehindWindow_.clear();
    NodeId nodeId = 1;
    auto surfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(nodeId);
    ASSERT_NE(surfaceRenderNode, nullptr);
    std::unordered_map<NodeId, std::shared_ptr<RSSurfaceRenderNode>> pendingNode;
    pendingNode.insert(std::make_pair(nodeId, surfaceRenderNode));
    auto it = pendingNode.begin();
    uifirstManager_.pendingNodeBehindWindow_[nodeId] = RSUifirstManager::NodeDataBehindWindow{
        .curTime = -30,
        .isFirst = false
    };

    uifirstManager_.HandlePurgeBehindWindow(it, pendingNode);
    EXPECT_FALSE(pendingNode.empty());

    it = pendingNode.begin();
    uifirstManager_.pendingNodeBehindWindow_[nodeId].isFirst = true;
    uifirstManager_.pendingNodeBehindWindow_[nodeId].curTime = -30;
    uifirstManager_.HandlePurgeBehindWindow(it, pendingNode);
    EXPECT_FALSE(pendingNode.empty());

    it = pendingNode.begin();
    uifirstManager_.pendingNodeBehindWindow_[nodeId].isFirst = true;
    uifirstManager_.pendingNodeBehindWindow_[nodeId].curTime = -29;
    uifirstManager_.HandlePurgeBehindWindow(it, pendingNode);
    EXPECT_FALSE(pendingNode.empty());

    it = pendingNode.begin();
    uifirstManager_.pendingNodeBehindWindow_[nodeId].isFirst = false;
    uifirstManager_.pendingNodeBehindWindow_[nodeId].curTime = -29;
    uifirstManager_.HandlePurgeBehindWindow(it, pendingNode);
    EXPECT_TRUE(pendingNode.empty());
}

/**
 * @tc.name: IsBehindWindowOcclusion001
 * @tc.desc: Test IsBehindWindowOcclusion
 * @tc.type: FUNC
 * @tc.require: issueICIXW6
 */
HWTEST_F(RSUifirstManagerTest, IsBehindWindowOcclusion001, TestSize.Level1)
{
    auto emptyRegion = Occlusion::Region();
    auto visibleRegion = Occlusion::Region({50, 50, 100, 100});
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    auto childSubSurfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);

    surfaceNode->SetGlobalAlpha(1.f);
    surfaceNode->uifirstContentDirty_ = false;
    surfaceNode->SetVisibleRegion(emptyRegion);
    surfaceNode->SetVisibleRegionBehindWindow(emptyRegion);
    surfaceNode->UpdateChildSubSurfaceNodes(childSubSurfaceNode, true);
    EXPECT_FALSE(uifirstManager_.IsBehindWindowOcclusion(surfaceNode));

    surfaceNode->childSubSurfaceNodes_.clear();
    auto appWindow = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(appWindow, nullptr);
    surfaceNode->AddChild(appWindow);
    surfaceNode->GenerateFullChildrenList();
    surfaceNode->UpdateChildSubSurfaceNodes(appWindow, true);
    appWindow->SetVisibleRegion(emptyRegion);
    appWindow->SetVisibleRegionBehindWindow(visibleRegion);
    EXPECT_FALSE(uifirstManager_.IsBehindWindowOcclusion(surfaceNode));

    appWindow->SetVisibleRegion(emptyRegion);
    appWindow->SetVisibleRegionBehindWindow(emptyRegion);
    EXPECT_FALSE(uifirstManager_.IsBehindWindowOcclusion(surfaceNode));

    appWindow->SetVisibleRegion(visibleRegion);
    appWindow->SetVisibleRegionBehindWindow(visibleRegion);
    EXPECT_FALSE(uifirstManager_.IsBehindWindowOcclusion(surfaceNode));

    appWindow->SetVisibleRegion(visibleRegion);
    appWindow->SetVisibleRegionBehindWindow(emptyRegion);
    EXPECT_TRUE(uifirstManager_.IsBehindWindowOcclusion(surfaceNode));

    surfaceNode->SetVisibleRegion(visibleRegion);
    auto surfaceDrawable = surfaceNode->GetRenderDrawable();
    surfaceDrawable->GetSyncDirtyManager()->SetUifirstFrameDirtyRect(RectI{0, 0, 100, 100});
    EXPECT_FALSE(uifirstManager_.IsBehindWindowOcclusion(surfaceNode));
}

/**
 * @tc.name: GetUiFirstMode
 * @tc.desc: Test different uifirst mode
 * @tc.type: FUNC
 * @tc.require: issueIBVHE7
 */
HWTEST_F(RSUifirstManagerTest, GetUiFirstMode, TestSize.Level1)
{
    uifirstManager_.SetUiFirstType(-1);
    EXPECT_EQ(uifirstManager_.GetUiFirstMode(), UiFirstModeType::SINGLE_WINDOW_MODE);

    uifirstManager_.SetUiFirstType((int)UiFirstCcmType::SINGLE);
    EXPECT_EQ(uifirstManager_.GetUiFirstMode(), UiFirstModeType::SINGLE_WINDOW_MODE);

    uifirstManager_.SetUiFirstType((int)UiFirstCcmType::MULTI);
    EXPECT_EQ(uifirstManager_.GetUiFirstMode(), UiFirstModeType::MULTI_WINDOW_MODE);

    uifirstManager_.SetFreeMultiWindowStatus(true);
    uifirstManager_.SetUiFirstType((int)UiFirstCcmType::HYBRID);
    EXPECT_EQ(uifirstManager_.GetUiFirstMode(), UiFirstModeType::MULTI_WINDOW_MODE);

    uifirstManager_.SetFreeMultiWindowStatus(false);
    uifirstManager_.SetUiFirstType((int)UiFirstCcmType::HYBRID);
    EXPECT_EQ(uifirstManager_.GetUiFirstMode(), UiFirstModeType::SINGLE_WINDOW_MODE);
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
    uifirstManager_.uifirstType_ = UiFirstCcmType::MULTI;
    surfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
    surfaceNode->isChildSupportUifirst_ = true;
    surfaceNode->firstLevelNodeId_ = surfaceNode->GetId();
    surfaceNode->forceUIFirst_ = true;
    surfaceNode->hasSharedTransitionNode_ = false;
    surfaceNode->hasFilter_ = false;
    surfaceNode->lastFrameUifirstFlag_ = MultiThreadCacheType::NONE;
    uifirstManager_.rotationChanged_ = false;
    uifirstManager_.UpdateUifirstNodes(*surfaceNode, true);
    EXPECT_TRUE(surfaceNode->GetLastFrameUifirstFlag() == MultiThreadCacheType::NONE);

    surfaceNode->lastFrameUifirstFlag_ = MultiThreadCacheType::NONFOCUS_WINDOW;
    uifirstManager_.UpdateUifirstNodes(*surfaceNode, true);
    EXPECT_TRUE(surfaceNode->GetLastFrameUifirstFlag() == MultiThreadCacheType::NONFOCUS_WINDOW);
    uifirstManager_.uifirstType_ = UiFirstCcmType::SINGLE;
}

/**
 * @tc.name: RefreshUIFirstParam
 * @tc.desc: Test RefreshUIFirstParam
 * @tc.type: FUNC
 * @tc.require: #IBV9YJ
 */
HWTEST_F(RSUifirstManagerTest, RefreshUIFirstParam, TestSize.Level1)
{
    uifirstManager_.RefreshUIFirstParam();
    ASSERT_EQ(uifirstManager_.purgeEnable_, RSSystemParameters::GetUIFirstPurgeEnabled());
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
@tc.name: CheckHwcChildrenType
@tc.desc: Test CheckHwcChildrenType
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
    auto surfaceNode1 = RSTestUtil::CreateSurfaceNode();
    surfaceNode->AddChildHardwareEnabledNode(surfaceNode1);
    ASSERT_EQ(surfaceNode->childHardwareEnabledNodes_.size(), 1);
    enabledType = SurfaceHwcNodeType::DEFAULT_HWC_TYPE;
    uifirstManager_.CheckHwcChildrenType(*surfaceNode, enabledType);

    surfaceNode1->name_ = "hipreview";
    uifirstManager_.CheckHwcChildrenType(*surfaceNode, enabledType);
    ASSERT_EQ(enabledType, SurfaceHwcNodeType::DEFAULT_HWC_VIDEO);

    surfaceNode1->name_ = "RosenWeb";
    uifirstManager_.CheckHwcChildrenType(*surfaceNode, enabledType);
    ASSERT_EQ(enabledType, SurfaceHwcNodeType::DEFAULT_HWC_ROSENWEB);

    enabledType = SurfaceHwcNodeType::DEFAULT_HWC_TYPE;
    surfaceNode->name_ = "hipreview";
    uifirstManager_.CheckHwcChildrenType(*surfaceNode, enabledType);
    ASSERT_EQ(enabledType, SurfaceHwcNodeType::DEFAULT_HWC_VIDEO);

    surfaceNode->name_ = "RosenWeb";
    uifirstManager_.CheckHwcChildrenType(*surfaceNode, enabledType);
    ASSERT_EQ(enabledType, SurfaceHwcNodeType::DEFAULT_HWC_ROSENWEB);

    surfaceNode->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    auto surfaceNdoe2 = RSTestUtil::CreateSurfaceNode();
    surfaceNode->AddChild(surfaceNdoe2);
    surfaceNode->GenerateFullChildrenList();
    uifirstManager_.CheckHwcChildrenType(*surfaceNode, enabledType);
    ASSERT_EQ(enabledType, SurfaceHwcNodeType::DEFAULT_HWC_ROSENWEB);
    surfaceNode->ResetChildHardwareEnabledNodes();
}

/**
@tc.name: ResetWindowCache
@tc.desc: Test ResetWindowCache
@tc.type: FUNC
@tc.require: #IBWRGL
*/
HWTEST_F(RSUifirstManagerTest, ResetWindowCache, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> surfaceNode1;
    ASSERT_EQ(surfaceNode1, nullptr);
    uifirstManager_.ResetWindowCache(surfaceNode1);

    auto surfaceNode2 = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode2, nullptr);
    uifirstManager_.ResetWindowCache(surfaceNode2);
}

/**
 * @tc.name: CheckMatchAndWaitNotify
 * @tc.desc: Test if first level node match
 * @tc.type: FUNC
 * @tc.require: issueIBVHE7
 */
HWTEST_F(RSUifirstManagerTest, CheckMatchAndWaitNotify, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    auto renderParams = surfaceNode->GetRenderParams().get();
    ASSERT_TRUE(RSUiFirstProcessStateCheckerHelper::CheckMatchAndWaitNotify(*renderParams, false));

    renderParams->SetUiFirstRootNode(1);
    renderParams->SetFirstLevelNode(1);
    RSUiFirstProcessStateCheckerHelper::curUifirstRootNodeId_ = 1;
    RSUiFirstProcessStateCheckerHelper::curFirstLevelNodeId_ = 1;
    ASSERT_TRUE(RSUiFirstProcessStateCheckerHelper::CheckMatchAndWaitNotify(*renderParams, true));
}

/**
 * @tc.name: UpdateCompletedSurface
 * @tc.desc: Test UpdateCompletedSurface
 * @tc.type: FUNC
 * @tc.require: issueIBVHE7
 */
HWTEST_F(RSUifirstManagerTest, UpdateCompletedSurface, TestSize.Level1)
{
    uifirstManager_.UpdateCompletedSurface(0);

    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    auto surfaceDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
    uifirstManager_.subthreadProcessingNode_.clear();
    uifirstManager_.subthreadProcessingNode_.insert(std::make_pair(surfaceNode->GetId(), surfaceDrawable));

    surfaceDrawable->GetRsSubThreadCache().cacheSurface_ = nullptr;
    uifirstManager_.UpdateCompletedSurface(surfaceNode->GetId());
    ASSERT_FALSE(surfaceDrawable->GetRsSubThreadCache().isCacheValid_);

    surfaceDrawable->GetRsSubThreadCache().cacheSurface_ = std::make_shared<Drawing::Surface>();
    surfaceDrawable->GetRsSubThreadCache().isCacheValid_ = true;
    uifirstManager_.UpdateCompletedSurface(surfaceNode->GetId());
    ASSERT_TRUE(surfaceDrawable->GetRsSubThreadCache().isCacheCompletedValid_);
}

/**
 * @tc.name: OnProcessEventResponse
 * @tc.desc: Test receive system event
 * @tc.type: FUNC
 * @tc.require: issueIBVHE7
 */
HWTEST_F(RSUifirstManagerTest, OnProcessEventResponse, TestSize.Level1)
{
    int32_t scbPid = 100;
    uifirstManager_.scbPid_ = scbPid;
    DataBaseRs scrollEvent {
        .uniqueId = 1,
        .appPid = scbPid,
        .sceneId = "LAUNCHER_SCROLL"
    };
    uifirstManager_.OnProcessEventResponse(scrollEvent);
    ASSERT_TRUE(uifirstManager_.currentFrameCanSkipFirstWait_);
    uifirstManager_.globalFrameEvent_.clear();

    DataBaseRs minimizedEvent {
        .uniqueId = 10,
        .appPid = scbPid,
        .sceneId = "WINDOW_TITLE_BAR_MINIMIZED"
    };
    uifirstManager_.OnProcessEventResponse(minimizedEvent);
    ASSERT_FALSE(uifirstManager_.currentFrameCanSkipFirstWait_);
    uifirstManager_.globalFrameEvent_.clear();
}

/**
 * @tc.name: PostUifistSubTasks
 * @tc.desc: Test post task to subthread
 * @tc.type: FUNC
 * @tc.require: issueIBVHE7
 */
HWTEST_F(RSUifirstManagerTest, PostUifistSubTasks, TestSize.Level1)
{
    uifirstManager_.sortedSubThreadNodeIds_.clear();
    uifirstManager_.pendingPostNodes_.clear();
    uifirstManager_.pendingPostCardNodes_.clear();
    auto surfaceNode1 = RSTestUtil::CreateSurfaceNode();
    surfaceNode1->isOnTheTree_ = true;
    uifirstManager_.pendingPostNodes_.insert(std::make_pair(surfaceNode1->GetId(), surfaceNode1));
    auto surfaceNode2 = RSTestUtil::CreateSurfaceNode();
    surfaceNode2->isOnTheTree_ = true;
    uifirstManager_.pendingPostNodes_.insert(std::make_pair(surfaceNode2->GetId(), surfaceNode2));
    auto surfaceNode3 = RSTestUtil::CreateSurfaceNode();
    surfaceNode3->isOnTheTree_ = true;
    uifirstManager_.pendingPostNodes_.insert(std::make_pair(surfaceNode3->GetId(), surfaceNode3));
    auto surfaceNode4 = RSTestUtil::CreateSurfaceNode();
    uifirstManager_.pendingPostNodes_.insert(std::make_pair(surfaceNode4->GetId(), surfaceNode4));
    uifirstManager_.PostUifistSubTasks();
    ASSERT_TRUE(uifirstManager_.sortedSubThreadNodeIds_.empty());
}

/**
 * @tc.name: ForceUpdateUifirstNodes001
 * @tc.desc: Test uifirst not enabled
 * @tc.type: FUNC
 * @tc.require: issueIBVHE7
 */
HWTEST_F(RSUifirstManagerTest, ForceUpdateUifirstNodes001, TestSize.Level1)
{
    // uifirst close
    uifirstManager_.isUiFirstOn_ = false;
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_TRUE(uifirstManager_.ForceUpdateUifirstNodes(*surfaceNode));
    ASSERT_EQ(surfaceNode->GetLastFrameUifirstFlag(), MultiThreadCacheType::NONE);

    // node not support
    uifirstManager_.isUiFirstOn_ = true;
    surfaceNode->isUifirstNode_ = false;
    surfaceNode->isChildSupportUifirst_ = true;
    ASSERT_TRUE(uifirstManager_.ForceUpdateUifirstNodes(*surfaceNode));
    ASSERT_EQ(surfaceNode->GetLastFrameUifirstFlag(), MultiThreadCacheType::NONE);

    // force disable uifirst
    uifirstManager_.isUiFirstOn_ = true;
    surfaceNode->isUifirstNode_ = true;
    surfaceNode->isChildSupportUifirst_ = true;
    surfaceNode->SetUIFirstSwitch(RSUIFirstSwitch::FORCE_DISABLE);
    ASSERT_TRUE(uifirstManager_.ForceUpdateUifirstNodes(*surfaceNode));
    ASSERT_EQ(surfaceNode->GetLastFrameUifirstFlag(), MultiThreadCacheType::NONE);

    // protect layer not support
    uifirstManager_.isUiFirstOn_ = true;
    surfaceNode->isUifirstNode_ = true;
    surfaceNode->isChildSupportUifirst_ = true;
    surfaceNode->SetUIFirstSwitch(RSUIFirstSwitch::NONE);
    surfaceNode->GetMultableSpecialLayerMgr().Set(SpecialLayerType::HAS_PROTECTED, true);
    ASSERT_TRUE(uifirstManager_.ForceUpdateUifirstNodes(*surfaceNode));
    ASSERT_EQ(surfaceNode->GetLastFrameUifirstFlag(), MultiThreadCacheType::NONE);
}

/**
 * @tc.name: ForceUpdateUifirstNodes002
 * @tc.desc: Test force enable or disable uifirst
 * @tc.type: FUNC
 * @tc.require: issueIBVHE7
 */
HWTEST_F(RSUifirstManagerTest, ForceUpdateUifirstNodes002, TestSize.Level1)
{
    uifirstManager_.isUiFirstOn_ = true;
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    surfaceNode->isUifirstNode_ = true;
    surfaceNode->isChildSupportUifirst_ = true;
    surfaceNode->SetUIFirstSwitch(RSUIFirstSwitch::NONE);

    surfaceNode->nodeType_ = RSSurfaceNodeType::ABILITY_COMPONENT_NODE;
    ASSERT_FALSE(uifirstManager_.ForceUpdateUifirstNodes(*surfaceNode));

    surfaceNode->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    // force enable
    surfaceNode->MarkUifirstNode(true, true);
    ASSERT_TRUE(uifirstManager_.ForceUpdateUifirstNodes(*surfaceNode));
    ASSERT_EQ(surfaceNode->GetLastFrameUifirstFlag(), MultiThreadCacheType::LEASH_WINDOW);
    surfaceNode->SetLastFrameUifirstFlag(MultiThreadCacheType::NONE);

    // force disable
    surfaceNode->MarkUifirstNode(true, false);
    ASSERT_TRUE(uifirstManager_.ForceUpdateUifirstNodes(*surfaceNode));
    ASSERT_EQ(surfaceNode->GetLastFrameUifirstFlag(), MultiThreadCacheType::NONE);
    surfaceNode->SetLastFrameUifirstFlag(MultiThreadCacheType::NONE);
}

/**
 * @tc.name: ForceUpdateUifirstNodes003
 * @tc.desc: Test force enable uifirst on phone or pc
 * @tc.type: FUNC
 * @tc.require: issueIBVHE7
 */
HWTEST_F(RSUifirstManagerTest, ForceUpdateUifirstNodes003, TestSize.Level1)
{
    uifirstManager_.isUiFirstOn_ = true;
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    surfaceNode->isUifirstNode_ = true;
    surfaceNode->isChildSupportUifirst_ = true;

    surfaceNode->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    surfaceNode->SetUIFirstSwitch(RSUIFirstSwitch::FORCE_ENABLE);

    // force enable on phone
    uifirstManager_.SetUiFirstType((int)UiFirstCcmType::SINGLE);
    ASSERT_TRUE(uifirstManager_.ForceUpdateUifirstNodes(*surfaceNode));
    ASSERT_EQ(surfaceNode->GetLastFrameUifirstFlag(), MultiThreadCacheType::LEASH_WINDOW);
    surfaceNode->SetLastFrameUifirstFlag(MultiThreadCacheType::NONE); // must reset uifirst flag

    // force enable on pc
    uifirstManager_.SetUiFirstType((int)UiFirstCcmType::MULTI);
    ASSERT_TRUE(uifirstManager_.ForceUpdateUifirstNodes(*surfaceNode));
    ASSERT_EQ(surfaceNode->GetLastFrameUifirstFlag(), MultiThreadCacheType::NONFOCUS_WINDOW);
    surfaceNode->SetLastFrameUifirstFlag(MultiThreadCacheType::NONE);
}

/**
 * @tc.name: AddProcessDoneNode
 * @tc.desc: Test AddProcessDoneNode
 * @tc.type: FUNC
 * @tc.require: issueIBVHE7
 */
HWTEST_F(RSUifirstManagerTest, PurgePendingPostNode, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    uifirstManager_.markForceUpdateByUifirst_.push_back(surfaceNode);
    uifirstManager_.pendingForceUpdateNode_.push_back(1);
    uifirstManager_.PurgePendingPostNodes();
    ASSERT_TRUE(uifirstManager_.markForceUpdateByUifirst_.empty());
    uifirstManager_.markForceUpdateByUifirst_.clear();
    uifirstManager_.pendingForceUpdateNode_.clear();
}

/**
 * @tc.name: AddProcessDoneNode
 * @tc.desc: Test AddProcessDoneNode
 * @tc.type: FUNC
 * @tc.require: issueIBVHE7
 */
HWTEST_F(RSUifirstManagerTest, AddProcessDoneNode, TestSize.Level1)
{
    uifirstManager_.subthreadProcessDoneNode_.clear();
    uifirstManager_.AddProcessDoneNode(0);
    ASSERT_TRUE(uifirstManager_.subthreadProcessDoneNode_.empty());

    uifirstManager_.AddProcessDoneNode(1);
    ASSERT_FALSE(uifirstManager_.subthreadProcessDoneNode_.empty());
    uifirstManager_.subthreadProcessDoneNode_.clear();
}

/**
 * @tc.name: AddCapturedNodes
 * @tc.desc: Test AddCapturedNodes
 * @tc.type: FUNC
 * @tc.require: issueIBVHE7
 */
HWTEST_F(RSUifirstManagerTest, AddCapturedNodes, TestSize.Level1)
{
    uifirstManager_.AddCapturedNodes(1);
    ASSERT_FALSE(uifirstManager_.capturedNodes_.empty());
}

/**
 * @tc.name: CheckCurrentFrameHasCardNodeReCreate
 * @tc.desc: Test if card node recreate on single frame
 * @tc.type: FUNC
 * @tc.require: issueIBVHE7
 */
HWTEST_F(RSUifirstManagerTest, CheckCurrentFrameHasCardNodeReCreate, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    surfaceNode->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    uifirstManager_.CheckCurrentFrameHasCardNodeReCreate(*surfaceNode);
    ASSERT_FALSE(uifirstManager_.isCurrentFrameHasCardNodeReCreate_);

    surfaceNode->nodeType_ = RSSurfaceNodeType::ABILITY_COMPONENT_NODE;
    surfaceNode->name_ = "ArkTSCardNode";

    // card off the tree
    surfaceNode->isOnTheTree_ = false;
    uifirstManager_.CheckCurrentFrameHasCardNodeReCreate(*surfaceNode);
    ASSERT_FALSE(uifirstManager_.isCurrentFrameHasCardNodeReCreate_);

    // card on the tree
    surfaceNode->isOnTheTree_ = true;
    uifirstManager_.CheckCurrentFrameHasCardNodeReCreate(*surfaceNode);
    ASSERT_TRUE(uifirstManager_.isCurrentFrameHasCardNodeReCreate_);

    uifirstManager_.ResetCurrentFrameDeletedCardNodes();
    ASSERT_FALSE(uifirstManager_.isCurrentFrameHasCardNodeReCreate_);
    ASSERT_TRUE(uifirstManager_.currentFrameDeletedCardNodes_.empty());
}

/**
 * @tc.name: ReadUIFirstCcmParam
 * @tc.desc: Test ccm config
 * @tc.type: FUNC
 * @tc.require: issueIBVHE7
 */
HWTEST_F(RSUifirstManagerTest, ReadUIFirstCcmParam, TestSize.Level1)
{
    uifirstManager_.ReadUIFirstCcmParam();
    ASSERT_TRUE(uifirstManager_.isUiFirstOn_);
    ASSERT_TRUE(uifirstManager_.isCardUiFirstOn_);
}

/**
 * @tc.name: IsSubHighPriorityType
 * @tc.desc: Test if surface has high priority
 * @tc.type: FUNC
 * @tc.require: issueIBVHE7
 */
HWTEST_F(RSUifirstManagerTest, IsSubHighPriorityType, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    surfaceNode->name_ = "hipreview";
    ASSERT_TRUE(uifirstManager_.IsSubHighPriorityType(*surfaceNode));

    surfaceNode->name_ = "video";
    ASSERT_FALSE(uifirstManager_.IsSubHighPriorityType(*surfaceNode));
}

/**
 * @tc.name: MarkSubHighPriorityType
 * @tc.desc: Test different priority strategy
 * @tc.type: FUNC
 * @tc.require: issueIBVHE7
 */
HWTEST_F(RSUifirstManagerTest, MarkSubHighPriorityType, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceNode->stagingRenderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);

    uifirstManager_.SetUiFirstType((int)UiFirstCcmType::SINGLE);
    uifirstManager_.MarkSubHighPriorityType(*surfaceNode);
    ASSERT_FALSE(surfaceParams->GetPreSubHighPriorityType());

    uifirstManager_.SetUiFirstType((int)UiFirstCcmType::MULTI);
    uifirstManager_.MarkSubHighPriorityType(*surfaceNode);
    ASSERT_FALSE(surfaceParams->GetPreSubHighPriorityType());
}

/**
 * @tc.name: GetCacheSurfaceProcessedStatusTest
 * @tc.desc: Test GetCacheSurfaceProcessedStatus
 * @tc.type: FUNC
 * @tc.require: issueIC3DK9
 */
HWTEST_F(RSUifirstManagerTest, GetCacheSurfaceProcessedStatusTest, TestSize.Level1)
{
    NodeId nodeId = 100;
    RSSurfaceRenderParams surfaceParams(nodeId);
    ASSERT_EQ(uifirstManager_.GetCacheSurfaceProcessedStatus(surfaceParams), CacheProcessStatus::UNKNOWN);

    surfaceParams.SetUiFirstRootNode(nodeId);
    surfaceParams.SetFirstLevelNode(nodeId);
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(nodeId);
    auto surfaceDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
    surfaceDrawable->GetRsSubThreadCache().SetCacheSurfaceProcessedStatus(CacheProcessStatus::DOING);
    ASSERT_EQ(uifirstManager_.GetCacheSurfaceProcessedStatus(surfaceParams), CacheProcessStatus::DOING);

    surfaceParams.SetUiFirstRootNode(0);
    surfaceParams.SetFirstLevelNode(nodeId);
    surfaceDrawable->GetRsSubThreadCache().SetCacheSurfaceProcessedStatus(CacheProcessStatus::WAITING);
    ASSERT_EQ(uifirstManager_.GetCacheSurfaceProcessedStatus(surfaceParams), CacheProcessStatus::WAITING);
}

/**
 * @tc.name: CheckHasTransAndFilter001
 * @tc.desc: Test strategy with trans and blur scenes by uifirst.
 * @tc.type: FUNC
 * @tc.require: issueIC4F7H
 */
HWTEST_F(RSUifirstManagerTest, CheckHasTransAndFilter001, TestSize.Level1)
{
    // no children node
    auto parentNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(parentNode, nullptr);
    parentNode->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    bool result = uifirstManager_.CheckHasTransAndFilter(*parentNode);
    ASSERT_EQ(result, false);

    // canvas children node
    parentNode->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
    NodeId nodeId = 100;
    auto childNode1 = std::make_shared<RSCanvasRenderNode>(nodeId);
    parentNode->AddChild(childNode1);
    parentNode->GenerateFullChildrenList();
    result = uifirstManager_.CheckHasTransAndFilter(*parentNode);
    ASSERT_EQ(result, false);

    // surface children node, but surface is not app window
    auto childNode2 = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(childNode2, nullptr);
    parentNode->AddChild(childNode2);
    parentNode->GenerateFullChildrenList();
    result = uifirstManager_.CheckHasTransAndFilter(*parentNode);
    ASSERT_EQ(result, false);

    // children type is app window
    auto childNode3 = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(childNode3, nullptr);
    childNode3->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    parentNode->AddChild(childNode3);
    parentNode->GenerateFullChildrenList();
    result = uifirstManager_.CheckHasTransAndFilter(*parentNode);
    ASSERT_EQ(result, false);
}

/**
 * @tc.name: CheckHasTransAndFilter002
 * @tc.desc: Test strategy with trans and blur scenes by uifirst.
 * @tc.type: FUNC
 * @tc.require: issueIC4F7H
 */
HWTEST_F(RSUifirstManagerTest, CheckHasTransAndFilter002, TestSize.Level1)
{
    // leash window --> app window
    auto parentNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(parentNode, nullptr);
    parentNode->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
    auto childNode1 = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(childNode1, nullptr);
    childNode1->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    childNode1->absDrawRect_ = {0, 0, 100, 100};
    parentNode->AddChild(childNode1);
    parentNode->GenerateFullChildrenList();
    auto result = uifirstManager_.CheckHasTransAndFilter(*parentNode);
    ASSERT_EQ(result, false);

    // leash window --> (app window, default surface) --> rootNode
    auto childNode2 = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(childNode2, nullptr);
    childNode2->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    childNode2->absDrawRect_ = {0, 0, 50, 50};
    parentNode->AddChild(childNode2);
    parentNode->GenerateFullChildrenList();
    NodeId nodeId = 200;
    auto rootChildNode = std::make_shared<RSRootRenderNode>(nodeId);
    childNode1->AddChild(rootChildNode);
    result = uifirstManager_.CheckHasTransAndFilter(*parentNode);
    ASSERT_EQ(result, false);

    // leash window --> (app window, default surface) --> rootNode --> canvasNode
    nodeId = 300;
    auto canvasChildNode = std::make_shared<RSCanvasRenderNode>(nodeId);
    rootChildNode->AddChild(canvasChildNode);
    result = uifirstManager_.CheckHasTransAndFilter(*parentNode);
    ASSERT_EQ(result, false);

    // has filter and transparent
    parentNode->childHasVisibleFilter_ = true;
    childNode2->absDrawRect_ = {0, 0, 200, 200};
    childNode2->abilityBgAlpha_ = 0;
    childNode2->globalAlpha_ = 0;
    result = uifirstManager_.CheckHasTransAndFilter(*parentNode);
    ASSERT_EQ(result, true);
}

/**
 * @tc.name: IsArkTsCardCache
 * @tc.desc: Test if ArkTsCard node should enable uifirst
 * @tc.type: FUNC
 * @tc.require: issueICCSTG
 */
HWTEST_F(RSUifirstManagerTest, IsArkTsCardCache, TestSize.Level1)
{
    RSSurfaceRenderNode node(100);
    RSDisplayNodeConfig displayConfig;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(10, displayConfig);
    node.SetAncestorDisplayNode(displayNode);

    // leash window doesn't
    uifirstManager_.SetUiFirstType(static_cast<int>(UiFirstCcmType::SINGLE));
    node.nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    node.name_ = "ArkTSCardNode";
    EXPECT_FALSE(RSUifirstManager::IsArkTsCardCache(node, true));

    // uifirst type doesn't match
    uifirstManager_.SetUiFirstType(static_cast<int>(UiFirstCcmType::MULTI));
    node.nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    node.name_ = "ArkTSCardNode";
    EXPECT_FALSE(RSUifirstManager::IsArkTsCardCache(node, true));

    // node name doesn't match
    uifirstManager_.SetUiFirstType(static_cast<int>(UiFirstCcmType::SINGLE));
    node.nodeType_ = RSSurfaceNodeType::ABILITY_COMPONENT_NODE;
    node.name_ = "TestNode";
    EXPECT_FALSE(RSUifirstManager::IsArkTsCardCache(node, true));

    uifirstManager_.entryViewNodeId_ = 1;
    uifirstManager_.negativeScreenNodeId_ = 1;
    node.instanceRootNodeId_ = 1;
    node.shouldPaint_ = true;

    uifirstManager_.SetUiFirstType(static_cast<int>(UiFirstCcmType::SINGLE));
    node.nodeType_ = RSSurfaceNodeType::ABILITY_COMPONENT_NODE;
    node.name_ = "ArkTSCardNode";
    EXPECT_TRUE(RSUifirstManager::IsArkTsCardCache(node, true));
}

/**
 * @tc.name: ProcessDoneNodeInnerTest
 * @tc.desc: Test ProcessDoneNodeInner
 * @tc.type: FUNC
 * @tc.require: issueIC4F7H
 */
HWTEST_F(RSUifirstManagerTest, ProcessDoneNodeInnerTest, TestSize.Level1)
{
    uifirstManager_.subthreadProcessDoneNode_.clear();
    uifirstManager_.ProcessDoneNodeInner();
    ASSERT_EQ(uifirstManager_.subthreadProcessDoneNode_.size(), 0);
    auto surfaceNode1 = RSTestUtil::CreateSurfaceNode();
    uifirstManager_.subthreadProcessDoneNode_.push_back(surfaceNode1->GetId());
    uifirstManager_.subthreadProcessDoneNode_.push_back(++RSTestUtil::id);
    auto tmp = uifirstManager_.subthreadProcessDoneNode_;
    uifirstManager_.ProcessDoneNodeInner();
    ASSERT_EQ(uifirstManager_.pendingForceUpdateNode_.size(), 0);

    uifirstManager_.subthreadProcessDoneNode_ = tmp;
    auto drawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(surfaceNode1->renderDrawable_);
    drawable->GetRsSubThreadCache().SetCacheSurfaceNeedUpdated(true);
    uifirstManager_.ProcessDoneNodeInner();
    ASSERT_EQ(uifirstManager_.pendingForceUpdateNode_.size(), 0);

    uifirstManager_.subthreadProcessDoneNode_ = tmp;
    drawable->GetRsSubThreadCache().cacheSurface_ = std::make_shared<Drawing::Surface>();
    uifirstManager_.ProcessDoneNodeInner();
    ASSERT_EQ(uifirstManager_.pendingForceUpdateNode_.size(), 1);

    uifirstManager_.subthreadProcessDoneNode_.clear();
    uifirstManager_.pendingForceUpdateNode_.clear();
}

/**
 * @tc.name: ProcessSkippedNodeTest
 * @tc.desc: Test ProcessSkippedNode
 * @tc.type: FUNC
 * @tc.require: issueIC4F7H
 */
HWTEST_F(RSUifirstManagerTest, ProcessSkippedNodeTest, TestSize.Level1)
{
    uifirstManager_.subthreadProcessSkippedNode_.clear();
    uifirstManager_.ProcessSkippedNode();
    ASSERT_EQ(uifirstManager_.subthreadProcessSkippedNode_.size(), 0);

    auto surfaceNode1 = RSTestUtil::CreateSurfaceNode();
    uifirstManager_.subthreadProcessSkippedNode_.insert(surfaceNode1->GetId());
    mainThread_->context_->nodeMap.RegisterRenderNode(surfaceNode1);
    auto surfaceNode2 = RSTestUtil::CreateSurfaceNode();
    surfaceNode2->isOnTheTree_ = true;
    surfaceNode2->lastFrameUifirstFlag_ = MultiThreadCacheType::ARKTS_CARD;
    uifirstManager_.subthreadProcessSkippedNode_.insert(surfaceNode2->GetId());
    mainThread_->context_->nodeMap.RegisterRenderNode(surfaceNode2);
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(++RSTestUtil::id);
    uifirstManager_.subthreadProcessSkippedNode_.insert(canvasNode->GetId());
    mainThread_->context_->nodeMap.RegisterRenderNode(canvasNode);
    auto tmp = uifirstManager_.subthreadProcessSkippedNode_;
    uifirstManager_.ProcessSkippedNode();
    ASSERT_EQ(uifirstManager_.pendingPostCardNodes_.size(), 1);

    auto surfaceNode3 = RSTestUtil::CreateSurfaceNode();
    surfaceNode2->isOnTheTree_ = true;
    surfaceNode2->lastFrameUifirstFlag_ = MultiThreadCacheType::LEASH_WINDOW;
    tmp.insert(surfaceNode3->GetId());
    uifirstManager_.subthreadProcessSkippedNode_ = tmp;
    mainThread_->context_->nodeMap.RegisterRenderNode(surfaceNode3);
    uifirstManager_.ProcessSkippedNode();
    ASSERT_EQ(uifirstManager_.pendingPostNodes_.size(), 1);
    uifirstManager_.pendingPostCardNodes_.clear();
    uifirstManager_.pendingPostNodes_.clear();
    mainThread_->context_->nodeMap.UnRegisterUnTreeNode(surfaceNode1->GetId());
    mainThread_->context_->nodeMap.UnRegisterUnTreeNode(surfaceNode2->GetId());
    mainThread_->context_->nodeMap.UnRegisterUnTreeNode(surfaceNode3->GetId());
    mainThread_->context_->nodeMap.UnRegisterUnTreeNode(canvasNode->GetId());
    uifirstManager_.subthreadProcessSkippedNode_.clear();
}
}