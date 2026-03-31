/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#include "drawable/rs_color_picker_drawable.h"
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
#include "feature/color_picker/color_pick_alt_manager.h"
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

class RSRenderNodeUnitTest4 : public testing::Test {
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

void RSRenderNodeUnitTest4::SetUpTestCase()
{
    canvas_ = new RSPaintFilterCanvas(&drawingCanvas_);
}
void RSRenderNodeUnitTest4::TearDownTestCase()
{
    delete canvas_;
    canvas_ = nullptr;
}
void RSRenderNodeUnitTest4::SetUp() {}
void RSRenderNodeUnitTest4::TearDown() {}

/**
 * @tc.name: MarkFilterCacheFlags01
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeUnitTest4, MarkFilterCacheFlags01, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest4, MarkFilterCacheFlags02, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest4, MarkFilterCacheFlags03, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest4, MarkFilterCacheFlags04, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest4, MarkFilterCacheFlags05, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest4, MarkFilterCacheFlags06, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest4, MarkFilterCacheFlags07, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest4, MarkFilterCacheFlags08, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest4, MarkFilterCacheFlags009, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest4, IsPixelStretchValid, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest4, ResortChildrenTest02, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest4, UpdateDrawableAfterPostPrepareTest, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest4, SetUIFirstSwitchTest001, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest4, SetUIFirstSwitchTest002, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest4, DumpModifiersTest, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest4, ApplyPositionZModifierTest, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest4, FilterModifiersByPidTest, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest4, GetDrawableVecTest001, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest4, SetIsOnTheTreeTest002, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest4, ClearDrawableVec2Test001, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest4, InitRenderDrawableAndDrawableVec001, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest4, InitRenderDrawableAndDrawableVec002, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest4, MarkBlurIntersectWithDRM, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest4, NodeDrawLargeAreaBlur, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest4, InvokeFilterDrawable, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest4, GetFilterDrawable2, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest4, MarkFilterInForegroundFilterAndCheckNeedForceClearCache, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest4, CollectAndUpdateLocalEffectRect001, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest4, CalVisibleFilterRect001, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest4, GetRSFilterDrawableWithSlot001, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest4, GetFilterDrawableSnapshotRegion001, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest4, UpdateDrawableEnableEDR, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest4, UpdateFilterCacheWithBackgroundAndAlphaDirtyTest, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest4, ApplyModifiersProcessUnionInfoAfterApplyModifiers001, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest4, ApplyModifiersProcessUnionInfoAfterApplyModifiers002, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSRenderNode>(0, rsContext);
    node->dirtyStatus_ = RSRenderNode::NodeDirty::DIRTY;
    node->stagingRenderParams_ = std::make_unique<RSRenderParams>(0);

    node->ApplyModifiers();
    ASSERT_FALSE(node->renderProperties_.useUnion_);
}

/**
 * @tc.name: ResetColorPickerAltMemoryOnTreeStateChange
 * @tc.desc: test ResetColorMemory is called when ColorPickAltManager node goes off the tree
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeUnitTest4, ResetColorPickerAltMemoryOnTreeStateChange, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSRenderNode>(0, rsContext);
    EXPECT_NE(node, nullptr);

    // Create a color picker drawable with alt manager (useAlt = true)
    auto colorPickerDrawable = std::make_shared<DrawableV2::RSColorPickerDrawable>(true, node->GetId());
    EXPECT_NE(colorPickerDrawable, nullptr);
    EXPECT_NE(colorPickerDrawable->colorPickerManager_, nullptr);

    // Add the color picker drawable to the node
    node->GetDrawableVec(__func__)[static_cast<int8_t>(RSDrawableSlot::COLOR_PICKER)] = colorPickerDrawable;

    // Cast to ColorPickAltManager to access pickedLuminance_
    auto altManager = std::static_pointer_cast<ColorPickAltManager>(colorPickerDrawable->colorPickerManager_);
    ASSERT_NE(altManager, nullptr);

    // Set a valid luminance value
    altManager->pickedLuminance_.store(200, std::memory_order_relaxed);
    EXPECT_EQ(altManager->pickedLuminance_.load(std::memory_order_relaxed), 200u);

    // Set node on the tree, then remove it - ResetColorMemory should reset luminance to invalid value
    node->SetIsOnTheTree(true, 0, 1, 1, 1);
    node->SetIsOnTheTree(false, 0, 1, 1, 1);

    // Verify that pickedLuminance_ was reset to RGBA_MAX + 1 (invalid initial value)
    EXPECT_EQ(altManager->pickedLuminance_.load(std::memory_order_relaxed),
        static_cast<uint32_t>(RGBA_MAX + 1));
}

/**
 * @tc.name: SetIsOnTheTreeTest
 * @tc.desc: SetIsOnTheTree ResetChildRelevantFlags and UpdateChildrenRect test
 * @tc.type: FUNC
 * @tc.require: issueI9US6V
 */
HWTEST_F(RSRenderNodeUnitTest4, SetIsOnTheTreeTest, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest4, SetIsOnTheTreeTest02, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest4, HDRStatusTest, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest4, SetHdrNum001, TestSize.Level1)
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

void RSRenderNodeUnitTest4::CheckWithStatusLevel(const RSScreenRenderNode::HeadroomMap &map,
    HdrStatus status, uint32_t level)
{
    ASSERT_EQ(map.count(status), 1U);
    ASSERT_EQ(map.at(status).count(level), 1U);
    ASSERT_EQ(map.at(status).at(level), 1U);
}

void RSRenderNodeUnitTest4::CheckWithoutStatusLevel(const RSScreenRenderNode::HeadroomMap &map,
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
HWTEST_F(RSRenderNodeUnitTest4, SetHdrNum002, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest4, SetHdrNum003, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest4, SetHdrNum004, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest4, SetHdrNum005, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest4, SetEnableHdrEffect, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest4, CheckHdrHeadroomInfoPointer, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest4, SetHdrPhotoHeadroom, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest4, SetHdrEffectHeadroom, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest4, SetHdrUIComponentHeadroom, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest4, GetHdrPhotoHeadroom, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest4, GetHdrEffectHeadroom, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest4, GetHdrUIComponentHeadroom, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest4, RSRenderNodeDumpTest, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest4, ForceMergeSubTreeDirtyRegionTest02, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest4, UpdateRenderStatus02, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest4, RSRenderNodeDumpTest02, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest4, ForceMergeSubTreeDirtyRegionTest03, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest4, ForceMergeSubTreeDirtyRegionTest04, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest4, PostPrepareForBlurFilterNode03, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest4, ManageDrawingCacheTest02, TestSize.Level2)
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
HWTEST_F(RSRenderNodeUnitTest4, ManageDrawingCacheTest03, TestSize.Level2)
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
HWTEST_F(RSRenderNodeUnitTest4, ManageDrawingCacheTest04, TestSize.Level2)
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
HWTEST_F(RSRenderNodeUnitTest4, ManageDrawingCacheTest05, TestSize.Level2)
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
HWTEST_F(RSRenderNodeUnitTest4,  SetGeoUpdateDelay, TestSize.Level2)
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
HWTEST_F(RSRenderNodeUnitTest4, UpdateDirtyRegionTest02, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest4, GenerateFullChildrenListTest02, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest4, UpdatePointLightDirtySlotTest02, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest4, UpdatePointLightDirtySlotTest03, TestSize.Level1)
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
} // namespace Rosen
} // namespace OHOS