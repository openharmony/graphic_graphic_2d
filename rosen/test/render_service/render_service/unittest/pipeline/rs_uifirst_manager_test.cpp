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
}

void RSUifirstManagerTest::TearDownTestCase() {}
void RSUifirstManagerTest::SetUp() {}
void RSUifirstManagerTest::TearDown() {}

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
 * @tc.desc: Test MergeOldDirty, preSurfaceCacheContentStatic_ is false, no dirty region
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSUifirstManagerTest, MergeOldDirty001, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceNode->GetStagingRenderParams().get());
    ASSERT_NE(surfaceParams, nullptr);
    surfaceParams->preSurfaceCacheContentStatic_ = false;
    surfaceNode->oldDirty_ = RSUifirstManagerTest::DEFAULT_RECT;
    uifirstManager_.MergeOldDirty(*surfaceNode);
    if (surfaceNode->GetDirtyManager()) {
        ASSERT_TRUE(surfaceNode->GetDirtyManager()->GetCurrentFrameDirtyRegion().IsEmpty());
    }
}

/**
 * @tc.name: MergeOldDirty002
 * @tc.desc: Test MergeOldDirty, preSurfaceCacheContentStatic_ is true, dirty region is not empty
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSUifirstManagerTest, MergeOldDirty002, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceNode->GetStagingRenderParams().get());
    ASSERT_NE(surfaceParams, nullptr);
    surfaceParams->preSurfaceCacheContentStatic_ = true;
    surfaceNode->oldDirty_ = RSUifirstManagerTest::DEFAULT_RECT;
    uifirstManager_.MergeOldDirty(*surfaceNode);
    if (surfaceNode->GetDirtyManager()) {
        ASSERT_FALSE(surfaceNode->GetDirtyManager()->GetCurrentFrameDirtyRegion().IsEmpty());
    }
}

/**
 * @tc.name: MergeOldDirty003
 * @tc.desc: Test MergeOldDirty, merge children dirty region
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSUifirstManagerTest, MergeOldDirty003, TestSize.Level1)
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
    childNode->oldDirty_= RSUifirstManagerTest::DEFAULT_RECT;
    surfaceNode->AddChild(childNode);
    surfaceNode->GenerateFullChildrenList();
    uifirstManager_.MergeOldDirty(*surfaceNode);
    if (childNode->GetDirtyManager()) {
        ASSERT_FALSE(childNode->GetDirtyManager()->GetCurrentFrameDirtyRegion().IsEmpty());
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
    auto drawable = RSSurfaceRenderNodeDrawable::OnGenerate(surfaceNode);
    auto surfaceDrawable = static_cast<RSSurfaceRenderNodeDrawable*>(drawable);
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
    auto drawable = RSSurfaceRenderNodeDrawable::OnGenerate(surfaceNode);
    auto surfaceDrawable = static_cast<RSSurfaceRenderNodeDrawable*>(drawable);
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
    auto drawable = RSSurfaceRenderNodeDrawable::OnGenerate(childNode);
    auto surfaceDrawable = static_cast<RSSurfaceRenderNodeDrawable*>(drawable);
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
 * @tc.require: #I9NVOG
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

    ASSERT_TRUE(uifirstManager_.CheckVisibleDirtyRegionIsEmpty(surfaceNode));
}

/**
 * @tc.name: CheckVisibleDirtyRegionIsEmpty
 * @tc.desc: Test CheckVisibleDirtyRegionIsEmpty, child has visible/invisible dirty region
 * @tc.type: FUNC
 * @tc.require: #I9UNQP
 */
HWTEST_F(RSUifirstManagerTest, CheckVisibleDirtyRegionIsEmpty003, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    auto childNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(childNode, nullptr);
    surfaceNode->AddChild(childNode);
    surfaceNode->GenerateFullChildrenList();
    // set visible region in both node and param
    childNode->SetVisibleRegion(DEFAULT_VISIBLE_REGION);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(childNode->GetRenderParams().get());
    ASSERT_NE(surfaceParams, nullptr);
    surfaceParams->SetVisibleRegion(DEFAULT_VISIBLE_REGION);

    ASSERT_NE(childNode->GetDirtyManager(), nullptr);
    childNode->GetDirtyManager()->SetCurrentFrameDirtyRect(VISIBLE_DIRTY_REGION);
    ASSERT_FALSE(uifirstManager_.CheckVisibleDirtyRegionIsEmpty(surfaceNode));
    childNode->GetDirtyManager()->SetCurrentFrameDirtyRect(INVISIBLE_DIRTY_REGION);
    ASSERT_TRUE(uifirstManager_.CheckVisibleDirtyRegionIsEmpty(surfaceNode));
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
    surfaceNode1->SetIsOnTheTree(false);
    surfaceNode1->SetIsNodeToBeCaptured(true);
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
 * @tc.desc: Test UpdateUifirstNodes, with different nodes
 * @tc.type: FUNC
 * @tc.require: #I9UNQP
 */
HWTEST_F(RSUifirstManagerTest, UpdateUifirstNodes, TestSize.Level1)
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
}