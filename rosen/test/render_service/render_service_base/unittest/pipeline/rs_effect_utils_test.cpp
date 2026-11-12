/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "gmock/gmock.h"
#include <gtest/gtest.h>

#include "common/rs_common_def.h"
#include "common/rs_obj_abs_geometry.h"
#include "drawable/rs_property_drawable.h"
#include "feature/filter_cache_manager/rs_filter_cache_manager.h"
#include "feature/filter_cache_manager/rs_cache_update_manager.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_effect_render_node.h"
#include "pipeline/rs_effect_utils.h"
#include "pipeline/rs_surface_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSEffectUtilsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline NodeId id;
    static inline std::weak_ptr<RSContext> context = {};
};

void RSEffectUtilsTest::SetUpTestCase()
{
}
void RSEffectUtilsTest::TearDownTestCase()
{
}
void RSEffectUtilsTest::SetUp() {}
void RSEffectUtilsTest::TearDown() {}

/**
 * @tc.name: AccumulateFilterRenderContext001
 * @tc.desc: test AccumulateFilterRenderContext with null geometry
 * @tc.type: FUNC
 * @tc.require: issue24382
 */
HWTEST_F(RSEffectUtilsTest, AccumulateFilterRenderContext001, TestSize.Level1)
{
    RSRenderNode node(id, context);
    RSRenderNode rootNode(id + 1, context);
    FilterRenderContext filterContext;
    auto result = RSEffectUtils::AccumulateFilterRenderContext(node, rootNode, filterContext);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: AccumulateFilterRenderContext002
 * @tc.desc: test AccumulateFilterRenderContext with valid tree structure
 * @tc.type: FUNC
 * @tc.require: issue24382
 */
HWTEST_F(RSEffectUtilsTest, AccumulateFilterRenderContext002, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(id, context);
    node->GetMutableRenderProperties().boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    auto rootNode = std::make_shared<RSRenderNode>(id + 1, context);
    rootNode->GetMutableRenderProperties().boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    auto parentNode = std::make_shared<RSRenderNode>(id + 2, context);
    parentNode->GetMutableRenderProperties().boundsGeo_ = std::make_shared<RSObjAbsGeometry>();

    parentNode->AddChild(node);
    rootNode->AddChild(parentNode);

    Drawing::Matrix identityMatrix;
    auto& nodeMatrix = node->GetMutableRenderProperties().boundsGeo_->matrix_;
    auto& parentNodeMatrix = parentNode->GetMutableRenderProperties().boundsGeo_->matrix_;
    auto& rootNodeAbsMatrix = rootNode->GetMutableRenderProperties().boundsGeo_->absMatrix_;
    nodeMatrix = identityMatrix;
    parentNodeMatrix = identityMatrix;
    rootNodeAbsMatrix = identityMatrix;
    rootNodeAbsMatrix->SetScale(10.0f, 10.0f);

    FilterRenderContext filterContext;
    filterContext.absMatrix = *rootNodeAbsMatrix;

    auto result = RSEffectUtils::AccumulateFilterRenderContext(*node, *rootNode, filterContext);
    EXPECT_TRUE(result);
    EXPECT_NE(filterContext.absMatrix.Get(0), identityMatrix.Get(0));
}

/**
 * @tc.name: AccumulateFilterRenderContext003
 * @tc.desc: test AccumulateFilterRenderContext with canvas node as offscreen parent
 * @tc.type: FUNC
 * @tc.require: issue24382
 */
HWTEST_F(RSEffectUtilsTest, AccumulateFilterRenderContext003, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(id, context);
    node->GetMutableRenderProperties().boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    auto rootNode = std::make_shared<RSRenderNode>(id + 1, context);
    rootNode->GetMutableRenderProperties().boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    auto canvasParent = std::make_shared<RSCanvasRenderNode>(id + 2, context);
    canvasParent->GetMutableRenderProperties().boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    canvasParent->SetDrawingCacheType(RSDrawingCacheType::FOREGROUND_FILTER_CACHE);

    canvasParent->AddChild(node);
    rootNode->AddChild(canvasParent);

    Drawing::Matrix identityMatrix;
    auto& nodeMatrix = node->GetMutableRenderProperties().boundsGeo_->matrix_;
    auto& canvasParentMatrix = canvasParent->GetMutableRenderProperties().boundsGeo_->matrix_;
    auto& rootNodeAbsMatrix = rootNode->GetMutableRenderProperties().boundsGeo_->absMatrix_;
    nodeMatrix = identityMatrix;
    canvasParentMatrix = identityMatrix;
    rootNodeAbsMatrix = identityMatrix;
    rootNodeAbsMatrix->SetScale(10.0f, 10.0f);

    FilterRenderContext filterContext;
    filterContext.absMatrix = rootNodeAbsMatrix.value();

    auto result = RSEffectUtils::AccumulateFilterRenderContext(*node, *rootNode, filterContext);
    EXPECT_TRUE(result);
    EXPECT_EQ(filterContext.offscreenNodeId, canvasParent->GetId());
}

/**
 * @tc.name: UpdateFilterCacheWithBelowDirtyAndPendingPurge001
 * @tc.desc: test UpdateFilterCacheWithBelowDirtyAndPendingPurge with MaterialFilter
 * @tc.type: FUNC
 * @tc.require: issue25152
 */
HWTEST_F(RSEffectUtilsTest, UpdateFilterCacheWithBelowDirtyAndPendingPurge001, TestSize.Level2)
{
    auto node = std::make_shared<RSCanvasRenderNode>(id, context);
    node->GetMutableRenderProperties().GetEffectProperties().GetFilterEffect().materialFilter_
        = std::make_shared<RSFilter>();

    RSDirtyRegionManager dirtyManager;
    dirtyManager.MergeDirtyRect(RectI(0, 0, 100, 100));

    RSEffectUtils::UpdateFilterCacheWithBelowDirtyAndPendingPurge(*node, dirtyManager);
    EXPECT_FALSE(dirtyManager.GetCurrentFrameDirtyRegion().IsEmpty());
}

/**
 * @tc.name: UpdateFilterCacheWithBelowDirtyAndPendingPurge002
 * @tc.desc: test UpdateFilterCacheWithBelowDirtyAndPendingPurge with BackgroundFilter
 * @tc.type: FUNC
 * @tc.require: issue25152
 */
HWTEST_F(RSEffectUtilsTest, UpdateFilterCacheWithBelowDirtyAndPendingPurge002, TestSize.Level2)
{
    auto node = std::make_shared<RSCanvasRenderNode>(id, context);
    node->GetMutableRenderProperties().GetEffectProperties().SetBackgroundFilter(std::make_shared<RSFilter>());

    RSDirtyRegionManager dirtyManager;
    dirtyManager.MergeDirtyRect(RectI(0, 0, 100, 100));

    RSEffectUtils::UpdateFilterCacheWithBelowDirtyAndPendingPurge(*node, dirtyManager);
    EXPECT_FALSE(dirtyManager.GetCurrentFrameDirtyRegion().IsEmpty());
}

/**
 * @tc.name: HasValidEffectNodeTest001
 * @tc.desc: test HasValidEffectNode with null node, loop body skipped and return false
 * @tc.type: FUNC
 * @tc.require: issue33156
 */
HWTEST_F(RSEffectUtilsTest, HasValidEffectNodeTest001, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nullNode = nullptr;
    EXPECT_FALSE(RSEffectUtils::HasValidEffectNode(nullNode));
}

/**
 * @tc.name: HasValidEffectNodeTest002
 * @tc.desc: test HasValidEffectNode with EFFECT_NODE type and all validity conditions met
 * @tc.type: FUNC
 * @tc.require: issue33156
 */
HWTEST_F(RSEffectUtilsTest, HasValidEffectNodeTest002, TestSize.Level1)
{
    auto effectNode = std::make_shared<RSEffectRenderNode>(id, context);
    effectNode->GetMutableRenderProperties().GetEffectProperties().SetBackgroundFilter(std::make_shared<RSFilter>());
    effectNode->SetGlobalAlpha(1.0f);
    effectNode->shouldPaint_ = true;
    effectNode->hasEffectChildrenWithoutEmptyRect_ = true;
    EXPECT_TRUE(RSEffectUtils::HasValidEffectNode(effectNode));
}

/**
 * @tc.name: HasValidEffectNodeTest003
 * @tc.desc: test HasValidEffectNode with SURFACE_NODE type, immediate return false
 * @tc.type: FUNC
 * @tc.require: issue33156
 */
HWTEST_F(RSEffectUtilsTest, HasValidEffectNodeTest003, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id);
    EXPECT_FALSE(RSEffectUtils::HasValidEffectNode(surfaceNode));
}

/**
 * @tc.name: HasValidEffectNodeTest004
 * @tc.desc: test HasValidEffectNode with CANVAS_NODE type, walk up parent chain without EFFECT_NODE
 * @tc.type: FUNC
 * @tc.require: issue33156
 */
HWTEST_F(RSEffectUtilsTest, HasValidEffectNodeTest004, TestSize.Level1)
{
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(id, context);
    auto parentNode = std::make_shared<RSRenderNode>(id + 1, context);
    parentNode->AddChild(canvasNode);
    EXPECT_FALSE(RSEffectUtils::HasValidEffectNode(canvasNode));
}

/**
 * @tc.name: HasValidFilterOrEffectTest001
 * @tc.desc: test HasValidFilterOrEffect with null BackgroundFilter and no other effect set
 * @tc.type: FUNC
 * @tc.require: issue33156
 */
HWTEST_F(RSEffectUtilsTest, HasValidFilterOrEffectTest001, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(id, context);
    // BackgroundFilter/MaterialFilter/UseEffect all default to null/false; BgBrightness invalid.
    EXPECT_FALSE(RSEffectUtils::HasValidFilterOrEffect(*node));
}

/**
 * @tc.name: HasValidFilterOrEffectTest002
 * @tc.desc: test HasValidFilterOrEffect with only MaterialFilter set, short-circuit at second condition
 * @tc.type: FUNC
 * @tc.require: issue33156
 */
HWTEST_F(RSEffectUtilsTest, HasValidFilterOrEffectTest002, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(id, context);
    node->GetMutableRenderProperties().GetEffectProperties().SetMaterialFilter(std::make_shared<RSFilter>());
    EXPECT_TRUE(RSEffectUtils::HasValidFilterOrEffect(*node));
}

/**
 * @tc.name: HasValidFilterOrEffectTest003
 * @tc.desc: test HasValidFilterOrEffect with only BackgroundFilter set, short-circuit at first condition
 * @tc.type: FUNC
 * @tc.require: issue33156
 */
HWTEST_F(RSEffectUtilsTest, HasValidFilterOrEffectTest003, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(id, context);
    node->GetMutableRenderProperties().GetEffectProperties().SetBackgroundFilter(std::make_shared<RSFilter>());
    EXPECT_TRUE(RSEffectUtils::HasValidFilterOrEffect(*node));
}

/**
 * @tc.name: HasValidEffectTest001
 * @tc.desc: test HasValidEffect: null input, surface-node termination, UseEffect/EFFECT_NODE walk
 * @tc.type: FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSEffectUtilsTest, HasValidEffectTest001, TestSize.Level1)
{
    auto rootSurfaceNode = std::make_shared<RSSurfaceRenderNode>(id);
    auto effectNode = std::make_shared<RSEffectRenderNode>(id + 1);
    auto testNode = std::make_shared<RSEffectRenderNode>(id + 2);
    auto renderNode = std::make_shared<RSRenderNode>(id + 3);

    testNode->GetMutableRenderProperties().SetUseEffect(true);
    testNode->SetGlobalAlpha(1.0f);
    testNode->UpdateChildrenOutOfRectFlag(true);

    rootSurfaceNode->AddChild(effectNode);
    effectNode->AddChild(testNode);
    testNode->AddChild(renderNode);

    EXPECT_FALSE(RSEffectUtils::HasValidEffect(nullptr));
    EXPECT_FALSE(RSEffectUtils::HasValidEffect(renderNode.get()));
    RSEffectUtils::HasValidEffect(rootSurfaceNode.get());
    EXPECT_FALSE(RSEffectUtils::HasValidEffect(testNode.get()));
    testNode->UpdateChildrenOutOfRectFlag(false);
    RSEffectUtils::HasValidEffect(testNode.get());
    testNode->SetGlobalAlpha(0.5f);
    EXPECT_FALSE(RSEffectUtils::HasValidEffect(testNode.get()));
    testNode->SetGlobalAlpha(1.0f);
    rootSurfaceNode->RemoveChild(effectNode);
    EXPECT_FALSE(RSEffectUtils::HasValidEffect(rootSurfaceNode.get()));
}

/**
 * @tc.name: UpdateNodeFilterRateToDrawableTest001
 * @tc.desc: test UpdateNodeFilterApsRateToDrawable and UpdateNodeFilterAppRateToDrawable on node without filter
 * @tc.type: FUNC
 */
HWTEST_F(RSEffectUtilsTest, UpdateNodeFilterRateToDrawableTest001, TestSize.Level1)
{
    RSRenderNode node(id, context);
    auto materialDrawable = std::make_shared<DrawableV2::RSFilterDrawable>();
    materialDrawable->stagingCacheManager_ = std::make_unique<RSFilterCacheManager>();
    node.drawableVec_ = std::make_unique<RSDrawable::Vec>();
    (*node.drawableVec_)[static_cast<int8_t>(RSDrawableSlot::MATERIAL_FILTER)] = materialDrawable;

    // node without any filter properties: rates must not be written to the drawable
    RSEffectUtils::UpdateNodeFilterApsRateToDrawable(node, 30);
    RSEffectUtils::UpdateNodeFilterAppRateToDrawable(node, 60);
    EXPECT_EQ(materialDrawable->stagingCacheManager_->GetRSCacheUpdateManager()->GetNodeFilterApsRate(),
        INVALID_FILTER_RATE);
    EXPECT_EQ(materialDrawable->stagingCacheManager_->GetRSCacheUpdateManager()->GetNodeFilterAppRate(),
        INVALID_FILTER_RATE);
}

/**
 * @tc.name: UpdateNodeFilterRateToDrawableTest002
 * @tc.desc: test UpdateNodeFilterApsRateToDrawable and UpdateNodeFilterAppRateToDrawable with filter properties but
 *           no filter drawable
 * @tc.type: FUNC
 */
HWTEST_F(RSEffectUtilsTest, UpdateNodeFilterRateToDrawableTest002, TestSize.Level1)
{
    auto node = std::make_shared<RSCanvasRenderNode>(id, context);
    node->GetMutableRenderProperties().GetEffectProperties().GetFilterEffect().materialFilter_ =
        std::make_shared<RSFilter>();
    node->GetMutableRenderProperties().GetEffectProperties().SetBackgroundFilter(std::make_shared<RSFilter>());
    node->GetMutableRenderProperties().GetEffectProperties().SetCompositingFilter(std::make_shared<RSFilter>());
    node->drawableVec_ = std::make_unique<RSDrawable::Vec>();

    // filter properties are set but no filter drawable exists: nothing is created or updated
    RSEffectUtils::UpdateNodeFilterApsRateToDrawable(*node, 30);
    RSEffectUtils::UpdateNodeFilterAppRateToDrawable(*node, 60);
    EXPECT_EQ((*node->drawableVec_)[static_cast<int8_t>(RSDrawableSlot::MATERIAL_FILTER)], nullptr);
    EXPECT_EQ((*node->drawableVec_)[static_cast<int8_t>(RSDrawableSlot::BACKGROUND_FILTER)], nullptr);
    EXPECT_EQ((*node->drawableVec_)[static_cast<int8_t>(RSDrawableSlot::COMPOSITING_FILTER)], nullptr);
}

/**
 * @tc.name: UpdateNodeFilterRateToDrawableTest003
 * @tc.desc: test UpdateNodeFilterApsRateToDrawable and UpdateNodeFilterAppRateToDrawable write rates into filter
 *           drawables through InvokeFilterDrawable and GetFilterDrawable
 * @tc.type: FUNC
 */
HWTEST_F(RSEffectUtilsTest, UpdateNodeFilterRateToDrawableTest003, TestSize.Level1)
{
    auto node = std::make_shared<RSCanvasRenderNode>(id, context);
    node->GetMutableRenderProperties().GetEffectProperties().GetFilterEffect().materialFilter_ =
        std::make_shared<RSFilter>();
    node->GetMutableRenderProperties().GetEffectProperties().SetCompositingFilter(std::make_shared<RSFilter>());

    auto materialDrawable = std::make_shared<DrawableV2::RSFilterDrawable>();
    materialDrawable->stagingCacheManager_ = std::make_unique<RSFilterCacheManager>();
    auto compositingDrawable = std::make_shared<DrawableV2::RSFilterDrawable>();
    compositingDrawable->stagingCacheManager_ = std::make_unique<RSFilterCacheManager>();
    node->drawableVec_ = std::make_unique<RSDrawable::Vec>();
    (*node->drawableVec_)[static_cast<int8_t>(RSDrawableSlot::MATERIAL_FILTER)] = materialDrawable;
    (*node->drawableVec_)[static_cast<int8_t>(RSDrawableSlot::COMPOSITING_FILTER)] = compositingDrawable;

    RSEffectUtils::UpdateNodeFilterApsRateToDrawable(*node, 30);
    EXPECT_EQ(materialDrawable->stagingCacheManager_->GetRSCacheUpdateManager()->GetNodeFilterApsRate(), 30);
    EXPECT_EQ(compositingDrawable->stagingCacheManager_->GetRSCacheUpdateManager()->GetNodeFilterApsRate(), 30);

    RSEffectUtils::UpdateNodeFilterAppRateToDrawable(*node, 60);
    EXPECT_EQ(materialDrawable->stagingCacheManager_->GetRSCacheUpdateManager()->GetNodeFilterAppRate(), 60);
    EXPECT_EQ(compositingDrawable->stagingCacheManager_->GetRSCacheUpdateManager()->GetNodeFilterAppRate(), 60);
}

/**
 * @tc.name: UpdateSurfaceFilterRate001
 * @tc.desc: test UpdateSurfaceFilterRate updates matched surface nodes only
 * @tc.type: FUNC
 */
HWTEST_F(RSEffectUtilsTest, UpdateSurfaceFilterRate001, TestSize.Level1)
{
    auto ctx = std::make_shared<RSContext>();
    ASSERT_NE(ctx, nullptr);
    auto& nodeMap = ctx->GetMutableNodeMap();

    constexpr NodeId matchedId = 920101;
    constexpr NodeId unmatchedId = 920102;
    RSSurfaceRenderNodeConfig matchedConfig = { .id = matchedId, .name = "demo_surface" };
    RSSurfaceRenderNodeConfig unmatchedConfig = { .id = unmatchedId, .name = "other_surface" };
    auto matchedNode = std::make_shared<RSSurfaceRenderNode>(matchedConfig, ctx);
    auto unmatchedNode = std::make_shared<RSSurfaceRenderNode>(unmatchedConfig, ctx);
    ASSERT_NE(matchedNode, nullptr);
    ASSERT_NE(unmatchedNode, nullptr);
    nodeMap.RegisterRenderNode(matchedNode);
    nodeMap.RegisterRenderNode(unmatchedNode);

    // suffix after last dot must be contained in node name to match
    RSEffectUtils::UpdateSurfaceFilterRate("com.test.demo", 30, ctx->GetNodeMap());
    EXPECT_EQ(matchedNode->GetApsFilterRate(), 30);
    EXPECT_EQ(unmatchedNode->GetApsFilterRate(), INVALID_FILTER_RATE);

    // zero rate is still applied to matched nodes
    RSEffectUtils::UpdateSurfaceFilterRate("com.test.demo", 0, ctx->GetNodeMap());
    EXPECT_EQ(matchedNode->GetApsFilterRate(), 0);
    EXPECT_EQ(unmatchedNode->GetApsFilterRate(), INVALID_FILTER_RATE);

    nodeMap.UnregisterRenderNode(matchedId);
    nodeMap.UnregisterRenderNode(unmatchedId);
}

/**
 * @tc.name: UpdateSurfaceFilterRate002
 * @tc.desc: test UpdateSurfaceFilterRate with empty suffix updates nothing
 * @tc.type: FUNC
 */
HWTEST_F(RSEffectUtilsTest, UpdateSurfaceFilterRate002, TestSize.Level1)
{
    auto ctx = std::make_shared<RSContext>();
    ASSERT_NE(ctx, nullptr);
    auto& nodeMap = ctx->GetMutableNodeMap();

    constexpr NodeId nodeId = 920103;
    RSSurfaceRenderNodeConfig config = { .id = nodeId, .name = "demo_surface" };
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(config, ctx);
    ASSERT_NE(surfaceNode, nullptr);
    nodeMap.RegisterRenderNode(surfaceNode);
    surfaceNode->SetApsFilterRate(30);

    // param without dot: suffix is empty, no surface should be updated
    RSEffectUtils::UpdateSurfaceFilterRate("nodotparam", 60, ctx->GetNodeMap());
    EXPECT_EQ(surfaceNode->GetApsFilterRate(), 30);

    nodeMap.UnregisterRenderNode(nodeId);
}

/**
 * @tc.name: UpdateSurfaceFilterRate003
 * @tc.desc: test UpdateSurfaceFilterRate with trailing dot param updates nothing
 * @tc.type: FUNC
 */
HWTEST_F(RSEffectUtilsTest, UpdateSurfaceFilterRate003, TestSize.Level1)
{
    auto ctx = std::make_shared<RSContext>();
    ASSERT_NE(ctx, nullptr);
    auto& nodeMap = ctx->GetMutableNodeMap();

    constexpr NodeId nodeId = 920104;
    RSSurfaceRenderNodeConfig config = { .id = nodeId, .name = "demo_surface" };
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(config, ctx);
    ASSERT_NE(surfaceNode, nullptr);
    nodeMap.RegisterRenderNode(surfaceNode);
    surfaceNode->SetApsFilterRate(30);

    // param ending with a dot: suffix after last dot is empty, no surface should be updated
    RSEffectUtils::UpdateSurfaceFilterRate("com.test.", 60, ctx->GetNodeMap());
    EXPECT_EQ(surfaceNode->GetApsFilterRate(), 30);

    nodeMap.UnregisterRenderNode(nodeId);
}

/**
 * @tc.name: UpdateSurfaceFilterRate004
 * @tc.desc: test UpdateSurfaceFilterRate skips null surface entries without crash
 * @tc.type: FUNC
 */
HWTEST_F(RSEffectUtilsTest, UpdateSurfaceFilterRate004, TestSize.Level1)
{
    auto ctx = std::make_shared<RSContext>();
    ASSERT_NE(ctx, nullptr);
    auto& nodeMap = ctx->GetMutableNodeMap();

    constexpr NodeId nodeId = 920105;
    constexpr NodeId nullEntryId = 920106;
    RSSurfaceRenderNodeConfig config = { .id = nodeId, .name = "demo_surface" };
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(config, ctx);
    ASSERT_NE(surfaceNode, nullptr);
    nodeMap.RegisterRenderNode(surfaceNode);
    // a null entry in the surface map exercises the null surfaceNode branch in the traversal lambda
    nodeMap.surfaceNodeMap_[nullEntryId] = nullptr;

    RSEffectUtils::UpdateSurfaceFilterRate("com.test.demo", 60, ctx->GetNodeMap());
    // traversal continues past the null entry and updates matched node
    EXPECT_EQ(surfaceNode->GetApsFilterRate(), 60);

    nodeMap.UnregisterRenderNode(nodeId);
    nodeMap.surfaceNodeMap_.erase(nullEntryId);
}
} // namespace Rosen
} // namespace OHOS
