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

#include "modifier_ng/appearance/rs_alpha_render_modifier.h"
#include "modifier_ng/geometry/rs_transform_render_modifier.h"
#include "common/rs_obj_abs_geometry.h"
#include "dirty_region/rs_gpu_dirty_collector.h"
#include "drawable/rs_property_drawable_foreground.h"
#include "metadata_helper.h"
#include "offscreen_render/rs_offscreen_render_thread.h"
#include "params/rs_render_params.h"
#include "pipeline/rs_canvas_drawing_render_node.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_dirty_region_manager.h"
#include "pipeline/rs_logical_display_render_node.h"
#include "pipeline/rs_screen_render_node.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_union_render_node.h"
#include "feature/window_keyframe/rs_window_keyframe_render_node.h"
#include "render_thread/rs_render_thread_visitor.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "skia_adapter/skia_canvas.h"
#include "drawable/rs_property_drawable_background.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
const std::string OUT_STR3 =
    ", Parent [null], Name [SurfaceNode], hasConsumer: 0, Alpha: 1.000000, Visible: 1, VisibleRegion [Empty], "
    "OpaqueRegion [Empty], OcclusionBg: 0, IsContainerTransparent: 0, SpecialLayer: 0, surfaceType: 0, "
    "ContainerConfig: [outR: 0 inR: 0 x: 0 y: 0 w: 0 h: 0], colorSpace: 4, uifirstColorGamut: 4"
    ", isSurfaceBufferOpaque: 0";
const std::string OUT_STR4 = ", Visible: 1, Size: [-inf, -inf], EnableRender: 1";
const std::string OUT_STR5 = ", skipLayer: 0";
const Rect DEFAULT_RECT = { 0, 0, 200, 200 };
const RectF DEFAULT_SELF_DRAW_RECT = { 0, 0, 200, 200 };

const int DEFAULT_NODE_ID = 1;
const uint64_t BUFFER_USAGE_GPU_RENDER_DIRTY = BUFFER_USAGE_HW_RENDER | BUFFER_USAGE_AUXILLARY_BUFFER0;

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

    void CheckWithStatusLevel(const RSScreenRenderNode::HeadroomMap &map, HdrStatus status, uint32_t level);
    void CheckWithoutStatusLevel(const RSScreenRenderNode::HeadroomMap &map, HdrStatus status, uint32_t level);
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
    node.displaySync_ = std::make_unique<RSRenderDisplaySync>(id);
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
    node.displaySync_ = std::make_unique<RSRenderDisplaySync>(1);
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
    node.displaySync_ = std::make_unique<RSRenderDisplaySync>(1);
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
 * @tc.name: CollectAndUpdateRenderFitRect
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, CollectAndUpdateRenderFitRect, TestSize.Level1)
{
    RSRenderNode node(id, context);
    node.CollectAndUpdateRenderFitRect();
    ASSERT_TRUE(true);

    node.GetMutableRenderProperties().SetFrameGravity(Gravity::CENTER);
    node.CollectAndUpdateRenderFitRect();
    ASSERT_TRUE(true);

    std::shared_ptr<RectF> rect = std::make_shared<RectF>();
    rect->SetAll(0, 0, 1000, 1000);
    node.GetMutableRenderProperties().SetDrawRegion(rect);
    node.CollectAndUpdateRenderFitRect();
    ASSERT_TRUE(true);

    auto modifier = std::make_shared<ModifierNG::RSCustomRenderModifier<ModifierNG::RSModifierType::CONTENT_STYLE>>();
    auto property = std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>();
    modifier->AttachProperty(ModifierNG::RSPropertyType::CONTENT_STYLE, property);
    node.modifiersNG_[ModifierNG::RSModifierType::CONTENT_STYLE].emplace_back(modifier);

    auto modifierCmdList =
        std::make_shared<ModifierNG::RSCustomRenderModifier<ModifierNG::RSModifierType::CONTENT_STYLE>>();
    auto ptr = std::make_shared<Drawing::DrawCmdList>(1000, 1000);
    PropertyId id = 1;
    auto propertyCmdList = std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>(ptr, id);
    modifierCmdList->AttachProperty(ModifierNG::RSPropertyType::CONTENT_STYLE, propertyCmdList);
    node.modifiersNG_[ModifierNG::RSModifierType::CONTENT_STYLE].emplace_back(modifierCmdList);

    auto modifierAlpha = std::make_shared<ModifierNG::RSAlphaRenderModifier>();
    auto propertyAlpha = std::make_shared<RSRenderProperty<float>>();
    modifierAlpha->AttachProperty(ModifierNG::RSPropertyType::ALPHA, property);
    node.modifiersNG_[ModifierNG::RSModifierType::ALPHA].emplace_back(modifierAlpha);
    node.CollectAndUpdateRenderFitRect();
    ASSERT_TRUE(true);
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
    ASSERT_EQ(src, nullptr);

    surfaceNode->GetRSSurfaceHandler()->buffer_.buffer = buffer;
    ASSERT_TRUE(surfaceNode->GetRSSurfaceHandler()->GetBuffer() != nullptr);
    surfaceNode->GetRSSurfaceHandler()->buffer_.damageRect = DEFAULT_RECT;
    surfaceNode->selfDrawRect_ = DEFAULT_SELF_DRAW_RECT;
    auto param = system::GetParameter("rosen.graphic.selfdrawingdirtyregion.enabled", "");
    system::SetParameter("rosen.graphic.selfdrawingdirtyregion.enabled", "1");
    surfaceNode->UpdateBufferDirtyRegion();
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
}

/**
 * @tc.name: UpdateBufferDirtyRegion005
 * @tc.desc: test UpdateBufferDirtyRegion when buffer size changed
 * @tc.type: FUNC
 * @tc.require: issue21097
 */
HWTEST_F(RSRenderNodeTest2, UpdateBufferDirtyRegion005, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(0);
    ASSERT_TRUE(surfaceNode->GetRSSurfaceHandler() != nullptr);
    auto buffer = SurfaceBuffer::Create();
    auto ret = buffer->Alloc(requestConfig);
    ASSERT_EQ(ret, GSERROR_OK);

    surfaceNode->GetRSSurfaceHandler()->buffer_.buffer = buffer;
    ASSERT_TRUE(surfaceNode->GetRSSurfaceHandler()->GetBuffer() != nullptr);
    surfaceNode->GetRSSurfaceHandler()->bufferSizeChanged_ = true;
    surfaceNode->selfDrawRect_ = DEFAULT_SELF_DRAW_RECT;
    surfaceNode->UpdateBufferDirtyRegion();
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
 * @tc.name: SetHDRUIBrightness
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, SetHDRUIBrightness, TestSize.Level1)
{
    RSRenderNode node(id, context);
    constexpr uint32_t INCORRECT_HEADROOM = static_cast<uint32_t>(-1);
    node.SetHdrUIComponentHeadroom(INCORRECT_HEADROOM);
    node.SetHDRUIBrightness(1.0F);
    ASSERT_NE(node.headroomInfo_, nullptr);
    ASSERT_NE(node.GetHdrUIComponentHeadroom(), INCORRECT_HEADROOM);
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
 * @tc.name: IsFilterCacheValid002
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
    EXPECT_FALSE(node.IsFilterCacheValid());
    properties.GetEffect().materialFilter_ = std::make_shared<RSFilter>();
    auto filterDrawable = std::make_shared<DrawableV2::RSMaterialFilterDrawable>();
    node.GetDrawableVec(__func__)[static_cast<int8_t>(RSDrawableSlot::MATERIAL_FILTER)] = filterDrawable;
    EXPECT_FALSE(node.IsFilterCacheValid());
    filterDrawable->stagingCacheManager_ = std::make_unique<RSFilterCacheManager>();
    filterDrawable->stagingCacheManager_->isFilterCacheValid_ = true;
    EXPECT_FALSE(node.IsFilterCacheValid());
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
 * @tc.name: ForceReduceAIBarCacheInterval
 * @tc.desc: test function RSRenderNode::ForceReduceAIBarCacheInterval
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeTest2, ForceReduceAIBarCacheInterval, TestSize.Level1)
{
    RSRenderNode node(id, context);
#ifdef RS_ENABLE_GPU
    RSProperties::filterCacheEnabled_ = false;
    EXPECT_FALSE(node.ForceReduceAIBarCacheInterval(true));

    RSProperties::filterCacheEnabled_ = true;
    EXPECT_FALSE(node.ForceReduceAIBarCacheInterval(true));

    RSDrawableSlot slot = RSDrawableSlot::BACKGROUND_FILTER;
    auto drawable = std::make_shared<DrawableV2::RSFilterDrawable>();
    node.GetDrawableVec(__func__)[static_cast<uint32_t>(slot)] = drawable;
    EXPECT_FALSE(node.ForceReduceAIBarCacheInterval(true));
#else
    EXPECT_FALSE(node.ForceReduceAIBarCacheInterval(true));
#endif
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
    node.UpdateFilterRegionInSkippedSubTree(*rsDirtyManager, subTreeRoot, filterRect, clipRect, clipRect);
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
    node.UpdateFilterRegionInSkippedSubTree(*rsDirtyManager, subTreeRoot, filterRect, clipRect, clipRect);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: FilterRectMergeDirtyRectInSkippedSubtree001
 * @tc.desc: test FilterRectMergeDirtyRectInSkippedSubtree last filter region is equal to filterRect
 * @tc.type: FUNC
 * @tc.require: issue21301
 */
HWTEST_F(RSRenderNodeTest2, FilterRectMergeDirtyRectInSkippedSubtree001, TestSize.Level1)
{
    RSRenderNode node(id, context);
    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    RectI filterRect{0, 0, 1000, 1000};
    node.lastFilterRegion_ = filterRect;
    node.FilterRectMergeDirtyRectInSkippedSubtree(*rsDirtyManager, filterRect);
    EXPECT_FALSE(node.isDirtyRegionUpdated_);
    EXPECT_TRUE(rsDirtyManager->GetCurrentFrameDirtyRegion().IsEmpty());
}

/**
 * @tc.name: FilterRectMergeDirtyRectInSkippedSubtree002
 * @tc.desc: test FilterRectMergeDirtyRectInSkippedSubtree last filter region is not equal to filterRect
 * @tc.type: FUNC
 * @tc.require: issue21301
 */
HWTEST_F(RSRenderNodeTest2, FilterRectMergeDirtyRectInSkippedSubtree002, TestSize.Level1)
{
    RSRenderNode node(id, context);
    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    RectI filterRect{1000, 1000, 1500, 1500};
    node.lastFilterRegion_ = RectI(0, 0, 500, 500);
    node.FilterRectMergeDirtyRectInSkippedSubtree(*rsDirtyManager, filterRect);
    EXPECT_TRUE(node.isDirtyRegionUpdated_);
    EXPECT_EQ(rsDirtyManager->GetCurrentFrameDirtyRegion(), filterRect);
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
    std::shared_ptr<DrawableV2::RSFilterDrawable> nullDrawable = nullptr;
    node.MarkFilterStatusChanged(nullDrawable, isForeground, isFilterRegionChanged);
    std::shared_ptr<DrawableV2::RSFilterDrawable> filterDrawable =
        std::make_shared<DrawableV2::RSBackgroundFilterDrawable>();
    node.MarkFilterStatusChanged(filterDrawable, isForeground, isFilterRegionChanged);
    isFilterRegionChanged = false;
    node.MarkFilterStatusChanged(filterDrawable, isForeground, isFilterRegionChanged);
    isForeground = false;
    node.MarkFilterStatusChanged(filterDrawable, isForeground, isFilterRegionChanged);
    isForeground = true;
    filterDrawable = std::make_shared<DrawableV2::RSCompositingFilterDrawable>();
    node.MarkFilterStatusChanged(filterDrawable, isForeground, isFilterRegionChanged);
    EXPECT_EQ(filterDrawable->stagingCacheManager_->stagingForceClearCache_, false);
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
 * @tc.name: UpdateFilterCacheWithBelowDirty001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest2, UpdateFilterCacheWithBelowDirty001, TestSize.Level1)
{
    RSRenderNode node(id, context);
    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    node.UpdateFilterCacheWithBelowDirty(Occlusion::Rect(rsDirtyManager->GetCurrentFrameDirtyRegion()),
        RSDrawableSlot::COMPOSITING_FILTER);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: UpdateFilterCacheWithBelowDirty002
 * @tc.desc: test UpdateFilterCacheWithBelowDirty
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeTest2, UpdateFilterCacheWithBelowDirty002, TestSize.Level1)
{
    RSRenderNode node(id, context);
    auto backgroundFilterDrawable = std::make_shared<DrawableV2::RSBackgroundFilterDrawable>();
    node.GetDrawableVec(__func__)[static_cast<uint32_t>(RSDrawableSlot::BACKGROUND_FILTER)] =
        backgroundFilterDrawable;
    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    rsDirtyManager->MergeDirtyRect(RectI(0, 0, 10, 10));
    backgroundFilterDrawable->lastStagingVisibleSnapshotRect_ = std::make_unique<RectI>(20, 20, 100, 100);
    node.UpdateFilterCacheWithBelowDirty(Occlusion::Rect(rsDirtyManager->GetCurrentFrameDirtyRegion()),
        RSDrawableSlot::BACKGROUND_FILTER);
    EXPECT_FALSE(node.backgroundFilterInteractWithDirty_);
    rsDirtyManager->MergeDirtyRect(RectI(20, 20, 100, 100));
    node.UpdateFilterCacheWithBelowDirty(Occlusion::Rect(rsDirtyManager->GetCurrentFrameDirtyRegion()),
        RSDrawableSlot::BACKGROUND_FILTER);
    EXPECT_TRUE(node.backgroundFilterInteractWithDirty_);
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
    properties.GetEffect().materialFilter_ = std::make_shared<RSFilter>();
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
    properties.GetEffect().needDrawBehindWindow_ = true;
    RectI inRegion(10, 10, 20, 20);
    RectI outRegion(90, 90, 110, 110);
    RectI lastRegion(0, 0, 100, 100);
    node.GetFilterRegionInfo().filterRegion_ = inRegion;
    node.GetFilterRegionInfo().defaultFilterRegion_ = inRegion;
    node.lastFilterRegion_ = lastRegion;
    node.UpdateFilterCacheWithSelfDirty();
    node.GetFilterRegionInfo().filterRegion_ = outRegion;
    node.GetFilterRegionInfo().defaultFilterRegion_ = outRegion;
    node.UpdateFilterCacheWithSelfDirty();
    ASSERT_TRUE(true);
}

/**
 * @tc.name: UpdateFilterCacheWithSelfDirty003
 * @tc.desc: test UpdateFilterCacheWithSelfDirty
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeTest2, UpdateFilterCacheWithSelfDirty003, TestSize.Level1)
{
    RSRenderNode node(id, context);
    auto filterDrawable = std::make_shared<DrawableV2::RSFilterDrawable>();
    filterDrawable->stagingCacheManager_ = std::make_unique<RSFilterCacheManager>();
    node.GetDrawableVec(__func__)[static_cast<uint32_t>(RSDrawableSlot::BACKGROUND_FILTER)] =
        filterDrawable;
    node.GetDrawableVec(__func__)[static_cast<uint32_t>(RSDrawableSlot::COMPOSITING_FILTER)] =
        filterDrawable;

    auto& properties = node.GetMutableRenderProperties();
    properties.backgroundFilter_ = std::make_shared<RSFilter>();
    properties.filter_ = std::make_shared<RSFilter>();

    RectI snapshotRect(20, 20, 100, 100);
    RectI lastSnapshotRect(20, 20, 30, 30);
    filterDrawable->lastStagingVisibleSnapshotRect_ = std::make_unique<RectI>(20, 20, 100, 100);
    filterDrawable->stagingVisibleRectInfo_ = std::make_unique<DrawableV2::RSFilterDrawable::FilterVisibleRectInfo>();
    filterDrawable->stagingVisibleRectInfo_->snapshotRect_ = snapshotRect;

    node.UpdateFilterCacheWithSelfDirty();
    EXPECT_FALSE(filterDrawable->stagingCacheManager_->stagingFilterRegionChanged_);

    filterDrawable->lastStagingVisibleSnapshotRect_ = std::make_unique<RectI>(20, 20, 30, 30);
    node.UpdateFilterCacheWithSelfDirty();
    EXPECT_TRUE(filterDrawable->stagingCacheManager_->stagingFilterRegionChanged_);
}

/**
 * @tc.name: UpdatePendingPurgeFilterDirtyRect001
 * @tc.desc: test pendingPurgeFilterRegion not intersect with node filter rect and not pendingPurge
 * @tc.type: FUNC
 * @tc.require: issue20473
 */
HWTEST_F(RSRenderNodeTest2, UpdatePendingPurgeFilterDirtyRect001, TestSize.Level1)
{
    RSRenderNode node(id, context);
    auto& geoPtr = node.renderProperties_.boundsGeo_;
    geoPtr = std::make_shared<RSObjAbsGeometry>();
    RectI rect(50, 50, 100, 100);
    geoPtr->absRect_ = rect;

    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    rsDirtyManager->GetFilterCollector().AddPendingPurgeFilterRegion(Occlusion::Region(Occlusion::Rect(0, 0, 10, 10)));
    node.UpdatePendingPurgeFilterDirtyRect(*rsDirtyManager, RSDrawableSlot::BACKGROUND_FILTER);

    EXPECT_FALSE(node.backgroundFilterInteractWithDirty_);
    const auto& pendingPurgeFilterRegion = rsDirtyManager->GetFilterCollector().GetPendingPurgeFilterRegion();
    auto filterRegion = Occlusion::Region(node.GetFilterRect());
    EXPECT_FALSE(filterRegion.Sub(pendingPurgeFilterRegion).IsEmpty());
}

/**
 * @tc.name: UpdatePendingPurgeFilterDirtyRect002
 * @tc.desc: test pendingPurgeFilterRegion intersect with node filter rect and pendingPurge
 * @tc.type: FUNC
 * @tc.require: issue20473
 */
HWTEST_F(RSRenderNodeTest2, UpdatePendingPurgeFilterDirtyRect002, TestSize.Level1)
{
    RSRenderNode node(id, context);
    auto& geoPtr = node.renderProperties_.boundsGeo_;
    geoPtr = std::make_shared<RSObjAbsGeometry>();
    RectI rect(50, 50, 100, 100);
    geoPtr->absRect_ = rect;
    node.GetFilterRegionInfo().filterRegion_ = node.GetFilterRect();
    node.GetFilterRegionInfo().defaultFilterRegion_ = node.GetFilterRect();
    node.lastFilterRegion_ = node.GetFilterRect();
    RSDrawableSlot slot = RSDrawableSlot::BACKGROUND_FILTER;
    auto filterDrawable = std::make_shared<DrawableV2::RSFilterDrawable>();
    node.GetDrawableVec(__func__)[static_cast<uint32_t>(slot)] = filterDrawable;
    filterDrawable->stagingCacheManager_ = std::make_unique<RSFilterCacheManager>();
    filterDrawable->stagingCacheManager_->pendingPurge_ = true;
    filterDrawable->stagingCacheManager_->stagingFilterInteractWithDirty_ = false;

    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    rsDirtyManager->GetFilterCollector().AddPendingPurgeFilterRegion(Occlusion::Region(Occlusion::Rect(0, 0, 60, 60)));
    node.UpdatePendingPurgeFilterDirtyRect(*rsDirtyManager, RSDrawableSlot::BACKGROUND_FILTER);

    EXPECT_TRUE(node.backgroundFilterInteractWithDirty_);
    const auto& pendingPurgeFilterRegion = rsDirtyManager->GetFilterCollector().GetPendingPurgeFilterRegion();
    auto filterRegion = Occlusion::Region(node.GetFilterRect());
    EXPECT_TRUE(filterRegion.Sub(pendingPurgeFilterRegion).IsEmpty());
}

/**
 * @tc.name: UpdatePendingPurgeFilterDirtyRect003
 * @tc.desc: test pendingPurgeFilterRegion
 * @tc.type: FUNC
 * @tc.require: issue20551
 */
HWTEST_F(RSRenderNodeTest2, UpdatePendingPurgeFilterDirtyRect003, TestSize.Level1)
{
    RSRenderNode node(id, context);
    auto& geoPtr = node.renderProperties_.boundsGeo_;
    geoPtr = std::make_shared<RSObjAbsGeometry>();
    RectI rect(50, 50, 100, 100);
    geoPtr->absRect_ = rect;

    RSDrawableSlot slot = RSDrawableSlot::BACKGROUND_FILTER;
    auto filterDrawable = std::make_shared<DrawableV2::RSFilterDrawable>();
    node.GetDrawableVec(__func__)[static_cast<uint32_t>(slot)] = filterDrawable;
    slot = RSDrawableSlot::COMPOSITING_FILTER;
    node.GetDrawableVec(__func__)[static_cast<uint32_t>(slot)] = filterDrawable;
    filterDrawable->stagingCacheManager_ = std::make_unique<RSFilterCacheManager>();
    filterDrawable->stagingCacheManager_->stagingForceUseCache_ = true;

    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    rsDirtyManager->GetFilterCollector().AddPendingPurgeFilterRegion(Occlusion::Region(Occlusion::Rect(0, 0, 60, 60)));
    node.UpdatePendingPurgeFilterDirtyRect(*rsDirtyManager, RSDrawableSlot::BACKGROUND_FILTER);
    EXPECT_FALSE(node.backgroundFilterInteractWithDirty_);

    filterDrawable->stagingCacheManager_->stagingForceUseCache_ = false;
    node.UpdatePendingPurgeFilterDirtyRect(*rsDirtyManager, RSDrawableSlot::COMPOSITING_FILTER);
    EXPECT_FALSE(node.backgroundFilterInteractWithDirty_);

    rsDirtyManager->GetFilterCollector().ClearPendingPurgeFilterRegion();
    rsDirtyManager->GetFilterCollector().AddPendingPurgeFilterRegion(Occlusion::Region(Occlusion::Rect(0, 0, 10, 10)));
    node.UpdatePendingPurgeFilterDirtyRect(*rsDirtyManager, RSDrawableSlot::BACKGROUND_FILTER);
    EXPECT_FALSE(node.backgroundFilterInteractWithDirty_);
    const auto& pendingPurgeFilterRegion = rsDirtyManager->GetFilterCollector().GetPendingPurgeFilterRegion();
    auto filterRegion = Occlusion::Region(node.GetFilterRect());
    EXPECT_FALSE(filterRegion.Sub(pendingPurgeFilterRegion).IsEmpty());
}

/**
 * @tc.name: UpdatePendingPurgeFilterDirtyRect005
 * @tc.desc: test pendingPurgeFilterRegion
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeTest2, UpdatePendingPurgeFilterDirtyRect005, TestSize.Level1)
{
    RSRenderNode node(id, context);
    RSDrawableSlot slot = RSDrawableSlot::BACKGROUND_FILTER;
    auto filterDrawable = std::make_shared<DrawableV2::RSFilterDrawable>();
    node.GetDrawableVec(__func__)[static_cast<uint32_t>(slot)] = filterDrawable;
    filterDrawable->stagingCacheManager_ = std::make_unique<RSFilterCacheManager>();
    filterDrawable->stagingCacheManager_->pendingPurge_ = true;
    filterDrawable->stagingCacheManager_->stagingFilterInteractWithDirty_ = false;
    node.lastFilterRegion_ = RectI(80, 80, 100, 100);

    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    rsDirtyManager->GetFilterCollector().AddPendingPurgeFilterRegion(Occlusion::Region(Occlusion::Rect(0, 0, 60, 60)));
    node.UpdatePendingPurgeFilterDirtyRect(*rsDirtyManager, slot);

    EXPECT_FALSE(node.backgroundFilterInteractWithDirty_);
}

/**
 * @tc.name: UpdatePendingPurgeFilterDirtyRect006
 * @tc.desc: test pendingPurgeFilterRegion
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeTest2, UpdatePendingPurgeFilterDirtyRect006, TestSize.Level1)
{
    RSRenderNode node(id, context);
    auto& geoPtr = node.renderProperties_.boundsGeo_;
    geoPtr = std::make_shared<RSObjAbsGeometry>();
    RectI rect(50, 50, 100, 100);
    geoPtr->absRect_ = rect;

    RSDrawableSlot slot = RSDrawableSlot::BACKGROUND_FILTER;
    auto filterDrawable = std::make_shared<DrawableV2::RSFilterDrawable>();
    node.GetDrawableVec(__func__)[static_cast<uint32_t>(slot)] = filterDrawable;
    filterDrawable->stagingCacheManager_ = std::make_unique<RSFilterCacheManager>();
    filterDrawable->stagingCacheManager_->pendingPurge_ = false;
    filterDrawable->stagingCacheManager_->stagingFilterInteractWithDirty_ = false;

    auto filterRegion = Occlusion::Region(node.GetFilterRect());
    node.lastFilterRegion_ = node.GetFilterRect();
    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();

    filterDrawable->stagingCacheManager_->lastHpaeClearCache_ = false;
    node.UpdatePendingPurgeFilterDirtyRect(*rsDirtyManager, RSDrawableSlot::BACKGROUND_FILTER);
    const auto& pendingPurgeFilterRegion = rsDirtyManager->GetFilterCollector().GetPendingPurgeFilterRegion();
    EXPECT_FALSE(filterRegion.Sub(pendingPurgeFilterRegion).IsEmpty());

    filterDrawable->stagingCacheManager_->lastHpaeClearCache_ = true;
    node.UpdatePendingPurgeFilterDirtyRect(*rsDirtyManager, RSDrawableSlot::BACKGROUND_FILTER);
    const auto& pendingPurgeFilterRegion2 = rsDirtyManager->GetFilterCollector().GetPendingPurgeFilterRegion();
    EXPECT_TRUE(filterRegion.Sub(pendingPurgeFilterRegion2).IsEmpty());

    filterDrawable->stagingCacheManager_->filterType_ = RSFilter::AIBAR;
    node.UpdatePendingPurgeFilterDirtyRect(*rsDirtyManager, RSDrawableSlot::BACKGROUND_FILTER);
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
    properties.GetEffect().needDrawBehindWindow_ = true;
    node.PostPrepareForBlurFilterNode(*rsDirtyManager, needRequestNextVsync);
    RSDrawableSlot slot = RSDrawableSlot::BACKGROUND_FILTER;
    node.GetDrawableVec(__func__)[static_cast<uint32_t>(slot)] = std::make_shared<DrawableV2::RSFilterDrawable>();
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
    auto modifier = std::make_shared<ModifierNG::RSCustomRenderModifier<ModifierNG::RSModifierType::CONTENT_STYLE>>();
    auto property = std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>();
    modifier->AttachProperty(ModifierNG::RSPropertyType::CONTENT_STYLE, property);
    nodeTest->modifiersNG_[ModifierNG::RSModifierType::CONTENT_STYLE].emplace_back(modifier);
    auto modifier2 = std::make_shared<ModifierNG::RSAlphaRenderModifier>();
    auto property2 = std::make_shared<RSRenderProperty<float>>();
    modifier2->AttachProperty(ModifierNG::RSPropertyType::ALPHA, property2);
    nodeTest->modifiersNG_[ModifierNG::RSModifierType::ALPHA].emplace_back(modifier2);
    nodeTest->DumpDrawCmdModifiers(outTest6);
    EXPECT_NE(outTest6, "");

    std::string outTest7;
    auto canvasDrawingNode = std::make_shared<RSCanvasDrawingRenderNode>(1);
    canvasDrawingNode->DumpSubClassNode(outTest7);
    EXPECT_EQ(outTest7, ", lastResetSurfaceTime: 0, opCountAfterReset: 0");
}

/**
 * @tc.name: RSWindowkeyFrameRenderNodeDumpTest
 * @tc.desc: test DumpSubClassNode for RSWindowkeyFrameRenderNode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeTest2, RSWindowkeyFrameRenderNodeDumpTest, TestSize.Level1)
{
    std::string outTest = "";
    auto node = std::make_shared<RSWindowKeyFrameRenderNode>(0);
    node->SetLinkedNodeId(1);
    node->DumpSubClassNode(outTest);
    EXPECT_EQ(outTest, ", linkedNodeId: 1");
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
    nodeTest->InitRenderParams();

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

    node.renderProperties_.SetDistortionK(0.5f); // 0.5 is k of value in distortion
    EXPECT_TRUE(node.renderProperties_.GetDistortionDirty());
    node.CollectAndUpdateLocalDistortionEffectRect();
    EXPECT_FALSE(node.renderProperties_.GetDistortionDirty());
}

/**
 * @tc.name: CollectAndUpdateLocalMagnifierEffectRectTest
 * @tc.desc: CollectAndUpdateLocalMagnifierEffectRect
 * @tc.type: FUNC
 * @tc.require: issue20188
 */
HWTEST_F(RSRenderNodeTest2, CollectAndUpdateLocalMagnifierEffectRectTest, TestSize.Level1)
{
    RSRenderNode node(id, context);
    node.CollectAndUpdateLocalMagnifierEffectRect();
    EXPECT_FALSE(node.renderProperties_.GetMagnifierDirty());

    std::shared_ptr<RSMagnifierParams> para = std::make_shared<RSMagnifierParams>();
    para->offsetX_ = 50.f; // set offset 50 of X
    para->offsetY_ = 100.f; // set offset 100 of Y
    para->factor_ = 0.5f; // set factor 0.5
    node.renderProperties_.SetMagnifierParams(para);
    node.CollectAndUpdateLocalMagnifierEffectRect();
    EXPECT_TRUE(node.renderProperties_.GetMagnifierDirty());
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
 * @tc.name: HDRStatusTest
 * @tc.desc: Test function GetHDRStatus UpdateHDRStatus ClearHDRVideoStatus
 * @tc.type: FUNC
 * @tc.require: issueI9US6V
 */
HWTEST_F(RSRenderNodeTest2, HDRStatusTest, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    EXPECT_NE(rsContext, nullptr);
    auto node = std::make_shared<RSRenderNode>(0, rsContext);
    std::unique_ptr<RSRenderParams> stagingRenderParams = std::make_unique<RSRenderParams>(0);
    EXPECT_NE(stagingRenderParams, nullptr);
    node->stagingRenderParams_ = std::move(stagingRenderParams);
    EXPECT_EQ(node->GetHDRStatus(), HdrStatus::NO_HDR);
    node->UpdateHDRStatus(HdrStatus::HDR_PHOTO, true);
    EXPECT_EQ(node->GetHDRStatus(), HdrStatus::HDR_PHOTO);
    node->UpdateHDRStatus(HdrStatus::HDR_PHOTO, false);
    EXPECT_EQ(node->GetHDRStatus(), HdrStatus::NO_HDR);
    node->UpdateHDRStatus(HdrStatus::HDR_VIDEO, true);
    node->ClearHDRVideoStatus();
    EXPECT_EQ(node->GetHDRStatus(), HdrStatus::NO_HDR);
}

/**
 * @tc.name: SetHdrNum001
 * @tc.desc: SetHdrNum test
 * @tc.type: FUNC
 * @tc.require: issueI9US6V
 */
HWTEST_F(RSRenderNodeTest2, SetHdrNum001, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    EXPECT_NE(rsContext, nullptr);
    auto node = std::make_shared<RSRenderNode>(0, rsContext);
    EXPECT_NE(node, nullptr);
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(1);
    EXPECT_NE(surfaceNode, nullptr);
    rsContext->nodeMap.renderNodeMap_[ExtractPid(1)][1] = surfaceNode;
    NodeId screenNodeId = 2UL;
    node->SetHdrNum(true, 1, screenNodeId, HDRComponentType::IMAGE);
    EXPECT_EQ(surfaceNode->hdrPhotoNum_, 1);
    node->SetHdrNum(true, 1, screenNodeId, HDRComponentType::UICOMPONENT);
    EXPECT_EQ(surfaceNode->hdrUIComponentNum_, 1);
    node->SetHdrNum(true, 1, screenNodeId, HDRComponentType::EFFECT);
    EXPECT_EQ(surfaceNode->hdrEffectNum_, 1);
    node->SetHdrNum(false, 1, screenNodeId, HDRComponentType::IMAGE);
    EXPECT_EQ(surfaceNode->hdrPhotoNum_, 0);
    node->SetHdrNum(false, 1, screenNodeId, HDRComponentType::UICOMPONENT);
    EXPECT_EQ(surfaceNode->hdrUIComponentNum_, 0);
    node->SetHdrNum(false, 1, screenNodeId, HDRComponentType::EFFECT);
    EXPECT_EQ(surfaceNode->hdrEffectNum_, 0);
}

void RSRenderNodeTest2::CheckWithStatusLevel(const RSScreenRenderNode::HeadroomMap &map,
    HdrStatus status, uint32_t level)
{
    ASSERT_EQ(map.count(status), 1U);
    ASSERT_EQ(map.at(status).count(level), 1U);
    ASSERT_EQ(map.at(status).at(level), 1U);
}

void RSRenderNodeTest2::CheckWithoutStatusLevel(const RSScreenRenderNode::HeadroomMap &map,
    HdrStatus status, uint32_t level)
{
    if (map.count(status) > 0U) {
        const auto &statusMap = map.at(status);
        if (statusMap.count(level) > 0U) {
            ASSERT_EQ(statusMap.at(level), 0U);
        }
    }
}

/**
 * @tc.name: SetHdrNum002
 * @tc.desc: SetHdrNum test
 * @tc.type: FUNC
 * @tc.require: issueI9US6V
 */
HWTEST_F(RSRenderNodeTest2, SetHdrNum002, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    EXPECT_NE(rsContext, nullptr);
    auto node = std::make_shared<RSRenderNode>(0, rsContext);
    EXPECT_NE(node, nullptr);
    NodeId surfaceNodeId = 1UL;
    NodeId screenNodeId = 2UL;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceNodeId);
    EXPECT_NE(surfaceNode, nullptr);
    rsContext->nodeMap.RegisterRenderNode(surfaceNode);
    auto screenNode = std::make_shared<RSScreenRenderNode>(screenNodeId, 0U, rsContext);
    EXPECT_NE(screenNode, nullptr);
    rsContext->nodeMap.RegisterRenderNode(screenNode);

    node->SetHdrPhotoHeadroom(RSRenderNode::DEFAULT_HEADROOM_VALUE);
    node->SetHdrUIComponentHeadroom(RSRenderNode::DEFAULT_HEADROOM_VALUE);
    node->SetEnableHdrEffect(RSRenderNode::DEFAULT_HEADROOM_VALUE);
    constexpr HDRComponentType INCORRECT_HDR_COMPONENT_TYPE = static_cast<HDRComponentType>(-1);
    node->SetHdrNum(true, surfaceNodeId, screenNodeId, INCORRECT_HDR_COMPONENT_TYPE);
    node->SetHdrNum(false, surfaceNodeId, screenNodeId, INCORRECT_HDR_COMPONENT_TYPE);
    const auto &map = screenNode->GetHeadroomMap();
    CheckWithoutStatusLevel(map, HdrStatus::HDR_PHOTO, RSRenderNode::DEFAULT_HEADROOM_VALUE);
    CheckWithoutStatusLevel(map, HdrStatus::HDR_VIDEO, RSRenderNode::DEFAULT_HEADROOM_VALUE);
    CheckWithoutStatusLevel(map, HdrStatus::AI_HDR_VIDEO_GTM, RSRenderNode::DEFAULT_HEADROOM_VALUE);
    CheckWithoutStatusLevel(map, HdrStatus::HDR_EFFECT, RSRenderNode::DEFAULT_HEADROOM_VALUE);
    CheckWithoutStatusLevel(map, HdrStatus::AI_HDR_VIDEO_GAINMAP, RSRenderNode::DEFAULT_HEADROOM_VALUE);
    CheckWithoutStatusLevel(map, HdrStatus::HDR_UICOMPONENT, RSRenderNode::DEFAULT_HEADROOM_VALUE);
}

/**
 * @tc.name: SetHdrNum003
 * @tc.desc: SetHdrNum test
 * @tc.type: FUNC
 * @tc.require: issueI9US6V
 */
HWTEST_F(RSRenderNodeTest2, SetHdrNum003, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    EXPECT_NE(rsContext, nullptr);
    auto node = std::make_shared<RSRenderNode>(0, rsContext);
    EXPECT_NE(node, nullptr);
    NodeId surfaceNodeId = 1UL;
    NodeId screenNodeId = 2UL;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceNodeId);
    EXPECT_NE(surfaceNode, nullptr);
    rsContext->nodeMap.RegisterRenderNode(surfaceNode);
    auto screenNode = std::make_shared<RSScreenRenderNode>(screenNodeId, 0U, rsContext);
    EXPECT_NE(screenNode, nullptr);
    rsContext->nodeMap.RegisterRenderNode(screenNode);

    node->SetHdrPhotoHeadroom(RSRenderNode::DEFAULT_HEADROOM_VALUE);
    node->SetHdrUIComponentHeadroom(RSRenderNode::DEFAULT_HEADROOM_VALUE);
    node->SetEnableHdrEffect(RSRenderNode::DEFAULT_HEADROOM_VALUE);
    const auto &map = screenNode->GetHeadroomMap();
    node->SetHdrNum(true, surfaceNodeId, screenNodeId, HDRComponentType::IMAGE);
    CheckWithStatusLevel(map, HdrStatus::HDR_PHOTO, RSRenderNode::DEFAULT_HEADROOM_VALUE);
    CheckWithoutStatusLevel(map, HdrStatus::HDR_UICOMPONENT, RSRenderNode::DEFAULT_HEADROOM_VALUE);
    CheckWithoutStatusLevel(map, HdrStatus::HDR_EFFECT, RSRenderNode::DEFAULT_HEADROOM_VALUE);
    node->SetHdrNum(false, surfaceNodeId, screenNodeId, HDRComponentType::IMAGE);
    CheckWithoutStatusLevel(map, HdrStatus::HDR_PHOTO, RSRenderNode::DEFAULT_HEADROOM_VALUE);
    CheckWithoutStatusLevel(map, HdrStatus::HDR_UICOMPONENT, RSRenderNode::DEFAULT_HEADROOM_VALUE);
    CheckWithoutStatusLevel(map, HdrStatus::HDR_EFFECT, RSRenderNode::DEFAULT_HEADROOM_VALUE);
}

/**
 * @tc.name: SetHdrNum004
 * @tc.desc: SetHdrNum test
 * @tc.type: FUNC
 * @tc.require: issueI9US6V
 */
HWTEST_F(RSRenderNodeTest2, SetHdrNum004, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    EXPECT_NE(rsContext, nullptr);
    auto node = std::make_shared<RSRenderNode>(0, rsContext);
    EXPECT_NE(node, nullptr);
    NodeId surfaceNodeId = 1UL;
    NodeId screenNodeId = 2UL;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceNodeId);
    EXPECT_NE(surfaceNode, nullptr);
    rsContext->nodeMap.RegisterRenderNode(surfaceNode);
    auto screenNode = std::make_shared<RSScreenRenderNode>(screenNodeId, 0U, rsContext);
    EXPECT_NE(screenNode, nullptr);
    rsContext->nodeMap.RegisterRenderNode(screenNode);

    node->SetHdrPhotoHeadroom(RSRenderNode::DEFAULT_HEADROOM_VALUE);
    node->SetHdrUIComponentHeadroom(RSRenderNode::DEFAULT_HEADROOM_VALUE);
    node->SetEnableHdrEffect(RSRenderNode::DEFAULT_HEADROOM_VALUE);
    const auto &map = screenNode->GetHeadroomMap();
    node->SetHdrNum(true, surfaceNodeId, screenNodeId, HDRComponentType::UICOMPONENT);
    CheckWithoutStatusLevel(map, HdrStatus::HDR_PHOTO, RSRenderNode::DEFAULT_HEADROOM_VALUE);
    CheckWithStatusLevel(map, HdrStatus::HDR_UICOMPONENT, RSRenderNode::DEFAULT_HEADROOM_VALUE);
    CheckWithoutStatusLevel(map, HdrStatus::HDR_EFFECT, RSRenderNode::DEFAULT_HEADROOM_VALUE);
    node->SetHdrNum(false, surfaceNodeId, screenNodeId, HDRComponentType::UICOMPONENT);
    CheckWithoutStatusLevel(map, HdrStatus::HDR_PHOTO, RSRenderNode::DEFAULT_HEADROOM_VALUE);
    CheckWithoutStatusLevel(map, HdrStatus::HDR_UICOMPONENT, RSRenderNode::DEFAULT_HEADROOM_VALUE);
    CheckWithoutStatusLevel(map, HdrStatus::HDR_EFFECT, RSRenderNode::DEFAULT_HEADROOM_VALUE);
}

/**
 * @tc.name: SetHdrNum005
 * @tc.desc: SetHdrNum test
 * @tc.type: FUNC
 * @tc.require: issueI9US6V
 */
HWTEST_F(RSRenderNodeTest2, SetHdrNum005, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    EXPECT_NE(rsContext, nullptr);
    auto node = std::make_shared<RSRenderNode>(0, rsContext);
    EXPECT_NE(node, nullptr);
    NodeId surfaceNodeId = 1UL;
    NodeId screenNodeId = 2UL;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceNodeId);
    EXPECT_NE(surfaceNode, nullptr);
    rsContext->nodeMap.RegisterRenderNode(surfaceNode);
    auto screenNode = std::make_shared<RSScreenRenderNode>(screenNodeId, 0U, rsContext);
    EXPECT_NE(screenNode, nullptr);
    rsContext->nodeMap.RegisterRenderNode(screenNode);

    node->SetHdrPhotoHeadroom(RSRenderNode::DEFAULT_HEADROOM_VALUE);
    node->SetHdrUIComponentHeadroom(RSRenderNode::DEFAULT_HEADROOM_VALUE);
    node->SetEnableHdrEffect(RSRenderNode::DEFAULT_HEADROOM_VALUE);
    const auto &map = screenNode->GetHeadroomMap();
    node->SetHdrNum(true, surfaceNodeId, screenNodeId, HDRComponentType::EFFECT);
    CheckWithoutStatusLevel(map, HdrStatus::HDR_PHOTO, RSRenderNode::DEFAULT_HEADROOM_VALUE);
    CheckWithoutStatusLevel(map, HdrStatus::HDR_UICOMPONENT, RSRenderNode::DEFAULT_HEADROOM_VALUE);
    CheckWithStatusLevel(map, HdrStatus::HDR_EFFECT, RSRenderNode::DEFAULT_HEADROOM_VALUE);
    node->SetHdrNum(false, surfaceNodeId, screenNodeId, HDRComponentType::EFFECT);
    CheckWithoutStatusLevel(map, HdrStatus::HDR_PHOTO, RSRenderNode::DEFAULT_HEADROOM_VALUE);
    CheckWithoutStatusLevel(map, HdrStatus::HDR_UICOMPONENT, RSRenderNode::DEFAULT_HEADROOM_VALUE);
    CheckWithoutStatusLevel(map, HdrStatus::HDR_EFFECT, RSRenderNode::DEFAULT_HEADROOM_VALUE);
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
    EXPECT_NE(node, nullptr);
    node->InitRenderParams();
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(1);
    EXPECT_NE(surfaceNode, nullptr);
    surfaceNode->InitRenderParams();
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
 * @tc.name: CheckHdrHeadroomInfoPointer
 * @tc.desc: CheckHdrHeadroomInfoPointer test
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeTest2, CheckHdrHeadroomInfoPointer, TestSize.Level1)
{
    NodeId nodeId = static_cast<NodeId>(0);
    auto node = std::make_shared<RSRenderNode>(nodeId);
    EXPECT_NE(node, nullptr);
    EXPECT_EQ(node->headroomInfo_, nullptr);
    node->CheckHdrHeadroomInfoPointer();
    EXPECT_NE(node->headroomInfo_, nullptr);
    node->CheckHdrHeadroomInfoPointer();
    EXPECT_NE(node->headroomInfo_, nullptr);
}

/**
 * @tc.name: SetHdrPhotoHeadroom
 * @tc.desc: SetHdrPhotoHeadroom test
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeTest2, SetHdrPhotoHeadroom, TestSize.Level1)
{
    NodeId nodeId = static_cast<NodeId>(0);
    auto node = std::make_shared<RSRenderNode>(nodeId);
    EXPECT_NE(node, nullptr);
    EXPECT_EQ(node->headroomInfo_, nullptr);
    constexpr uint32_t HEADROOM = 1U;
    node->SetHdrPhotoHeadroom(HEADROOM);
    EXPECT_NE(node->headroomInfo_, nullptr);
    EXPECT_NE(node->headroomInfo_->hdrPhotoHeadroom, RSRenderNode::DEFAULT_HEADROOM_VALUE);
}

/**
 * @tc.name: SetHdrEffectHeadroom
 * @tc.desc: SetHdrEffectHeadroom test
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeTest2, SetHdrEffectHeadroom, TestSize.Level1)
{
    NodeId nodeId = static_cast<NodeId>(0);
    auto node = std::make_shared<RSRenderNode>(nodeId);
    EXPECT_NE(node, nullptr);
    EXPECT_EQ(node->headroomInfo_, nullptr);
    constexpr uint32_t HEADROOM = 1U;
    node->SetHdrEffectHeadroom(HEADROOM);
    EXPECT_NE(node->headroomInfo_, nullptr);
    EXPECT_NE(node->headroomInfo_->hdrEffectHeadroom, RSRenderNode::DEFAULT_HEADROOM_VALUE);
}

/**
 * @tc.name: SetHdrUIComponentHeadroom
 * @tc.desc: SetHdrUIComponentHeadroom test
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeTest2, SetHdrUIComponentHeadroom, TestSize.Level1)
{
    NodeId nodeId = static_cast<NodeId>(0);
    auto node = std::make_shared<RSRenderNode>(nodeId);
    EXPECT_NE(node, nullptr);
    EXPECT_EQ(node->headroomInfo_, nullptr);
    constexpr uint32_t HEADROOM = 1U;
    node->SetHdrUIComponentHeadroom(HEADROOM);
    EXPECT_NE(node->headroomInfo_, nullptr);
    EXPECT_NE(node->headroomInfo_->hdrUIComponentHeadroom, RSRenderNode::DEFAULT_HEADROOM_VALUE);
}

/**
 * @tc.name: GetHdrPhotoHeadroom
 * @tc.desc: GetHdrPhotoHeadroom test
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeTest2, GetHdrPhotoHeadroom, TestSize.Level1)
{
    NodeId nodeId = static_cast<NodeId>(0);
    auto node = std::make_shared<RSRenderNode>(nodeId);
    EXPECT_NE(node, nullptr);
    EXPECT_EQ(node->headroomInfo_, nullptr);
    uint32_t photoHeadroom = node->GetHdrPhotoHeadroom();
    EXPECT_EQ(photoHeadroom, RSRenderNode::DEFAULT_HEADROOM_VALUE);
    constexpr uint32_t HEADROOM = 1U;
    node->SetHdrPhotoHeadroom(HEADROOM);
    photoHeadroom = node->GetHdrPhotoHeadroom();
    EXPECT_NE(node->headroomInfo_, nullptr);
    EXPECT_EQ(photoHeadroom, HEADROOM);
    EXPECT_NE(photoHeadroom, RSRenderNode::DEFAULT_HEADROOM_VALUE);
}

/**
 * @tc.name: GetHdrEffectHeadroom
 * @tc.desc: GetHdrEffectHeadroom test
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeTest2, GetHdrEffectHeadroom, TestSize.Level1)
{
    NodeId nodeId = static_cast<NodeId>(0);
    auto node = std::make_shared<RSRenderNode>(nodeId);
    EXPECT_NE(node, nullptr);
    EXPECT_EQ(node->headroomInfo_, nullptr);
    uint32_t effectHeadroom = node->GetHdrEffectHeadroom();
    EXPECT_EQ(effectHeadroom, RSRenderNode::DEFAULT_HEADROOM_VALUE);
    constexpr uint32_t HEADROOM = 1U;
    node->SetHdrEffectHeadroom(HEADROOM);
    effectHeadroom = node->GetHdrEffectHeadroom();
    EXPECT_NE(node->headroomInfo_, nullptr);
    EXPECT_EQ(effectHeadroom, HEADROOM);
    EXPECT_NE(effectHeadroom, RSRenderNode::DEFAULT_HEADROOM_VALUE);
}

/**
 * @tc.name: GetHdrUIComponentHeadroom
 * @tc.desc: GetHdrUIComponentHeadroom test
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeTest2, GetHdrUIComponentHeadroom, TestSize.Level1)
{
    NodeId nodeId = static_cast<NodeId>(0);
    auto node = std::make_shared<RSRenderNode>(nodeId);
    EXPECT_NE(node, nullptr);
    EXPECT_EQ(node->headroomInfo_, nullptr);
    uint32_t uiComponentHeadroom = node->GetHdrUIComponentHeadroom();
    EXPECT_EQ(uiComponentHeadroom, RSRenderNode::DEFAULT_HEADROOM_VALUE);
    constexpr uint32_t HEADROOM = 1U;
    node->SetHdrUIComponentHeadroom(HEADROOM);
    uiComponentHeadroom = node->GetHdrUIComponentHeadroom();
    EXPECT_NE(node->headroomInfo_, nullptr);
    EXPECT_EQ(uiComponentHeadroom, HEADROOM);
    EXPECT_NE(uiComponentHeadroom, RSRenderNode::DEFAULT_HEADROOM_VALUE);
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
    std::unique_ptr<RSRenderParams> stagingRenderParams = std::make_unique<RSRenderParams>(0);
    EXPECT_NE(stagingRenderParams, nullptr);
    nodeTest->stagingRenderParams_ = std::move(stagingRenderParams);
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
    properties.GetEffect().needDrawBehindWindow_ = false;
    node.PostPrepareForBlurFilterNode(*rsDirtyManager, needRequestNextVsync);
    RSDrawableSlot slot = RSDrawableSlot::BACKGROUND_FILTER;
    node.GetDrawableVec(__func__)[static_cast<uint32_t>(slot)] = std::make_shared<DrawableV2::RSFilterDrawable>();
    node.PostPrepareForBlurFilterNode(*rsDirtyManager, needRequestNextVsync);
    ASSERT_NE(node.GetFilterDrawable(false), nullptr);
}

/**
 * @tc.name: ManageDrawingCacheTest02
 * @tc.desc: SetDrawingCacheChanged and test
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

    // GetDrawingCacheChanged test
    EXPECT_TRUE(nodeTest->GetDrawingCacheChanged());
}

/**
 * @tc.name: ManageDrawingCacheTest03
 * @tc.desc: SetDrawingCacheChanged test
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

    // GetDrawingCacheChanged test
    EXPECT_TRUE(nodeTest->GetDrawingCacheChanged());
}

/**
 * @tc.name: ManageDrawingCacheTest04
 * @tc.desc: SetDrawingCacheChanged test
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

    // GetDrawingCacheChanged test
    EXPECT_TRUE(nodeTest->GetDrawingCacheChanged());
}

/**
 * @tc.name: ManageDrawingCacheTest05
 * @tc.desc: SetDrawingCacheChanged test
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
 * @tc.name: MarkFilterCacheFlags009
 * @tc.desc: test
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeTest2, MarkFilterCacheFlags009, TestSize.Level1)
{
    RSRenderNode node(id, context);
    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    std::shared_ptr<DrawableV2::RSFilterDrawable> filterDrawable = std::make_shared<DrawableV2::RSFilterDrawable>();
    filterDrawable->stagingCacheManager_ = std::make_unique<RSFilterCacheManager>();
    filterDrawable->stagingCacheManager_->stagingForceClearCache_ = false;
    filterDrawable->stagingCacheManager_->stagingForceUseCache_ = false;
    filterDrawable->stagingCacheManager_->stagingFilterInteractWithDirty_ = false;
    filterDrawable->stagingCacheManager_->pendingPurge_ = false;
    filterDrawable->stagingVisibleRectInfo_ = std::make_unique<DrawableV2::RSFilterDrawable::FilterVisibleRectInfo>();
    filterDrawable->stagingVisibleRectInfo_->snapshotRect_ = RectI(0, 0, 10, 10);
    node.MarkFilterCacheFlags(filterDrawable, *rsDirtyManager, true);
    EXPECT_FALSE(filterDrawable->stagingCacheManager_->stagingIsLargeArea_);
    filterDrawable->stagingVisibleRectInfo_->snapshotRect_ = RectI(0, 0, 500, 500);
    node.MarkFilterCacheFlags(filterDrawable, *rsDirtyManager, true);
    EXPECT_TRUE(filterDrawable->stagingCacheManager_->stagingIsLargeArea_);
}

/**
 * @tc.name: IsPixelStretchValid
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueICTJLI
 */
HWTEST_F(RSRenderNodeTest2, IsPixelStretchValid, TestSize.Level1)
{
    auto testFunc = [](bool propValid, bool drawableValid, bool paramValid, bool expectation) {
        RSRenderNode node(id, context);
        // mock pixel stretch in rs property.
        node.GetMutableRenderProperties().SetPixelStretch(
            propValid ? std::make_optional<Vector4f>(1.f, 1.f, 1.f, 1.f) : std::nullopt);
        // mock pixel stretch in drawable vec.
        RSDrawableSlot slot = RSDrawableSlot::PIXEL_STRETCH;
        auto pixelStretchDrawable = std::make_shared<DrawableV2::RSPixelStretchDrawable>();
        node.GetDrawableVec(__func__)[static_cast<uint32_t>(slot)] = drawableValid ? pixelStretchDrawable : nullptr;
        // mock pixel stretch param in  drawable.
        pixelStretchDrawable->SetPixelStretch(
            paramValid ? std::make_optional<Vector4f>(1.f, 1.f, 1.f, 1.f) : std::nullopt);
        ASSERT_EQ(node.IsPixelStretchValid(), expectation);
    };

    for (bool propValid : {true, false}) {
        for (bool drawableValid : {true, false}) {
            for (bool paramValid : {true, false}) {
                bool expectation = propValid && drawableValid && paramValid;
                testFunc(propValid, drawableValid, paramValid, expectation);
            }
        }
    }
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
 * @tc.name: UpdateDrawableAfterPostPrepareTest
 * @tc.desc: UpdateDrawableAfterPostPrepareTest
 * @tc.type: FUNC
 * @tc.require: issueIBDI0L
 */
HWTEST_F(RSRenderNodeTest2, UpdateDrawableAfterPostPrepareTest, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSRenderNode>(0, rsContext);
    node->UpdateDrawableAfterPostPrepare(ModifierNG::RSModifierType::BACKGROUND_FILTER);
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
    nodeTest->modifiersNG_[ModifierNG::RSModifierType::ALPHA].emplace_back(modifier);
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
    displayNode->ApplyPositionZModifier();
    EXPECT_FALSE(displayNode->dirtyTypesNG_.test(transformModifierType));
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
    auto node = std::make_shared<RSRenderNode>(1);
    EXPECT_NE(node, nullptr);
    auto transformModifier = std::make_shared<ModifierNG::RSTransformRenderModifier>();
    node->AddModifier(transformModifier);
    node->FilterModifiersByPid(1);
    EXPECT_FALSE(node->GetModifiersNG(ModifierNG::RSModifierType::TRANSFORM).empty());
    node->FilterModifiersByPid(0);
    EXPECT_TRUE(node->GetModifiersNG(ModifierNG::RSModifierType::TRANSFORM).empty());
}

/**
 * @tc.name: GetDrawableVecTest001
 * @tc.desc: GetDrawableVec test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeTest2, GetDrawableVecTest001, TestSize.Level1)
{
#ifdef RS_ENABLE_MEMORY_DOWNTREE
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSRenderNode>(0, rsContext);
    EXPECT_NE(node, nullptr);
    node->drawableVec_ = nullptr;
    node->GetDrawableVec(__func__);
    EXPECT_NE(node->drawableVec, nullptr);
#endif
    ASSERT_TRUE(true);
}

/**
 * @tc.name: SetIsOnTheTreeTest002
 * @tc.desc: SetIsOnTheTree test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeTest2, SetIsOnTheTreeTest002, TestSize.Level1)
{
#ifdef RS_ENABLE_MEMORY_DOWNTREE
    auto node = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(node, nullptr);
    node->isTextureExportNode_ = true;
    node->isOnTheTree_ = false;
    node->SetIsOnTheTree(true, 0, 1, 1, 1);
    node->isOnTheTree_ = true;
    node->SetIsOnTheTree(false, 0, 1, 1, 1);
#endif
    ASSERT_TRUE(true);
}

/**
 * @tc.name: ClearDrawableVec2Test001
 * @tc.desc: ClearDrawableVec2 test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeTest2, ClearDrawableVec2Test001, TestSize.Level1)
{
#ifdef RS_ENABLE_MEMORY_DOWNTREE
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSRenderNode>(0, rsContext);
    EXPECT_NE(node, nullptr);
    node->drawableVecNeedClear_ = true;
    node->GetDrawableVec(__func__)[static_cast<int8_t>(RSDrawableSlot::CONTENT_STYLE)] =
        std::make_shared<DrawableV2::RSBackgroundColorDrawable>();
    node->ClearDrawableVec2();
    EXPECT_EQ(node->GetDrawableVec(__func__)[static_cast<int8_t>(RSDrawableSlot::CONTENT_STYLE)], nullptr);

    auto backgroundColorDrawable =
        std::make_shared<DrawableV2::RSBackgroundColorDrawable>();
    node->drawableVecNeedClear_ = true;
    node->GetDrawableVec(__func__)[static_cast<int8_t>(RSDrawableSlot::TRANSITION)] =
        std::make_shared<DrawableV2::RSBackgroundColorDrawable>();
    node->ClearDrawableVec2();
    EXPECT_EQ(node->GetDrawableVec(__func__)[static_cast<int8_t>(RSDrawableSlot::TRANSITION)], nullptr);
    node->drawableVecNeedClear_ = true;
    node->GetDrawableVec(__func__)[static_cast<int8_t>(RSDrawableSlot::BACKGROUND_STYLE)] =
        std::make_shared<DrawableV2::RSBackgroundColorDrawable>();
    node->ClearDrawableVec2();
    EXPECT_EQ(node->GetDrawableVec(__func__)[static_cast<int8_t>(RSDrawableSlot::BACKGROUND_STYLE)], nullptr);

    node->drawableVecNeedClear_ = true;
    node->GetDrawableVec(__func__)[static_cast<int8_t>(RSDrawableSlot::FOREGROUND_STYLE)] =
        std::make_shared<DrawableV2::RSBackgroundColorDrawable>();
    node->ClearDrawableVec2();
    EXPECT_EQ(node->GetDrawableVec(__func__)[static_cast<int8_t>(RSDrawableSlot::FOREGROUND_STYLE)], nullptr);

    node->drawableVecNeedClear_ = true;
    node->GetDrawableVec(__func__)[static_cast<int8_t>(RSDrawableSlot::OVERLAY)] =
        std::make_shared<DrawableV2::RSBackgroundColorDrawable>();
    node->ClearDrawableVec2();
    EXPECT_EQ(node->GetDrawableVec(__func__)[static_cast<int8_t>(RSDrawableSlot::OVERLAY)], nullptr);
#endif
    ASSERT_TRUE(true);
}

/**
 * @tc.name: InitRenderDrawableAndDrawableVec001
 * @tc.desc: InitRenderDrawableAndDrawableVec test if not reset node on the tree
 * @tc.type: FUNC
 * @tc.require: issues20621
 */
HWTEST_F(RSRenderNodeTest2, InitRenderDrawableAndDrawableVec001, TestSize.Level1)
{
#ifdef RS_ENABLE_MEMORY_DOWNTREE
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSRenderNode>(0, rsContext);
    EXPECT_NE(node, nullptr);
    node->released_ = false;

    std::shared_ptr<RSRenderNode> child = std::make_shared<RSRenderNode>(1);
    EXPECT_NE(child, nullptr);

    // renderDrawable_ not null, drawableVec_ not null
    node->renderDrawable_ = std::make_shared<RSRenderNodeDrawableAdapterBoy>(child);
    EXPECT_NE(node->renderDrawable_, nullptr);
    node->drawableVec_ = std::make_unique<RSDrawable::Vec>();
    node->InitRenderDrawableAndDrawableVec();
    EXPECT_NE(static_cast<int>(node->dirtyStatus_), 1);
    EXPECT_FALSE(node->released_);

    // renderDrawable_ null, drawableVec_ null
    node->renderDrawable_ = nullptr;
    EXPECT_EQ(node->renderDrawable_, nullptr);
    node->drawableVec_ = nullptr;
    node->InitRenderDrawableAndDrawableVec();
    EXPECT_NE(static_cast<int>(node->dirtyStatus_), 1);
    EXPECT_FALSE(node->released_);
#endif
    ASSERT_TRUE(true);
}

/**
 * @tc.name: InitRenderDrawableAndDrawableVec002
 * @tc.desc: InitRenderDrawableAndDrawableVec test if reset on node the tree
 * @tc.type: FUNC
 * @tc.require: issues20621
 */
HWTEST_F(RSRenderNodeTest2, InitRenderDrawableAndDrawableVec002, TestSize.Level1)
{
#ifdef RS_ENABLE_MEMORY_DOWNTREE
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSRenderNode>(0, rsContext);
    EXPECT_NE(node, nullptr);

    std::shared_ptr<RSRenderNode> parent = std::make_shared<RSRenderNode>(1);
    EXPECT_NE(parent, nullptr);
    std::shared_ptr<RSRenderNode> child = std::make_shared<RSRenderNode>(2);
    EXPECT_NE(child, nullptr);

    node->renderDrawable_ = std::make_shared<RSRenderNodeDrawableAdapterBoy>(child);
    EXPECT_NE(node->renderDrawable_, nullptr);
    node->drawableVec_ = std::make_unique<RSDrawable::Vec>();

    node->released_ = true;
    EXPECT_TRUE(node->parent_.expired());
    EXPECT_EQ(node->stagingRenderParams_, nullptr);
    node->InitRenderDrawableAndDrawableVec();
    EXPECT_EQ(static_cast<int>(node->dirtyStatus_), 1);
    EXPECT_FALSE(node->released_);

    node->released_ = true;
    std::unique_ptr<RSRenderParams> stagingRenderParams = std::make_unique<RSRenderParams>(0);
    EXPECT_NE(stagingRenderParams, nullptr);
    node->stagingRenderParams_ = std::move(stagingRenderParams);
    node->SetParent(parent);
    EXPECT_FALSE(node->parent_.expired());
    EXPECT_NE(node->stagingRenderParams_, nullptr);
    node->InitRenderDrawableAndDrawableVec();
    EXPECT_TRUE(node->stagingRenderParams_->dirtyType_.test(RSRenderParamsDirtyType::MATRIX_DIRTY));
    EXPECT_TRUE(node->stagingRenderParams_->dirtyType_.test(RSRenderParamsDirtyType::DRAWING_CACHE_TYPE_DIRTY));
    EXPECT_EQ(static_cast<int>(node->dirtyStatus_), 1);
    EXPECT_TRUE(node->parent_.lock()->dirtyTypesNG_.test(static_cast<size_t>(ModifierNG::RSModifierType::CHILDREN)));
    EXPECT_FALSE(node->released_);

#endif
    ASSERT_TRUE(true);
}

/**
 * @tc.name: MarkBlurIntersectWithDRM
 * @tc.desc: MarkBlurIntersectWithDRM test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeTest2, MarkBlurIntersectWithDRM, TestSize.Level1)
{
#ifdef RS_ENABLE_MEMORY_DOWNTREE
    // material filter
    RSRenderNode backgroundNode(id, context);
    auto& bgProperties = backgroundNode.GetMutableRenderProperties();
    bgProperties.GetEffect().materialFilter_ = std::make_shared<RSFilter>();
    backgroundNode.MarkBlurIntersectWithDRM(true, true);
    auto filterDrawable = std::make_shared<DrawableV2::RSMaterialFilterDrawable>();
    backgroundNode.GetDrawableVec(__func__)[static_cast<int8_t>(RSDrawableSlot::MATERIAL_FILTER)] = filterDrawable;
    backgroundNode.MarkBlurIntersectWithDRM(true, true);
    EXPECT_EQ(filterDrawable->stagingIntersectWithDRM_, true);
#endif
    ASSERT_TRUE(true);
}

/**
 * @tc.name: NodeDrawLargeAreaBlur
 * @tc.desc: NodeDrawLargeAreaBlur test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeTest2, NodeDrawLargeAreaBlur, TestSize.Level1)
{
#ifdef RS_ENABLE_MEMORY_DOWNTREE
    // material filter
    RSRenderNode backgroundNode(id, context);
    auto& bgProperties = backgroundNode.GetMutableRenderProperties();
    bgProperties.GetEffect().materialFilter_ = std::make_shared<RSFilter>();
    auto filterDrawable = std::make_shared<DrawableV2::RSMaterialFilterDrawable>();
    backgroundNode.GetDrawableVec(__func__)[static_cast<int8_t>(RSDrawableSlot::MATERIAL_FILTER)] = filterDrawable;
    std::pair<bool, bool> nodeBlurState = {true, true};
    backgroundNode.NodeDrawLargeAreaBlur(nodeBlurState);
    EXPECT_EQ(nodeBlurState.first, false);
    EXPECT_EQ(nodeBlurState.second, false);
#endif
    ASSERT_TRUE(true);
}

/**
 * @tc.name: InvokeFilterDrawable
 * @tc.desc: InvokeFilterDrawable test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeTest2, InvokeFilterDrawable, TestSize.Level1)
{
#ifdef RS_ENABLE_MEMORY_DOWNTREE
    // material filter
    RSRenderNode backgroundNode(id, context);
    auto& bgProperties = backgroundNode.GetMutableRenderProperties();
    bgProperties.GetEffect().materialFilter_ = std::make_shared<RSFilter>();
    auto filterDrawable = std::make_shared<DrawableV2::RSMaterialFilterDrawable>();
    backgroundNode.GetDrawableVec(__func__)[static_cast<int8_t>(RSDrawableSlot::MATERIAL_FILTER)] = filterDrawable;
    EXPECT_EQ(backgroundNode.InvokeFilterDrawable(RSDrawableSlot::MATERIAL_FILTER, nullptr), false);
    auto invokeFunc = [](std::shared_ptr<DrawableV2::RSFilterDrawable> drawable) {
        if (drawable) {
            drawable->MarkFilterForceClearCache();
        }
    };
    EXPECT_EQ(backgroundNode.InvokeFilterDrawable(RSDrawableSlot::MATERIAL_FILTER, invokeFunc), true);
    EXPECT_EQ(backgroundNode.InvokeFilterDrawable(RSDrawableSlot::BACKGROUND_FILTER, invokeFunc), false);
#endif
    ASSERT_TRUE(true);
}

/**
 * @tc.name: GetFilterDrawable2
 * @tc.desc: GetFilterDrawable2 test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeTest2, GetFilterDrawable2, TestSize.Level1)
{
#ifdef RS_ENABLE_MEMORY_DOWNTREE
    // material filter
    RSRenderNode backgroundNode(id, context);
    auto& bgProperties = backgroundNode.GetMutableRenderProperties();
    bgProperties.GetEffect().materialFilter_ = std::make_shared<RSFilter>();
    auto filterDrawable = std::make_shared<DrawableV2::RSMaterialFilterDrawable>();
    backgroundNode.GetDrawableVec(__func__)[static_cast<int8_t>(RSDrawableSlot::MATERIAL_FILTER)] = filterDrawable;
    EXPECT_EQ(backgroundNode.GetFilterDrawable(RSDrawableSlot::BACKGROUND_IMAGE) != nullptr, false);
    EXPECT_EQ(backgroundNode.GetFilterDrawable(RSDrawableSlot::MATERIAL_FILTER) != nullptr, true);
    EXPECT_EQ(backgroundNode.GetFilterDrawable(RSDrawableSlot::BACKGROUND_FILTER) != nullptr, false);
#endif
    ASSERT_TRUE(true);
}

/**
 * @tc.name: MarkFilterInForegroundFilterAndCheckNeedForceClearCache
 * @tc.desc: MarkFilterInForegroundFilterAndCheckNeedForceClearCache test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeTest2, MarkFilterInForegroundFilterAndCheckNeedForceClearCache, TestSize.Level1)
{
#ifdef RS_ENABLE_MEMORY_DOWNTREE
    // material filter
    RSRenderNode backgroundNode(id, context);
    auto& bgProperties = backgroundNode.GetMutableRenderProperties();
    bgProperties.GetEffect().materialFilter_ = std::make_shared<RSFilter>();
    bgProperties.backgroundFilter_ = std::make_shared<RSFilter>();
    auto filterDrawable = std::make_shared<DrawableV2::RSMaterialFilterDrawable>();
    backgroundNode.GetDrawableVec(__func__)[static_cast<int8_t>(RSDrawableSlot::MATERIAL_FILTER)] = filterDrawable;
    backgroundNode.MarkFilterInForegroundFilterAndCheckNeedForceClearCache(id);
    EXPECT_EQ(filterDrawable->stagingCacheManager_->stagingForceClearCache_, false);
#endif
    ASSERT_TRUE(true);
}

/**
 * @tc.name: CollectAndUpdateLocalEffectRect001
 * @tc.desc: CollectAndUpdateLocalEffectRect test
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeTest2, CollectAndUpdateLocalEffectRect001, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSRenderNode>(0, rsContext);
    node->dirtySlots_.insert(RSDrawableSlot::BACKGROUND_FILTER);
    node->dirtySlots_.insert(RSDrawableSlot::COMPOSITING_FILTER);
    node->dirtySlots_.insert(RSDrawableSlot::MATERIAL_FILTER);
    auto& properties = node->GetMutableRenderProperties();
    properties.backgroundFilter_ = std::make_shared<RSFilter>();
    properties.filter_ = std::make_shared<RSFilter>();

    node->selfDrawRect_ = RectF();
    node->CollectAndUpdateLocalEffectRect();
    EXPECT_EQ(node->selfDrawRect_, RectF());
}

/**
 * @tc.name: CalVisibleFilterRect001
 * @tc.desc: CalVisibleFilterRect test
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeTest2, CalVisibleFilterRect001, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSRenderNode>(0, rsContext);
    auto geo = node->GetRenderProperties().boundsGeo_;
    node->CalVisibleFilterRect(std::nullopt, std::nullopt);
    auto filterDrawable = std::make_shared<DrawableV2::RSFilterDrawable>();
    node->GetDrawableVec(__func__)[static_cast<uint32_t>(RSDrawableSlot::BACKGROUND_FILTER)] =
        filterDrawable;

    auto absRect = RectI(0, 0, 10, 10);
    node->CalVisibleFilterRect(absRect, Drawing::Matrix(), std::nullopt, std::nullopt);
    EXPECT_EQ(node->GetFilterRegionInfo().defaultFilterRegion_, absRect);

    node->GetMutableRenderProperties().boundsGeo_ = nullptr;
    node->CalVisibleFilterRect(std::nullopt, std::nullopt);
}

/**
 * @tc.name: GetRSFilterDrawableWithSlot001
 * @tc.desc: GetRSFilterDrawableWithSlot test
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeTest2, GetRSFilterDrawableWithSlot001, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSRenderNode>(0, rsContext);
    EXPECT_EQ(node->GetRSFilterWithSlot(RSDrawableSlot::SHADOW), nullptr);
}

/**
 * @tc.name: GetFilterDrawableSnapshotRegion001
 * @tc.desc: GetFilterDrawableSnapshotRegion test
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeTest2, GetFilterDrawableSnapshotRegion001, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSRenderNode>(0, rsContext);
    auto& properties = node->GetMutableRenderProperties();
    properties.backgroundFilter_ = std::make_shared<RSFilter>();
    properties.filter_ = std::make_shared<RSFilter>();
    auto filterDrawable = std::make_shared<DrawableV2::RSFilterDrawable>();
    node->GetDrawableVec(__func__)[static_cast<uint32_t>(RSDrawableSlot::BACKGROUND_FILTER)] =
        filterDrawable;
    node->UpdateFilterRectInfo();
    EXPECT_EQ(node->GetFilterDrawableSnapshotRegion(), RectI());
}

/**
 * @tc.name: UpdateDrawableEnableEDR
 * @tc.desc: Test function UpdateDrawableEnableEDR
 * @tc.type: FUNC
 * @tc.require: issueICD8D6
 */
HWTEST_F(RSRenderNodeTest2, UpdateDrawableEnableEDR, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(2);
    node->stagingRenderParams_ = std::make_unique<RSRenderParams>(0);
    node->UpdateDrawableEnableEDR();
    EXPECT_FALSE(node->enableHdrEffect_);
    auto filterDrawable = std::make_shared<DrawableV2::RSFilterDrawable>();
    filterDrawable->enableEDREffect_ = true;
    node->GetDrawableVec(__func__)[static_cast<uint32_t>(RSDrawableSlot::BACKGROUND_FILTER)] =
        filterDrawable;
    node->UpdateDrawableEnableEDR();
    EXPECT_TRUE(node->enableHdrEffect_);
}

/**
 * @tc.name: UpdateFilterCacheWithBackgroundAndAlphaDirtyTest
 * @tc.desc: Test function UpdateFilterCacheWithBackgroundAndAlphaDirtyTest
 * @tc.type: FUNC
 * @tc.require: issueICD8D6
 */
HWTEST_F(RSRenderNodeTest2, UpdateFilterCacheWithBackgroundAndAlphaDirtyTest, TestSize.Level1)
{
    // both false
    {
        auto renderNode = std::make_shared<RSRenderNode>(1);
        ASSERT_NE(renderNode, nullptr);
        renderNode->UpdateFilterCacheWithBackgroundDirty();
    }

    // alphaDirty
    {
        auto renderNode = std::make_shared<RSRenderNode>(1);
        ASSERT_NE(renderNode, nullptr);
        renderNode->dirtyTypesNG_.set(static_cast<size_t>(ModifierNG::RSModifierType::ALPHA), true);
        renderNode->UpdateFilterCacheWithBackgroundDirty();
    }

    // both true
    {
        auto renderNode = std::make_shared<RSRenderNode>(1);
        renderNode->dirtyTypesNG_.set(static_cast<size_t>(ModifierNG::RSModifierType::ALPHA), true);
        auto backgroundColorDrawable = std::make_shared<DrawableV2::RSBackgroundColorDrawable>();
        EXPECT_NE(backgroundColorDrawable, nullptr);
        renderNode->GetDrawableVec(__func__)[static_cast<uint32_t>(RSDrawableSlot::BACKGROUND_COLOR)]
            = backgroundColorDrawable;
        renderNode->UpdateFilterCacheWithBackgroundDirty();
    }
}

/**
 * @tc.name: ApplyModifiersProcessUnionInfoAfterApplyModifiers001
 * @tc.desc: test ApplyModifiersProcessUnionInfoAfterApplyModifiers001
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeTest2, ApplyModifiersProcessUnionInfoAfterApplyModifiers001, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSRenderNode>(0, rsContext);
    node->dirtyStatus_ = RSRenderNode::NodeDirty::DIRTY;
    node->dirtyTypesNG_.set(static_cast<size_t>(ModifierNG::RSModifierType::BOUNDS), true);
    node->stagingRenderParams_ = std::make_unique<RSRenderParams>(0);

    node->ApplyModifiers();
    ASSERT_FALSE(node->renderProperties_.useUnion_);
}

/**
 * @tc.name: ApplyModifiersProcessUnionInfoAfterApplyModifiers002
 * @tc.desc: test ApplyModifiersProcessUnionInfoAfterApplyModifiers001
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeTest2, ApplyModifiersProcessUnionInfoAfterApplyModifiers002, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSRenderNode>(0, rsContext);
    node->dirtyStatus_ = RSRenderNode::NodeDirty::DIRTY;
    node->stagingRenderParams_ = std::make_unique<RSRenderParams>(0);

    node->ApplyModifiers();
    ASSERT_FALSE(node->renderProperties_.useUnion_);
}
} // namespace Rosen
} // namespace OHOS