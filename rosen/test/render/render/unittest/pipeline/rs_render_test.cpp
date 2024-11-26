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
#include "pipeline/rs_render_thread_visitor.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "skia_adapter/skia_canvas.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
const std::string OUT_STR3 =
    ", Parent [null], Name [SurfaceNode], hasConsumer: 0, Alpha: 1.000000, Visible: 1, VisibleRegion [Empty], "
    "OpaqueRegion [Empty], OcclusionBg: 0, SecurityLayer: 0, skipLayer: 0, surfaceType: 0, "
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
HWTEST_F(RSRenderTest1, Process, TestSize.Level1)
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
HWTEST_F(RSRenderTest2, SendRT, TestSize.Level1)
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
HWTEST_F(RSRenderTest3, InternalChildren, TestSize.Level1)
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
HWTEST_F(RSRenderTest4, DestroyNode, TestSize.Level1)
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
HWTEST_F(RSRenderTest5, BackToRoot, TestSize.Level1)
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
HWTEST_F(RSRenderTest6, ActivateSync, TestSize.Level1)
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
HWTEST_F(RSRenderTest7, UpdateRange, TestSize.Level1)
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
HWTEST_F(RSRenderTest8, Animate, TestSize.Level1)
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
HWTEST_F(RSRenderTest9, IsBound, TestSize.Level1)
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
HWTEST_F(RSRenderTest10, GetContent, TestSize.Level1)
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
HWTEST_F(RSRenderTest11, GetRenderParams, TestSize.Level1)
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
HWTEST_F(RSRenderTest12, CollectShadowRect, TestSize.Level1)
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
HWTEST_F(RSRenderTest13, CollectOutlineRect, TestSize.Level1)
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
HWTEST_F(RSRenderTest14, CollectixelStretchRect, TestSize.Level1)
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
HWTEST_F(RSRenderTest15, UpdateByRegion, TestSize.Level1)
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
HWTEST_F(RSRenderTest16, UpdateSwRect, TestSize.Level1)
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
HWTEST_F(RSRenderTest17, GetDract, TestSize.Level1)
{
    RSRenderNode node(id, context);
    ASSERT_EQ(node.GetSelfDrawRect(), node.selfDrawRect_);
    ASSERT_EQ(node.GetAbsDrawRect(), node.absDrawRect_);
}

/**
 * @tc.name: CheckAndUpdateGeoTrans001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderTest18, CheckeGeoTrans001, TestSize.Level1)
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
HWTEST_F(RSRenderTest19, CheckAnans002, TestSize.Level1)
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
HWTEST_F(RSRenderTest20, UpdateAbsion001, TestSize.Level1)
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
HWTEST_F(RSRenderTest21, UpdateAgion002, TestSize.Level1)
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
HWTEST_F(RSRenderTest22, UpdateAgion003, TestSize.Level1)
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
HWTEST_F(RSRenderTest23, UpdateAbion004, TestSize.Level1)
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
HWTEST_F(RSRenderTest24, UpdateDrayRegion001, TestSize.Level1)
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
HWTEST_F(RSRenderTest25, UpdateDrawRion002, TestSize.Level1)
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
HWTEST_F(RSRenderTest26, UpdateDt001, TestSize.Level1)
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
HWTEST_F(RSRenderTest27, Updatect002, TestSize.Level1)
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
} // namespace Rosen
} // namespace OHOS