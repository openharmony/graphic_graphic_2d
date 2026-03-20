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
    node->GetOpincCache().SetLayerPartRender(true);
    auto dirtyManager = node->GetOpincCache().GetLayerPartRenderDirtyManager();
    ASSERT_NE(dirtyManager, nullptr);
    dirtyManager->SetCurrentFrameDirtyRect(DEFAULT_OLD_RECT);

    RSOpincManager::Instance().CalculateAndUpdateLayerPartRenderDirtyRegion(*node, dirtyManager, DEFAULT_OLD_RECT);

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

    RSOpincManager::Instance().CalculateAndUpdateLayerPartRenderDirtyRegion(*node, dirtyManager, DEFAULT_ABS_RECT);

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
    node->GetOpincCache().SetLayerPartRender(true);
    auto dirtyManager = std::make_shared<RSDirtyRegionManager>();
    ASSERT_NE(dirtyManager, nullptr);

    auto& stagingRenderParams = node->GetStagingRenderParams();
    ASSERT_NE(stagingRenderParams, nullptr);
    stagingRenderParams.reset();
    ASSERT_EQ(node->GetStagingRenderParams(), nullptr);

    RSOpincManager::Instance().CalculateAndUpdateLayerPartRenderDirtyRegion(*node, dirtyManager, DEFAULT_ABS_RECT);

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
    node->GetOpincCache().SetLayerPartRenderNodeStrategyType(NodeStrategyType::CACHE_DISABLE);
    auto dirtyManager = std::make_shared<RSDirtyRegionManager>();
    auto dirtyManagerHold = dirtyManager;
    ASSERT_NE(dirtyManager, nullptr);
    auto& stagingRenderParams = node->GetStagingRenderParams();
    ASSERT_NE(stagingRenderParams, nullptr);
    stagingRenderParams->SetLayerPartRenderEnabled(true);

    RSOpincManager::Instance().CalculateAndUpdateLayerPartRenderDirtyRegion(*node, dirtyManager, DEFAULT_ABS_RECT);

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
    node->GetOpincCache().SetLayerPartRender(true);
    auto dirtyManager = node->GetOpincCache().GetLayerPartRenderDirtyManager();
    ASSERT_NE(dirtyManager, nullptr);
    dirtyManager->SetCurrentFrameDirtyRect(DEFAULT_OLD_RECT);

    RSOpincManager::Instance().CalculateAndUpdateLayerPartRenderDirtyRegion(*node, dirtyManager, DEFAULT_OLD_RECT);

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
 * @tc.name: CalculateLayerPartRenderDirtyRegionFallbackToNodeAbsRect
 * @tc.desc: Verify unchanged node falls back to node abs rect when dirty region is not fully inside node abs rect
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincManagerLayerPartTest, CalculateLayerPartRenderDirtyRegionFallbackToNodeAbsRect, TestSize.Level1)
{
    auto node = CreateCanvasNode(THIRD_NODE_ID);
    node->GetOpincCache().SetLayerPartRender(true);
    node->MarkNodeGroup(RSRenderNode::NodeGroupType::GROUPED_BY_USER, true, false);
    WarmUpLayerPartUnchangeState(node->GetOpincCache());

    auto dirtyManager = node->GetOpincCache().GetLayerPartRenderDirtyManager();
    ASSERT_NE(dirtyManager, nullptr);
    dirtyManager->SetCurrentFrameDirtyRect(DEFAULT_OLD_RECT);

    RSOpincManager::Instance().CalculateAndUpdateLayerPartRenderDirtyRegion(*node, dirtyManager, DEFAULT_OLD_RECT);

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
    auto dirtyManager = std::make_shared<RSDirtyRegionManager>();
    ASSERT_NE(dirtyManager, nullptr);
    auto& stagingRenderParams = node->GetStagingRenderParams();
    ASSERT_NE(stagingRenderParams, nullptr);
    stagingRenderParams->SetLayerPartRenderEnabled(true);

    RSOpincManager::Instance().CalculateAndUpdateLayerPartRenderDirtyRegion(*node, dirtyManager, DEFAULT_ABS_RECT);

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
    node->GetOpincCache().SetLayerPartRender(true);
    node->GetMutableRenderProperties().boundsGeo_ = nullptr;
    auto dirtyManager = std::make_shared<RSDirtyRegionManager>();
    ASSERT_NE(dirtyManager, nullptr);
    auto& stagingRenderParams = node->GetStagingRenderParams();
    ASSERT_NE(stagingRenderParams, nullptr);
    stagingRenderParams->SetLayerPartRenderEnabled(true);

    RSOpincManager::Instance().CalculateAndUpdateLayerPartRenderDirtyRegion(*node, dirtyManager, DEFAULT_ABS_RECT);

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

    RSOpincManager::Instance().CalculateAndUpdateLayerPartRenderDirtyRegion(*node, dirtyManager, DEFAULT_ABS_RECT);

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
    node->GetOpincCache().SetLayerPartRender(true);
    node->MarkNodeGroup(RSRenderNode::NodeGroupType::GROUPED_BY_USER, true, false);
    WarmUpLayerPartUnchangeState(node->GetOpincCache());

    auto dirtyManager = node->GetOpincCache().GetLayerPartRenderDirtyManager();
    ASSERT_NE(dirtyManager, nullptr);
    dirtyManager->SetCurrentFrameDirtyRect(DEFAULT_OUTSIDE_RECT);

    RSOpincManager::Instance().CalculateAndUpdateLayerPartRenderDirtyRegion(*node, dirtyManager,
        DEFAULT_OUTSIDE_RECT);

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
    node->GetOpincCache().SetLayerPartRender(true);
    node->MarkNodeGroup(RSRenderNode::NodeGroupType::GROUPED_BY_USER, true, false);
    WarmUpLayerPartUnchangeState(node->GetOpincCache());

    auto dirtyManager = node->GetOpincCache().GetLayerPartRenderDirtyManager();
    ASSERT_NE(dirtyManager, nullptr);
    dirtyManager->SetCurrentFrameDirtyRect(DEFAULT_INSIDE_RECT);

    RSOpincManager::Instance().CalculateAndUpdateLayerPartRenderDirtyRegion(*node, dirtyManager,
        DEFAULT_INSIDE_RECT);

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
 * @tc.name: CalculateAndUpdateLayerPartRenderDirtyRegionCacheDisableBranch
 * @tc.desc: Verify CACHE_DISABLE branch clears cache state and releases dirty manager
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincManagerLayerPartTest, CalculateAndUpdateLayerPartRenderDirtyRegionCacheDisableBranch, TestSize.Level1)
{
    auto node = CreateCanvasNode(SECOND_NODE_ID);
    node->GetOpincCache().SetLayerPartRenderNodeStrategyType(NodeStrategyType::CACHE_DISABLE);
    auto dirtyManager = std::make_shared<RSDirtyRegionManager>();
    ASSERT_NE(dirtyManager, nullptr);

    auto& stagingRenderParams = node->GetStagingRenderParams();
    ASSERT_NE(stagingRenderParams, nullptr);
    stagingRenderParams->SetLayerPartRenderEnabled(true);

    RSOpincManager::Instance().CalculateAndUpdateLayerPartRenderDirtyRegion(*node, dirtyManager, DEFAULT_ABS_RECT);

    ASSERT_EQ(dirtyManager, nullptr);
    ASSERT_FALSE(stagingRenderParams->GetLayerPartRenderEnabled());
    ASSERT_EQ(node->GetOpincCache().GetLayerPartRenderNodeStrategyType(), NodeStrategyType::CACHE_NONE);
}

/**
 * @tc.name: CalculateAndUpdateLayerPartRenderDirtyRegionCalculateFailedBranch
 * @tc.desc: Verify calculate-failed branch disables layer-part render state
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincManagerLayerPartTest,
    CalculateAndUpdateLayerPartRenderDirtyRegionCalculateFailedBranch, TestSize.Level1)
{
    auto node = CreateCanvasNode(THIRD_NODE_ID);
    node->GetOpincCache().SetLayerPartRender(true);
    node->GetMutableRenderProperties().boundsGeo_ = nullptr;
    auto dirtyManager = std::make_shared<RSDirtyRegionManager>();
    ASSERT_NE(dirtyManager, nullptr);

    auto& stagingRenderParams = node->GetStagingRenderParams();
    ASSERT_NE(stagingRenderParams, nullptr);
    stagingRenderParams->SetLayerPartRenderEnabled(true);

    RSOpincManager::Instance().CalculateAndUpdateLayerPartRenderDirtyRegion(*node, dirtyManager, DEFAULT_ABS_RECT);

    ASSERT_NE(dirtyManager, nullptr);
    ASSERT_FALSE(stagingRenderParams->GetLayerPartRenderEnabled());
}

/**
 * @tc.name: CalculateAndUpdateLayerPartRenderDirtyRegionGroupedByUserFalseBranch
 * @tc.desc: Verify grouped-by-user node skips MarkNodeGroup branch and still updates render params
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincManagerLayerPartTest, CalculateAndUpdateLayerPartRenderDirtyRegionGroupedByUserFalseBranch,
    TestSize.Level1)
{
    auto node = CreateCanvasNode(DEFAULT_NODE_ID);
    node->GetOpincCache().SetLayerPartRender(true);
    node->MarkNodeGroup(RSRenderNode::NodeGroupType::GROUPED_BY_USER, true, false);
    auto dirtyManager = std::make_shared<RSDirtyRegionManager>();
    ASSERT_NE(dirtyManager, nullptr);

    auto& stagingRenderParams = node->GetStagingRenderParams();
    ASSERT_NE(stagingRenderParams, nullptr);
    stagingRenderParams->SetLayerPartRenderEnabled(false);

    RSOpincManager::Instance().CalculateAndUpdateLayerPartRenderDirtyRegion(*node, dirtyManager, DEFAULT_ABS_RECT);

    ASSERT_EQ(node->GetNodeGroupType(), RSRenderNode::NodeGroupType::GROUPED_BY_USER);
    ASSERT_TRUE(stagingRenderParams->GetLayerPartRenderEnabled());
    ASSERT_EQ(dirtyManager, nullptr);
}

/**
 * @tc.name: CalculateAndUpdateLayerPartRenderDirtyRegionCacheDisableCleanup
 * @tc.desc: Verify branch at line 274 enters CACHE_DISABLE cleanup path
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincManagerLayerPartTest, CalculateAndUpdateLayerPartRenderDirtyRegionCacheDisableCleanup, TestSize.Level1)
{
    auto node = CreateCanvasNode(DEFAULT_NODE_ID);
    node->GetOpincCache().SetLayerPartRenderNodeStrategyType(NodeStrategyType::CACHE_DISABLE);
    auto dirtyManager = std::make_shared<RSDirtyRegionManager>();
    ASSERT_NE(dirtyManager, nullptr);

    auto& stagingRenderParams = node->GetStagingRenderParams();
    ASSERT_NE(stagingRenderParams, nullptr);
    stagingRenderParams->SetLayerPartRenderEnabled(true);

    RSOpincManager::Instance().CalculateAndUpdateLayerPartRenderDirtyRegion(*node, dirtyManager, DEFAULT_ABS_RECT);

    ASSERT_EQ(dirtyManager, nullptr);
    ASSERT_FALSE(stagingRenderParams->GetLayerPartRenderEnabled());
    ASSERT_EQ(node->GetOpincCache().GetLayerPartRenderNodeStrategyType(), NodeStrategyType::CACHE_NONE);
}

/**
 * @tc.name: CalculateAndUpdateLayerPartRenderDirtyRegionCalculateFailure
 * @tc.desc: Verify branch at line 286 handles calculate failure and clears layer-part state
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincManagerLayerPartTest, CalculateAndUpdateLayerPartRenderDirtyRegionCalculateFailure, TestSize.Level1)
{
    auto node = CreateCanvasNode(DEFAULT_NODE_ID);
    node->GetOpincCache().SetLayerPartRender(true);
    node->GetMutableRenderProperties().boundsGeo_ = nullptr;
    auto dirtyManager = std::make_shared<RSDirtyRegionManager>();
    ASSERT_NE(dirtyManager, nullptr);

    auto& stagingRenderParams = node->GetStagingRenderParams();
    ASSERT_NE(stagingRenderParams, nullptr);
    stagingRenderParams->SetLayerPartRenderEnabled(true);

    RSOpincManager::Instance().CalculateAndUpdateLayerPartRenderDirtyRegion(*node, dirtyManager, DEFAULT_ABS_RECT);

    ASSERT_NE(dirtyManager, nullptr);
    ASSERT_FALSE(stagingRenderParams->GetLayerPartRenderEnabled());
}

/**
 * @tc.name: CalculateAndUpdateLayerPartRenderDirtyRegionKeepGroupedByUser
 * @tc.desc: Verify branch at line 293 is false when node is already grouped by user
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincManagerLayerPartTest, CalculateAndUpdateLayerPartRenderDirtyRegionKeepGroupedByUser, TestSize.Level1)
{
    auto node = CreateCanvasNode(SECOND_NODE_ID);
    node->GetOpincCache().SetLayerPartRender(true);
    node->MarkNodeGroup(RSRenderNode::NodeGroupType::GROUPED_BY_USER, true, false);
    auto dirtyManager = std::make_shared<RSDirtyRegionManager>();
    ASSERT_NE(dirtyManager, nullptr);

    auto& stagingRenderParams = node->GetStagingRenderParams();
    ASSERT_NE(stagingRenderParams, nullptr);
    stagingRenderParams->SetLayerPartRenderEnabled(false);

    RSOpincManager::Instance().CalculateAndUpdateLayerPartRenderDirtyRegion(*node, dirtyManager, DEFAULT_ABS_RECT);

    ASSERT_EQ(node->GetNodeGroupType(), RSRenderNode::NodeGroupType::GROUPED_BY_USER);
    ASSERT_TRUE(stagingRenderParams->GetLayerPartRenderEnabled());
    ASSERT_EQ(dirtyManager, nullptr);
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
    std::shared_ptr<RSDirtyRegionManager> dirtyManager = nullptr;

    ASSERT_FALSE(node->UpdateLayerPartRenderDirtyRegion(dirtyManager));
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

    ASSERT_TRUE(node->UpdateLayerPartRenderDirtyRegion(dirtyManager));
    ASSERT_TRUE(dirtyManager->GetCurrentFrameDirtyRegion().IsEmpty());
    ASSERT_EQ(node->GetOpincCache().GetLayerPartRenderOldAbsDrawRect(), DEFAULT_ABS_RECT);
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

    ASSERT_TRUE(node->UpdateLayerPartRenderDirtyRegion(dirtyManager));
    ASSERT_FALSE(dirtyManager->GetCurrentFrameDirtyRegion().IsEmpty());
    ASSERT_EQ(node->GetOpincCache().GetLayerPartRenderOldAbsDrawRect(), DEFAULT_ABS_RECT);
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
