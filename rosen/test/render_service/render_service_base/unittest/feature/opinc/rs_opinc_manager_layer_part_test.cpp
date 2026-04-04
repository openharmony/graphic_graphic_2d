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

#include "gtest/gtest.h"

#include "common/rs_obj_abs_geometry.h"
#include "feature/opinc/rs_opinc_manager.h"
#include "params/rs_render_params.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_dirty_region_manager.h"
#include "pipeline/rs_surface_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
constexpr NodeId DEFAULT_NODE_ID = 10;
constexpr NodeId SECOND_NODE_ID = 11;
constexpr NodeId THIRD_NODE_ID = 12;
constexpr int32_t ABS_RECT_LEFT = 10;
constexpr int32_t ABS_RECT_TOP = 10;
constexpr int32_t ABS_RECT_WIDTH = 100;
constexpr int32_t ABS_RECT_HEIGHT = 100;
constexpr int32_t OLD_RECT_LEFT = 0;
constexpr int32_t OLD_RECT_TOP = 0;
constexpr int32_t OLD_RECT_WIDTH = 20;
constexpr int32_t OLD_RECT_HEIGHT = 20;
constexpr int32_t CHILD_RECT_LEFT = 5;
constexpr int32_t CHILD_RECT_TOP = 5;
constexpr int32_t CHILD_RECT_WIDTH = 30;
constexpr int32_t CHILD_RECT_HEIGHT = 30;
constexpr int32_t WARMUP_UNCHANGED_COUNT = 5;
constexpr int32_t OUTSIDE_RECT_LEFT = 1000;
constexpr int32_t OUTSIDE_RECT_TOP = 1000;
constexpr int32_t OUTSIDE_RECT_WIDTH = 50;
constexpr int32_t OUTSIDE_RECT_HEIGHT = 50;
constexpr int32_t INSIDE_RECT_LEFT = 20;
constexpr int32_t INSIDE_RECT_TOP = 20;
constexpr int32_t INSIDE_RECT_WIDTH = 10;
constexpr int32_t INSIDE_RECT_HEIGHT = 10;
const RectI DEFAULT_ABS_RECT = { ABS_RECT_LEFT, ABS_RECT_TOP, ABS_RECT_WIDTH, ABS_RECT_HEIGHT };
const RectI DEFAULT_OLD_RECT = { OLD_RECT_LEFT, OLD_RECT_TOP, OLD_RECT_WIDTH, OLD_RECT_HEIGHT };
const RectI DEFAULT_CHILD_RECT = { CHILD_RECT_LEFT, CHILD_RECT_TOP, CHILD_RECT_WIDTH, CHILD_RECT_HEIGHT };
const RectI DEFAULT_OUTSIDE_RECT = { OUTSIDE_RECT_LEFT, OUTSIDE_RECT_TOP, OUTSIDE_RECT_WIDTH, OUTSIDE_RECT_HEIGHT };
const RectI DEFAULT_INSIDE_RECT = { INSIDE_RECT_LEFT, INSIDE_RECT_TOP, INSIDE_RECT_WIDTH, INSIDE_RECT_HEIGHT };
}

class RSOpincManagerLayerPartTest : public testing::Test {
public:
    struct LayerPartUpdateCaseContext {
        std::shared_ptr<RSCanvasRenderNode> node;
        std::shared_ptr<RSDirtyRegionManager> dirtyManager;
        RSRenderParams* stagingRenderParams = nullptr;
    };

    static std::shared_ptr<RSCanvasRenderNode> CreateCanvasNode(NodeId nodeId)
    {
        auto node = std::make_shared<RSCanvasRenderNode>(nodeId);
        auto& geo = node->GetMutableRenderProperties().boundsGeo_;
        if (geo == nullptr) {
            geo = std::make_shared<RSObjAbsGeometry>();
        }
        geo->absMatrix_ = Drawing::Matrix();
        geo->absRect_ = DEFAULT_ABS_RECT;
        node->absDrawRect_ = DEFAULT_ABS_RECT;
        return node;
    }

    static void WarmUpLayerPartUnchangeState(RSOpincCache& opincCache)
    {
        for (int32_t index = 0; index < WARMUP_UNCHANGED_COUNT; ++index) {
            (void)opincCache.IsLayerPartRenderUnchangeState();
        }
    }

    static LayerPartUpdateCaseContext CreateLayerPartUpdateCaseContext(NodeId nodeId, bool layerPartRenderEnabled)
    {
        LayerPartUpdateCaseContext context;
        context.node = CreateCanvasNode(nodeId);
        context.node->InitRenderParams();
        context.dirtyManager = std::make_shared<RSDirtyRegionManager>();
        if (context.node != nullptr) {
            context.stagingRenderParams = context.node->GetStagingRenderParams().get();
            if (context.stagingRenderParams != nullptr) {
                context.stagingRenderParams->SetLayerPartRenderEnabled(layerPartRenderEnabled);
            }
        }
        return context;
    }
};

/**
 * @tc.name: InitLayerPartRenderNodeDisabled
 * @tc.desc: Verify InitLayerPartRenderNode returns early when switch is disabled
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincManagerLayerPartTest, InitLayerPartRenderNodeDisabled, TestSize.Level1)
{
    auto node = CreateCanvasNode(DEFAULT_NODE_ID);
    std::shared_ptr<RSDirtyRegionManager> dirtyManager = nullptr;

    RSOpincManager::Instance().InitLayerPartRenderNode(false, *node, dirtyManager);

    ASSERT_EQ(dirtyManager, nullptr);
    ASSERT_FALSE(node->GetOpincCache().IsLayerPartRender());
}

/**
 * @tc.name: InitLayerPartRenderNodeSuggestedClearsDirtyManager
 * @tc.desc: Verify InitLayerPartRenderNode clears and reuses dirty manager for suggested node
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincManagerLayerPartTest, InitLayerPartRenderNodeSuggestedClearsDirtyManager, TestSize.Level1)
{
    auto node = CreateCanvasNode(SECOND_NODE_ID);
    node->GetOpincCache().MarkSuggestLayerPartRenderNode(true);
    auto& cachedDirtyManager = node->GetOpincCache().GetLayerPartRenderDirtyManager();
    ASSERT_NE(cachedDirtyManager, nullptr);
    cachedDirtyManager->MergeDirtyRect(DEFAULT_ABS_RECT);
    ASSERT_FALSE(cachedDirtyManager->GetCurrentFrameDirtyRegion().IsEmpty());

    std::shared_ptr<RSDirtyRegionManager> dirtyManager = nullptr;
    RSOpincManager::Instance().InitLayerPartRenderNode(true, *node, dirtyManager);

    ASSERT_EQ(dirtyManager, cachedDirtyManager);
    ASSERT_TRUE(dirtyManager->GetCurrentFrameDirtyRegion().IsEmpty());
    ASSERT_TRUE(node->GetOpincCache().IsLayerPartRender());
}

/**
 * @tc.name: InitLayerPartRenderNodeEnableByDisableStrategy
 * @tc.desc: Verify InitLayerPartRenderNode still enters layer-part flow when strategy is CACHE_DISABLE
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincManagerLayerPartTest, InitLayerPartRenderNodeEnableByDisableStrategy, TestSize.Level1)
{
    auto node = CreateCanvasNode(DEFAULT_NODE_ID);
    node->GetOpincCache().MarkSuggestLayerPartRenderNode(false);
    node->GetOpincCache().SetLayerPartRenderNodeStrategyType(NodeStrategyType::CACHE_DISABLE);
    std::shared_ptr<RSDirtyRegionManager> dirtyManager = nullptr;

    RSOpincManager::Instance().InitLayerPartRenderNode(true, *node, dirtyManager);

    ASSERT_NE(dirtyManager, nullptr);
    ASSERT_TRUE(node->GetOpincCache().IsLayerPartRender());
}

/**
 * @tc.name: InitLayerPartRenderNodeReturnWhenNotSuggested
 * @tc.desc: Verify InitLayerPartRenderNode returns early when not suggested and strategy is not CACHE_DISABLE
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincManagerLayerPartTest, InitLayerPartRenderNodeReturnWhenNotSuggested, TestSize.Level1)
{
    auto node = CreateCanvasNode(SECOND_NODE_ID);
    node->GetOpincCache().MarkSuggestLayerPartRenderNode(false);
    node->GetOpincCache().SetLayerPartRenderNodeStrategyType(NodeStrategyType::CACHE_NONE);
    std::shared_ptr<RSDirtyRegionManager> dirtyManager = nullptr;

    RSOpincManager::Instance().InitLayerPartRenderNode(true, *node, dirtyManager);

    ASSERT_EQ(dirtyManager, nullptr);
    ASSERT_FALSE(node->GetOpincCache().IsLayerPartRender());
}

/**
 * @tc.name: CalculateLayerPartRenderDirtyRegionNormalPath
 * @tc.desc: Verify CalculateAndUpdateLayerPartRenderDirtyRegion normal path enables layer-part and sets dirty region
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincManagerLayerPartTest, CalculateLayerPartRenderDirtyRegionNormalPath, TestSize.Level1)
{
    auto node = CreateCanvasNode(THIRD_NODE_ID);
    node->InitRenderParams();
    node->GetOpincCache().SetLayerPartRender(true);
    auto dirtyManager = node->GetOpincCache().GetLayerPartRenderDirtyManager();
    ASSERT_NE(dirtyManager, nullptr);
    dirtyManager->SetCurrentFrameDirtyRect(DEFAULT_OLD_RECT);

    RSOpincManager::Instance().CalculateAndUpdateLayerPartRenderDirtyRegion(*node, dirtyManager, DEFAULT_OLD_RECT,
        false);

    auto& cachedDirtyManager = node->GetOpincCache().GetLayerPartRenderDirtyManager();
    auto& stagingRenderParams = node->GetStagingRenderParams();
    ASSERT_NE(cachedDirtyManager, nullptr);
    ASSERT_NE(stagingRenderParams, nullptr);
    ASSERT_TRUE(stagingRenderParams->GetLayerPartRenderEnabled());
    ASSERT_EQ(stagingRenderParams->GetLayerPartRenderCurrentFrameDirtyRegion(), DEFAULT_ABS_RECT);
    ASSERT_EQ(node->GetNodeGroupType(), RSRenderNode::NodeGroupType::GROUPED_BY_USER);
    ASSERT_EQ(dirtyManager, nullptr);
}

/**
 * @tc.name: CalculateLayerPartRenderDirtyRegionNullDirtyManager
 * @tc.desc: Verify CalculateLayerPartRenderDirtyRegion returns early for null manager
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincManagerLayerPartTest, CalculateLayerPartRenderDirtyRegionNullDirtyManager, TestSize.Level1)
{
    auto node = CreateCanvasNode(THIRD_NODE_ID);
    std::shared_ptr<RSDirtyRegionManager> dirtyManager = nullptr;

    RSOpincManager::Instance().CalculateAndUpdateLayerPartRenderDirtyRegion(*node, dirtyManager, DEFAULT_ABS_RECT,
        false);

    ASSERT_EQ(dirtyManager, nullptr);
}

/**
 * @tc.name: CalculateLayerPartRenderDirtyRegionNullStagingParams
 * @tc.desc: Verify CalculateAndUpdateLayerPartRenderDirtyRegion returns early when staging params is null
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincManagerLayerPartTest, CalculateLayerPartRenderDirtyRegionNullStagingParams, TestSize.Level1)
{
    auto node = CreateCanvasNode(DEFAULT_NODE_ID);
    node->InitRenderParams();
    node->GetOpincCache().SetLayerPartRender(true);
    auto dirtyManager = std::make_shared<RSDirtyRegionManager>();
    ASSERT_NE(dirtyManager, nullptr);

    auto& stagingRenderParams = node->GetStagingRenderParams();
    ASSERT_NE(stagingRenderParams, nullptr);
    stagingRenderParams.reset();
    ASSERT_EQ(node->GetStagingRenderParams(), nullptr);

    RSOpincManager::Instance().CalculateAndUpdateLayerPartRenderDirtyRegion(*node, dirtyManager, DEFAULT_ABS_RECT,
        false);

    ASSERT_NE(dirtyManager, nullptr);
    ASSERT_EQ(node->GetStagingRenderParams(), nullptr);
}

/**
 * @tc.name: CalculateLayerPartRenderDirtyRegionLayerPartDisabled
 * @tc.desc: Verify CalculateLayerPartRenderDirtyRegion disables layer-part state when node is not enabled
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincManagerLayerPartTest, CalculateLayerPartRenderDirtyRegionLayerPartDisabled, TestSize.Level1)
{
    auto node = CreateCanvasNode(DEFAULT_NODE_ID);
    node->InitRenderParams();
    node->GetOpincCache().SetLayerPartRenderNodeStrategyType(NodeStrategyType::CACHE_DISABLE);
    auto dirtyManager = std::make_shared<RSDirtyRegionManager>();
    auto dirtyManagerHold = dirtyManager;
    ASSERT_NE(dirtyManager, nullptr);
    auto& stagingRenderParams = node->GetStagingRenderParams();
    ASSERT_NE(stagingRenderParams, nullptr);
    stagingRenderParams->SetLayerPartRenderEnabled(true);

    RSOpincManager::Instance().CalculateAndUpdateLayerPartRenderDirtyRegion(*node, dirtyManager, DEFAULT_ABS_RECT,
        false);

    ASSERT_NE(dirtyManagerHold, nullptr);
    ASSERT_FALSE(stagingRenderParams->GetLayerPartRenderEnabled());
    ASSERT_EQ(node->GetOpincCache().GetLayerPartRenderNodeStrategyType(), NodeStrategyType::CACHE_NONE);
    ASSERT_EQ(node->GetNodeGroupType(), RSRenderNode::NodeGroupType::NONE);
}

/**
 * @tc.name: CalculateLayerPartRenderDirtyRegionUsesNodeAbsRect
 * @tc.desc: Verify CalculateLayerPartRenderDirtyRegion uses node abs rect for changing node
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincManagerLayerPartTest, CalculateLayerPartRenderDirtyRegionUsesNodeAbsRect, TestSize.Level1)
{
    auto node = CreateCanvasNode(SECOND_NODE_ID);
    node->InitRenderParams();
    node->GetOpincCache().SetLayerPartRender(true);
    auto dirtyManager = node->GetOpincCache().GetLayerPartRenderDirtyManager();
    ASSERT_NE(dirtyManager, nullptr);
    dirtyManager->SetCurrentFrameDirtyRect(DEFAULT_OLD_RECT);

    RSOpincManager::Instance().CalculateAndUpdateLayerPartRenderDirtyRegion(*node, dirtyManager, DEFAULT_OLD_RECT,
        false);

    auto& cachedDirtyManager = node->GetOpincCache().GetLayerPartRenderDirtyManager();
    auto& stagingRenderParams = node->GetStagingRenderParams();
    ASSERT_NE(cachedDirtyManager, nullptr);
    ASSERT_NE(stagingRenderParams, nullptr);
    ASSERT_TRUE(stagingRenderParams->GetLayerPartRenderEnabled());
    ASSERT_EQ(stagingRenderParams->GetLayerPartRenderCurrentFrameDirtyRegion(), DEFAULT_ABS_RECT);
    ASSERT_EQ(node->GetNodeGroupType(), RSRenderNode::NodeGroupType::GROUPED_BY_USER);
    ASSERT_EQ(dirtyManager, nullptr);
}

/**
 * @tc.name: CalculateLayerPartRenderDirtyRegionChangeStateRefreshDirtyManager
 * @tc.desc: Verify changed-state branch merges node abs rect into dirty manager before mapping
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincManagerLayerPartTest, CalculateLayerPartRenderDirtyRegionChangeStateRefreshDirtyManager,
    TestSize.Level1)
{
    auto node = CreateCanvasNode(SECOND_NODE_ID);
    node->InitRenderParams();
    auto dirtyManager = std::make_shared<RSDirtyRegionManager>();
    ASSERT_NE(dirtyManager, nullptr);
    RectI layerCurDirty;

    bool ret = RSOpincManager::Instance().CalculateLayerPartRenderDirtyRegion(
        *node, dirtyManager, DEFAULT_OLD_RECT, layerCurDirty);

    ASSERT_TRUE(ret);
    EXPECT_EQ(layerCurDirty, DEFAULT_ABS_RECT);
    EXPECT_EQ(dirtyManager->GetDirtyRegion(), DEFAULT_ABS_RECT);
}

/**
 * @tc.name: CalculateLayerPartRenderDirtyRegionFallbackToNodeAbsRect
 * @tc.desc: Verify unchanged node falls back to node abs rect when dirty region is not fully inside node abs rect
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincManagerLayerPartTest, CalculateLayerPartRenderDirtyRegionFallbackToNodeAbsRect, TestSize.Level1)
{
    auto node = CreateCanvasNode(THIRD_NODE_ID);
    node->InitRenderParams();
    node->GetOpincCache().SetLayerPartRender(true);
    node->MarkNodeGroup(RSRenderNode::NodeGroupType::GROUPED_BY_USER, true, false);
    WarmUpLayerPartUnchangeState(node->GetOpincCache());

    auto dirtyManager = node->GetOpincCache().GetLayerPartRenderDirtyManager();
    ASSERT_NE(dirtyManager, nullptr);
    dirtyManager->SetCurrentFrameDirtyRect(DEFAULT_OLD_RECT);

    RSOpincManager::Instance().CalculateAndUpdateLayerPartRenderDirtyRegion(*node, dirtyManager, DEFAULT_OLD_RECT,
        false);

    auto& cachedDirtyManager = node->GetOpincCache().GetLayerPartRenderDirtyManager();
    auto& stagingRenderParams = node->GetStagingRenderParams();
    ASSERT_NE(cachedDirtyManager, nullptr);
    ASSERT_NE(stagingRenderParams, nullptr);
    ASSERT_TRUE(stagingRenderParams->GetLayerPartRenderEnabled());
    ASSERT_EQ(stagingRenderParams->GetLayerPartRenderCurrentFrameDirtyRegion(), DEFAULT_ABS_RECT);
    ASSERT_EQ(dirtyManager, nullptr);
}

/**
 * @tc.name: CalculateLayerPartRenderDirtyRegionNotLayerPartRender
 * @tc.desc: Verify CalculateAndUpdateLayerPartRenderDirtyRegion returns early when node is not layer-part render
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincManagerLayerPartTest, CalculateLayerPartRenderDirtyRegionNotLayerPartRender, TestSize.Level1)
{
    auto node = CreateCanvasNode(DEFAULT_NODE_ID);
    node->InitRenderParams();
    auto dirtyManager = std::make_shared<RSDirtyRegionManager>();
    ASSERT_NE(dirtyManager, nullptr);
    auto& stagingRenderParams = node->GetStagingRenderParams();
    ASSERT_NE(stagingRenderParams, nullptr);
    stagingRenderParams->SetLayerPartRenderEnabled(true);

    RSOpincManager::Instance().CalculateAndUpdateLayerPartRenderDirtyRegion(*node, dirtyManager, DEFAULT_ABS_RECT,
        false);

    ASSERT_NE(dirtyManager, nullptr);
    ASSERT_TRUE(stagingRenderParams->GetLayerPartRenderEnabled());
}

/**
 * @tc.name: CalculateLayerPartRenderDirtyRegionGeoNull
 * @tc.desc: Verify CalculateAndUpdateLayerPartRenderDirtyRegion clears layer-part state when geometry is null
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincManagerLayerPartTest, CalculateLayerPartRenderDirtyRegionGeoNull, TestSize.Level1)
{
    auto node = std::make_shared<RSCanvasRenderNode>(SECOND_NODE_ID);
    ASSERT_NE(node, nullptr);
    node->InitRenderParams();
    node->GetOpincCache().SetLayerPartRender(true);
    node->GetMutableRenderProperties().boundsGeo_ = nullptr;
    auto dirtyManager = std::make_shared<RSDirtyRegionManager>();
    ASSERT_NE(dirtyManager, nullptr);
    auto& stagingRenderParams = node->GetStagingRenderParams();
    ASSERT_NE(stagingRenderParams, nullptr);
    stagingRenderParams->SetLayerPartRenderEnabled(true);

    RSOpincManager::Instance().CalculateAndUpdateLayerPartRenderDirtyRegion(*node, dirtyManager, DEFAULT_ABS_RECT,
        false);

    ASSERT_NE(dirtyManager, nullptr);
    ASSERT_FALSE(stagingRenderParams->GetLayerPartRenderEnabled());
    ASSERT_EQ(node->GetNodeGroupType(), RSRenderNode::NodeGroupType::NONE);
}

/**
 * @tc.name: CalculateLayerPartRenderDirtyRegionInvertFailed
 * @tc.desc: Verify CalculateAndUpdateLayerPartRenderDirtyRegion clears layer-part state when matrix invert fails
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincManagerLayerPartTest, CalculateLayerPartRenderDirtyRegionInvertFailed, TestSize.Level1)
{
    auto node = CreateCanvasNode(SECOND_NODE_ID);
    node->InitRenderParams();
    node->GetOpincCache().SetLayerPartRender(true);
    auto& geo = node->GetMutableRenderProperties().boundsGeo_;
    ASSERT_NE(geo, nullptr);
    geo->absMatrix_->Set(Drawing::Matrix::SCALE_X, 0.0f);
    geo->absMatrix_->Set(Drawing::Matrix::SCALE_Y, 0.0f);

    auto dirtyManager = std::make_shared<RSDirtyRegionManager>();
    ASSERT_NE(dirtyManager, nullptr);
    auto& stagingRenderParams = node->GetStagingRenderParams();
    ASSERT_NE(stagingRenderParams, nullptr);
    stagingRenderParams->SetLayerPartRenderEnabled(true);

    RSOpincManager::Instance().CalculateAndUpdateLayerPartRenderDirtyRegion(*node, dirtyManager, DEFAULT_ABS_RECT,
        false);

    ASSERT_NE(dirtyManager, nullptr);
    ASSERT_FALSE(stagingRenderParams->GetLayerPartRenderEnabled());
    ASSERT_EQ(node->GetNodeGroupType(), RSRenderNode::NodeGroupType::NONE);
}

/**
 * @tc.name: CalculateLayerPartRenderDirtyRegionOutsideNodeAbsRect
 * @tc.desc: Verify out-of-range dirty region falls back to node abs rect
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincManagerLayerPartTest, CalculateLayerPartRenderDirtyRegionOutsideNodeAbsRect, TestSize.Level1)
{
    auto node = CreateCanvasNode(THIRD_NODE_ID);
    node->InitRenderParams();
    node->GetOpincCache().SetLayerPartRender(true);
    node->MarkNodeGroup(RSRenderNode::NodeGroupType::GROUPED_BY_USER, true, false);
    WarmUpLayerPartUnchangeState(node->GetOpincCache());

    auto dirtyManager = node->GetOpincCache().GetLayerPartRenderDirtyManager();
    ASSERT_NE(dirtyManager, nullptr);
    dirtyManager->SetCurrentFrameDirtyRect(DEFAULT_OUTSIDE_RECT);

    RSOpincManager::Instance().CalculateAndUpdateLayerPartRenderDirtyRegion(*node, dirtyManager,
        DEFAULT_OUTSIDE_RECT, false);

    auto& cachedDirtyManager = node->GetOpincCache().GetLayerPartRenderDirtyManager();
    auto& stagingRenderParams = node->GetStagingRenderParams();
    ASSERT_NE(cachedDirtyManager, nullptr);
    ASSERT_NE(stagingRenderParams, nullptr);
    ASSERT_EQ(stagingRenderParams->GetLayerPartRenderCurrentFrameDirtyRegion(), DEFAULT_ABS_RECT);
    ASSERT_EQ(dirtyManager, nullptr);
}

/**
 * @tc.name: CalculateLayerPartRenderDirtyRegionInsideNodeAbsRect
 * @tc.desc: Verify unchanged node keeps inside dirty region and does not fallback to node abs rect
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincManagerLayerPartTest, CalculateLayerPartRenderDirtyRegionInsideNodeAbsRect, TestSize.Level1)
{
    auto node = CreateCanvasNode(THIRD_NODE_ID);
    node->InitRenderParams();
    node->GetOpincCache().SetLayerPartRender(true);
    node->MarkNodeGroup(RSRenderNode::NodeGroupType::GROUPED_BY_USER, true, false);
    WarmUpLayerPartUnchangeState(node->GetOpincCache());

    auto dirtyManager = node->GetOpincCache().GetLayerPartRenderDirtyManager();
    ASSERT_NE(dirtyManager, nullptr);
    dirtyManager->SetCurrentFrameDirtyRect(DEFAULT_INSIDE_RECT);

    RSOpincManager::Instance().CalculateAndUpdateLayerPartRenderDirtyRegion(*node, dirtyManager,
        DEFAULT_INSIDE_RECT, false);

    auto& cachedDirtyManager = node->GetOpincCache().GetLayerPartRenderDirtyManager();
    auto& stagingRenderParams = node->GetStagingRenderParams();
    ASSERT_NE(cachedDirtyManager, nullptr);
    ASSERT_NE(stagingRenderParams, nullptr);
    ASSERT_TRUE(stagingRenderParams->GetLayerPartRenderEnabled());
    ASSERT_EQ(stagingRenderParams->GetLayerPartRenderCurrentFrameDirtyRegion(), DEFAULT_INSIDE_RECT);
    ASSERT_EQ(node->GetNodeGroupType(), RSRenderNode::NodeGroupType::GROUPED_BY_USER);
    ASSERT_EQ(dirtyManager, nullptr);
}

/**
 * @tc.name: CalculateLayerPartRenderDirtyRegionGeoNullReturnFalse
 * @tc.desc: Verify branch at line 215 returns false when bounds geometry is null
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincManagerLayerPartTest, CalculateLayerPartRenderDirtyRegionGeoNullReturnFalse, TestSize.Level1)
{
    auto node = std::make_shared<RSCanvasRenderNode>(DEFAULT_NODE_ID);
    ASSERT_NE(node, nullptr);
    node->InitRenderParams();
    node->GetMutableRenderProperties().boundsGeo_ = nullptr;

    auto dirtyManager = std::make_shared<RSDirtyRegionManager>();
    ASSERT_NE(dirtyManager, nullptr);
    RectI layerCurDirty;

    bool ret = RSOpincManager::Instance().CalculateLayerPartRenderDirtyRegion(
        *node, dirtyManager, DEFAULT_ABS_RECT, layerCurDirty);

    ASSERT_FALSE(ret);
}

/**
 * @tc.name: CalculateLayerPartRenderDirtyRegionInvertFailureReturnFalse
 * @tc.desc: Verify branch at line 220 returns false when matrix invert fails
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincManagerLayerPartTest, CalculateLayerPartRenderDirtyRegionInvertFailureReturnFalse, TestSize.Level1)
{
    auto node = CreateCanvasNode(SECOND_NODE_ID);
    node->InitRenderParams();
    auto& geo = node->GetMutableRenderProperties().boundsGeo_;
    ASSERT_NE(geo, nullptr);
    geo->absMatrix_->Set(Drawing::Matrix::SCALE_X, 0.0f);
    geo->absMatrix_->Set(Drawing::Matrix::SCALE_Y, 0.0f);

    auto dirtyManager = std::make_shared<RSDirtyRegionManager>();
    ASSERT_NE(dirtyManager, nullptr);
    RectI layerCurDirty;

    bool ret = RSOpincManager::Instance().CalculateLayerPartRenderDirtyRegion(
        *node, dirtyManager, DEFAULT_ABS_RECT, layerCurDirty);

    ASSERT_FALSE(ret);
}

/**
 * @tc.name: CalculateLayerPartRenderDirtyRegionOutsideDirtyFallback
 * @tc.desc: Verify branch at line 247 falls back to node abs rect when dirty is outside
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincManagerLayerPartTest, CalculateLayerPartRenderDirtyRegionOutsideDirtyFallback, TestSize.Level1)
{
    auto node = CreateCanvasNode(THIRD_NODE_ID);
    node->InitRenderParams();
    node->MarkNodeGroup(RSRenderNode::NodeGroupType::GROUPED_BY_USER, true, false);
    WarmUpLayerPartUnchangeState(node->GetOpincCache());

    auto dirtyManager = std::make_shared<RSDirtyRegionManager>();
    ASSERT_NE(dirtyManager, nullptr);
    dirtyManager->SetCurrentFrameDirtyRect(DEFAULT_OUTSIDE_RECT);

    RectI layerCurDirty;
    bool ret = RSOpincManager::Instance().CalculateLayerPartRenderDirtyRegion(
        *node, dirtyManager, DEFAULT_OUTSIDE_RECT, layerCurDirty);

    ASSERT_TRUE(ret);
    ASSERT_EQ(layerCurDirty, DEFAULT_ABS_RECT);
}

/**
 * @tc.name: CalculateAndUpdateLayerPartRenderDirtyRegionCacheDisablePathHit
 * @tc.desc: Verify cache-disable strategy path clears strategy to cache-none and releases dirty manager
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincManagerLayerPartTest, CalculateAndUpdateLayerPartRenderDirtyRegionCacheDisablePathHit,
    TestSize.Level1)
{
    auto context = CreateLayerPartUpdateCaseContext(DEFAULT_NODE_ID + 30, true);
    ASSERT_NE(context.node, nullptr);
    ASSERT_NE(context.dirtyManager, nullptr);
    ASSERT_NE(context.stagingRenderParams, nullptr);
    context.stagingRenderParams->SetDrawingCacheType(RSDrawingCacheType::FORCED_CACHE);
    context.node->GetOpincCache().SetLayerPartRenderNodeStrategyType(NodeStrategyType::CACHE_DISABLE);
    context.node->GetOpincCache().SetLayerPartRender(true);

    RSOpincManager::Instance().CalculateAndUpdateLayerPartRenderDirtyRegion(
        *context.node, context.dirtyManager, DEFAULT_ABS_RECT, false);

    EXPECT_EQ(context.dirtyManager, nullptr);
    EXPECT_FALSE(context.stagingRenderParams->GetLayerPartRenderEnabled());
    EXPECT_EQ(context.stagingRenderParams->GetDrawingCacheType(), context.node->GetDrawingCacheType());
    EXPECT_EQ(context.node->GetOpincCache().GetLayerPartRenderNodeStrategyType(), NodeStrategyType::CACHE_NONE);
}

/**
 * @tc.name: CalculateAndUpdateLayerPartRenderDirtyRegionHasUifirstChildPathHit
 * @tc.desc: Verify has-uifirst-child path clears layer-part render and keeps dirty manager for caller
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincManagerLayerPartTest, CalculateAndUpdateLayerPartRenderDirtyRegionHasUifirstChildPathHit,
    TestSize.Level1)
{
    auto context = CreateLayerPartUpdateCaseContext(DEFAULT_NODE_ID + 34, true);
    ASSERT_NE(context.node, nullptr);
    ASSERT_NE(context.dirtyManager, nullptr);
    ASSERT_NE(context.stagingRenderParams, nullptr);
    context.node->GetOpincCache().SetLayerPartRender(true);
    context.node->MarkNodeGroup(RSRenderNode::NodeGroupType::GROUPED_BY_USER, true, false);
    context.dirtyManager->SetHasUifirstChild(true);

    RSOpincManager::Instance().CalculateAndUpdateLayerPartRenderDirtyRegion(
        *context.node, context.dirtyManager, DEFAULT_ABS_RECT, false);

    EXPECT_NE(context.dirtyManager, nullptr);
    EXPECT_FALSE(context.stagingRenderParams->GetLayerPartRenderEnabled());
    EXPECT_EQ(context.node->GetNodeGroupType(), RSRenderNode::NodeGroupType::NONE);
    EXPECT_EQ(context.stagingRenderParams->GetDrawingCacheType(), context.node->GetDrawingCacheType());
}

/**
 * @tc.name: CalculateAndUpdateLayerPartRenderDirtyRegionDisableAnimationPathHit
 * @tc.desc: Verify disable-animation path clears layer-part render and keeps dirty manager for caller
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincManagerLayerPartTest, CalculateAndUpdateLayerPartRenderDirtyRegionDisableAnimationPathHit,
    TestSize.Level1)
{
    auto context = CreateLayerPartUpdateCaseContext(DEFAULT_NODE_ID + 35, true);
    ASSERT_NE(context.node, nullptr);
    ASSERT_NE(context.dirtyManager, nullptr);
    ASSERT_NE(context.stagingRenderParams, nullptr);
    context.node->GetOpincCache().SetLayerPartRender(true);
    context.node->MarkNodeGroup(RSRenderNode::NodeGroupType::GROUPED_BY_USER, true, false);

    RSOpincManager::Instance().CalculateAndUpdateLayerPartRenderDirtyRegion(
        *context.node, context.dirtyManager, DEFAULT_ABS_RECT, true);

    EXPECT_NE(context.dirtyManager, nullptr);
    EXPECT_FALSE(context.stagingRenderParams->GetLayerPartRenderEnabled());
    EXPECT_EQ(context.node->GetNodeGroupType(), RSRenderNode::NodeGroupType::NONE);
    EXPECT_EQ(context.stagingRenderParams->GetDrawingCacheType(), context.node->GetDrawingCacheType());
}

/**
 * @tc.name: CalculateAndUpdateLayerPartRenderDirtyRegionMaterialNodePathHit
 * @tc.desc: Verify material-node path updates strategy to cache-disable and keeps dirty manager alive
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincManagerLayerPartTest, CalculateAndUpdateLayerPartRenderDirtyRegionMaterialNodePathHit,
    TestSize.Level1)
{
    auto context = CreateLayerPartUpdateCaseContext(DEFAULT_NODE_ID + 31, false);
    ASSERT_NE(context.node, nullptr);
    ASSERT_NE(context.dirtyManager, nullptr);
    ASSERT_NE(context.stagingRenderParams, nullptr);
    context.node->GetOpincCache().SetLayerPartRender(true);
    context.node->GetOpincCache().MarkMaterialNode(true);
    context.node->GetOpincCache().MarkSuggestLayerPartRenderNode(true);
    context.node->GetOpincCache().SetLayerPartRenderNodeStrategyType(NodeStrategyType::NODE_GROUP);

    RSOpincManager::Instance().CalculateAndUpdateLayerPartRenderDirtyRegion(
        *context.node, context.dirtyManager, DEFAULT_ABS_RECT, false);

    EXPECT_NE(context.dirtyManager, nullptr);
    EXPECT_FALSE(context.stagingRenderParams->GetLayerPartRenderEnabled());
    EXPECT_FALSE(context.node->GetOpincCache().IsSuggestLayerPartRenderNode());
    EXPECT_EQ(context.node->GetOpincCache().GetLayerPartRenderNodeStrategyType(), NodeStrategyType::CACHE_DISABLE);
}

/**
 * @tc.name: CalculateAndUpdateLayerPartRenderDirtyRegionCalculateFailedPathHit
 * @tc.desc: Verify calculate-failure path disables layer-part and keeps dirty manager for caller
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincManagerLayerPartTest, CalculateAndUpdateLayerPartRenderDirtyRegionCalculateFailedPathHit,
    TestSize.Level1)
{
    auto context = CreateLayerPartUpdateCaseContext(DEFAULT_NODE_ID + 32, true);
    ASSERT_NE(context.node, nullptr);
    ASSERT_NE(context.dirtyManager, nullptr);
    ASSERT_NE(context.stagingRenderParams, nullptr);
    context.node->GetOpincCache().SetLayerPartRender(true);
    context.node->GetMutableRenderProperties().boundsGeo_ = nullptr;

    RSOpincManager::Instance().CalculateAndUpdateLayerPartRenderDirtyRegion(
        *context.node, context.dirtyManager, DEFAULT_ABS_RECT, false);

    EXPECT_NE(context.dirtyManager, nullptr);
    EXPECT_FALSE(context.stagingRenderParams->GetLayerPartRenderEnabled());
}

/**
 * @tc.name: CalculateAndUpdateLayerPartRenderDirtyRegionKeepGroupedByUserPathHit
 * @tc.desc: Verify grouped-by-user path bypasses regrouping and still enables layer-part render
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincManagerLayerPartTest, CalculateAndUpdateLayerPartRenderDirtyRegionKeepGroupedByUserPathHit,
    TestSize.Level1)
{
    auto context = CreateLayerPartUpdateCaseContext(DEFAULT_NODE_ID + 33, false);
    ASSERT_NE(context.node, nullptr);
    ASSERT_NE(context.dirtyManager, nullptr);
    ASSERT_NE(context.stagingRenderParams, nullptr);
    context.node->GetOpincCache().SetLayerPartRender(true);
    context.node->MarkNodeGroup(RSRenderNode::NodeGroupType::GROUPED_BY_USER, true, false);

    RSOpincManager::Instance().CalculateAndUpdateLayerPartRenderDirtyRegion(
        *context.node, context.dirtyManager, DEFAULT_ABS_RECT, false);

    EXPECT_EQ(context.node->GetNodeGroupType(), RSRenderNode::NodeGroupType::GROUPED_BY_USER);
    EXPECT_TRUE(context.stagingRenderParams->GetLayerPartRenderEnabled());
    EXPECT_EQ(context.dirtyManager, nullptr);
}

/**
 * @tc.name: UpdateLayerPartRenderDirtyRegionNullManager
 * @tc.desc: Verify UpdateLayerPartRenderDirtyRegion returns false for null manager
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincManagerLayerPartTest, UpdateLayerPartRenderDirtyRegionNullManager, TestSize.Level1)
{
    auto node = CreateCanvasNode(DEFAULT_NODE_ID);
    node->InitRenderParams();
    std::shared_ptr<RSDirtyRegionManager> dirtyManager = nullptr;

    EXPECT_FALSE(node->UpdateLayerPartRenderDirtyRegion(dirtyManager));
}

/**
 * @tc.name: UpdateLayerPartRenderDirtyRegionWithoutDirtyFlag
 * @tc.desc: Verify UpdateLayerPartRenderDirtyRegion still refreshes cached old rect without merging dirty
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincManagerLayerPartTest, UpdateLayerPartRenderDirtyRegionWithoutDirtyFlag, TestSize.Level1)
{
    auto node = CreateCanvasNode(SECOND_NODE_ID);
    auto dirtyManager = std::make_shared<RSDirtyRegionManager>();
    ASSERT_NE(dirtyManager, nullptr);
    node->GetOpincCache().SetLayerPartRenderDirtyFlag(false);

    EXPECT_TRUE(node->UpdateLayerPartRenderDirtyRegion(dirtyManager));
    EXPECT_TRUE(dirtyManager->GetCurrentFrameDirtyRegion().IsEmpty());
    EXPECT_EQ(node->GetOpincCache().GetLayerPartRenderOldAbsDrawRect(), DEFAULT_ABS_RECT);
}

/**
 * @tc.name: UpdateLayerPartRenderDirtyRegionWithDirtyFlag
 * @tc.desc: Verify UpdateLayerPartRenderDirtyRegion merges current and old abs rect when dirty flag is set
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincManagerLayerPartTest, UpdateLayerPartRenderDirtyRegionWithDirtyFlag, TestSize.Level1)
{
    auto node = CreateCanvasNode(THIRD_NODE_ID);
    auto dirtyManager = std::make_shared<RSDirtyRegionManager>();
    ASSERT_NE(dirtyManager, nullptr);

    RectI oldAbsRect = DEFAULT_OLD_RECT;
    node->GetOpincCache().SetLayerPartRenderOldAbsDrawRect(oldAbsRect);
    node->GetOpincCache().SetLayerPartRenderDirtyFlag(true);

    EXPECT_TRUE(node->UpdateLayerPartRenderDirtyRegion(dirtyManager));
    EXPECT_FALSE(dirtyManager->GetCurrentFrameDirtyRegion().IsEmpty());
    EXPECT_EQ(node->GetOpincCache().GetLayerPartRenderOldAbsDrawRect(), DEFAULT_ABS_RECT);
}

/**
 * @tc.name: SubTreeSkipPrepareWithLayerPartDirtyManager
 * @tc.desc: Verify SubTreeSkipPrepare also merges subtree dirty into layer-part dirty manager
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincManagerLayerPartTest, SubTreeSkipPrepareWithLayerPartDirtyManager, TestSize.Level1)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(DEFAULT_NODE_ID);
    ASSERT_NE(node, nullptr);
    node->InitRenderParams();
#ifdef RS_ENABLE_GPU
    ASSERT_NE(node->GetStagingRenderParams(), nullptr);
#endif
    auto layerPartDirtyManager = std::make_shared<RSDirtyRegionManager>();
    ASSERT_NE(layerPartDirtyManager, nullptr);

    node->hasChildrenOutOfRect_ = true;
    node->lastFrameHasChildrenOutOfRect_ = true;
    node->childrenRect_ = DEFAULT_CHILD_RECT;
    node->oldChildrenRect_ = DEFAULT_CHILD_RECT;
    node->oldClipRect_ = DEFAULT_ABS_RECT;

    RSDirtyRegionManager dirtyManager;
    node->SubTreeSkipPrepare(dirtyManager, true, true, DEFAULT_ABS_RECT, layerPartDirtyManager);

    ASSERT_FALSE(layerPartDirtyManager->GetCurrentFrameDirtyRegion().IsEmpty());
}
} // namespace OHOS::Rosen
