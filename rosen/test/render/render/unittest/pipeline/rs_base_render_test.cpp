/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "params/rs_render_params.h"
#include "pipeline/rs_base_render_node.h"
#include "render_thread/rs_render_thread_visitor.h"
#include "platform/common/rs_log.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
    const RectI DEFAULT_RECT = {0, 0, 100, 100};
} // namespace

class RSBaseRenderTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline NodeId id;
    static inline std::weak_ptr<RSContext> context = {};
};

void RSBaseRenderTest::SetUpTestCase() {}
void RSBaseRenderTest::TearDownTestCase() {}
void RSBaseRenderTest::SetUp() {}
void RSBaseRenderTest::TearDown() {}

/**
 * @tc.name: AddChild001
 * @tc.desc: test results of AddChild
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSBaseRenderTest, AddChild001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. AddChild
     */
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    auto childone = std::make_shared<RSBaseRenderNode>(id + 1, context);
    auto childtwo = std::make_shared<RSBaseRenderNode>(id + 2, context);
    int index = -1;
    node->SetIsOnTheTree(true);
    node->AddChild(node, index);
    ASSERT_EQ(node->GetChildrenCount(), 0);

    node->AddChild(childone, index);
    ASSERT_EQ(node->GetChildrenCount(), 1);
    ASSERT_TRUE(childone->IsOnTheTree());

    index = 0;
    node->AddChild(childtwo, index);
    ASSERT_EQ(node->GetChildrenCount(), 2);
    ASSERT_TRUE(childtwo->IsOnTheTree());

    node->AddChild(childtwo, index);
    ASSERT_EQ(node->GetChildrenCount(), 2);
}

/**
 * @tc.name: RemoveChild001
 * @tc.desc: test results of RemoveChild
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSBaseRenderTest, RemoveChild001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. AddChild
     */
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    auto childone = std::make_shared<RSBaseRenderNode>(id + 1, context);
    int index = -1;
    node->AddChild(childone, index);
    ASSERT_EQ(node->GetChildrenCount(), 1);

    /**
     * @tc.steps: step2. RemoveChild
     */
    bool skipTransition = false;
    node->RemoveChild(childone, skipTransition);
    ASSERT_EQ(node->GetChildrenCount(), 0);
}

/**
 * @tc.name: ClearChildren001
 * @tc.desc: test results of ClearChildren
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSBaseRenderTest, ClearChildren001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. AddChild
     */
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    auto childone = std::make_shared<RSBaseRenderNode>(id + 1, context);
    int index = -1;
    node->AddChild(childone, index);
    ASSERT_EQ(node->GetChildrenCount(), 1);

    /**
     * @tc.steps: step2. ClearChildren
     */
    node->ClearChildren();
    ASSERT_EQ(node->GetChildrenCount(), 0);
}

/**
 * @tc.name: MoveChild001
 * @tc.desc: test results of MoveChild
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderTest, MoveChild001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. AddChild
     */
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    auto childone = std::make_shared<RSBaseRenderNode>(id + 1, context);
    int index = -1;
    node->AddChild(childone, index);
    ASSERT_EQ(node->GetChildrenCount(), 1);

    /**
     * @tc.steps: step2. ClearChildren
     */
    node->MoveChild(childone, index);
    ASSERT_EQ(node->GetChildrenCount(), 1);
}

/**
 * @tc.name: SetIsOnTheTree001
 * @tc.desc: test results of SetIsOnTheTree
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderTest, SetIsOnTheTree001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. AddChild
     */
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    auto childone = nullptr;
    int index = -1;
    node->SetIsOnTheTree(true);
    node->AddChild(childone, index);
    ASSERT_EQ(node->GetChildrenCount(), 0);

    /**
     * @tc.steps: step2. ClearChildren
     */
    node->ClearChildren();
    ASSERT_EQ(node->GetChildrenCount(), 0);
}

/**
 * @tc.name: AddCrossParentChild001
 * @tc.desc: test results of AddCrossParentChild
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderTest, AddCrossParentChild001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. AddChild
     */
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    auto childone = std::make_shared<RSBaseRenderNode>(id + 1, context);
    int index = -1;
    node->AddCrossParentChild(childone, index);
    ASSERT_EQ(node->GetChildrenCount(), 1);

    /**
     * @tc.steps: step2. ClearChildren
     */
    node->ClearChildren();
    ASSERT_EQ(node->GetChildrenCount(), 0);
}

/**
 * @tc.name: RemoveCrossParentChild001
 * @tc.desc: test results of RemoveCrossParentChild
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderTest, RemoveCrossParentChild001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. AddChild
     */
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    auto childone = std::make_shared<RSBaseRenderNode>(id + 1, context);
    auto newParent = std::make_shared<RSBaseRenderNode>(id + 2, context);
    int index = -1;
    node->AddCrossParentChild(childone, index);
    ASSERT_EQ(node->GetChildrenCount(), 1);

    /**
     * @tc.steps: step2. ClearChildren
     */
    node->RemoveCrossParentChild(childone, newParent);
}

/**
 * @tc.name: SetIsOnTheTree002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderTest, SetIsOnTheTree002, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    auto childone = nullptr;
    int index = -1;
    node->SetIsOnTheTree(true);
    node->AddChild(childone, index);
    bool skipTransition = false;
    node->RemoveChild(childone, skipTransition);
    EXPECT_EQ(node->GetChildrenCount(), 0);
}

/**
 * @tc.name: AddCrossParentChildTest001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderTest, AddCrossParentChildTest001, TestSize.Level1)
{
    int32_t index = 1;
    int32_t index_ = 0;
    std::shared_ptr<RSBaseRenderNode> child = nullptr;
    std::shared_ptr<RSBaseRenderNode> child_;
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    node->AddCrossParentChild(child, index);
    node->AddCrossParentChild(child_, index_);
}

/**
 * @tc.name: RemoveCrossParentChildTest001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderTest, RemoveCrossParentChildTest001, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    std::shared_ptr<RSBaseRenderNode> child = nullptr;
    std::weak_ptr<RSBaseRenderNode> newParent;
    node->RemoveCrossParentChild(child, newParent);
    EXPECT_NE(node, nullptr);
}

/**
 * @tc.name: RemoveFromTreeTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderTest, RemoveFromTreeTest, TestSize.Level1)
{
    bool skipTransition = false;
    bool skipTransition_ = true;
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    EXPECT_NE(node, nullptr);
    node->RemoveFromTree(skipTransition);
    node->RemoveFromTree(skipTransition_);
}

/**
 * @tc.name: PrepareTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderTest, PrepareTest, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    std::shared_ptr<RSNodeVisitor> visitor = nullptr;
    node->Prepare(visitor);
}

/**
 * @tc.name: RemoveModifier
 * @tc.desc: test results of RemoveModifier
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderTest, RemoveModifier, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    Drawing::Matrix matrix;
    PropertyId id = 1;
    std::shared_ptr<RSRenderProperty<Drawing::Matrix>> property =
        std::make_shared<RSRenderProperty<Drawing::Matrix>>(matrix, id);
    std::shared_ptr<RSGeometryTransRenderModifier> modifierCast =
        std::make_shared<RSGeometryTransRenderModifier>(property);
    std::shared_ptr<RSRenderModifier> modifier = modifierCast;
    node->AddGeometryModifier(modifier);
    node->RemoveModifier(id);
    ASSERT_EQ(node->modifiers_.find(id), node->modifiers_.end());
}

/**
 * @tc.name: UpdateDrawableVec
 * @tc.desc: test results of UpdateDrawableVec
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderTest, UpdateDrawableVec, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    node->UpdateDrawableVec();
    ASSERT_TRUE(true);
}

/**
 * @tc.name: UpdateDrawableVecInternal
 * @tc.desc: test results of UpdateDrawableVecInternal
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderTest, UpdateDrawableVecInternal, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    std::unordered_set<RSPropertyDrawableSlot> dirtySlots;
    node->UpdateDrawableVecInternal(dirtySlots);

    node->drawableVecStatus_ = 1;
    dirtySlots.insert(RSPropertyDrawableSlot::BOUNDS_MATRIX);
    RSPropertyDrawable::InitializeSaveRestore(*node->renderContent_, node->renderContent_->propertyDrawablesVec_);
    node->UpdateDrawableVecInternal(dirtySlots);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: UpdateEffectRegion
 * @tc.desc: test results of UpdateEffectRegion
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderTest, UpdateEffectRegion, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    std::optional<Drawing::RectI> region;
    bool isForced = true;
    node->UpdateEffectRegion(region, isForced);

    region.emplace(Drawing::RectI(1, 1, 1, 1));
    isForced = false;
    node->UpdateEffectRegion(region, isForced);

    isForced = true;
    node->UpdateEffectRegion(region, isForced);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: GetModifier
 * @tc.desc: test results of GetModifier
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderTest, GetModifier, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    Drawing::Matrix matrix;
    PropertyId id = 1;
    std::shared_ptr<RSRenderProperty<Drawing::Matrix>> property =
        std::make_shared<RSRenderProperty<Drawing::Matrix>>(matrix, id);
    std::shared_ptr<RSGeometryTransRenderModifier> modifierCast =
        std::make_shared<RSGeometryTransRenderModifier>(property);
    std::shared_ptr<RSRenderModifier> modifier = modifierCast;
    node->GetModifier(modifier->GetPropertyId());

    modifierCast->drawStyle_ = RSModifierType::BOUNDS;
    node->AddGeometryModifier(modifier);
    ASSERT_NE(node->GetModifier(modifier->GetPropertyId()), nullptr);
}

/**
 * @tc.name: FilterModifiersByPid
 * @tc.desc: test results of FilterModifiersByPid
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderTest, FilterModifiersByPid, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    pid_t pid = 1;
    node->FilterModifiersByPid(pid);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: UpdateShouldPaint
 * @tc.desc: test results of UpdateShouldPaint
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderTest, UpdateShouldPaint, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    node->UpdateShouldPaint();
    ASSERT_TRUE(node->shouldPaint_);
}

/**
 * @tc.name: SetSharedTransitionParam
 * @tc.desc: test results of SetSharedTransitionParam
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderTest, SetSharedTransitionParam, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    std::shared_ptr<RSRenderNode> inNode = std::make_shared<RSBaseRenderNode>(id + 1, context);
    std::shared_ptr<RSRenderNode> outNode = std::make_shared<RSBaseRenderNode>(id + 2, context);
    std::shared_ptr<SharedTransitionParam> sharedTransitionParam;
    node->SetSharedTransitionParam(sharedTransitionParam);

    sharedTransitionParam = std::make_shared<SharedTransitionParam>(inNode, outNode);
    node->SetSharedTransitionParam(sharedTransitionParam);

    node->SetSharedTransitionParam(sharedTransitionParam);
    ASSERT_TRUE(node->sharedTransitionParam_);
}

/**
 * @tc.name: SetGlobalAlpha
 * @tc.desc: test results of SetGlobalAlpha
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderTest, SetGlobalAlpha, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    node->InitRenderParams();
    float alpha = 1.0f;
    node->SetGlobalAlpha(alpha);

    alpha = 0.7f;
    node->SetGlobalAlpha(alpha);
    ASSERT_EQ(node->globalAlpha_, alpha);
}

/**
 * @tc.name: NeedInitCacheSurface
 * @tc.desc: test results of NeedInitCacheSurface
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderTest, NeedInitCacheSurface, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    EXPECT_EQ(node->NeedInitCacheSurface(), true);

    node->cacheSurface_ = std::make_shared<Drawing::Surface>();
    ASSERT_EQ(node->NeedInitCacheSurface(), true);
}

/**
 * @tc.name: NeedInitCacheCompletedSurface
 * @tc.desc: test results of NeedInitCacheCompletedSurface
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderTest, NeedInitCacheCompletedSurface, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    EXPECT_EQ(node->NeedInitCacheCompletedSurface(), true);

    node->cacheCompletedSurface_ = std::make_shared<Drawing::Surface>();
    ASSERT_EQ(node->NeedInitCacheCompletedSurface(), true);
}

/**
 * @tc.name: GetOptionalBufferSize
 * @tc.desc: test results of GetOptionalBufferSize
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderTest, GetOptionalBufferSize, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    Drawing::Matrix matrix;
    PropertyId id = 1;
    std::shared_ptr<RSRenderProperty<Drawing::Matrix>> property =
        std::make_shared<RSRenderProperty<Drawing::Matrix>>(matrix, id);
    std::shared_ptr<RSRenderModifier> frameModifier = std::make_shared<RSGeometryTransRenderModifier>(property);
    node->frameModifier_ = frameModifier;
    node->GetOptionalBufferSize();

    std::shared_ptr<RSRenderModifier> boundsModifier = std::make_shared<RSGeometryTransRenderModifier>(property);
    node->boundsModifier_ = boundsModifier;
    node->GetOptionalBufferSize();
    ASSERT_TRUE(true);
}

/**
 * @tc.name: DrawCacheSurface
 * @tc.desc: test results of DrawCacheSurface
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderTest, DrawCacheSurface, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    uint32_t threadIndex = 1;
    bool isUIFirst = true;
    node->DrawCacheSurface(paintFilterCanvas, threadIndex, isUIFirst);

    node->boundsHeight_ = 1.0f;
    node->DrawCacheSurface(paintFilterCanvas, threadIndex, isUIFirst);

    node->boundsWidth_ = 1.0f;
    node->DrawCacheSurface(paintFilterCanvas, threadIndex, isUIFirst);

    isUIFirst = false;
    node->DrawCacheSurface(paintFilterCanvas, threadIndex, isUIFirst);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: GetCompletedImage
 * @tc.desc: test results of GetCompletedImage
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderTest, GetCompletedImage, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    uint32_t threadIndex = 1;
    bool isUIFirst = false;
    EXPECT_EQ(node->GetCompletedImage(paintFilterCanvas, threadIndex, isUIFirst), nullptr);

    node->cacheCompletedSurface_ = std::make_shared<Drawing::Surface>();
    EXPECT_EQ(node->GetCompletedImage(paintFilterCanvas, threadIndex, isUIFirst), nullptr);
}

/**
 * @tc.name: UpdateBackendTexture
 * @tc.desc: test results of UpdateBackendTexture
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderTest, UpdateBackendTexture, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    node->UpdateBackendTexture();

    node->cacheSurface_ = std::make_shared<Drawing::Surface>();
    node->UpdateBackendTexture();
    ASSERT_TRUE(true);
}

/**
 * @tc.name: GetCompletedCacheSurface
 * @tc.desc: test results of GetCompletedCacheSurface
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderTest, GetCompletedCacheSurface, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    uint32_t threadIndex = 1;
    bool needCheckThread = false;
    bool releaseAfterGet = false;
    ASSERT_EQ(node->GetCompletedCacheSurface(threadIndex, needCheckThread, releaseAfterGet), nullptr);

    needCheckThread = true;
    ASSERT_EQ(node->GetCompletedCacheSurface(threadIndex, needCheckThread, releaseAfterGet), nullptr);

    releaseAfterGet = true;
    ASSERT_EQ(node->GetCompletedCacheSurface(threadIndex, needCheckThread, releaseAfterGet), nullptr);
}

/**
 * @tc.name: ClearCacheSurfaceInThread
 * @tc.desc: test results of ClearCacheSurfaceInThread
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderTest, ClearCacheSurfaceInThread, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    node->ClearCacheSurfaceInThread();

    auto clearCacheSurfaceLambda = [](std::shared_ptr<Drawing::Surface>&& surface1,
                                       std::shared_ptr<Drawing::Surface>&& surface2, uint32_t param1,
                                       uint32_t param2) {};
    RSRenderNode::ClearCacheSurfaceFunc func = clearCacheSurfaceLambda;
    node->clearCacheSurfaceFunc_ = func;
    node->ClearCacheSurfaceInThread();
    ASSERT_TRUE(true);
}

/**
 * @tc.name: GetCacheSurface
 * @tc.desc: test results of GetCacheSurface
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderTest, GetCacheSurface, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    uint32_t threadIndex = 1;
    bool needCheckThread = false;
    bool releaseAfterGet = false;
    ASSERT_EQ(node->GetCacheSurface(threadIndex, needCheckThread, releaseAfterGet), nullptr);

    needCheckThread = true;
    ASSERT_EQ(node->GetCacheSurface(threadIndex, needCheckThread, releaseAfterGet), nullptr);

    releaseAfterGet = true;
    ASSERT_EQ(node->GetCacheSurface(threadIndex, needCheckThread, releaseAfterGet), nullptr);
}

/**
 * @tc.name: CheckGroupableAnimation
 * @tc.desc: test results of CheckGroupableAnimation
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderTest, CheckGroupableAnimation, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    PropertyId id = 1;
    bool isAnimAdd = false;
    node->CheckGroupableAnimation(id, isAnimAdd);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: MarkNodeGroup
 * @tc.desc: test results of MarkNodeGroup
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderTest, MarkNodeGroup, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id + 1, context);

    RSRenderNode::NodeGroupType type = RSRenderNode::NodeGroupType::GROUP_TYPE_BUTT;
    bool isNodeGroup = false;
    bool includeProperty = true;
    node->InitRenderParams();
    node->MarkNodeGroup(type, isNodeGroup, includeProperty);

    type = RSRenderNode::NodeGroupType::GROUPED_BY_USER;
    node->MarkNodeGroup(type, isNodeGroup, includeProperty);

    isNodeGroup = true;
    type = RSRenderNode::NodeGroupType::GROUPED_BY_UI;
    node->MarkNodeGroup(type, isNodeGroup, includeProperty);
    ASSERT_EQ(node->nodeGroupIncludeProperty_, includeProperty);
}

/**
 * @tc.name: CheckDrawingCacheType
 * @tc.desc: test results of CheckDrawingCacheType
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderTest, CheckDrawingCacheType, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id + 1, context);
    node->CheckDrawingCacheType();
    ASSERT_EQ(node->drawingCacheType_, RSDrawingCacheType::DISABLED_CACHE);

    node->nodeGroupType_ = RSRenderNode::NodeGroupType::GROUPED_BY_USER;
    node->CheckDrawingCacheType();
    ASSERT_EQ(node->drawingCacheType_, RSDrawingCacheType::FORCED_CACHE);

    node->nodeGroupType_ = RSRenderNode::NodeGroupType::GROUPED_BY_ANIM;
    node->CheckDrawingCacheType();
    ASSERT_EQ(node->drawingCacheType_, RSDrawingCacheType::TARGETED_CACHE);
}

/**
 * @tc.name: GetFilterRectsInCache
 * @tc.desc: test results of GetFilterRectsInCache
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderTest, GetFilterRectsInCache, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    std::unordered_map<NodeId, std::unordered_set<NodeId>> allRects;
    node->GetFilterRectsInCache(allRects);

    std::unordered_set<NodeId> curRects = { 1, 2, 3 };
    node->curCacheFilterRects_ = curRects;
    node->GetFilterRectsInCache(allRects);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: GetFilterRect
 * @tc.desc: test results of GetFilterRect
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderTest, GetFilterRect, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    node->GetFilterRect();
    ASSERT_TRUE(true);
}

/**
 * @tc.name: OnTreeStateChanged
 * @tc.desc: test results of OnTreeStateChanged
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderTest, OnTreeStateChanged, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    node->OnTreeStateChanged();

    node->isOnTheTree_ = true;
    node->OnTreeStateChanged();
    ASSERT_TRUE(true);
}

/**
 * @tc.name: HasDisappearingTransition
 * @tc.desc: test results of HasDisappearingTransition
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderTest, HasDisappearingTransition, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    bool recursive = false;
    ASSERT_FALSE(node->HasDisappearingTransition(recursive));

    node->isOnTheTree_ = true;
    node->disappearingTransitionCount_ = 1;
    ASSERT_TRUE(node->HasDisappearingTransition(recursive));

    node->disappearingTransitionCount_ = 0;
    recursive = true;
    ASSERT_FALSE(node->HasDisappearingTransition(recursive));
}

/**
 * @tc.name: GenerateFullChildrenList
 * @tc.desc: test results of GenerateFullChildrenList
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderTest, GenerateFullChildrenList, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    node->GenerateFullChildrenList();
    {
        auto node = std::make_shared<RSBaseRenderNode>(id, context);
        auto childone = std::make_shared<RSBaseRenderNode>(id + 1, context);
        int index = 0;
        node->AddChild(node, index);
        index = 1;
        node->AddChild(childone, index);
        node->GenerateFullChildrenList();
    }
    ASSERT_TRUE(true);
}

/**
 * @tc.name: HasChildrenOutOfRect
 * @tc.desc: test results of HasChildrenOutOfRect
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderTest, HasChildrenOutOfRect, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    ASSERT_FALSE(node->HasChildrenOutOfRect());
}

/**
 * @tc.name: GetInstanceRootNode
 * @tc.desc: test results of GetInstanceRootNode
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderTest, GetInstanceRootNode, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    node->GetInstanceRootNode();
    ASSERT_TRUE(true);
}

/**
 * @tc.name: GetFirstLevelNode
 * @tc.desc: test results of GetFirstLevelNode
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderTest, GetFirstLevelNode, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    ASSERT_EQ(node->GetFirstLevelNode(), nullptr);
}

/**
 * @tc.name: SetStaticCached
 * @tc.desc: test results of SetStaticCached
 * @tc.type: FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSBaseRenderTest, SetStaticCached, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    EXPECT_NE(node, nullptr);
    node->stagingRenderParams_ = std::make_unique<RSRenderParams>(0);
    EXPECT_NE(node->stagingRenderParams_, nullptr);
    bool isStaticCached = true;
    node->SetStaticCached(isStaticCached);
    ASSERT_TRUE(node->isStaticCached_);

    isStaticCached = false;
    node->SetStaticCached(isStaticCached);
    ASSERT_FALSE(node->isStaticCached_);
}

/**
 * @tc.name: UpdateSubSurfaceCnt
 * @tc.desc: test results of UpdateSubSurfaceCnt
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderTest, UpdateSubSurfaceCnt, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    auto preParent = std::make_shared<RSBaseRenderNode>(id + 1, context);
    node->UpdateSubSurfaceCnt(node, preParent);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: GetNodeGroupType
 * @tc.desc: test results of GetNodeGroupType
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderTest, GetNodeGroupType, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    ASSERT_EQ(node->GetNodeGroupType(), RSRenderNode::NodeGroupType::NONE);
}

/**
 * @tc.name: IsPureContainerTest
 * @tc.desc: test results of IsPureContainer
 * @tc.type:FUNC
 * @tc.require: issueI9KBCZ
 */
HWTEST_F(RSBaseRenderTest, IsPureContainer, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    ASSERT_TRUE(node->IsPureContainer());
}

/**
 * @tc.name: IsContentNodeTest
 * @tc.desc: test results of IsContentNode
 * @tc.type:FUNC
 * @tc.require: issueI9KBCZ
 */
HWTEST_F(RSBaseRenderTest, IsContentNodeTest, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    ASSERT_TRUE(node->IsContentNode());
}

/**
 * @tc.name: SetContainBootAnimation
 * @tc.desc: test results of SetContainBootAnimation
 * @tc.type:FUNC
 * @tc.require: issueI9KBCZ
 */
HWTEST_F(RSBaseRenderTest, SetContainBootAnimation, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    bool isContainBootAnimation = true;
    node->SetContainBootAnimation(isContainBootAnimation);
    ASSERT_TRUE(node->isContainBootAnimation_);
}

/**
 * @tc.name: UpdateChildrenRect
 * @tc.desc: test results of UpdateChildrenRect
 * @tc.type:FUNC
 * @tc.require: issueI9KBCZ
 */
HWTEST_F(RSBaseRenderTest, UpdateChildrenRect, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    RectI rect(1, 1, 1, 1);
    node->UpdateChildrenRect(rect);
    EXPECT_TRUE(!node->GetChildrenRect().IsEmpty());

    node->childrenRect_.Clear();
    node->UpdateChildrenRect(rect);
    EXPECT_TRUE(!node->GetChildrenRect().IsEmpty());

    rect.Clear();
    node->UpdateChildrenRect(rect);
    node->childrenRect_.Clear();
    ASSERT_TRUE(node->GetChildrenRect().IsEmpty());
}

/**
 * @tc.name: SetParent
 * @tc.desc: test results of SetParent
 * @tc.type:FUNC
 * @tc.require: issueI9KBCZ
 */
HWTEST_F(RSBaseRenderTest, SetParent, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    auto parent = std::make_shared<RSBaseRenderNode>(id + 1, context);
    node->SetParent(parent);
    ASSERT_NE(node->parent_.lock(), nullptr);
}

/**
 * @tc.name: ResetParent
 * @tc.desc: test results of ResetParent
 * @tc.type:FUNC
 * @tc.require: issueI9KBCZ
 */
HWTEST_F(RSBaseRenderTest, ResetParent, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    node->ResetParent();
    ASSERT_EQ(node->parent_.lock(), nullptr);
}

/**
 * @tc.name: FallbackAnimationsToRoot
 * @tc.desc: test results of FallbackAnimationsToRoot
 * @tc.type:FUNC
 * @tc.require: issueI9KBCZ
 */
HWTEST_F(RSBaseRenderTest, FallbackAnimationsToRoot, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    node->FallbackAnimationsToRoot();
    ASSERT_TRUE(true);
}

/**
 * @tc.name: ActivateDisplaySync
 * @tc.desc: test results of ActivateDisplaySync
 * @tc.type:FUNC
 * @tc.require: issueI9KBCZ
 */
HWTEST_F(RSBaseRenderTest, ActivateDisplaySync, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    node->ActivateDisplaySync();
    node->ActivateDisplaySync();
    ASSERT_NE(node->displaySync_, nullptr);
}

/**
 * @tc.name: UpdateDisplaySyncRange
 * @tc.desc: test results of UpdateDisplaySyncRange
 * @tc.type:FUNC
 * @tc.require: issueI9KBCZ
 */
HWTEST_F(RSBaseRenderTest, UpdateDisplaySyncRange, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    node->UpdateDisplaySyncRange();

    node->displaySync_ = std::make_shared<RSRenderDisplaySync>(1);
    node->UpdateDisplaySyncRange();
    ASSERT_NE(node->displaySync_, nullptr);
}

/**
 * @tc.name: Animate
 * @tc.desc: test results of Animate
 * @tc.type:FUNC
 * @tc.require: issueI9KBCZ
 */
HWTEST_F(RSBaseRenderTest, Animate, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    int64_t timestamp = 4;
    int64_t period = 2;
    bool isDisplaySyncEnabled = true;
    node->Animate(timestamp, period, isDisplaySyncEnabled);

    node->displaySync_ = std::make_shared<RSRenderDisplaySync>(1);
    node->Animate(timestamp, period, isDisplaySyncEnabled);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: Update
 * @tc.desc: test results of Update
 * @tc.type:FUNC
 * @tc.require: issueI9KBCZ
 */
HWTEST_F(RSBaseRenderTest, Update, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    RSDirtyRegionManager dirtyManager;
    std::shared_ptr<RSRenderNode> parent = std::make_shared<RSRenderNode>(id + 1);
    bool parentDirty = true;
    std::optional<RectI> clipRect;
    node->Update(dirtyManager, parent, parentDirty, clipRect);

    node->shouldPaint_ = false;
    node->isLastVisible_ = true;
    ASSERT_TRUE(node->Update(dirtyManager, parent, parentDirty, clipRect));
}

/**
 * @tc.name: UpdateBufferDirtyRegion
 * @tc.desc: test results of UpdateBufferDirtyRegion
 * @tc.type:FUNC
 * @tc.require: issueI9KBCZ
 */
HWTEST_F(RSBaseRenderTest, UpdateBufferDirtyRegion, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    RectI dirtyRect(1, 1, 1, 1);
    RectI drawRegion;
    node->UpdateBufferDirtyRegion(dirtyRect, drawRegion);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: UpdateDirtyRegion
 * @tc.desc: test results of UpdateDirtyRegion
 * @tc.type:FUNC
 * @tc.require: issueI9KBCZ
 */
HWTEST_F(RSBaseRenderNodeTest, UpdateDirtyRegion, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    RSDirtyRegionManager dirtyManager;
    bool geoDirty = false;
    std::optional<RectI> clipRect;
    node->dirtyStatus_ = RSRenderNode::NodeDirty::DIRTY;
    node->UpdateDirtyRegion(dirtyManager, geoDirty, clipRect);

    geoDirty = true;
    node->UpdateDirtyRegion(dirtyManager, geoDirty, clipRect);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: IsSelfDrawingNode
 * @tc.desc: test results of IsSelfDrawingNode
 * @tc.type:FUNC
 * @tc.require: issueI9KBCZ
 */
HWTEST_F(RSBaseRenderNodeTest, IsSelfDrawingNode, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    ASSERT_FALSE(node->IsSelfDrawingNode());
}

/**
 * @tc.name: IsDirty
 * @tc.desc: test results of IsDirty
 * @tc.type:FUNC
 * @tc.require: issueI9KBCZ
 */
HWTEST_F(RSBaseRenderNodeTest, IsDirty, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    ASSERT_FALSE(node->IsDirty());
}

/**
 * @tc.name: IsContentDirty
 * @tc.desc: test results of IsContentDirty
 * @tc.type:FUNC
 * @tc.require: issueI9KBCZ
 */
HWTEST_F(RSBaseRenderNodeTest, IsContentDirty, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    ASSERT_FALSE(node->IsContentDirty());
}

/**
 * @tc.name: UpdateRenderStatus
 * @tc.desc: test results of UpdateRenderStatus
 * @tc.type:FUNC
 * @tc.require: issueI9KBCZ
 */
HWTEST_F(RSBaseRenderNodeTest, UpdateRenderStatus, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    RectI dirtyRegion(1, 1, 1, 1);
    bool isPartialRenderEnabled = false;
    node->UpdateRenderStatus(dirtyRegion, isPartialRenderEnabled);

    isPartialRenderEnabled = true;
    node->UpdateRenderStatus(dirtyRegion, isPartialRenderEnabled);
    ASSERT_TRUE(node->isRenderUpdateIgnored_);
}

/**
 * @tc.name: UpdateParentChildrenRect
 * @tc.desc: test results of UpdateParentChildrenRect
 * @tc.type:FUNC
 * @tc.require: issueI9KBCZ
 */
HWTEST_F(RSBaseRenderNodeTest, UpdateParentChildrenRect, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    std::shared_ptr<RSRenderNode> parentNode;
    node->UpdateParentChildrenRect(parentNode);

    parentNode = std::make_shared<RSRenderNode>(id + 1);
    node->UpdateParentChildrenRect(parentNode);

    node->shouldPaint_ = false;
    node->UpdateParentChildrenRect(parentNode);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: RenderTraceDebug
 * @tc.desc: test results of RenderTraceDebug
 * @tc.type:FUNC
 * @tc.require: issueI9KBCZ
 */
HWTEST_F(RSBaseRenderNodeTest, RenderTraceDebug, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    node->RenderTraceDebug();
    ASSERT_TRUE(true);
}

/**
 * @tc.name: AddModifier
 * @tc.desc: test results of AddModifier
 * @tc.type:FUNC
 * @tc.require: issueI9KBCZ
 */
HWTEST_F(RSBaseRenderNodeTest, AddModifier, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    std::shared_ptr<RSRenderModifier> modifier;
    bool isSingleFrameComposer = true;
    node->AddModifier(modifier, isSingleFrameComposer);

    Drawing::Matrix matrix;
    PropertyId id = 1;
    std::shared_ptr<RSRenderProperty<Drawing::Matrix>> property =
        std::make_shared<RSRenderProperty<Drawing::Matrix>>(matrix, id);
    std::shared_ptr<RSGeometryTransRenderModifier> modifierCast =
        std::make_shared<RSGeometryTransRenderModifier>(property);
    std::shared_ptr<RSRenderModifier> modifierTwo = modifierCast;
    node->AddModifier(modifierTwo, isSingleFrameComposer);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: AddGeometryModifier
 * @tc.desc: test results of AddGeometryModifier
 * @tc.type:FUNC
 * @tc.require: issueI9KBCZ
 */
HWTEST_F(RSBaseRenderNodeTest, AddGeometryModifier, TestSize.Level1)
{
    {
        auto node = std::make_shared<RSBaseRenderNode>(id, context);
        Drawing::Matrix matrix;
        PropertyId id = 1;
        std::shared_ptr<RSRenderProperty<Drawing::Matrix>> property =
            std::make_shared<RSRenderProperty<Drawing::Matrix>>(matrix, id);
        std::shared_ptr<RSGeometryTransRenderModifier> modifierCast =
            std::make_shared<RSGeometryTransRenderModifier>(property);
        std::shared_ptr<RSRenderModifier> modifier = modifierCast;
        node->AddGeometryModifier(modifier);

        modifierCast->drawStyle_ = RSModifierType::BOUNDS;
        node->AddGeometryModifier(modifier);

        node->boundsModifier_ = modifier;
        node->AddGeometryModifier(modifier);
        ASSERT_NE(node->boundsModifier_, nullptr);
    }

    {
        auto node = std::make_shared<RSBaseRenderNode>(id, context);
        Drawing::Matrix matrix;
        PropertyId id = 1;
        std::shared_ptr<RSRenderProperty<Drawing::Matrix>> property =
            std::make_shared<RSRenderProperty<Drawing::Matrix>>(matrix, id);
        std::shared_ptr<RSGeometryTransRenderModifier> modifierCast =
            std::make_shared<RSGeometryTransRenderModifier>(property);
        modifierCast->drawStyle_ = RSModifierType::FRAME;
        std::shared_ptr<RSRenderModifier> modifier = modifierCast;
        node->AddGeometryModifier(modifier);

        node->frameModifier_ = modifier;
        node->AddGeometryModifier(modifier);
        ASSERT_NE(node->frameModifier_, nullptr);
    }
}

/**
 * @tc.name: OpincSetInAppState
 * @tc.desc: test OpincSetInAppStateStart and OpincSetInAppStateEnd
 * @tc.type: FUNC
 * @tc.require: issueI9SPVO
 */
HWTEST_F(RSBaseRenderNodeTest, OpincSetInAppState, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    bool unchangeMarkInApp = false;
    node->OpincSetInAppStateStart(unchangeMarkInApp);
    ASSERT_TRUE(unchangeMarkInApp);
    node->OpincSetInAppStateEnd(unchangeMarkInApp);
    ASSERT_FALSE(unchangeMarkInApp);
}

/**
 * @tc.name: OpincQuickMarkStableNode01
 * @tc.desc: test OpincQuickMarkStableNode when content dirty
 * @tc.type: FUNC
 * @tc.require: issueI9SPVO
 */
HWTEST_F(RSBaseRenderNodeTest, OpincQuickMarkStableNode01, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    node->stagingRenderParams_ = std::make_unique<RSRenderParams>(id);
    node->SetContentDirty();
    bool unchangeMarkInApp = true;
    bool unchangeMarkEnable = true;
    node->OpincQuickMarkStableNode(unchangeMarkInApp, unchangeMarkEnable, false);
    ASSERT_EQ(node->nodeCacheState_, NodeCacheState::STATE_CHANGE);
}

/**
 * @tc.name: OpincQuickMarkStableNode02
 * @tc.desc: test OpincQuickMarkStableNode when subtree dirty
 * @tc.type: FUNC
 * @tc.require: issueI9SPVO
 */
HWTEST_F(RSBaseRenderNodeTest, OpincQuickMarkStableNode02, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    node->stagingRenderParams_ = std::make_unique<RSRenderParams>(id);
    node->SetSubTreeDirty(true);
    bool unchangeMarkInApp = true;
    bool unchangeMarkEnable = true;
    node->OpincQuickMarkStableNode(unchangeMarkInApp, unchangeMarkEnable, false);
    ASSERT_EQ(node->nodeCacheState_, NodeCacheState::STATE_CHANGE);
}
} // namespace OHOS::Rosen
