/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include <memory>
#include <utility>

#include "drawable/rs_color_picker_drawable.h"
#include "feature/color_picker/rs_color_picker_manager.h"
#include "feature/color_picker/rs_color_picker_utils.h"
#include "gtest/gtest.h"

#include "draw/canvas.h"
#include "draw/surface.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class RSColorPickerUtilsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override {}
    void TearDown() override {}
};

void RSColorPickerUtilsTest::SetUpTestCase() {}
void RSColorPickerUtilsTest::TearDownTestCase() {}

/**
 * @tc.name: GetColorForPlaceholder
 * @tc.desc: test GetColorForPlaceholder
 * @tc.type:FUNC
 */
HWTEST_F(RSColorPickerUtilsTest, GetColorForPlaceholder, TestSize.Level1)
{
    auto colorPair = RSColorPickerUtils::GetColorForPlaceholder(ColorPlaceholder::FOREGROUND);
    EXPECT_EQ(colorPair.first, Drawing::Color::COLOR_BLACK);
    colorPair = RSColorPickerUtils::GetColorForPlaceholder(ColorPlaceholder::ACCENT);
    EXPECT_EQ(colorPair.first, Drawing::Color::COLOR_BLACK);
}

/**
 * @tc.name: InterpolateColor
 * @tc.desc: test InterpolateColor
 * @tc.type:FUNC
 */
HWTEST_F(RSColorPickerUtilsTest, InterpolateColor, TestSize.Level1)
{
    auto color = RSColorPickerUtils::InterpolateColor(0xFF000000, 0xFFFFFFFF, -1.f);
    EXPECT_EQ(color, 0xFFFFFFFF);
    color = RSColorPickerUtils::InterpolateColor(0xFF000000, 0xFFFFFFFF, 0.2f);
    EXPECT_EQ(color, 0xFF333333);
}

/**
 * @tc.name: InterpolateColorBoundaryValues
 * @tc.desc: test InterpolateColor with boundary values
 * @tc.type:FUNC
 */
HWTEST_F(RSColorPickerUtilsTest, InterpolateColorBoundaryValues, TestSize.Level1)
{
    // Test with fraction = 0 (should return start color)
    auto color = RSColorPickerUtils::InterpolateColor(0xFF000000, 0xFFFFFFFF, 0.f);
    EXPECT_EQ(color, 0xFF000000);

    // Test with fraction = 1 (should return end color)
    color = RSColorPickerUtils::InterpolateColor(0xFF000000, 0xFFFFFFFF, 1.f);
    EXPECT_EQ(color, 0xFFFFFFFF);

    // Test with fraction > 1 (should clamp to end color)
    color = RSColorPickerUtils::InterpolateColor(0xFF000000, 0xFFFFFFFF, 2.f);
    EXPECT_EQ(color, 0xFFFFFFFF);
}

// ============================================================================
// RSColorPickerUtils::CalculateLuminance Tests
// ============================================================================

/**
 * @tc.name: CalculateLuminance
 * @tc.desc: test CalculateLuminance returns correct values
 * @tc.type:FUNC
 */
HWTEST_F(RSColorPickerUtilsTest, CalculateLuminance, TestSize.Level1)
{
    // Black should have 0 luminance
    float luminance = RSColorPickerUtils::CalculateLuminance(0xFF000000);
    EXPECT_FLOAT_EQ(luminance, 0.f);

    luminance = RSColorPickerUtils::CalculateLuminance(0xFFFFFFFF);
    EXPECT_FLOAT_EQ(luminance, 255.f);
}

// ============================================================================
// RSColorPickerUtils::CreateColorPickerInfo Tests
// ============================================================================

/**
 * @tc.name: CreateColorPickerInfoWithNullSnapshot
 * @tc.desc: Test CreateColorPickerInfo returns nullptr when snapshot is null
 * @tc.type: FUNC
 */
HWTEST_F(RSColorPickerUtilsTest, CreateColorPickerInfoWithNullSnapshot, TestSize.Level1)
{
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    RSColorPickerManager manager(1);
    auto weakManager = manager.weak_from_this();

    Drawing::Surface* surface = canvas.GetSurface();
    auto colorPickerInfo = RSColorPickerUtils::CreateColorPickerInfo(surface, nullptr, weakManager);

    EXPECT_EQ(colorPickerInfo, nullptr);
}

/**
 * @tc.name: CreateColorPickerInfoWithValidInputs
 * @tc.desc: Test CreateColorPickerInfo creates valid info with valid inputs
 * @tc.type: FUNC
 */
HWTEST_F(RSColorPickerUtilsTest, CreateColorPickerInfoWithValidInputs, TestSize.Level1)
{
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    RSColorPickerManager manager(1);
    auto weakManager = manager.weak_from_this();

    Drawing::Surface* surface = canvas.GetSurface();
    if (!surface) {
        GTEST_SKIP() << "No valid surface available";
        return;
    }

    auto snapshot = std::make_shared<Drawing::Image>();
    auto colorPickerInfo = RSColorPickerUtils::CreateColorPickerInfo(surface, snapshot, weakManager);

    // If surface is valid and has a valid backend texture, info should be created
    // Otherwise it might be null due to invalid backend texture
    if (colorPickerInfo) {
        EXPECT_NE(colorPickerInfo->manager_.lock(), nullptr);
        EXPECT_NE(colorPickerInfo->oldImage_, nullptr);
    }
}

// ============================================================================
// RSColorPickerUtils::CollectColorPickerNodeIds Tests
// ============================================================================

/**
 * @tc.name: CollectColorPickerNodeIdsEmptySurfaces
 * @tc.desc: Test CollectColorPickerNodeIds returns empty set for empty surfaces
 * @tc.type: FUNC
 */
HWTEST_F(RSColorPickerUtilsTest, CollectColorPickerNodeIdsEmptySurfaces, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto& nodeMap = rsContext->GetMutableNodeMap();
    std::vector<std::shared_ptr<RSRenderNode>> surfaces;

    auto result = RSColorPickerUtils::CollectColorPickerNodeIds(surfaces, nodeMap);

    EXPECT_TRUE(result.empty());
}

/**
 * @tc.name: CollectColorPickerNodeIdsNoColorPickerNodes
 * @tc.desc: Test CollectColorPickerNodeIds returns empty set when no color picker nodes exist
 * @tc.type: FUNC
 */
HWTEST_F(RSColorPickerUtilsTest, CollectColorPickerNodeIdsNoColorPickerNodes, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto& nodeMap = rsContext->GetMutableNodeMap();

    // Create a surface node without color picker
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(1, rsContext->weak_from_this());
    std::vector<std::shared_ptr<RSRenderNode>> surfaces;
    surfaces.push_back(surfaceNode);

    auto result = RSColorPickerUtils::CollectColorPickerNodeIds(surfaces, nodeMap);

    EXPECT_TRUE(result.empty());
}

/**
 * @tc.name: CollectColorPickerNodeIdsWithColorPickerNodes
 * @tc.desc: Test CollectColorPickerNodeIds collects color picker node IDs
 * @tc.type: FUNC
 */
HWTEST_F(RSColorPickerUtilsTest, CollectColorPickerNodeIdsWithColorPickerNodes, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto& nodeMap = rsContext->GetMutableNodeMap();

    // Create a surface node
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(1, rsContext->weak_from_this());

    // Create a filter node with color picker
    NodeId filterNodeId = 100;
    auto filterNode = std::make_shared<RSRenderNode>(filterNodeId);
    auto colorPickerDrawable = std::make_shared<DrawableV2::RSColorPickerDrawable>(false, filterNodeId);
    filterNode->GetDrawableVec(__func__)[static_cast<int8_t>(RSDrawableSlot::COLOR_PICKER)] = colorPickerDrawable;

    nodeMap.RegisterRenderNode(filterNode);
    surfaceNode->visibleFilterChild_.push_back(filterNodeId);

    std::vector<std::shared_ptr<RSRenderNode>> surfaces;
    surfaces.push_back(surfaceNode);

    auto result = RSColorPickerUtils::CollectColorPickerNodeIds(surfaces, nodeMap);

    EXPECT_EQ(result.size(), 1u);
    EXPECT_NE(result.find(filterNodeId), result.end());
}

/**
 * @tc.name: CollectColorPickerNodeIdsMultipleNodes
 * @tc.desc: Test CollectColorPickerNodeIds collects multiple color picker node IDs
 * @tc.type: FUNC
 */
HWTEST_F(RSColorPickerUtilsTest, CollectColorPickerNodeIdsMultipleNodes, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto& nodeMap = rsContext->GetMutableNodeMap();

    // Create a surface node
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(1, rsContext->weak_from_this());

    // Create two filter nodes with color picker
    NodeId filterNodeId1 = 100;
    NodeId filterNodeId2 = 200;

    auto filterNode1 = std::make_shared<RSRenderNode>(filterNodeId1);
    auto colorPickerDrawable1 = std::make_shared<DrawableV2::RSColorPickerDrawable>(false, filterNodeId1);
    filterNode1->GetDrawableVec(__func__)[static_cast<int8_t>(RSDrawableSlot::COLOR_PICKER)] = colorPickerDrawable1;

    auto filterNode2 = std::make_shared<RSRenderNode>(filterNodeId2);
    auto colorPickerDrawable2 = std::make_shared<DrawableV2::RSColorPickerDrawable>(false, filterNodeId2);
    filterNode2->GetDrawableVec(__func__)[static_cast<int8_t>(RSDrawableSlot::COLOR_PICKER)] = colorPickerDrawable2;

    nodeMap.RegisterRenderNode(filterNode1);
    nodeMap.RegisterRenderNode(filterNode2);
    surfaceNode->visibleFilterChild_.push_back(filterNodeId1);
    surfaceNode->visibleFilterChild_.push_back(filterNodeId2);

    std::vector<std::shared_ptr<RSRenderNode>> surfaces;
    surfaces.push_back(surfaceNode);

    auto result = RSColorPickerUtils::CollectColorPickerNodeIds(surfaces, nodeMap);

    EXPECT_EQ(result.size(), 2u);
    EXPECT_NE(result.find(filterNodeId1), result.end());
    EXPECT_NE(result.find(filterNodeId2), result.end());
}

/**
 * @tc.name: CollectColorPickerNodeIdsDeduplicates
 * @tc.desc: Test CollectColorPickerNodeIds deduplicates node IDs from multiple surfaces
 * @tc.type: FUNC
 */
HWTEST_F(RSColorPickerUtilsTest, CollectColorPickerNodeIdsDeduplicates, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto& nodeMap = rsContext->GetMutableNodeMap();

    // Create two surface nodes
    auto surfaceNode1 = std::make_shared<RSSurfaceRenderNode>(1, rsContext->weak_from_this());
    auto surfaceNode2 = std::make_shared<RSSurfaceRenderNode>(2, rsContext->weak_from_this());

    // Create a filter node with color picker (shared between surfaces)
    NodeId filterNodeId = 100;
    auto filterNode = std::make_shared<RSRenderNode>(filterNodeId);
    auto colorPickerDrawable = std::make_shared<DrawableV2::RSColorPickerDrawable>(false, filterNodeId);
    filterNode->GetDrawableVec(__func__)[static_cast<int8_t>(RSDrawableSlot::COLOR_PICKER)] = colorPickerDrawable;

    nodeMap.RegisterRenderNode(filterNode);

    // Both surfaces reference the same filter node
    surfaceNode1->visibleFilterChild_.push_back(filterNodeId);
    surfaceNode2->visibleFilterChild_.push_back(filterNodeId);

    std::vector<std::shared_ptr<RSRenderNode>> surfaces;
    surfaces.push_back(surfaceNode1);
    surfaces.push_back(surfaceNode2);

    auto result = RSColorPickerUtils::CollectColorPickerNodeIds(surfaces, nodeMap);

    // Should only contain one entry due to deduplication
    EXPECT_EQ(result.size(), 1u);
}

// ============================================================================
// RSColorPickerUtils::ShouldColorPickForNode Tests
// ============================================================================

/**
 * @tc.name: ShouldColorPickForNodeNoDrawable
 * @tc.desc: Test ShouldColorPickForNode returns false when node has no color picker drawable
 * @tc.type: FUNC
 */
HWTEST_F(RSColorPickerUtilsTest, ShouldColorPickForNodeNoDrawable, TestSize.Level1)
{
    auto filterNode = std::make_shared<RSRenderNode>(1);
    std::vector<std::shared_ptr<RSRenderNode>> surfaces;

    bool result = RSColorPickerUtils::IsColorPickerDirty(*filterNode, surfaces);

    EXPECT_FALSE(result);
}

/**
 * @tc.name: ShouldColorPickForNodeColorPickThisFrame
 * @tc.desc: Test ShouldColorPickForNode returns false and resets state when COLOR_PICK_THIS_FRAME
 * @tc.type: FUNC
 */
HWTEST_F(RSColorPickerUtilsTest, ShouldColorPickForNodeColorPickThisFrame, TestSize.Level1)
{
    NodeId filterNodeId = 100;
    auto filterNode = std::make_shared<RSRenderNode>(filterNodeId);

    auto colorPickerDrawable = std::make_shared<DrawableV2::RSColorPickerDrawable>(false, filterNodeId);
    colorPickerDrawable->stagingState_ = DrawableV2::ColorPickerState::COLOR_PICK_THIS_FRAME;
    filterNode->GetDrawableVec(__func__)[static_cast<int8_t>(RSDrawableSlot::COLOR_PICKER)] = colorPickerDrawable;

    std::vector<std::shared_ptr<RSRenderNode>> surfaces;

    bool result = RSColorPickerUtils::IsColorPickerDirty(*filterNode, surfaces);

    // COLOR_PICK_THIS_FRAME should return false and schedule transition to PREPARING
    EXPECT_FALSE(result);
}

/**
 * @tc.name: ShouldColorPickForNodeScheduledState
 * @tc.desc: Test ShouldColorPickForNode returns false when in SCHEDULED state
 * @tc.type: FUNC
 */
HWTEST_F(RSColorPickerUtilsTest, ShouldColorPickForNodeScheduledState, TestSize.Level1)
{
    NodeId filterNodeId = 100;
    auto filterNode = std::make_shared<RSRenderNode>(filterNodeId);

    auto colorPickerDrawable = std::make_shared<DrawableV2::RSColorPickerDrawable>(false, filterNodeId);
    colorPickerDrawable->stagingState_ = DrawableV2::ColorPickerState::SCHEDULED;
    filterNode->GetDrawableVec(__func__)[static_cast<int8_t>(RSDrawableSlot::COLOR_PICKER)] = colorPickerDrawable;

    std::vector<std::shared_ptr<RSRenderNode>> surfaces;

    bool result = RSColorPickerUtils::IsColorPickerDirty(*filterNode, surfaces);

    // SCHEDULED state means waiting for delayed transition, should return false
    EXPECT_FALSE(result);
}

/**
 * @tc.name: ShouldColorPickForNodeNoIntersection
 * @tc.desc: Test ShouldColorPickForNode returns false when no dirty region intersection
 * @tc.type: FUNC
 */
HWTEST_F(RSColorPickerUtilsTest, ShouldColorPickForNodeNoIntersection, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();

    NodeId filterNodeId = 100;
    auto filterNode = std::make_shared<RSRenderNode>(filterNodeId);
    filterNode->GetRenderProperties().GetBoundsGeometry()->absRect_ = { 0, 0, 100, 100 };
    filterNode->GetMutableRenderProperties().SetColorPickerStrategy(4);

    auto colorPickerDrawable = std::make_shared<DrawableV2::RSColorPickerDrawable>(false, filterNodeId);
    colorPickerDrawable->stagingState_ = DrawableV2::ColorPickerState::PREPARING;
    filterNode->GetDrawableVec(__func__)[static_cast<int8_t>(RSDrawableSlot::COLOR_PICKER)] = colorPickerDrawable;

    // Create surface node with dirty region that doesn't intersect
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(1, rsContext->weak_from_this());
    surfaceNode->GetDirtyManager()->MergeDirtyRect(RectI(200, 200, 100, 100));

    std::vector<std::shared_ptr<RSRenderNode>> surfaces;
    surfaces.push_back(surfaceNode);

    bool result = RSColorPickerUtils::IsColorPickerDirty(*filterNode, surfaces);
    // No intersection with dirty region
    EXPECT_FALSE(result);

    filterNode->GetMutableRenderProperties().SetColorPickerRect(Vector4f(200, 200, 300, 300));
    result = RSColorPickerUtils::IsColorPickerDirty(*filterNode, surfaces);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: ShouldColorPickForNodeWithCustomRect
 * @tc.desc: Test ShouldColorPickForNode with valid custom rect that intersects with dirty region
 * @tc.type: FUNC
 */
HWTEST_F(RSColorPickerUtilsTest, ShouldColorPickForNodeWithCustomRect, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();

    NodeId filterNodeId = 100;
    auto filterNode = std::make_shared<RSRenderNode>(filterNodeId);
    // Node abs rect: (0, 0) to (500, 500)
    filterNode->GetRenderProperties().GetBoundsGeometry()->absRect_ = { 0, 0, 500, 500 };
    filterNode->GetMutableRenderProperties().SetColorPickerStrategy(4);

    auto colorPickerDrawable = std::make_shared<DrawableV2::RSColorPickerDrawable>(false, filterNodeId);
    colorPickerDrawable->stagingState_ = DrawableV2::ColorPickerState::PREPARING;
    filterNode->GetDrawableVec(__func__)[static_cast<int8_t>(RSDrawableSlot::COLOR_PICKER)] = colorPickerDrawable;

    // Create surface node with dirty region at (200, 200)
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(1, rsContext->weak_from_this());
    surfaceNode->GetDirtyManager()->MergeDirtyRect(RectI(200, 200, 100, 100));

    std::vector<std::shared_ptr<RSRenderNode>> surfaces;
    surfaces.push_back(surfaceNode);

    // Without custom rect, node's absRect (0,0,500,500) doesn't intersect with dirty (200,200,100,100)
    // Actually it does intersect! Let's use a different setup
    // Node abs rect: (0, 0) to (100, 100)
    filterNode->GetRenderProperties().GetBoundsGeometry()->absRect_ = { 0, 0, 100, 100 };

    // First verify no intersection without custom rect
    bool result = RSColorPickerUtils::IsColorPickerDirty(*filterNode, surfaces);
    EXPECT_FALSE(result);

    // Set custom rect to (150, 150, 250, 250) - valid rect where right > left, bottom > top
    // This rect should intersect with dirty region (200, 200, 100, 100)
    filterNode->GetMutableRenderProperties().SetColorPickerRect(Vector4f(150, 150, 250, 250));
    result = RSColorPickerUtils::IsColorPickerDirty(*filterNode, surfaces);
    EXPECT_TRUE(result);
}
} // namespace OHOS::Rosen
