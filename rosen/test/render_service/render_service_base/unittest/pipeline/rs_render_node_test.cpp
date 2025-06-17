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

#include <gtest/gtest.h>

#include "common/rs_obj_abs_geometry.h"
#include "drawable/rs_property_drawable_foreground.h"
#include "offscreen_render/rs_offscreen_render_thread.h"
#include "params/rs_render_params.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_dirty_region_manager.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "skia_adapter/skia_canvas.h"
#include "parameters.h"
#ifdef NEW_SKIA
#include "include/gpu/GrDirectContext.h"
#else
#include "third_party/flutter/skia/include/gpu/GrContext.h"
#endif

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
const std::string OUT_STR1 =
    "DISPLAY_NODERS_NODESURFACE_NODECANVAS_NODEROOT_NODEPROXY_NODECANVAS_DRAWING_NODEEFFECT_NODEUNKNOWN_NODE";
const std::string OUT_STR2 =
    "| RS_NODE[0], instanceRootNodeId[0], SharedTransitionParam: [0 -> 0], [nodeGroup1], uifirstRootNodeId_: 1, "
    "Properties: Bounds[-inf -inf -inf -inf] Frame[-inf -inf -inf -inf], GetBootAnimation: true, "
    "isContainBootAnimation: true, isNodeDirty: 1, isPropertyDirty: true, isSubTreeDirty: true, IsPureContainer: true, "
    "Children list needs update, current count: 0 expected count: 0, disappearingChildren: 1\n"
    "  | RS_NODE[0], instanceRootNodeId[0], Properties: Bounds[-inf -inf -inf -inf] Frame[-inf -inf -inf -inf], "
    "IsPureContainer: true\n";
const int DEFAULT_BOUNDS_SIZE = 10;
const int DEFAULT_NODE_ID = 1;
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

class PropertyDrawableTest : public RSPropertyDrawable {
public:
    void Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const override
    {
        return;
    }
    bool Update(const RSRenderContent& content) override
    {
        return false;
    };
};

class DrawableTest : public RSDrawable {
public:
    bool OnUpdate(const RSRenderNode& content) override
    {
        return true;
    }
    Drawing::RecordingCanvas::DrawFunc CreateDrawFunc() const override
    {
        return nullptr;
    }
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
}

/**
 * @tc.name: ProcessTransitionBeforeChildrenTest
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeTest, AddModifierTest, TestSize.Level1)
{
    std::shared_ptr<RSRenderModifier> modifier = nullptr;
    RSRenderNode node(id, context);
    node.AddModifier(modifier);
    ASSERT_FALSE(node.IsDirty());
}

/**
 * @tc.name: InitCacheSurfaceTest
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeTest, InitCacheSurfaceTest, TestSize.Level1)
{
    RSRenderNode node(id, context);
    CacheType type = CacheType::ANIMATE_PROPERTY;
    node.SetCacheType(type);
    node.InitCacheSurface(canvas_->GetGPUContext().get());
}

/**
 * @tc.name: DrawCacheSurfaceTest
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeTest, DrawCacheSurfaceTest001, TestSize.Level1)
{
    RSRenderNode node(id, context);
    bool isSubThreadNode = false;
    node.DrawCacheSurface(*canvas_, isSubThreadNode);
    if (node.GetCompletedCacheSurface() == nullptr) {
        ASSERT_EQ(NULL, node.GetCacheSurface());
    }
}

/**
 * @tc.name: DrawCacheSurfaceTest002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeTest, DrawCacheSurfaceTest002, TestSize.Level1)
{
    RSRenderNode node(id, context);
    bool isSubThreadNode = false;
    CacheType type = CacheType::ANIMATE_PROPERTY;
    node.SetCacheType(type);
    node.DrawCacheSurface(*canvas_, isSubThreadNode);
    ASSERT_EQ(node.GetCompletedCacheSurface(), NULL);
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
 * @tc.name: ResetFilterRectsInCacheTest
 * @tc.desc: test ResetFilterRectsInCache api
 * @tc.type: FUNC
 * @tc.require: issueI84LBZ
 */
HWTEST_F(RSRenderNodeTest, ResetFilterRectsInCacheTest, TestSize.Level2)
{
    RSRenderNode node(id, context);
    RSRenderNode cacheNode(id + 1, context);
    std::unordered_set<NodeId> setRects = {};
    std::unordered_map<NodeId, std::unordered_set<NodeId>> getRects = {};
    setRects.insert(cacheNode.GetId());

    node.ResetFilterRectsInCache(setRects);
    node.GetFilterRectsInCache(getRects);
    ASSERT_NE(getRects[node.GetId()].size(), 0);
}

/**
 * @tc.name: ManageDrawingCacheTest001
 * @tc.desc: SetDrawingCacheChanged and ResetDrawingCacheNeedUpdate test
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

    // ResetDrawingCacheNeedUpdate test
    nodeTest->drawingCacheNeedUpdate_ = true;
    nodeTest->ResetDrawingCacheNeedUpdate();
    EXPECT_FALSE(nodeTest->drawingCacheNeedUpdate_);

    // GetDrawingCacheChanged test
    EXPECT_TRUE(nodeTest->GetDrawingCacheChanged());
}

/**
 * @tc.name: ResetDrawingCacheNeedUpdateTest001
 * @tc.desc: test ResetDrawingCacheNeedUpdateTest api
 * @tc.type: FUNC
 * @tc.require: issueI84LBZ
 */
HWTEST_F(RSRenderNodeTest, ResetDrawingCacheNeedUpdateTest001, TestSize.Level2)
{
    auto node = std::make_shared<RSRenderNode>(id, context);
    node->InitRenderParams();
    bool isDrawingCacheChanged = true;

    node->SetDrawingCacheChanged(isDrawingCacheChanged);
    node->ResetDrawingCacheNeedUpdate();
    ASSERT_EQ(node->drawingCacheNeedUpdate_, false);
}

/**
 * @tc.name: ResetDrawingCacheNeedUpdateTest002
 * @tc.desc: don't reset DrawingCacheNeedUpdate after set DrawingCacheChanged
 * @tc.type: FUNC
 * @tc.require: issueI84LBZ
 */
HWTEST_F(RSRenderNodeTest, ResetDrawingCacheNeedUpdateTest002, TestSize.Level2)
{
    auto node = std::make_shared<RSRenderNode>(id, context);
    node->InitRenderParams();
    node->SetDrawingCacheChanged(true);
    ASSERT_EQ(node->GetDrawingCacheChanged(), true);
}

/**
 * @tc.name: SetVisitedCacheRootIdsTest
 * @tc.desc: test SetVisitedCacheRootIds api
 * @tc.type: FUNC
 * @tc.require: issueI84LBZ
 */
HWTEST_F(RSRenderNodeTest, SetVisitedCacheRootIdsTest, TestSize.Level2)
{
    RSRenderNode node(id, context);
    RSRenderNode cacheNode(id + 1, context);
    std::unordered_set<NodeId> VisitedIds = {};
    VisitedIds.insert(cacheNode.GetId());

    node.SetVisitedCacheRootIds(VisitedIds);
    ASSERT_NE(node.GetVisitedCacheRootIds().size(), 0);
}

/**
 * @tc.name: SetDrawingCacheRootIdTest
 * @tc.desc: test SetDrawingCacheRootId api
 * @tc.type: FUNC
 * @tc.require: issueI84LBZ
 */
HWTEST_F(RSRenderNodeTest,  SetDrawingCacheRootIdTest, TestSize.Level2)
{
    RSRenderNode node(id, context);
    RSRenderNode drawingCacheRootNode(id + 1, context);

    node.SetDrawingCacheRootId(drawingCacheRootNode.GetId());
    ASSERT_EQ(node.GetDrawingCacheRootId(), drawingCacheRootNode.GetId());
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
    properties.boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
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
    properties.boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
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
    properties.boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
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
 * @tc.name: GetFilterRectsInCacheTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest, GetFilterRectsInCacheTest, TestSize.Level1)
{
    RSRenderNode node(id, context);
    std::unordered_set<NodeId> curRects = { 0, 1 };
    node.ResetFilterRectsInCache(curRects);
    EXPECT_TRUE(!node.curCacheFilterRects_.empty());
    std::unordered_map<NodeId, std::unordered_set<NodeId>> allRects;
    node.GetFilterRectsInCache(allRects);
    EXPECT_FALSE(allRects.empty());
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
    node.renderContent_->renderProperties_.SetClipBounds(rsPath);
    auto rect1 = node.GetFilterRect();
    EXPECT_TRUE(rect1.ToString().compare("[0, 0, 0, 0]") == 0);

    node.renderContent_->renderProperties_.boundsGeo_ = nullptr;
    auto rect2 = node.GetFilterRect();
    EXPECT_TRUE(rect2.ToString().compare("[0, 0, 0, 0]") == 0);
}

/**
 * @tc.name: CalVisibleFilterRectTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest, CalVisibleFilterRectTest, TestSize.Level1)
{
    RSRenderNode node(id, context);
    RectI prepareClipRect { 1, 1, 1, 1 };
    node.CalVisibleFilterRect(prepareClipRect);
    EXPECT_TRUE(node.filterRegion_.ToString().compare("[0, 0, 0, 0]") == 0);
}

/**
 * @tc.name: OnTreeStateChangedTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest, OnTreeStateChangedTest, TestSize.Level1)
{
    RSRenderNode node(id, context); // isOnTheTree_ false
    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(floatData[0], floatData[1]);
    node.renderContent_->renderProperties_.SetFilter(filter);
    node.OnTreeStateChanged();
    EXPECT_FALSE(node.isOnTheTree_);
    EXPECT_TRUE(node.HasBlurFilter());

    std::shared_ptr<RSRenderNode> inNode = std::make_shared<RSBaseRenderNode>(id, context);
    std::shared_ptr<RSRenderNode> outNode = std::make_shared<RSBaseRenderNode>(id + 1, context);
    auto sharedTransitionParam = std::make_shared<SharedTransitionParam>(inNode, outNode);
    node.SetSharedTransitionParam(sharedTransitionParam);
    node.OnTreeStateChanged();
    EXPECT_FALSE(node.sharedTransitionParam_->paired_);

    node.isOnTheTree_ = true;
    node.OnTreeStateChanged();
    EXPECT_TRUE(node.IsDirty());
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
    node->renderContent_->renderProperties_.clipToBounds_ = true;
    EXPECT_TRUE(node->GetRenderProperties().GetClipToBounds());
    EXPECT_FALSE(node->HasChildrenOutOfRect());
}

/**
 * @tc.name: UpdateVisibleFilterChildTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest, UpdateVisibleFilterChildTest, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(id, context);
    auto childNode = std::make_shared<RSRenderNode>(id + 1, context);
    childNode->GetMutableRenderProperties().needFilter_ = true;
    EXPECT_TRUE(childNode->GetRenderProperties().NeedFilter());
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
    childNode->GetMutableRenderProperties().useEffect_ = true;
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
 * @tc.name: UpdateCompletedCacheSurfaceTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest, UpdateCompletedCacheSurfaceTest, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(id, context);
    auto cacheId = 1;
    auto completeId = 2;
    node->cacheSurfaceThreadIndex_ = cacheId;
    node->completedSurfaceThreadIndex_ = completeId;
    node->UpdateCompletedCacheSurface();
    EXPECT_TRUE(cacheId == node->completedSurfaceThreadIndex_);
}

/**
 * @tc.name: ClearCacheSurfaceTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest, ClearCacheSurfaceTest, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(id, context);
    node->cacheCompletedSurface_ = std::make_shared<Drawing::Surface>();
    node->ClearCacheSurface();
    EXPECT_TRUE(node->cacheCompletedSurface_ == nullptr);
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
 * @tc.name: StoreMustRenewedInfoTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest, StoreMustRenewedInfoTest, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(id, context);
    node->hasHardwareNode_ = true;
    EXPECT_FALSE(node->mustRenewedInfo_);
    node->StoreMustRenewedInfo();
    EXPECT_TRUE(node->mustRenewedInfo_);

    node->mustRenewedInfo_ = false;
    node->hasHardwareNode_ = false;
    node->childHasVisibleFilter_ = true;
    node->StoreMustRenewedInfo();
    EXPECT_TRUE(node->mustRenewedInfo_);

    node->mustRenewedInfo_ = false;
    node->childHasVisibleFilter_ = false;
    node->childHasVisibleEffect_ = true;
    node->StoreMustRenewedInfo();
    EXPECT_TRUE(node->mustRenewedInfo_);
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
 * @tc.name: UpdateCurCornerRadiusTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest, UpdateCurCornerRadiusTest, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(id, context);
    auto maxFloatData = std::numeric_limits<float>::max();
    auto minFloatData = std::numeric_limits<float>::min();
    Vector4f curCornerRadius(floatData[0], floatData[1], floatData[2], minFloatData);
    Vector4f cornerRadius(floatData[0], floatData[1], floatData[2], maxFloatData);
    node->GetMutableRenderProperties().SetCornerRadius(cornerRadius);
    node->UpdateCurCornerRadius(curCornerRadius);
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
 * @tc.name: OnSyncTest
 * @tc.desc: OnSync Test
 * @tc.type: FUNC
 * @tc.require: issueIA5Y41
 */
HWTEST_F(RSRenderNodeTest, OnSyncTest, TestSize.Level1)
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

    node->drawableVec_[static_cast<uint32_t>(RSDrawableSlot::BACKGROUND_FILTER)] = drawableFilter;
    node->drawingCacheType_ = RSDrawingCacheType::FORCED_CACHE;
    node->stagingRenderParams_->freezeFlag_ = true;
    node->needClearSurface_ = true;
    std::function<void()> clearTask = []() { printf("ClearSurfaceTask CallBack\n"); };
    node->isOpincRootFlag_ = true;
    node->OnSync();
    EXPECT_TRUE(node->dirtySlots_.empty());
    EXPECT_FALSE(node->drawCmdListNeedSync_);
    EXPECT_FALSE(node->uifirstNeedSync_);
    EXPECT_FALSE(node->needClearSurface_);
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
    properties.lightSourcePtr_ = std::make_shared<RSLightSource>();
    properties.lightSourcePtr_->intensity_ = floatData[1];
    EXPECT_TRUE(properties.lightSourcePtr_->IsLightSourceValid());
    properties.illuminatedPtr_ = std::make_shared<RSIlluminated>();
    properties.illuminatedPtr_->illuminatedType_ = IlluminatedType::BORDER;
    EXPECT_TRUE(properties.illuminatedPtr_->IsIlluminatedValid());
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
 * @tc.name: GetPairedNodeTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest, GetPairedNodeTest, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> inNode = std::make_shared<RSBaseRenderNode>(id + 1, context);
    std::shared_ptr<RSRenderNode> outNode = std::make_shared<RSBaseRenderNode>(id + 2, context);
    auto sharedTransitionParam = std::make_shared<SharedTransitionParam>(inNode, outNode);
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
 * @tc.name: UpdateHierarchyAndReturnIsLowerTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest, UpdateHierarchyAndReturnIsLowerTest, TestSize.Level1)
{
    auto inNodeId = id + 1;
    auto outNodeId = id + 2;
    auto otherNodeId = id + 3;
    std::shared_ptr<RSRenderNode> inNode = std::make_shared<RSBaseRenderNode>(inNodeId, context);
    std::shared_ptr<RSRenderNode> outNode = std::make_shared<RSBaseRenderNode>(outNodeId, context);
    auto sharedTransitionParam = std::make_shared<SharedTransitionParam>(inNode, outNode);
    sharedTransitionParam->relation_ = SharedTransitionParam::NodeHierarchyRelation::IN_NODE_BELOW_OUT_NODE;
    auto ret = sharedTransitionParam->UpdateHierarchyAndReturnIsLower(inNodeId);
    EXPECT_TRUE(ret);
    sharedTransitionParam->relation_ = SharedTransitionParam::NodeHierarchyRelation::IN_NODE_ABOVE_OUT_NODE;
    ret = sharedTransitionParam->UpdateHierarchyAndReturnIsLower(outNodeId);
    EXPECT_TRUE(ret);
    sharedTransitionParam->relation_ = SharedTransitionParam::NodeHierarchyRelation::UNKNOWN;
    ret = sharedTransitionParam->UpdateHierarchyAndReturnIsLower(otherNodeId);
    EXPECT_FALSE(ret);
    EXPECT_EQ(sharedTransitionParam->relation_, SharedTransitionParam::NodeHierarchyRelation::UNKNOWN);

    ret = sharedTransitionParam->UpdateHierarchyAndReturnIsLower(outNodeId);
    EXPECT_EQ(sharedTransitionParam->relation_, SharedTransitionParam::NodeHierarchyRelation::IN_NODE_ABOVE_OUT_NODE);

    sharedTransitionParam->relation_ = SharedTransitionParam::NodeHierarchyRelation::UNKNOWN;
    ret = sharedTransitionParam->UpdateHierarchyAndReturnIsLower(inNodeId);
    EXPECT_EQ(sharedTransitionParam->relation_, SharedTransitionParam::NodeHierarchyRelation::IN_NODE_BELOW_OUT_NODE);
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
    auto sharedTransitionParam = std::make_shared<SharedTransitionParam>(inNode, outNode);

    (void)sharedTransitionParam->UpdateHierarchyAndReturnIsLower(inNode->GetId());
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
    auto sharedTransitionParam = std::make_shared<SharedTransitionParam>(inNode, outNode);
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
    auto sharedTransitionParam = std::make_shared<SharedTransitionParam>(inNode, outNode);
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
    EXPECT_EQ(nodeTest2->renderContent_->type_, nodeTest2->GetType());
}

/**
 * @tc.name: IsContentNodeTest003
 * @tc.desc: IsContentNode test
 * @tc.type: FUNC
 * @tc.require: issueI9US6V
 */
HWTEST_F(RSRenderNodeTest, IsContentNodeTest003, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);
    nodeTest->renderContent_->drawCmdModifiers_.clear();
    nodeTest->renderContent_->renderProperties_.isDrawn_ = false;
    EXPECT_TRUE(nodeTest->IsContentNode());
    std::shared_ptr<RSRenderProperty<Drawing::DrawCmdListPtr>> property =
        std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>();
    nodeTest->renderContent_->drawCmdModifiers_[RSModifierType::CONTENT_STYLE].emplace_back(
        std::make_shared<RSDrawCmdListRenderModifier>(property));
    EXPECT_TRUE(nodeTest->IsContentNode());
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
 * @tc.name: ParentChildRelationshipTest006
 * @tc.desc: RemoveChild and ResetClearSurfaeFunc test
 * @tc.type: FUNC
 * @tc.require: issueI9US6V
 */
HWTEST_F(RSRenderNodeTest, ParentChildRelationshipTest006, TestSize.Level1)
{
    // RemoveChild test
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);

    std::shared_ptr<RSRenderNode> childTest = nullptr;
    nodeTest->RemoveChild(childTest, false);

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
 * @tc.name: DrawSurfaceNodesTest012
 * @tc.desc: IsFirstLevelNode SubSurfaceNodeNeedDraw test
 * @tc.type: FUNC
 * @tc.require: issueI9US6V
 */
HWTEST_F(RSRenderNodeTest, DrawSurfaceNodesTest012, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);
    nodeTest->firstLevelNodeId_ = 0;
    EXPECT_TRUE(nodeTest->IsFirstLevelNode());

    std::vector<std::weak_ptr<RSRenderNode>> subSurfaceNodesTest1;
    std::shared_ptr<RSSurfaceRenderNode> surfaceTest1 = std::make_shared<RSSurfaceRenderNode>(0);
    EXPECT_NE(surfaceTest1, nullptr);
    surfaceTest1->SetOldDirtyInSurface(RectI { 0, 0, 1, 1 });
    surfaceTest1->visibleRegion_.rects_.emplace_back(RectI { 0, 0, 2, 2 });
    subSurfaceNodesTest1.emplace_back(surfaceTest1);
    nodeTest->subSurfaceNodes_.emplace(0, subSurfaceNodesTest1);
    nodeTest->subSurfaceNodes_.clear();

    std::vector<std::weak_ptr<RSRenderNode>> subSurfaceNodesTest2;
    std::shared_ptr<RSSurfaceRenderNode> surfaceTest2 = std::make_shared<RSSurfaceRenderNode>(0);
    EXPECT_NE(surfaceTest2, nullptr);
    surfaceTest2->SetOldDirtyInSurface(RectI { 0, 0, 1, 1 });
    surfaceTest2->visibleRegion_.rects_.clear();
    std::vector<std::weak_ptr<RSRenderNode>> subSurfaceNodesTest3;
    std::shared_ptr<RSSurfaceRenderNode> surfaceTest3 = std::make_shared<RSSurfaceRenderNode>(0);
    EXPECT_NE(surfaceTest3, nullptr);
    surfaceTest3->SetOldDirtyInSurface(RectI { 0, 0, 1, 1 });
    surfaceTest3->visibleRegion_.rects_.emplace_back(RectI { 0, 0, 2, 2 });
    subSurfaceNodesTest3.emplace_back(surfaceTest3);
    surfaceTest2->subSurfaceNodes_.emplace(0, subSurfaceNodesTest3);
    subSurfaceNodesTest2.emplace_back(surfaceTest2);
    nodeTest->subSurfaceNodes_.emplace(0, subSurfaceNodesTest2);
    nodeTest->subSurfaceNodes_.clear();
}

/**
 * @tc.name: UpdateDrawingCacheInfoBeforeChildrenTest013
 * @tc.desc: CheckDrawingCacheType and UpdateDrawingCacheInfoBeforeChildren test
 * @tc.type: FUNC
 * @tc.require: issueI9US6V
 */
HWTEST_F(RSRenderNodeTest, UpdateDrawingCacheInfoBeforeChildrenTest013, TestSize.Level1)
{
    RSRenderNode node(0);

    // CheckDrawingCacheType test
    node.nodeGroupType_ = RSRenderNode::NONE;
    node.CheckDrawingCacheType();
    EXPECT_EQ(node.GetDrawingCacheType(), RSDrawingCacheType::DISABLED_CACHE);
    node.nodeGroupType_ = RSRenderNode::GROUPED_BY_USER;
    node.CheckDrawingCacheType();
    EXPECT_EQ(node.GetDrawingCacheType(), RSDrawingCacheType::FORCED_CACHE);
    node.nodeGroupType_ = RSRenderNode::GROUPED_BY_ANIM;
    node.CheckDrawingCacheType();
    EXPECT_EQ(node.GetDrawingCacheType(), RSDrawingCacheType::TARGETED_CACHE);

    // UpdateDrawingCacheInfoBeforeChildren test
    // shouldPaint_ is false
    node.shouldPaint_ = false;
    node.UpdateDrawingCacheInfoBeforeChildren(false);
    EXPECT_EQ(node.GetDrawingCacheType(), RSDrawingCacheType::DISABLED_CACHE);
    // shouldPaint_ is true
    node.shouldPaint_ = true;
    node.UpdateDrawingCacheInfoBeforeChildren(true);
    EXPECT_EQ(node.GetDrawingCacheType(), RSDrawingCacheType::DISABLED_CACHE);
    // isScreenRotation is true
    node.nodeGroupType_ = RSRenderNode::NONE;
    node.UpdateDrawingCacheInfoBeforeChildren(false);
    EXPECT_EQ(node.GetDrawingCacheType(), RSDrawingCacheType::DISABLED_CACHE);

    // isScreenRotation is false
    node.nodeGroupType_ = RSRenderNode::GROUPED_BY_ANIM;
    std::unique_ptr<RSRenderParams> stagingRenderParams = std::make_unique<RSRenderParams>(0);
    EXPECT_NE(stagingRenderParams, nullptr);
    node.stagingRenderParams_ = std::move(stagingRenderParams);
    node.UpdateDrawingCacheInfoBeforeChildren(false);
    EXPECT_FALSE(node.stagingRenderParams_->needSync_);
}

/**
 * @tc.name: RemoveModifierTest014
 * @tc.desc: RemoveModifier test
 * @tc.type: FUNC
 * @tc.require: issueI9US6V
 */
HWTEST_F(RSRenderNodeTest, ProcessTest014, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);

    std::shared_ptr<RSRenderProperty<Drawing::DrawCmdListPtr>> propertyTest1 =
        std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>();
    std::shared_ptr<RSDrawCmdListRenderModifier> modifier1 =
        std::make_shared<RSDrawCmdListRenderModifier>(propertyTest1);
    std::shared_ptr<RSRenderProperty<Drawing::DrawCmdListPtr>> propertyTest2 =
        std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>();
    std::shared_ptr<RSDrawCmdListRenderModifier> modifier2 =
        std::make_shared<RSDrawCmdListRenderModifier>(propertyTest2);

    nodeTest->modifiers_.emplace(0, modifier1);
    nodeTest->modifiers_.emplace(1, modifier2);

    EXPECT_EQ(nodeTest->modifiers_.size(), 2);
    nodeTest->RemoveModifier(1);
    EXPECT_EQ(nodeTest->modifiers_.size(), 1);
    nodeTest->RemoveModifier(1);
    EXPECT_EQ(nodeTest->modifiers_.size(), 1);
}

/**
 * @tc.name: AccmulateDirtyInOcclusionTest015
 * @tc.desc: AccmulateDirtyInOcclusion
 * @tc.type: FUNC
 * @tc.require: issueI9US6V
 */
HWTEST_F(RSRenderNodeTest, AccmulateDirtyInOcclusionTest015, TestSize.Level1)
{
    // AccmulateDirtyInOcclusion AccmulateDirtyTypes AccmulateDirtyStatus GetMutableRenderProperties
    // ResetAccmulateDirtyTypes ResetAccmulateDirtyStatus RecordCurDirtyTypes test
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);
    nodeTest->curDirtyTypes_.set(static_cast<size_t>(RSModifierType::PIVOT_Z), true);
    nodeTest->curDirtyTypes_.set(static_cast<size_t>(RSModifierType::PERSP), true);

    nodeTest->AccmulateDirtyInOcclusion(true);
    nodeTest->AccmulateDirtyInOcclusion(false);

    nodeTest->dirtyTypes_.set(static_cast<size_t>(RSModifierType::PIVOT_Z), true);
    nodeTest->dirtyTypes_.set(static_cast<size_t>(RSModifierType::PERSP), true);
    nodeTest->RecordCurDirtyTypes();
}

/**
 * @tc.name: GenerateFullChildrenListTest016
 * @tc.desc: RemoveModifier and ApplyPositionZModifier test
 * @tc.type: FUNC
 * @tc.require: issueI9US6V
 */
HWTEST_F(RSRenderNodeTest, GenerateFullChildrenListTest016, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);

    nodeTest->isFullChildrenListValid_ = false;
    nodeTest->GenerateFullChildrenList();
    EXPECT_TRUE(nodeTest->isFullChildrenListValid_);

    std::shared_ptr<RSRenderNode> childrenTest1 = nullptr;
    std::shared_ptr<RSRenderNode> childrenTest2 = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(childrenTest2, nullptr);
    std::shared_ptr<RSRenderNode> childrenTest3 = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(childrenTest3, nullptr);
    nodeTest->children_.emplace_back(childrenTest1);
    nodeTest->isContainBootAnimation_ = true;
    childrenTest2->isBootAnimation_ = false;
    childrenTest2->isBootAnimation_ = true;
    nodeTest->children_.emplace_back(childrenTest2);

    std::shared_ptr<RSRenderNode> disappearingChildrenTest1 = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(disappearingChildrenTest1, nullptr);
    std::shared_ptr<RSRenderNode> disappearingChildrenTest2 = std::make_shared<RSRenderNode>(1);
    EXPECT_NE(disappearingChildrenTest2, nullptr);
    disappearingChildrenTest1->isBootAnimation_ = false;
    nodeTest->disappearingChildren_.emplace_back(std::pair(disappearingChildrenTest1, 0));
    nodeTest->disappearingChildren_.emplace_back(std::pair(disappearingChildrenTest2, 1));

    // ApplyPositionZModifier test
    disappearingChildrenTest1->dirtyTypes_.set(static_cast<size_t>(RSModifierType::POSITION_Z), false);
    std::shared_ptr<RSRenderProperty<Drawing::DrawCmdListPtr>> propertyTest1 =
        std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>();
    EXPECT_NE(propertyTest1, nullptr);
    std::shared_ptr<RSDrawCmdListRenderModifier> modifier1 =
        std::make_shared<RSDrawCmdListRenderModifier>(propertyTest1);
    EXPECT_NE(modifier1, nullptr);
    modifier1->SetType(RSModifierType::POSITION_Z);
    disappearingChildrenTest2->modifiers_.emplace(0, modifier1);

    nodeTest->isChildrenSorted_ = false;
    nodeTest->GenerateFullChildrenList();
    EXPECT_TRUE(nodeTest->isChildrenSorted_);

    disappearingChildrenTest2->dirtyTypes_.set(static_cast<size_t>(RSModifierType::POSITION_Z), true);
    nodeTest->isChildrenSorted_ = false;
    nodeTest->GenerateFullChildrenList();
    EXPECT_TRUE(nodeTest->isChildrenSorted_);
}

/**
 * @tc.name: ApplyModifiersTest017
 * @tc.desc: ApplyModifiers test
 * @tc.type: FUNC
 * @tc.require: issueI9US6V
 */
HWTEST_F(RSRenderNodeTest, ApplyModifiersTest017, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);
    std::shared_ptr<RSRenderProperty<Drawing::DrawCmdListPtr>> propertyTest1 =
        std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>();
    EXPECT_NE(propertyTest1, nullptr);
    std::shared_ptr<RSDrawCmdListRenderModifier> modifier1 =
        std::make_shared<RSDrawCmdListRenderModifier>(propertyTest1);
    EXPECT_NE(modifier1, nullptr);
    modifier1->SetType(RSModifierType::POSITION_Z);
    nodeTest->dirtyTypes_.set(static_cast<size_t>(RSModifierType::POSITION_Z), false);
    nodeTest->dirtyTypes_.set(static_cast<size_t>(RSModifierType::SANDBOX), true);
    nodeTest->modifiers_.emplace(0, modifier1);
    nodeTest->isFullChildrenListValid_ = false;
    nodeTest->renderContent_->renderProperties_.alpha_ = 0.0f;
    RSRenderNode::SharedPtr inNode = std::make_shared<RSRenderNode>(0);
    RSRenderNode::SharedPtr outNode = std::make_shared<RSRenderNode>(0);
    std::shared_ptr<SharedTransitionParam> sharedTransitionParam =
        std::make_shared<SharedTransitionParam>(inNode, outNode);
    nodeTest->sharedTransitionParam_ = sharedTransitionParam;
    nodeTest->GenerateFullChildrenList();
    EXPECT_TRUE(nodeTest->isFullChildrenListValid_);

    nodeTest->modifiers_.clear();
    nodeTest->isChildrenSorted_ = false;
    nodeTest->GenerateFullChildrenList();

    nodeTest->isChildrenSorted_ = true;
    nodeTest->childrenHasSharedTransition_ = true;
    nodeTest->GenerateFullChildrenList();

    nodeTest->dirtyTypes_.reset();
    EXPECT_TRUE(nodeTest->dirtyTypes_.none());
    nodeTest->childrenHasSharedTransition_ = false;
    nodeTest->GenerateFullChildrenList();
}

/**
 * @tc.name: InvalidateHierarchyTest018
 * @tc.desc: MarkParentNeedRegenerateChildren and UpdateDrawableVec UpdateDrawableVecInternal test
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

    nodeTest1->SetIsUsedBySubThread(false);
    nodeTest1->renderContent_->renderProperties_.pixelStretch_ = 1.0f;
    nodeTest1->dirtyTypes_.set(static_cast<size_t>(RSModifierType::BOUNDS), true);
    nodeTest1->dirtyTypes_.set(static_cast<size_t>(RSModifierType::SCALE), true);
    std::unique_ptr<PropertyDrawableTest> property = std::make_unique<PropertyDrawableTest>();
    nodeTest1->renderContent_->propertyDrawablesVec_.at(0) = std::move(property);
    nodeTest1->UpdateDrawableVec();

    std::shared_ptr<RSRenderNode> nodeTest2 = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest2, nullptr);
    nodeTest2->SetIsUsedBySubThread(true);
    std::shared_ptr<RSContext> contextTest = std::make_shared<RSContext>();
    nodeTest2->context_ = contextTest;
    nodeTest2->UpdateDrawableVec();
}

/**
 * @tc.name: UpdateDrawableVecV2Test019
 * @tc.desc: UpdateDrawableVecV2 test
 * @tc.type: FUNC
 * @tc.require: issueI9US6V
 */
HWTEST_F(RSRenderNodeTest, UpdateDrawableVecV2Test019, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);

    nodeTest->UpdateDrawableVecV2();

    nodeTest->dirtyTypes_.set(static_cast<size_t>(RSModifierType::BOUNDS), true);
    nodeTest->dirtyTypes_.set(static_cast<size_t>(RSModifierType::ROTATION_X), true);
    std::shared_ptr<DrawableTest> drawableTest1 = std::make_shared<DrawableTest>();
    nodeTest->drawableVec_.at(1) = drawableTest1;
    EXPECT_TRUE(nodeTest->dirtySlots_.empty());

    nodeTest->stagingRenderParams_ = std::make_unique<RSRenderParams>(0);
    nodeTest->UpdateDrawableVecV2();
    auto sum = nodeTest->dirtySlots_.size();
    EXPECT_NE(nodeTest->dirtySlots_.size(), 0);

    nodeTest->dirtyTypes_.set(static_cast<size_t>(RSModifierType::PIVOT), true);
    std::shared_ptr<DrawableTest> drawableTest2 = std::make_shared<DrawableTest>();
    nodeTest->drawableVec_.at(4) = drawableTest2;
    RSShadow rsShadow;
    std::optional<RSShadow> shadow(rsShadow);
    shadow->colorStrategy_ = SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_AVERAGE;
    nodeTest->renderContent_->renderProperties_.shadow_ = shadow;
    RRect rrect;
    nodeTest->renderContent_->renderProperties_.rrect_ = rrect;
    nodeTest->UpdateDrawableVecV2();
    EXPECT_NE(nodeTest->dirtySlots_.size(), sum);
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
    std::unique_ptr<RSRenderParams> stagingRenderParams = std::make_unique<RSRenderParams>(0);
    EXPECT_NE(stagingRenderParams, nullptr);
    nodeTest->stagingRenderParams_ = std::move(stagingRenderParams);
    nodeTest->drawCmdListNeedSync_ = false;
    nodeTest->UpdateDisplayList();
    EXPECT_TRUE(nodeTest->drawCmdListNeedSync_);

    nodeTest->drawableVecStatus_ = 48;
    std::shared_ptr<DrawableTest> drawableTest1 = std::make_shared<DrawableTest>();
    nodeTest->drawableVec_.at(1) = drawableTest1;
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
    nodeTest->renderContent_->renderProperties_.useEffect_ = true;
    std::shared_ptr<RSObjAbsGeometry> boundsGeo = std::make_shared<RSObjAbsGeometry>();
    nodeTest->renderContent_->renderProperties_.boundsGeo_ = boundsGeo;
    nodeTest->UpdateEffectRegion(region, true);

    // GetModifier test
    std::shared_ptr<RSDrawCmdListRenderModifier> modifier = nullptr;
    nodeTest->modifiers_.emplace(0, modifier);
    EXPECT_EQ(nodeTest->GetModifier(0), nullptr);
    EXPECT_EQ(nodeTest->GetModifier(1), nullptr);
    std::shared_ptr<RSRenderProperty<Drawing::DrawCmdListPtr>> propertyTest =
        std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>();
    EXPECT_NE(propertyTest, nullptr);
    std::shared_ptr<RSDrawCmdListRenderModifier> drawCmdModifiersTest =
        std::make_shared<RSDrawCmdListRenderModifier>(propertyTest);
    EXPECT_NE(drawCmdModifiersTest, nullptr);
    drawCmdModifiersTest->property_->id_ = 1;
    nodeTest->renderContent_->drawCmdModifiers_[RSModifierType::BOUNDS].emplace_back(drawCmdModifiersTest);
    EXPECT_NE(nodeTest->GetModifier(1), nullptr);

    // FilterModifiersByPid test
    nodeTest->FilterModifiersByPid(1);

    // UpdateShouldPaint test
    nodeTest->renderContent_->renderProperties_.alpha_ = -1.0f;
    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(0.0f, 0.1f);
    nodeTest->renderContent_->renderProperties_.filter_ = filter;
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
    sharedTransitionParam = std::make_shared<SharedTransitionParam>(inNode, outNode);
    EXPECT_NE(sharedTransitionParam, nullptr);
    nodeTest->parent_ = std::make_shared<RSRenderNode>(0);
    nodeTest->SetSharedTransitionParam(sharedTransitionParam);
    EXPECT_NE(nodeTest->sharedTransitionParam_, nullptr);
}

/**
 * @tc.name: ManageRenderingResourcesTest022
 * @tc.desc: SetGlobalAlpha NeedInitCacheSurface GetOptionalBufferSize test
 * @tc.type: FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSRenderNodeTest, ManageRenderingResourcesTest022, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);
    nodeTest->InitRenderParams();
    // SetGlobalAlpha test
    nodeTest->SetGlobalAlpha(1.0f);
    EXPECT_EQ(nodeTest->globalAlpha_, 1.0f);
    nodeTest->SetGlobalAlpha(1.01f);
    EXPECT_EQ(nodeTest->globalAlpha_, 1.01f);

    // NeedInitCacheSurface and GetOptionalBufferSize test
    nodeTest->SetCacheType(CacheType::ANIMATE_PROPERTY);
    RSShadow rsShadow;
    std::optional<RSShadow> shadow(rsShadow);
    nodeTest->renderContent_->renderProperties_.shadow_ = shadow;
    nodeTest->renderContent_->renderProperties_.shadow_->radius_ = 1.0f;
    nodeTest->renderContent_->renderProperties_.isSpherizeValid_ = true;
    nodeTest->renderContent_->renderProperties_.isAttractionValid_ = true;
    nodeTest->cacheSurface_ = nullptr;
    EXPECT_TRUE(nodeTest->NeedInitCacheSurface());

    nodeTest->boundsModifier_ = nullptr;
    nodeTest->frameModifier_ = nullptr;
    nodeTest->SetCacheType(CacheType::CONTENT);
    EXPECT_TRUE(nodeTest->NeedInitCacheSurface());

    nodeTest->boundsModifier_ = nullptr;
    auto propertyTest = std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>();
    EXPECT_NE(propertyTest, nullptr);
    auto frameModifier = std::make_shared<RSDrawCmdListRenderModifier>(propertyTest);
    EXPECT_NE(frameModifier, nullptr);
    frameModifier->property_ = std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>();
    EXPECT_NE(frameModifier->property_, nullptr);
    frameModifier->property_->GetRef() = std::make_shared<Drawing::DrawCmdList>(10, 10);
    frameModifier->SetType(RSModifierType::FRAME);
    nodeTest->frameModifier_ = frameModifier;
    nodeTest->cacheSurface_ = std::make_shared<Drawing::Surface>();
    EXPECT_NE(nodeTest->cacheSurface_, nullptr);
    nodeTest->cacheSurface_->cachedCanvas_ = nullptr;
    EXPECT_TRUE(nodeTest->NeedInitCacheSurface());
    nodeTest->cacheSurface_->cachedCanvas_ = std::make_shared<Drawing::Canvas>();
    EXPECT_NE(nodeTest, nullptr);
}

/**
 * @tc.name: NeedInitCacheCompletedSurfaceTest023
 * @tc.desc: NeedInitCacheCompletedSurface test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest, NeedInitCacheCompletedSurfaceTest023, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);

    // NeedInitCacheCompletedSurface test
    nodeTest->boundsModifier_ = nullptr;
    nodeTest->frameModifier_ = nullptr;
    nodeTest->cacheCompletedSurface_ = nullptr;
    EXPECT_TRUE(nodeTest->NeedInitCacheCompletedSurface());
    nodeTest->cacheCompletedSurface_ = std::make_shared<Drawing::Surface>();
    EXPECT_NE(nodeTest->cacheCompletedSurface_, nullptr);
    nodeTest->cacheCompletedSurface_->cachedCanvas_ = nullptr;
    EXPECT_TRUE(nodeTest->NeedInitCacheCompletedSurface());
    nodeTest->cacheCompletedSurface_->cachedCanvas_ = std::make_shared<Drawing::Canvas>();
    EXPECT_NE(nodeTest->cacheCompletedSurface_->cachedCanvas_, nullptr);
    EXPECT_FALSE(nodeTest->NeedInitCacheCompletedSurface());
}

/**
 * @tc.name: InitCacheSurfaceTest024
 * @tc.desc: InitCacheSurface test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest, InitCacheSurfaceTest024, TestSize.Level1)
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
    nodeTest->renderContent_->renderProperties_.shadow_ = shadow;
    nodeTest->renderContent_->renderProperties_.shadow_->radius_ = 1.0f;
    nodeTest->renderContent_->renderProperties_.isSpherizeValid_ = false;
    nodeTest->renderContent_->renderProperties_.isAttractionValid_ = false;
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
 * @tc.name: DrawCacheSurfaceTest025
 * @tc.desc: DrawCacheSurface test
 * @tc.type: FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSRenderNodeTest, DrawCacheSurfaceTest025, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);
    nodeTest->boundsModifier_ = nullptr;
    nodeTest->frameModifier_ = nullptr;
    Drawing::Canvas canvasTest1;
    std::shared_ptr<Drawing::GPUContext> gpuContextTest1 = std::make_shared<Drawing::GPUContext>();
    canvasTest1.gpuContext_ = gpuContextTest1;
    RSPaintFilterCanvas paintFilterCanvasTest1(&canvasTest1);

    nodeTest->boundsWidth_ = 0.0f;
    nodeTest->DrawCacheSurface(paintFilterCanvasTest1, 0, true);
    nodeTest->boundsWidth_ = 10.0f;
    nodeTest->DrawCacheSurface(paintFilterCanvasTest1, 0, false);

    nodeTest->boundsWidth_ = 10.0f;
    nodeTest->boundsHeight_ = 10.0f;
    nodeTest->cacheCompletedBackendTexture_.isValid_ = false;
    paintFilterCanvasTest1.canvas_->paintBrush_.hasFilter_ = true;
    nodeTest->DrawCacheSurface(paintFilterCanvasTest1, 0, true);
    EXPECT_TRUE(paintFilterCanvasTest1.canvas_->paintBrush_.hasFilter_);

    // RSSystemPrperties:GetRecordongEnabled() is false
    nodeTest->cacheCompletedBackendTexture_.isValid_ = true;
    RSShadow rsShadow;
    std::optional <RSShadow>shadow(rsShadow);
    nodeTest->renderContent_->renderProperties_.shadow_ = shadow;
    nodeTest->renderContent_->renderProperties_.shadow_->radius_ = 1.0f;
    nodeTest->cacheType_ = CacheType::ANIMATE_PROPERTY;
    Drawing::Canvas canvasTest2;
    std::shared_ptr<Drawing::GPUContext> gpuContextTest2 = std::make_shared<Drawing::GPUContext>();
    canvasTest2.gpuContext_ = gpuContextTest2;
    RSPaintFilterCanvas paintFilterCanvasTest2(&canvasTest2);
    std::shared_ptr<Drawing::SkiaCanvas> implTest1 = std::make_shared<Drawing::SkiaCanvas>();
    implTest1->skCanvas_ = nullptr;
    paintFilterCanvasTest2.canvas_->impl_ = implTest1;
    paintFilterCanvasTest2.canvas_->paintBrush_.hasFilter_ = true;
    nodeTest->DrawCacheSurface(paintFilterCanvasTest2, 0, true);
    EXPECT_NE(paintFilterCanvasTest2.canvas_, nullptr);

    // RSSystemPrperties::GetRecordongEnabled() is false
    // cacheCompletedSurface_->GetImageSnapshot() and RSSystemProperties::GetRecordingEnabled() is false
    // so isUIFirst only is true

    nodeTest->cacheType_ = CacheType::CONTENT;
    Drawing::Canvas canvasTest3;
    std::shared_ptr<Drawing::GPUContext> gpuContextTest3 = std::make_shared<Drawing::GPUContext>();
    canvasTest3.gpuContext_ = gpuContextTest3;
    RSPaintFilterCanvas paintFilterCanvasTest3(&canvasTest3);
    std::shared_ptr<Drawing::SkiaCanvas> implTest2 = std::make_shared<Drawing::SkiaCanvas>();
    implTest2->skCanvas_ = nullptr;
    paintFilterCanvasTest3.canvas_->impl_ = implTest2;
    paintFilterCanvasTest3.canvas_->paintBrush_.hasFilter_ = true;
    nodeTest->DrawCacheSurface(paintFilterCanvasTest3, 0, true);
    EXPECT_NE(paintFilterCanvasTest3.canvas_, nullptr);
}

/**
 * @tc.name: GetCompletedImageTest026
 * @tc.desc: GetCompletedImage test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest, GetCompletedImageTest026, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);

    Drawing::Canvas canvastest;
    std::shared_ptr<Drawing::GPUContext> gpuContext = std::make_shared<Drawing::GPUContext>();
    EXPECT_NE(gpuContext, nullptr);
    canvastest.gpuContext_ = gpuContext;
    RSPaintFilterCanvas canvas(&canvastest);

    std::shared_ptr<Drawing::Surface> cacheCompletedSurface = std::make_shared<Drawing::Surface>();
    EXPECT_NE(cacheCompletedSurface, nullptr);
    nodeTest->cacheCompletedSurface_ = cacheCompletedSurface;

    nodeTest->cacheCompletedBackendTexture_.isValid_ = false;
    EXPECT_EQ(nodeTest->GetCompletedImage(canvas, 0, true), nullptr);

    nodeTest->cacheCompletedBackendTexture_.isValid_ = true;
#ifdef RS_ENABLE_VK
    // nullptr as cacheCompletedCleanupHelper_ is false
    nodeTest->GetCompletedImage(canvas, 0, true);
#else
    EXPECT_NE(nodeTest->GetCompletedImage(canvas, 0, true), nullptr);
#endif

    // cacheCompletedSurface_->GetImageSnapshot() is false
    EXPECT_EQ(nodeTest->GetCompletedImage(canvas, 0, false), nullptr);

    nodeTest->cacheCompletedSurface_ = nullptr;
    EXPECT_EQ(nodeTest->GetCompletedImage(canvas, 0, false), nullptr);
}

/**
 * @tc.name: ManageCachingTest027
 * @tc.desc: GetCompletedCacheSurface GetCacheSurface MarkNodeGroup test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest, ManageCachingTest027, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);
    nodeTest->InitRenderParams();
    // GetCompletedCacheSurface test
    nodeTest->cacheCompletedSurface_ = nullptr;
    EXPECT_EQ(nodeTest->GetCompletedCacheSurface(0, true, true), nullptr);
    EXPECT_EQ(nodeTest->GetCompletedCacheSurface(0, true, false), nullptr);
    std::shared_ptr<Drawing::Surface> cacheCompletedSurface = std::make_shared<Drawing::Surface>();
    EXPECT_NE(cacheCompletedSurface, nullptr);
    nodeTest->cacheCompletedSurface_ = cacheCompletedSurface;

    EXPECT_NE(nodeTest->cacheCompletedSurface_, nullptr);
    EXPECT_NE(nodeTest->cacheCompletedSurface_, nullptr);
    EXPECT_EQ(nodeTest->GetCompletedCacheSurface(0, true, false), nullptr);
    EXPECT_EQ(nodeTest->GetCompletedCacheSurface(0, false, false), nullptr);
    nodeTest->completedSurfaceThreadIndex_ = 0;
    EXPECT_EQ(nodeTest->GetCompletedCacheSurface(0, true, false), nullptr);
    // GetCacheSurface test
    nodeTest->cacheSurface_ = nullptr;
    EXPECT_EQ(nodeTest->GetCacheSurface(0, true, true), nullptr);
    EXPECT_EQ(nodeTest->GetCacheSurface(0, true, false), nullptr);
    std::shared_ptr<Drawing::Surface> cacheSurface = std::make_shared<Drawing::Surface>();
    nodeTest->cacheSurface_ = cacheSurface;
    EXPECT_NE(nodeTest->cacheSurface_, nullptr);
    EXPECT_EQ(nodeTest->GetCacheSurface(0, true, false), nullptr);
    EXPECT_EQ(nodeTest->GetCacheSurface(0, false, false), nullptr);
    nodeTest->completedSurfaceThreadIndex_ = 0;
    EXPECT_EQ(nodeTest->GetCacheSurface(0, true, false), nullptr);

    // MarkNodeGroup test
    nodeTest->nodeGroupIncludeProperty_ = false;
    nodeTest->nodeGroupType_ = RSRenderNode::NONE;
    std::shared_ptr<RSContext> contextTest = std::make_shared<RSContext>();
    contextTest->nodeMap.residentSurfaceNodeMap_.emplace(
        std::make_pair(0, std::make_shared<RSSurfaceRenderNode>(0, std::make_shared<RSContext>())));
    nodeTest->context_ = contextTest;
    nodeTest->MarkNodeGroup(RSRenderNode::GROUPED_BY_UI, true, true);
    EXPECT_EQ(nodeTest->nodeGroupType_, RSRenderNode::GROUPED_BY_UI);
    EXPECT_TRUE(nodeTest->nodeGroupIncludeProperty_);

    nodeTest->MarkNodeGroup(RSRenderNode::NONE, true, true);
    EXPECT_TRUE(nodeTest->nodeGroupIncludeProperty_);
    nodeTest->MarkNodeGroup(RSRenderNode::NONE, false, true);
    EXPECT_TRUE(nodeTest->nodeGroupIncludeProperty_);
}

/**
 * @tc.name: CheckGroupableAnimationTest028
 * @tc.desc: CheckGroupableAnimation test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest, CheckGroupableAnimationTest028, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);

    nodeTest->CheckGroupableAnimation(0, true);

    nodeTest->CheckGroupableAnimation(1, true);

    std::shared_ptr<RSCanvasRenderNode> canvasRenderNodeTest = std::make_shared<RSCanvasRenderNode>(0);
    EXPECT_NE(canvasRenderNodeTest, nullptr);
    std::shared_ptr<RSContext> context = nullptr;
    nodeTest->context_ = context;
    // RSSystemProperties::GetAnimationCacheEnabled() is false
    canvasRenderNodeTest->CheckGroupableAnimation(1, true);
}

/**
 * @tc.name: ClearCacheSurfaceInThreadTest029
 * @tc.desc: ClearCacheSurfaceInThread test
 * @tc.type: FUNC
 * @tc.require: issueI9V3BK
 */
HWTEST_F(RSRenderNodeTest, ClearCacheSurfaceInThreadTest029, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);

    std::shared_ptr<Drawing::Surface> surfaceTest1 = std::make_shared<Drawing::Surface>();
    EXPECT_NE(surfaceTest1, nullptr);
    std::shared_ptr<Drawing::Surface> surfaceTest2 = std::make_shared<Drawing::Surface>();
    EXPECT_NE(surfaceTest2, nullptr);
    nodeTest->clearCacheSurfaceFunc_ = [surfaceTest1, surfaceTest2](std::shared_ptr<Drawing::Surface>&& s1,
                                           std::shared_ptr<Drawing::Surface>&& s2, uint32_t w, uint32_t h) {};
    nodeTest->ClearCacheSurfaceInThread();
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
    filterDrawable->stagingForceClearCache_ = true;
    filterDrawable->stagingForceUseCache_ = true;
    filterDrawable->pendingPurge_ = true;
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
    filterDrawable->stagingForceClearCache_ = true;
    filterDrawable->stagingForceUseCache_ = true;
    filterDrawable->stagingFilterInteractWithDirty_ = true;
    filterDrawable->cacheUpdateInterval_ = 1;
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
 * @tc.name: MarkForceClearFilterCacheWithInvisible
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest, MarkForceClearFilterCacheWithInvisible, TestSize.Level1)
{
    RSRenderNode node(id, context);
    auto& properties = node.GetMutableRenderProperties();
    properties.backgroundFilter_ = std::make_shared<RSFilter>();
    properties.filter_ = std::make_shared<RSFilter>();
    node.MarkForceClearFilterCacheWithInvisible();
    ASSERT_TRUE(true);
}

/**
 * @tc.name: SetOccludedStatus
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest, SetOccludedStatus, TestSize.Level1)
{
    RSRenderNode node(id, context);
    auto& properties = node.GetMutableRenderProperties();
    properties.backgroundFilter_ = std::make_shared<RSFilter>();
    properties.filter_ = std::make_shared<RSFilter>();
    bool occluded = true;
    node.SetOccludedStatus(occluded);
    ASSERT_TRUE(true);
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
 * @tc.name: ApplyBoundsGeometry
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest, ApplyBoundsGeometry, TestSize.Level1)
{
    RSRenderNode node(id, context);
    node.ApplyBoundsGeometry(*canvas_);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: ApplyAlpha
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeTest, ApplyAlpha, TestSize.Level1)
{
    RSRenderNode node(id, context);
    node.ApplyAlpha(*canvas_);
    ASSERT_TRUE(true);
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
    canvasNode->InitRenderParams();
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
    canvasNode->InitRenderParams();
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

} // namespace Rosen
} // namespace OHOS
