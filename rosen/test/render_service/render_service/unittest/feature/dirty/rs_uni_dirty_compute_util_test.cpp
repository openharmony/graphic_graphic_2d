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

RSSurfaceRenderNodeDrawable* createDrawableWithDirtyAndParams(NodeId id, bool isSurfaceRenderParamsNotNull,
    bool isApp, bool firstLevel, bool isDirtyMangerNotNull)
{
    std::shared_ptr<RSSurfaceRenderNode> renderNode = std::make_shared<RSSurfaceRenderNode>(id);
    auto surfaceAdapter = RSSurfaceRenderNodeDrawable::OnGenerate(renderNode);
    auto surfaceDrawable = static_cast<RSSurfaceRenderNodeDrawable*>(surfaceAdapter);
    if (surfaceDrawable == nullptr) {
        return surfaceDrawable;
    }
    if (isSurfaceRenderParamsNotNull) {
        surfaceDrawable->renderParams_ = std::make_unique<RSSurfaceRenderParams>(id);
    } else {
        surfaceDrawable->renderParams_ = nullptr;
    }
    if (surfaceDrawable->renderParams_ == nullptr) {
        return surfaceDrawable;
    }
    if (isDirtyMangerNotNull) {
        surfaceDrawable->syncDirtyManager_ = std::make_shared<RSDirtyRegionManager>();
    } else {
        surfaceDrawable->syncDirtyManager_ = nullptr;
    }
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable->renderParams_.get());
    surfaceParams->SetWindowInfo(isApp, isApp, isApp);
    surfaceParams->isFirstLevelCrossNode_ = firstLevel;
    if (surfaceDrawable->syncDirtyManager_ == nullptr) {
        return surfaceDrawable;
    }
    surfaceDrawable->syncDirtyManager_->SetCurrentFrameDirtyRect(RSUniDirtyComputeUtilTest::DEFAULT_RECT1);
    return surfaceDrawable;
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
 * @tc.name: GetVisibleFilterRect_ColorPickerDrawable
 * @tc.desc: test GetVisibleFilterRect skips color picker only nodes
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniDirtyComputeUtilTest, GetVisibleFilterRect_ColorPickerDrawable, TestSize.Level1)
{
    NodeId id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id);
    ASSERT_NE(surfaceNode, nullptr);

    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    surfaceNode->context_ = context;

    // Create a filter node with only ColorPickerDrawable (no real filter)
    NodeId colorPickerOnlyNodeId = 100;
    auto colorPickerOnlyNode = std::make_shared<RSRenderNode>(colorPickerOnlyNodeId);
    colorPickerOnlyNode->SetOldDirtyInSurface(RectI(100, 100, 500, 500));
    surfaceNode->visibleFilterChild_.push_back(colorPickerOnlyNodeId);

    // Create a ColorPicker drawable for the node
    auto colorPickerDrawable = std::make_shared<DrawableV2::RSColorPickerDrawable>(false, 0);
    ASSERT_NE(colorPickerDrawable, nullptr);
    colorPickerOnlyNode->GetDrawableVec(__func__).at(
        static_cast<int8_t>(RSDrawableSlot::COLOR_PICKER)) = colorPickerDrawable;

    // Register the node in nodeMap
    context->GetMutableNodeMap().RegisterRenderNode(colorPickerOnlyNode);

    // GetVisibleFilterRect should skip color picker only nodes, resulting in empty rect
    RectI filterRect = RSUniFilterDirtyComputeUtil::GetVisibleFilterRect(*surfaceNode);
    ASSERT_TRUE(filterRect.IsEmpty());

    // Now add a real filter to the node (background filter)
    colorPickerOnlyNode->renderProperties_.backgroundFilter_ = std::make_shared<RSFilter>();

    // GetVisibleFilterRect should now include the node since it has a real filter
    filterRect = RSUniFilterDirtyComputeUtil::GetVisibleFilterRect(*surfaceNode);
    ASSERT_FALSE(filterRect.IsEmpty());
}

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
        surfaceParams->SetWindowInfo(isApp, isApp, isApp);
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
 * @tc.name: SkipedVirtualExpandScreenDirtyRegionsOperateTest01
 * @tc.desc: test SkipedVirtualExpandScreenDirtyRegionsOperateTest01
 * @tc.type: FUNC
 * @tc.require: #ICW5FR
 */
HWTEST_F(RSUniDirtyComputeUtilTest, SkipedVirtualExpandScreenDirtyRegionsOperateTest01, TestSize.Level1)
{
    NodeId defaultScreenNodeId = 0;
    ScreenId screenId = 1;
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    RSScreenRenderNodeDrawable* screenDrawable = GenerateScreenDrawableById(defaultScreenNodeId, screenId, context);
    ASSERT_NE(screenDrawable, nullptr);
    std::unique_ptr<RSScreenRenderParams> params = std::make_unique<RSScreenRenderParams>(defaultScreenNodeId);
    params->isFirstVisitCrossNodeDisplay_ = false;
    std::vector<std::shared_ptr<RSRenderNodeDrawableAdapter>> surfaceAdapters{nullptr};

    NodeId defaultSurfaceId = 10;
    std::shared_ptr<RSSurfaceRenderNode> renderNode = std::make_shared<RSSurfaceRenderNode>(defaultSurfaceId);
    auto surfaceAdapter = RSSurfaceRenderNodeDrawable::OnGenerate(renderNode);
    // default surface
    surfaceAdapters.emplace_back(surfaceAdapter);
    screenDrawable->syncDirtyManager_->SetCurrentFrameDirtyRect(DEFAULT_RECT1);
    surfaceAdapters.emplace_back(createDrawableWithDirtyAndParams(++defaultSurfaceId, false, true, true, true));
    surfaceAdapters.emplace_back(createDrawableWithDirtyAndParams(++defaultSurfaceId, false, false, true, false));
    surfaceAdapters.emplace_back(createDrawableWithDirtyAndParams(++defaultSurfaceId, true, false, false, false));
    surfaceAdapters.emplace_back(createDrawableWithDirtyAndParams(++defaultSurfaceId, true, true, false, true));
    surfaceAdapters.emplace_back(createDrawableWithDirtyAndParams(++defaultSurfaceId, true, true, false, false));
    surfaceAdapters.emplace_back(createDrawableWithDirtyAndParams(++defaultSurfaceId, true, true, true, true));

    params->isFirstVisitCrossNodeDisplay_ = false;
    params->SetAllMainAndLeashSurfaceDrawables(surfaceAdapters);
    RSUniDirtyComputeUtil::AccumulateVirtualExpandScreenDirtyRegions(*screenDrawable, *params);
    RSUniDirtyComputeUtil::MergeVirtualExpandScreenAccumulatedDirtyRegions(*screenDrawable, *params);
    RSUniDirtyComputeUtil::ClearVirtualExpandScreenAccumulatedDirtyRegions(*screenDrawable, *params);
    ASSERT_EQ(screenDrawable->syncDirtyManager_->GetVirtualExpandScreenAccumulatedDirtyRegions().size(), 0);
    params->isFirstVisitCrossNodeDisplay_ = true;
    RSUniDirtyComputeUtil::AccumulateVirtualExpandScreenDirtyRegions(*screenDrawable, *params);
    RSUniDirtyComputeUtil::MergeVirtualExpandScreenAccumulatedDirtyRegions(*screenDrawable, *params);
    RSUniDirtyComputeUtil::ClearVirtualExpandScreenAccumulatedDirtyRegions(*screenDrawable, *params);
    ASSERT_EQ(screenDrawable->syncDirtyManager_->GetVirtualExpandScreenAccumulatedDirtyRegions().size(), 0);
    params->SetAccumulatedSpecialLayerStatusChanged(true);
    RSUniDirtyComputeUtil::AccumulateVirtualExpandScreenDirtyRegions(*screenDrawable, *params);
    RSUniDirtyComputeUtil::MergeVirtualExpandScreenAccumulatedDirtyRegions(*screenDrawable, *params);
    RSUniDirtyComputeUtil::ClearVirtualExpandScreenAccumulatedDirtyRegions(*screenDrawable, *params);
    ASSERT_EQ(screenDrawable->syncDirtyManager_->GetVirtualExpandScreenAccumulatedDirtyRegions().size(), 0);
    screenDrawable->syncDirtyManager_ = nullptr;
    RSUniDirtyComputeUtil::AccumulateVirtualExpandScreenDirtyRegions(*screenDrawable, *params);
    RSUniDirtyComputeUtil::MergeVirtualExpandScreenAccumulatedDirtyRegions(*screenDrawable, *params);
    RSUniDirtyComputeUtil::ClearVirtualExpandScreenAccumulatedDirtyRegions(*screenDrawable, *params);
    auto surfaceNodeDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(
        params->GetAllMainAndLeashSurfaceDrawables().back());
    ASSERT_EQ(surfaceNodeDrawable->syncDirtyManager_->GetVirtualExpandScreenAccumulatedDirtyRegions().size(), 0);
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
    RSUniDirtyComputeUtil::UpdateVirtualExpandScreenAccumulatedParams(*params, *screenDrawable, nullptr);
    ASSERT_TRUE(params->GetAccumulatedDirty());
    ASSERT_TRUE(params->GetAccumulatedHdrStatusChanged());
}

/**
 * @tc.name: UpdateVirtualExpandScreenAccumulatedParams002
 * @tc.desc: UpdateVirtualExpandScreenAccumulatedParams can update params
 * @tc.type: FUNC
 * @tc.require: issueICCV9N
 */
HWTEST_F(RSUniDirtyComputeUtilTest, UpdateVirtualExpandScreenAccumulatedParams002, TestSize.Level1)
{
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    RSScreenRenderNodeDrawable* screenDrawable = GenerateScreenDrawableById(DEFAULT_ID, 0, context);
    ASSERT_NE(screenDrawable, nullptr);
    screenDrawable->renderParams_ = std::make_unique<RSScreenRenderParams>(DEFAULT_ID);
    auto params = static_cast<RSScreenRenderParams*>(screenDrawable->GetRenderParams().get());
    auto screenParams = static_cast<RSScreenRenderParams*>(screenDrawable->GetRenderParams().get());
    RSLogicalDisplayRenderNodeDrawable* displayDrawable = GenerateLogicalscreenDrawableById(DEFAULT_ID + 1, context);
    ASSERT_NE(displayDrawable, nullptr);
    displayDrawable->renderParams_ = std::make_unique<RSLogicalDisplayRenderParams>(DEFAULT_ID + 1);
    auto displayParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable->GetRenderParams().get());
    ASSERT_NE(displayParams, nullptr);
    RSSpecialLayerManager specialLayerManager;
    specialLayerManager.Set(SpecialLayerType::HAS_SECURITY, true);
    displayParams->specialLayerManager_ = specialLayerManager;
    screenParams->logicalDisplayNodeDrawables_.emplace_back(displayDrawable);
    screenParams->logicalDisplayNodeDrawables_.emplace_back(nullptr);
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    RSUniDirtyComputeUtil::UpdateVirtualExpandScreenAccumulatedParams(*params, *screenDrawable, screenManager);

    std::unordered_set<NodeId> blackListVector({1, 2, 3});
    params->SetLastBlackList(blackListVector);
    RSUniDirtyComputeUtil::UpdateVirtualExpandScreenAccumulatedParams(*params, *screenDrawable, screenManager);
    std::unordered_set<NodeId> blackListVector2({});
    params->SetLastBlackList(blackListVector2);
    params->SetLastSecExemption(true);
    RSUniDirtyComputeUtil::UpdateVirtualExpandScreenAccumulatedParams(*params, *screenDrawable, screenManager);
    screenParams->logicalDisplayNodeDrawables_.emplace_back(nullptr);
    displayDrawable->renderParams_ = nullptr;
    params->SetLastSecExemption(false);
    RSUniDirtyComputeUtil::UpdateVirtualExpandScreenAccumulatedParams(*params, *screenDrawable, screenManager);
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
    ASSERT_TRUE(filterInfo.filterDirty_.Area() == filterInfo.intersectRegion_.Area());
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

/**
 * @tc.name: CheckMergeFilterDirty001
 * @tc.desc: test CheckMergeFilterDirty, if filter can be partially rendered, dirty collection can be skipped.
 * @tc.type: FUNC
 * @tc.require: #issuesICMQKE
 */
HWTEST_F(RSUniDirtyComputeUtilTest, CheckMergeFilterDirty001, TestSize.Level1)
{
    auto testFunc = [](bool cacheValid, bool partialRender, bool forceDisable, bool expectation) {
        auto dirtyManager = std::make_shared<RSDirtyRegionManager>();
        ASSERT_NE(dirtyManager, nullptr);
        NodeId nodeId = 1;
        FilterDirtyRegionInfo filterInfo = {
            .id_ = nodeId,
            .intersectRegion_ = Occlusion::Rect(DEFAULT_RECT1),
            .filterDirty_ = Occlusion::Rect(DEFAULT_RECT2),
            .forceDisablePartialRender_ = forceDisable
        };
        dirtyManager->GetFilterCollector().CollectFilterDirtyRegionInfo(filterInfo, true);

        Occlusion::Region damageRegion = Occlusion::Rect(DEFAULT_RECT1);
        RSFilterDirtyCollector::RecordFilterCacheValidForOcclusion(nodeId, cacheValid);
        RSFilterDirtyCollector::SetValidCachePartialRender(partialRender);

        RSUniFilterDirtyComputeUtil::CheckMergeFilterDirty(
            damageRegion, damageRegion, *dirtyManager, std::nullopt, std::nullopt);
        ASSERT_EQ(damageRegion.Area() == DEFAULT_RECT1.GetWidth() * DEFAULT_RECT1.GetHeight(), expectation);
        RSFilterDirtyCollector::ResetFilterCacheValidForOcclusion();
    };
    // check cache status, if cache is valid and no particular reson to disable partial render,
    // dirty collection can be skipped.
    testFunc(false, false, false, false);
    testFunc(false, true, false, false);
    testFunc(true, false, false, false);
    testFunc(true, true, false, true);
    // if pixel stretch is valid for this node, dirty region should be expanded.
    testFunc(false, false, true, false);
    testFunc(false, true, true, false);
    testFunc(true, false, true, false);
    testFunc(true, true, true, false);
}

/**
 * @tc.name: CheckCurrentFrameHasDirtyInVirtual001
 * @tc.desc: test CheckCurrentFrameHasDirtyInVirtual
 * @tc.type: FUNC
 * @tc.require: issueICWRWD
 */
HWTEST_F(RSUniDirtyComputeUtilTest, CheckCurrentFrameHasDirtyInVirtual001, TestSize.Level1)
{
    NodeId defaultScreenNodeId = 0;
    NodeId mirroredScreenNodeId = 1;
    NodeId logicalScreenNodeId = 2;
    ScreenId screenId = 1;
    RectI defaultDirtyRect = { 0, 0, 100, 100};
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    std::shared_ptr<RSScreenRenderNode> screenNode =
        std::make_shared<RSScreenRenderNode>(defaultScreenNodeId, screenId, context);
    screenNode->InitRenderParams();

    RSScreenRenderNodeDrawable* mirroredScreenDrawable = GenerateScreenDrawableById(
        mirroredScreenNodeId, screenId + 1, context);
    ASSERT_NE(mirroredScreenDrawable, nullptr);
    auto screenDrawable = static_cast<RSScreenRenderNodeDrawable*>(screenNode->renderDrawable_.get());
    ASSERT_NE(screenDrawable, nullptr);
    auto screenParams = static_cast<RSScreenRenderParams*>(screenDrawable->GetRenderParams().get());
    mirroredScreenDrawable->renderParams_ = std::make_unique<RSScreenRenderParams>(mirroredScreenNodeId);
    auto mirroredScreenParams = static_cast<RSScreenRenderParams*>(mirroredScreenDrawable->GetRenderParams().get());
    mirroredScreenParams->mirrorSourceDrawable_ = screenNode->GetRenderDrawable();;
    screenDrawable->syncDirtyManager_ = std::make_shared<RSDirtyRegionManager>();
    ASSERT_NE(screenDrawable->syncDirtyManager_, nullptr);
    screenDrawable->syncDirtyManager_->SetCurrentFrameDirtyRect(defaultDirtyRect);
    EXPECT_TRUE(RSUniDirtyComputeUtil::CheckCurrentFrameHasDirtyInVirtual(*mirroredScreenDrawable));

    screenDrawable->syncDirtyManager_->currentFrameDirtyRegion_.Clear();
    screenDrawable->syncDirtyManager_->hwcDirtyRegion_ = defaultDirtyRect;
    EXPECT_TRUE(RSUniDirtyComputeUtil::CheckCurrentFrameHasDirtyInVirtual(*mirroredScreenDrawable));
    screenDrawable->syncDirtyManager_->hwcDirtyRegion_.Clear();

    RSLogicalDisplayRenderNodeDrawable* displayDrawable =
        GenerateLogicalscreenDrawableById(logicalScreenNodeId, context);
    ASSERT_NE(displayDrawable, nullptr);
    displayDrawable->renderParams_ = std::make_unique<RSLogicalDisplayRenderParams>(logicalScreenNodeId);
    auto displayParams = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable->GetRenderParams().get());
    ASSERT_NE(displayParams, nullptr);
    mirroredScreenParams->logicalDisplayNodeDrawables_.emplace_back(displayDrawable);
    
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(screenManager, nullptr);
    NodeType cursorType = 12;
    std::vector<NodeType> typeBlackList = {cursorType};
    screenManager->SetVirtualScreenTypeBlackList(logicalScreenNodeId, typeBlackList);

    screenDrawable->syncDirtyManager_->typeHwcDirtyRegion_ =
        {{RSSurfaceNodeType::CURSOR_NODE, defaultDirtyRect}};
    EXPECT_TRUE(RSUniDirtyComputeUtil::CheckCurrentFrameHasDirtyInVirtual(*mirroredScreenDrawable));

    screenDrawable->syncDirtyManager_->typeHwcDirtyRegion_ =
        {{RSSurfaceNodeType::APP_WINDOW_NODE, defaultDirtyRect}};
    EXPECT_TRUE(RSUniDirtyComputeUtil::CheckCurrentFrameHasDirtyInVirtual(*mirroredScreenDrawable));
    
    screenDrawable->syncDirtyManager_->typeHwcDirtyRegion_ =
        {{RSSurfaceNodeType::APP_WINDOW_NODE, RectI()}};
    std::vector<std::shared_ptr<RSRenderNodeDrawableAdapter>> surfaceAdapters{nullptr};
    
    NodeId defaultSurfaceId = 10;
    auto createDrawableWithDirtyManager = [](NodeId id, bool flag,
        RSSpecialLayerManager specialLayerManager) -> RSRenderNodeDrawable::Ptr {
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
        if (flag) {
            RectI rect = {0, 0, 100, 100};
            surfaceDrawable->syncDirtyManager_->SetCurrentFrameDirtyRect(rect);
            Occlusion::Region region = Occlusion::Region(rect);
            surfaceParams->SetVisibleRegionInVirtual(region);
        }
        surfaceParams->SetWindowInfo(true, true, true);
        surfaceParams->specialLayerManager_ = specialLayerManager;
        return surfaceDrawable;
    };
    RSSpecialLayerManager specialLayerManager1;
    specialLayerManager1.SetWithScreen(displayParams->GetScreenId(), SpecialLayerType::IS_BLACK_LIST, true);
    surfaceAdapters.emplace_back(createDrawableWithDirtyManager(++defaultSurfaceId, true, specialLayerManager1));

    RSSpecialLayerManager specialLayerManager2;
    specialLayerManager2.Set(SpecialLayerType::SKIP, true);
    surfaceAdapters.emplace_back(createDrawableWithDirtyManager(++defaultSurfaceId, true, specialLayerManager2));

    RSSpecialLayerManager specialLayerManager3;
    surfaceAdapters.emplace_back(createDrawableWithDirtyManager(++defaultSurfaceId, true, specialLayerManager3));

    screenParams->SetAllMainAndLeashSurfaceDrawables(surfaceAdapters);
    EXPECT_FALSE(RSUniDirtyComputeUtil::CheckCurrentFrameHasDirtyInVirtual(*mirroredScreenDrawable));

    surfaceAdapters.clear();
    std::shared_ptr<RSSurfaceRenderNode> renderNode = std::make_shared<RSSurfaceRenderNode>(++defaultSurfaceId);
    auto surfaceAdapter = RSSurfaceRenderNodeDrawable::OnGenerate(renderNode);
    surfaceAdapters.emplace_back(surfaceAdapter);

    RSSpecialLayerManager specialLayerManager4;
    surfaceAdapters.emplace_back(createDrawableWithDirtyManager(++defaultSurfaceId, false, specialLayerManager4));

    screenParams->SetAllMainAndLeashSurfaceDrawables(surfaceAdapters);
    EXPECT_FALSE(RSUniDirtyComputeUtil::CheckCurrentFrameHasDirtyInVirtual(*mirroredScreenDrawable));
    
    screenDrawable = nullptr;
    mirroredScreenDrawable = nullptr;
    displayDrawable = nullptr;
}

/**
 * @tc.name: GetVisibleFilterRect_001
 * @tc.desc: test GetVisibleFilterRect
 * @tc.type: FUNC
 * @tc.require: issue20192
 */
HWTEST_F(RSUniDirtyComputeUtilTest, GetVisibleFilterRect_001, TestSize.Level1)
{
    NodeId id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id);
    ASSERT_NE(surfaceNode, nullptr);

    RectI filterRect = RSUniFilterDirtyComputeUtil::GetVisibleFilterRect(*surfaceNode);
    ASSERT_TRUE(filterRect.IsEmpty());

    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    surfaceNode->context_ = context;
    NodeId filterNodeId = 100;
    auto filterNode = std::make_shared<RSRenderNode>(filterNodeId);
    filterNode->SetOldDirtyInSurface(RectI(100, 100, 500, 500));
    surfaceNode->visibleFilterChild_.push_back(filterNodeId);
    filterRect = RSUniFilterDirtyComputeUtil::GetVisibleFilterRect(*surfaceNode);
    ASSERT_TRUE(filterRect.IsEmpty());

    context->GetMutableNodeMap().RegisterRenderNode(filterNode);
    filterRect = RSUniFilterDirtyComputeUtil::GetVisibleFilterRect(*surfaceNode);
    ASSERT_FALSE(filterRect.IsEmpty());
}

/**
* @tc.name: HasMirrorDisplay001
* @tc.desc: Test HasMirrorDisplay with multiple mirror displays
* @tc.type: FUNC
* @tc.require: issue22079
*/
HWTEST_F(RSUniDirtyComputeUtilTest, HasMirrorDisplay001, TestSize.Level1)
{
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
    NodeId displayId = 0;
    RSDisplayNodeConfig config;

    auto sourceNode = std::make_shared<RSLogicalDisplayRenderNode>(++displayId, config);
    auto displayNode1 = std::make_shared<RSLogicalDisplayRenderNode>(++displayId, config);
    auto displayNode2 = std::make_shared<RSLogicalDisplayRenderNode>(++displayId, config);
    ASSERT_NE(sourceNode, nullptr);
    ASSERT_NE(displayNode1, nullptr);
    ASSERT_NE(displayNode2, nullptr);
    displayNode1->SetIsMirrorDisplay(true);
    displayNode1->SetMirrorSource(sourceNode);
    displayNode2->SetMirrorSource(nullptr);

    uint32_t index = 0;
    nodeMap.logicalDisplayNodeMap_.emplace(index++, displayNode1);
    nodeMap.logicalDisplayNodeMap_.emplace(index++, displayNode2);
    nodeMap.logicalDisplayNodeMap_.emplace(index, nullptr);
    EXPECT_TRUE(RSUniDirtyComputeUtil::HasMirrorDisplay());
    nodeMap.logicalDisplayNodeMap_.clear();
}
} // namespace OHOS::Rosen
