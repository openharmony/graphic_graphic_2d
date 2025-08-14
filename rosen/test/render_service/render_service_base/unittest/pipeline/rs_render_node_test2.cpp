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
#include <parameters.h>

#if defined(MODIFIER_NG)
#include "modifier_ng/appearance/rs_alpha_render_modifier.h"
#include "modifier_ng/geometry/rs_transform_render_modifier.h"
#endif
#include "common/rs_obj_abs_geometry.h"
#include "dirty_region/rs_gpu_dirty_collector.h"
#include "drawable/rs_property_drawable_foreground.h"
#include "metadata_helper.h"
#include "offscreen_render/rs_offscreen_render_thread.h"
#include "params/rs_render_params.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_dirty_region_manager.h"
#include "pipeline/rs_logical_display_render_node.h"
#include "pipeline/rs_screen_render_node.h"
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
    "ContainerConfig: [outR: 0 inR: 0 x: 0 y: 0 w: 0 h: 0], colorSpace: 4, uifirstColorGamut: 4";
const std::string OUT_STR4 = ", Visible: 1, Size: [-inf, -inf], EnableRender: 1";
const std::string OUT_STR5 = ", skipLayer: 0";
const Rect DEFAULT_RECT = { 0, 0, 200, 200 };
const RectF DEFAULT_SELF_DRAW_RECT = { 0, 0, 200, 200 };

const int DEFAULT_NODE_ID = 1;
const uint64_t BUFFER_USAGE_GPU_RENDER_DIRTY = BUFFER_USAGE_HW_RENDER | BUFFER_USAGE_AUXILLARY_BUFFER0;
class RSRenderNodeTest2 : public testing::Test {
public:
    constexpr static float floatData[] = {
        0.0f,
        485.44f,
        -34.4f,
        std::numeric_limits<float>::max(),
        std::numeric_limits<float>::min(),
    };
    static inline BufferRequestConfig requestConfig = {
        .width = 200,
        .height = 200,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA | BUFFER_USAGE_GPU_RENDER_DIRTY,
        .timeout = 0,
        .colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DCI_P3,
    };

    static inline BufferSelfDrawingData defaultSelfDrawingRect = {
        .gpuDirtyEnable = true,
        .curFrameDirtyEnable = true,
        .left = 0,
        .top = 0,
        .right = 100,
        .bottom = 100,
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
    int64_t leftDelayTime = 0;
    node.Animate(timestamp, leftDelayTime, period, isDisplaySyncEnabled);
    node.displaySync_ = std::make_shared<RSRenderDisplaySync>(1);
    node.Animate(timestamp, leftDelayTime, period, isDisplaySyncEnabled);
    auto context_shared = std::make_shared<RSContext>();
    std::weak_ptr<RSContext> context2 = context_shared;
    RSRenderNode node2(id, context2);
    node2.Animate(timestamp, leftDelayTime, period, isDisplaySyncEnabled);
    RSSurfaceRenderNode node3(id, context2);
    node3.Animate(timestamp, leftDelayTime, period, isDisplaySyncEnabled);
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
 * @tc.name: UpdateBufferDirtyRegion002
 * @tc.desc: test UpdateBufferDirtyRegion when gpuDirtyRegion is valid
 * @tc.type: FUNC
 * @tc.require: issuesICA3L1
 */
HWTEST_F(RSRenderNodeTest2, UpdateBufferDirtyRegion002, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(0);
    ASSERT_TRUE(surfaceNode->GetRSSurfaceHandler() != nullptr);
    auto buffer = SurfaceBuffer::Create();
    auto ret = buffer->Alloc(requestConfig);
    ASSERT_EQ(ret, GSERROR_OK);
 
    auto src = RSGpuDirtyCollector::GetBufferSelfDrawingData(buffer);
    ASSERT_NE(src, nullptr);
    (*src) = defaultSelfDrawingRect;
 
    surfaceNode->GetRSSurfaceHandler()->buffer_.buffer = buffer;
    ASSERT_TRUE(surfaceNode->GetRSSurfaceHandler()->GetBuffer() != nullptr);
    surfaceNode->GetRSSurfaceHandler()->buffer_.damageRect = DEFAULT_RECT;
    surfaceNode->selfDrawRect_ = DEFAULT_SELF_DRAW_RECT;
    auto param = system::GetParameter("rosen.graphic.selfdrawingdirtyregion.enabled", "");
    system::SetParameter("rosen.graphic.selfdrawingdirtyregion.enabled", "1");
    surfaceNode->UpdateBufferDirtyRegion();
    ASSERT_EQ(surfaceNode->selfDrawingNodeDirtyRect_.width_, 100);
    system::SetParameter("rosen.graphic.selfdrawingdirtyregion.enabled", param);
}

/**
 * @tc.name: UpdateBufferDirtyRegion003
 * @tc.desc: test UpdateBufferDirtyRegion when upper layer don't have selfDrawingDirtyRegion
 * @tc.type: FUNC
 * @tc.require: issuesICA3L1
 */
HWTEST_F(RSRenderNodeTest2, UpdateBufferDirtyRegion003, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(0);
    auto buffer = SurfaceBuffer::Create();
    auto ret = buffer->Alloc(requestConfig);
    ASSERT_EQ(ret, GSERROR_OK);
    ASSERT_TRUE(surfaceNode->GetRSSurfaceHandler() != nullptr);
    surfaceNode->GetRSSurfaceHandler()->buffer_.damageRect = DEFAULT_RECT;
    surfaceNode->GetRSSurfaceHandler()->buffer_.buffer = buffer;
    ASSERT_TRUE(surfaceNode->GetRSSurfaceHandler()->GetBuffer() != nullptr);
    surfaceNode->selfDrawRect_ = DEFAULT_SELF_DRAW_RECT;
    surfaceNode->UpdateBufferDirtyRegion();
    ASSERT_EQ(surfaceNode->selfDrawingNodeDirtyRect_.width_, 200);
}

/**
 * @tc.name: UpdateBufferDirtyRegion004
 * @tc.desc: test UpdateBufferDirtyRegion when upper layer has selfDrawingDirtyRegion
 * @tc.type: FUNC
 * @tc.require: issuesICA3L1
 */
HWTEST_F(RSRenderNodeTest2, UpdateBufferDirtyRegion004, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(0);
    auto buffer = SurfaceBuffer::Create();
    ASSERT_TRUE(surfaceNode->GetRSSurfaceHandler() != nullptr);
    surfaceNode->GetRSSurfaceHandler()->buffer_.damageRect = DEFAULT_RECT;
    surfaceNode->GetRSSurfaceHandler()->buffer_.buffer = buffer;
    ASSERT_TRUE(surfaceNode->GetRSSurfaceHandler()->GetBuffer() != nullptr);
    surfaceNode->selfDrawRect_ = DEFAULT_SELF_DRAW_RECT;
    surfaceNode->UpdateBufferDirtyRegion();
    ASSERT_EQ(surfaceNode->selfDrawingNodeDirtyRect_.width_, 200);
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
    node.drawCmdModifiers_ = map;
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
    node.drawCmdModifiers_ = map;
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
 * @tc.name: CheckAndUpdateAIBarCacheStatus
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, CheckAndUpdateAIBarCacheStatus, TestSize.Level1)
{
    RSRenderNode node(id, context);
    ASSERT_EQ(node.CheckAndUpdateAIBarCacheStatus(false), false);
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
    RSBaseRenderNode rootNode(id + 1, context);
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
    RSBaseRenderNode rootNode(id + 1, context);
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
    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    RSBaseRenderNode subTreeRoot(id + 1, context);
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
    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    RSBaseRenderNode subTreeRoot(id + 1, context);
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
    node.UpdateFilterCacheWithBelowDirty(Occlusion::Rect(rsDirtyManager->GetCurrentFrameDirtyRegion()), isForeground);
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

    ScreenId screenId = 1;
    auto rsContext = std::make_shared<RSContext>();
    std::shared_ptr<RSScreenRenderNode> nodeTest3 = std::make_shared<RSScreenRenderNode>(0, screenId, rsContext);
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
#if defined(MODIFIER_NG)
    auto modifier = std::make_shared<ModifierNG::RSCustomRenderModifier<ModifierNG::RSModifierType::CONTENT_STYLE>>();
    auto property = std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>();
    modifier->AttachProperty(ModifierNG::RSPropertyType::CONTENT_STYLE, property);
    nodeTest->modifiersNG_[static_cast<uint16_t>(ModifierNG::RSModifierType::CONTENT_STYLE)].emplace_back(modifier);
    auto modifier2 = std::make_shared<ModifierNG::RSAlphaRenderModifier>();
    auto property2 = std::make_shared<RSRenderProperty<float>>();
    modifier2->AttachProperty(ModifierNG::RSPropertyType::ALPHA, property2);
    nodeTest->modifiersNG_[static_cast<uint16_t>(ModifierNG::RSModifierType::ALPHA)].emplace_back(modifier2);
#else
    std::shared_ptr<RSDrawCmdListRenderModifier> drawCmdModifiersTest =
        std::make_shared<RSDrawCmdListRenderModifier>(propertyTest);
    EXPECT_NE(drawCmdModifiersTest, nullptr);
    nodeTest->drawCmdModifiers_[RSModifierType::CHILDREN].emplace_back(drawCmdModifiersTest);
#endif
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
    EXPECT_NE(nodeTest->renderProperties_.boundsGeo_, nullptr);
    nodeTest->hasChildrenOutOfRect_ = true;
    nodeTest->SubTreeSkipPrepare(dirtyManagerTest2, true, true, clipRectTest2);

    RSDirtyRegionManager dirtyManagerTest3;
    RectI clipRectTest3 = RectI { 0, 0, 1, 1 };
    nodeTest->srcOrClipedAbsDrawRectChangeFlag_ = true;
    nodeTest->hasChildrenOutOfRect_ = false;
    nodeTest->lastFrameHasChildrenOutOfRect_ = true;
    nodeTest->renderProperties_.boundsGeo_ = nullptr;
    nodeTest->SubTreeSkipPrepare(dirtyManagerTest3, false, true, clipRectTest3);
}

/**
 * @tc.name: IsUifirstArkTsCardNodeTest034
 * @tc.desc: Prepare QuickPrepare IsUifirstArkTsCardNode test
 * @tc.type: FUNC
 * @tc.require: issueIA5Y41
 */
HWTEST_F(RSRenderNodeTest2, IsUifirstArkTsCardNodeTest034, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> nodeTest = std::make_shared<RSSurfaceRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);

    std::shared_ptr<RSNodeVisitor> visitor = nullptr;
    nodeTest->Prepare(visitor);
    nodeTest->QuickPrepare(visitor);

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
    node.renderProperties_.SetBounds(bounds);
    node.CollectAndUpdateLocalDistortionEffectRect();
    EXPECT_FALSE(node.localDistortionEffectRect_.width_ > static_cast<int>(width));

    node.renderProperties_.SetDistortionK(0.5f); // 0.5 is k of value in distortion
    EXPECT_TRUE(node.renderProperties_.GetDistortionDirty());
    node.CollectAndUpdateLocalDistortionEffectRect();
    EXPECT_FALSE(node.renderProperties_.GetDistortionDirty());
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
    node->renderProperties_.SetUseEffect(true);
    ASSERT_TRUE(node->renderProperties_.GetUseEffect());
    node->renderProperties_.SetUseEffectType(1);
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
    node->renderProperties_.SetUseEffect(false);
    ASSERT_FALSE(node->renderProperties_.GetUseEffect());
    node->ProcessBehindWindowAfterApplyModifiers();
    node->renderProperties_.SetUseEffect(true);
    ASSERT_TRUE(node->renderProperties_.GetUseEffect());
    node->renderProperties_.SetUseEffectType(1);
    node->ProcessBehindWindowAfterApplyModifiers();
}

/**
 * @tc.name: SetIsOnTheTree
 * @tc.desc: SetIsOnTheTree ResetChildRelevantFlags and UpdateChildrenRect test
 * @tc.type: FUNC
 * @tc.require: issueI9US6V
 */
HWTEST_F(RSRenderNodeTest2, SetIsOnTheTree, TestSize.Level1)
{
    auto node = std::make_shared<RSCanvasRenderNode>(0);
    node->isOnTheTree_ = false;
    node->hasHdrPresent_ = true;
    node->renderProperties_.hdrUIBrightness_ = 2.0f; // hdr brightness
    node->SetIsOnTheTree(true, 0, 1, 1, 1);
    EXPECT_EQ(node->isOnTheTree_, true);

    node->hasHdrPresent_ = false;
    node->SetIsOnTheTree(false, 0, 1, 1, 1);
    EXPECT_EQ(node->isOnTheTree_, false);
}

/**
 * @tc.name: SetIsOnTheTreeTest
 * @tc.desc: SetIsOnTheTree ResetChildRelevantFlags and UpdateChildrenRect test
 * @tc.type: FUNC
 * @tc.require: issueI9US6V
 */
HWTEST_F(RSRenderNodeTest2, SetIsOnTheTreeTest, TestSize.Level1)
{
    // SetIsOnTheTree test
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(DEFAULT_NODE_ID, context);
    canvasNode->InitRenderParams();
    EXPECT_NE(canvasNode, nullptr);

    canvasNode->SetHDRPresent(true);
    canvasNode->isOnTheTree_ = false;
    canvasNode->SetIsOnTheTree(false, 0, 1, 1, 1);

    canvasNode->isOnTheTree_ = true;
    canvasNode->SetIsOnTheTree(false, 0, 1, 1, 1);
    EXPECT_FALSE(canvasNode->isOnTheTree_);

    canvasNode->children_.emplace_back(std::make_shared<RSRenderNode>(0));
    canvasNode->children_.emplace_back(std::make_shared<RSRenderNode>(0));
    std::shared_ptr<RSRenderNode> renderNodeTest = nullptr;
    canvasNode->children_.emplace_back(renderNodeTest);

    canvasNode->disappearingChildren_.emplace_back(std::make_shared<RSRenderNode>(0), 0);
    canvasNode->SetIsOnTheTree(true, 0, 1, 1, 1);
    EXPECT_TRUE(canvasNode->isOnTheTree_);

    // ResetChildRelevantFlags test
    canvasNode->ResetChildRelevantFlags();
    EXPECT_FALSE(canvasNode->hasChildrenOutOfRect_);

    // UpdateChildrenRect test
    RectI subRect = RectI { 0, 0, 1, 1 };
    canvasNode->childrenRect_.Clear();
    canvasNode->UpdateChildrenRect(subRect);
    canvasNode->UpdateChildrenRect(subRect);
}

/**
 * @tc.name: SetIsOnTheTreeTest02
 * @tc.desc: SetIsOnTheTree ResetChildRelevantFlags and UpdateChildrenRect test
 * @tc.type: FUNC
 * @tc.require: issueI9US6V
 */
HWTEST_F(RSRenderNodeTest2, SetIsOnTheTreeTest02, TestSize.Level1)
{
    // SetIsOnTheTree test
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(DEFAULT_NODE_ID, context);
    canvasNode->InitRenderParams();
    EXPECT_NE(canvasNode, nullptr);

    canvasNode->SetHDRPresent(true);

    std::shared_ptr<RSContext> contextTest = std::make_shared<RSContext>();
    EXPECT_NE(contextTest, nullptr);

    canvasNode->context_ = contextTest;
    canvasNode->isOnTheTree_ = false;
    canvasNode->SetIsOnTheTree(false, 0, 1, 1, 1);

    canvasNode->isOnTheTree_ = true;
    canvasNode->SetIsOnTheTree(false, 0, 1, 1, 1);
    EXPECT_FALSE(canvasNode->isOnTheTree_);
}

/**
 * @tc.name: SetHdrNum
 * @tc.desc: SetHdrNum test
 * @tc.type: FUNC
 * @tc.require: issueI9US6V
 */
HWTEST_F(RSRenderNodeTest2, SetHdrNum, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    EXPECT_NE(rsContext, nullptr);
    auto node = std::make_shared<RSRenderNode>(0, rsContext);
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(1);
    EXPECT_NE(surfaceNode, nullptr);
    rsContext->nodeMap.renderNodeMap_[ExtractPid(1)][1] = surfaceNode;
    node->SetHdrNum(true, 1, HDRComponentType::IMAGE);
    EXPECT_EQ(surfaceNode->hdrPhotoNum_, 1);
    node->SetHdrNum(true, 1, HDRComponentType::UICOMPONENT);
    EXPECT_EQ(surfaceNode->hdrUIComponentNum_, 1);
    node->SetHdrNum(false, 1, HDRComponentType::IMAGE);
    EXPECT_EQ(surfaceNode->hdrPhotoNum_, 0);
    node->SetHdrNum(false, 1, HDRComponentType::UICOMPONENT);
    EXPECT_EQ(surfaceNode->hdrUIComponentNum_, 0);
}

/**
 * @tc.name: SetEnableHdrEffect
 * @tc.desc: SetEnableHdrEffect test
 * @tc.type: FUNC
 * @tc.require: issueI9US6V
 */
HWTEST_F(RSRenderNodeTest2, SetEnableHdrEffect, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    EXPECT_NE(rsContext, nullptr);
    auto node = std::make_shared<RSRenderNode>(0, rsContext);
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(1);
    EXPECT_NE(surfaceNode, nullptr);
    rsContext->nodeMap.renderNodeMap_[ExtractPid(1)][1] = surfaceNode;
    node->SetEnableHdrEffect(false);
    EXPECT_EQ(surfaceNode->hdrEffectNum_, 0);
    node->SetEnableHdrEffect(true);
    EXPECT_EQ(surfaceNode->hdrEffectNum_, 0);
    node->SetIsOnTheTree(true, 0, 1, 1, 1);
    EXPECT_EQ(surfaceNode->hdrEffectNum_, 0);
    node->SetEnableHdrEffect(false);
    EXPECT_EQ(surfaceNode->hdrEffectNum_, 0);
}

/**
 * @tc.name: RSRenderNodeDumpTest
 * @tc.desc: DumpNodeType DumpTree and DumpSubClassNode test
 * @tc.type: FUNC
 * @tc.require: issueI9US6V
 */
HWTEST_F(RSRenderNodeTest2, RSRenderNodeDumpTest, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);

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
    nodeTest->drawCmdModifiers_.clear();
    nodeTest->isFullChildrenListValid_ = false;
    nodeTest->disappearingChildren_.emplace_back(std::make_shared<RSRenderNode>(0), 0);
    nodeTest->DumpTree(257, outTest2);
    EXPECT_EQ(outTest2, "===== WARNING: exceed max depth for dumping render node tree =====\n");
}

/**
 * @tc.name: ForceMergeSubTreeDirtyRegionTest02
 * @tc.desc: ForceMergeSubTreeDirtyRegion SubTreeSkipPrepare test
 * @tc.type: FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSRenderNodeTest2, ForceMergeSubTreeDirtyRegionTest02, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> nodeTest = std::make_shared<RSSurfaceRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);

    RSDirtyRegionManager dirtyManagerTest1;
    RectI clipRectTest1 = RectI { 0, 0, 1, 1 };
    nodeTest->lastFrameSubTreeSkipped_ = true;
    nodeTest->geoUpdateDelay_ = false;
    nodeTest->ForceMergeSubTreeDirtyRegion(dirtyManagerTest1, clipRectTest1);
    EXPECT_FALSE(nodeTest->lastFrameSubTreeSkipped_);

    RSDirtyRegionManager dirtyManagerTest2;
    RectI clipRectTest2 = RectI { 0, 0, 1, 1 };
    nodeTest->lastFrameHasChildrenOutOfRect_ = false;
    EXPECT_NE(nodeTest->renderProperties_.boundsGeo_, nullptr);
    nodeTest->hasChildrenOutOfRect_ = true;
    nodeTest->SubTreeSkipPrepare(dirtyManagerTest2, false, false, clipRectTest2);

    RSDirtyRegionManager dirtyManagerTest3;
    RectI clipRectTest3 = RectI { 0, 0, 1, 1 };
    nodeTest->srcOrClipedAbsDrawRectChangeFlag_ = true;
    nodeTest->hasChildrenOutOfRect_ = false;
    nodeTest->lastFrameHasChildrenOutOfRect_ = false;
    nodeTest->renderProperties_.boundsGeo_ = nullptr;
    nodeTest->SubTreeSkipPrepare(dirtyManagerTest3, false, true, clipRectTest3);
}

/**
 * @tc.name: IsSubTreeNeedPrepareTest02
 * @tc.desc: Prepare QuickPrepare IsSubTreeNeedPrepare IsUifirstArkTsCardNode test
 * @tc.type: FUNC
 * @tc.require: issueIA5Y41
 */
HWTEST_F(RSRenderNodeTest2, IsSubTreeNeedPrepareTest02, TestSize.Level1)
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

    nodeTest->childHasSharedTransition_ = false;
    nodeTest->isAccumulatedClipFlagChanged_ = true;
    nodeTest->childHasVisibleFilter_ = true;
    EXPECT_TRUE(nodeTest->IsSubTreeNeedPrepare(false, false));

    nodeTest->nodeGroupType_ = RSRenderNode::NONE;
    EXPECT_FALSE(nodeTest->IsUifirstArkTsCardNode());
    nodeTest->nodeGroupType_ = RSRenderNode::GROUPED_BY_ANIM;
    EXPECT_FALSE(nodeTest->IsUifirstArkTsCardNode());
}

/**
 * @tc.name: UpdateRenderStatus02
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeTest2, UpdateRenderStatus02, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> nodeTest = std::make_shared<RSSurfaceRenderNode>(0);
    ASSERT_TRUE(nodeTest->IsNodeDirty());
    RectI dirtyRegion;
    bool isPartialRenderEnabled = false;
    nodeTest->hasChildrenOutOfRect_ = true;
    nodeTest->UpdateRenderStatus(dirtyRegion, isPartialRenderEnabled);
}

/**
 * @tc.name: RSRenderNodeDumpTest02
 * @tc.desc: DumpNodeType DumpTree and DumpSubClassNode test
 * @tc.type: FUNC
 * @tc.require: issueI9US6V
 */
HWTEST_F(RSRenderNodeTest2, RSRenderNodeDumpTest02, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);

    std::string outTest = "";
    std::shared_ptr<RSRenderNode> inNode = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(inNode, nullptr);
    std::shared_ptr<RSRenderNode> outNode = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(outNode, nullptr);
    nodeTest->sharedTransitionParam_ = std::make_shared<SharedTransitionParam>(inNode, outNode, true);
    EXPECT_NE(nodeTest->sharedTransitionParam_, nullptr);
    nodeTest->nodeGroupType_ = RSRenderNode::GROUPED_BY_ANIM;
    nodeTest->uifirstRootNodeId_ = 1;
    nodeTest->SetBootAnimation(false);
    nodeTest->SetContainBootAnimation(false);
    nodeTest->dirtyStatus_ = RSRenderNode::NodeDirty::DIRTY;
    nodeTest->renderProperties_.isDirty_ = false;
    nodeTest->isSubTreeDirty_ = false;
    nodeTest->renderProperties_.isDrawn_ = false;
    nodeTest->renderProperties_.alphaNeedApply_ = false;
    nodeTest->drawCmdModifiers_.clear();
    nodeTest->isFullChildrenListValid_ = false;
    nodeTest->disappearingChildren_.emplace_back(std::make_shared<RSRenderNode>(0), 0);
    nodeTest->DumpTree(257, outTest);
}

/**
 * @tc.name: ForceMergeSubTreeDirtyRegionTest03
 * @tc.desc: ForceMergeSubTreeDirtyRegion SubTreeSkipPrepare test
 * @tc.type: FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSRenderNodeTest2, ForceMergeSubTreeDirtyRegionTest03, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> nodeTest = std::make_shared<RSSurfaceRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);

    RSDirtyRegionManager dirtyManagerTest1;
    RectI clipRectTest1 = RectI { 0, 0, 1, 1 };
    nodeTest->lastFrameSubTreeSkipped_ = true;
    nodeTest->geoUpdateDelay_ = false;
    nodeTest->ForceMergeSubTreeDirtyRegion(dirtyManagerTest1, clipRectTest1);
    EXPECT_FALSE(nodeTest->lastFrameSubTreeSkipped_);

    RSDirtyRegionManager dirtyManagerTest2;
    RectI clipRectTest2 = RectI { 0, 0, 1, 1 };
    nodeTest->lastFrameHasChildrenOutOfRect_ = true;
    EXPECT_NE(nodeTest->renderProperties_.boundsGeo_, nullptr);
    nodeTest->hasChildrenOutOfRect_ = true;
    nodeTest->SubTreeSkipPrepare(dirtyManagerTest2, true, true, clipRectTest2);

    RSDirtyRegionManager dirtyManagerTest3;
    RectI clipRectTest3 = RectI { 0, 0, 1, 1 };
    nodeTest->srcOrClipedAbsDrawRectChangeFlag_ = true;
    nodeTest->hasChildrenOutOfRect_ = false;
    nodeTest->lastFrameHasChildrenOutOfRect_ = false;
    nodeTest->renderProperties_.boundsGeo_ = nullptr;
    nodeTest->SubTreeSkipPrepare(dirtyManagerTest3, false, true, clipRectTest3);
}

/**
 * @tc.name: ForceMergeSubTreeDirtyRegionTest04
 * @tc.desc: ForceMergeSubTreeDirtyRegion SubTreeSkipPrepare test
 * @tc.type: FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSRenderNodeTest2, ForceMergeSubTreeDirtyRegionTest04, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> nodeTest4 = std::make_shared<RSSurfaceRenderNode>(0);
    EXPECT_NE(nodeTest4, nullptr);

    RSDirtyRegionManager dirtyManagerTest1;
    RectI clipRectTest1 = RectI { 0, 0, 1, 1 };
    nodeTest4->lastFrameSubTreeSkipped_ = true;
    nodeTest4->geoUpdateDelay_ = false;
    nodeTest4->ForceMergeSubTreeDirtyRegion(dirtyManagerTest1, clipRectTest1);
    EXPECT_FALSE(nodeTest4->lastFrameSubTreeSkipped_);

    RSDirtyRegionManager dirtyManagerTest2;
    RectI clipRectTest2 = RectI { 0, 0, 1, 1 };
    nodeTest4->lastFrameHasChildrenOutOfRect_ = false;
    EXPECT_NE(nodeTest4->renderProperties_.boundsGeo_, nullptr);
    nodeTest4->hasChildrenOutOfRect_ = true;
    nodeTest4->SubTreeSkipPrepare(dirtyManagerTest2, false, false, clipRectTest2);

    RSDirtyRegionManager dirtyManagerTest3;
    RectI clipRectTest3 = RectI { 0, 0, 1, 1 };
    nodeTest4->srcOrClipedAbsDrawRectChangeFlag_ = true;
    nodeTest4->hasChildrenOutOfRect_ = true;
    nodeTest4->lastFrameHasChildrenOutOfRect_ = true;
    nodeTest4->renderProperties_.boundsGeo_ = nullptr;
    nodeTest4->SubTreeSkipPrepare(dirtyManagerTest3, false, true, clipRectTest3);
}

/**
 * @tc.name: PostPrepareForBlurFilterNode03
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueIB0UQV
 */
HWTEST_F(RSRenderNodeTest2, PostPrepareForBlurFilterNode03, TestSize.Level1)
{
    ASSERT_TRUE(RSProperties::filterCacheEnabled_);
    RSRenderNode node(id, context);
    bool needRequestNextVsync = true;
    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    auto& properties = node.GetMutableRenderProperties();
    properties.needDrawBehindWindow_ = false;
    node.PostPrepareForBlurFilterNode(*rsDirtyManager, needRequestNextVsync);
    RSDrawableSlot slot = RSDrawableSlot::BACKGROUND_FILTER;
    node.drawableVec_[static_cast<uint32_t>(slot)] = std::make_shared<DrawableV2::RSFilterDrawable>();
    node.PostPrepareForBlurFilterNode(*rsDirtyManager, needRequestNextVsync);
    ASSERT_NE(node.GetFilterDrawable(false), nullptr);
}

/**
 * @tc.name: ManageDrawingCacheTest02
 * @tc.desc: SetDrawingCacheChanged and ResetDrawingCacheNeedUpdate test
 * @tc.type: FUNC
 * @tc.require: issueI9US6V
 */
HWTEST_F(RSRenderNodeTest2, ManageDrawingCacheTest02, TestSize.Level2)
{
    // SetDrawingCacheChanged test
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);
    std::unique_ptr<RSRenderParams> stagingRenderParams = std::make_unique<RSRenderParams>(0);
    EXPECT_NE(stagingRenderParams, nullptr);
    nodeTest->stagingRenderParams_ = std::move(stagingRenderParams);

    nodeTest->lastFrameSynced_ = false;
    nodeTest->stagingRenderParams_->needSync_ = true;
    nodeTest->stagingRenderParams_->isDrawingCacheChanged_ = true;
    nodeTest->SetDrawingCacheChanged(true);
    EXPECT_TRUE(nodeTest->stagingRenderParams_->needSync_);

    nodeTest->SetDrawingCacheChanged(false);
    EXPECT_TRUE(nodeTest->stagingRenderParams_->isDrawingCacheChanged_);

    nodeTest->lastFrameSynced_ = false;
    nodeTest->stagingRenderParams_->needSync_ = false;
    nodeTest->SetDrawingCacheChanged(true);
    EXPECT_TRUE(nodeTest->stagingRenderParams_->needSync_);
    EXPECT_TRUE(nodeTest->stagingRenderParams_->isDrawingCacheChanged_);

    // ResetDrawingCacheNeedUpdate test
    nodeTest->drawingCacheNeedUpdate_ = true;
    nodeTest->ResetDrawingCacheNeedUpdate();
    EXPECT_FALSE(nodeTest->drawingCacheNeedUpdate_);

    // GetDrawingCacheChanged test
    EXPECT_TRUE(nodeTest->GetDrawingCacheChanged());
}

/**
 * @tc.name: ManageDrawingCacheTest03
 * @tc.desc: SetDrawingCacheChanged and ResetDrawingCacheNeedUpdate test
 * @tc.type: FUNC
 * @tc.require: issueI9US6V
 */
HWTEST_F(RSRenderNodeTest2, ManageDrawingCacheTest03, TestSize.Level2)
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
    nodeTest->stagingRenderParams_->needSync_ = true;
    nodeTest->SetDrawingCacheChanged(true);
    EXPECT_TRUE(nodeTest->stagingRenderParams_->needSync_);
    EXPECT_TRUE(nodeTest->stagingRenderParams_->isDrawingCacheChanged_);

    // ResetDrawingCacheNeedUpdate test
    nodeTest->drawingCacheNeedUpdate_ = false;
    nodeTest->ResetDrawingCacheNeedUpdate();
    EXPECT_FALSE(nodeTest->drawingCacheNeedUpdate_);

    // GetDrawingCacheChanged test
    EXPECT_TRUE(nodeTest->GetDrawingCacheChanged());
}

/**
 * @tc.name: ManageDrawingCacheTest04
 * @tc.desc: SetDrawingCacheChanged and ResetDrawingCacheNeedUpdate test
 * @tc.type: FUNC
 * @tc.require: issueI9US6V
 */
HWTEST_F(RSRenderNodeTest2, ManageDrawingCacheTest04, TestSize.Level2)
{
    // SetDrawingCacheChanged test
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);
    std::unique_ptr<RSRenderParams> stagingRenderParams = std::make_unique<RSRenderParams>(0);
    EXPECT_NE(stagingRenderParams, nullptr);
    nodeTest->stagingRenderParams_ = std::move(stagingRenderParams);

    nodeTest->lastFrameSynced_ = false;
    nodeTest->stagingRenderParams_->needSync_ = false;
    nodeTest->stagingRenderParams_->isDrawingCacheChanged_ = false;
    nodeTest->SetDrawingCacheChanged(true);
    EXPECT_TRUE(nodeTest->stagingRenderParams_->needSync_);

    nodeTest->SetDrawingCacheChanged(false);
    EXPECT_TRUE(nodeTest->stagingRenderParams_->isDrawingCacheChanged_);

    nodeTest->lastFrameSynced_ = false;
    nodeTest->stagingRenderParams_->needSync_ = false;
    nodeTest->SetDrawingCacheChanged(true);
    EXPECT_TRUE(nodeTest->stagingRenderParams_->needSync_);
    EXPECT_TRUE(nodeTest->stagingRenderParams_->isDrawingCacheChanged_);

    // ResetDrawingCacheNeedUpdate test
    nodeTest->drawingCacheNeedUpdate_ = true;
    nodeTest->ResetDrawingCacheNeedUpdate();
    EXPECT_FALSE(nodeTest->drawingCacheNeedUpdate_);

    // GetDrawingCacheChanged test
    EXPECT_TRUE(nodeTest->GetDrawingCacheChanged());
}

/**
 * @tc.name: ManageDrawingCacheTest05
 * @tc.desc: SetDrawingCacheChanged and ResetDrawingCacheNeedUpdate test
 * @tc.type: FUNC
 * @tc.require: issueI9US6V
 */
HWTEST_F(RSRenderNodeTest2, ManageDrawingCacheTest05, TestSize.Level2)
{
    // SetDrawingCacheChanged test
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);
    std::unique_ptr<RSRenderParams> stagingRenderParams = std::make_unique<RSRenderParams>(0);
    EXPECT_NE(stagingRenderParams, nullptr);
    nodeTest->stagingRenderParams_ = std::move(stagingRenderParams);

    nodeTest->lastFrameSynced_ = true;
    nodeTest->stagingRenderParams_->needSync_ = true;
    nodeTest->stagingRenderParams_->isDrawingCacheChanged_ = true;
    nodeTest->SetDrawingCacheChanged(true);
    EXPECT_TRUE(nodeTest->stagingRenderParams_->needSync_);

    nodeTest->SetDrawingCacheChanged(true);
    EXPECT_TRUE(nodeTest->stagingRenderParams_->isDrawingCacheChanged_);

    nodeTest->lastFrameSynced_ = false;
    nodeTest->stagingRenderParams_->needSync_ = true;
    nodeTest->SetDrawingCacheChanged(true);
    EXPECT_TRUE(nodeTest->stagingRenderParams_->needSync_);
    EXPECT_TRUE(nodeTest->stagingRenderParams_->isDrawingCacheChanged_);

    // ResetDrawingCacheNeedUpdate test
    nodeTest->drawingCacheNeedUpdate_ = false;
    nodeTest->ResetDrawingCacheNeedUpdate();
    EXPECT_FALSE(nodeTest->drawingCacheNeedUpdate_);

    // GetDrawingCacheChanged test
    EXPECT_TRUE(nodeTest->GetDrawingCacheChanged());
}

/**
 * @tc.name: SetGeoUpdateDelay
 * @tc.desc: test SetGeoUpdateDelay once
 * @tc.type: FUNC
 * @tc.require: issueI8JMN8
 */
HWTEST_F(RSRenderNodeTest2,  SetGeoUpdateDelay, TestSize.Level2)
{
    RSRenderNode node(id, context);
    // test default value
    ASSERT_EQ(node.GetGeoUpdateDelay(), false);

    node.SetGeoUpdateDelay(true);
    ASSERT_EQ(node.GetGeoUpdateDelay(), true);
    node.SetGeoUpdateDelay(false);
    ASSERT_EQ(node.GetGeoUpdateDelay(), true);
    node.ResetGeoUpdateDelay();
    ASSERT_EQ(node.GetGeoUpdateDelay(), false);
}

/**
 * @tc.name: UpdateDirtyRegionTest02
 * @tc.desc: check dirty region add successfully
 * @tc.type: FUNC
 * @tc.require: issueI97LXT
 */
HWTEST_F(RSRenderNodeTest2, UpdateDirtyRegionTest02, TestSize.Level1)
{
    RSRenderNode node(id, context);
    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    rsDirtyManager->SetSurfaceSize(1000, 1000);
    RectI clipRect{0, 0, 1000, 1000};
    node.SetDirty();
    node.shouldPaint_ = false;
    RectI absRect = RectI{0, 0, 100, 100};
    auto& properties = node.GetMutableRenderProperties();
    properties.boundsGeo_->absRect_ = absRect;
    properties.clipToBounds_ = true;
    bool geoDirty = false;
    node.UpdateDirtyRegion(*rsDirtyManager, geoDirty, clipRect);
    bool isDirtyRectCorrect = (rsDirtyManager->GetCurrentFrameDirtyRegion() == absRect);
    ASSERT_EQ(isDirtyRectCorrect, false);
}

/**
 * @tc.name: GenerateFullChildrenListTest02
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest2, GenerateFullChildrenListTest02, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(id, context);
    node->GenerateFullChildrenList();

    // children_ isn't empty
    std::weak_ptr<RSRenderNode> wNode1;
    node->children_.emplace_back(wNode1); // wNode1 is nullptr
    node->GenerateFullChildrenList();
    EXPECT_TRUE(node->children_.empty());
    std::shared_ptr<RSRenderNode> sNode1 = std::make_shared<RSRenderNode>(id, context);
    sNode1->isBootAnimation_ = false;
    wNode1 = sNode1;
    node->children_.emplace_back(wNode1); // wNode1 isn't nullptr

    node->isContainBootAnimation_ = false;
    node->GenerateFullChildrenList();

    node->isContainBootAnimation_ = true;
    node->GenerateFullChildrenList();

    // disappearingChildren_ isn't empty
    node->disappearingChildren_.emplace_back(sNode1, id);
    node->GenerateFullChildrenList();

    node->isContainBootAnimation_ = false;
    node->GenerateFullChildrenList();
}

/**
 * @tc.name: UpdatePointLightDirtySlotTest02
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest2, UpdatePointLightDirtySlotTest02, TestSize.Level1)
{
    RSRenderNode node(id, context);
    EXPECT_TRUE(node.dirtySlots_.empty());
    node.UpdateDirtySlotsAndPendingNodes(RSDrawableSlot::TRANSITION);
    EXPECT_FALSE(node.dirtySlots_.empty());

    node.UpdateDirtySlotsAndPendingNodes(RSDrawableSlot::ENV_FOREGROUND_COLOR);
    EXPECT_FALSE(node.dirtySlots_.empty());

    node.UpdateDirtySlotsAndPendingNodes(RSDrawableSlot::SHADOW);
    EXPECT_FALSE(node.dirtySlots_.empty());

    node.UpdateDirtySlotsAndPendingNodes(RSDrawableSlot::BACKGROUND_COLOR);
    EXPECT_FALSE(node.dirtySlots_.empty());

    node.UpdateDirtySlotsAndPendingNodes(RSDrawableSlot::BACKGROUND_SHADER);
    EXPECT_FALSE(node.dirtySlots_.empty());

    node.UpdateDirtySlotsAndPendingNodes(RSDrawableSlot::BACKGROUND_IMAGE);
    EXPECT_FALSE(node.dirtySlots_.empty());

    node.UpdateDirtySlotsAndPendingNodes(RSDrawableSlot::BACKGROUND_FILTER);
    EXPECT_FALSE(node.dirtySlots_.empty());
}

/**
 * @tc.name: UpdatePointLightDirtySlotTest03
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest2, UpdatePointLightDirtySlotTest03, TestSize.Level1)
{
    RSRenderNode node(id, context);
    EXPECT_TRUE(node.dirtySlots_.empty());
    node.UpdateDirtySlotsAndPendingNodes(RSDrawableSlot::TRANSITION);
    EXPECT_FALSE(node.dirtySlots_.empty());

    node.UpdateDirtySlotsAndPendingNodes(RSDrawableSlot::ENV_FOREGROUND_COLOR);
    EXPECT_FALSE(node.dirtySlots_.empty());

    node.UpdateDirtySlotsAndPendingNodes(RSDrawableSlot::SHADOW);
    EXPECT_FALSE(node.dirtySlots_.empty());

    node.UpdateDirtySlotsAndPendingNodes(RSDrawableSlot::BACKGROUND_COLOR);
    EXPECT_FALSE(node.dirtySlots_.empty());

    node.UpdateDirtySlotsAndPendingNodes(RSDrawableSlot::BACKGROUND_SHADER);
    EXPECT_FALSE(node.dirtySlots_.empty());

    node.UpdateDirtySlotsAndPendingNodes(RSDrawableSlot::BACKGROUND_IMAGE);
    EXPECT_FALSE(node.dirtySlots_.empty());

    node.UpdateDirtySlotsAndPendingNodes(RSDrawableSlot::BACKGROUND_FILTER);
    EXPECT_FALSE(node.dirtySlots_.empty());
}


/**
 * @tc.name: MarkFilterCacheFlags01
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest2, MarkFilterCacheFlags01, TestSize.Level1)
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
 * @tc.name: MarkFilterCacheFlags02
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest2, MarkFilterCacheFlags02, TestSize.Level1)
{
    RSRenderNode node(id, context);
    bool needRequestNextVsync = false;
    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    std::shared_ptr<DrawableV2::RSFilterDrawable> filterDrawable = std::make_shared<DrawableV2::RSFilterDrawable>();
    filterDrawable->stagingCacheManager_->stagingForceClearCache_ = true;
    filterDrawable->stagingCacheManager_->stagingForceUseCache_ = true;
    filterDrawable->stagingCacheManager_->stagingFilterInteractWithDirty_ = true;
    filterDrawable->stagingCacheManager_->cacheUpdateInterval_ = 1;
    auto& properties = node.GetMutableRenderProperties();
    properties.backgroundFilter_ = std::make_shared<RSFilter>();
    properties.filter_ = std::make_shared<RSFilter>();
    node.MarkFilterCacheFlags(filterDrawable, *rsDirtyManager, needRequestNextVsync);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: MarkFilterCacheFlags03
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest2, MarkFilterCacheFlags03, TestSize.Level1)
{
    RSRenderNode node(id, context);
    bool needRequestNextVsync = true;
    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    std::shared_ptr<DrawableV2::RSFilterDrawable> filterDrawable = std::make_shared<DrawableV2::RSFilterDrawable>();
    filterDrawable->stagingCacheManager_->stagingForceClearCache_ = false;
    filterDrawable->stagingCacheManager_->stagingForceUseCache_ = true;
    filterDrawable->stagingCacheManager_->pendingPurge_ = true;
    auto& properties = node.GetMutableRenderProperties();
    properties.backgroundFilter_ = std::make_shared<RSFilter>();
    properties.filter_ = std::make_shared<RSFilter>();
    node.MarkFilterCacheFlags(filterDrawable, *rsDirtyManager, needRequestNextVsync);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: MarkFilterCacheFlags04
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest2, MarkFilterCacheFlags04, TestSize.Level1)
{
    RSRenderNode node(id, context);
    bool needRequestNextVsync = false;
    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    std::shared_ptr<DrawableV2::RSFilterDrawable> filterDrawable = std::make_shared<DrawableV2::RSFilterDrawable>();
    filterDrawable->stagingCacheManager_->stagingForceClearCache_ = true;
    filterDrawable->stagingCacheManager_->stagingForceUseCache_ = false;
    filterDrawable->stagingCacheManager_->stagingFilterInteractWithDirty_ = true;
    filterDrawable->stagingCacheManager_->cacheUpdateInterval_ = 1;
    auto& properties = node.GetMutableRenderProperties();
    properties.backgroundFilter_ = std::make_shared<RSFilter>();
    properties.filter_ = std::make_shared<RSFilter>();
    node.MarkFilterCacheFlags(filterDrawable, *rsDirtyManager, needRequestNextVsync);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: MarkFilterCacheFlags05
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest2, MarkFilterCacheFlags05, TestSize.Level1)
{
    RSRenderNode node(id, context);
    bool needRequestNextVsync = false;
    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    std::shared_ptr<DrawableV2::RSFilterDrawable> filterDrawable = std::make_shared<DrawableV2::RSFilterDrawable>();
    filterDrawable->stagingCacheManager_->stagingForceClearCache_ = true;
    filterDrawable->stagingCacheManager_->stagingForceUseCache_ = true;
    filterDrawable->stagingCacheManager_->stagingFilterInteractWithDirty_ = false;
    filterDrawable->stagingCacheManager_->cacheUpdateInterval_ = 1;
    auto& properties = node.GetMutableRenderProperties();
    properties.backgroundFilter_ = std::make_shared<RSFilter>();
    properties.filter_ = std::make_shared<RSFilter>();
    node.MarkFilterCacheFlags(filterDrawable, *rsDirtyManager, needRequestNextVsync);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: MarkFilterCacheFlags06
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest2, MarkFilterCacheFlags06, TestSize.Level1)
{
    RSRenderNode node(id, context);
    bool needRequestNextVsync = false;
    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    std::shared_ptr<DrawableV2::RSFilterDrawable> filterDrawable = std::make_shared<DrawableV2::RSFilterDrawable>();
    filterDrawable->stagingCacheManager_->stagingForceClearCache_ = true;
    filterDrawable->stagingCacheManager_->stagingForceUseCache_ = false;
    filterDrawable->stagingCacheManager_->stagingFilterInteractWithDirty_ = false;
    filterDrawable->stagingCacheManager_->cacheUpdateInterval_ = 1;
    auto& properties = node.GetMutableRenderProperties();
    properties.backgroundFilter_ = std::make_shared<RSFilter>();
    properties.filter_ = std::make_shared<RSFilter>();
    node.MarkFilterCacheFlags(filterDrawable, *rsDirtyManager, needRequestNextVsync);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: MarkFilterCacheFlags07
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest2, MarkFilterCacheFlags07, TestSize.Level1)
{
    RSRenderNode node(id, context);
    bool needRequestNextVsync = true;
    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    std::shared_ptr<DrawableV2::RSFilterDrawable> filterDrawable = std::make_shared<DrawableV2::RSFilterDrawable>();
    filterDrawable->stagingCacheManager_->stagingForceClearCache_ = true;
    filterDrawable->stagingCacheManager_->stagingForceUseCache_ = true;
    filterDrawable->stagingCacheManager_->stagingFilterInteractWithDirty_ = false;
    filterDrawable->stagingCacheManager_->cacheUpdateInterval_ = 1;
    auto& properties = node.GetMutableRenderProperties();
    properties.backgroundFilter_ = std::make_shared<RSFilter>();
    properties.filter_ = std::make_shared<RSFilter>();
    node.MarkFilterCacheFlags(filterDrawable, *rsDirtyManager, needRequestNextVsync);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: MarkFilterCacheFlags08
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest2, MarkFilterCacheFlags08, TestSize.Level1)
{
    RSRenderNode node(id, context);
    bool needRequestNextVsync = true;
    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    std::shared_ptr<DrawableV2::RSFilterDrawable> filterDrawable = std::make_shared<DrawableV2::RSFilterDrawable>();
    filterDrawable->stagingCacheManager_->stagingForceClearCache_ = true;
    filterDrawable->stagingCacheManager_->stagingForceUseCache_ = false;
    filterDrawable->stagingCacheManager_->stagingFilterInteractWithDirty_ = false;
    filterDrawable->stagingCacheManager_->cacheUpdateInterval_ = 1;
    auto& properties = node.GetMutableRenderProperties();
    properties.backgroundFilter_ = std::make_shared<RSFilter>();
    properties.filter_ = std::make_shared<RSFilter>();
    node.MarkFilterCacheFlags(filterDrawable, *rsDirtyManager, needRequestNextVsync);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: InitCacheSurfaceTest02
 * @tc.desc: InitCacheSurface test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, InitCacheSurfaceTest02, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);
    nodeTest->boundsModifier_ = nullptr;
    nodeTest->frameModifier_ = nullptr;

    RSRenderNode::ClearCacheSurfaceFunc funcTest1 = nullptr;
    Drawing::GPUContext gpuContextTest1;
    nodeTest->cacheType_ = CacheType::ANIMATE_PROPERTY;
    RSShadow rsShadow;
    std::optional<RSShadow> shadow(rsShadow);
    nodeTest->renderProperties_.shadow_ = shadow;
    nodeTest->renderProperties_.shadow_->radius_ = 1.0f;
    nodeTest->renderProperties_.isSpherizeValid_ = true;
    nodeTest->renderProperties_.isAttractionValid_ = true;
    nodeTest->cacheSurface_ = nullptr;
    nodeTest->InitCacheSurface(&gpuContextTest1, funcTest1, 1);
    EXPECT_EQ(nodeTest->cacheSurface_, nullptr);

    nodeTest->cacheSurface_ = std::make_shared<Drawing::Surface>();
    EXPECT_NE(nodeTest->cacheSurface_, nullptr);
    nodeTest->cacheSurfaceThreadIndex_ = 1;
    nodeTest->completedSurfaceThreadIndex_ = 1;
    Drawing::GPUContext gpuContextTest2;
    nodeTest->cacheType_ = CacheType::CONTENT;
    std::shared_ptr<Drawing::Surface> surfaceTest1 = std::make_shared<Drawing::Surface>();
    EXPECT_NE(surfaceTest1, nullptr);
    std::shared_ptr<Drawing::Surface> surfaceTest2 = std::make_shared<Drawing::Surface>();
    EXPECT_NE(surfaceTest2, nullptr);
    RSRenderNode::ClearCacheSurfaceFunc funcTest2 = [surfaceTest1, surfaceTest2](std::shared_ptr<Drawing::Surface>&& s1,
                                                        std::shared_ptr<Drawing::Surface>&& s2, uint32_t w,
                                                        uint32_t h) {};
    nodeTest->InitCacheSurface(&gpuContextTest2, funcTest2, 1);
    EXPECT_EQ(nodeTest->cacheSurface_, nullptr);

    Drawing::GPUContext* gpuContextTest3 = nullptr;
    std::shared_ptr<Drawing::Surface> surfaceTest3 = std::make_shared<Drawing::Surface>();
    EXPECT_NE(surfaceTest3, nullptr);
    std::shared_ptr<Drawing::Surface> surfaceTest4 = std::make_shared<Drawing::Surface>();
    EXPECT_NE(surfaceTest4, nullptr);
    RSRenderNode::ClearCacheSurfaceFunc funcTest3 = [surfaceTest3, surfaceTest4](std::shared_ptr<Drawing::Surface>&& s1,
                                                        std::shared_ptr<Drawing::Surface>&& s2, uint32_t w,
                                                        uint32_t h) {};
    nodeTest->clearCacheSurfaceFunc_ = nullptr;
    nodeTest->cacheSurfaceThreadIndex_ = 1;
    nodeTest->completedSurfaceThreadIndex_ = 1;
    nodeTest->InitCacheSurface(gpuContextTest3, funcTest3, 1);
    EXPECT_EQ(nodeTest->cacheSurface_, nullptr);
}

/**
 * @tc.name: ResortChildrenTest02
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest2, ResortChildrenTest02, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(id, context);
    auto node1 = std::make_shared<RSRenderNode>(id + 1, context);
    auto node2 = std::make_shared<RSRenderNode>(id + 2, context);
    auto node3 = std::make_shared<RSRenderNode>(id + 3, context);
    auto node4 = std::make_shared<RSRenderNode>(id + 4, context);
    auto node5 = std::make_shared<RSRenderNode>(id + 5, context);
    auto node6 = std::make_shared<RSRenderNode>(id + 6, context);
    auto node7 = std::make_shared<RSRenderNode>(id + 7, context);
    auto node8 = std::make_shared<RSRenderNode>(id + 8, context);
    auto node9 = std::make_shared<RSRenderNode>(id + 9, context);
    auto node0 = std::make_shared<RSRenderNode>(id + 10, context);
    auto fullChildrenList = std::make_shared<std::vector<std::shared_ptr<RSRenderNode>>>();
    fullChildrenList->emplace_back(std::move(node1));
    fullChildrenList->emplace_back(std::move(node2));
    fullChildrenList->emplace_back(std::move(node3));
    fullChildrenList->emplace_back(std::move(node4));
    fullChildrenList->emplace_back(std::move(node5));
    fullChildrenList->emplace_back(std::move(node6));
    fullChildrenList->emplace_back(std::move(node7));
    fullChildrenList->emplace_back(std::move(node8));
    fullChildrenList->emplace_back(std::move(node9));
    fullChildrenList->emplace_back(std::move(node0));
    node->fullChildrenList_ = fullChildrenList;
    node->isChildrenSorted_ = false;
    EXPECT_TRUE(!node->fullChildrenList_->empty());
    node->ResortChildren();
    EXPECT_TRUE(node->isChildrenSorted_);
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

#if defined(MODIFIER_NG)
/**
 * @tc.name: DumpModifiersTest
 * @tc.desc: DumpModifiersTest test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeTest2, DumpModifiersTest, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);
    std::string outTest = "";
    nodeTest->DumpModifiers(outTest);
    EXPECT_EQ(outTest, "");
    auto modifier = std::make_shared<ModifierNG::RSAlphaRenderModifier>();
    auto property = std::make_shared<RSRenderProperty<float>>();
    modifier->AttachProperty(ModifierNG::RSPropertyType::ALPHA, property);
    nodeTest->modifiersNG_[static_cast<uint16_t>(ModifierNG::RSModifierType::ALPHA)].emplace_back(modifier);
    nodeTest->DumpModifiers(outTest);
    EXPECT_NE(outTest, "");
}

/**
 * @tc.name: ApplyPositionZModifierTest
 * @tc.desc: ApplyPositionZModifier test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeTest2, ApplyPositionZModifierTest, TestSize.Level1)
{
    constexpr auto transformModifierType = static_cast<uint16_t>(ModifierNG::RSModifierType::TRANSFORM);
    auto node = std::make_shared<RSRenderNode>(1);
    EXPECT_NE(node, nullptr);
    node->ApplyPositionZModifier();
    EXPECT_FALSE(node->dirtyTypesNG_.test(transformModifierType));
    node->dirtyTypesNG_.set(transformModifierType, true);
    node->ApplyPositionZModifier();
    EXPECT_TRUE(node->dirtyTypesNG_.test(transformModifierType));

    auto transformModifier = std::make_shared<ModifierNG::RSTransformRenderModifier>();
    node->AddModifier(transformModifier);
    node->ApplyPositionZModifier();
    EXPECT_FALSE(node->dirtyTypesNG_.test(transformModifierType));

    RSDisplayNodeConfig config;
    NodeId nodeId = 2;
    auto displayNode = std::make_shared<RSLogicalDisplayRenderNode>(nodeId, config);
    displayNode->AddModifier(transformModifier);
    displayNode->currentScbPid_ = 0;
    displayNode->ApplyPositionZModifier();
    EXPECT_FALSE(displayNode->dirtyTypesNG_.test(transformModifierType));
    displayNode->currentScbPid_ = 1;
    displayNode->dirtyTypesNG_.set(transformModifierType, true);
    displayNode->ApplyPositionZModifier();
    EXPECT_FALSE(displayNode->dirtyTypesNG_.test(transformModifierType));
}

/**
 * @tc.name: FilterModifiersByPidTest
 * @tc.desc: FilterModifiersByPid test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeTest2, FilterModifiersByPidTest, TestSize.Level1)
{
    constexpr auto transformModifierType = static_cast<uint16_t>(ModifierNG::RSModifierType::TRANSFORM);
    auto node = std::make_shared<RSRenderNode>(1);
    EXPECT_NE(node, nullptr);
    auto transformModifier = std::make_shared<ModifierNG::RSTransformRenderModifier>();
    node->AddModifier(transformModifier);
    node->FilterModifiersByPid(1);
    EXPECT_FALSE(node->modifiersNG_[transformModifierType].empty());
    node->FilterModifiersByPid(0);
    EXPECT_TRUE(node->modifiersNG_[transformModifierType].empty());
}
#endif
} // namespace Rosen
} // namespace OHOS