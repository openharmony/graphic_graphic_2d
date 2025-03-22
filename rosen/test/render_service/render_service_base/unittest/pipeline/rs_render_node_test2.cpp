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

#include <gtest/gtest.h>

#include "common/rs_obj_abs_geometry.h"
#include "drawable/rs_property_drawable_foreground.h"
#include "offscreen_render/rs_offscreen_render_thread.h"
#include "params/rs_render_params.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_dirty_region_manager.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_render_node.h"
#include "render_thread/rs_render_thread_visitor.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "skia_adapter/skia_canvas.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
const std::string OUT_STR3 =
    ", Parent [null], Name [SurfaceNode], hasConsumer: 0, Alpha: 1.000000, Visible: 1, VisibleRegion [Empty], "
    "OpaqueRegion [Empty], OcclusionBg: 0, SpecialLayer: 0, surfaceType: 0, "
    "ContainerConfig: [outR: 0 inR: 0 x: 0 y: 0 w: 0 h: 0]";
const std::string OUT_STR4 = ", Visible: 1, Size: [-inf, -inf], EnableRender: 1";
const std::string OUT_STR5 = ", skipLayer: 0";

class RSRenderNodeTest2 : public testing::Test {
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

void RSRenderNodeTest2::SetUpTestCase()
{
    canvas_ = new RSPaintFilterCanvas(&drawingCanvas_);
}
void RSRenderNodeTest2::TearDownTestCase()
{
    delete canvas_;
    canvas_ = nullptr;
}
void RSRenderNodeTest2::SetUp() {}
void RSRenderNodeTest2::TearDown() {}
/**
 * @tc.name: Process
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, Process, TestSize.Level1)
{
    RSRenderNode node(id, context);
    std::shared_ptr<RSNodeVisitor> visitor = nullptr;
    node.Process(visitor);
    visitor = std::make_shared<RSRenderThreadVisitor>();
    node.Process(visitor);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: SendCommandFromRT
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, SendCommandFromRT, TestSize.Level1)
{
    RSRenderNode node(id, context);
    std::unique_ptr<RSCommand> command;
    NodeId nodeId = 0;
    node.SendCommandFromRT(command, nodeId);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: InternalRemoveSelfFromDisappearingChildren
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, InternalRemoveSelfFromDisappearingChildren, TestSize.Level1)
{
    RSRenderNode node(id, context);
    node.InternalRemoveSelfFromDisappearingChildren();
    ASSERT_TRUE(true);
}

/**
 * @tc.name: DestroyRSRenderNode
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, DestroyRSRenderNode, TestSize.Level1)
{
    RSRenderNode node(id, context);
    node.appPid_ = 1;
    ASSERT_TRUE(true);
}

/**
 * @tc.name: FallbackAnimationsToRoot
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, FallbackAnimationsToRoot, TestSize.Level1)
{
    RSRenderNode node(id, context);
    node.FallbackAnimationsToRoot();
    node.FallbackAnimationsToRoot();
    node.animationManager_.animations_.clear();
    node.FallbackAnimationsToRoot();
    ASSERT_TRUE(true);
}

/**
 * @tc.name: ActivateDisplaySync
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, ActivateDisplaySync, TestSize.Level1)
{
    RSRenderNode node(id, context);
    node.ActivateDisplaySync();
    node.displaySync_ = std::make_shared<RSRenderDisplaySync>(id);
    node.ActivateDisplaySync();
    ASSERT_TRUE(true);
}

/**
 * @tc.name: UpdateDisplaySyncRange
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, UpdateDisplaySyncRange, TestSize.Level1)
{
    RSRenderNode node(id, context);
    node.UpdateDisplaySyncRange();
    node.displaySync_ = std::make_shared<RSRenderDisplaySync>(1);
    node.UpdateDisplaySyncRange();
    ASSERT_TRUE(true);
}

/**
 * @tc.name: Animate
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, Animate, TestSize.Level1)
{
    RSRenderNode node(id, context);
    int64_t timestamp = 4;
    int64_t period = 2;
    bool isDisplaySyncEnabled = true;
    node.Animate(timestamp, period, isDisplaySyncEnabled);
    node.displaySync_ = std::make_shared<RSRenderDisplaySync>(1);
    node.Animate(timestamp, period, isDisplaySyncEnabled);
    auto context_shared = std::make_shared<RSContext>();
    std::weak_ptr<RSContext> context2 = context_shared;
    RSRenderNode node2(id, context2);
    node2.Animate(timestamp, period, isDisplaySyncEnabled);
    RSSurfaceRenderNode node3(id, context2);
    node3.Animate(timestamp, period, isDisplaySyncEnabled);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: IsClipBound
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, IsClipBound, TestSize.Level1)
{
    RSRenderNode node(id, context);
    node.IsClipBound();
    ASSERT_TRUE(true);
}

/**
 * @tc.name: GetRenderContent
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, GetRenderContent, TestSize.Level1)
{
    RSRenderNode node(id, context);
    ASSERT_NE(node.GetRenderContent(), nullptr);
}

/**
 * @tc.name: GetStagingRenderParams
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, GetStagingRenderParams, TestSize.Level1)
{
    RSRenderNode node(id, context);
    ASSERT_EQ(node.GetStagingRenderParams(), nullptr);
}

/**
 * @tc.name: CollectAndUpdateLocalShadowRect
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, CollectAndUpdateLocalShadowRect, TestSize.Level1)
{
    RSRenderNode node(id, context);
    node.CollectAndUpdateLocalShadowRect();
    RSDrawableSlot slot = RSDrawableSlot::SHADOW;
    node.dirtySlots_.insert(slot);
    node.CollectAndUpdateLocalShadowRect();
    ASSERT_TRUE(true);
}

/**
 * @tc.name: CollectAndUpdateLocalOutlineRect
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, CollectAndUpdateLocalOutlineRect, TestSize.Level1)
{
    RSRenderNode node(id, context);
    node.CollectAndUpdateLocalOutlineRect();
    RSDrawableSlot slot = RSDrawableSlot::OUTLINE;
    node.dirtySlots_.insert(slot);
    node.CollectAndUpdateLocalOutlineRect();
    ASSERT_TRUE(true);
}

/**
 * @tc.name: CollectAndUpdateLocalPixelStretchRect
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, CollectAndUpdateLocalPixelStretchRect, TestSize.Level1)
{
    RSRenderNode node(id, context);
    node.CollectAndUpdateLocalPixelStretchRect();
    RSDrawableSlot slot = RSDrawableSlot::PIXEL_STRETCH;
    node.dirtySlots_.insert(slot);
    node.CollectAndUpdateLocalPixelStretchRect();
    ASSERT_TRUE(true);
}

/**
 * @tc.name: UpdateBufferDirtyRegion
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, UpdateBufferDirtyRegion, TestSize.Level1)
{
    RSRenderNode node(id, context);
    node.UpdateBufferDirtyRegion();
    RectI dirtyRect{0, 0, 1000, 1000};
    RectI drawRegion{0, 0, 1000, 1000};
    node.UpdateBufferDirtyRegion(dirtyRect, drawRegion);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: UpdateSelfDrawRect
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, UpdateSelfDrawRect, TestSize.Level1)
{
    RSRenderNode node(id, context);
    node.UpdateSelfDrawRect();
    ASSERT_TRUE(true);
}

/**
 * @tc.name: GetSelfDrawRect
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, GetDrawRect, TestSize.Level1)
{
    RSRenderNode node(id, context);
    ASSERT_EQ(node.GetSelfDrawRect(), node.selfDrawRect_);
    ASSERT_EQ(node.GetAbsDrawRect(), node.absDrawRect_);
}

/**
 * @tc.name: GetHDRBrightness
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, GetHDRBrightness, TestSize.Level1)
{
    RSRenderNode node(id, context);
    node.GetHDRBrightness();
    ASSERT_TRUE(true);
}

/**
 * @tc.name: CheckAndUpdateGeoTrans001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, CheckAndUpdateGeoTrans001, TestSize.Level1)
{
    RSRenderNode node(id, context);
    std::shared_ptr<RSObjAbsGeometry> geoPtr = std::make_shared<RSObjAbsGeometry>();
    ASSERT_EQ(node.CheckAndUpdateGeoTrans(geoPtr), false);
}

/**
 * @tc.name: CheckAndUpdateGeoTrans002
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, CheckAndUpdateGeoTrans002, TestSize.Level1)
{
    RSRenderNode node(id, context);
    std::shared_ptr<RSObjAbsGeometry> geoPtr = std::make_shared<RSObjAbsGeometry>();
    Drawing::Matrix matrix;
    PropertyId id = 1;
    std::shared_ptr<RSRenderProperty<Drawing::Matrix>> property =
        std::make_shared<RSRenderProperty<Drawing::Matrix>>(matrix, id);
    std::list<std::shared_ptr<RSRenderModifier>> list { std::make_shared<RSGeometryTransRenderModifier>(property) };
    std::map<RSModifierType, std::list<std::shared_ptr<RSRenderModifier>>> map;
    map[RSModifierType::GEOMETRYTRANS] = list;
    node.renderContent_->drawCmdModifiers_ = map;
    ASSERT_EQ(node.CheckAndUpdateGeoTrans(geoPtr), true);
}

/**
 * @tc.name: UpdateAbsDirtyRegion001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, UpdateAbsDirtyRegion001, TestSize.Level1)
{
    RSRenderNode node(id, context);
    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    RectI clipRect{0, 0, 1000, 1000};
    node.UpdateAbsDirtyRegion(*rsDirtyManager, clipRect);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: UpdateAbsDirtyRegion002
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, UpdateAbsDirtyRegion002, TestSize.Level1)
{
    RSRenderNode node(id, context);
    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    RectI clipRect{0, 0, 1000, 1000};
    node.isSelfDrawingNode_ = true;
    node.absDrawRect_ = {1, 2, 3, 4};
    node.oldAbsDrawRect_ = {2, 2, 3, 4};
    node.UpdateAbsDirtyRegion(*rsDirtyManager, clipRect);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: UpdateAbsDirtyRegion003
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, UpdateAbsDirtyRegion003, TestSize.Level1)
{
    RSRenderNode node(id, context);
    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    RectI clipRect{0, 0, 1000, 1000};
    node.isSelfDrawingNode_ = true;
    node.absDrawRect_ = {1, 2, 3, 4};
    node.oldAbsDrawRect_ = {2, 2, 3, 4};
    node.shouldPaint_ = false;
    node.isLastVisible_ = true;
    node.UpdateAbsDirtyRegion(*rsDirtyManager, clipRect);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: UpdateAbsDirtyRegion004
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, UpdateAbsDirtyRegion004, TestSize.Level1)
{
    RSRenderNode node(id, context);
    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    RectI clipRect{0, 0, 1000, 1000};
    node.isSelfDrawingNode_ = true;
    node.absDrawRect_ = {1, 2, 3, 4};
    node.oldAbsDrawRect_ = {2, 2, 3, 4};
    node.shouldPaint_ = true;
    node.isLastVisible_ = true;
    node.UpdateAbsDirtyRegion(*rsDirtyManager, clipRect);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: UpdateDrawRectAndDirtyRegion001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, UpdateDrawRectAndDirtyRegion001, TestSize.Level1)
{
    RSRenderNode node(id, context);
    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    RectI clipRect{0, 0, 1000, 1000};
    Drawing::Matrix matrix;
    node.SetDirty();
    ASSERT_EQ(node.UpdateDrawRectAndDirtyRegion(*rsDirtyManager, false, clipRect, matrix), true);
}

/**
 * @tc.name: UpdateDrawRectAndDirtyRegion002
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, UpdateDrawRectAndDirtyRegion002, TestSize.Level1)
{
    RSRenderNode node(id, context);
    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    RectI clipRect{0, 0, 1000, 1000};
    Drawing::Matrix matrix;
    node.SetDirty();
    auto& properties = node.GetMutableRenderProperties();
    properties.clipToBounds_ = true;
    properties.clipToFrame_ = true;
    properties.geoDirty_ = true;
    node.dirtyStatus_ = RSRenderNode::NodeDirty::DIRTY;
    node.isSelfDrawingNode_ = true;
    node.srcOrClipedAbsDrawRectChangeFlag_ = true;
    node.shouldPaint_ = true;
    node.isLastVisible_ = true;
    ASSERT_EQ(node.UpdateDrawRectAndDirtyRegion(*rsDirtyManager, false, clipRect, matrix), true);
}

/**
 * @tc.name: UpdateDrawRect001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, UpdateDrawRect001, TestSize.Level1)
{
    RSRenderNode node(id, context);
    RectI clipRect{0, 0, 1000, 1000};
    Drawing::Matrix matrix;
    RSRenderNode parentNode(id + 1, context);
    bool accumGeoDirty = true;
    node.UpdateDrawRect(accumGeoDirty, clipRect, matrix);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: UpdateDrawRect002
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, UpdateDrawRect002, TestSize.Level1)
{
    RSRenderNode node(id, context);
    RectI clipRect{0, 0, 1000, 1000};
    Drawing::Matrix matrix;
    auto& properties = node.GetMutableRenderProperties();
    properties.sandbox_ = std::make_unique<Sandbox>();
    bool accumGeoDirty = true;
    node.UpdateDrawRect(accumGeoDirty, clipRect, matrix);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: UpdateDirtyRegionInfoForDFX001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, UpdateDirtyRegionInfoForDFX001, TestSize.Level1)
{
    RSRenderNode node(id, context);
    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    auto& properties = node.GetMutableRenderProperties();
    properties.drawRegion_ = std::make_shared<RectF>();
    properties.boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    node.UpdateDirtyRegionInfoForDFX(*rsDirtyManager);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: UpdateDirtyRegionInfoForDFX002
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, UpdateDirtyRegionInfoForDFX002, TestSize.Level1)
{
    RSRenderNode node(id, context);
    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    node.UpdateDirtyRegionInfoForDFX(*rsDirtyManager);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: Update001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, Update001, TestSize.Level1)
{
    RSRenderNode node(id, context);
    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    std::shared_ptr<RSRenderNode> parentNode;
    RectI clipRect{0, 0, 1000, 1000};
    node.shouldPaint_ = false;
    node.isLastVisible_ = false;
    bool parentDirty = true;
    ASSERT_EQ(node.Update(*rsDirtyManager, parentNode, parentDirty, clipRect), false);
}

/**
 * @tc.name: Update002
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, Update002, TestSize.Level1)
{
    RSRenderNode node(id, context);
    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    std::shared_ptr<RSRenderNode> parentNode;
    RectI clipRect{0, 0, 1000, 1000};
    node.shouldPaint_ = true;
    node.isLastVisible_ = false;
    node.SetDirty();
    Drawing::Matrix matrix;
    PropertyId id = 1;
    std::shared_ptr<RSRenderProperty<Drawing::Matrix>> property =
        std::make_shared<RSRenderProperty<Drawing::Matrix>>(matrix, id);
    std::list<std::shared_ptr<RSRenderModifier>> list { std::make_shared<RSGeometryTransRenderModifier>(property) };
    std::map<RSModifierType, std::list<std::shared_ptr<RSRenderModifier>>> map;
    map[RSModifierType::GEOMETRYTRANS] = list;
    node.renderContent_->drawCmdModifiers_ = map;
    bool parentDirty = true;
    ASSERT_EQ(node.Update(*rsDirtyManager, parentNode, parentDirty, clipRect), true);
}

/**
 * @tc.name: GetMutableRenderProperties
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, GetMutableRenderProperties, TestSize.Level1)
{
    RSRenderNode node(id, context);
    node.GetMutableRenderProperties();
    ASSERT_TRUE(true);
}

/**
 * @tc.name: GetMutableRenderProperties001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, UpdateBufferDirtyRegion001, TestSize.Level1)
{
    RSRenderNode node(id, context);
    RectI dirtyRect{0, 0, 1000, 1000};
    RectI drawRegion{0, 0, 1000, 1000};
    ASSERT_EQ(node.UpdateBufferDirtyRegion(dirtyRect, drawRegion), false);
}

/**
 * @tc.name: IsSelfDrawingNode
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, IsSelfDrawingNodeAndDirty, TestSize.Level1)
{
    RSRenderNode node(id, context);
    node.IsSelfDrawingNode();
    node.IsDirty();
    ASSERT_TRUE(true);
}

/**
 * @tc.name: SubTreeDirty
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, SubTreeDirty, TestSize.Level1)
{
    RSRenderNode node(id, context);
    bool val = true;
    node.SetSubTreeDirty(val);
    ASSERT_TRUE(node.isSubTreeDirty_);
    node.IsSubTreeDirty();
    ASSERT_TRUE(true);
}

/**
 * @tc.name: SetParentSubTreeDirty
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, SetParentSubTreeDirty, TestSize.Level1)
{
    RSRenderNode node(id, context);
    node.SetParentSubTreeDirty();
    ASSERT_TRUE(true);
}

/**
 * @tc.name: IsContentDirty
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, IsContentDirty, TestSize.Level1)
{
    RSRenderNode node(id, context);
    node.IsContentDirty();
    ASSERT_TRUE(true);
}

/**
 * @tc.name: MapAndUpdateChildrenRect001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, MapAndUpdateChildrenRect001, TestSize.Level1)
{
    RSRenderNode node(id, context);
    node.shouldPaint_ = true;
    node.MapAndUpdateChildrenRect();
    ASSERT_TRUE(true);
}

/**
 * @tc.name: MapAndUpdateChildrenRect002
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, MapAndUpdateChildrenRect002, TestSize.Level1)
{
    RSRenderNode node(id, context);
    node.shouldPaint_ = false;
    auto& properties = node.GetMutableRenderProperties();
    properties.boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    node.selfDrawRect_ = {0, 0, 1000, 1000};
    node.MapAndUpdateChildrenRect();
    ASSERT_TRUE(true);
}

/**
 * @tc.name: MapAndUpdateChildrenRect003
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, MapAndUpdateChildrenRect003, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    std::shared_ptr<RSSurfaceRenderNode> surfaceNode = std::make_shared<RSSurfaceRenderNode>(1);

    surfaceNode->shouldPaint_ = true;
    auto& properties = nodeTest->GetMutableRenderProperties();
    properties.boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    properties.SetClipToFrame(true);
    surfaceNode->selfDrawRect_ = {0, 0, 1000, 1100};
    surfaceNode->childrenRect_ = {0, 0, 1000, 1000};
    nodeTest->selfDrawRect_ = {0, 0, 1000, 800};
    surfaceNode->parent_ = nodeTest;
    surfaceNode->MapAndUpdateChildrenRect();
    ASSERT_TRUE(true);
}

/**
 * @tc.name: MapAndUpdateChildrenRect004
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, MapAndUpdateChildrenRect004, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    std::shared_ptr<RSSurfaceRenderNode> surfaceNode = std::make_shared<RSSurfaceRenderNode>(1);

    surfaceNode->shouldPaint_ = true;
    auto& properties = nodeTest->GetMutableRenderProperties();
    properties.boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    properties.SetClipToFrame(false);
    surfaceNode->selfDrawRect_ = {0, 0, 1000, 1100};
    surfaceNode->childrenRect_ = {0, 0, 1000, 1000};
    nodeTest->selfDrawRect_ = {0, 0, 1000, 800};
    surfaceNode->parent_ = nodeTest;
    surfaceNode->MapAndUpdateChildrenRect();
    ASSERT_TRUE(true);
}
/**
 * @tc.name: MapAndUpdateChildrenRect005
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueIAM93Q
 */
HWTEST_F(RSRenderNodeTest2, MapAndUpdateChildrenRect005, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> parentNode = std::make_shared<RSBaseRenderNode>(id, context);
    std::shared_ptr<RSRenderNode> inNode = std::make_shared<RSBaseRenderNode>(id + 1, context);
    std::shared_ptr<RSRenderNode> outNode = std::make_shared<RSBaseRenderNode>(id + 2, context);
    auto sharedTransitionParam = std::make_shared<SharedTransitionParam>(inNode, outNode, true);
    parentNode->GetRenderProperties().GetBoundsGeometry()->absMatrix_ = Drawing::Matrix();
    inNode->parent_ = parentNode;
    inNode->shouldPaint_ = true;
    inNode->GetMutableRenderProperties().sandbox_ = std::make_unique<Sandbox>();
    Vector2f vec2f(1.0f, 1.0f);
    inNode->GetMutableRenderProperties().sandbox_->position_ = vec2f;
    inNode->SetSharedTransitionParam(sharedTransitionParam);
    inNode->selfDrawRect_ = {0, 0, 1000, 1000};
    inNode->MapAndUpdateChildrenRect();
    ASSERT_TRUE(true);
}

/**
 * @tc.name: IsFilterCacheValid001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, IsFilterCacheValid001, TestSize.Level1)
{
    RSRenderNode node(id, context);
    node.shouldPaint_ = true;
    node.IsFilterCacheValid();
    ASSERT_TRUE(true);
}

/**
 * @tc.name: IsFilterCacheValid001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, IsFilterCacheValid002, TestSize.Level1)
{
    RSRenderNode node(id, context);
    node.shouldPaint_ = true;
    auto& properties = node.GetMutableRenderProperties();
    properties.filter_ = std::make_shared<RSFilter>();
    node.IsFilterCacheValid();
    ASSERT_TRUE(true);
}

/**
 * @tc.name: IsAIBarFilterCacheValid
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, IsAIBarFilterCacheValid, TestSize.Level1)
{
    RSRenderNode node(id, context);
    ASSERT_EQ(node.IsAIBarFilterCacheValid(), false);
}

/**
 * @tc.name: GetFilterCachedRegionAndHasBlurFilter
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, GetFilterCachedRegionAndHasBlurFilter, TestSize.Level1)
{
    RSRenderNode node(id, context);
    node.GetFilterCachedRegion();
    node.HasBlurFilter();
    ASSERT_TRUE(true);
}

/**
 * @tc.name: UpdateLastFilterCacheRegion
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, UpdateLastFilterCacheRegion, TestSize.Level1)
{
    RSRenderNode node(id, context);
    node.UpdateLastFilterCacheRegion();
    ASSERT_TRUE(true);
}

/**
 * @tc.name: GetAbsMatrixReverse001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, GetAbsMatrixReverse001, TestSize.Level1)
{
    RSRenderNode node(id, context);
    auto& properties = node.GetMutableRenderProperties();
    properties.boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    RSBaseRenderNode rootNode(id + 1, context);
    auto& properties1 = node.GetMutableRenderProperties();
    properties1.boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    Drawing::Matrix absMatrix;
    node.GetAbsMatrixReverse(rootNode, absMatrix);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: GetAbsMatrixReverse002
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, GetAbsMatrixReverse002, TestSize.Level1)
{
    RSRenderNode node(id, context);
    auto& properties = node.GetMutableRenderProperties();
    properties.boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    RSBaseRenderNode rootNode(id + 1, context);
    auto& properties1 = rootNode.GetMutableRenderProperties();
    properties1.boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    Drawing::Matrix absMatrix;
    node.GetAbsMatrixReverse(rootNode, absMatrix);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: UpdateFilterRegionInSkippedSubTree001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, UpdateFilterRegionInSkippedSubTree001, TestSize.Level1)
{
    RSRenderNode node(id, context);
    auto& properties = node.GetMutableRenderProperties();
    properties.boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    RSBaseRenderNode subTreeRoot(id + 1, context);
    auto& properties1 = subTreeRoot.GetMutableRenderProperties();
    properties1.boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    RectI filterRect{0, 0, 1000, 1000};
    RectI clipRect{0, 0, 1000, 1000};
    node.UpdateFilterRegionInSkippedSubTree(*rsDirtyManager, subTreeRoot, filterRect, clipRect);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: UpdateFilterRegionInSkippedSubTree002
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, UpdateFilterRegionInSkippedSubTree002, TestSize.Level1)
{
    RSRenderNode node(id, context);
    auto& properties = node.GetMutableRenderProperties();
    properties.boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    RSBaseRenderNode subTreeRoot(id + 1, context);
    auto& properties1 = subTreeRoot.GetMutableRenderProperties();
    properties1.boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    RectI filterRect{0, 0, 1000, 1000};
    node.lastFilterRegion_ = filterRect;
    RectI clipRect{0, 0, 1000, 1000};
    node.UpdateFilterRegionInSkippedSubTree(*rsDirtyManager, subTreeRoot, filterRect, clipRect);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: CheckBlurFilterCacheNeedForceClearOrSave
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, CheckBlurFilterCacheNeedForceClearOrSave, TestSize.Level1)
{
    RSRenderNode node(id, context);
    bool rotationChanged = true;
    bool rotationStatusChanged = true;
    auto& properties = node.GetMutableRenderProperties();
    properties.backgroundFilter_ = std::make_shared<RSFilter>();
    properties.filter_ = std::make_shared<RSFilter>();
    node.CheckBlurFilterCacheNeedForceClearOrSave(rotationChanged, rotationStatusChanged);
    rotationStatusChanged = false;
    node.CheckBlurFilterCacheNeedForceClearOrSave(rotationChanged, rotationStatusChanged);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: IsForceClearOrUseFilterCache
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, IsForceClearOrUseFilterCache, TestSize.Level1)
{
    RSRenderNode node(id, context);
    std::shared_ptr<DrawableV2::RSFilterDrawable> filterDrawable = std::make_shared<DrawableV2::RSFilterDrawable>();
    ASSERT_EQ(node.IsForceClearOrUseFilterCache(filterDrawable), false);
}

/**
 * @tc.name: MarkFilterStatusChanged
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, MarkFilterStatusChanged, TestSize.Level1)
{
    RSRenderNode node(id, context);
    bool isForeground = true;
    bool isFilterRegionChanged = true;
    node.MarkFilterStatusChanged(isForeground, isFilterRegionChanged);
    isFilterRegionChanged = false;
    node.MarkFilterStatusChanged(isForeground, isFilterRegionChanged);
    isForeground = false;
    node.MarkFilterStatusChanged(isForeground, isFilterRegionChanged);
    isForeground = true;
    node.MarkFilterStatusChanged(isForeground, isFilterRegionChanged);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: GetFilterDrawable
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, GetFilterDrawable, TestSize.Level1)
{
    RSRenderNode node(id, context);
    bool isForeground = true;
    node.GetFilterDrawable(isForeground);
    isForeground = false;
    node.GetFilterDrawable(isForeground);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: UpdateFilterCacheWithBelowDirty
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, UpdateFilterCacheWithBelowDirty, TestSize.Level1)
{
    RSRenderNode node(id, context);
    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    bool isForeground = true;
    node.UpdateFilterCacheWithBelowDirty(*rsDirtyManager, isForeground);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: UpdateFilterCacheWithSelfDirty
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, UpdateFilterCacheWithSelfDirty, TestSize.Level1)
{
    RSRenderNode node(id, context);
    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    auto& properties = node.GetMutableRenderProperties();
    properties.backgroundFilter_ = std::make_shared<RSFilter>();
    properties.filter_ = std::make_shared<RSFilter>();
    node.UpdateFilterCacheWithSelfDirty();
    ASSERT_TRUE(true);
}

/**
 * @tc.name: UpdateFilterCacheWithSelfDirty002
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueIB0UQV
 */
HWTEST_F(RSRenderNodeTest2, UpdateFilterCacheWithSelfDirty002, TestSize.Level1)
{
    ASSERT_TRUE(RSProperties::filterCacheEnabled_);
    RSRenderNode node(id, context);
    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    auto& properties = node.GetMutableRenderProperties();
    properties.needDrawBehindWindow_ = true;
    RectI inRegion(10, 10, 20, 20);
    RectI outRegion(90, 90, 110, 110);
    RectI lastRegion(0, 0, 100, 100);
    node.filterRegion_ = inRegion;
    node.lastFilterRegion_ = lastRegion;
    node.UpdateFilterCacheWithSelfDirty();
    node.filterRegion_ = outRegion;
    node.UpdateFilterCacheWithSelfDirty();
    ASSERT_TRUE(true);
}

/**
 * @tc.name: IsBackgroundInAppOrNodeSelfDirty
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, IsBackgroundInAppOrNodeSelfDirty, TestSize.Level1)
{
    RSRenderNode node(id, context);
    node.IsBackgroundInAppOrNodeSelfDirty();
    ASSERT_TRUE(true);
}

/**
 * @tc.name: UpdateDirtySlotsAndPendingNodes
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, UpdateDirtySlotsAndPendingNodes, TestSize.Level1)
{
    RSRenderNode node(id, context);
    RSDrawableSlot slot = RSDrawableSlot::SHADOW;
    node.UpdateDirtySlotsAndPendingNodes(slot);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: PostPrepareForBlurFilterNode
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, PostPrepareForBlurFilterNode, TestSize.Level1)
{
    RSRenderNode node(id, context);
    bool needRequestNextVsync = true;
    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    auto& properties = node.GetMutableRenderProperties();
    properties.backgroundFilter_ = std::make_shared<RSFilter>();
    properties.filter_ = std::make_shared<RSFilter>();
    node.PostPrepareForBlurFilterNode(*rsDirtyManager, needRequestNextVsync);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: PostPrepareForBlurFilterNode002
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueIB0UQV
 */
HWTEST_F(RSRenderNodeTest2, PostPrepareForBlurFilterNode002, TestSize.Level1)
{
    ASSERT_TRUE(RSProperties::filterCacheEnabled_);
    RSRenderNode node(id, context);
    bool needRequestNextVsync = true;
    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    auto& properties = node.GetMutableRenderProperties();
    properties.needDrawBehindWindow_ = true;
    node.PostPrepareForBlurFilterNode(*rsDirtyManager, needRequestNextVsync);
    RSDrawableSlot slot = RSDrawableSlot::BACKGROUND_FILTER;
    node.drawableVec_[static_cast<uint32_t>(slot)] = std::make_shared<DrawableV2::RSFilterDrawable>();
    node.PostPrepareForBlurFilterNode(*rsDirtyManager, needRequestNextVsync);
    ASSERT_NE(node.GetFilterDrawable(false), nullptr);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: DumpSubClassNodeTest032
 * @tc.desc: DumpSubClassNode and DumpDrawCmdModifiers test
 * @tc.type: FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSRenderNodeTest2, DumpSubClassNodeTest032, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> nodeTest1 = std::make_shared<RSSurfaceRenderNode>(0);
    EXPECT_NE(nodeTest1, nullptr);
    std::string outTest3 = "";
    nodeTest1->DumpSubClassNode(outTest3);
    EXPECT_EQ(outTest3, OUT_STR3);

    std::shared_ptr<RSRootRenderNode> nodeTest2 = std::make_shared<RSRootRenderNode>(0);
    EXPECT_NE(nodeTest2, nullptr);
    std::string outTest4 = "";
    nodeTest2->DumpSubClassNode(outTest4);
    EXPECT_EQ(outTest4, OUT_STR4);

    RSDisplayNodeConfig config;
    std::shared_ptr<RSDisplayRenderNode> nodeTest3 = std::make_shared<RSDisplayRenderNode>(0, config);
    EXPECT_NE(nodeTest3, nullptr);
    std::string outTest5 = "";
    nodeTest3->DumpSubClassNode(outTest5);
    EXPECT_NE(outTest5, OUT_STR5);

    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);
    std::string outTest6 = "";
    nodeTest->DumpDrawCmdModifiers(outTest6);
    EXPECT_EQ(outTest6, "");
    std::shared_ptr<RSRenderProperty<Drawing::DrawCmdListPtr>> propertyTest =
        std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>();
    EXPECT_NE(propertyTest, nullptr);
    std::shared_ptr<RSDrawCmdListRenderModifier> drawCmdModifiersTest =
        std::make_shared<RSDrawCmdListRenderModifier>(propertyTest);
    EXPECT_NE(drawCmdModifiersTest, nullptr);
    nodeTest->renderContent_->drawCmdModifiers_[RSModifierType::CHILDREN].emplace_back(drawCmdModifiersTest);
    nodeTest->DumpDrawCmdModifiers(outTest6);
    EXPECT_NE(outTest6, "");
}

/**
 * @tc.name: ForceMergeSubTreeDirtyRegionTest033
 * @tc.desc: ForceMergeSubTreeDirtyRegion SubTreeSkipPrepare test
 * @tc.type: FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSRenderNodeTest2, ForceMergeSubTreeDirtyRegionTest033, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> nodeTest = std::make_shared<RSSurfaceRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);

    RSDirtyRegionManager dirtyManagerTest1;
    RectI clipRectTest1 = RectI { 0, 0, 1, 1 };
    nodeTest->lastFrameSubTreeSkipped_ = true;
    nodeTest->geoUpdateDelay_ = true;
    nodeTest->ForceMergeSubTreeDirtyRegion(dirtyManagerTest1, clipRectTest1);
    EXPECT_FALSE(nodeTest->lastFrameSubTreeSkipped_);

    RSDirtyRegionManager dirtyManagerTest2;
    RectI clipRectTest2 = RectI { 0, 0, 1, 1 };
    nodeTest->lastFrameHasChildrenOutOfRect_ = false;
    nodeTest->renderContent_->renderProperties_.boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    EXPECT_NE(nodeTest->renderContent_->renderProperties_.boundsGeo_, nullptr);
    nodeTest->hasChildrenOutOfRect_ = true;
    nodeTest->SubTreeSkipPrepare(dirtyManagerTest2, true, true, clipRectTest2);

    RSDirtyRegionManager dirtyManagerTest3;
    RectI clipRectTest3 = RectI { 0, 0, 1, 1 };
    nodeTest->srcOrClipedAbsDrawRectChangeFlag_ = true;
    nodeTest->hasChildrenOutOfRect_ = false;
    nodeTest->lastFrameHasChildrenOutOfRect_ = true;
    nodeTest->renderContent_->renderProperties_.boundsGeo_ = nullptr;
    nodeTest->SubTreeSkipPrepare(dirtyManagerTest3, false, true, clipRectTest3);
}

/**
 * @tc.name: IsSubTreeNeedPrepareTest034
 * @tc.desc: Prepare QuickPrepare IsSubTreeNeedPrepare IsUifirstArkTsCardNode test
 * @tc.type: FUNC
 * @tc.require: issueIA5Y41
 */
HWTEST_F(RSRenderNodeTest2, IsSubTreeNeedPrepareTest034, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> nodeTest = std::make_shared<RSSurfaceRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);

    std::shared_ptr<RSNodeVisitor> visitor = nullptr;
    nodeTest->Prepare(visitor);
    nodeTest->QuickPrepare(visitor);

    nodeTest->shouldPaint_ = false;
    EXPECT_FALSE(nodeTest->IsSubTreeNeedPrepare(false, false));
    nodeTest->shouldPaint_ = true;
    EXPECT_FALSE(nodeTest->IsSubTreeNeedPrepare(false, true));
    nodeTest->isSubTreeDirty_ = true;
    EXPECT_TRUE(nodeTest->IsSubTreeNeedPrepare(false, false));
    nodeTest->isSubTreeDirty_ = false;
    nodeTest->childHasSharedTransition_ = true;
    EXPECT_TRUE(nodeTest->IsSubTreeNeedPrepare(false, false));
    nodeTest->childHasSharedTransition_ = false;
    nodeTest->childHasVisibleFilter_ = true;
    EXPECT_FALSE(nodeTest->IsSubTreeNeedPrepare(false, false));

    nodeTest->nodeGroupType_ = RSRenderNode::NONE;
    EXPECT_FALSE(nodeTest->IsUifirstArkTsCardNode());
    nodeTest->nodeGroupType_ = RSRenderNode::GROUPED_BY_ANIM;
    EXPECT_FALSE(nodeTest->IsUifirstArkTsCardNode());
}

/**
 * @tc.name: SetDrawRegionTest
 * @tc.desc: SetDrawRegionTest
 * @tc.type: FUNC
 * @tc.require: issueIA5Y41
 */
HWTEST_F(RSRenderNodeTest2, SetDrawRegionTest, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    std::shared_ptr<RectF> rect = std::make_shared<RectF>();
    rect->SetAll(0, 0, (std::numeric_limits<uint16_t>::max() + 1),
        (std::numeric_limits<uint16_t>::max() + 1));
    nodeTest->SetDrawRegion(rect);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: CollectAndUpdateLocalDistortionEffectRecttest
 * @tc.desc: CollectAndUpdateLocalDistortionEffectRect
 * @tc.type: FUNC
 * @tc.require: issueIAS8IM
 */
HWTEST_F(RSRenderNodeTest2, CollectAndUpdateLocalDistortionEffectRecttest, TestSize.Level1)
{
    RSRenderNode node(id, context);
    float width = 100.0f; // 100: set width of bounds
    float height = 100.0f; // 100: set height of bounds
    Vector4f bounds(0.0, 0.0, width, height);
    node.renderContent_->renderProperties_.SetBounds(bounds);
    node.CollectAndUpdateLocalDistortionEffectRect();
    EXPECT_FALSE(node.localDistortionEffectRect_.width_ > static_cast<int>(width));

    node.renderContent_->renderProperties_.SetDistortionK(0.5f); // 0.5 is k of value in distortion
    EXPECT_TRUE(node.renderContent_->renderProperties_.GetDistortionDirty());
    node.CollectAndUpdateLocalDistortionEffectRect();
    EXPECT_FALSE(node.renderContent_->renderProperties_.GetDistortionDirty());
}

/**
 * @tc.name: ChildrenBlurBehindWindowTest
 * @tc.desc: ChildrenBlurBehindWindowTest
 * @tc.type: FUNC
 * @tc.require: issueIB0UQV
 */
HWTEST_F(RSRenderNodeTest2, ChildrenBlurBehindWindowTest, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSRenderNode>(0, rsContext);
    NodeId idOne = 1;
    NodeId idTwo = 2;
    node->AddChildBlurBehindWindow(idOne);
    ASSERT_FALSE(node->NeedDrawBehindWindow());
    node->RemoveChildBlurBehindWindow(idTwo);
    ASSERT_FALSE(node->NeedDrawBehindWindow());
    node->RemoveChildBlurBehindWindow(idOne);
    ASSERT_FALSE(node->NeedDrawBehindWindow());
}

/**
 * @tc.name: ProcessBehindWindowOnTreeStateChangedTest
 * @tc.desc: ProcessBehindWindowOnTreeStateChangedTest
 * @tc.type: FUNC
 * @tc.require: issueIB0UQV
 */
HWTEST_F(RSRenderNodeTest2, ProcessBehindWindowOnTreeStateChangedTest, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSRenderNode>(0, rsContext);
    node->ProcessBehindWindowOnTreeStateChanged();
    auto rootNode = std::make_shared<RSRenderNode>(1);
    rsContext->nodeMap.renderNodeMap_[ExtractPid(1)][1] = rootNode;
    node->renderContent_->renderProperties_.SetUseEffect(true);
    ASSERT_TRUE(node->renderContent_->renderProperties_.GetUseEffect());
    node->renderContent_->renderProperties_.SetUseEffectType(1);
    node->isOnTheTree_ = true;
    node->ProcessBehindWindowOnTreeStateChanged();
    node->isOnTheTree_ = false;
    node->ProcessBehindWindowOnTreeStateChanged();
}

/**
 * @tc.name: ProcessBehindWindowAfterApplyModifiersTest
 * @tc.desc: ProcessBehindWindowAfterApplyModifiersTest
 * @tc.type: FUNC
 * @tc.require: issueIB0UQV
 */
HWTEST_F(RSRenderNodeTest2, ProcessBehindWindowAfterApplyModifiersTest, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSRenderNode>(0, rsContext);
    node->ProcessBehindWindowAfterApplyModifiers();
    auto rootNode = std::make_shared<RSRenderNode>(1);
    rsContext->nodeMap.renderNodeMap_[ExtractPid(1)][1] = rootNode;
    node->renderContent_->renderProperties_.SetUseEffect(false);
    ASSERT_FALSE(node->renderContent_->renderProperties_.GetUseEffect());
    node->ProcessBehindWindowAfterApplyModifiers();
    node->renderContent_->renderProperties_.SetUseEffect(true);
    ASSERT_TRUE(node->renderContent_->renderProperties_.GetUseEffect());
    node->renderContent_->renderProperties_.SetUseEffectType(1);
    node->ProcessBehindWindowAfterApplyModifiers();
}

/**
 * @tc.name: UpdateDrawableBehindWindowTest
 * @tc.desc: UpdateDrawableBehindWindowTest
 * @tc.type: FUNC
 * @tc.require: issueIBDI0L
 */
HWTEST_F(RSRenderNodeTest2, UpdateDrawableBehindWindowTest, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSRenderNode>(0, rsContext);
    node->UpdateDrawableBehindWindow();
    EXPECT_TRUE(node->dirtySlots_.count(RSDrawableSlot::BACKGROUND_FILTER) != 0);
}

/**
 * @tc.name: SetUIFirstSwitchTest001
 * @tc.desc: SetUIFirstSwitch with Node does not have firstLevelNoode
 * @tc.type: FUNC
 * @tc.require: issueIBH5UD
 */
HWTEST_F(RSRenderNodeTest2, SetUIFirstSwitchTest001, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(id);
    ASSERT_NE(node, nullptr);
    node->SetUIFirstSwitch(RSUIFirstSwitch::MODAL_WINDOW_CLOSE);
    ASSERT_EQ(node->GetUIFirstSwitch(), RSUIFirstSwitch::MODAL_WINDOW_CLOSE);
}

/**
 * @tc.name: SetUIFirstSwitchTest002
 * @tc.desc: SetUIFirstSwitch with Node has firstLevelNoode
 * @tc.type: FUNC
 * @tc.require: issueIBH5UD
 */
HWTEST_F(RSRenderNodeTest2, SetUIFirstSwitchTest002, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto node = std::make_shared<RSRenderNode>(id, rsContext);
    ASSERT_NE(node, nullptr);
    auto firstNode = std::make_shared<RSSurfaceRenderNode>(id + 1, rsContext);
    ASSERT_NE(firstNode, nullptr);
    node->firstLevelNodeId_ = id + 1;
    rsContext->nodeMap.RegisterRenderNode(node);
    rsContext->nodeMap.RegisterRenderNode(firstNode);
    node->SetUIFirstSwitch(RSUIFirstSwitch::MODAL_WINDOW_CLOSE);
    ASSERT_EQ(firstNode->GetUIFirstSwitch(), RSUIFirstSwitch::MODAL_WINDOW_CLOSE);
}

/**
 * @tc.name: GenerateIDTest
 * @tc.desc: SetUIFirstSwitch with Node has firstLevelNoode
 * @tc.type: FUNC
 * @tc.require: issueIBH5UD
 */
HWTEST_F(RSRenderNodeTest2, GenerateIDTest, TestSize.Level1)
{
    auto id = RSRenderNode::GenerateId();
    ASSERT_EQ(RSRenderNode::GenerateId() - id, 1);
}
} // namespace Rosen
} // namespace OHOS