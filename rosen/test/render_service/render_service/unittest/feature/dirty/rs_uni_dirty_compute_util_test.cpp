/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <parameters.h>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "drawable/rs_logical_display_render_node_drawable.h"
#include "pipeline/rs_logical_display_render_node.h"
#include "drawable/rs_screen_render_node_drawable.h"
#include "pipeline/rs_screen_render_node.h"
#include "drawable/rs_surface_render_node_drawable.h"
#include "feature/dirty/rs_uni_dirty_compute_util.h"
#include "params/rs_surface_render_params.h"
#include "params/rs_logical_display_render_params.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "pipeline/rs_effect_render_node.h"
#include "pipeline/rs_test_util.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::DrawableV2;

namespace OHOS::Rosen {
namespace {
constexpr NodeId DEFAULT_ID = 0xFFFF;
}
class RSUniDirtyComputeUtilTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline RectI DEFAULT_RECT1 {0, 0, 500, 500};
    static inline RectI DEFAULT_RECT2 {500, 500, 1000, 1000};
};

RSScreenRenderNodeDrawable* GenerateScreenDrawableById(NodeId id,
    ScreenId screenId, const std::weak_ptr<RSContext>& context)
{
    std::shared_ptr<RSScreenRenderNode> renderNode = std::make_shared<RSScreenRenderNode>(id, screenId, context);
    if (!renderNode) {
        return nullptr;
    }
    RSRenderNodeDrawableAdapter* screenAdapter = RSScreenRenderNodeDrawable::OnGenerate(renderNode);
    if (!screenAdapter) {
        return nullptr;
    }
    return static_cast<RSScreenRenderNodeDrawable*>(screenAdapter);
}

RSLogicalDisplayRenderNodeDrawable* GenerateLogicalscreenDrawableById(
    NodeId id, const std::weak_ptr<RSContext>& context)
{
    RSDisplayNodeConfig config;
    std::shared_ptr<RSLogicalDisplayRenderNode> renderNode =
        std::make_shared<RSLogicalDisplayRenderNode>(id, config, context);
    if (!renderNode) {
        return nullptr;
    }
    RSRenderNodeDrawableAdapter* displayAdapter = RSLogicalDisplayRenderNodeDrawable::OnGenerate(renderNode);
    if (!displayAdapter) {
        return nullptr;
    }
    return static_cast<RSLogicalDisplayRenderNodeDrawable*>(displayAdapter);
}

void RSUniDirtyComputeUtilTest::SetUpTestCase()
{
    RSTestUtil::InitRenderNodeGC();
}
void RSUniDirtyComputeUtilTest::TearDownTestCase() {}
void RSUniDirtyComputeUtilTest::SetUp() {}
void RSUniDirtyComputeUtilTest::TearDown() {}

class MockRSSurfaceRenderNode : public RSSurfaceRenderNode {
public:
    explicit MockRSSurfaceRenderNode(NodeId id,
        const std::weak_ptr<RSContext>& context = {}, bool isTextureExportNode = false)
        : RSSurfaceRenderNode(id, context, isTextureExportNode) {}
    ~MockRSSurfaceRenderNode() override {}
    MOCK_CONST_METHOD0(NeedDrawBehindWindow, bool());
    MOCK_CONST_METHOD0(GetFilterRect, RectI());
    MOCK_CONST_METHOD0(GetOldDirtyInSurface, RectI());
};

/**
 * @tc.name: GetCurrentFrameVisibleDirty001
 * @tc.desc: test GetCurrentFrameVisibleDirty
 * @tc.type: FUNC
 * @tc.require: #IBIA3V
 */
HWTEST_F(RSUniDirtyComputeUtilTest, GetCurrentFrameVisibleDirty001, TestSize.Level1)
{
    NodeId defaultScreenNodeId = 0;
    ScreenId screenId = 1;
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    RSScreenRenderNodeDrawable* screenDrawable = GenerateScreenDrawableById(defaultScreenNodeId, screenId, context);
    ASSERT_NE(screenDrawable, nullptr);
    std::unique_ptr<RSScreenRenderParams> params = std::make_unique<RSScreenRenderParams>(defaultScreenNodeId);
    std::vector<std::shared_ptr<RSRenderNodeDrawableAdapter>> surfaceAdapters{nullptr};

    NodeId defaultSurfaceId = 10;
    std::shared_ptr<RSSurfaceRenderNode> renderNode = std::make_shared<RSSurfaceRenderNode>(defaultSurfaceId);
    auto surfaceAdapter = RSSurfaceRenderNodeDrawable::OnGenerate(renderNode);
    // default surface
    surfaceAdapters.emplace_back(surfaceAdapter);
    
    auto createDrawableWithDirtyManager = [](NodeId id, bool isApp, bool emptyDstRect) -> RSRenderNodeDrawable::Ptr {
        std::shared_ptr<RSSurfaceRenderNode> renderNode = std::make_shared<RSSurfaceRenderNode>(id);
        auto surfaceAdapter = RSSurfaceRenderNodeDrawable::OnGenerate(renderNode);
        auto surfaceDrawable = static_cast<RSSurfaceRenderNodeDrawable*>(surfaceAdapter);
        if (surfaceDrawable == nullptr) {
            return nullptr;
        }
        surfaceDrawable->syncDirtyManager_ = std::make_shared<RSDirtyRegionManager>();
        surfaceDrawable->renderParams_ = std::make_unique<RSSurfaceRenderParams>(id);
        if (surfaceDrawable->syncDirtyManager_ == nullptr || surfaceDrawable->renderParams_ == nullptr) {
            return nullptr;
        }
        auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable->renderParams_.get());
        surfaceParams->isLeashorMainWindow_ = isApp;
        surfaceParams->dstRect_ = emptyDstRect ? RectI() : DEFAULT_RECT1;
        return surfaceDrawable;
    };

    surfaceAdapters.emplace_back(createDrawableWithDirtyManager(++defaultSurfaceId, true, true));
    surfaceAdapters.emplace_back(createDrawableWithDirtyManager(++defaultSurfaceId, true, false));
    surfaceAdapters.emplace_back(createDrawableWithDirtyManager(++defaultSurfaceId, false, true));
    surfaceAdapters.emplace_back(createDrawableWithDirtyManager(++defaultSurfaceId, false, false));

    params->SetAllMainAndLeashSurfaceDrawables(surfaceAdapters);
    ScreenInfo screenInfo;
    auto rects = RSUniDirtyComputeUtil::GetCurrentFrameVisibleDirty(*screenDrawable, screenInfo, *params);
    EXPECT_EQ(rects.empty(), true);
    screenDrawable->syncDirtyManager_ = nullptr;
    rects = RSUniDirtyComputeUtil::GetCurrentFrameVisibleDirty(*screenDrawable, screenInfo, *params);
    EXPECT_EQ(rects.empty(), true);
    screenDrawable = nullptr;
}

/**
 * @tc.name: ScreenIntersectDirtyRectsTest
 * @tc.desc: Verify function ScreenIntersectDirtyRects
 * @tc.type: FUNC
 * @tc.require:issuesI9KRF1
 */
HWTEST_F(RSUniDirtyComputeUtilTest, ScreenIntersectDirtyRectsTest, Function | SmallTest | Level2)
{
    Occlusion::Region region;
    ScreenInfo screenInfo;
    EXPECT_TRUE(RSUniDirtyComputeUtil::ScreenIntersectDirtyRects(region, screenInfo).empty());
}

/**
 * @tc.name: UpdateVirtualExpandScreenAccumulatedParams001
 * @tc.desc: UpdateVirtualExpandScreenAccumulatedParams can update params
 * @tc.type: FUNC
 * @tc.require: issueICCV9N
 */
HWTEST_F(RSUniDirtyComputeUtilTest, UpdateVirtualExpandScreenAccumulatedParams001, TestSize.Level1)
{
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    RSScreenRenderNodeDrawable* screenDrawable = GenerateScreenDrawableById(DEFAULT_ID, 0, context);
    ASSERT_NE(screenDrawable, nullptr);
    screenDrawable->renderParams_ = std::make_unique<RSScreenRenderParams>(DEFAULT_ID);
    auto params = static_cast<RSScreenRenderParams*>(screenDrawable->GetRenderParams().get());
    ASSERT_NE(params, nullptr);
    params->SetMainAndLeashSurfaceDirty(true);
    params->SetHDRStatusChanged(true);
    RSUniDirtyComputeUtil::UpdateVirtualExpandScreenAccumulatedParams(*params, *screenDrawable);
    ASSERT_TRUE(params->GetAccumulatedDirty());
    ASSERT_TRUE(params->GetAccumulatedHdrStatusChanged());
}

/**
 * @tc.name: CheckVirtualExpandScreenSkip001
 * @tc.desc: CheckVirtualExpandScreenSkip return true when no accumulate status
 * @tc.type: FUNC
 * @tc.require: issueICCV9N
 */
HWTEST_F(RSUniDirtyComputeUtilTest, CheckVirtualExpandScreenSkip001, TestSize.Level1)
{
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    RSScreenRenderNodeDrawable* screenDrawable = GenerateScreenDrawableById(DEFAULT_ID, 0, context);
    ASSERT_NE(screenDrawable, nullptr);
    screenDrawable->renderParams_ = std::make_unique<RSScreenRenderParams>(DEFAULT_ID);
    auto params = static_cast<RSScreenRenderParams*>(screenDrawable->GetRenderParams().get());
    ASSERT_NE(params, nullptr);
    params->SetAccumulatedDirty(false);
    params->SetAccumulatedHdrStatusChanged(false);
    bool result = RSUniDirtyComputeUtil::CheckVirtualExpandScreenSkip(*params, *screenDrawable);
    ASSERT_TRUE(result);
}

/**
 * @tc.name: CheckVirtualExpandScreenSkip002
 * @tc.desc: CheckVirtualExpandScreenSkip return false when skip enabled is false
 * @tc.type: FUNC
 * @tc.require: issueICCV9N
 */
HWTEST_F(RSUniDirtyComputeUtilTest, CheckVirtualExpandScreenSkip002, TestSize.Level1)
{
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    RSScreenRenderNodeDrawable* screenDrawable = GenerateScreenDrawableById(DEFAULT_ID, 0, context);
    ASSERT_NE(screenDrawable, nullptr);
    screenDrawable->renderParams_ = std::make_unique<RSScreenRenderParams>(DEFAULT_ID);
    auto params = static_cast<RSScreenRenderParams*>(screenDrawable->GetRenderParams().get());
    ASSERT_NE(params, nullptr);
    auto type = system::GetParameter("rosen.uni.virtualexpandscreenskip.enabled", "1");
    system::SetParameter("rosen.uni.virtualexpandscreenskip.enabled", "0");
    bool result = RSUniDirtyComputeUtil::CheckVirtualExpandScreenSkip(*params, *screenDrawable);
    ASSERT_FALSE(result);
    system::SetParameter("rosen.uni.virtualexpandscreenskip.enabled", type);
}

/**
 * @tc.name: CheckVirtualExpandScreenSkip003
 * @tc.desc: CheckVirtualExpandScreenSkip return false when has special layer
 * @tc.type: FUNC
 * @tc.require: issueICCV9N
 */
HWTEST_F(RSUniDirtyComputeUtilTest, CheckVirtualExpandScreenSkip003, TestSize.Level1)
{
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    RSScreenRenderNodeDrawable* screenDrawable = GenerateScreenDrawableById(DEFAULT_ID, 0, context);
    ASSERT_NE(screenDrawable, nullptr);
    screenDrawable->renderParams_ = std::make_unique<RSScreenRenderParams>(DEFAULT_ID);
    auto screenParams = static_cast<RSScreenRenderParams*>(screenDrawable->GetRenderParams().get());
    ASSERT_NE(screenParams, nullptr);
    RSLogicalDisplayRenderNodeDrawable* displayDrawable = GenerateLogicalscreenDrawableById(DEFAULT_ID + 1, context);
    ASSERT_NE(displayDrawable, nullptr);
    displayDrawable->renderParams_ = std::make_unique<RSLogicalDisplayRenderParams>(DEFAULT_ID + 1);
    auto displayParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable->GetRenderParams().get());
    ASSERT_NE(displayParams, nullptr);
    RSSpecialLayerManager specialLayerManager;
    specialLayerManager.Set(SpecialLayerType::HAS_SECURITY, true);
    displayParams->specialLayerManager_ = specialLayerManager;
    screenParams->logicalDisplayNodeDrawables_.emplace_back(displayDrawable);
    bool result = RSUniDirtyComputeUtil::CheckVirtualExpandScreenSkip(*screenParams, *screenDrawable);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: CheckVirtualExpandScreenSkip004
 * @tc.desc: CheckVirtualExpandScreenSkip return false when has accumulate dirty
 * @tc.type: FUNC
 * @tc.require: issueICCV9N
 */
HWTEST_F(RSUniDirtyComputeUtilTest, CheckVirtualExpandScreenSkip004, TestSize.Level1)
{
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    RSScreenRenderNodeDrawable* screenDrawable = GenerateScreenDrawableById(DEFAULT_ID, 0, context);
    ASSERT_NE(screenDrawable, nullptr);
    screenDrawable->renderParams_ = std::make_unique<RSScreenRenderParams>(DEFAULT_ID);
    auto params = static_cast<RSScreenRenderParams*>(screenDrawable->GetRenderParams().get());
    ASSERT_NE(params, nullptr);
    params->SetAccumulatedDirty(true);
    bool result = RSUniDirtyComputeUtil::CheckVirtualExpandScreenSkip(*params, *screenDrawable);
    ASSERT_FALSE(result);
}

/*
 * @tc.name: IntersectRect
 * @tc.desc: test GraphicIRect intersect with GraphicIRect
 * @tc.type: FUNC
 * @tc.require: issueIARLU9
 */
HWTEST_F(RSUniDirtyComputeUtilTest, IntersectRect, TestSize.Level2)
{
    GraphicIRect srcRect = { 0, 0, 1080, 1920 };
    GraphicIRect emptyRect = { 0, 0, 0, 0 };

    GraphicIRect rect = emptyRect; // no intersect
    GraphicIRect result = RSUniDirtyComputeUtil::IntersectRect(srcRect, rect);
    ASSERT_EQ(result, emptyRect);

    rect = { -500, -500, -100, -100 }; // no intersect
    result = RSUniDirtyComputeUtil::IntersectRect(srcRect, rect);
    ASSERT_EQ(result, emptyRect);

    rect = { 1100, 0, 100, 100 }; // no intersect
    result = RSUniDirtyComputeUtil::IntersectRect(srcRect, rect);
    ASSERT_EQ(result, emptyRect);

    rect = { 200, 200, 800, 800 }; // all intersect
    result = RSUniDirtyComputeUtil::IntersectRect(srcRect, rect);
    ASSERT_EQ(result, rect);

    rect = { -100, -100, 3000, 3000 }; // src rect
    result = RSUniDirtyComputeUtil::IntersectRect(srcRect, rect);
    ASSERT_EQ(result, srcRect);

    rect = { -100, -100, 1000, 1000 }; // partial intersect
    result = RSUniDirtyComputeUtil::IntersectRect(srcRect, rect);
    GraphicIRect expect = { 0, 0, 900, 900 };
    ASSERT_EQ(result, expect);

    rect = { 500, 500, 2000, 2000 }; // partial intersect
    result = RSUniDirtyComputeUtil::IntersectRect(srcRect, rect);
    expect = { 500, 500, 580, 1420 };
    ASSERT_EQ(result, expect);
}

/**
 * @tc.name: GenerateFilterDirtyRegionInfo_001
 * @tc.desc: test GenerateFilterDirtyRegionInfo, for effect node, filterRegion and DirtyRegion differs.
 * @tc.type: FUNC
 * @tc.require: #issuesICA3L1
 */
HWTEST_F(RSUniDirtyComputeUtilTest, GenerateFilterDirtyRegionInfo_001, TestSize.Level1)
{
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    ASSERT_NE(context, nullptr);
    auto& nodeMap = context->GetMutableNodeMap();

    NodeId id = 1;
    auto effectNode = std::make_shared<RSEffectRenderNode>(id, context->weak_from_this());
    ASSERT_NE(effectNode, nullptr);
    effectNode->childHasVisibleEffect_ = true;
    effectNode->GetMutableRenderProperties().boundsGeo_->absRect_ = DEFAULT_RECT1;  // mock filter rect.

    auto subNode = std::make_shared<RSBaseRenderNode>(++id, context->weak_from_this());
    ASSERT_NE(subNode, nullptr);
    subNode->oldDirtyInSurface_ = DEFAULT_RECT2;  // mock child effect rect.
    effectNode->visibleEffectChild_.emplace(subNode->GetId());
    nodeMap.RegisterRenderNode(subNode);

    FilterDirtyRegionInfo filterInfo =
        RSUniFilterDirtyComputeUtil::GenerateFilterDirtyRegionInfo(*effectNode, std::nullopt, true);
    ASSERT_TRUE(filterInfo.intersectRegion_.Sub(Occlusion::Region(Occlusion::Rect(DEFAULT_RECT2))).IsEmpty());
    ASSERT_FALSE(filterInfo.filterDirty_.Sub(filterInfo.intersectRegion_).IsEmpty());
}

/**
 * @tc.name: GenerateFilterDirtyRegionInfo_002
 * @tc.desc: test GenerateFilterDirtyRegionInfo, for surface node with side bar, filterRegion and DirtyRegion differs.
 * @tc.type: FUNC
 * @tc.require: #issuesICA3L1
 */
HWTEST_F(RSUniDirtyComputeUtilTest, GenerateFilterDirtyRegionInfo_002, TestSize.Level1)
{
    NodeId id = 1;
    auto testNode = std::make_shared<MockRSSurfaceRenderNode>(id);
    ASSERT_NE(testNode, nullptr);
    EXPECT_CALL(*testNode, GetOldDirtyInSurface()).WillRepeatedly(testing::Return(DEFAULT_RECT1));
    EXPECT_CALL(*testNode, NeedDrawBehindWindow()).WillRepeatedly(testing::Return(true));
    EXPECT_CALL(*testNode, GetFilterRect()).WillRepeatedly(testing::Return(DEFAULT_RECT2));

    FilterDirtyRegionInfo filterInfo =
        RSUniFilterDirtyComputeUtil::GenerateFilterDirtyRegionInfo(*testNode, std::nullopt, true);
    ASSERT_FALSE(filterInfo.filterDirty_.Sub(Occlusion::Region(Occlusion::Rect(DEFAULT_RECT1))).IsEmpty());
}

/**
 * @tc.name: DealWithFilterDirtyRegion_001
 * @tc.desc: test DealWithFilterDirtyRegion_001, test for display side case.
 * @tc.type: FUNC
 * @tc.require: #issuesICCYYL
 */
HWTEST_F(RSUniDirtyComputeUtilTest, DealWithFilterDirtyRegion_001, TestSize.Level1)
{
    NodeId defaultDisplayId = 1;
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    RSScreenRenderNodeDrawable* screenDrawable = GenerateScreenDrawableById(defaultDisplayId, 0, context);
    ASSERT_NE(screenDrawable, nullptr);
    Occlusion::Region damageRegion;
    Occlusion::Region drawRegion;
    // test with null param.
    RSUniFilterDirtyComputeUtil::DealWithFilterDirtyRegion(damageRegion, drawRegion, *screenDrawable, std::nullopt);
    ASSERT_TRUE(damageRegion.IsEmpty());
    // test with non-null param.
    screenDrawable->renderParams_ = std::make_unique<RSScreenRenderParams>(defaultDisplayId);
    RSUniFilterDirtyComputeUtil::DealWithFilterDirtyRegion(damageRegion, drawRegion, *screenDrawable, std::nullopt);
    ASSERT_TRUE(damageRegion.IsEmpty());
}

/**
 * @tc.name: DealWithFilterDirtyRegion_002
 * @tc.desc: test DealWithFilterDirtyRegion_002, test if display has empty/non-empty damage region.
 * @tc.type: FUNC
 * @tc.require: #issuesICCYYL
 */
HWTEST_F(RSUniDirtyComputeUtilTest, DealWithFilterDirtyRegion_002, TestSize.Level1)
{
    NodeId defaultDisplayId = 1;
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    RSScreenRenderNodeDrawable* screenDrawable = GenerateScreenDrawableById(defaultDisplayId, 0, context);
    ASSERT_NE(screenDrawable, nullptr);
    screenDrawable->renderParams_ = std::make_unique<RSScreenRenderParams>(defaultDisplayId);
    ASSERT_NE(screenDrawable->renderParams_, nullptr);
    screenDrawable->syncDirtyManager_ = std::make_shared<RSDirtyRegionManager>();
    ASSERT_NE(screenDrawable->syncDirtyManager_, nullptr);

    Occlusion::Region filterRegion = Occlusion::Region(Occlusion::Rect(DEFAULT_RECT1));
    FilterDirtyRegionInfo filterInfo = {
        .intersectRegion_ = filterRegion,
        .filterDirty_ = filterRegion
    };
    screenDrawable->syncDirtyManager_->GetFilterCollector().CollectFilterDirtyRegionInfo(filterInfo, true);
    Occlusion::Region damageRegion = Occlusion::Region();
    // test with empty damageRegion.
    RSUniFilterDirtyComputeUtil::DealWithFilterDirtyRegion(damageRegion, damageRegion, *screenDrawable, std::nullopt);
    ASSERT_TRUE(damageRegion.IsEmpty());

    // test with non-empty damageRegion.
    damageRegion = Occlusion::Region(Occlusion::Rect{0, 0, 1, 1});
    RSUniFilterDirtyComputeUtil::DealWithFilterDirtyRegion(damageRegion, damageRegion, *screenDrawable, std::nullopt);
    ASSERT_FALSE(damageRegion.Area() == 1);
}

/**
 * @tc.name: DealWithFilterDirtyRegion_003
 * @tc.desc: test DealWithFilterDirtyRegion_003, test if surface has empty/non-empty damage region.
 * @tc.type: FUNC
 * @tc.require: #issuesICCYYL
 */
HWTEST_F(RSUniDirtyComputeUtilTest, DealWithFilterDirtyRegion_003, TestSize.Level1)
{
    NodeId nodeId = 1;
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    RSScreenRenderNodeDrawable* screenDrawable = GenerateScreenDrawableById(nodeId, 0, context);
    ASSERT_NE(screenDrawable, nullptr);
    screenDrawable->renderParams_ = std::make_unique<RSScreenRenderParams>(nodeId);
    ASSERT_NE(screenDrawable->renderParams_, nullptr);
    auto& surfaceDrawables = screenDrawable->renderParams_->GetAllMainAndLeashSurfaceDrawables();

    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(++nodeId);
    auto surfaceDrawable = std::make_shared<RSSurfaceRenderNodeDrawable>(surfaceNode);
    ASSERT_NE(surfaceDrawable, nullptr);
    surfaceDrawables.push_back(surfaceDrawable);

    Occlusion::Region damageRegion = Occlusion::Region();
    RSUniFilterDirtyComputeUtil::DealWithFilterDirtyRegion(damageRegion, damageRegion, *screenDrawable, std::nullopt);
    ASSERT_TRUE(damageRegion.IsEmpty());
}

/**
 * @tc.name: DealWithFilterDirtyRegion_004
 * @tc.desc: test DealWithFilterDirtyRegion_004, test for surface ivisible/visible
 * @tc.type: FUNC
 * @tc.require: #issuesICCYYL
 */
HWTEST_F(RSUniDirtyComputeUtilTest, DealWithFilterDirtyRegion_004, TestSize.Level1)
{
    NodeId nodeId = 1;
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    RSScreenRenderNodeDrawable* screenDrawable = GenerateScreenDrawableById(nodeId, 0, context);
    ASSERT_NE(screenDrawable, nullptr);
    screenDrawable->renderParams_ = std::make_unique<RSScreenRenderParams>(nodeId);
    ASSERT_NE(screenDrawable->renderParams_, nullptr);
    auto& surfaceDrawables = screenDrawable->renderParams_->GetAllMainAndLeashSurfaceDrawables();

    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(++nodeId);
    auto surfaceDrawable = std::make_shared<RSSurfaceRenderNodeDrawable>(surfaceNode);
    ASSERT_NE(surfaceDrawable, nullptr);
    surfaceDrawable->renderParams_ = std::make_unique<RSSurfaceRenderParams>(nodeId);
    surfaceDrawable->syncDirtyManager_ = std::make_shared<RSDirtyRegionManager>();

    // filter info1
    Occlusion::Region filterRegion = Occlusion::Region(Occlusion::Rect(DEFAULT_RECT1));
    FilterDirtyRegionInfo filterInfo1 = {
        .intersectRegion_ = filterRegion,
        .filterDirty_ = filterRegion
    };
    surfaceDrawable->syncDirtyManager_->GetFilterCollector().CollectFilterDirtyRegionInfo(filterInfo1, true);
    // filter info2, addedToDirty_ = true
    FilterDirtyRegionInfo filterInfo2 = {
        .intersectRegion_ = filterRegion,
        .filterDirty_ = filterRegion,
        .addToDirty_ = true
    };
    surfaceDrawable->syncDirtyManager_->GetFilterCollector().CollectFilterDirtyRegionInfo(filterInfo2, true);
    surfaceDrawables.push_back(surfaceDrawable);

    Occlusion::Region damageRegion = Occlusion::Region(Occlusion::Rect{0, 0, 1, 1});
    RSUniFilterDirtyComputeUtil::DealWithFilterDirtyRegion(damageRegion, damageRegion, *screenDrawable, std::nullopt);
    ASSERT_TRUE(damageRegion.Area() == 1);

    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    surfaceParams->SetVisibleRegion(Occlusion::Region(Occlusion::Rect(DEFAULT_RECT1)));
    RSUniFilterDirtyComputeUtil::DealWithFilterDirtyRegion(damageRegion, damageRegion, *screenDrawable, std::nullopt);
    ASSERT_FALSE(damageRegion.Area() == 1);
}

/**
 * @tc.name: DealWithFilterDirtyRegion_005
 * @tc.desc: test DealWithFilterDirtyRegion_005, test for nullptr side cases.
 * @tc.type: FUNC
 * @tc.require: #issuesICCYYL
 */
HWTEST_F(RSUniDirtyComputeUtilTest, DealWithFilterDirtyRegion_005, TestSize.Level1)
{
    NodeId nodeId = 1;
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    RSScreenRenderNodeDrawable* screenDrawable = GenerateScreenDrawableById(nodeId, 0, context);
    ASSERT_NE(screenDrawable, nullptr);
    screenDrawable->syncDirtyManager_ = nullptr;
    screenDrawable->renderParams_ = std::make_unique<RSScreenRenderParams>(nodeId);
    ASSERT_NE(screenDrawable->renderParams_, nullptr);
    auto& surfaceDrawables = screenDrawable->renderParams_->GetAllMainAndLeashSurfaceDrawables();
    surfaceDrawables.push_back(nullptr);

    Occlusion::Region damageRegion;
    ASSERT_FALSE(RSUniFilterDirtyComputeUtil::DealWithFilterDirtyForScreen(
        damageRegion, damageRegion, *screenDrawable, std::nullopt));
    ASSERT_FALSE(RSUniFilterDirtyComputeUtil::DealWithFilterDirtyForSurface(
        damageRegion, damageRegion, surfaceDrawables, std::nullopt));
    RSUniFilterDirtyComputeUtil::ResetFilterInfoStatus(*screenDrawable, surfaceDrawables);
}
} // namespace OHOS::Rosen
