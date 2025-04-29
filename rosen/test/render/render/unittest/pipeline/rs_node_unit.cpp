/*
* Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include "pipeline/rs_root_render_node.h"
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
    "Children list needs update, current count: 0 expected count: 0, disappearingChildren: 1(0 )\n"
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

class RSNodeUnitTest : public testing::Test {
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

void RSNodeUnitTest::SetUpTestCase()
{
    canvas_ = new RSPaintFilterCanvas(&drawingCanvas_);
}
void RSNodeUnitTest::TearDownTestCase()
{
    delete canvas_;
    canvas_ = nullptr;
}
void RSNodeUnitTest::SetUp() {}
void RSNodeUnitTest::TearDown() {}

/**
 * @tc.name: UpdateRenderStatus001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSNodeUnitTest, UpdateRenderStatus001, TestSize.Level1)
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
HWTEST_F(RSNodeUnitTest, UpdateParentChildrenRectTest, TestSize.Level1)
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
HWTEST_F(RSNodeUnitTest, ProcessTransitionBeforeChildrenTest, TestSize.Level1)
{
    RSRenderNode node(id, context);
    node.ProcessTransitionBeforeChildren(*canvas_);
    ASSERT_TRUE(canvas_->GetRecordingCanvas() == nullptr);
}

/**
 * @tc.name: ProcessTransitionBeforeChildrenTest
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSNodeUnitTest, AddModifierTest, TestSize.Level1)
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
HWTEST_F(RSNodeUnitTest, InitCacheSurfaceTest, TestSize.Level1)
{
    RSRenderNode node(id, context);
    ASSERT_NE(node.GetRenderContent(), nullptr);
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
HWTEST_F(RSNodeUnitTest, DrawCacheSurfaceTest001, TestSize.Level1)
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
HWTEST_F(RSNodeUnitTest, DrawCacheSurfaceTest002, TestSize.Level1)
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
HWTEST_F(RSNodeUnitTest, SetDrawingCacheTypeTest, TestSize.Level2)
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
HWTEST_F(RSNodeUnitTest, ResetFilterRectsInCacheTest, TestSize.Level2)
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
HWTEST_F(RSNodeUnitTest, ManageDrawingCacheTest001, TestSize.Level2)
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
HWTEST_F(RSNodeUnitTest, ResetDrawingCacheNeedUpdateTest001, TestSize.Level2)
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
HWTEST_F(RSNodeUnitTest, ResetDrawingCacheNeedUpdateTest002, TestSize.Level2)
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
HWTEST_F(RSNodeUnitTest, SetVisitedCacheRootIdsTest, TestSize.Level2)
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
HWTEST_F(RSNodeUnitTest,  SetDrawingCacheRootIdTest, TestSize.Level2)
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
HWTEST_F(RSNodeUnitTest,  SetGeoUpdateDelay01, TestSize.Level2)
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
HWTEST_F(RSNodeUnitTest,  SetGeoUpdateDelay02, TestSize.Level2)
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
HWTEST_F(RSNodeUnitTest,  ResetGeoUpdateDelay01, TestSize.Level2)
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
HWTEST_F(RSNodeUnitTest, SetBootAnimationTest, TestSize.Level1)
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
HWTEST_F(RSNodeUnitTest, OnlyBasicGeoTransfromTest01, TestSize.Level1)
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
HWTEST_F(RSNodeUnitTest, OnlyBasicGeoTransfromTest02, TestSize.Level1)
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
HWTEST_F(RSNodeUnitTest, OnlyBasicGeoTransfromTest03, TestSize.Level1)
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
HWTEST_F(RSNodeUnitTest, UpdateDirtyRegionTest01, TestSize.Level1)
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
HWTEST_F(RSNodeUnitTest, UpdateDirtyRegionTest02, TestSize.Level1)
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
HWTEST_F(RSNodeUnitTest, UpdateDirtyRegionTest03, TestSize.Level1)
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
HWTEST_F(RSNodeUnitTest, UpdateDirtyRegionTest04, TestSize.Level1)
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
HWTEST_F(RSNodeUnitTest, UpdateDirtyRegionTest05, TestSize.Level1)
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
 * @tc.name: UpdateDirtyRegionTest06
 * @tc.desc: check outline dirty region add successfully
 * @tc.type: FUNC
 * @tc.require: issueI97LXT
 */
HWTEST_F(RSNodeUnitTest, UpdateDirtyRegionTest06, TestSize.Level1)
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
    properties.boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
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
HWTEST_F(RSNodeUnitTest, UpdateDirtyRegionTest07, TestSize.Level1)
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
    properties.boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
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
HWTEST_F(RSNodeUnitTest, UpdateDirtyRegionTest08, TestSize.Level1)
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
    properties.boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
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
 * @tc.name: GetFilterRectsInCacheTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSNodeUnitTest, GetFilterRectsInCacheTest, TestSize.Level1)
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
HWTEST_F(RSNodeUnitTest, GetFilterRectTest, TestSize.Level1)
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
 * @tc.name: CalVisibleFilterRect
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSNodeUnitTest, CalVisibleFilterRectTest, TestSize.Level1)
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
HWTEST_F(RSNodeUnitTest, OnTreeStateChangedTest, TestSize.Level1)
{
    RSRenderNode node(id, context); // isOnTheTree_ false
    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(floatData[0], floatData[1]);
    node.renderContent_->renderProperties_.SetFilter(filter);
    node.OnTreeStateChanged();
    EXPECT_FALSE(node.isOnTheTree_);
    EXPECT_TRUE(node.HasBlurFilter());

    std::shared_ptr<RSRenderNode> inNode = std::make_shared<RSBaseRenderNode>(id, context);
    std::shared_ptr<RSRenderNode> outNode = std::make_shared<RSBaseRenderNode>(id + 1, context);
    auto sharedTransitionParam = std::make_shared<SharedTransitionParam>(inNode, outNode, true);
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
HWTEST_F(RSNodeUnitTest, HasDisappearingTransitionTest, TestSize.Level1)
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
HWTEST_F(RSNodeUnitTest, GenerateFullChildrenListTest, TestSize.Level1)
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
HWTEST_F(RSNodeUnitTest, ResortChildrenTest, TestSize.Level1)
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
HWTEST_F(RSNodeUnitTest, HasChildrenOutOfRectTest, TestSize.Level1)
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
HWTEST_F(RSNodeUnitTest, UpdateVisibleFilterChildTest, TestSize.Level1)
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
HWTEST_F(RSNodeUnitTest, UpdateVisibleEffectChildTest, TestSize.Level1)
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
HWTEST_F(RSNodeUnitTest, GetInstanceRootNodeTest, TestSize.Level1)
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
HWTEST_F(RSNodeUnitTest, AddUIExtensionChildTest, TestSize.Level1)
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
HWTEST_F(RSNodeUnitTest, MoveUIExtensionChildTest, TestSize.Level1)
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
HWTEST_F(RSNodeUnitTest, RemoveUIExtensionChildTest, TestSize.Level1)
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
HWTEST_F(RSNodeUnitTest, NeedRoutedBasedOnUIExtensionTest, TestSize.Level1)
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
HWTEST_F(RSNodeUnitTest, UpdateTreeUifirstRootNodeIdTest, TestSize.Level1)
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
HWTEST_F(RSNodeUnitTest, GetFirstLevelNodeTest, TestSize.Level1)
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
HWTEST_F(RSNodeUnitTest, UpdateCompletedCacheSurfaceTest, TestSize.Level1)
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
HWTEST_F(RSNodeUnitTest, ClearCacheSurfaceTest, TestSize.Level1)
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
HWTEST_F(RSNodeUnitTest, SetGeoUpdateDelayTest, TestSize.Level1)
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
HWTEST_F(RSNodeUnitTest, UpdateSubSurfaceCntTest001, TestSize.Level1)
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
HWTEST_F(RSNodeUnitTest, UpdateSubSurfaceCntTest002, TestSize.Level1)
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
HWTEST_F(RSNodeUnitTest, GetNodeGroupTypeTest, TestSize.Level1)
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
HWTEST_F(RSNodeUnitTest, InitRenderParamsTest, TestSize.Level1)
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
HWTEST_F(RSNodeUnitTest, UpdateRenderParamsTest, TestSize.Level1)
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
HWTEST_F(RSNodeUnitTest, UpdateCurCornerRadiusTest, TestSize.Level1)
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
HWTEST_F(RSNodeUnitTest, UpdateSrcOrClipedAbsDrawRectChangeStateTest, TestSize.Level1)
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
HWTEST_F(RSNodeUnitTest, OnSyncTest, TestSize.Level1)
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

    node->uifirstSkipPartialSync_ = true;
    node->dirtySlots_.emplace(RSDrawableSlot::BACKGROUND_FILTER);
    node->OnSync();
    EXPECT_TRUE(node->dirtySlots_.empty());
}

/**
 * @tc.name: ValidateLightResourcesTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSNodeUnitTest, ValidateLightResourcesTest, TestSize.Level1)
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
HWTEST_F(RSNodeUnitTest, UpdatePointLightDirtySlotTest, TestSize.Level1)
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
HWTEST_F(RSNodeUnitTest, AddToPendingSyncListTest, TestSize.Level1)
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
HWTEST_F(RSNodeUnitTest, SetChildrenHasSharedTransitionTest, TestSize.Level1)
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
HWTEST_F(RSNodeUnitTest, GetPairedNodeTest, TestSize.Level1)
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
HWTEST_F(RSNodeUnitTest, UpdateAbsDrawRect, TestSize.Level1)
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
HWTEST_F(RSNodeUnitTest, UpdateHierarchyTest, TestSize.Level1)
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
    EXPECT_EQ(sharedTransitionParam->relation_, SharedTransitionParam::NodeHierarchyRelation::UNKNOWN);

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
HWTEST_F(RSNodeUnitTest, HasRelationTest, TestSize.Level1)
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
HWTEST_F(RSNodeUnitTest, SetNeedGenerateDrawableTest, TestSize.Level1)
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
HWTEST_F(RSNodeUnitTest, GenerateDrawableTest, TestSize.Level1)
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
HWTEST_F(RSNodeUnitTest, UpdateUnpairedSharedTransitionMapTest, TestSize.Level1)
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
} // namespace Rosen
} // namespace OHOS
