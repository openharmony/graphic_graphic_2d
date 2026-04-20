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

class RSRenderNodeUnitTest2 : public testing::Test {
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

void RSRenderNodeUnitTest2::SetUpTestCase()
{
    canvas_ = new RSPaintFilterCanvas(&drawingCanvas_);
}
void RSRenderNodeUnitTest2::TearDownTestCase()
{
    delete canvas_;
    canvas_ = nullptr;
}
void RSRenderNodeUnitTest2::SetUp() {}
void RSRenderNodeUnitTest2::TearDown() {}

/**
 * @tc.name: MarkFilterCacheFlags001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeUnitTest2, MarkFilterCacheFlags001, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest2, MarkFilterCacheFlags002, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest2, CheckFilterCacheAndUpdateDirtySlots, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest2, GetOldAbsMatrix001, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest2, UpdateAbsDirtyRegion001, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest2, UpdateDrawRectAndDirtyRegion001, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest2, UpdateDrawRectAndDirtyRegion002, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest2, MarkForceClearFilterCacheWithInvisible, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest2, RenderTraceDebug, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest2, ApplyAlphaAndBoundsGeometry, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest2, ProcessRenderBeforeChildren, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest2, ProcessTransitionAfterChildren, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest2, IsCrossNodeTest, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest2, SetCrossNodeVisitedStatusTest, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest2, UpdateDirtyRegionInfoForDFX001, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest2, UpdateDirtyRegionInfoForDFX002, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest2, SetNodeName, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest2, SetAccumulatedClipFlagTest, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest2, GetIsFullChildrenListValid, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest2, IsPureBackgroundColorTest, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest2, IsPureBackgroundColorTest002, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest2, SetUIContextTokenTest001, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest2, GetUIContextTokenListTest001, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest2, UpdateFilterCacheForceClearWithBackgroundAndAlphaDirtyTest, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest2, NotForceClearFilterCacheWithoutBackgroundDirtyTest, TestSize.Level1)
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
    auto backgroundColorDrawable = std::make_shared<DrawableV2::RSBackgroundColorDrawable>();
    renderNode->GetDrawableVec(__func__)[static_cast<uint32_t>(RSDrawableSlot::BACKGROUND_COLOR)]
        = backgroundColorDrawable;
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
HWTEST_F(RSRenderNodeUnitTest2, NotForceClearFilterCacheWithoutAlphaDirtyTest, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest2, NotForceClearFilterCacheWithoutBackBackgoundImageDirtyTest, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest2, ForceClearFilterCacheWhenBackgroundDirty, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest2, ForceClearForegroundFilterCacheWhenDirty, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest2, HasHpaeBackgroundFilter, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest2, UpdateVirtualScreenWhiteListInfo, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest2, GetNeedUseCmdlistDrawRegion001, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest2, GetNeedUseCmdlistDrawRegion002, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest2, ResetDirtyStatusTest001, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest2, ResetParent, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest2, ResetAndApplyModifiers001, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest2, RepaintBoundary, TestSize.Level1)
{
    auto renderNode = std::make_shared<RSRenderNode>(1);
    ASSERT_NE(renderNode, nullptr);
    renderNode->MarkRepaintBoundary(true);
    ASSERT_EQ(renderNode->IsRepaintBoundary(), true);
}

/**
 * @tc.name: RSUIContextDumpTest
 * @tc.desc: DumpNodeType DumpTree
 * @tc.type: FUNC
 * @tc.require: issueICPQSU
 */
HWTEST_F(RSRenderNodeUnitTest2, RSUIContextDumpTest, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest2, AddChildTest004, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest2, AddChildTest005, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest2, MoveChildTest005, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest2, SetContainBootAnimationTest001, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest2, ParentChildRelationshipTest006, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest2, RSRenderNodeTreeTest007, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest2, AddCrossParentChildTest008, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest2, RemoveCrossParentChild009, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest2, AddCrossScreenChild, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest2, RemoveCrossScreenChild, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest2, RemoveFromTreeTest010, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest2, ClearChildrenTest011, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest2, UpdateDrawingCacheInfoAfterChildrenTest001, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest2, UpdateDrawingCacheInfoAfterChildrenTest002, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest2, UpdateDrawingCacheInfoAfterChildrenTest003, TestSize.Level1)
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
 * @tc.name: UpdateDrawingCacheInfoAfterChildrenTest005
 * @tc.desc: Test ForceDisableNodeGroup
 * @tc.type: FUNC
 * @tc.require: issue21180
 */
HWTEST_F(RSRenderNodeUnitTest2, UpdateDrawingCacheInfoAfterChildrenTest005, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest2, UpdateDrawingCacheInfoAfterChildrenTest006, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest2, UpdateDrawingCacheInfoAfterChildrenTest007, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest2, UpdateDrawingCacheInfoAfterChildrenTest008, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest2, UpdateDrawingCacheInfoBeforeChildrenTest001, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest2, UpdateDrawingCacheInfoBeforeChildrenTest002, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest2, UpdateDrawingCacheInfoBeforeChildrenTest003, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest2, ExcludedFromNodeGroupTest, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest2, SetHasChildExcludedFromNodeGroupTest, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest2, SetRenderGroupExcludedStateChangedTest, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest2, SetRenderGroupSubTreeDirtyTest, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest2, InvalidateHierarchyTest018, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest2, UpdateDrawableVecV2Test019, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> renderNodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(renderNodeTest, nullptr);

    renderNodeTest->UpdateDrawableVecV2();

    renderNodeTest->dirtyTypesNG_.set(static_cast<size_t>(ModifierNG::RSModifierType::BOUNDS), true);
    renderNodeTest->dirtyTypesNG_.set(static_cast<size_t>(ModifierNG::RSModifierType::TRANSFORM), true);
    std::shared_ptr<DrawableTest> drawableTest1 = std::make_shared<DrawableTest>();
    renderNodeTest->GetDrawableVec(__func__)[1] = drawableTest1;
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
HWTEST_F(RSRenderNodeUnitTest2, UpdateDisplayListTest020, TestSize.Level1)
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
HWTEST_F(RSRenderNodeUnitTest2, UpdateRenderingTest021, TestSize.Level1)
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
} // namespace Rosen
} // namespace OHOS
