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
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "skia_adapter/skia_canvas.h"
#include "parameters.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSRenderNodeUnitTest : public testing::Test {
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

void RSRenderNodeUnitTest::SetUpTestCase()
{
    canvas_ = new RSPaintFilterCanvas(&drawingCanvas_);
}
void RSRenderNodeUnitTest::TearDownTestCase()
{
    delete canvas_;
    canvas_ = nullptr;
}
void RSRenderNodeUnitTest::SetUp() {}
void RSRenderNodeUnitTest::TearDown() {}

/**
 * @tc.name: UpdateParentChildrenRectTest
 * @tc.desc: test UpdateParentChildrenRect api
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeUnitTest, UpdateParentChildrenRectTest, TestSize.Level1)
{
    RSRenderNode node(id, context);
    std::shared_ptr<RSBaseRenderNode> parentNode;
    node.UpdateParentChildrenRect(parentNode);
    RSBaseRenderNode rsBaseRenderNode(id, context);
    ASSERT_FALSE(rsBaseRenderNode.HasChildrenOutOfRect());
}

/**
 * @tc.name: AddModifierTest
 * @tc.desc: test AddModifier api
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeUnitTest, AddModifierTest, TestSize.Level1)
{
    std::shared_ptr<RSRenderModifier> modifier = nullptr;
    RSRenderNode node(id, context);
    node.AddModifier(modifier);
    ASSERT_FALSE(node.IsDirty());
}

/**
 * @tc.name: DrawCacheSurfaceTest
 * @tc.desc: test DrawCacheSurface api
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeUnitTest, DrawCacheSurfaceTest, TestSize.Level1)
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
 * @tc.require:
 */
HWTEST_F(RSRenderNodeUnitTest, SetDrawingCacheTypeTest, TestSize.Level2)
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
 * @tc.require:
 */
HWTEST_F(RSRenderNodeUnitTest, ResetFilterRectsInCacheTest, TestSize.Level2)
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
 * @tc.name: ManageDrawingCacheTest
 * @tc.desc: SetDrawingCacheChanged and ResetDrawingCacheNeedUpdate test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeUnitTest, ManageDrawingCacheTest, TestSize.Level2)
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
 * @tc.require:
 */
HWTEST_F(RSRenderNodeUnitTest, ResetDrawingCacheNeedUpdateTest001, TestSize.Level2)
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
 * @tc.require:
 */
HWTEST_F(RSRenderNodeUnitTest, ResetDrawingCacheNeedUpdateTest002, TestSize.Level2)
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
 * @tc.require:
 */
HWTEST_F(RSRenderNodeUnitTest, SetVisitedCacheRootIdsTest, TestSize.Level2)
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
 * @tc.require:
 */
HWTEST_F(RSRenderNodeUnitTest, SetDrawingCacheRootIdTest, TestSize.Level2)
{
    RSRenderNode node(id, context);
    RSRenderNode drawingCacheRootNode(id + 1, context);

    node.SetDrawingCacheRootId(drawingCacheRootNode.GetId());
    ASSERT_EQ(node.GetDrawingCacheRootId(), drawingCacheRootNode.GetId());
}

/**
 * @tc.name: SetGeoUpdateDelayTest01
 * @tc.desc: test SetGeoUpdateDelay api
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeUnitTest, SetGeoUpdateDelayTest01, TestSize.Level2)
{
    RSRenderNode node(id, context);
    // test default value
    ASSERT_EQ(node.GetGeoUpdateDelay(), false);

    node.SetGeoUpdateDelay(true);
    ASSERT_EQ(node.GetGeoUpdateDelay(), true);
}

/**
 * @tc.name: SetGeoUpdateDelayTest02
 * @tc.desc: test SetGeoUpdateDelay would not be covered by later setting
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeUnitTest, SetGeoUpdateDelayTest02, TestSize.Level2)
{
    RSRenderNode node(id, context);
    node.SetGeoUpdateDelay(true);
    node.SetGeoUpdateDelay(false);
    ASSERT_EQ(node.GetGeoUpdateDelay(), true);
}

/**
 * @tc.name: ResetGeoUpdateDelayTest01
 * @tc.desc: test SetGeoUpdateDelay would be reset
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeUnitTest, ResetGeoUpdateDelayTest01, TestSize.Level2)
{
    RSRenderNode node(id, context);
    node.SetGeoUpdateDelay(true);
    node.ResetGeoUpdateDelay();
    ASSERT_EQ(node.GetGeoUpdateDelay(), false);
}

/**
 * @tc.name: SetBootAnimationTest
 * @tc.desc: SetBootAnimation and GetBootAnimation
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeUnitTest, SetBootAnimationTest, TestSize.Level1)
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
 * @tc.require:
 */
HWTEST_F(RSRenderNodeUnitTest, OnlyBasicGeoTransfromTest01, TestSize.Level1)
{
    RSRenderNode node(id, context);
    ASSERT_EQ(node.IsOnlyBasicGeoTransform(), true);
}

/**
 * @tc.name: OnlyBasicGeoTransfromTest02
 * @tc.desc: Check node contains more than BasicGeoTransfrom if content dirty
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeUnitTest, OnlyBasicGeoTransfromTest02, TestSize.Level1)
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
 * @tc.require:
 */
HWTEST_F(RSRenderNodeUnitTest, OnlyBasicGeoTransfromTest03, TestSize.Level1)
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
 * @tc.require:
 */
HWTEST_F(RSRenderNodeUnitTest, UpdateDirtyRegionTest01, TestSize.Level1)
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
 * @tc.require:
 */
HWTEST_F(RSRenderNodeUnitTest, UpdateDirtyRegionTest02, TestSize.Level1)
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
 * @tc.require:
 */
HWTEST_F(RSRenderNodeUnitTest, UpdateDirtyRegionTest03, TestSize.Level1)
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
 * @tc.require:
 */
HWTEST_F(RSRenderNodeUnitTest, UpdateDirtyRegionTest04, TestSize.Level1)
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
 * @tc.require:
 */
HWTEST_F(RSRenderNodeUnitTest, UpdateDirtyRegionTest05, TestSize.Level1)
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
 * @tc.desc: test GetFilterRectsInCache api
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeUnitTest, GetFilterRectsInCacheTest, TestSize.Level1)
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
 * @tc.desc: test GetFilterRect api
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeUnitTest, GetFilterRectTest, TestSize.Level1)
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
 * @tc.desc: test CalVisibleFilterRect api
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeUnitTest, CalVisibleFilterRectTest, TestSize.Level1)
{
    RSRenderNode node(id, context);
    RectI prepareClipRect { 1, 1, 1, 1 };
    node.CalVisibleFilterRect(prepareClipRect);
    EXPECT_TRUE(node.filterRegion_.ToString().compare("[0, 0, 0, 0]") == 0);
}

/**
 * @tc.name: OnTreeStateChangedTest
 * @tc.desc: test OnTreeStateChanged api
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeUnitTest, OnTreeStateChangedTest, TestSize.Level1)
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
 * @tc.desc: test HasDisappearingTransition api
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeUnitTest, HasDisappearingTransitionTest, TestSize.Level1)
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
 * @tc.desc: test GenerateFullChildrenList api
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeUnitTest, GenerateFullChildrenListTest, TestSize.Level1)
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
 * @tc.desc: test ResortChildren api
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeUnitTest, ResortChildrenTest, TestSize.Level1)
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
 * @tc.desc: test HasChildrenOutOfRect api
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeUnitTest, HasChildrenOutOfRectTest, TestSize.Level1)
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
 * @tc.desc: test UpdateVisibleFilterChild api
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeUnitTest, UpdateVisibleFilterChildTest, TestSize.Level1)
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
 * @tc.desc: test UpdateVisibleEffectChild api
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeUnitTest, UpdateVisibleEffectChildTest, TestSize.Level1)
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
 * @tc.desc: test GetInstanceRootNode api
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeUnitTest, GetInstanceRootNodeTest, TestSize.Level1)
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
 * @tc.name: UpdateTreeUifirstRootNodeIdTest
 * @tc.desc: test UpdateTreeUifirstRootNodeId api
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeUnitTest, UpdateTreeUifirstRootNodeIdTest, TestSize.Level1)
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
 * @tc.desc: test GetFirstLevelNode api
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeUnitTest, GetFirstLevelNodeTest, TestSize.Level1)
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
 * @tc.desc: test UpdateCompletedCacheSurface api
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeUnitTest, UpdateCompletedCacheSurfaceTest, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(id, context);
    auto cacheId = 1;
    auto completeId = 2;
    node->cacheSurfaceThreadIndex_ = cacheId;
    node->completedSurfaceThreadIndex_ = completeId;
    node->UpdateCompletedCacheSurface();
    EXPECT_EQ(cacheId, node->completedSurfaceThreadIndex_);
}

/**
 * @tc.name: ClearCacheSurfaceTest
 * @tc.desc: test ClearCacheSurface api
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeUnitTest, ClearCacheSurfaceTest, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(id, context);
    node->cacheCompletedSurface_ = std::make_shared<Drawing::Surface>();
    node->ClearCacheSurface();
    EXPECT_EQ(node->cacheCompletedSurface_, nullptr);
}

/**
 * @tc.name: SetGeoUpdateDelayTest
 * @tc.desc: test SetGeoUpdateDelay api
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeUnitTest, SetGeoUpdateDelayTest, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(id, context);
    EXPECT_FALSE(node->geoUpdateDelay_);
    node->SetGeoUpdateDelay(true);
    EXPECT_TRUE(node->geoUpdateDelay_);
}

/**
 * @tc.name: StoreMustRenewedInfoTest
 * @tc.desc: test StoreMustRenewedInfo api
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeUnitTest, StoreMustRenewedInfoTest, TestSize.Level1)
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
 * @tc.desc: test UpdateSubSurfaceCnt api
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeUnitTest, UpdateSubSurfaceCntTest, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(id, context);
    auto curParent = std::make_shared<RSRenderNode>(id + 1, context);
    auto preParent = std::make_shared<RSRenderNode>(id + 2, context);
    EXPECT_TRUE(node->GetType() != RSRenderNodeType::SURFACE_NODE);
    node->UpdateSubSurfaceCnt(curParent, preParent);

    const auto cnt = 9;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    auto curSurfaceParent = std::make_shared<RSSurfaceRenderNode>(id + 1, context);
    auto preSurfaceParent = std::make_shared<RSSurfaceRenderNode>(id + 2, context);
    curSurfaceParent->subSurfaceCnt_ = cnt;
    preSurfaceParent->subSurfaceCnt_ = cnt;
    surfaceNode->UpdateSubSurfaceCnt(curSurfaceParent, preSurfaceParent);
    EXPECT_EQ(curSurfaceParent->subSurfaceCnt_, cnt + 1);
    EXPECT_EQ(preSurfaceParent->subSurfaceCnt_, cnt - 1);
}

/**
 * @tc.name: GetNodeGroupTypeTest
 * @tc.desc: test GetNodeGroupType api
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeUnitTest, GetNodeGroupTypeTest, TestSize.Level1)
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
 * @tc.desc: test InitRenderParams api
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeUnitTest, InitRenderParamsTest, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(id, context);
    node->InitRenderParams();
    EXPECT_TRUE(node->renderDrawable_ == nullptr);
}

/**
 * @tc.name: UpdateRenderParamsTest
 * @tc.desc: test UpdateRenderParams api
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeUnitTest, UpdateRenderParamsTest, TestSize.Level1)
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
 * @tc.desc: test UpdateCurCornerRadius api
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeUnitTest, UpdateCurCornerRadiusTest, TestSize.Level1)
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
 * @tc.desc: test UpdateSrcOrClipedAbsDrawRectChangeState api
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeUnitTest, UpdateSrcOrClipedAbsDrawRectChangeStateTest, TestSize.Level1)
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
 * @tc.name: AddToPendingSyncListTest
 * @tc.desc: test AddToPendingSyncList api
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeUnitTest, AddToPendingSyncListTest, TestSize.Level1)
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
 * @tc.desc: test SetChildrenHasSharedTransition api
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeUnitTest, SetChildrenHasSharedTransitionTest, TestSize.Level1)
{
    RSRenderNode node(id, context);
    node.SetChildrenHasSharedTransition(true);
    EXPECT_TRUE(node.childrenHasSharedTransition_);
}

/**
 * @tc.name: GetPairedNodeTest
 * @tc.desc: test GetPairedNode api
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeUnitTest, GetPairedNodeTest, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> inNode = std::make_shared<RSBaseRenderNode>(id + 1, context);
    std::shared_ptr<RSRenderNode> outNode = std::make_shared<RSBaseRenderNode>(id + 2, context);
    auto sharedTransitionParam = std::make_shared<SharedTransitionParam>(inNode, outNode);
    auto ptr = sharedTransitionParam->GetPairedNode(id + 1);
    EXPECT_EQ(ptr, outNode);
    ptr = sharedTransitionParam->GetPairedNode(id + 2);
    EXPECT_EQ(ptr, inNode);
    ptr = sharedTransitionParam->GetPairedNode(id);
    EXPECT_EQ(ptr, nullptr);
}
} // namespace Rosen
} // namespace OHOS
