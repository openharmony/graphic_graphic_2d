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

/**
 * @tc.name: ProcessRenderText
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderTest1, ProcessRenderText, TestSize.Level1)
{
    RSRenderNode node(id, context);
    std::shared_ptr<RSNodeVisitor> visitor = nullptr;
    node.Process(visitor);
    visitor = std::make_shared<RSRenderThreadVisitor>();
    node.Process(visitor);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: SendRT
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
 * @tc.name: InternalChildren
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
 * @tc.name: DestroyNode
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
 * @tc.name: BackToRoot
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
 * @tc.name: ActivateSync
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
 * @tc.name: UpdateRange
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
 * @tc.name: IsBound
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
 * @tc.name: GetContent
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
 * @tc.name: GetRenderParams
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
 * @tc.name: CollectShadowRect
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
 * @tc.name: CollectOutlineRect
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
 * @tc.name: CollectPixelStretchRect
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderTest14, CollectPixelStretchRect, TestSize.Level1)
{
    RSRenderNode node(id, context);
    node.CollectAndUpdateLocalPixelStretchRect();
    RSDrawableSlot slot = RSDrawableSlot::PIXEL_STRETCH;
    node.dirtySlots_.insert(slot);
    node.CollectAndUpdateLocalPixelStretchRect();
    ASSERT_TRUE(true);
}

/**
 * @tc.name: UpdateByRegion
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
 * @tc.name: UpdateDrawingRect
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderTest16, UpdateDrawingRect, TestSize.Level1)
{
    RSRenderNode node(id, context);
    node.UpdateSelfDrawRect();
    ASSERT_TRUE(true);
}

/**
 * @tc.name: GetRect
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderTest17, GetRect, TestSize.Level1)
{
    RSRenderNode node(id, context);
    ASSERT_EQ(node.GetSelfDrawRect(), node.selfDrawRect_);
    ASSERT_EQ(node.GetAbsDrawRect(), node.absDrawRect_);
}

/**
 * @tc.name: CheckGeoTrans001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderTest18, CheckGeoTrans001, TestSize.Level1)
{
    RSRenderNode node(id, context);
    std::shared_ptr<RSObjAbsGeometry> geoPtr = std::make_shared<RSObjAbsGeometry>();
    ASSERT_EQ(node.CheckAndUpdateGeoTrans(geoPtr), false);
}

/**
 * @tc.name: CheckGeoTrans002
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderTest19, CheckGeoTrans002, TestSize.Level1)
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
 * @tc.name: UpdateAbs001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderTest20, UpdateAbs001, TestSize.Level1)
{
    RSRenderNode node(id, context);
    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    RectI clipRect{0, 0, 1000, 1000};
    node.UpdateAbsDirtyRegion(*rsDirtyManager, clipRect);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: UpdateAbs002
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderTest21, UpdateAbs002, TestSize.Level1)
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
 * @tc.name: UpdateAbs003
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderTest22, UpdateAbs003, TestSize.Level1)
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
 * @tc.name: UpdateAbs004
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderTest23, UpdateAbs004, TestSize.Level1)
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
 * @tc.name: UpdateDrawRegion001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderTest24, UpdateDrawRegion001, TestSize.Level1)
{
    RSRenderNode node(id, context);
    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    RectI clipRect{0, 0, 1000, 1000};
    Drawing::Matrix matrix;
    node.SetDirty();
    ASSERT_EQ(node.UpdateDrawRectAndDirtyRegion(*rsDirtyManager, false, clipRect, matrix), true);
}

/**
 * @tc.name: UpdateDrawRegion002
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderTest25, UpdateDrawRegion002, TestSize.Level1)
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
 * @tc.name: UpdateRect001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderTest26, UpdateRect001, TestSize.Level1)
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
 * @tc.name: UpdateRect002
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderTest27, UpdateRect002, TestSize.Level1)
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
 * @tc.name: RegionTest001
 * @tc.desc: RegionTest001 test
 * @tc.type: FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSRenderTest28, RegionTest001, TestSize.Level1)
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
 * @tc.name: RegionTest002
 * @tc.desc: RegionTest002 test
 * @tc.type: FUNC
 * @tc.require: issueIA5Y41
 */
HWTEST_F(RSRenderTest29, RegionTest002, TestSize.Level1)
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
} // namespace Rosen
} // namespace OHOS