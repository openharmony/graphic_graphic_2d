/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "gmock/gmock.h"
#include <gtest/gtest.h>

#include "common/rs_common_def.h"
#include "common/rs_obj_abs_geometry.h"
#include "dirty_region/rs_optimize_canvas_dirty_collector.h"
#include "drawable/rs_color_picker_drawable.h"
#include "drawable/rs_property_drawable.h"
#include "drawable/rs_property_drawable_background.h"
#include "drawable/rs_property_drawable_foreground.h"
#include "modifier_ng/custom/rs_custom_modifier.h"
#include "offscreen_render/rs_offscreen_render_thread.h"
#include "params/rs_render_params.h"
#include "pipeline/rs_canvas_drawing_render_node.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_dirty_region_manager.h"
#include "pipeline/rs_logical_display_render_node.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_screen_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "render/rs_filter.h"
#include "skia_adapter/skia_canvas.h"
#include "parameters.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
const std::string OUT_STR1 =
    "DISPLAY_NODERS_NODESURFACE_NODECANVAS_NODEROOT_NODEPROXY_NODECANVAS_DRAWING_NODEEFFECT_NODEUNKNOWN_NODE";
const std::string OUT_STR2 =
    "| RS_NODE[0], instanceRootNodeId[0], SharedTransitionParam: [0 -> 0], [nodeGroup1], uifirstRootNodeId_: 1, "
    "Properties: Bounds[-inf -inf -inf -inf] Frame[-inf -inf -inf -inf], NodeColorSpace: 4, "
    "RSUIContextToken: NO_RSUIContext, "
    "GetBootAnimation: true, isContainBootAnimation: true, isNodeDirty: 1, isPropertyDirty: true, "
    "isSubTreeDirty: true, IsPureContainer: true, Children list needs update, current count: 0 expected count: 0, "
    "disappearingChildren: 1(0 )\n  | RS_NODE[0], instanceRootNodeId[0], Properties: Bounds[-inf -inf -inf -inf] "
    "Frame[-inf -inf -inf -inf], NodeColorSpace: 4, RSUIContextToken: NO_RSUIContext, IsPureContainer: true\n";
const int DEFAULT_BOUNDS_SIZE = 10;
const int DEFAULT_NODE_ID = 1;
const NodeId TARGET_NODE_ID = 9999999999;
const NodeId INVALID_NODE_ID = 99999999999;
class RSRenderNodeDrawableAdapterBoy : public DrawableV2::RSRenderNodeDrawableAdapter {
public:
    explicit RSRenderNodeDrawableAdapterBoy(std::shared_ptr<const RSRenderNode> node)
        : RSRenderNodeDrawableAdapter(std::move(node))
    {
        renderParams_ = std::make_unique<RSRenderParams>(renderNode_.lock()->GetId());
        uifirstRenderParams_ = std::make_unique<RSRenderParams>(renderNode_.lock()->GetId());
    }
    ~RSRenderNodeDrawableAdapterBoy() override = default;

    void Draw(Drawing::Canvas& canvas) override
    {
        printf("Draw:GetRecordingState: %d \n", canvas.GetRecordingState());
    }
};

class RSRenderNodeTest : public testing::Test {
public:
    constexpr static float floatData[] = {
        0.0f,
        485.44f,
        -34.4f,
        std::numeric_limits<float>::max(),
        std::numeric_limits<float>::min(),
    };
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline NodeId id;
    static inline std::weak_ptr<RSContext> context = {};
    static inline RSPaintFilterCanvas* canvas_;
    static inline Drawing::Canvas drawingCanvas_;
};

class DrawableTest : public RSDrawable {
public:
    bool OnUpdate(const RSRenderNode& content) override
    {
        return true;
    }
    void OnDraw(Drawing::Canvas* canvas, const Drawing::Rect* rect) const override {}
    void OnSync() override
    {
        return;
    }
};

void RSRenderNodeTest::SetUpTestCase()
{
    canvas_ = new RSPaintFilterCanvas(&drawingCanvas_);
}
void RSRenderNodeTest::TearDownTestCase()
{
    delete canvas_;
    canvas_ = nullptr;
}
void RSRenderNodeTest::SetUp() {}
void RSRenderNodeTest::TearDown() {}

/**
 * @tc.name: UpdateRenderStatus001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeTest, UpdateRenderStatus001, TestSize.Level1)
{
    RSSurfaceRenderNode node(id, context);
    ASSERT_TRUE(node.IsNodeDirty());
    RectI dirtyRegion;
    bool isPartialRenderEnabled = false;
    node.UpdateRenderStatus(dirtyRegion, isPartialRenderEnabled);
}

/**
 * @tc.name: UpdateParentChildrenRectTest
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeTest, UpdateParentChildrenRectTest, TestSize.Level1)
{
    RSRenderNode node(id, context);
    std::shared_ptr<RSBaseRenderNode> parentNode;
    node.UpdateParentChildrenRect(parentNode);
    RSBaseRenderNode rsBaseRenderNode(id, context);
    ASSERT_FALSE(rsBaseRenderNode.HasChildrenOutOfRect());
}

/**
 * @tc.name: ProcessTransitionBeforeChildrenTest
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeTest, ProcessTransitionBeforeChildrenTest, TestSize.Level1)
{
    RSRenderNode node(id, context);
    node.ProcessTransitionBeforeChildren(*canvas_);
    ASSERT_TRUE(canvas_->GetRecordingCanvas() == nullptr);
}

/**
 * @tc.name: AddModifierTest001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeTest, AddModifierTest001, TestSize.Level1)
{
    std::shared_ptr<ModifierNG::RSRenderModifier> modifier = nullptr;
    RSRenderNode node(id, context);
    node.AddModifier(modifier);
    ASSERT_FALSE(node.IsDirty());
}

/**
 * @tc.name: AddModifierTest002
 * @tc.desc: test AddModifierTest
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeTest, AddModifierTest002, TestSize.Level1)
{
    std::shared_ptr<Drawing::DrawCmdList> drawCmdList = nullptr;
    auto property = std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>();
    property->GetRef() = drawCmdList;
    ModifierId id = 1;
    auto modifier = ModifierNG::RSRenderModifier::MakeRenderModifier(
        ModifierNG::RSModifierType::BOUNDS, property, id, ModifierNG::RSPropertyType::BOUNDS);
    auto modifier1 = ModifierNG::RSRenderModifier::MakeRenderModifier(
        ModifierNG::RSModifierType::FRAME, property, ++id, ModifierNG::RSPropertyType::FRAME);
    RSRenderNode node(id, context);
    node.AddModifier(modifier);
    node.AddModifier(modifier1);
    ASSERT_TRUE(node.IsDirty());
}

/**
 * @tc.name: GetPropertyTest
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeTest, GetPropertyTest, TestSize.Level1)
{
    RSRenderNode node(id, context);
    auto rsRenderPropertyBase = node.GetProperty(id);
    EXPECT_EQ(rsRenderPropertyBase, nullptr);
}

/**
 * @tc.name: SetDrawingCacheTypeTest
 * @tc.desc: test SetDrawingCacheType for all drawing cache types
 * @tc.type: FUNC
 * @tc.require: issueI84LBZ
 */
HWTEST_F(RSRenderNodeTest, SetDrawingCacheTypeTest, TestSize.Level2)
{
    RSRenderNode node(id, context);
    node.SetDrawingCacheType(RSDrawingCacheType::DISABLED_CACHE);
    ASSERT_EQ(node.GetDrawingCacheType(), RSDrawingCacheType::DISABLED_CACHE);
    node.SetDrawingCacheType(RSDrawingCacheType::FORCED_CACHE);
    ASSERT_EQ(node.GetDrawingCacheType(), RSDrawingCacheType::FORCED_CACHE);
    node.SetDrawingCacheType(RSDrawingCacheType::TARGETED_CACHE);
    ASSERT_EQ(node.GetDrawingCacheType(), RSDrawingCacheType::TARGETED_CACHE);
}

/**
 * @tc.name: SetDrawNodeType
 * @tc.desc: test results of SetDrawNodeType
 * @tc.type: FUNC
 * @tc.require: IC8BLE
 */
HWTEST_F(RSRenderNodeTest, SetDrawNodeType001, TestSize.Level1)
{
    RSRenderNode rsNode(id, context);
    rsNode.SetDrawNodeType(DrawNodeType::PureContainerType);
    ASSERT_EQ(rsNode.drawNodeType_, DrawNodeType::PureContainerType);
}

/**
 * @tc.name: GetDrawNodeType
 * @tc.desc: test results of GetDrawNodeType
 * @tc.type: FUNC
 * @tc.require: IC8BLE
 */
HWTEST_F(RSRenderNodeTest, SetDrawNodeType002, TestSize.Level1)
{
    RSRenderNode rsNode(id, context);
    rsNode.SetDrawNodeType(DrawNodeType::GeometryPropertyType);
    ASSERT_EQ(rsNode.GetDrawNodeType(), DrawNodeType::GeometryPropertyType);
}

/**
 * @tc.name: ManageDrawingCacheTest001
 * @tc.desc: SetDrawingCacheChanged test
 * @tc.type: FUNC
 * @tc.require: issueI9US6V
 */
HWTEST_F(RSRenderNodeTest, ManageDrawingCacheTest001, TestSize.Level2)
{
    // SetDrawingCacheChanged test
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);
    std::unique_ptr<RSRenderParams> stagingRenderParams = std::make_unique<RSRenderParams>(0);
    EXPECT_NE(stagingRenderParams, nullptr);
    nodeTest->stagingRenderParams_ = std::move(stagingRenderParams);

    nodeTest->lastFrameSynced_ = true;
    nodeTest->stagingRenderParams_->needSync_ = false;
    nodeTest->stagingRenderParams_->isDrawingCacheChanged_ = true;
    nodeTest->SetDrawingCacheChanged(true);
    EXPECT_TRUE(nodeTest->stagingRenderParams_->needSync_);

    nodeTest->SetDrawingCacheChanged(false);
    EXPECT_FALSE(nodeTest->stagingRenderParams_->isDrawingCacheChanged_);

    nodeTest->lastFrameSynced_ = false;
    nodeTest->stagingRenderParams_->needSync_ = false;
    nodeTest->SetDrawingCacheChanged(true);
    EXPECT_TRUE(nodeTest->stagingRenderParams_->needSync_);
    EXPECT_TRUE(nodeTest->stagingRenderParams_->isDrawingCacheChanged_);

    // GetDrawingCacheChanged test
    EXPECT_TRUE(nodeTest->GetDrawingCacheChanged());
}

/**
 * @tc.name: SetGeoUpdateDelay01
 * @tc.desc: test SetGeoUpdateDelay once
 * @tc.type: FUNC
 * @tc.require: issueI8JMN8
 */
HWTEST_F(RSRenderNodeTest,  SetGeoUpdateDelay01, TestSize.Level2)
{
    RSRenderNode node(id, context);
    // test default value
    ASSERT_EQ(node.GetGeoUpdateDelay(), false);

    node.SetGeoUpdateDelay(true);
    ASSERT_EQ(node.GetGeoUpdateDelay(), true);
}

/**
 * @tc.name: SetGeoUpdateDelay02
 * @tc.desc: test SetGeoUpdateDelay would not be covered by later setting
 * @tc.type: FUNC
 * @tc.require: issueI8JMN8
 */
HWTEST_F(RSRenderNodeTest,  SetGeoUpdateDelay02, TestSize.Level2)
{
    RSRenderNode node(id, context);
    node.SetGeoUpdateDelay(true);
    node.SetGeoUpdateDelay(false);
    ASSERT_EQ(node.GetGeoUpdateDelay(), true);
}

/**
 * @tc.name: ResetGeoUpdateDelay01
 * @tc.desc: test SetGeoUpdateDelay would be reset
 * @tc.type: FUNC
 * @tc.require: issueI8JMN8
 */
HWTEST_F(RSRenderNodeTest,  ResetGeoUpdateDelay01, TestSize.Level2)
{
    RSRenderNode node(id, context);
    node.SetGeoUpdateDelay(true);
    node.ResetGeoUpdateDelay();
    ASSERT_EQ(node.GetGeoUpdateDelay(), false);
}

/**
 * @tc.name: SetBootAnimationTest
 * @tc.desc: SetBootAnimation and GetBootAnimation
 * @tc.type:FUNC
 * @tc.require:SR000HSUII
 */
HWTEST_F(RSRenderNodeTest, SetBootAnimationTest, TestSize.Level1)
{
    RSRenderNode node(id, context);
    node.SetBootAnimation(true);
    ASSERT_EQ(node.GetBootAnimation(), true);
    node.SetBootAnimation(false);
    ASSERT_FALSE(node.GetBootAnimation());

    RSDisplayNodeConfig config;
    NodeId nodeId = 2;
    auto logicalDisplayNode = std::make_shared<RSLogicalDisplayRenderNode>(nodeId, config);
    auto rsRenderNode = std::static_pointer_cast<RSRenderNode>(logicalDisplayNode);
    auto tempRenderNode = std::make_shared<RSRenderNode>(1);
    rsRenderNode->parent_ = tempRenderNode;
    rsRenderNode->SetBootAnimation(false);
}

/**
 * @tc.name: OnlyBasicGeoTransfromTest01
 * @tc.desc: Check node only contains BasicGeoTransfrom by default
 * @tc.type: FUNC
 * @tc.require: issueI8IXTX
 */
HWTEST_F(RSRenderNodeTest, OnlyBasicGeoTransfromTest01, TestSize.Level1)
{
    RSRenderNode node(id, context);
    ASSERT_EQ(node.IsOnlyBasicGeoTransform(), true);
}

/**
 * @tc.name: OnlyBasicGeoTransfromTest02
 * @tc.desc: Check node contains more than BasicGeoTransfrom if content dirty
 * @tc.type: FUNC
 * @tc.require: issueI8IXTX
 */
HWTEST_F(RSRenderNodeTest, OnlyBasicGeoTransfromTest02, TestSize.Level1)
{
    RSRenderNode node(id, context);
    node.SetContentDirty();
    ASSERT_EQ(node.IsContentDirty(), true);
    ASSERT_EQ(node.IsOnlyBasicGeoTransform(), false);
}

/**
 * @tc.name: OnlyBasicGeoTransfromTest03
 * @tc.desc: Check node resets BasicGeoTransfrom as true
 * @tc.type: FUNC
 * @tc.require: issueI8KEUU
 */
HWTEST_F(RSRenderNodeTest, OnlyBasicGeoTransfromTest03, TestSize.Level1)
{
    RSRenderNode node(id, context);
    node.SetContentDirty();
    ASSERT_EQ(node.IsOnlyBasicGeoTransform(), false);
    node.ResetIsOnlyBasicGeoTransform();
    ASSERT_EQ(node.IsOnlyBasicGeoTransform(), true);
}

/**
 * @tc.name: UpdateDirtyRegionTest01
 * @tc.desc: check dirty region not join when not dirty
 * @tc.type: FUNC
 * @tc.require: issueI97LXT
 */
HWTEST_F(RSRenderNodeTest, UpdateDirtyRegionTest01, TestSize.Level1)
{
    RSRenderNode node(id, context);
    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    RectI clipRect{0, 0, 1000, 1000};
    bool geoDirty = false;
    node.UpdateDirtyRegion(*rsDirtyManager, geoDirty, clipRect);
    ASSERT_EQ(rsDirtyManager->GetCurrentFrameDirtyRegion().IsEmpty(), true);
}

/**
 * @tc.name: UpdateDirtyRegionTest02
 * @tc.desc: check dirty region not join when not geometry dirty
 * @tc.type: FUNC
 * @tc.require: issueI97LXT
 */
HWTEST_F(RSRenderNodeTest, UpdateDirtyRegionTest02, TestSize.Level1)
{
    RSRenderNode node(id, context);
    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    RectI clipRect{0, 0, 1000, 1000};
    node.SetDirty();
    node.geometryChangeNotPerceived_ = false;
    bool geoDirty = false;
    node.UpdateDirtyRegion(*rsDirtyManager, geoDirty, clipRect);
    ASSERT_EQ(rsDirtyManager->GetCurrentFrameDirtyRegion().IsEmpty(), true);
}

/**
 * @tc.name: UpdateDirtyRegionTest03
 * @tc.desc: check dirty region add successfully
 * @tc.type: FUNC
 * @tc.require: issueI97LXT
 */
HWTEST_F(RSRenderNodeTest, UpdateDirtyRegionTest03, TestSize.Level1)
{
    RSRenderNode node(id, context);
    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    rsDirtyManager->SetSurfaceSize(1000, 1000);
    RectI clipRect{0, 0, 1000, 1000};
    node.SetDirty();
    node.shouldPaint_ = true;
    RectI absRect = RectI{0, 0, 100, 100};
    auto& properties = node.GetMutableRenderProperties();
    properties.boundsGeo_->absRect_ = absRect;
    properties.clipToBounds_ = true;
    bool geoDirty = true;
    node.UpdateDirtyRegion(*rsDirtyManager, geoDirty, clipRect);
    bool isDirtyRectCorrect = (rsDirtyManager->GetCurrentFrameDirtyRegion() == absRect);
    ASSERT_EQ(isDirtyRectCorrect, true);
}

/**
 * @tc.name: UpdateDirtyRegionTest04
 * @tc.desc: check shadow dirty region add successfully
 * @tc.type: FUNC
 * @tc.require: issueI97LXT
 */
HWTEST_F(RSRenderNodeTest, UpdateDirtyRegionTest04, TestSize.Level1)
{
    RSRenderNode node(id, context);
    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    rsDirtyManager->SetSurfaceSize(1000, 1000);
    RectI clipRect{0, 0, 1000, 1000};
    node.SetDirty();
    node.shouldPaint_ = true;
    RectI absRect = RectI{0, 0, 100, 100};
    auto& properties = node.GetMutableRenderProperties();
    properties.boundsGeo_->absRect_ = absRect;
    properties.clipToBounds_ = true;
    properties.SetShadowOffsetX(10.0f);
    properties.SetShadowOffsetY(10.0f);
    properties.SetShadowRadius(10.0f);
    bool geoDirty = true;
    node.UpdateDirtyRegion(*rsDirtyManager, geoDirty, clipRect);
    ASSERT_EQ(rsDirtyManager->GetCurrentFrameDirtyRegion().IsEmpty(), false);
}

/**
 * @tc.name: UpdateDirtyRegionTest05
 * @tc.desc: check outline dirty region add successfully
 * @tc.type: FUNC
 * @tc.require: issueI97LXT
 */
HWTEST_F(RSRenderNodeTest, UpdateDirtyRegionTest05, TestSize.Level1)
{
    RSRenderNode node(id, context);
    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    rsDirtyManager->SetSurfaceSize(1000, 1000);
    RectI clipRect{0, 0, 1000, 1000};
    node.SetDirty();
    node.shouldPaint_ = true;
    RectI absRect = RectI{0, 0, 100, 100};
    auto& properties = node.GetMutableRenderProperties();
    properties.boundsGeo_->absRect_ = absRect;
    properties.clipToBounds_ = true;
    properties.SetOutlineWidth(10.0f);
    properties.SetOutlineStyle((uint32_t)BorderStyle::SOLID);
    properties.SetOutlineColor(RSColor(UINT8_MAX, UINT8_MAX, UINT8_MAX, UINT8_MAX));
    bool geoDirty = true;
    node.UpdateDirtyRegion(*rsDirtyManager, geoDirty, clipRect);
    ASSERT_EQ(rsDirtyManager->GetCurrentFrameDirtyRegion().IsEmpty(), false);
}

/**
 * @tc.name: UpdateDirtyRegionTest06
 * @tc.desc: check outline dirty region add successfully
 * @tc.type: FUNC
 * @tc.require: issueI97LXT
 */
HWTEST_F(RSRenderNodeTest, UpdateDirtyRegionTest06, TestSize.Level1)
{
    //test RSRenderNode
    RSRenderNode node(id, context);
    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    //set dirty surfacesize
    rsDirtyManager->SetSurfaceSize(500, 500);
    rsDirtyManager->MarkAsTargetForDfx();
    //create clip 500*500
    RectI clipRect{0, 0, 500, 500};
    node.SetDirty();
    node.shouldPaint_ = true;
    RectI absRect = RectI{0, 0, 50, 50};
    auto& properties = node.GetMutableRenderProperties();
    properties.boundsGeo_->absRect_ = absRect;
    properties.clipToBounds_ = true;
    //set border width 5
    properties.SetOutlineWidth(5.0f);
    properties.SetOutlineStyle((uint32_t)BorderStyle::SOLID);
    properties.SetOutlineColor(RSColor(UINT8_MAX, UINT8_MAX, UINT8_MAX, UINT8_MAX));
    bool geoDirty = true;
    node.UpdateDirtyRegion(*rsDirtyManager, geoDirty, clipRect);
    ASSERT_EQ(rsDirtyManager->GetCurrentFrameDirtyRegion().IsEmpty(), false);
}

/**
 * @tc.name: UpdateDirtyRegionTest07
 * @tc.desc: check outline dirty region add successfully
 * @tc.type: FUNC
 * @tc.require: issueI97LXT
 */
HWTEST_F(RSRenderNodeTest, UpdateDirtyRegionTest07, TestSize.Level1)
{
    //test RSCanvasRenderNode
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(DEFAULT_NODE_ID, context);
    canvasNode->InitRenderParams();
    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    //set dirty surfacesize
    rsDirtyManager->SetSurfaceSize(800, 800);
    rsDirtyManager->MarkAsTargetForDfx();
    //create clip 800*800
    RectI clipRect{0, 0, 800, 800};
    canvasNode->SetDirty();
    canvasNode->shouldPaint_ = true;
    RectI absRect = RectI{0, 0, 100, 100};
    auto& properties = canvasNode->GetMutableRenderProperties();
    properties.boundsGeo_->absRect_ = absRect;
    properties.clipToBounds_ = true;
    //set border width 8
    properties.SetOutlineWidth(8.0f);
    properties.SetOutlineStyle((uint32_t)BorderStyle::SOLID);
    properties.SetOutlineColor(RSColor(UINT8_MAX, UINT8_MAX, UINT8_MAX, UINT8_MAX));
    bool geoDirty = true;
    canvasNode->UpdateDirtyRegion(*rsDirtyManager, geoDirty, clipRect);
    ASSERT_EQ(rsDirtyManager->GetCurrentFrameDirtyRegion().IsEmpty(), false);
}

/**
 * @tc.name: UpdateDirtyRegionTest08
 * @tc.desc: check outline dirty region add successfully
 * @tc.type: FUNC
 * @tc.require: issueI97LXT
 */
HWTEST_F(RSRenderNodeTest, UpdateDirtyRegionTest08, TestSize.Level1)
{
    //test RSSurfaceRenderNode
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    //set dirty surfacesize
    rsDirtyManager->SetSurfaceSize(1000, 1000);
    rsDirtyManager->MarkAsTargetForDfx();
    //create clip 1000*1000
    RectI clipRect{0, 0, 1000, 1000};
    surfaceNode->SetDirty();
    surfaceNode->shouldPaint_ = true;
    RectI absRect = RectI{0, 0, 100, 100};
    auto& properties = surfaceNode->GetMutableRenderProperties();
    properties.boundsGeo_->absRect_ = absRect;
    properties.clipToBounds_ = true;
    //set border width 10
    properties.SetOutlineWidth(10.0f);
    properties.SetOutlineStyle((uint32_t)BorderStyle::SOLID);
    properties.SetOutlineColor(RSColor(UINT8_MAX, UINT8_MAX, UINT8_MAX, UINT8_MAX));
    bool geoDirty = true;
    surfaceNode->UpdateDirtyRegion(*rsDirtyManager, geoDirty, clipRect);
    ASSERT_EQ(rsDirtyManager->GetCurrentFrameDirtyRegion().IsEmpty(), false);
}

/**
 * @tc.name: GetFilterRectTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest, GetFilterRectTest, TestSize.Level1)
{
    RSRenderNode node(id, context);
    auto rect = node.GetFilterRect();
    EXPECT_TRUE(rect.ToString().compare("[0, 0, 0, 0]") == 0);

    std::shared_ptr<RSPath> rsPath = std::make_shared<RSPath>();
    node.renderProperties_.SetClipBounds(rsPath);
    auto rect1 = node.GetFilterRect();
    EXPECT_TRUE(rect1.ToString().compare("[0, 0, 0, 0]") == 0);

    node.renderProperties_.boundsGeo_ = nullptr;
    auto rect2 = node.GetFilterRect();
    EXPECT_TRUE(rect2.ToString().compare("[0, 0, 0, 0]") == 0);
}

/**
 * @tc.name: CalVisibleFilterRect
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest, CalVisibleFilterRectTest, TestSize.Level1)
{
    RSRenderNode node(id, context);
    RectI prepareClipRect { 1, 1, 1, 1 };
    node.CalVisibleFilterRect(prepareClipRect, prepareClipRect);
    EXPECT_TRUE(node.GetFilterRegionInfo().filterRegion_.ToString().compare("[0, 0, 0, 0]") == 0);
}

/**
 * @tc.name: OnTreeStateChangedTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest, OnTreeStateChangedTest, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> node = std::make_shared<RSRenderNode>(id, context); // isOnTheTree_ false
    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(floatData[0], floatData[1]);
    node->renderProperties_.filter_ = filter;
    node->OnTreeStateChanged();
    EXPECT_FALSE(node->isOnTheTree_);
    EXPECT_TRUE(node->HasBlurFilter());

    std::shared_ptr<RSRenderNode> inNode = std::make_shared<RSBaseRenderNode>(id, context);
    std::shared_ptr<RSRenderNode> outNode = std::make_shared<RSBaseRenderNode>(id + 1, context);
    auto sharedTransitionParam = std::make_shared<SharedTransitionParam>(inNode, outNode, true);
    node->SetSharedTransitionParam(sharedTransitionParam);
    node->OnTreeStateChanged();
    EXPECT_FALSE(node->sharedTransitionParam_->paired_);

    node->isOnTheTree_ = true;
    node->OnTreeStateChanged();
    EXPECT_TRUE(node->IsDirty());

    auto canvasDrawingNode = std::make_shared<RSCanvasDrawingRenderNode>(1);
    canvasDrawingNode->isNeverOnTree_ = false;
    canvasDrawingNode->OnTreeStateChanged();
    EXPECT_FALSE(canvasDrawingNode->isNeverOnTree_);
    canvasDrawingNode->isOnTheTree_ = true;
    canvasDrawingNode->OnTreeStateChanged();
    EXPECT_FALSE(canvasDrawingNode->isNeverOnTree_);
}

/**
 * @tc.name: HasDisappearingTransitionTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest, HasDisappearingTransitionTest, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(id, context);
    node->disappearingTransitionCount_ = 1;
    node->isOnTheTree_ = true;
    EXPECT_TRUE(node->HasDisappearingTransition(true));
    node->disappearingTransitionCount_ = 0;
    EXPECT_FALSE(node->HasDisappearingTransition(false));
    auto node1 = std::make_shared<RSRenderNode>(id + 1, context);
    auto parent = node1->weak_from_this();
    node->SetParent(parent);
    EXPECT_FALSE(node->HasDisappearingTransition(true));
}

/**
 * @tc.name: GenerateFullChildrenListTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest, GenerateFullChildrenListTest, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(id, context);
    node->GenerateFullChildrenList();
    EXPECT_TRUE(node->isChildrenSorted_ && node->children_.empty() && node->disappearingChildren_.empty());

    // children_ isn't empty
    std::weak_ptr<RSRenderNode> wNode1;
    node->children_.emplace_back(wNode1); // wNode1 is nullptr
    node->GenerateFullChildrenList();
    EXPECT_TRUE(node->children_.empty());
    std::shared_ptr<RSRenderNode> sNode1 = std::make_shared<RSRenderNode>(id, context);
    sNode1->isBootAnimation_ = false;
    wNode1 = sNode1;
    node->children_.emplace_back(wNode1); // wNode1 isn't nullptr

    node->isContainBootAnimation_ = true;
    node->GenerateFullChildrenList();
    EXPECT_TRUE(!node->children_.empty());

    node->isContainBootAnimation_ = false;
    node->GenerateFullChildrenList();
    EXPECT_TRUE(!node->children_.empty());

    // disappearingChildren_ isn't empty
    node->disappearingChildren_.emplace_back(sNode1, id);
    node->GenerateFullChildrenList();
    EXPECT_FALSE(node->isContainBootAnimation_);

    node->isContainBootAnimation_ = true;
    node->GenerateFullChildrenList();
    EXPECT_TRUE(node->isContainBootAnimation_);
}

/**
 * @tc.name: ResortChildrenTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest, ResortChildrenTest, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(id, context);
    auto node1 = std::make_shared<RSRenderNode>(id + 1, context);
    auto node2 = std::make_shared<RSRenderNode>(id + 2, context);
    auto fullChildrenList = std::make_shared<std::vector<std::shared_ptr<RSRenderNode>>>();
    fullChildrenList->emplace_back(std::move(node1));
    fullChildrenList->emplace_back(std::move(node2));
    node->fullChildrenList_ = fullChildrenList;
    node->isChildrenSorted_ = false;
    EXPECT_TRUE(!node->fullChildrenList_->empty());
    node->ResortChildren();
    EXPECT_TRUE(node->isChildrenSorted_);
}

/**
 * @tc.name: HasChildrenOutOfRectTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest, HasChildrenOutOfRectTest, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(id, context);
    node->hasChildrenOutOfRect_ = true;
    EXPECT_TRUE(node->HasChildrenOutOfRect());
    node->renderProperties_.clipToBounds_ = true;
    EXPECT_TRUE(node->GetRenderProperties().GetClipToBounds());
    EXPECT_FALSE(node->HasChildrenOutOfRect());
}

/**
 * @tc.name: UpdateVisibleFilterChildTest_001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest, UpdateVisibleFilterChildTest_001, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(id, context);
    auto childNode = std::make_shared<RSRenderNode>(id + 1, context);
    childNode->GetMutableRenderProperties().needFilter_ = true;
    EXPECT_TRUE(childNode->GetRenderProperties().NeedFilter());
    node->UpdateVisibleFilterChild(*childNode);
    EXPECT_TRUE(!node->visibleFilterChild_.empty());
}

/**
 * @tc.name: UpdateVisibleFilterChildTest_002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest, UpdateVisibleFilterChildTest_002, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(id, context);
    auto childNode = std::make_shared<RSRenderNode>(id + 1, context);
    childNode->GetHwcRecorder().SetBlendWithBackground(true);
    node->UpdateVisibleFilterChild(*childNode);
    EXPECT_TRUE(!node->visibleFilterChild_.empty());
}

/**
 * @tc.name: UpdateVisibleFilterChildTest_003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest, UpdateVisibleFilterChildTest_003, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(id, context);
    auto childNode = std::make_shared<RSRenderNode>(id + 1, context);
    childNode->GetHwcRecorder().SetForegroundColorValid(true);
    node->UpdateVisibleFilterChild(*childNode);
    EXPECT_TRUE(!node->visibleFilterChild_.empty());
}

/**
 * @tc.name: UpdateVisibleEffectChildTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest, UpdateVisibleEffectChildTest, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(id, context);
    auto childNode = std::make_shared<RSRenderNode>(id + 1, context);
    childNode->GetMutableRenderProperties().GetEffect().useEffect_ = true;
    childNode->SetOldDirtyInSurface(RectI(0, 0, 10, 10));
    EXPECT_TRUE(childNode->GetRenderProperties().GetUseEffect());
    node->UpdateVisibleEffectChild(*childNode);
    EXPECT_TRUE(!node->visibleEffectChild_.empty());
}

/**
 * @tc.name: GetInstanceRootNodeTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest, GetInstanceRootNodeTest, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(id, context);
    auto retNode = node->GetInstanceRootNode();
    EXPECT_TRUE(retNode == nullptr);
    auto sContext = std::make_shared<RSContext>();
    node->context_ = sContext;
    node->GetInstanceRootNode();
    EXPECT_TRUE(node->GetContext().lock() != nullptr);
}

/**
 * @tc.name: AddUIExtensionChildTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueIBK74G
 */
HWTEST_F(RSRenderNodeTest, AddUIExtensionChildTest, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(id, context);
    auto childNode = std::make_shared<RSRenderNode>(id + 1, context);
    node->AddUIExtensionChild(childNode);
    EXPECT_FALSE(childNode->GetParent().lock());

    auto uiExtension = std::make_shared<RSSurfaceRenderNode>(id + 2, context);
    uiExtension->SetUIExtensionUnobscured(true);
    uiExtension->SetSurfaceNodeType(RSSurfaceNodeType::UI_EXTENSION_COMMON_NODE);
    node->AddUIExtensionChild(uiExtension);
    EXPECT_FALSE(uiExtension->GetParent().lock());

    auto parent = std::make_shared<RSSurfaceRenderNode>(id + 3, context);
    parent->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    parent->AddUIExtensionChild(uiExtension);
    EXPECT_TRUE(uiExtension->GetParent().lock());
}

/**
 * @tc.name: MoveUIExtensionChildTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueIBK74G
 */
HWTEST_F(RSRenderNodeTest, MoveUIExtensionChildTest, TestSize.Level1)
{
    auto uiExtension = std::make_shared<RSSurfaceRenderNode>(id + 1, context);
    uiExtension->SetUIExtensionUnobscured(true);
    uiExtension->SetSurfaceNodeType(RSSurfaceNodeType::UI_EXTENSION_COMMON_NODE);
    auto parent = std::make_shared<RSSurfaceRenderNode>(id + 2, context);
    parent->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    parent->AddUIExtensionChild(uiExtension);
    EXPECT_TRUE(uiExtension->GetParent().lock());
    parent->MoveUIExtensionChild(uiExtension);
    EXPECT_TRUE(uiExtension->GetParent().lock());
}

/**
 * @tc.name: RemoveUIExtensionChildTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueIBK74G
 */
HWTEST_F(RSRenderNodeTest, RemoveUIExtensionChildTest, TestSize.Level1)
{
    auto uiExtension = std::make_shared<RSSurfaceRenderNode>(id + 1, context);
    uiExtension->SetUIExtensionUnobscured(true);
    uiExtension->SetSurfaceNodeType(RSSurfaceNodeType::UI_EXTENSION_COMMON_NODE);
    auto parent = std::make_shared<RSSurfaceRenderNode>(id + 2, context);
    parent->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    parent->AddUIExtensionChild(uiExtension);
    EXPECT_TRUE(uiExtension->GetParent().lock());
    parent->RemoveUIExtensionChild(uiExtension);
    EXPECT_FALSE(uiExtension->GetParent().lock());
}

/**
 * @tc.name: NeedRoutedBasedOnUIExtensionTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueIBK74G
 */
HWTEST_F(RSRenderNodeTest, NeedRoutedBasedOnUIExtensionTest, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(id, context);
    auto uiExtension = std::make_shared<RSSurfaceRenderNode>(id + 1, context);
    uiExtension->SetUIExtensionUnobscured(true);
    EXPECT_FALSE(node->NeedRoutedBasedOnUIExtension(uiExtension));
}

/**
 * @tc.name: UpdateTreeUifirstRootNodeIdTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest, UpdateTreeUifirstRootNodeIdTest, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(id, context);
    auto childId = id + 1;
    auto childNode = std::make_shared<RSRenderNode>(childId, context);
    auto fullChildrenList = std::make_shared<std::vector<std::shared_ptr<RSRenderNode>>>();
    fullChildrenList->emplace_back(std::move(childNode));
    node->fullChildrenList_ = fullChildrenList;
    node->UpdateTreeUifirstRootNodeId(childId);
    EXPECT_TRUE(node->uifirstRootNodeId_ == childId);
    for (auto& child : *node->GetChildren()) {
        EXPECT_TRUE(child != nullptr);
    }
}

/**
 * @tc.name: GetFirstLevelNodeTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest, GetFirstLevelNodeTest, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(id, context);
    auto retNode = node->GetFirstLevelNode();
    EXPECT_TRUE(retNode == nullptr);
    auto sContext = std::make_shared<RSContext>();
    node->context_ = sContext;
    node->GetFirstLevelNode();
    EXPECT_TRUE(node->GetContext().lock() != nullptr);
}

/**
 * @tc.name: SetGeoUpdateDelayTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest, SetGeoUpdateDelayTest, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(id, context);
    EXPECT_FALSE(node->geoUpdateDelay_);
    node->SetGeoUpdateDelay(true);
    EXPECT_TRUE(node->geoUpdateDelay_);
}

/**
 * @tc.name: UpdateSubSurfaceCntTest
 * @tc.desc: test addChild and removeChild
 * @tc.type: FUNC
 * @tc.require: issueBAYJG
 */
HWTEST_F(RSRenderNodeTest, UpdateSubSurfaceCntTest001, TestSize.Level1)
{
    const int cnt = 0;
    auto rootNode = std::make_shared<RSRenderNode>(DEFAULT_NODE_ID);

    auto leashNode = std::make_shared<RSSurfaceRenderNode>(DEFAULT_NODE_ID + 1);
    leashNode->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    rootNode->AddChild(leashNode);
    rootNode->UpdateSubSurfaceCnt(leashNode->subSurfaceCnt_);
    EXPECT_EQ(rootNode->subSurfaceCnt_, cnt + 1);

    auto selfDrawNode = std::make_shared<RSSurfaceRenderNode>(DEFAULT_NODE_ID + 2);
    selfDrawNode->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_NODE;
    leashNode->AddChild(selfDrawNode);
    leashNode->UpdateSubSurfaceCnt(selfDrawNode->subSurfaceCnt_);
    EXPECT_EQ(rootNode->subSurfaceCnt_, cnt + 1);

    rootNode->RemoveChild(leashNode);
    rootNode->UpdateSubSurfaceCnt(-leashNode->subSurfaceCnt_);
    EXPECT_EQ(rootNode->subSurfaceCnt_, cnt);
}

/**
 * @tc.name: UpdateSubSurfaceCntTest
 * @tc.desc: test addChild and loop
 * @tc.type: FUNC
 * @tc.require: issueBAYJG
 */
HWTEST_F(RSRenderNodeTest, UpdateSubSurfaceCntTest002, TestSize.Level1)
{
    const int cnt = 0;
    auto rootNode = std::make_shared<RSRenderNode>(DEFAULT_NODE_ID);

    auto leashNode = std::make_shared<RSSurfaceRenderNode>(DEFAULT_NODE_ID + 1);
    leashNode->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    rootNode->AddChild(leashNode);
    rootNode->UpdateSubSurfaceCnt(leashNode->subSurfaceCnt_);
    EXPECT_EQ(rootNode->subSurfaceCnt_, cnt + 1);

    auto appNode = std::make_shared<RSSurfaceRenderNode>(DEFAULT_NODE_ID + 2);
    appNode->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    leashNode->AddChild(appNode);
    leashNode->UpdateSubSurfaceCnt(appNode->subSurfaceCnt_);
    EXPECT_EQ(rootNode->subSurfaceCnt_, cnt + 2);

    auto antherApp = std::make_shared<RSSurfaceRenderNode>(DEFAULT_NODE_ID + 3);
    antherApp->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    appNode->AddChild(antherApp);
    appNode->UpdateSubSurfaceCnt(antherApp->subSurfaceCnt_);
    EXPECT_EQ(rootNode->subSurfaceCnt_, cnt + 3);

    // loop tree
    antherApp->AddChild(leashNode);
    antherApp->UpdateSubSurfaceCnt(antherApp->subSurfaceCnt_);
    EXPECT_EQ(rootNode->subSurfaceCnt_, cnt + 3);
}

/**
 * @tc.name: GetNodeGroupTypeTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest, GetNodeGroupTypeTest, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(id, context);
    auto retType = node->GetNodeGroupType();
    EXPECT_EQ(retType, RSRenderNode::NodeGroupType::NONE);
    node->nodeGroupType_ = RSRenderNode::NodeGroupType::GROUP_TYPE_BUTT;
    retType = node->GetNodeGroupType();
    EXPECT_EQ(retType, RSRenderNode::NodeGroupType::GROUP_TYPE_BUTT);
}

/**
 * @tc.name: InitRenderParamsTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest, InitRenderParamsTest, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(id, context);
    node->InitRenderParams();
    EXPECT_TRUE(node->renderDrawable_ == nullptr);
}

/**
 * @tc.name: UpdateRenderParamsTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest, UpdateRenderParamsTest, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(id, context);

    node->stagingRenderParams_ = std::make_unique<RSRenderParams>(node->id_);
    node->UpdateRenderParams();
    EXPECT_TRUE(node->GetRenderProperties().GetSandBox() == std::nullopt);

    node->GetMutableRenderProperties().sandbox_ = std::make_unique<Sandbox>();
    Vector2f vec2f(floatData[0], floatData[1]);
    node->GetMutableRenderProperties().sandbox_->position_ = vec2f;
    node->UpdateRenderParams();
    EXPECT_TRUE(node->GetRenderProperties().GetSandBox() != std::nullopt);

    node->GetMutableRenderProperties().boundsGeo_ = nullptr;
    node->UpdateRenderParams();
    EXPECT_TRUE(node->GetRenderProperties().GetBoundsGeometry() == nullptr);
}

/**
 * @tc.name: UpdateCurCornerInfoTest
 * @tc.desc: Verify the boundary value handling when updating corner radius information,
 *           including max/min float values and empty radius scenarios.
 * @tc.type: FUNC
 * @tc.require: issueICVCOG
 */
HWTEST_F(RSRenderNodeTest, UpdateCurCornerInfoTest, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(id, context);
    auto maxFloatData = std::numeric_limits<float>::max();
    auto minFloatData = std::numeric_limits<float>::min();
    Vector4f curCornerRadius(floatData[0], floatData[1], floatData[2], minFloatData);
    Vector4f cornerRadius(floatData[0], floatData[1], floatData[2], maxFloatData);
    RectI curCornerRect;
    Vector4f emptyCornerRadius;
    node->UpdateCurCornerInfo(curCornerRadius, curCornerRect);
    EXPECT_TRUE(curCornerRadius[3] == minFloatData);
    node->GetMutableRenderProperties().SetCornerRadius(cornerRadius);
    node->UpdateCurCornerInfo(emptyCornerRadius, curCornerRect);
    EXPECT_TRUE(emptyCornerRadius[3] == maxFloatData);
    node->UpdateCurCornerInfo(curCornerRadius, curCornerRect);
    EXPECT_TRUE(curCornerRadius[3] == maxFloatData);
}

/**
 * @tc.name: UpdateSrcOrClipedAbsDrawRectChangeStateTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueIAZV18
 */
HWTEST_F(RSRenderNodeTest, UpdateSrcOrClipedAbsDrawRectChangeStateTest, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(id, context);
    auto left = 0;
    auto top = 0;
    auto width = 1920;
    auto height = 1048;
    RectI rectI(left, top, width, height);

    node->srcOrClipedAbsDrawRectChangeFlag_ = true;
    node->UpdateSrcOrClipedAbsDrawRectChangeState(rectI);
    EXPECT_FALSE(node->geometryChangeNotPerceived_);

    EXPECT_EQ(RSSystemProperties::GetSkipGeometryNotChangeEnabled(), true);
    node->srcOrClipedAbsDrawRectChangeFlag_ = true;
    node->geometryChangeNotPerceived_ = true;
    node->UpdateSrcOrClipedAbsDrawRectChangeState(rectI);
    EXPECT_FALSE(node->srcOrClipedAbsDrawRectChangeFlag_);
}

/**
 * @tc.name: OnSyncTest1
 * @tc.desc: OnSync Test1
 * @tc.type: FUNC
 * @tc.require: issueIA5Y41
 */
HWTEST_F(RSRenderNodeTest, OnSyncTest1, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> node = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(node, nullptr);
    node->renderDrawable_ = nullptr;
    node->OnSync();

    std::shared_ptr<RSRenderNode> child = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(child, nullptr);

    node->renderDrawable_ = std::make_shared<RSRenderNodeDrawableAdapterBoy>(child);
    EXPECT_NE(node->renderDrawable_, nullptr);

    node->drawCmdListNeedSync_ = true;
    node->stagingRenderParams_ = std::make_unique<RSRenderParams>(0);
    EXPECT_NE(node->stagingRenderParams_, nullptr);

    node->stagingRenderParams_->needSync_ = true;
    node->renderDrawable_->renderParams_ = std::make_unique<RSRenderParams>(0);
    EXPECT_NE(node->renderDrawable_->renderParams_, nullptr);

    node->uifirstNeedSync_ = true;
    node->renderDrawable_->uifirstRenderParams_ = std::make_unique<RSRenderParams>(0);
    EXPECT_NE(node->renderDrawable_->uifirstRenderParams_, nullptr);

    node->uifirstSkipPartialSync_ = false;
    node->dirtySlots_.emplace(RSDrawableSlot::BACKGROUND_FILTER);
    auto drawableFilter = std::make_shared<DrawableV2::RSForegroundFilterDrawable>();
    EXPECT_NE(drawableFilter, nullptr);

    node->GetDrawableVec(__func__)[static_cast<uint32_t>(RSDrawableSlot::BACKGROUND_FILTER)] = drawableFilter;
    node->drawingCacheType_ = RSDrawingCacheType::FORCED_CACHE;
    node->stagingRenderParams_->freezeFlag_ = true;
    node->needClearSurface_ = true;
    std::function<void()> clearTask = []() { printf("ClearSurfaceTask CallBack\n"); };
    node->GetOpincCache().isOpincRootFlag_ = true;
    node->OnSync();
    EXPECT_TRUE(node->dirtySlots_.empty());
    EXPECT_FALSE(node->drawCmdListNeedSync_);
    EXPECT_FALSE(node->uifirstNeedSync_);
    EXPECT_FALSE(node->needClearSurface_);
}

/**
 * @tc.name: OnSyncTest2
 * @tc.desc: OnSync Test2
 * @tc.type: FUNC
 * @tc.require: issueIA5Y41
 */
HWTEST_F(RSRenderNodeTest, OnSyncTest2, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> node = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(node, nullptr);

    node->uifirstSkipPartialSync_ = true;
    node->dirtySlots_.emplace(RSDrawableSlot::BACKGROUND_FILTER);
    node->OnSync();
    EXPECT_TRUE(node->dirtySlots_.empty());

    node->uifirstSkipPartialSync_ = false;
    auto backgroundColorDrawable = std::make_shared<DrawableV2::RSBackgroundColorDrawable>();
    EXPECT_NE(backgroundColorDrawable, nullptr);
    node->GetDrawableVec(__func__)[static_cast<uint32_t>(RSDrawableSlot::BACKGROUND_COLOR)]
        = backgroundColorDrawable;
    node->dirtySlots_.emplace(RSDrawableSlot::BACKGROUND_COLOR);
    node->OnSync();
    EXPECT_TRUE(node->dirtySlots_.empty());

    node->uifirstSkipPartialSync_ = true;
    auto backgroundColorDrawable2 = std::make_shared<DrawableV2::RSBackgroundColorDrawable>();
    EXPECT_NE(backgroundColorDrawable2, nullptr);
    node->GetDrawableVec(__func__)[static_cast<uint32_t>(RSDrawableSlot::BACKGROUND_COLOR)]
        = backgroundColorDrawable2;
    node->dirtySlots_.emplace(RSDrawableSlot::BACKGROUND_COLOR);
    node->OnSync();
    EXPECT_TRUE(node->dirtySlots_.empty());
}

/**
 * @tc.name: ValidateLightResourcesTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest, ValidateLightResourcesTest, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(id, context);
    auto& properties = node->GetMutableRenderProperties();
    properties.GetEffect().lightSourcePtr_ = std::make_shared<RSLightSource>();
    properties.GetEffect().lightSourcePtr_->intensity_ = floatData[1];
    EXPECT_TRUE(properties.GetEffect().lightSourcePtr_->IsLightSourceValid());
    properties.GetEffect().illuminatedPtr_ = std::make_shared<RSIlluminated>();
    properties.GetEffect().illuminatedPtr_->illuminatedType_ = IlluminatedType::BORDER;
    EXPECT_TRUE(properties.GetEffect().illuminatedPtr_->IsIlluminatedValid());
    node->ValidateLightResources();
}

/**
 * @tc.name: UpdatePointLightDirtySlotTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest, UpdatePointLightDirtySlotTest, TestSize.Level1)
{
    RSRenderNode node(id, context);
    EXPECT_TRUE(node.dirtySlots_.empty());
    node.UpdateDirtySlotsAndPendingNodes(RSDrawableSlot::MASK);
    EXPECT_FALSE(node.dirtySlots_.empty());
}

/**
 * @tc.name: UpdatePointLightDirtySlotTest2
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest, UpdatePointLightDirtySlotTest2, TestSize.Level1)
{
    RSRenderNode node(id, context);
    node.UpdatePointLightDirtySlot();
    EXPECT_FALSE(node.enableHdrEffect_);
}
/**
 * @tc.name: AddToPendingSyncListTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest, AddToPendingSyncListTest, TestSize.Level1)
{
    auto sContext = std::make_shared<RSContext>();
    context = sContext;
    auto node = std::make_shared<RSRenderNode>(id, context);
    node->addedToPendingSyncList_ = true;
    node->AddToPendingSyncList();
    EXPECT_TRUE(node->addedToPendingSyncList_);
    node->addedToPendingSyncList_ = false;
    node->AddToPendingSyncList();
    EXPECT_TRUE(node->addedToPendingSyncList_);
    node->context_.reset();
    node->addedToPendingSyncList_ = false;
    node->AddToPendingSyncList();
    EXPECT_FALSE(node->addedToPendingSyncList_);
}

/**
 * @tc.name: SetChildrenHasSharedTransitionTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest, SetChildrenHasSharedTransitionTest, TestSize.Level1)
{
    RSRenderNode node(id, context);
    node.SetChildrenHasSharedTransition(true);
    EXPECT_TRUE(node.childrenHasSharedTransition_);
}

/**
 * @tc.name: HasBlurFilterTest
 * @tc.desc: HasBlurFilter
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeTest, HasBlurFilterTest, TestSize.Level1)
{
    RSRenderNode node(id, context);
    EXPECT_FALSE(node.HasBlurFilter());
    node.renderProperties_.effect_ = std::make_unique<RSProperties::CommonEffectParams>();

    node.renderProperties_.effect_->materialFilter_ = std::make_shared<RSFilter>();
    EXPECT_TRUE(node.HasBlurFilter());

    node.renderProperties_.filter_ = std::make_shared<RSFilter>();
    EXPECT_TRUE(node.HasBlurFilter());

    node.renderProperties_.backgroundFilter_ = std::make_shared<RSFilter>();
    EXPECT_TRUE(node.HasBlurFilter());
    EXPECT_TRUE(node.renderProperties_.GetBackgroundFilter());
}

/**
 * @tc.name: GetPairedNodeTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest, GetPairedNodeTest, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> inNode = std::make_shared<RSBaseRenderNode>(id + 1, context);
    std::shared_ptr<RSRenderNode> outNode = std::make_shared<RSBaseRenderNode>(id + 2, context);
    auto sharedTransitionParam = std::make_shared<SharedTransitionParam>(inNode, outNode, true);
    auto ptr = sharedTransitionParam->GetPairedNode(id + 1);
    EXPECT_TRUE(ptr == outNode);
    ptr = sharedTransitionParam->GetPairedNode(id + 2);
    EXPECT_TRUE(ptr == inNode);
    ptr = sharedTransitionParam->GetPairedNode(id);
    EXPECT_FALSE(ptr);
}

/**
 @tc.name: UpdateAbsDrawRect
 @tc.desc: update node absrect.
 @tc.type: FUNC
 @tc.require: issueIAL4RE
 */
HWTEST_F(RSRenderNodeTest, UpdateAbsDrawRect, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);
    std::unique_ptr<RSRenderParams> stagingRenderParams = std::make_unique<RSRenderParams>(0);
    EXPECT_NE(stagingRenderParams, nullptr);
    nodeTest->stagingRenderParams_ = std::move(stagingRenderParams);
    RectI absRect = {10, 10, 10, 10};
    nodeTest->stagingRenderParams_->SetAbsDrawRect(absRect);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: UpdateHierarchyTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest, UpdateHierarchyTest, TestSize.Level1)
{
    auto inNodeId = id + 1;
    auto outNodeId = id + 2;
    auto otherNodeId = id + 3;
    std::shared_ptr<RSRenderNode> inNode = std::make_shared<RSBaseRenderNode>(inNodeId, context);
    std::shared_ptr<RSRenderNode> outNode = std::make_shared<RSBaseRenderNode>(outNodeId, context);
    auto sharedTransitionParam = std::make_shared<SharedTransitionParam>(inNode, outNode, true);
    sharedTransitionParam->relation_ = SharedTransitionParam::NodeHierarchyRelation::IN_NODE_BELOW_OUT_NODE;
    auto ret = sharedTransitionParam->IsLower(inNodeId);
    EXPECT_TRUE(ret);
    sharedTransitionParam->relation_ = SharedTransitionParam::NodeHierarchyRelation::IN_NODE_ABOVE_OUT_NODE;
    ret = sharedTransitionParam->IsLower(outNodeId);
    EXPECT_TRUE(ret);
    sharedTransitionParam->relation_ = SharedTransitionParam::NodeHierarchyRelation::UNKNOWN;
    ret = sharedTransitionParam->IsLower(otherNodeId);
    EXPECT_FALSE(ret);

    sharedTransitionParam->UpdateHierarchy(outNodeId);
    EXPECT_EQ(sharedTransitionParam->relation_, SharedTransitionParam::NodeHierarchyRelation::IN_NODE_ABOVE_OUT_NODE);

    sharedTransitionParam->relation_ = SharedTransitionParam::NodeHierarchyRelation::UNKNOWN;
    sharedTransitionParam->UpdateHierarchy(inNodeId);
    EXPECT_EQ(sharedTransitionParam->relation_, SharedTransitionParam::NodeHierarchyRelation::IN_NODE_BELOW_OUT_NODE);
}

/**
 * @tc.name: HasRelation
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest, HasRelationTest, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> inNode = std::make_shared<RSBaseRenderNode>(id + 1, context);
    std::shared_ptr<RSRenderNode> outNode = std::make_shared<RSBaseRenderNode>(id + 2, context);
    auto sharedTransitionParam = std::make_shared<SharedTransitionParam>(inNode, outNode, true);
    EXPECT_FALSE(sharedTransitionParam->HasRelation());

    sharedTransitionParam->UpdateHierarchy(inNode->GetId());
    EXPECT_TRUE(sharedTransitionParam->HasRelation());
}

/**
 * @tc.name: SetNeedGenerateDrawable
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest, SetNeedGenerateDrawableTest, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> inNode = std::make_shared<RSBaseRenderNode>(id + 1, context);
    std::shared_ptr<RSRenderNode> outNode = std::make_shared<RSBaseRenderNode>(id + 2, context);
    auto sharedTransitionParam = std::make_shared<SharedTransitionParam>(inNode, outNode, true);

    sharedTransitionParam->SetNeedGenerateDrawable(true);
    EXPECT_TRUE(sharedTransitionParam->needGenerateDrawable_);

    sharedTransitionParam->SetNeedGenerateDrawable(false);
    EXPECT_FALSE(sharedTransitionParam->needGenerateDrawable_);
}

/**
 * @tc.name: GenerateDrawable
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest, GenerateDrawableTest, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> parent = std::make_shared<RSRenderNode>(0);
    parent->stagingRenderParams_ = std::make_unique<RSRenderParams>(0);
    parent->childHasSharedTransition_ = true;
    std::shared_ptr<RSRenderNode> inNode = std::make_shared<RSBaseRenderNode>(id + 1, context);
    std::shared_ptr<RSRenderNode> outNode = std::make_shared<RSBaseRenderNode>(id + 2, context);
    inNode->parent_ = parent;
    outNode->parent_ = parent;

    auto sharedTransitionParam = std::make_shared<SharedTransitionParam>(inNode, outNode, true);
    inNode->SetSharedTransitionParam(sharedTransitionParam);
    outNode->SetSharedTransitionParam(sharedTransitionParam);

    sharedTransitionParam->GenerateDrawable(*outNode);
    EXPECT_FALSE(sharedTransitionParam->needGenerateDrawable_);

    sharedTransitionParam->needGenerateDrawable_ = true;
    sharedTransitionParam->UpdateHierarchy(inNode->GetId());
    EXPECT_TRUE(sharedTransitionParam->IsLower(inNode->GetId()));
    sharedTransitionParam->GenerateDrawable(*inNode);
    EXPECT_TRUE(sharedTransitionParam->needGenerateDrawable_);

    EXPECT_FALSE(sharedTransitionParam->IsLower(outNode->GetId()));
    sharedTransitionParam->GenerateDrawable(*outNode);
    EXPECT_FALSE(sharedTransitionParam->needGenerateDrawable_);
}

/**
 * @tc.name: UpdateUnpairedSharedTransitionMap
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest, UpdateUnpairedSharedTransitionMapTest, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> inNode = std::make_shared<RSBaseRenderNode>(id + 1, context);
    std::shared_ptr<RSRenderNode> outNode = std::make_shared<RSBaseRenderNode>(id + 2, context);
    auto sharedTransitionParam = std::make_shared<SharedTransitionParam>(inNode, outNode, true);
    EXPECT_TRUE(SharedTransitionParam::unpairedShareTransitions_.empty());

    SharedTransitionParam::UpdateUnpairedSharedTransitionMap(sharedTransitionParam);
    EXPECT_FALSE(SharedTransitionParam::unpairedShareTransitions_.empty());

    SharedTransitionParam::UpdateUnpairedSharedTransitionMap(sharedTransitionParam);
    EXPECT_TRUE(SharedTransitionParam::unpairedShareTransitions_.empty());
}

/**
 * @tc.name: ResetRelation
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest, ResetRelationTest, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> inNode = std::make_shared<RSBaseRenderNode>(id + 1, context);
    std::shared_ptr<RSRenderNode> outNode = std::make_shared<RSBaseRenderNode>(id + 2, context);
    auto sharedTransitionParam = std::make_shared<SharedTransitionParam>(inNode, outNode, true);

    sharedTransitionParam->UpdateHierarchy(inNode->GetId());
    EXPECT_NE(sharedTransitionParam->relation_, SharedTransitionParam::NodeHierarchyRelation::UNKNOWN);

    sharedTransitionParam->ResetRelation();
    EXPECT_EQ(sharedTransitionParam->relation_, SharedTransitionParam::NodeHierarchyRelation::UNKNOWN);
}

/**
 * @tc.name: DumpTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest, DumpTest, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> inNode = std::make_shared<RSBaseRenderNode>(id + 1, context);
    std::shared_ptr<RSRenderNode> outNode = std::make_shared<RSBaseRenderNode>(id + 2, context);
    auto sharedTransitionParam = std::make_shared<SharedTransitionParam>(inNode, outNode, true);
    auto dumpStr = sharedTransitionParam->Dump();

    EXPECT_FALSE(dumpStr.empty());
}

/**
 * @tc.name: InternalUnregisterSelfTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest, InternalUnregisterSelfTest, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> inNode = std::make_shared<RSBaseRenderNode>(id + 1, context);
    std::shared_ptr<RSRenderNode> outNode = std::make_shared<RSBaseRenderNode>(id + 2, context);
    auto sharedTransitionParam = std::make_shared<SharedTransitionParam>(inNode, outNode, true);
    sharedTransitionParam->InternalUnregisterSelf();

    ASSERT_FALSE(inNode->sharedTransitionParam_);
    ASSERT_FALSE(outNode->sharedTransitionParam_);
}

/**
 * @tc.name: RSRenderNodeDirtyTest001
 * @tc.desc: SetDirty OnRegisterTest SetParentSubTreeDirty and InitRenderParams test
 * @tc.type: FUNC
 * @tc.require: issueI9US6V
 */
HWTEST_F(RSRenderNodeTest, RSRenderNodeDirtyTest001, TestSize.Level1)
{
    std::shared_ptr<RSContext> contextTest1 = nullptr;
    RSRenderNode nodetest1(0, contextTest1);
    // SetDirty test
    std::shared_ptr<RSRenderNode> parent = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(parent, nullptr);
    parent->isSubTreeDirty_ = false;
    nodetest1.parent_ = parent;
    nodetest1.dirtyStatus_ = RSRenderNode::NodeDirty::CLEAN;
    // dirtyStatus_ is NodeDirty::CLEAN
    nodetest1.SetDirty(true);
    EXPECT_EQ(nodetest1.dirtyStatus_, RSRenderNode::NodeDirty::DIRTY);
    // dirtyStatus_ is not NodeDirty::CLEAN
    nodetest1.SetDirty(true);
    EXPECT_TRUE(parent->isSubTreeDirty_);

    // OnRegister and InitRenderParams test
    std::shared_ptr<RSContext> contextTest2 = std::make_shared<RSContext>();
    EXPECT_NE(contextTest2, nullptr);
    std::shared_ptr<RSRenderNode> nodeTest2 = std::make_shared<RSRenderNode>(0, contextTest2);
    EXPECT_NE(nodeTest2, nullptr);
    nodeTest2->renderDrawable_ = nullptr;
    std::shared_ptr<RSContext> contextTest3 = std::make_shared<RSContext>();
    EXPECT_NE(contextTest3, nullptr);
    nodeTest2->OnRegister(contextTest3);
    EXPECT_EQ(nodeTest2->dirtyStatus_, RSRenderNode::NodeDirty::DIRTY);
}

/**
 * @tc.name: RSRenderNodeDirtyTest002
 * @tc.desc: SetTreeStateChangeDirty and IsTreeStateChangeDirty test
 * @tc.type: FUNC
 * @tc.require: issueI9US6V
 */
HWTEST_F(RSRenderNodeTest, RSRenderNodeDirtyTest002, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodetest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodetest, nullptr);
    nodetest->SetTreeStateChangeDirty(true);
    EXPECT_TRUE(nodetest->IsTreeStateChangeDirty());
    nodetest->SetTreeStateChangeDirty(false);
    EXPECT_FALSE(nodetest->IsTreeStateChangeDirty());
}

/**
 * @tc.name: RSRenderNodeDirtyTest003
 * @tc.desc: SetParentTreeStateChangeDirty test
 * @tc.type: FUNC
 * @tc.require: issueI9US6V
 */
HWTEST_F(RSRenderNodeTest, RSRenderNodeDirtyTest003, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> parent = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(parent, nullptr);
    std::shared_ptr<RSRenderNode> child1 = std::make_shared<RSRenderNode>(1);
    EXPECT_NE(child1, nullptr);
    child1->parent_ = parent;
    std::shared_ptr<RSRenderNode> child2 = std::make_shared<RSRenderNode>(2);
    EXPECT_NE(child2, nullptr);
    child2->parent_ = child1;

    child2->SetParentTreeStateChangeDirty(true);
    EXPECT_TRUE(child1->IsTreeStateChangeDirty());
    EXPECT_TRUE(parent->IsTreeStateChangeDirty());
}

/**
 * @tc.name: SetChildrenTreeStateChangeDirty
 * @tc.desc: SetChildrenTreeStateChangeDirty test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeTest, SetChildrenTreeStateChangeDirtyTest, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> parent = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(parent, nullptr);
    std::shared_ptr<RSRenderNode> child1 = std::make_shared<RSRenderNode>(1);
    EXPECT_NE(child1, nullptr);
    parent->AddChild(child1, -1);
    child1->parent_ = parent;
    std::shared_ptr<RSRenderNode> child2 = std::make_shared<RSRenderNode>(2);
    EXPECT_NE(child2, nullptr);
    child2->SetTreeStateChangeDirty(true);
    child2->parent_ = child1;
    parent->AddChild(child1, -1);

    parent->SetChildrenTreeStateChangeDirty();
    EXPECT_TRUE(child2->IsTreeStateChangeDirty());
}

/**
 * @tc.name: IsSubTreeNeedPrepareTest001
 * @tc.desc: IsSubTreeNeedPrepare test
 * @tc.type: FUNC
 * @tc.require: issueI9US6V
 */
HWTEST_F(RSRenderNodeTest, IsSubTreeNeedPrepareTest001, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> parent = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(parent, nullptr);

    system::SetParameter("persist.sys.graphic.SubTreePrepareCheckType.type", "0");
    auto checkType = RSSystemProperties::GetSubTreePrepareCheckType();
    EXPECT_EQ(checkType, SubTreePrepareCheckType::DISABLED);
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(true, true));
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(true, false));
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(false, true));
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(false, false));

    // restore SubTreePrepareCheckType to default
    system::SetParameter("persist.sys.graphic.SubTreePrepareCheckType.type", "2");
    checkType = RSSystemProperties::GetSubTreePrepareCheckType();
    EXPECT_EQ(checkType, SubTreePrepareCheckType::ENABLED);
}

/**
 * @tc.name: IsSubTreeNeedPrepareTest002
 * @tc.desc: IsSubTreeNeedPrepare test
 * @tc.type: FUNC
 * @tc.require: issueI9US6V
 */
HWTEST_F(RSRenderNodeTest, IsSubTreeNeedPrepareTest002, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> parent = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(parent, nullptr);

    system::SetParameter("persist.sys.graphic.SubTreePrepareCheckType.type", "1");
    auto checkType = RSSystemProperties::GetSubTreePrepareCheckType();
    EXPECT_EQ(checkType, SubTreePrepareCheckType::DISABLE_SUBTREE_DIRTY_CHECK);
    parent->shouldPaint_ = false;
    EXPECT_FALSE(parent->IsSubTreeNeedPrepare(true, true));
    EXPECT_FALSE(parent->IsSubTreeNeedPrepare(true, false));
    EXPECT_FALSE(parent->IsSubTreeNeedPrepare(false, true));
    EXPECT_FALSE(parent->IsSubTreeNeedPrepare(false, false));

    parent->shouldPaint_ = true;
    bool isOccluded = false;
    parent->SetFirstLevelCrossNode(true);
    parent->SetTreeStateChangeDirty(true);
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(true, isOccluded));
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(false, isOccluded));
    parent->SetFirstLevelCrossNode(false);
    parent->SetTreeStateChangeDirty(true);
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(true, isOccluded));
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(false, isOccluded));
    parent->SetFirstLevelCrossNode(true);
    parent->SetTreeStateChangeDirty(false);
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(true, isOccluded));
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(false, isOccluded));
    parent->SetFirstLevelCrossNode(false);
    parent->SetTreeStateChangeDirty(false);
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(true, isOccluded));
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(false, isOccluded));

    isOccluded = true;
    parent->SetFirstLevelCrossNode(true);
    parent->SetTreeStateChangeDirty(true);
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(true, isOccluded));
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(false, isOccluded));
    parent->SetFirstLevelCrossNode(true);
    parent->SetTreeStateChangeDirty(false);
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(true, isOccluded));
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(false, isOccluded));
    parent->SetFirstLevelCrossNode(false);
    parent->SetTreeStateChangeDirty(true);
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(true, isOccluded));
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(false, isOccluded));
    parent->SetFirstLevelCrossNode(false);
    parent->SetTreeStateChangeDirty(false);
    EXPECT_FALSE(parent->IsSubTreeNeedPrepare(true, isOccluded));
    EXPECT_FALSE(parent->IsSubTreeNeedPrepare(false, isOccluded));

    // restore SubTreePrepareCheckType to default
    system::SetParameter("persist.sys.graphic.SubTreePrepareCheckType.type", "2");
    checkType = RSSystemProperties::GetSubTreePrepareCheckType();
    EXPECT_EQ(checkType, SubTreePrepareCheckType::ENABLED);
}

/**
 * @tc.name: IsSubTreeNeedPrepareTest003
 * @tc.desc: IsSubTreeNeedPrepare test
 * @tc.type: FUNC
 * @tc.require: issueI9US6V
 */
HWTEST_F(RSRenderNodeTest, IsSubTreeNeedPrepareTest003, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> parent = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(parent, nullptr);
    std::unique_ptr<RSRenderParams> stagingRenderParams = std::make_unique<RSRenderParams>(0);
    EXPECT_NE(stagingRenderParams, nullptr);
    parent->stagingRenderParams_ = std::move(stagingRenderParams);

    auto checkType = RSSystemProperties::GetSubTreePrepareCheckType();
    EXPECT_EQ(checkType, SubTreePrepareCheckType::ENABLED);
    parent->shouldPaint_ = true;
    bool isOccluded = false;

    parent->SetSubTreeDirty(true);
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(true, isOccluded));
    parent->SetSubTreeDirty(false);

    parent->SetChildHasVisibleFilter(false);
    parent->childHasSharedTransition_ = false;
    parent->isAccumulatedClipFlagChanged_ = false;
    parent->subSurfaceCnt_ = 0; // false
    EXPECT_FALSE(parent->IsSubTreeNeedPrepare(true, isOccluded));
    EXPECT_FALSE(parent->IsSubTreeNeedPrepare(false, isOccluded));
    parent->isAccumulatedClipFlagChanged_ = true;
    parent->subSurfaceCnt_ = 0; // false
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(true, isOccluded));
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(false, isOccluded));
    parent->isAccumulatedClipFlagChanged_ = false;
    parent->subSurfaceCnt_ = 1; // true
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(true, isOccluded));
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(false, isOccluded));
    parent->isAccumulatedClipFlagChanged_ = true;
    parent->subSurfaceCnt_ = 1; // true
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(true, isOccluded));
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(false, isOccluded));

    parent->childHasSharedTransition_ = true;
    parent->isAccumulatedClipFlagChanged_ = false;
    parent->subSurfaceCnt_ = 0; // false
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(true, isOccluded));
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(false, isOccluded));
    parent->isAccumulatedClipFlagChanged_ = true;
    parent->subSurfaceCnt_ = 0; // false
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(true, isOccluded));
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(false, isOccluded));
    parent->isAccumulatedClipFlagChanged_ = false;
    parent->subSurfaceCnt_ = 1; // true
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(true, isOccluded));
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(false, isOccluded));
    parent->isAccumulatedClipFlagChanged_ = true;
    parent->subSurfaceCnt_ = 1; // true
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(true, isOccluded));
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(false, isOccluded));
}

/**
 * @tc.name: IsSubTreeNeedPrepareTest004
 * @tc.desc: IsSubTreeNeedPrepare test
 * @tc.type: FUNC
 * @tc.require: issueI9US6V
 */
HWTEST_F(RSRenderNodeTest, IsSubTreeNeedPrepareTest004, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> parent = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(parent, nullptr);
    std::unique_ptr<RSRenderParams> stagingRenderParams = std::make_unique<RSRenderParams>(0);
    EXPECT_NE(stagingRenderParams, nullptr);
    parent->stagingRenderParams_ = std::move(stagingRenderParams);

    auto checkType = RSSystemProperties::GetSubTreePrepareCheckType();
    EXPECT_EQ(checkType, SubTreePrepareCheckType::ENABLED);
    parent->shouldPaint_ = true;
    bool isOccluded = false;
    bool filterInGlobal = false;
    parent->SetSubTreeDirty(false);
    parent->childHasSharedTransition_ = false;

    parent->SetRenderGroupExcludedStateChanged(true);
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(filterInGlobal, isOccluded));
    EXPECT_FALSE(parent->IsRenderGroupExcludedStateChanged());

    parent->SetChildHasVisibleFilter(false);
    EXPECT_FALSE(parent->IsSubTreeNeedPrepare(filterInGlobal, isOccluded));
    filterInGlobal = true;
    EXPECT_FALSE(parent->IsSubTreeNeedPrepare(filterInGlobal, isOccluded));

    parent->SetChildHasVisibleFilter(true);
    filterInGlobal = false;
    EXPECT_FALSE(parent->IsSubTreeNeedPrepare(filterInGlobal, isOccluded));
    filterInGlobal = true;
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(filterInGlobal, isOccluded));
}

/**
 * @tc.name: RSRenderNodeDumpTest002
 * @tc.desc: DumpNodeType DumpTree and DumpSubClassNode test
 * @tc.type: FUNC
 * @tc.require: issueI9US6V
 */
HWTEST_F(RSRenderNodeTest, RSRenderNodeDumpTest002, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);
    nodeTest->InitRenderParams();

    std::string outTest1 = "";
    nodeTest->DumpNodeType(RSRenderNodeType::SCREEN_NODE, outTest1);
    nodeTest->DumpNodeType(RSRenderNodeType::RS_NODE, outTest1);
    nodeTest->DumpNodeType(RSRenderNodeType::SURFACE_NODE, outTest1);
    nodeTest->DumpNodeType(RSRenderNodeType::CANVAS_NODE, outTest1);
    nodeTest->DumpNodeType(RSRenderNodeType::ROOT_NODE, outTest1);
    nodeTest->DumpNodeType(RSRenderNodeType::PROXY_NODE, outTest1);
    nodeTest->DumpNodeType(RSRenderNodeType::CANVAS_DRAWING_NODE, outTest1);
    nodeTest->DumpNodeType(RSRenderNodeType::EFFECT_NODE, outTest1);
    nodeTest->DumpNodeType(RSRenderNodeType::UNKNOW, outTest1);
    EXPECT_EQ(outTest1, OUT_STR1);

    std::string outTest2 = "";
    std::shared_ptr<RSRenderNode> inNode = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(inNode, nullptr);
    std::shared_ptr<RSRenderNode> outNode = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(outNode, nullptr);
    nodeTest->sharedTransitionParam_ = std::make_shared<SharedTransitionParam>(inNode, outNode, true);
    EXPECT_NE(nodeTest->sharedTransitionParam_, nullptr);
    nodeTest->nodeGroupType_ = RSRenderNode::GROUPED_BY_ANIM;
    nodeTest->uifirstRootNodeId_ = 1;
    nodeTest->SetBootAnimation(true);
    nodeTest->SetContainBootAnimation(true);
    nodeTest->dirtyStatus_ = RSRenderNode::NodeDirty::DIRTY;
    nodeTest->renderProperties_.isDirty_ = true;
    nodeTest->isSubTreeDirty_ = true;
    nodeTest->renderProperties_.isDrawn_ = false;
    nodeTest->renderProperties_.alphaNeedApply_ = false;
    nodeTest->isFullChildrenListValid_ = false;
    auto childNode = std::make_shared<RSRenderNode>(0);
    childNode->InitRenderParams();
    nodeTest->disappearingChildren_.emplace_back(childNode, 0);
    nodeTest->DumpTree(0, outTest2);
    EXPECT_EQ(outTest2, OUT_STR2);

    std::string outTest3 = "";
    nodeTest->DumpSubClassNode(outTest3);
    EXPECT_EQ(outTest3, "");
}

/**
 * @tc.name: RSRenderNodeDumpTest003
 * @tc.desc: DumpNodeType DumpTree and DumpSubClassNode test
 * @tc.type: FUNC
 * @tc.require: issueICCYNK
 */
HWTEST_F(RSRenderNodeTest, RSRenderNodeDumpTest003, TestSize.Level1)
{
    std::string outTest = "";
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(0);
    surfaceNode->InitRenderParams();
    auto consumerSurfacePtr = IConsumerSurface::Create();
    auto buffer = SurfaceBuffer::Create();
    ASSERT_TRUE(surfaceNode->GetRSSurfaceHandler() != nullptr);
    surfaceNode->GetRSSurfaceHandler()->buffer_.buffer = buffer;
    ASSERT_TRUE(surfaceNode->GetRSSurfaceHandler()->GetBuffer() != nullptr);
    ASSERT_FALSE(surfaceNode->GetRSSurfaceHandler()->GetConsumer() != nullptr);
    surfaceNode->DumpTree(0, outTest);
    ASSERT_TRUE(outTest.find("ScalingMode") != string::npos);
    ASSERT_FALSE(outTest.find("TransformType") != string::npos);

    outTest = "";
    surfaceNode->GetRSSurfaceHandler()->SetConsumer(consumerSurfacePtr);
    ASSERT_TRUE(surfaceNode->GetRSSurfaceHandler()->GetBuffer() != nullptr);
    ASSERT_TRUE(surfaceNode->GetRSSurfaceHandler()->GetConsumer() != nullptr);
    surfaceNode->DumpTree(0, outTest);
    ASSERT_TRUE(outTest.find("ScalingMode") != string::npos);
    ASSERT_TRUE(outTest.find("TransformType") != string::npos);

    outTest = "";
    surfaceNode->isRepaintBoundary_ = true;
    surfaceNode->DumpTree(0, outTest);
    ASSERT_TRUE(outTest.find("RB: true") != string::npos);
}

/**
 * @tc.name: RSRenderNodeDumpTest004
 * @tc.desc: DumpNodeType DumpTree and DumpSubClassNode test, node is subSurfaceNode
 * @tc.type: FUNC
 * @tc.require: issueIAJ6BA
 */
HWTEST_F(RSRenderNodeTest, RSRenderNodeDumpTest004, TestSize.Level1)
{
    std::string outTest = "";
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(0);
    surfaceNode->InitRenderParams();
    surfaceNode->isSubSurfaceNode_ = true;
    surfaceNode->DumpTree(0, outTest);
    ASSERT_TRUE(outTest.find("isSubSurfaceId") != string::npos);
}

/**
 * @tc.name: RSSurfaceRenderNodeDumpTest
 * @tc.desc: DumpNodeType DumpTree and DumpSubClassNode test
 * @tc.type: FUNC
 * @tc.require: issueIAJ6BA
 */
HWTEST_F(RSRenderNodeTest, RSSurfaceRenderNodeDumpTest, TestSize.Level1)
{
    std::string outTest = "";
    auto renderNode = std::make_shared<RSSurfaceRenderNode>(0);
    renderNode->DumpSubClassNode(outTest);
    EXPECT_EQ(outTest, ", Parent [null], Name [SurfaceNode], hasConsumer: 0, Alpha: 1.000000, Visible: 1, "
        "VisibleRegion [Empty], OpaqueRegion [Empty], OcclusionBg: 0, IsContainerTransparent: 0, SpecialLayer: 0, "
        "surfaceType: 0, ContainerConfig: [outR: 0 inR: 0 x: 0 y: 0 w: 0 h: 0], colorSpace: 4, uifirstColorGamut: 4"
        ", isSurfaceBufferOpaque: 0");
}

/**
 * @tc.name: RSScreenRenderNodeDumpTest
 * @tc.desc: DumpNodeType DumpTree and DumpSubClassNode test
 * @tc.type: FUNC
 * @tc.require: issueIAJ6BA
 */
HWTEST_F(RSRenderNodeTest, RSScreenRenderNodeDumpTest, TestSize.Level1)
{
    std::string outTest = "";
    auto context = std::make_shared<RSContext>();
    auto renderNode = std::make_shared<RSScreenRenderNode>(0, 0, context);
    renderNode->InitRenderParams();
    renderNode->DumpSubClassNode(outTest);
    EXPECT_EQ(outTest, ", colorSpace: 4");

    renderNode->DumpTree(0, outTest);
}

/**
 * @tc.name: RSRootRenderNodeDumpTest
 * @tc.desc: DumpNodeType DumpTree and DumpSubClassNode test
 * @tc.type: FUNC
 * @tc.require: issueIAJ6BA
 */
HWTEST_F(RSRenderNodeTest, RSRootRenderNodeDumpTest, TestSize.Level1)
{
    std::string outTest = "";
    auto renderNode = std::make_shared<RSRootRenderNode>(0);
    renderNode->DumpSubClassNode(outTest);
    EXPECT_EQ(outTest, ", Visible: 1, Size: [-inf, -inf], EnableRender: 1");
}

/**
 * @tc.name: RSUIContextDumpTest
 * @tc.desc: DumpNodeType DumpTree
 * @tc.type: FUNC
 * @tc.require: issueICPQSU
 */
HWTEST_F(RSRenderNodeTest, RSUIContextDumpTest, TestSize.Level1)
{
    std::string outTest = "";
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(0);
    canvasNode->InitRenderParams();
    uint64_t token = 1000;
    canvasNode->SetUIContextToken(token++);
    canvasNode->SetUIContextToken(token);
    canvasNode->DumpTree(0, outTest);
    ASSERT_TRUE(outTest.find("RSUIContextToken") != string::npos);
}

/**
 * @tc.name: AddChildTest004
 * @tc.desc: AddChild test
 * @tc.type: FUNC
 * @tc.require: issueI9US6V
 */
HWTEST_F(RSRenderNodeTest, AddChildTest004, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);
    std::shared_ptr<RSRenderNode> childTest1 = nullptr;
    nodeTest->AddChild(childTest1, 0);

    childTest1 = std::make_shared<RSRenderNode>(0);
    nodeTest->AddChild(childTest1, 0);

    std::shared_ptr<RSRenderNode> childTest2 = std::make_shared<RSRenderNode>(1);
    EXPECT_NE(childTest2, nullptr);
    std::shared_ptr<RSRenderNode> parent = nullptr;
    childTest2->parent_ = parent;
    nodeTest->isOnTheTree_ = false;
    nodeTest->isFullChildrenListValid_ = true;
    nodeTest->AddChild(childTest2, -1);
    EXPECT_FALSE(nodeTest->isFullChildrenListValid_);

    nodeTest->isFullChildrenListValid_ = true;
    nodeTest->AddChild(childTest2, 1);
    EXPECT_FALSE(nodeTest->isFullChildrenListValid_);

    parent = std::make_shared<RSRenderNode>(0);
    nodeTest->isFullChildrenListValid_ = true;
    nodeTest->isOnTheTree_ = true;
    nodeTest->AddChild(childTest2, 1);
    EXPECT_FALSE(nodeTest->isFullChildrenListValid_);
}

/**
 * @tc.name: AddChildTest005
 * @tc.desc: AddChild test when status of needUseCmdlistDrawRegion is true
 * @tc.type: FUNC
 * @tc.require: issueICI6YB
 */
HWTEST_F(RSRenderNodeTest, AddChildTest005, TestSize.Level1)
{
    std::vector<pid_t> pidList;
    pidList.emplace_back(ExtractPid(1));
    RSOptimizeCanvasDirtyCollector::GetInstance().SetOptimizeCanvasDirtyPidList(pidList);
    ASSERT_EQ(RSOptimizeCanvasDirtyCollector::GetInstance().IsOptimizeCanvasDirtyEnabled(1), true);
    auto optimizeCanvasDrawRegionEnabled = RSSystemProperties::GetOptimizeCanvasDrawRegionEnabled();
    if (!optimizeCanvasDrawRegionEnabled) {
        system::SetParameter("rosen.graphic.optimizeCanvasDrawRegion.enabled", "1");
    }
    auto optimizeParentNodeRegionEnabled = RSSystemProperties::GetOptimizeParentNodeRegionEnabled();
    if (optimizeParentNodeRegionEnabled) {
        system::SetParameter("rosen.graphic.optimizeParentNodeRegion.enabled", "0");
    }

    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(1);
    EXPECT_NE(nodeTest, nullptr);
    std::shared_ptr<RSRenderNode> childTest = std::make_shared<RSRenderNode>(1);
    EXPECT_NE(childTest, nullptr);
    childTest->SetNeedUseCmdlistDrawRegion(true);
    EXPECT_TRUE(RSSystemProperties::GetOptimizeCanvasDrawRegionEnabled());
    EXPECT_TRUE(childTest->needUseCmdlistDrawRegion_);
    EXPECT_TRUE(childTest->GetNeedUseCmdlistDrawRegion());
    nodeTest->AddChild(childTest, 0);
    system::SetParameter("rosen.graphic.optimizeCanvasDrawRegion.enabled",
        std::to_string(optimizeCanvasDrawRegionEnabled));
    system::SetParameter("rosen.graphic.optimizeParentNodeRegion.enabled",
        std::to_string(optimizeParentNodeRegionEnabled));
    pidList.assign(1, ExtractPid(INVALID_NODE_ID));
    RSOptimizeCanvasDirtyCollector::GetInstance().SetOptimizeCanvasDirtyPidList(pidList);
    ASSERT_EQ(RSOptimizeCanvasDirtyCollector::GetInstance().IsOptimizeCanvasDirtyEnabled(TARGET_NODE_ID), false);
}

/**
 * @tc.name: MoveChildTest005
 * @tc.desc: MoveChild test
 * @tc.type: FUNC
 * @tc.require: issueI9US6V
 */
HWTEST_F(RSRenderNodeTest, MoveChildTest005, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);

    std::shared_ptr<RSRenderNode> childTest = nullptr;
    nodeTest->MoveChild(childTest, 0);

    childTest = std::make_shared<RSRenderNode>(0);
    nodeTest->isFullChildrenListValid_ = true;
    std::shared_ptr<RSRenderNode> parent = std::make_shared<RSRenderNode>(0);
    childTest->parent_ = parent;
    nodeTest->MoveChild(childTest, 0);
    EXPECT_TRUE(nodeTest->isFullChildrenListValid_);

    std::shared_ptr<RSRenderNode> child1 = std::make_shared<RSRenderNode>(1);
    std::shared_ptr<RSRenderNode> child2 = std::make_shared<RSRenderNode>(2);
    std::shared_ptr<RSRenderNode> child3 = std::make_shared<RSRenderNode>(3);
    nodeTest->AddChild(child1);
    nodeTest->AddChild(child2);
    nodeTest->AddChild(child3);
    nodeTest->isFullChildrenListValid_ = true;
    nodeTest->MoveChild(child2, 3);
    EXPECT_FALSE(nodeTest->isFullChildrenListValid_);
    nodeTest->isFullChildrenListValid_ = true;
    nodeTest->MoveChild(child2, -1);
    EXPECT_FALSE(nodeTest->isFullChildrenListValid_);
    nodeTest->isFullChildrenListValid_ = true;
    nodeTest->MoveChild(child2, 4);
    EXPECT_FALSE(nodeTest->isFullChildrenListValid_);
}

/**
 * @tc.name: SetContainBootAnimationTest001
 * @tc.desc: SetContainBootAnimation test
 * @tc.type: FUNC
 * @tc.require: issueI9US6V
 */
HWTEST_F(RSRenderNodeTest, SetContainBootAnimationTest001, TestSize.Level1)
{
    NodeId id = 0;
    ScreenId screenId = 1;
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    auto screenNode = std::make_shared<RSScreenRenderNode>(id, screenId, context);
    screenNode->SetContainBootAnimation(true);
    auto tempRenderNode = std::make_shared<RSRenderNode>(1);
    screenNode->parent_ = tempRenderNode;
    screenNode->SetContainBootAnimation(true);

    NodeId canvasId = 2;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(canvasId);

    canvasNode->SetContainBootAnimation(true);
    EXPECT_FALSE(canvasNode->isFullChildrenListValid_);
}

/**
 * @tc.name: ParentChildRelationshipTest006
 * @tc.desc: RemoveChild and ResetClearSurfaeFunc test
 * @tc.type: FUNC
 * @tc.require: issueI9US6V
 */
HWTEST_F(RSRenderNodeTest, ParentChildRelationshipTest006, TestSize.Level1)
{
    std::vector<pid_t> pidList;
    pidList.emplace_back(ExtractPid(1));
    RSOptimizeCanvasDirtyCollector::GetInstance().SetOptimizeCanvasDirtyPidList(pidList);
    ASSERT_EQ(RSOptimizeCanvasDirtyCollector::GetInstance().IsOptimizeCanvasDirtyEnabled(1), true);
    auto optimizeCanvasDrawRegionEnabled = RSSystemProperties::GetOptimizeCanvasDrawRegionEnabled();
    if (!optimizeCanvasDrawRegionEnabled) {
        system::SetParameter("rosen.graphic.optimizeCanvasDrawRegion.enabled", "1");
    }
    // RemoveChild test
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);
    nodeTest->needUseCmdlistDrawRegion_ = true;

    std::shared_ptr<RSRenderNode> childTest = nullptr;
    nodeTest->RemoveChild(childTest, false);
    system::SetParameter("rosen.graphic.optimizeCanvasDrawRegion.enabled",
        std::to_string(optimizeCanvasDrawRegionEnabled));

    childTest = std::make_shared<RSRenderNode>(0);
    std::shared_ptr<RSRenderNode> parent = std::make_shared<RSRenderNode>(0);
    childTest->parent_ = parent;
    nodeTest->isFullChildrenListValid_ = true;
    nodeTest->RemoveChild(childTest, false);
    EXPECT_TRUE(nodeTest->isFullChildrenListValid_);

    nodeTest->isFullChildrenListValid_ = true;
    nodeTest->disappearingTransitionCount_ = 1;
    nodeTest->isBootAnimation_ = true;

    std::shared_ptr<RSRenderNode> child1 = std::make_shared<RSRenderNode>(1);
    std::shared_ptr<RSRenderNode> child2 = std::make_shared<RSRenderNode>(2);
    std::shared_ptr<RSRenderNode> child3 = std::make_shared<RSRenderNode>(3);
    nodeTest->AddChild(child1);
    nodeTest->AddChild(child2);
    nodeTest->AddChild(child3);
    nodeTest->RemoveChild(child1, false);
    EXPECT_FALSE(nodeTest->isFullChildrenListValid_);

    nodeTest->isFullChildrenListValid_ = true;
    nodeTest->RemoveChild(child2, true);
    EXPECT_FALSE(nodeTest->isFullChildrenListValid_);
    pidList.assign(1, ExtractPid(INVALID_NODE_ID));
    RSOptimizeCanvasDirtyCollector::GetInstance().SetOptimizeCanvasDirtyPidList(pidList);
    ASSERT_EQ(RSOptimizeCanvasDirtyCollector::GetInstance().IsOptimizeCanvasDirtyEnabled(TARGET_NODE_ID), false);
}

/**
 * @tc.name: RSRenderNodeTreeTest007
 * @tc.desc: SetIsOnTheTree ResetChildRelevantFlags and UpdateChildrenRect test
 * @tc.type: FUNC
 * @tc.require: issueI9US6V
 */
HWTEST_F(RSRenderNodeTest, RSRenderNodeTreeTest007, TestSize.Level1)
{
    // SetIsOnTheTree test
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);
    nodeTest->InitRenderParams();

    nodeTest->isOnTheTree_ = false;
    nodeTest->SetIsOnTheTree(false, 0, 1, 1, 1);

    nodeTest->isOnTheTree_ = true;
    nodeTest->SetIsOnTheTree(false, 0, 1, 1, 1);
    EXPECT_FALSE(nodeTest->isOnTheTree_);

    nodeTest->children_.emplace_back(std::make_shared<RSRenderNode>(0));
    nodeTest->children_.emplace_back(std::make_shared<RSRenderNode>(0));
    std::shared_ptr<RSRenderNode> renderNodeTest = nullptr;
    nodeTest->children_.emplace_back(renderNodeTest);

    nodeTest->disappearingChildren_.emplace_back(std::make_shared<RSRenderNode>(0), 0);
    nodeTest->SetIsOnTheTree(true, 0, 1, 1, 1);
    EXPECT_TRUE(nodeTest->isOnTheTree_);

    // ResetChildRelevantFlags test
    nodeTest->ResetChildRelevantFlags();
    EXPECT_FALSE(nodeTest->hasChildrenOutOfRect_);

    // UpdateChildrenRect test
    RectI subRect = RectI { 0, 0, 1, 1 };
    nodeTest->childrenRect_.Clear();
    nodeTest->UpdateChildrenRect(subRect);
    nodeTest->UpdateChildrenRect(subRect);
}

/**
 * @tc.name: AddCrossParentChildTest008
 * @tc.desc: AddCrossParentChild test
 * @tc.type: FUNC
 * @tc.require: issueI9US6V
 */
HWTEST_F(RSRenderNodeTest, AddCrossParentChildTest008, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);
    nodeTest->isOnTheTree_ = false;
    std::shared_ptr<RSRenderNode> childTest = nullptr;
    nodeTest->AddCrossParentChild(childTest, -1);

    std::shared_ptr<RSRenderNode> child1 = std::make_shared<RSRenderNode>(1);
    std::shared_ptr<RSRenderNode> child2 = std::make_shared<RSRenderNode>(2);
    std::shared_ptr<RSRenderNode> child3 = std::make_shared<RSRenderNode>(3);
    nodeTest->isFullChildrenListValid_ = true;
    nodeTest->AddCrossParentChild(child1, -1);
    EXPECT_FALSE(nodeTest->isFullChildrenListValid_);

    nodeTest->isFullChildrenListValid_ = true;
    nodeTest->AddCrossParentChild(child2, 3);
    EXPECT_FALSE(nodeTest->isFullChildrenListValid_);

    nodeTest->isOnTheTree_ = true;
    nodeTest->isFullChildrenListValid_ = true;
    nodeTest->AddCrossParentChild(child3, 4);
    EXPECT_FALSE(nodeTest->isFullChildrenListValid_);
}

/**
 * @tc.name: RemoveCrossParentChildTest009
 * @tc.desc: RemoveCrossParentChild test
 * @tc.type: FUNC
 * @tc.require: issueI9US6V
 */
HWTEST_F(RSRenderNodeTest, RemoveCrossParentChild009, TestSize.Level1)
{
    // RemoveChild test
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);

    std::shared_ptr<RSRenderNode> newParent1 = std::make_shared<RSRenderNode>(1);
    std::shared_ptr<RSRenderNode> newParent2 = std::make_shared<RSRenderNode>(2);
    std::shared_ptr<RSRenderNode> childTest = nullptr;
    nodeTest->RemoveCrossParentChild(childTest, newParent1);

    childTest = std::make_shared<RSRenderNode>(0);
    std::shared_ptr<RSRenderNode> parent = std::make_shared<RSRenderNode>(0);
    childTest->parent_ = parent;
    nodeTest->isFullChildrenListValid_ = true;
    nodeTest->RemoveCrossParentChild(childTest, newParent1);
    EXPECT_TRUE(nodeTest->isFullChildrenListValid_);

    nodeTest->isFullChildrenListValid_ = true;
    nodeTest->disappearingTransitionCount_ = 1;

    std::shared_ptr<RSRenderNode> child1 = std::make_shared<RSRenderNode>(1);
    std::shared_ptr<RSRenderNode> child2 = std::make_shared<RSRenderNode>(2);
    nodeTest->AddCrossParentChild(child1);
    nodeTest->AddCrossParentChild(child2);

    nodeTest->RemoveCrossParentChild(child1, newParent1);
    EXPECT_FALSE(nodeTest->isFullChildrenListValid_);

    nodeTest->disappearingTransitionCount_ = 0;
    parent = nullptr;
    nodeTest->isFullChildrenListValid_ = true;
    nodeTest->RemoveCrossParentChild(child2, newParent2);
    EXPECT_FALSE(nodeTest->isFullChildrenListValid_);
}

/**
 * @tc.name: AddCrossScreenChild
 * @tc.desc: AddCrossScreenChild test
 * @tc.type: FUNC
 * @tc.require: issueIBF3VR
 */
HWTEST_F(RSRenderNodeTest, AddCrossScreenChild, TestSize.Level1)
{
    NodeId id = 1;
    auto context = std::make_shared<RSContext>();
    auto displayRenderNode = std::make_shared<RSScreenRenderNode>(id, 0, context);
    EXPECT_NE(displayRenderNode, nullptr);
    auto childTest1 = nullptr;
    displayRenderNode->AddCrossScreenChild(childTest1, 2, -1);

    id = 2;
    auto childTest2 = std::make_shared<RSSurfaceRenderNode>(id);
    ASSERT_EQ(childTest2->cloneCrossNodeVec_.size(), 0);
    displayRenderNode->AddCrossScreenChild(childTest2, 2, -1);
    ASSERT_EQ(childTest2->cloneCrossNodeVec_.size(), 1);
    auto cloneNode = childTest2->cloneCrossNodeVec_[0];
    ASSERT_EQ(cloneNode->GetParent().lock(), displayRenderNode);
}

/**
 * @tc.name: AddCrossScreenChild
 * @tc.desc: AddCrossScreenChild test
 * @tc.type: FUNC
 * @tc.require: issueIBF3VR
 */
HWTEST_F(RSRenderNodeTest, RemoveCrossScreenChild, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);
    auto childTest1 = nullptr;
    nodeTest->RemoveCrossScreenChild(childTest1);

    auto childTest2 = std::make_shared<RSSurfaceRenderNode>(1);
    nodeTest->RemoveCrossScreenChild(childTest2);
    ASSERT_EQ(childTest2->cloneCrossNodeVec_.size(), 0);
}
/**
 * @tc.name: RemoveFromTreeTest010
 * @tc.desc: RemoveFromTree test
 * @tc.type: FUNC
 * @tc.require: issueIA5Y41
 */
HWTEST_F(RSRenderNodeTest, RemoveFromTreeTest010, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);

    std::shared_ptr<RSRenderNode> parentTest1 = nullptr;
    nodeTest->parent_ = parentTest1;
    nodeTest->RemoveFromTree(false);

    std::shared_ptr<RSRenderNode> parentTest2 = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(parentTest2, nullptr);

    nodeTest->parent_ = parentTest2;
    parentTest2->isFullChildrenListValid_ = true;
    nodeTest->isFullChildrenListValid_ = true;
    nodeTest->RemoveFromTree(false);
    EXPECT_TRUE(parentTest2->isFullChildrenListValid_);

    parentTest2->isFullChildrenListValid_ = true;
    nodeTest->isFullChildrenListValid_ = true;
    nodeTest->RemoveFromTree(true);
    EXPECT_FALSE(parentTest2->isFullChildrenListValid_);
}

/**
 * @tc.name: ClearChildrenTest011
 * @tc.desc: ClearChildren test
 * @tc.type: FUNC
 * @tc.require: issueI9US6V
 */
HWTEST_F(RSRenderNodeTest, ClearChildrenTest011, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);

    nodeTest->children_.clear();
    nodeTest->ClearChildren();

    nodeTest->disappearingTransitionCount_ = 1;
    nodeTest->children_.emplace_back(std::make_shared<RSRenderNode>(0));
    nodeTest->children_.emplace_back(std::make_shared<RSRenderNode>(0));
    std::shared_ptr<RSRenderNode> renderNodeTest1 = nullptr;
    nodeTest->children_.emplace_back(renderNodeTest1);
    EXPECT_EQ(nodeTest->children_.size(), 3);
    nodeTest->ClearChildren();
    EXPECT_EQ(nodeTest->children_.size(), 0);

    nodeTest->disappearingTransitionCount_ = 0;
    std::shared_ptr<RSRenderNode> renderNodeTest2 = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(renderNodeTest2, nullptr);
    renderNodeTest2->disappearingTransitionCount_ = 1;
    nodeTest->children_.emplace_back(renderNodeTest2);
    EXPECT_EQ(nodeTest->children_.size(), 1);
    nodeTest->ClearChildren();
    EXPECT_EQ(nodeTest->children_.size(), 0);

    std::shared_ptr<RSRenderNode> renderNodeTest3 = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(renderNodeTest3, nullptr);
    renderNodeTest3->disappearingTransitionCount_ = 0;
    nodeTest->children_.emplace_back(renderNodeTest3);
    EXPECT_EQ(nodeTest->children_.size(), 1);
    nodeTest->ClearChildren();
    EXPECT_EQ(nodeTest->children_.size(), 0);
}

/**
 * @tc.name: UpdateDrawingCacheInfoAfterChildrenTest012
 * @tc.desc: UpdateDrawingCacheInfoAfterChildrenTest_001
 * @tc.type: FUNC
 * @tc.require: issueI9US6V
 */
HWTEST_F(RSRenderNodeTest, UpdateDrawingCacheInfoAfterChildrenTest001, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);
    nodeTest->InitRenderParams();

    std::shared_ptr<RSRenderNode> childNode = std::make_shared<RSSurfaceRenderNode>(1);
    EXPECT_NE(childNode, nullptr);
    childNode->InitRenderParams();
    nodeTest->AddChild(childNode, 1);
    nodeTest->GenerateFullChildrenList();

    nodeTest->nodeGroupType_ = RSRenderNode::GROUPED_BY_USER;
    nodeTest->CheckDrawingCacheType();
    EXPECT_EQ(nodeTest->GetDrawingCacheType(), RSDrawingCacheType::FORCED_CACHE);

    childNode->SetLastFrameUifirstFlag(MultiThreadCacheType::ARKTS_CARD);
    // ArkTsCard disable render group
    nodeTest->UpdateDrawingCacheInfoAfterChildren();
    EXPECT_EQ(nodeTest->GetDrawingCacheType(), RSDrawingCacheType::DISABLED_CACHE);

    childNode->SetLastFrameUifirstFlag(MultiThreadCacheType::NONE);
    nodeTest->SetDrawingCacheType(RSDrawingCacheType::TARGETED_CACHE);
    nodeTest->UpdateDrawingCacheInfoAfterChildren();
    EXPECT_EQ(nodeTest->GetDrawingCacheType(), RSDrawingCacheType::TARGETED_CACHE);
}

/**
 * @tc.name: UpdateDrawingCacheInfoAfterChildrenTest012
 * @tc.desc: UpdateDrawingCacheInfoAfterChildrenTest_002
 * @tc.type: FUNC
 * @tc.require: issueI9US6V
 */
HWTEST_F(RSRenderNodeTest, UpdateDrawingCacheInfoAfterChildrenTest002, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);
    nodeTest->InitRenderParams();
    nodeTest->nodeGroupType_ = RSRenderNode::GROUPED_BY_USER;
    nodeTest->CheckDrawingCacheType();
    EXPECT_EQ(nodeTest->GetDrawingCacheType(), RSDrawingCacheType::FORCED_CACHE);

    nodeTest->hasChildrenOutOfRect_ = true;
    nodeTest->UpdateDrawingCacheInfoAfterChildren();
    EXPECT_EQ(nodeTest->GetDrawingCacheType(), RSDrawingCacheType::FORCED_CACHE);

    nodeTest->SetDrawingCacheType(RSDrawingCacheType::TARGETED_CACHE);
    nodeTest->UpdateDrawingCacheInfoAfterChildren();
    EXPECT_EQ(nodeTest->GetDrawingCacheType(), RSDrawingCacheType::DISABLED_CACHE);
}

/**
 * @tc.name: UpdateDrawingCacheInfoAfterChildrenTest012
 * @tc.desc: UpdateDrawingCacheInfoAfterChildrenTest_003
 * @tc.type: FUNC
 * @tc.require: issueI9US6V
 */
HWTEST_F(RSRenderNodeTest, UpdateDrawingCacheInfoAfterChildrenTest003, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);
    nodeTest->InitRenderParams();
    nodeTest->nodeGroupType_ = RSRenderNode::GROUPED_BY_USER;
    nodeTest->CheckDrawingCacheType();
    EXPECT_EQ(nodeTest->GetDrawingCacheType(), RSDrawingCacheType::FORCED_CACHE);
    nodeTest->UpdateDrawingCacheInfoAfterChildren(true);
    auto& stagingRenderParams = nodeTest->GetStagingRenderParams();
    EXPECT_NE(stagingRenderParams, nullptr);
    EXPECT_EQ(stagingRenderParams->NodeGroupHasChildInBlacklist(), true);
}

/**
 * @tc.name: UpdateDrawingCacheInfoAfterChildrenTest004
 * @tc.desc: Test ForceDisableNodeGroup
 * @tc.type: FUNC
 * @tc.require: issueI9US6V
 */
HWTEST_F(RSRenderNodeTest, UpdateDrawingCacheInfoAfterChildrenTest004, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);
    nodeTest->InitRenderParams();

    std::shared_ptr<RSRenderNode> childNode = std::make_shared<RSSurfaceRenderNode>(1);
    EXPECT_NE(childNode, nullptr);
    childNode->InitRenderParams();
    nodeTest->AddChild(childNode, 1);
    nodeTest->GenerateFullChildrenList();
    bool isInBlackList = false;

    nodeTest->nodeGroupType_ = RSRenderNode::GROUPED_BY_USER;
    nodeTest->CheckDrawingCacheType();
    EXPECT_EQ(nodeTest->GetDrawingCacheType(), RSDrawingCacheType::FORCED_CACHE);

    childNode->SetUIFirstSwitch(RSUIFirstSwitch::FORCE_DISABLE_CARD);
    childNode->UpdateDrawingCacheInfoAfterChildren(isInBlackList);
    EXPECT_TRUE(childNode->IsForceDisableNodeGroup());
    EXPECT_TRUE(nodeTest->IsForceDisableNodeGroup());

    nodeTest->UpdateDrawingCacheInfoAfterChildren(isInBlackList);
    EXPECT_EQ(nodeTest->GetDrawingCacheType(), RSDrawingCacheType::DISABLED_CACHE);
}

/**
 * @tc.name: UpdateDrawingCacheInfoAfterChildrenTest005
 * @tc.desc: Test ForceDisableNodeGroup
 * @tc.type: FUNC
 * @tc.require: issue21180
 */
HWTEST_F(RSRenderNodeTest, UpdateDrawingCacheInfoAfterChildrenTest005, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);
    nodeTest->InitRenderParams();

    std::shared_ptr<RSRenderNode> childNode = std::make_shared<RSSurfaceRenderNode>(1);
    EXPECT_NE(childNode, nullptr);
    childNode->InitRenderParams();
    nodeTest->AddChild(childNode, 1);
    nodeTest->GenerateFullChildrenList();

    nodeTest->nodeGroupType_ = RSRenderNode::GROUPED_BY_FOREGROUND_FILTER;
    nodeTest->CheckDrawingCacheType();
    EXPECT_EQ(nodeTest->GetDrawingCacheType(), RSDrawingCacheType::FOREGROUND_FILTER_CACHE);

    std::unordered_set<NodeId> childHasProtectedNodeSet;
    childHasProtectedNodeSet.insert(nodeTest->GetId());
    nodeTest->UpdateDrawingCacheInfoAfterChildren(false, childHasProtectedNodeSet);
    EXPECT_EQ(nodeTest->GetDrawingCacheType(), RSDrawingCacheType::DISABLED_CACHE);
}

/**
 * @tc.name: UpdateDrawingCacheInfoAfterChildrenTest006
 * @tc.desc: Test ForceDisableNodeGroup
 * @tc.type: FUNC
 * @tc.require: issue21180
 */
HWTEST_F(RSRenderNodeTest, UpdateDrawingCacheInfoAfterChildrenTest006, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);
    nodeTest->InitRenderParams();

    std::shared_ptr<RSRenderNode> childNode = std::make_shared<RSSurfaceRenderNode>(1);
    EXPECT_NE(childNode, nullptr);
    childNode->InitRenderParams();
    nodeTest->AddChild(childNode, 1);
    nodeTest->GenerateFullChildrenList();

    nodeTest->nodeGroupType_ = RSRenderNode::GROUPED_BY_USER;
    nodeTest->CheckDrawingCacheType();
    EXPECT_EQ(nodeTest->GetDrawingCacheType(), RSDrawingCacheType::FORCED_CACHE);

    std::unordered_set<NodeId> childHasProtectedNodeSet;
    childHasProtectedNodeSet.insert(nodeTest->GetId());
    nodeTest->UpdateDrawingCacheInfoAfterChildren(false, childHasProtectedNodeSet);
    nodeTest->UpdateDrawingCacheInfoAfterChildren();
    EXPECT_EQ(nodeTest->GetDrawingCacheType(), RSDrawingCacheType::FORCED_CACHE);
}

/**
 * @tc.name: UpdateDrawingCacheInfoAfterChildrenTest007
 * @tc.desc: Test ForceDisableNodeGroup
 * @tc.type: FUNC
 * @tc.require: issue21180
 */
HWTEST_F(RSRenderNodeTest, UpdateDrawingCacheInfoAfterChildrenTest007, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);
    nodeTest->InitRenderParams();

    std::shared_ptr<RSRenderNode> childNode = std::make_shared<RSSurfaceRenderNode>(1);
    EXPECT_NE(childNode, nullptr);
    childNode->InitRenderParams();
    nodeTest->AddChild(childNode, 1);
    nodeTest->GenerateFullChildrenList();

    nodeTest->nodeGroupType_ = RSRenderNode::GROUPED_BY_FOREGROUND_FILTER;
    nodeTest->CheckDrawingCacheType();
    EXPECT_EQ(nodeTest->GetDrawingCacheType(), RSDrawingCacheType::FOREGROUND_FILTER_CACHE);

    nodeTest->UpdateDrawingCacheInfoAfterChildren();
    EXPECT_EQ(nodeTest->GetDrawingCacheType(), RSDrawingCacheType::FOREGROUND_FILTER_CACHE);
}

/**
 * @tc.name: UpdateDrawingCacheInfoAfterChildrenTest008
 * @tc.desc: Test setRednerGroupSubtreeDirty
 * @tc.type: FUNC
 * @tc.require: issue21180
 */
HWTEST_F(RSRenderNodeTest, UpdateDrawingCacheInfoAfterChildrenTest008, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);
    nodeTest->InitRenderParams();
    nodeTest->nodeGroupType_ = RSRenderNode::GROUPED_BY_USER;
    nodeTest->CheckDrawingCacheType();
    EXPECT_EQ(nodeTest->GetDrawingCacheType(), RSDrawingCacheType::FORCED_CACHE);

    nodeTest->SetRenderGroupSubTreeDirty(true);
    nodeTest->UpdateDrawingCacheInfoAfterChildren();
    EXPECT_TRUE(nodeTest->GetDrawingCacheChanged());
    EXPECT_FALSE(nodeTest->IsRenderGroupSubTreeDirty());
}

/**
 * @tc.name: UpdateDrawingCacheInfoBeforeChildrenTest001
 * @tc.desc: CheckDrawingCacheType and UpdateDrawingCacheInfoBeforeChildren test
 * @tc.type: FUNC
 * @tc.require: issueI9US6V
 */
HWTEST_F(RSRenderNodeTest, UpdateDrawingCacheInfoBeforeChildrenTest001, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);
    nodeTest->InitRenderParams();
    // CheckDrawingCacheType test
    nodeTest->nodeGroupType_ = RSRenderNode::NONE;
    nodeTest->CheckDrawingCacheType();
    EXPECT_EQ(nodeTest->GetDrawingCacheType(), RSDrawingCacheType::DISABLED_CACHE);
    nodeTest->nodeGroupType_ = RSRenderNode::GROUPED_BY_USER;
    nodeTest->CheckDrawingCacheType();
    EXPECT_EQ(nodeTest->GetDrawingCacheType(), RSDrawingCacheType::FORCED_CACHE);
    nodeTest->nodeGroupType_ = RSRenderNode::GROUPED_BY_ANIM;
    nodeTest->CheckDrawingCacheType();
    EXPECT_EQ(nodeTest->GetDrawingCacheType(), RSDrawingCacheType::TARGETED_CACHE);

    // UpdateDrawingCacheInfoBeforeChildren test
    // shouldPaint_ is false
    nodeTest->shouldPaint_ = false;
    nodeTest->UpdateDrawingCacheInfoBeforeChildren(false, false);
    EXPECT_EQ(nodeTest->GetDrawingCacheType(), RSDrawingCacheType::DISABLED_CACHE);
    // shouldPaint_ is true
    nodeTest->shouldPaint_ = true;
    nodeTest->UpdateDrawingCacheInfoBeforeChildren(true, false);
    if (RSSystemProperties::GetCacheOptimizeRotateEnable()) {
        EXPECT_EQ(nodeTest->GetDrawingCacheType(), RSDrawingCacheType::TARGETED_CACHE);
    } else {
        EXPECT_EQ(nodeTest->GetDrawingCacheType(), RSDrawingCacheType::DISABLED_CACHE);
    }
    // isScreenRotation is true
    nodeTest->nodeGroupType_ = RSRenderNode::NONE;
    nodeTest->UpdateDrawingCacheInfoBeforeChildren(false, false);
    EXPECT_EQ(nodeTest->GetDrawingCacheType(), RSDrawingCacheType::DISABLED_CACHE);

    // isScreenRotation is false
    nodeTest->nodeGroupType_ = RSRenderNode::GROUPED_BY_ANIM;
    std::unique_ptr<RSRenderParams> stagingRenderParams = std::make_unique<RSRenderParams>(0);
    EXPECT_NE(stagingRenderParams, nullptr);
    nodeTest->stagingRenderParams_ = std::move(stagingRenderParams);
    nodeTest->UpdateDrawingCacheInfoBeforeChildren(false, false);
    EXPECT_FALSE(nodeTest->stagingRenderParams_->needSync_);
}

/**
 * @tc.name: UpdateDrawingCacheInfoBeforeChildrenTest002
 * @tc.desc: test instanceRootNodeInfo
 * @tc.type: FUNC
 * @tc.require: issueI9US6V
 */
HWTEST_F(RSRenderNodeTest, UpdateDrawingCacheInfoBeforeChildrenTest002, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);
    nodeTest->InitRenderParams();
    EXPECT_NE(nodeTest->stagingRenderParams_, nullptr);
    nodeTest->nodeGroupType_ = RSRenderNode::GROUPED_BY_USER;
    nodeTest->CheckDrawingCacheType();
    EXPECT_EQ(nodeTest->GetDrawingCacheType(), RSDrawingCacheType::FORCED_CACHE);

    std::shared_ptr<RSSurfaceRenderNode> surfaceNode = std::make_shared<RSSurfaceRenderNode>(10086);
    EXPECT_NE(surfaceNode, nullptr);
    surfaceNode->name_ = "instanceRootNode";
    auto sContext = std::make_shared<RSContext>();
    nodeTest->context_ = sContext;
    nodeTest->instanceRootNodeId_ = surfaceNode->GetId();
    auto& nodeMap = sContext->GetMutableNodeMap();
    EXPECT_TRUE(nodeMap.RegisterRenderNode(surfaceNode));
    auto instanceRootNode = nodeTest->GetInstanceRootNode()->ReinterpretCastTo<RSSurfaceRenderNode>();
    EXPECT_NE(instanceRootNode, nullptr);
    EXPECT_EQ(instanceRootNode->GetName(), surfaceNode->GetName());
    nodeTest->UpdateDrawingCacheInfoBeforeChildren(false, false);
    EXPECT_NE(nodeTest->stagingRenderParams_->GetInstanceRootNodeId(), surfaceNode->GetId());
    EXPECT_NE(nodeTest->stagingRenderParams_->GetInstanceRootNodeName(), surfaceNode->GetName());

    surfaceNode->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    nodeTest->UpdateDrawingCacheInfoBeforeChildren(false, false);
    EXPECT_EQ(nodeTest->stagingRenderParams_->GetInstanceRootNodeId(), surfaceNode->GetId());
    EXPECT_EQ(nodeTest->stagingRenderParams_->GetInstanceRootNodeName(), surfaceNode->GetName());
}

/**
 * @tc.name: UpdateDrawingCacheInfoBeforeChildrenTest003
 * @tc.desc: test UpdateDrawingCacheInfoBeforeChildren with excluded node
 * @tc.type: FUNC
 * @tc.require: issueI9US6V
 */
HWTEST_F(RSRenderNodeTest, UpdateDrawingCacheInfoBeforeChildrenTest003, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);
    nodeTest->InitRenderParams();
    EXPECT_NE(nodeTest->stagingRenderParams_, nullptr);
    nodeTest->nodeGroupType_ = RSRenderNode::GROUPED_BY_USER;

    bool isScreenRotation = false;
    bool isOnExcludedSubTree = false;
    nodeTest->UpdateDrawingCacheInfoBeforeChildren(isScreenRotation, isOnExcludedSubTree);
    EXPECT_EQ(nodeTest->GetDrawingCacheType(), RSDrawingCacheType::FORCED_CACHE);

    isScreenRotation = false;
    isOnExcludedSubTree = true;
    nodeTest->UpdateDrawingCacheInfoBeforeChildren(isScreenRotation, isOnExcludedSubTree);
    EXPECT_EQ(nodeTest->GetDrawingCacheType(), RSDrawingCacheType::DISABLED_CACHE);
}

/**
 * @tc.name: ExcludedFromNodeGroupTest
 * @tc.desc: Test ExcludedFromNodeGroup
 * @tc.type: FUNC
 * @tc.require: issues/20738
 */
HWTEST_F(RSRenderNodeTest, ExcludedFromNodeGroupTest, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);
    nodeTest->InitRenderParams();
    EXPECT_NE(nodeTest->stagingRenderParams_, nullptr);

    nodeTest->ExcludedFromNodeGroup(true);
    EXPECT_TRUE(nodeTest->IsExcludedFromNodeGroup());

    nodeTest->ExcludedFromNodeGroup(false);
    EXPECT_FALSE(nodeTest->IsExcludedFromNodeGroup());
}

/**
 * @tc.name: SetHasChildExcludedFromNodeGroupTest
 * @tc.desc: Test SetHasChildExcludedFromNodeGroup
 * @tc.type: FUNC
 * @tc.require: issues/20738
 */
HWTEST_F(RSRenderNodeTest, SetHasChildExcludedFromNodeGroupTest, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);
    nodeTest->InitRenderParams();
    EXPECT_NE(nodeTest->stagingRenderParams_, nullptr);

    nodeTest->SetHasChildExcludedFromNodeGroup(true);
    EXPECT_TRUE(nodeTest->HasChildExcludedFromNodeGroup());

    nodeTest->SetHasChildExcludedFromNodeGroup(false);
    EXPECT_FALSE(nodeTest->HasChildExcludedFromNodeGroup());
}

/**
 * @tc.name: SetRenderGroupExcludedStateChangedTest
 * @tc.desc: Test SetRenderGroupExcludedStateChanged
 * @tc.type: FUNC
 * @tc.require: issues/20738
 */
HWTEST_F(RSRenderNodeTest, SetRenderGroupExcludedStateChangedTest, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);
    nodeTest->InitRenderParams();
    EXPECT_NE(nodeTest->stagingRenderParams_, nullptr);

    nodeTest->SetRenderGroupExcludedStateChanged(true);
    EXPECT_TRUE(nodeTest->IsRenderGroupExcludedStateChanged());

    nodeTest->SetRenderGroupExcludedStateChanged(false);
    EXPECT_FALSE(nodeTest->IsRenderGroupExcludedStateChanged());
}

/**
 * @tc.name: SetRenderGroupSubTreeDirtyTest
 * @tc.desc: Test SetRenderGroupSubTreeDirty
 * @tc.type: FUNC
 * @tc.require: issues/20738
 */
HWTEST_F(RSRenderNodeTest, SetRenderGroupSubTreeDirtyTest, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);
    nodeTest->InitRenderParams();
    EXPECT_NE(nodeTest->stagingRenderParams_, nullptr);

    nodeTest->SetRenderGroupSubTreeDirty(true);
    EXPECT_TRUE(nodeTest->IsRenderGroupSubTreeDirty());

    nodeTest->SetRenderGroupSubTreeDirty(false);
    EXPECT_FALSE(nodeTest->IsRenderGroupSubTreeDirty());
}

/**
 * @tc.name: InvalidateHierarchyTest018
 * @tc.desc: MarkParentNeedRegenerateChildren test
 * @tc.type: FUNC
 * @tc.require: issueI9US6V
 */
HWTEST_F(RSRenderNodeTest, InvalidateHierarchyTest018, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest1 = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest1, nullptr);

    std::shared_ptr<RSRenderNode> parentTest1 = nullptr;
    nodeTest1->parent_ = parentTest1;
    nodeTest1->MarkParentNeedRegenerateChildren();
    std::shared_ptr<RSRenderNode> parentTest2 = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(parentTest2, nullptr);
    nodeTest1->parent_ = parentTest2;
    nodeTest1->MarkParentNeedRegenerateChildren();
    EXPECT_FALSE(parentTest2->isChildrenSorted_);
}

/**
 * @tc.name: UpdateDrawableVecV2Test019
 * @tc.desc: UpdateDrawableVecV2 test
 * @tc.type: FUNC
 * @tc.require: issueI9US6V
 */
HWTEST_F(RSRenderNodeTest, UpdateDrawableVecV2Test019, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> renderNodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(renderNodeTest, nullptr);

    renderNodeTest->UpdateDrawableVecV2();

    renderNodeTest->dirtyTypesNG_.set(static_cast<size_t>(ModifierNG::RSModifierType::BOUNDS), true);
    renderNodeTest->dirtyTypesNG_.set(static_cast<size_t>(ModifierNG::RSModifierType::TRANSFORM), true);
    std::shared_ptr<DrawableTest> drawableTest1 = std::make_shared<DrawableTest>();
    renderNodeTest->GetDrawableVec(__func__).at(1) = drawableTest1;
    EXPECT_TRUE(renderNodeTest->dirtySlots_.empty());

    renderNodeTest->stagingRenderParams_ = std::make_unique<RSRenderParams>(0);
    renderNodeTest->UpdateDrawableVecV2();
    EXPECT_TRUE(renderNodeTest->dirtySlots_.count(RSDrawableSlot::CLIP_TO_BOUNDS));
    EXPECT_TRUE(renderNodeTest->dirtySlots_.count(RSDrawableSlot::MASK));
    EXPECT_EQ(renderNodeTest->dirtySlots_.size(), 2);

    renderNodeTest->dirtyTypesNG_.set(static_cast<size_t>(ModifierNG::RSModifierType::TRANSFORM), true);
    std::shared_ptr<DrawableTest> drawableTest2 = std::make_shared<DrawableTest>();
    renderNodeTest->GetDrawableVec(__func__).at(4) = drawableTest2;
    RSShadow rsShadow;
    std::optional<RSShadow> shadow(rsShadow);
    shadow->colorStrategy_ = SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_AVERAGE;
    renderNodeTest->renderProperties_.GetEffect().shadow_ = shadow;
    RRect rrect;
    renderNodeTest->renderProperties_.rrect_ = rrect;
    renderNodeTest->UpdateDrawableVecV2();
    EXPECT_EQ(renderNodeTest->dirtySlots_.size(), 2);
}

/**
 * @tc.name: UpdateDisplayListTest020
 * @tc.desc: UpdateDisplayList test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest, UpdateDisplayListTest020, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);

    // UpdateDisplayList test
    auto& vec = nodeTest->GetDrawableVec(__func__);
    for (auto i = 0; i < static_cast<int8_t>(RSDrawableSlot::MAX); ++i) {
        vec[i] = nullptr;
    }
    std::unique_ptr<RSRenderParams> stagingRenderParams = std::make_unique<RSRenderParams>(0);
    EXPECT_NE(stagingRenderParams, nullptr);
    nodeTest->stagingRenderParams_ = std::move(stagingRenderParams);
    nodeTest->drawCmdListNeedSync_ = false;
    nodeTest->UpdateDisplayList();
    EXPECT_TRUE(nodeTest->drawCmdListNeedSync_);

    nodeTest->drawableVecStatus_ = 48;
    std::shared_ptr<DrawableTest> drawableTest1 = std::make_shared<DrawableTest>();
    nodeTest->GetDrawableVec(__func__).at(1) = drawableTest1;
    nodeTest->drawCmdListNeedSync_ = false;
    nodeTest->UpdateDisplayList();
    EXPECT_TRUE(nodeTest->drawCmdListNeedSync_);

    nodeTest->drawableVecStatus_ = 32;
    nodeTest->UpdateDisplayList();
    EXPECT_EQ(nodeTest->stagingDrawCmdIndex_.contentIndex_, -1);
}

/**
 * @tc.name: UpdateRenderingTest021
 * @tc.desc: UpdateEffectRegion GetModifier UpdateShouldPaint SetSharedTransitionParam test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest, UpdateRenderingTest021, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);

    // UpdateEffectRegion test
    std::optional<Drawing::RectI> region = std::nullopt;
    nodeTest->UpdateEffectRegion(region, false);
    Drawing::RectI rectI;
    region = rectI;
    nodeTest->UpdateEffectRegion(region, false);
    nodeTest->renderProperties_.GetEffect().useEffect_ = true;
    nodeTest->UpdateEffectRegion(region, true);
    nodeTest->renderProperties_.hasHarmonium_ = true;
    nodeTest->UpdateEffectRegion(region, true);

    // FilterModifiersByPid test
    nodeTest->FilterModifiersByPid(1);

    // UpdateShouldPaint test
    nodeTest->renderProperties_.alpha_ = -1.0f;
    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(0.0f, 0.1f);
    nodeTest->renderProperties_.filter_ = filter;
    nodeTest->sharedTransitionParam_ = nullptr;
    nodeTest->UpdateShouldPaint();

    // SetSharedTransitionParam test
    std::shared_ptr<SharedTransitionParam> sharedTransitionParam = nullptr;
    nodeTest->sharedTransitionParam_ = nullptr;
    nodeTest->SetSharedTransitionParam(sharedTransitionParam);
    RSRenderNode::SharedPtr inNode = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(inNode, nullptr);
    RSRenderNode::SharedPtr outNode = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(outNode, nullptr);
    sharedTransitionParam = std::make_shared<SharedTransitionParam>(inNode, outNode, true);
    EXPECT_NE(sharedTransitionParam, nullptr);
    nodeTest->parent_ = std::make_shared<RSRenderNode>(0);
    nodeTest->SetSharedTransitionParam(sharedTransitionParam);
    EXPECT_NE(nodeTest->sharedTransitionParam_, nullptr);
}

/**
 * @tc.name: MarkFilterCacheFlags001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest, MarkFilterCacheFlags001, TestSize.Level1)
{
    RSRenderNode node(id, context);
    bool needRequestNextVsync = true;
    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    std::shared_ptr<DrawableV2::RSFilterDrawable> filterDrawable = std::make_shared<DrawableV2::RSFilterDrawable>();
    filterDrawable->MarkFilterForceUseCache(true);
    filterDrawable->MarkFilterForceClearCache();
    filterDrawable->stagingCacheManager_->pendingPurge_ = true;
    auto& properties = node.GetMutableRenderProperties();
    properties.backgroundFilter_ = std::make_shared<RSFilter>();
    properties.filter_ = std::make_shared<RSFilter>();
    node.MarkFilterCacheFlags(filterDrawable, *rsDirtyManager, needRequestNextVsync);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: MarkFilterCacheFlags002
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest, MarkFilterCacheFlags002, TestSize.Level1)
{
    RSRenderNode node(id, context);
    bool needRequestNextVsync = false;
    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    std::shared_ptr<DrawableV2::RSFilterDrawable> filterDrawable = std::make_shared<DrawableV2::RSFilterDrawable>();
    filterDrawable->MarkFilterForceUseCache(true);
    filterDrawable->MarkFilterForceClearCache();
    filterDrawable->MarkFilterRegionInteractWithDirty();
    filterDrawable->stagingCacheManager_->cacheUpdateInterval_ = 1;
    auto& properties = node.GetMutableRenderProperties();
    properties.backgroundFilter_ = std::make_shared<RSFilter>();
    properties.filter_ = std::make_shared<RSFilter>();
    node.MarkFilterCacheFlags(filterDrawable, *rsDirtyManager, needRequestNextVsync);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: CheckFilterCacheAndUpdateDirtySlots
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest, CheckFilterCacheAndUpdateDirtySlots, TestSize.Level1)
{
    RSRenderNode node(id, context);
    std::shared_ptr<DrawableV2::RSFilterDrawable> filterDrawable = std::make_shared<DrawableV2::RSFilterDrawable>();
    RSDrawableSlot slot = RSDrawableSlot::SHADOW;
    node.CheckFilterCacheAndUpdateDirtySlots(filterDrawable, slot);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: GetOldAbsMatrix001
 * @tc.desc: test GetOldAbsMatrix
 * @tc.type: FUNC
 * @tc.require: issueICAJPW
 */
HWTEST_F(RSRenderNodeTest, GetOldAbsMatrix001, TestSize.Level1)
{
    Drawing::Matrix testMatrix;
    constexpr float translateX{2.f};
    constexpr float translateY{3.f};
    testMatrix.Translate(translateX, translateY);

    RSRenderNode node(id, context);
    node.oldAbsMatrix_ = testMatrix;
    EXPECT_EQ(node.GetOldAbsMatrix(), testMatrix);
}

/**
 * @tc.name: UpdateAbsDirtyRegion001
 * @tc.desc: test UpdateAbsDirtyRegion
 * @tc.type: FUNC
 * @tc.require: issueICAJPW
 */
HWTEST_F(RSRenderNodeTest, UpdateAbsDirtyRegion001, TestSize.Level1)
{
    constexpr int surfaceWidth{1000};
    constexpr int surfaceHeight{2000};
    constexpr int defaultLeft{10};
    constexpr int defaultTop{20};
    constexpr int defaultWidth{500};
    constexpr int defaultHeight{600};

    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    rsDirtyManager->SetSurfaceSize(surfaceWidth, surfaceHeight);
    RSRenderNode node(id, context);
    node.GetMutableRenderProperties().subTreeAllDirty_ = true;
    node.oldChildrenRect_ = RectI(defaultLeft, defaultTop, defaultWidth, defaultHeight);
    node.oldClipRect_ = RectI(0, 0, defaultWidth, defaultHeight);

    RectI clipRect(0, 0, surfaceWidth, surfaceHeight);
    node.UpdateAbsDirtyRegion(*rsDirtyManager, clipRect);
    EXPECT_EQ(rsDirtyManager->GetCurrentFrameDirtyRegion(), node.oldChildrenRect_.IntersectRect(node.oldClipRect_));

    rsDirtyManager->Clear();
    node.isFirstLevelCrossNode_ = true;
    node.UpdateAbsDirtyRegion(*rsDirtyManager, clipRect);
    EXPECT_EQ(rsDirtyManager->GetCurrentFrameDirtyRegion(), node.oldChildrenRect_);
}

/**
 * @tc.name: UpdateDrawRectAndDirtyRegion001
 * @tc.desc: test UpdateDrawRectAndDirtyRegion with both foreground filter and background filter.
 * @tc.type: FUNC
 * @tc.require: issueICEQZR
 */
HWTEST_F(RSRenderNodeTest, UpdateDrawRectAndDirtyRegion001, TestSize.Level1)
{
    RSDirtyRegionManager rsDirtyManager;
    // forground filter
    RSRenderNode foregroundNode(id, context);
    auto& fgProperties = foregroundNode.GetMutableRenderProperties();
    fgProperties.filter_ = std::make_shared<RSFilter>();
    foregroundNode.UpdateDrawRectAndDirtyRegion(rsDirtyManager, false, RectI(), Drawing::Matrix());
    ASSERT_FALSE(foregroundNode.IsBackgroundInAppOrNodeSelfDirty());
    // background filter
    RSRenderNode backgroundNode(id, context);
    auto& bgProperties = backgroundNode.GetMutableRenderProperties();
    bgProperties.backgroundFilter_ = std::make_shared<RSFilter>();
    backgroundNode.UpdateDrawRectAndDirtyRegion(rsDirtyManager, false, RectI(), Drawing::Matrix());
    ASSERT_FALSE(backgroundNode.IsBackgroundInAppOrNodeSelfDirty());
    // material filter
    RSRenderNode materialNode(id, context);
    auto& materialProperties = materialNode.GetMutableRenderProperties();
    materialProperties.GetEffect().materialFilter_ = std::make_shared<RSFilter>();
    materialNode.UpdateDrawRectAndDirtyRegion(rsDirtyManager, false, RectI(), Drawing::Matrix());
    ASSERT_FALSE(materialNode.IsBackgroundInAppOrNodeSelfDirty());
}

/**
 * @tc.name: UpdateDrawRectAndDirtyRegion002
 * @tc.desc: test UpdateDrawRectAndDirtyRegion with both foreground filter and background filter.
 * @tc.type: FUNC
 * @tc.require: issueICI6YB
 */
HWTEST_F(RSRenderNodeTest, UpdateDrawRectAndDirtyRegion002, TestSize.Level1)
{
    std::vector<pid_t> pidList;
    pidList.emplace_back(ExtractPid(1));
    RSOptimizeCanvasDirtyCollector::GetInstance().SetOptimizeCanvasDirtyPidList(pidList);
    ASSERT_EQ(RSOptimizeCanvasDirtyCollector::GetInstance().IsOptimizeCanvasDirtyEnabled(1), true);
    auto optimizeCanvasDrawRegionEnabled = RSSystemProperties::GetOptimizeCanvasDrawRegionEnabled();
    if (!optimizeCanvasDrawRegionEnabled) {
        system::SetParameter("rosen.graphic.optimizeCanvasDrawRegion.enabled", "1");
    }
    RSDirtyRegionManager rsDirtyManager;
    // use cmdlistDrawRegion
    RSRenderNode node(id, context);
    auto& properties = node.GetMutableRenderProperties();
    properties.geoDirty_ = false;
    node.cmdlistDrawRegion_ = RectF(0.f, 0.f, 0.f, 0.f);
    node.needUseCmdlistDrawRegion_ = true;
    node.UpdateDrawRectAndDirtyRegion(rsDirtyManager, false, RectI(), Drawing::Matrix());

    // not use cmdlistDrawRegion
    node.needUseCmdlistDrawRegion_ = false;
    node.UpdateDrawRectAndDirtyRegion(rsDirtyManager, false, RectI(), Drawing::Matrix());
    system::SetParameter("rosen.graphic.optimizeCanvasDrawRegion.enabled",
        std::to_string(optimizeCanvasDrawRegionEnabled));
    pidList.assign(1, ExtractPid(INVALID_NODE_ID));
    RSOptimizeCanvasDirtyCollector::GetInstance().SetOptimizeCanvasDirtyPidList(pidList);
    ASSERT_EQ(RSOptimizeCanvasDirtyCollector::GetInstance().IsOptimizeCanvasDirtyEnabled(TARGET_NODE_ID), false);
}

/**
 * @tc.name: MarkForceClearFilterCacheWithInvisible
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest, MarkForceClearFilterCacheWithInvisible, TestSize.Level1)
{
    RSRenderNode node(id, context);
    auto& properties = node.GetMutableRenderProperties();
    node.MarkForceClearFilterCacheWithInvisible();
    properties.backgroundFilter_ = std::make_shared<RSFilter>();
    properties.filter_ = std::make_shared<RSFilter>();
    properties.GetEffect().materialFilter_ = std::make_shared<RSFilter>();
    node.MarkForceClearFilterCacheWithInvisible();
    auto filterDrawable = std::make_shared<DrawableV2::RSMaterialFilterDrawable>();
    node.GetDrawableVec(__func__)[static_cast<int8_t>(RSDrawableSlot::MATERIAL_FILTER)] = filterDrawable;
    node.MarkForceClearFilterCacheWithInvisible();
    ASSERT_TRUE(filterDrawable->stagingCacheManager_->stagingForceClearCache_);
}

/**
 * @tc.name: RenderTraceDebug
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest, RenderTraceDebug, TestSize.Level1)
{
    RSRenderNode node(id, context);
    node.RenderTraceDebug();
    ASSERT_TRUE(true);
}

/**
 * @tc.name: ApplyAlphaAndBoundsGeometry
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeTest, ApplyAlphaAndBoundsGeometry, TestSize.Level1)
{
    RSRenderNode node(id, context);
    canvas_->SetAlpha(1);
    float newAlpha = 0.8;
    node.renderProperties_.SetAlpha(newAlpha);
    node.renderProperties_.SetAlphaOffscreen(false);
    node.ApplyAlphaAndBoundsGeometry(*canvas_);
    ASSERT_TRUE(ROSEN_EQ(canvas_->GetAlpha(), newAlpha));
}

/**
 * @tc.name: ProcessRenderBeforeChildren
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest, ProcessRenderBeforeChildren, TestSize.Level1)
{
    RSRenderNode node(id, context);
    node.ProcessRenderBeforeChildren(*canvas_);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: ProcessTransitionAfterChildren
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest, ProcessTransitionAfterChildren, TestSize.Level1)
{
    RSRenderNode node(id, context);
    node.ProcessTransitionAfterChildren(*canvas_);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: IsCrossNodeTest
 * @tc.desc: test IsCrossNodeTest
 * @tc.type: FUNC
 * @tc.require: issueB2YOV
 */
HWTEST_F(RSRenderNodeTest, IsCrossNodeTest, TestSize.Level1)
{
    auto renderNode = std::make_shared<RSRenderNode>(1);
    ASSERT_NE(renderNode, nullptr);
    renderNode->SetIsCrossNode(true);
    ASSERT_TRUE(renderNode->isCrossNode_);

    renderNode->SetIsCrossNode(false);
    ASSERT_FALSE(renderNode->isCrossNode_);
}

/**
 * @tc.name: SetCrossNodeVisitedStatusTest
 * @tc.desc: test SetCrossNodeVisitedStatusTest
 * @tc.type: FUNC
 * @tc.require: issueIBV3N4
 */
HWTEST_F(RSRenderNodeTest, SetCrossNodeVisitedStatusTest, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(1);
    ASSERT_NE(node, nullptr);
    auto cloneNode = std::make_shared<RSRenderNode>(2);
    ASSERT_NE(cloneNode, nullptr);
    node->isCrossNode_ = true;
    cloneNode->isCloneCrossNode_ = true;
    cloneNode->sourceCrossNode_ = node;
    node->cloneCrossNodeVec_.push_back(cloneNode);

    node->SetCrossNodeVisitedStatus(true);
    ASSERT_TRUE(node->HasVisitedCrossNode());
    ASSERT_TRUE(cloneNode->HasVisitedCrossNode());

    node->SetCrossNodeVisitedStatus(false);
    ASSERT_FALSE(node->HasVisitedCrossNode());
    ASSERT_FALSE(cloneNode->HasVisitedCrossNode());

    cloneNode->SetCrossNodeVisitedStatus(true);
    ASSERT_TRUE(node->HasVisitedCrossNode());
    ASSERT_TRUE(cloneNode->HasVisitedCrossNode());

    cloneNode->SetCrossNodeVisitedStatus(false);
    ASSERT_FALSE(node->HasVisitedCrossNode());
    ASSERT_FALSE(cloneNode->HasVisitedCrossNode());
}

/**
 * @tc.name: UpdateDirtyRegionInfoForDFX001
 * @tc.desc: test if subTreeDirtyRegion can be correctly collected.
 * @tc.type: FUNC
 * @tc.require: issueA7UVD
 */
HWTEST_F(RSRenderNodeTest, UpdateDirtyRegionInfoForDFX001, TestSize.Level1)
{
    bool isPropertyChanged = false;
    if (RSSystemProperties::GetDirtyRegionDebugType() == DirtyRegionDebugType::DISABLED) {
        system::SetParameter("rosen.dirtyregiondebug.enabled", "1");
        ASSERT_NE(RSSystemProperties::GetDirtyRegionDebugType(), DirtyRegionDebugType::DISABLED);
        isPropertyChanged = true;
    }
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(DEFAULT_NODE_ID, context);
    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    canvasNode->lastFrameSubTreeSkipped_ = true;
    canvasNode->subTreeDirtyRegion_ = RectI(0, 0, DEFAULT_BOUNDS_SIZE, DEFAULT_BOUNDS_SIZE);
    // 'resize' added to avoid segmentation fault crash in
    // the RSDirtyRegionManager::UpdateDirtyRegionInfoForDfx() in line
    // dirtyCanvasNodeInfo_[dirtyType].emplace(std::make_pair(id, rect))
    rsDirtyManager->dirtyCanvasNodeInfo_.resize(DirtyRegionType::TYPE_AMOUNT);
    canvasNode->UpdateDirtyRegionInfoForDFX(*rsDirtyManager);
    EXPECT_FALSE(rsDirtyManager->dirtyCanvasNodeInfo_.empty());
    if (isPropertyChanged) {
        system::SetParameter("rosen.dirtyregiondebug.enabled", "0");
        ASSERT_EQ(RSSystemProperties::GetDirtyRegionDebugType(), DirtyRegionDebugType::DISABLED);
    }
}

/**
 * @tc.name: UpdateDirtyRegionInfoForDFX002
 * @tc.desc: test if absDrawRect can be correctly collected when clip property is true.
 * @tc.type: FUNC
 * @tc.require: issueA7UVD
 */
HWTEST_F(RSRenderNodeTest, UpdateDirtyRegionInfoForDFX002, TestSize.Level1)
{
    bool isPropertyChanged = false;
    if (RSSystemProperties::GetDirtyRegionDebugType() == DirtyRegionDebugType::DISABLED) {
        system::SetParameter("rosen.dirtyregiondebug.enabled", "1");
        ASSERT_NE(RSSystemProperties::GetDirtyRegionDebugType(), DirtyRegionDebugType::DISABLED);
        isPropertyChanged = true;
    }
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(DEFAULT_NODE_ID, context);
    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    auto& properties = canvasNode->GetMutableRenderProperties();
    properties.clipToBounds_ = true;
    canvasNode->absDrawRect_ = RectI(0, 0, DEFAULT_BOUNDS_SIZE, DEFAULT_BOUNDS_SIZE);
    // 'resize' added to avoid segmentation fault crash in
    // the RSDirtyRegionManager::UpdateDirtyRegionInfoForDfx() in line
    // dirtyCanvasNodeInfo_[dirtyType].emplace(std::make_pair(id, rect))
    rsDirtyManager->dirtyCanvasNodeInfo_.resize(DirtyRegionType::TYPE_AMOUNT);
    canvasNode->UpdateDirtyRegionInfoForDFX(*rsDirtyManager);
    EXPECT_FALSE(rsDirtyManager->dirtyCanvasNodeInfo_.empty());
    if (isPropertyChanged) {
        system::SetParameter("rosen.dirtyregiondebug.enabled", "0");
        ASSERT_EQ(RSSystemProperties::GetDirtyRegionDebugType(), DirtyRegionDebugType::DISABLED);
    }
}

/**
 * @tc.name: SetNodeName
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: IAJ46S
 */
HWTEST_F(RSRenderNodeTest, SetNodeName, TestSize.Level1)
{
    RSRenderNode node(id, context);

    std::string nodeName = "";
    node.SetNodeName(nodeName);
    auto name = node.GetNodeName();
    ASSERT_EQ(name, "");

    nodeName = "0";
    node.SetNodeName(nodeName);
    name = node.GetNodeName();
    ASSERT_EQ(name, "0");
}

/**
 * @tc.name: SetAccumulatedClipFlagTest
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: IAJ46S
 */
HWTEST_F(RSRenderNodeTest, SetAccumulatedClipFlagTest, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    ASSERT_TRUE(nodeTest->SetAccumulatedClipFlag(true));
    ASSERT_FALSE(nodeTest->SetAccumulatedClipFlag(false));
}

/**
 * @tc.name: GetIsFullChildrenListValid
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeTest, GetIsFullChildrenListValid, TestSize.Level1)
{
    auto renderNode = std::make_shared<RSRenderNode>(1);
    ASSERT_NE(renderNode, nullptr);
    renderNode->isFullChildrenListValid_ = true;
    ASSERT_TRUE(renderNode->GetIsFullChildrenListValid());
    renderNode->isFullChildrenListValid_ = false;
    ASSERT_FALSE(renderNode->GetIsFullChildrenListValid());
}

/**
 * @tc.name: IsPureBackgroundColor
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeTest, IsPureBackgroundColorTest, TestSize.Level1)
{
    auto rsRenderNode = std::make_shared<RSRenderNode>(1);
    ASSERT_NE(rsRenderNode, nullptr);
    auto& drawableVec = rsRenderNode->GetDrawableVec(__func__);
    for (auto i = static_cast<int8_t>(RSDrawableSlot::SAVE_ALL); i < static_cast<int8_t>(RSDrawableSlot::MAX); ++i) {
        drawableVec[i] = nullptr;
    }
    bool result = rsRenderNode->IsPureBackgroundColor();
    EXPECT_TRUE(result);

    drawableVec[static_cast<int8_t>(RSDrawableSlot::CLIP_TO_BOUNDS)] = std::make_shared<DrawableTest>();
    result = rsRenderNode->IsPureBackgroundColor();
    EXPECT_TRUE(result);

    drawableVec[static_cast<int8_t>(RSDrawableSlot::CLIP_TO_BOUNDS)] = nullptr;
    result = rsRenderNode->IsPureBackgroundColor();
    EXPECT_TRUE(result);

    for (int8_t i = 0; i < static_cast<int8_t>(RSDrawableSlot::MAX); ++i) {
        drawableVec[i] = std::make_shared<DrawableTest>();
    }
    result = rsRenderNode->IsPureBackgroundColor();
    EXPECT_FALSE(result);
}

/**
 * @tc.name: IsPureBackgroundColorTest002
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issue21014
 */
HWTEST_F(RSRenderNodeTest, IsPureBackgroundColorTest002, TestSize.Level1)
{
    auto rsRenderNode = std::make_shared<RSRenderNode>(1);
    ASSERT_NE(rsRenderNode, nullptr);
    auto& drawableVec = rsRenderNode->GetDrawableVec(__func__);

    drawableVec[static_cast<int8_t>(RSDrawableSlot::BLENDER)] = std::make_shared<DrawableTest>();
    bool result = rsRenderNode->IsPureBackgroundColor();
    EXPECT_FALSE(result);

    auto& property = rsRenderNode->GetMutableRenderProperties();
    property.SetColorBlendMode(static_cast<int>(RSColorBlendMode::SRC_OVER));
    result = rsRenderNode->IsPureBackgroundColor();
    EXPECT_TRUE(result);

    property.SetColorBlendApplyType(static_cast<int>(RSColorBlendApplyType::SAVE_LAYER));
    result = rsRenderNode->IsPureBackgroundColor();
    EXPECT_FALSE(result);

    drawableVec[static_cast<int8_t>(RSDrawableSlot::BACKGROUND_SHADER)] = std::make_shared<DrawableTest>();
    result = rsRenderNode->IsPureBackgroundColor();
    EXPECT_FALSE(result);
}

/**
 * @tc.name: SetUIContextTokenTest001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeTest, SetUIContextTokenTest001, TestSize.Level1)
{
    auto renderNode = std::make_shared<RSRenderNode>(1);
    ASSERT_NE(renderNode, nullptr);
    uint64_t token = 1001;
    renderNode->SetUIContextToken(token);
    ASSERT_EQ(renderNode->GetUIContextToken(), token);
    renderNode->SetUIContextToken(token);
    ASSERT_EQ(renderNode->uiContextTokenList_.size(), 1);
}

/**
 * @tc.name: GetUIContextTokenListTest001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueICPQSU
 */
HWTEST_F(RSRenderNodeTest, GetUIContextTokenListTest001, TestSize.Level1)
{
    auto renderNode = std::make_shared<RSRenderNode>(1);
    ASSERT_NE(renderNode, nullptr);
    uint64_t token = 1001;
    renderNode->SetUIContextToken(token);
    auto uiContextTokenList = renderNode->GetUIContextTokenList();
    ASSERT_EQ(uiContextTokenList.back(), token);
}

/**
 * @tc.name: UpdateFilterCacheForceClearWithBackgroundAndAlphaDirtyTest
 * @tc.desc: Test function UpdateFilterCacheWithBackgroundDirty
 * @tc.type: FUNC
 * @tc.require: issueICD8D6
 */
HWTEST_F(RSRenderNodeTest, UpdateFilterCacheForceClearWithBackgroundAndAlphaDirtyTest, TestSize.Level1)
{
    auto renderNode = std::make_shared<RSRenderNode>(1);
    ASSERT_NE(renderNode, nullptr);
    renderNode->dirtyTypesNG_.set(static_cast<size_t>(ModifierNG::RSModifierType::ALPHA), true);

    auto backgroundColorDrawable = std::make_shared<DrawableV2::RSBackgroundColorDrawable>();
    EXPECT_NE(backgroundColorDrawable, nullptr);
    renderNode->GetDrawableVec(__func__)[static_cast<uint32_t>(RSDrawableSlot::BACKGROUND_COLOR)]
        = backgroundColorDrawable;

    renderNode->dirtySlots_.emplace(RSDrawableSlot::BACKGROUND_FILTER);
    auto backgroundFilterDrawable = std::make_shared<DrawableV2::RSBackgroundFilterDrawable>();
    EXPECT_NE(backgroundFilterDrawable, nullptr);
    renderNode->GetDrawableVec(__func__)[static_cast<uint32_t>(RSDrawableSlot::BACKGROUND_FILTER)]
        = backgroundFilterDrawable;
    renderNode->UpdateFilterCacheWithBackgroundDirty();

    EXPECT_NE(backgroundFilterDrawable->stagingCacheManager_, nullptr);
    EXPECT_EQ(backgroundFilterDrawable->stagingCacheManager_->stagingForceClearCache_, true);
}

/**
 * @tc.name: NotForceClearFilterCacheWithoutBackgroundDirtyTest
 * @tc.desc: Test function UpdateFilterCacheWithBackgroundDirty
 * @tc.type: FUNC
 * @tc.require: issueICD8D6
 */
HWTEST_F(RSRenderNodeTest, NotForceClearFilterCacheWithoutBackgroundDirtyTest, TestSize.Level1)
{
    auto renderNode = std::make_shared<RSRenderNode>(1);
    ASSERT_NE(renderNode, nullptr);
    renderNode->UpdateFilterCacheWithBackgroundDirty();
    renderNode->dirtyTypesNG_.set(static_cast<size_t>(ModifierNG::RSModifierType::ALPHA), true);

    renderNode->dirtySlots_.emplace(RSDrawableSlot::BACKGROUND_FILTER);
    auto backgroundFilterDrawable = std::make_shared<DrawableV2::RSBackgroundFilterDrawable>();
    EXPECT_NE(backgroundFilterDrawable, nullptr);
    renderNode->GetDrawableVec(__func__)[static_cast<uint32_t>(RSDrawableSlot::BACKGROUND_FILTER)]
        = backgroundFilterDrawable;
    auto filterDrawable = std::make_shared<DrawableV2::RSMaterialFilterDrawable>();
    renderNode->GetDrawableVec(__func__)[static_cast<int8_t>(RSDrawableSlot::MATERIAL_FILTER)] = filterDrawable;
    renderNode->GetMutableRenderProperties().GetEffect().materialFilter_ = std::make_shared<RSFilter>();
    renderNode->MarkForceClearFilterCacheWithInvisible();
    renderNode->UpdateFilterCacheWithBackgroundDirty();
    EXPECT_NE(backgroundFilterDrawable->stagingCacheManager_, nullptr);
    EXPECT_EQ(backgroundFilterDrawable->stagingCacheManager_->stagingForceClearCache_, true);
    EXPECT_NE(filterDrawable->stagingCacheManager_, nullptr);
    EXPECT_EQ(filterDrawable->stagingCacheManager_->stagingForceClearCache_, true);
}

/**
 * @tc.name: NotForceClearFilterCacheWithoutAlphaDirtyTest
 * @tc.desc: Test function UpdateFilterCacheWithBackgroundDirty
 * @tc.type: FUNC
 * @tc.require: issueICD8D6
 */
HWTEST_F(RSRenderNodeTest, NotForceClearFilterCacheWithoutAlphaDirtyTest, TestSize.Level1)
{
    auto renderNode = std::make_shared<RSRenderNode>(1);
    ASSERT_NE(renderNode, nullptr);

    auto backgroundColorDrawable = std::make_shared<DrawableV2::RSBackgroundColorDrawable>();
    EXPECT_NE(backgroundColorDrawable, nullptr);
    renderNode->GetDrawableVec(__func__)[static_cast<uint32_t>(RSDrawableSlot::BACKGROUND_COLOR)]
        = backgroundColorDrawable;

    renderNode->dirtySlots_.emplace(RSDrawableSlot::BACKGROUND_FILTER);
    auto backgroundFilterDrawable = std::make_shared<DrawableV2::RSBackgroundFilterDrawable>();
    EXPECT_NE(backgroundFilterDrawable, nullptr);
    renderNode->GetDrawableVec(__func__)[static_cast<uint32_t>(RSDrawableSlot::BACKGROUND_FILTER)]
        = backgroundFilterDrawable;
    renderNode->UpdateFilterCacheWithBackgroundDirty();

    EXPECT_NE(backgroundFilterDrawable->stagingCacheManager_, nullptr);
    EXPECT_EQ(backgroundFilterDrawable->stagingCacheManager_->stagingForceClearCache_, false);
}

/**
 * @tc.name: NotForceClearFilterCacheWithoutBackBackgoundImageDirtyTest
 * @tc.desc: Test function UpdateFilterCacheWithBackgroundDirty
 * @tc.type: FUNC
 * @tc.require: issueICD8D6
 */
HWTEST_F(RSRenderNodeTest, NotForceClearFilterCacheWithoutBackBackgoundImageDirtyTest, TestSize.Level1)
{
    auto renderNode = std::make_shared<RSRenderNode>(1);
    ASSERT_NE(renderNode, nullptr);

    auto backgroundImageDrawable = std::make_shared<DrawableV2::RSBackgroundImageDrawable>();
    EXPECT_NE(backgroundImageDrawable, nullptr);
    renderNode->GetDrawableVec(__func__)[static_cast<uint32_t>(RSDrawableSlot::BACKGROUND_IMAGE)]
        = backgroundImageDrawable;

    renderNode->dirtySlots_.emplace(RSDrawableSlot::BACKGROUND_FILTER);
    auto backgroundFilterDrawable = std::make_shared<DrawableV2::RSBackgroundFilterDrawable>();
    EXPECT_NE(backgroundFilterDrawable, nullptr);
    renderNode->GetDrawableVec(__func__)[static_cast<uint32_t>(RSDrawableSlot::BACKGROUND_FILTER)]
        = backgroundFilterDrawable;
    renderNode->UpdateFilterCacheWithBackgroundDirty();

    EXPECT_NE(backgroundFilterDrawable->stagingCacheManager_, nullptr);
    EXPECT_EQ(backgroundFilterDrawable->stagingCacheManager_->stagingForceClearCache_, false);
}

/**
 * @tc.name: ForceClearFilterCacheWhenBackgroundDirty
 * @tc.desc: Test function UpdateFilterCacheWithBackgroundDirty
 * @tc.type: FUNC
 * @tc.require: issueICD8D6
 */
HWTEST_F(RSRenderNodeTest, ForceClearFilterCacheWhenBackgroundDirty, TestSize.Level1)
{
    auto renderNode = std::make_shared<RSRenderNode>(1);
    ASSERT_NE(renderNode, nullptr);
    auto& properties = renderNode->GetMutableRenderProperties();
    properties.backgroundFilter_ = std::make_shared<RSFilter>();

    renderNode->dirtySlots_.emplace(RSDrawableSlot::BACKGROUND_COLOR);
    auto backgroundFilterDrawable = std::make_shared<DrawableV2::RSBackgroundFilterDrawable>();
    EXPECT_NE(backgroundFilterDrawable, nullptr);
    renderNode->GetDrawableVec(__func__)[static_cast<uint32_t>(RSDrawableSlot::BACKGROUND_FILTER)]
        = backgroundFilterDrawable;
    renderNode->CheckBlurFilterCacheNeedForceClearOrSave();

    EXPECT_NE(backgroundFilterDrawable->stagingCacheManager_, nullptr);
    EXPECT_EQ(backgroundFilterDrawable->stagingCacheManager_->stagingForceClearCache_, true);
}

/**
 * @tc.name: ForceClearForegroundFilterCacheWhenDirty
 * @tc.desc: Test function CheckBlurFilterCacheNeedForceClearOrSave
 * @tc.type: FUNC
 * @tc.require: issueICD8D6
 */
HWTEST_F(RSRenderNodeTest, ForceClearForegroundFilterCacheWhenDirty, TestSize.Level1)
{
    auto renderNode = std::make_shared<RSRenderNode>(1);
    ASSERT_NE(renderNode, nullptr);
    auto& properties = renderNode->GetMutableRenderProperties();
    properties.filter_ = std::make_shared<RSFilter>();

    renderNode->dirtySlots_.emplace(RSDrawableSlot::CHILDREN);
    auto compositingFilterDrawable = std::make_shared<DrawableV2::RSCompositingFilterDrawable>();
    EXPECT_NE(compositingFilterDrawable, nullptr);
    renderNode->GetDrawableVec(__func__)[static_cast<uint32_t>(RSDrawableSlot::COMPOSITING_FILTER)]
        = compositingFilterDrawable;
    renderNode->CheckBlurFilterCacheNeedForceClearOrSave();

    EXPECT_NE(compositingFilterDrawable->stagingCacheManager_, nullptr);
    EXPECT_EQ(compositingFilterDrawable->stagingCacheManager_->stagingForceClearCache_, true);
}

/**
 * @tc.name: HasHpaeBackgroundFilter
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: wzwz
 */
HWTEST_F(RSRenderNodeTest, HasHpaeBackgroundFilter, TestSize.Level1)
{
    auto renderNode = std::make_shared<RSRenderNode>(1);
    ASSERT_NE(renderNode, nullptr);
    ASSERT_FALSE(renderNode->HasHpaeBackgroundFilter());

    auto drawableFilter = std::make_shared<DrawableV2::RSCompositingFilterDrawable>();
    EXPECT_NE(drawableFilter, nullptr);
    renderNode->GetDrawableVec(__func__)[static_cast<uint32_t>(RSDrawableSlot::COMPOSITING_FILTER)] = drawableFilter;
    ASSERT_TRUE(renderNode->HasHpaeBackgroundFilter());
}
/*
 * @tc.name: UpdateVirtualScreenWhiteListInfo
 * @tc.desc: Test function UpdateVirtualScreenWhiteListInfo
 * @tc.type: FUNC
 * @tc.require: issueICF7P6
 */
HWTEST_F(RSRenderNodeTest, UpdateVirtualScreenWhiteListInfo, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(1);
    ASSERT_NE(node, nullptr);
    std::shared_ptr<RSRenderNode> parent = nullptr;
    node->SetParent(parent);
    ASSERT_EQ(node->parent_.lock(), nullptr);
    node->UpdateVirtualScreenWhiteListInfo();
    parent = std::make_shared<RSRenderNode>(id + 1);
    node->SetParent(parent);
    ASSERT_NE(node->parent_.lock(), nullptr);
    ScreenId screenId = 1;
    node->hasVirtualScreenWhiteList_[screenId] = false;
    node->UpdateVirtualScreenWhiteListInfo();
}

/*
 * @tc.name: GetNeedUseCmdlistDrawRegion001
 * @tc.desc: Test function GetNeedUseCmdlistDrawRegion when application to which the node belongs is not targetScene
 * @tc.type: FUNC
 * @tc.require: issueICI6YB
 */
HWTEST_F(RSRenderNodeTest, GetNeedUseCmdlistDrawRegion001, TestSize.Level1)
{
    std::vector<pid_t> pidList;
    pidList.emplace_back(ExtractPid(TARGET_NODE_ID));
    RSOptimizeCanvasDirtyCollector::GetInstance().SetOptimizeCanvasDirtyPidList(pidList);
    ASSERT_EQ(RSOptimizeCanvasDirtyCollector::GetInstance().IsOptimizeCanvasDirtyEnabled(1), false);

    auto optimizeCanvasDrawRegionEnabled = RSSystemProperties::GetOptimizeCanvasDrawRegionEnabled();
    if (!optimizeCanvasDrawRegionEnabled) {
        system::SetParameter("rosen.graphic.optimizeCanvasDrawRegion.enabled", "1");
    }
    auto node = std::make_shared<RSRenderNode>(1);
    ASSERT_NE(node, nullptr);
    RectF rect { 1.0f, 1.0f, 1.0f, 1.0f };
    node->cmdlistDrawRegion_ = rect;
    node->SetNeedUseCmdlistDrawRegion(false);
    ASSERT_EQ(node->GetNeedUseCmdlistDrawRegion(), false);
    node->SetNeedUseCmdlistDrawRegion(true);
    ASSERT_EQ(node->GetNeedUseCmdlistDrawRegion(), false);
    system::SetParameter("rosen.graphic.optimizeCanvasDrawRegion.enabled",
        std::to_string(optimizeCanvasDrawRegionEnabled));
    pidList.assign(1, ExtractPid(INVALID_NODE_ID));
    RSOptimizeCanvasDirtyCollector::GetInstance().SetOptimizeCanvasDirtyPidList(pidList);
    ASSERT_EQ(RSOptimizeCanvasDirtyCollector::GetInstance().IsOptimizeCanvasDirtyEnabled(TARGET_NODE_ID), false);
}

/*
 * @tc.name: GetNeedUseCmdlistDrawRegion002
 * @tc.desc: Test function GetNeedUseCmdlistDrawRegion when application to which the node belongs is not targetScene
 * @tc.type: FUNC
 * @tc.require: issueICI6YB
 */
HWTEST_F(RSRenderNodeTest, GetNeedUseCmdlistDrawRegion002, TestSize.Level1)
{
    std::vector<pid_t> pidList;
    pidList.emplace_back(ExtractPid(TARGET_NODE_ID));
    RSOptimizeCanvasDirtyCollector::GetInstance().SetOptimizeCanvasDirtyPidList(pidList);
    ASSERT_EQ(RSOptimizeCanvasDirtyCollector::GetInstance().IsOptimizeCanvasDirtyEnabled(TARGET_NODE_ID), true);

    auto optimizeCanvasDrawRegionEnabled = RSSystemProperties::GetOptimizeCanvasDrawRegionEnabled();
    if (!optimizeCanvasDrawRegionEnabled) {
        system::SetParameter("rosen.graphic.optimizeCanvasDrawRegion.enabled", "1");
    }
    auto node = std::make_shared<RSRenderNode>(TARGET_NODE_ID);
    ASSERT_NE(node, nullptr);
    RectF rect { 1.0f, 1.0f, 1.0f, 1.0f };
    node->cmdlistDrawRegion_ = rect;
    node->SetNeedUseCmdlistDrawRegion(false);
    ASSERT_EQ(node->GetNeedUseCmdlistDrawRegion(), false);
    node->SetNeedUseCmdlistDrawRegion(true);
    ASSERT_EQ(node->GetNeedUseCmdlistDrawRegion(), true);
    system::SetParameter("rosen.graphic.optimizeCanvasDrawRegion.enabled",
        std::to_string(optimizeCanvasDrawRegionEnabled));
    pidList.assign(1, ExtractPid(INVALID_NODE_ID));
    RSOptimizeCanvasDirtyCollector::GetInstance().SetOptimizeCanvasDirtyPidList(pidList);
    ASSERT_EQ(RSOptimizeCanvasDirtyCollector::GetInstance().IsOptimizeCanvasDirtyEnabled(TARGET_NODE_ID), false);
}

/*
 * @tc.name: ResetDirtyStatusTest001
 * @tc.desc: Test function ResetDirtyStatus
 * @tc.type: FUNC
 * @tc.require: issueICI6YB
 */
HWTEST_F(RSRenderNodeTest, ResetDirtyStatusTest001, TestSize.Level1)
{
    auto renderNode = std::make_shared<RSRenderNode>(1);
    ASSERT_NE(renderNode, nullptr);
    renderNode->isLastVisible_ = true;
    renderNode->shouldPaint_ = false;
    renderNode->cmdlistDrawRegion_ = RectF(1.0f, 1.0f, 1.0f, 1.0f);
    renderNode->ResetDirtyStatus();
    EXPECT_FALSE(renderNode->isLastVisible_);
    EXPECT_FALSE(renderNode->shouldPaint_);
    EXPECT_TRUE(renderNode->cmdlistDrawRegion_.IsEmpty());
}

/**
 * @tc.name: ResetParent
 * @tc.desc: test results of ResetParent
 * @tc.type:FUNC
 * @tc.require: issueICI6YB
 */
HWTEST_F(RSRenderNodeTest, ResetParent, TestSize.Level1)
{
    std::vector<pid_t> pidList;
    pidList.emplace_back(ExtractPid(1));
    RSOptimizeCanvasDirtyCollector::GetInstance().SetOptimizeCanvasDirtyPidList(pidList);
    ASSERT_EQ(RSOptimizeCanvasDirtyCollector::GetInstance().IsOptimizeCanvasDirtyEnabled(1), true);
    auto optimizeCanvasDrawRegionEnabled = RSSystemProperties::GetOptimizeCanvasDrawRegionEnabled();
    if (!optimizeCanvasDrawRegionEnabled) {
        system::SetParameter("rosen.graphic.optimizeCanvasDrawRegion.enabled", "1");
    }
    auto node1 = std::make_shared<RSRenderNode>(1);
    node1->needUseCmdlistDrawRegion_ = true;
    node1->ResetParent();
    ASSERT_EQ(node1->parent_.lock(), nullptr);

    auto node2 = std::make_shared<RSRenderNode>(1);
    node2->needUseCmdlistDrawRegion_ = false;
    node2->ResetParent();
    ASSERT_EQ(node2->parent_.lock(), nullptr);
    system::SetParameter("rosen.graphic.optimizeCanvasDrawRegion.enabled",
        std::to_string(optimizeCanvasDrawRegionEnabled));
    pidList.assign(1, ExtractPid(INVALID_NODE_ID));
    RSOptimizeCanvasDirtyCollector::GetInstance().SetOptimizeCanvasDirtyPidList(pidList);
    ASSERT_EQ(RSOptimizeCanvasDirtyCollector::GetInstance().IsOptimizeCanvasDirtyEnabled(TARGET_NODE_ID), false);
}

class RSC_EXPORT MockRSRenderNode : public RSRenderNode {
public:
    explicit MockRSRenderNode(NodeId id, const std::weak_ptr<RSContext>& context = {}, bool isTextureExportNode = false)
        : RSRenderNode(id, context, isTextureExportNode) {}
    virtual ~MockRSRenderNode() = default;
    MOCK_METHOD0(ResetAndApplyModifiers, void());
};

/**
 * @tc.name: ResetAndApplyModifiers001
 * @tc.desc: test results of ResetAndApplyModifiers
 * @tc.type:FUNC
 * @tc.require: issueICI6YB
 */
HWTEST_F(RSRenderNodeTest, ResetAndApplyModifiers001, TestSize.Level1)
{
    std::vector<pid_t> pidList;
    pidList.emplace_back(ExtractPid(TARGET_NODE_ID));
    RSOptimizeCanvasDirtyCollector::GetInstance().SetOptimizeCanvasDirtyPidList(pidList);
    ASSERT_EQ(RSOptimizeCanvasDirtyCollector::GetInstance().IsOptimizeCanvasDirtyEnabled(TARGET_NODE_ID), true);
    auto optimizeCanvasDrawRegionEnabled = RSSystemProperties::GetOptimizeCanvasDrawRegionEnabled();
    if (!optimizeCanvasDrawRegionEnabled) {
        system::SetParameter("rosen.graphic.optimizeCanvasDrawRegion.enabled", "1");
    }
    auto node = std::make_shared<MockRSRenderNode>(1);
    EXPECT_CALL(*node, ResetAndApplyModifiers()).Times(1);
    node->needUseCmdlistDrawRegion_ = true;
    std::shared_ptr<Drawing::DrawCmdList> drawCmdList = nullptr;
    auto property = std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>();
    property->GetRef() = drawCmdList;
    ModifierId id = 1;
    auto modifier = ModifierNG::RSRenderModifier::MakeRenderModifier(
        ModifierNG::RSModifierType::CONTENT_STYLE, property, id, ModifierNG::RSPropertyType::CONTENT_STYLE);
    ASSERT_NE(modifier, nullptr);
    RSRootRenderNode::ModifierNGContainer modifiers { modifier };
    node->modifiersNG_.emplace(ModifierNG::RSModifierType::CONTENT_STYLE, modifiers);
    node->ResetAndApplyModifiers();
    system::SetParameter("rosen.graphic.optimizeCanvasDrawRegion.enabled",
        std::to_string(optimizeCanvasDrawRegionEnabled));
    pidList.assign(1, ExtractPid(INVALID_NODE_ID));
    RSOptimizeCanvasDirtyCollector::GetInstance().SetOptimizeCanvasDirtyPidList(pidList);
    ASSERT_EQ(RSOptimizeCanvasDirtyCollector::GetInstance().IsOptimizeCanvasDirtyEnabled(TARGET_NODE_ID), false);
}

/*
 * @tc.name: RepaintBoundary
 * @tc.desc: Test function RepaintBoundary
 * @tc.type: FUNC
 * @tc.require: issueICI6YB
 */
HWTEST_F(RSRenderNodeTest, RepaintBoundary, TestSize.Level1)
{
    auto renderNode = std::make_shared<RSRenderNode>(1);
    ASSERT_NE(renderNode, nullptr);
    renderNode->MarkRepaintBoundary(true);
    ASSERT_EQ(renderNode->IsRepaintBoundary(), true);
}

#ifdef SUBTREE_PARALLEL_ENABLE
/*
 * @tc.name: UpdateSubTreeParallelNodes
 * @tc.desc: Test function UpdateSubTreeParallelNodes
 * @tc.type: FUNC
 * @tc.require: issueICI6YB
 */
HWTEST_F(RSRenderNodeTest, UpdateSubTreeParallelNodesTest, TestSize.Level1)
{
    auto renderNode = std::make_shared<RSRenderNode>(1);
    ASSERT_NE(renderNode, nullptr);
    renderNode->drawingCacheType_ = RSDrawingCacheType::DISABLED_CACHE;

    renderNode->isRepaintBoundary_ = false;
    renderNode->UpdateSubTreeParallelNodes();

    renderNode->isRepaintBoundary_ = true;
    renderNode->UpdateSubTreeParallelNodes();

    for (int i = 2; i < 8; i++) {
        auto node = std::make_shared<RSRenderNode>(i);
        renderNode->children_.emplace_back(node);
    }

    renderNode->isAllChildRepaintBoundary_ = false;
    ASSERT_FALSE(renderNode->GetChildrenCount() <= 5);
    renderNode->UpdateSubTreeParallelNodes();

    renderNode->isAllChildRepaintBoundary_ = true;
    renderNode->UpdateSubTreeParallelNodes();

    renderNode->childHasVisibleEffect_ = true;
    renderNode->UpdateSubTreeParallelNodes();

    renderNode->childHasVisibleEffect_ = false;
    renderNode->UpdateSubTreeParallelNodes();

    renderNode->drawingCacheType_ = RSDrawingCacheType::TARGETED_CACHE;
    renderNode->UpdateSubTreeParallelNodes();
}
#endif

/*
 * @tc.name: RemoveModifier
 * @tc.desc: Test function RemoveModifier
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeTest, RemoveModifier, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);

    std::shared_ptr<Drawing::DrawCmdList> drawCmdList = nullptr;
    auto property = std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>();
    property->GetRef() = drawCmdList;
    ModifierId id = 1;
    auto modifier = ModifierNG::RSRenderModifier::MakeRenderModifier(
        ModifierNG::RSModifierType::CONTENT_STYLE, property, id, ModifierNG::RSPropertyType::CONTENT_STYLE);

    nodeTest->RemoveModifier(ModifierNG::RSModifierType::BOUNDS, id);
    RSRootRenderNode::ModifierNGContainer modifiers { modifier };
    nodeTest->modifiersNG_.emplace(ModifierNG::RSModifierType::BOUNDS, modifiers);
    nodeTest->RemoveModifier(ModifierNG::RSModifierType::BOUNDS, id);
}

/*
 * @tc.name: GetHDRBrightness
 * @tc.desc: Test function GetHDRBrightness
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeTest, GetHDRBrightness, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);

    std::shared_ptr<Drawing::DrawCmdList> drawCmdList = nullptr;
    auto property = std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>();
    property->GetRef() = drawCmdList;
    ModifierId id = 1;
    auto modifier = ModifierNG::RSRenderModifier::MakeRenderModifier(
        ModifierNG::RSModifierType::HDR_BRIGHTNESS, property, id, ModifierNG::RSPropertyType::HDR_BRIGHTNESS);
    RSRootRenderNode::ModifierNGContainer modifiers { modifier };
    nodeTest->modifiersNG_.emplace(ModifierNG::RSModifierType::HDR_BRIGHTNESS, modifiers);
    float result = nodeTest->GetHDRBrightness();
    ASSERT_EQ(result, 0.0f);
}

/**
 * @tc.name: UpdateNodeColorSpace
 * @tc.desc: Test function UpdateNodeColorSpace
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeTest, UpdateNodeColorSpace, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    nodeTest->InitRenderParams();

    nodeTest->UpdateNodeColorSpace();
    EXPECT_EQ(nodeTest->GetNodeColorSpace(), GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
}

/**
 * @tc.name: SetAndResetNodeColorSpace
 * @tc.desc: Test function SetNodeColorSpace and ResetNodeColorSpace
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeTest, SetAndResetNodeColorSpace, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    nodeTest->InitRenderParams();

    nodeTest->SetNodeColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
    EXPECT_EQ(nodeTest->GetNodeColorSpace(), GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);

    nodeTest->ResetNodeColorSpace();
    EXPECT_EQ(nodeTest->GetNodeColorSpace(), GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
}

/**
 * @tc.name: UpdateShadowRectTest001
 * @tc.desc: UpdateShadowRect test
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeTest, UpdateShadowRectTest001, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);

    auto drawable = std::make_shared<DrawableV2::RSShadowDrawable>();
    drawable->colorStrategy_ = SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_NONE;

    // case 1 : nullptr
    nodeTest->UpdateShadowRect();

    // case 2 : non-nullptr + COLOR_STRATEGY_NONE
    nodeTest->GetDrawableVec(__func__).at(static_cast<int8_t>(RSDrawableSlot::SHADOW)) = drawable;
    nodeTest->UpdateShadowRect();

    // case 3 : non-nullptr + COLOR_STRATEGY_AVERAGE
    drawable->colorStrategy_ = SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_AVERAGE;
    nodeTest->UpdateShadowRect();
}

/**
 * @tc.name: ModifierDeduplicationWithEnableTest
 * @tc.desc: Test BOUNDS modifier deduplication when deduplication is enabled
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeTest, ModifierDeduplicationWithEnableTest, TestSize.Level1)
{
    RSRenderNode node(id, context);
    ModifierId modifierId = 100;

    // Create first BOUNDS modifier with bounds (0, 0, 100, 100)
    Vector4f bounds1(0.0f, 0.0f, 100.0f, 100.0f);
    auto property1 = std::make_shared<RSRenderProperty<Vector4f>>();
    property1->GetRef() = bounds1;
    auto modifier1 = ModifierNG::RSRenderModifier::MakeRenderModifier(
        ModifierNG::RSModifierType::BOUNDS, property1, modifierId, ModifierNG::RSPropertyType::BOUNDS);
    modifier1->SetDeduplicationEnabled(true);  // enable deduplication

    // Add first modifier
    node.AddModifier(modifier1);

    // Verify modifier is added
    auto& modifiersNG = node.modifiersNG_;
    auto it = modifiersNG.find(ModifierNG::RSModifierType::BOUNDS);
    ASSERT_TRUE(it != modifiersNG.end());
    ASSERT_EQ(it->second.size(), 1u);

    // Create second BOUNDS modifier with same ID but different bounds (0, 0, 200, 200)
    Vector4f bounds2(0.0f, 0.0f, 200.0f, 200.0f);
    auto property2 = std::make_shared<RSRenderProperty<Vector4f>>();
    property2->GetRef() = bounds2;
    auto modifier2 = ModifierNG::RSRenderModifier::MakeRenderModifier(
        ModifierNG::RSModifierType::BOUNDS, property2, modifierId, ModifierNG::RSPropertyType::BOUNDS);
    modifier2->SetDeduplicationEnabled(true);  // enable deduplication

    // Add second modifier with same ID
    node.AddModifier(modifier2);

    // Verify: Still only 1 modifier in the container (deduplication worked)
    ASSERT_EQ(it->second.size(), 1u);

    // Verify: The modifier's bounds value was updated
    auto& storedModifier = it->second.front();
    Vector4f storedBounds = storedModifier->Getter<Vector4f>(ModifierNG::RSPropertyType::BOUNDS, Vector4f());
    ASSERT_FLOAT_EQ(storedBounds.x_, 0.0f);
    ASSERT_FLOAT_EQ(storedBounds.y_, 0.0f);
    ASSERT_FLOAT_EQ(storedBounds.z_, 200.0f);  // Should be updated to 200
    ASSERT_FLOAT_EQ(storedBounds.w_, 200.0f);  // Should be updated to 200
}

/**
 * @tc.name: ModifierNoDeduplicationWithDisableTest
 * @tc.desc: Test BOUNDS modifier no deduplication when deduplication is disabled
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeTest, ModifierNoDeduplicationWithDisableTest, TestSize.Level1)
{
    RSRenderNode node(id, context);
    ModifierId modifierId = 100;

    // Create first BOUNDS modifier
    Vector4f bounds1(0.0f, 0.0f, 100.0f, 100.0f);
    auto property1 = std::make_shared<RSRenderProperty<Vector4f>>();
    property1->GetRef() = bounds1;
    auto modifier1 = ModifierNG::RSRenderModifier::MakeRenderModifier(
        ModifierNG::RSModifierType::BOUNDS, property1, modifierId, ModifierNG::RSPropertyType::BOUNDS);
    modifier1->SetDeduplicationEnabled(false);  // disable deduplication (default)

    node.AddModifier(modifier1);

    auto& modifiersNG = node.modifiersNG_;
    auto it = modifiersNG.find(ModifierNG::RSModifierType::BOUNDS);
    ASSERT_TRUE(it != modifiersNG.end());
    ASSERT_EQ(it->second.size(), 1u);

    // Create second BOUNDS modifier with same ID
    Vector4f bounds2(0.0f, 0.0f, 200.0f, 200.0f);
    auto property2 = std::make_shared<RSRenderProperty<Vector4f>>();
    property2->GetRef() = bounds2;
    auto modifier2 = ModifierNG::RSRenderModifier::MakeRenderModifier(
        ModifierNG::RSModifierType::BOUNDS, property2, modifierId, ModifierNG::RSPropertyType::BOUNDS);
    modifier2->SetDeduplicationEnabled(false);  // disable deduplication

    node.AddModifier(modifier2);

    // Verify: Now we have 2 modifiers in the container (no deduplication)
    ASSERT_EQ(it->second.size(), 2u);

    // Verify: First modifier still has original bounds
    Vector4f storedBounds1 = it->second[0]->Getter<Vector4f>(ModifierNG::RSPropertyType::BOUNDS, Vector4f());
    ASSERT_FLOAT_EQ(storedBounds1.z_, 100.0f);  // Should remain 100

    // Verify: Second modifier has new bounds
    Vector4f storedBounds2 = it->second[1]->Getter<Vector4f>(ModifierNG::RSPropertyType::BOUNDS, Vector4f());
    ASSERT_FLOAT_EQ(storedBounds2.z_, 200.0f);  // Should be 200
}

/**
 * @tc.name: ModifierDeduplicationWithDifferentIdTest
 * @tc.desc: Test BOUNDS modifier with different IDs should not be deduplicated
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeTest, ModifierDeduplicationWithDifferentIdTest, TestSize.Level1)
{
    RSRenderNode node(id, context);
    ModifierId modifierId1 = 100;
    ModifierId modifierId2 = 101;  // Different ID

    // Create first modifier
    Vector4f bounds1(0.0f, 0.0f, 100.0f, 100.0f);
    auto property1 = std::make_shared<RSRenderProperty<Vector4f>>();
    property1->GetRef() = bounds1;
    auto modifier1 = ModifierNG::RSRenderModifier::MakeRenderModifier(
        ModifierNG::RSModifierType::BOUNDS, property1, modifierId1, ModifierNG::RSPropertyType::BOUNDS);
    modifier1->SetDeduplicationEnabled(true);

    node.AddModifier(modifier1);

    // Create second modifier with different ID
    Vector4f bounds2(0.0f, 0.0f, 200.0f, 200.0f);
    auto property2 = std::make_shared<RSRenderProperty<Vector4f>>();
    property2->GetRef() = bounds2;
    auto modifier2 = ModifierNG::RSRenderModifier::MakeRenderModifier(
        ModifierNG::RSModifierType::BOUNDS, property2, modifierId2, ModifierNG::RSPropertyType::BOUNDS);
    modifier2->SetDeduplicationEnabled(true);

    node.AddModifier(modifier2);

    // Verify: Should have 2 modifiers (different IDs, no deduplication)
    auto& modifiersNG = node.modifiersNG_;
    auto it = modifiersNG.find(ModifierNG::RSModifierType::BOUNDS);
    ASSERT_TRUE(it != modifiersNG.end());
    ASSERT_EQ(it->second.size(), 2u);
}

/**
 * @tc.name: FrameModifierDeduplicationTest
 * @tc.desc: Test FRAME modifier deduplication
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeTest, FrameModifierDeduplicationTest, TestSize.Level1)
{
    RSRenderNode node(id, context);
    ModifierId modifierId = 200;

    // Create first FRAME modifier
    Vector4f frame1(10.0f, 10.0f, 100.0f, 100.0f);
    auto property1 = std::make_shared<RSRenderProperty<Vector4f>>();
    property1->GetRef() = frame1;
    auto modifier1 = ModifierNG::RSRenderModifier::MakeRenderModifier(
        ModifierNG::RSModifierType::FRAME, property1, modifierId, ModifierNG::RSPropertyType::FRAME);
    modifier1->SetDeduplicationEnabled(true);

    node.AddModifier(modifier1);

    // Create second FRAME modifier with same ID
    Vector4f frame2(20.0f, 20.0f, 200.0f, 200.0f);
    auto property2 = std::make_shared<RSRenderProperty<Vector4f>>();
    property2->GetRef() = frame2;
    auto modifier2 = ModifierNG::RSRenderModifier::MakeRenderModifier(
        ModifierNG::RSModifierType::FRAME, property2, modifierId, ModifierNG::RSPropertyType::FRAME);
    modifier2->SetDeduplicationEnabled(true);

    node.AddModifier(modifier2);

    // Verify: Only 1 modifier in container
    auto& modifiersNG = node.modifiersNG_;
    auto it = modifiersNG.find(ModifierNG::RSModifierType::FRAME);
    ASSERT_TRUE(it != modifiersNG.end());
    ASSERT_EQ(it->second.size(), 1u);

    // Verify: Frame value was updated
    auto& storedModifier = it->second.front();
    Vector4f storedFrame = storedModifier->Getter<Vector4f>(ModifierNG::RSPropertyType::FRAME, Vector4f());
    ASSERT_FLOAT_EQ(storedFrame.x_, 20.0f);  // Updated to 20
    ASSERT_FLOAT_EQ(storedFrame.y_, 20.0f);  // Updated to 20
    ASSERT_FLOAT_EQ(storedFrame.z_, 200.0f); // Updated to 200
    ASSERT_FLOAT_EQ(storedFrame.w_, 200.0f); // Updated to 200
}

/**
 * @tc.name: OtherModifierTypeNoDeduplicationTest
 * @tc.desc: Test that non-BOUNDS/FRAME modifiers are not affected by deduplication logic
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeTest, OtherModifierTypeNoDeduplicationTest, TestSize.Level1)
{
    RSRenderNode node(id, context);
    ModifierId modifierId = 300;

    // Create ALPHA modifiers (not BOUNDS or FRAME)
    float alpha1 = 0.5f;
    auto property1 = std::make_shared<RSRenderProperty<float>>();
    property1->GetRef() = alpha1;
    auto modifier1 = ModifierNG::RSRenderModifier::MakeRenderModifier(
        ModifierNG::RSModifierType::ALPHA, property1, modifierId, ModifierNG::RSPropertyType::ALPHA);
    modifier1->SetDeduplicationEnabled(true);  // Even with dedup enabled

    node.AddModifier(modifier1);

    float alpha2 = 0.8f;
    auto property2 = std::make_shared<RSRenderProperty<float>>();
    property2->GetRef() = alpha2;
    auto modifier2 = ModifierNG::RSRenderModifier::MakeRenderModifier(
        ModifierNG::RSModifierType::ALPHA, property2, modifierId, ModifierNG::RSPropertyType::ALPHA);
    modifier2->SetDeduplicationEnabled(true);

    node.AddModifier(modifier2);

    // Verify: Should have 2 ALPHA modifiers (ALPHA type is not affected by deduplication)
    auto& modifiersNG = node.modifiersNG_;
    auto it = modifiersNG.find(ModifierNG::RSModifierType::ALPHA);
    ASSERT_TRUE(it != modifiersNG.end());
    ASSERT_EQ(it->second.size(), 2u);
}

/**
 * @tc.name: DefaultBehaviorNoDeduplicationTest
 * @tc.desc: Test default behavior (deduplication is disabled (default)) does not deduplicate
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeTest, DefaultBehaviorNoDeduplicationTest, TestSize.Level1)
{
    RSRenderNode node(id, context);
    ModifierId modifierId = 400;

    // Create modifiers without setting enableDeduplication (default is false)
    Vector4f bounds1(0.0f, 0.0f, 100.0f, 100.0f);
    auto property1 = std::make_shared<RSRenderProperty<Vector4f>>();
    property1->GetRef() = bounds1;
    auto modifier1 = ModifierNG::RSRenderModifier::MakeRenderModifier(
        ModifierNG::RSModifierType::BOUNDS, property1, modifierId, ModifierNG::RSPropertyType::BOUNDS);
    // Not calling SetDeduplicationEnabled, using default value (false)

    node.AddModifier(modifier1);

    Vector4f bounds2(0.0f, 0.0f, 200.0f, 200.0f);
    auto property2 = std::make_shared<RSRenderProperty<Vector4f>>();
    property2->GetRef() = bounds2;
    auto modifier2 = ModifierNG::RSRenderModifier::MakeRenderModifier(
        ModifierNG::RSModifierType::BOUNDS, property2, modifierId, ModifierNG::RSPropertyType::BOUNDS);
    // Not calling SetDeduplicationEnabled, using default value (false)

    node.AddModifier(modifier2);

    // Verify: Should have 2 modifiers (default behavior is no deduplication)
    auto& modifiersNG = node.modifiersNG_;
    auto it = modifiersNG.find(ModifierNG::RSModifierType::BOUNDS);
    ASSERT_TRUE(it != modifiersNG.end());
    ASSERT_EQ(it->second.size(), 2u);
}

/**
 * @tc.name: DumpSubClassNodeTest
 * @tc.desc: DumpNodeType DumpTree and DumpSubClassNodeTest test
 * @tc.type: FUNC
 * @tc.require: issueI9US6V
 */
HWTEST_F(RSRenderNodeTest, DumpSubClassNodeTest005, TestSize.Level1)
{
    std::string outTest = "";
    auto renderNode = std::make_shared<RSSurfaceRenderNode>(0);
    renderNode->isCloneNode_ = true;
    renderNode->DumpSubClassNode(outTest);
    ASSERT_TRUE(outTest.find("isCloneNode") != string::npos);
    ASSERT_TRUE(outTest.find("isRelated") != string::npos);
    ASSERT_TRUE(outTest.find("sourceNodeId") != string::npos);
}

/**
 * @tc.name: GetColorPickerDrawable001
 * @tc.desc: Test GetColorPickerDrawable returns nullptr when drawable is null
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeTest, GetColorPickerDrawable001, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(1);
    ASSERT_NE(node, nullptr);

    // No ColorPicker drawable set
    auto colorPickerDrawable = node->GetColorPickerDrawable();
    EXPECT_EQ(colorPickerDrawable, nullptr);
}

/**
 * @tc.name: GetColorPickerDrawable002
 * @tc.desc: Test GetColorPickerDrawable successfully retrieves ColorPicker drawable
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeTest, GetColorPickerDrawable002, TestSize.Level1)
{
    RSRenderNode node(1);

    // Create a ColorPicker drawable
    auto colorPickerDrawable = std::make_shared<DrawableV2::RSColorPickerDrawable>(false, 0);
    ASSERT_NE(colorPickerDrawable, nullptr);

    // Set it at the COLOR_PICKER slot
    node.GetDrawableVec(__func__).at(static_cast<int8_t>(RSDrawableSlot::COLOR_PICKER)) = colorPickerDrawable;

    // Verify GetColorPickerDrawable returns the correct drawable
    auto retrievedDrawable = node.GetColorPickerDrawable();
    EXPECT_NE(retrievedDrawable, nullptr);
    EXPECT_EQ(retrievedDrawable, colorPickerDrawable);
}
} // namespace Rosen
} // namespace OHOS
