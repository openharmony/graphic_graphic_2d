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
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "drawable/rs_display_render_node_drawable.h"
#include "drawable/rs_surface_render_node_drawable.h"
#include "feature/dirty/rs_uni_dirty_compute_util.h"
#include "params/rs_surface_render_params.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "pipeline/rs_effect_render_node.h"
#include "pipeline/rs_test_util.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::DrawableV2;

namespace OHOS::Rosen {
class RSUniDirtyComputeUtilTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline RectI DEFAULT_RECT1 {0, 0, 500, 500};
    static inline RectI DEFAULT_RECT2 {500, 500, 1000, 1000};
};

RSDisplayRenderNodeDrawable* GenerateDisplayDrawableById(NodeId id, RSDisplayNodeConfig config)
{
    std::shared_ptr<RSDisplayRenderNode> renderNode = std::make_shared<RSDisplayRenderNode>(id, config);
    if (!renderNode) {
        return nullptr;
    }
    RSRenderNodeDrawableAdapter* displayAdapter = RSDisplayRenderNodeDrawable::OnGenerate(renderNode);
    if (!displayAdapter) {
        return nullptr;
    }
    return static_cast<RSDisplayRenderNodeDrawable*>(displayAdapter);
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
    NodeId defaultDisplayId = 5;
    RSDisplayNodeConfig config;
    RSDisplayRenderNodeDrawable* displayDrawable = GenerateDisplayDrawableById(defaultDisplayId, config);
    ASSERT_NE(displayDrawable, nullptr);
    std::unique_ptr<RSDisplayRenderParams> params = std::make_unique<RSDisplayRenderParams>(defaultDisplayId);
    params->isFirstVisitCrossNodeDisplay_ = false;
    std::vector<std::shared_ptr<RSRenderNodeDrawableAdapter>> surfaceAdapters{nullptr};

    NodeId defaultSurfaceId = 10;
    std::shared_ptr<RSSurfaceRenderNode> renderNode = std::make_shared<RSSurfaceRenderNode>(defaultSurfaceId);
    auto surfaceAdapter = RSSurfaceRenderNodeDrawable::OnGenerate(renderNode);
    // default surface
    surfaceAdapters.emplace_back(surfaceAdapter);
    
    params->SetAllMainAndLeashSurfaceDrawables(surfaceAdapters);
    ScreenInfo screenInfo;
    auto rects = RSUniDirtyComputeUtil::GetCurrentFrameVisibleDirty(*displayDrawable, screenInfo, *params);
    EXPECT_EQ(rects.empty(), true);
    displayDrawable = nullptr;
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
 * @tc.name: GetCurrentFrameVisibleDirty001
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
    effectNode->GetMutableRenderProperties().boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    effectNode->GetMutableRenderProperties().boundsGeo_->absRect_ = DEFAULT_RECT1;  // mock filter rect.

    auto subNode = std::make_shared<RSBaseRenderNode>(++id, context->weak_from_this());
    ASSERT_NE(subNode, nullptr);
    subNode->oldDirtyInSurface_ = DEFAULT_RECT2;  // mock child effect rect.
    effectNode->visibleEffectChild_.emplace(subNode->GetId());
    nodeMap.RegisterRenderNode(subNode);

    FilterDirtyRegionInfo filterInfo =
        RSUniFilterDirtyComputeUtil::GenerateFilterDirtyRegionInfo(*effectNode, std::nullopt);
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
        RSUniFilterDirtyComputeUtil::GenerateFilterDirtyRegionInfo(*testNode, std::nullopt);
    ASSERT_FALSE(filterInfo.filterDirty_.Sub(Occlusion::Region(Occlusion::Rect(DEFAULT_RECT1))).IsEmpty());
}
} // namespace OHOS::Rosen
