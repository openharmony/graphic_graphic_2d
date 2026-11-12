/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>

#include "draw/surface.h"
#include "drawable/rs_effect_drawable_utils.h"
#include "drawable/rs_filter_drawable.h"
#include "drawable/rs_property_drawable_utils.h"
#include "drawable/rs_render_node_drawable_adapter.h"
#include "effect/rs_render_filter_base.h"
#include "effect/rs_render_shape_base.h"
#include "modifier_ng/appearance/rs_behind_window_filter_render_modifier.h"
#include "pipeline/rs_effect_render_node.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "render/rs_drawing_filter.h"
#include "render/rs_path.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSFilterDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

class ConcreteRSRenderNodeDrawableAdapter : public DrawableV2::RSRenderNodeDrawableAdapter {
public:
    explicit ConcreteRSRenderNodeDrawableAdapter(std::shared_ptr<const RSRenderNode> node)
        : RSRenderNodeDrawableAdapter(std::move(node))
    {}
    void Draw(Drawing::Canvas& canvas) {}
};

namespace DrawableV2 {
class MockFilterDrawable : public RSFilterDrawable {
public:
    std::shared_ptr<RSFilter> GetFilter(const RSRenderNode& node) const override
    {
        return node.GetRenderProperties().GetBackgroundFilter();
    }
    bool RenderWithDRM(Drawing::Canvas* canvas, const Drawing::Rect* rect) const override
    {
        return false;
    }
};
} // namespace DrawableV2

void RSFilterDrawableTest::SetUpTestCase() {}
void RSFilterDrawableTest::TearDownTestCase() {}
void RSFilterDrawableTest::SetUp() {}
void RSFilterDrawableTest::TearDown() {}

/**
 * @tc.name: RSFilterDrawable_FilterCacheEnable
 * @tc.desc: Test FilterCacheEnable
 * @tc.type: FUNC
 */
HWTEST_F(RSFilterDrawableTest, RSFilterDrawable_FilterCacheEnable, TestSize.Level1)
{
    // disable filter cache
    RSFilterCacheManager::SetFilterCacheEnabledByCCM(false);
    EXPECT_FALSE(RSFilterCacheManager::IsFilterCacheEnabled());
    auto drawable = std::make_shared<DrawableV2::MockFilterDrawable>();
    ASSERT_NE(drawable, nullptr);
    EXPECT_EQ(drawable->cacheManager_, nullptr);
    EXPECT_EQ(drawable->stagingCacheManager_, nullptr);

    auto node = std::make_shared<RSRenderNode>(1);
    std::shared_ptr<RSFilter> filter =
        std::make_shared<RSDrawingFilter>(std::make_shared<RSRenderFilterParaBase>());
    node->GetMutableRenderProperties().GetEffectProperties().SetBackgroundFilter(filter);
    drawable->OnUpdate(*node);
    drawable->OnSync();

    drawable->stagingCacheManager_ = std::make_unique<RSFilterCacheManager>();
    drawable->OnSync();
    drawable->cacheManager_ = std::make_unique<RSFilterCacheManager>();
    drawable->OnSync();
    drawable->renderFilter_ = nullptr;
    EXPECT_FALSE(drawable->needSync_);
    drawable->OnSync();
    EXPECT_EQ(drawable->renderFilter_, nullptr);
    drawable->renderFilter_ = std::make_shared<RSFilter>();
    drawable->OnSync();
    drawable->renderFilter_ = std::make_shared<RSDrawingFilter>();
    EXPECT_FALSE(drawable->enableEDREffect_);
    drawable->OnSync();
    drawable->enableEDREffect_ = true;
    drawable->OnSync();

    // re-enable
    RSFilterCacheManager::SetFilterCacheEnabledByCCM(true);
}

/**
 * @tc.name: RSFilterDrawable_OnDraw
 * @tc.desc: Test RSFilterDrawable OnDraw
 * @tc.type: FUNC
 */
HWTEST_F(RSFilterDrawableTest, RSFilterDrawable_OnDraw, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::MockFilterDrawable>();
    ASSERT_NE(drawable, nullptr);
    Drawing::Canvas canvas;
    Drawing::Rect rect;
    drawable->renderIntersectWithDRM_ = true;
    drawable->OnDraw(&canvas, &rect);
    drawable->renderFilter_ = std::make_shared<RSDrawingFilter>();
    drawable->OnDraw(&canvas, &rect);
}

/**
 * @tc.name: RSFilterDrawable_CalcFilterRects
 * @tc.desc: Test RSFilterDrawable CalcFilterRects
 * @tc.type: FUNC
 */
HWTEST_F(RSFilterDrawableTest, RSFilterDrawable_CalcFilterRects, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::MockFilterDrawable>();
    ASSERT_NE(drawable, nullptr);
    Drawing::Rect rect = Drawing::Rect(-5.0f, -5.0f, 8.0f, 8.0f);
    Drawing::RectI snapshotRect;
    Drawing::RectI drawRect;
    drawable->CalcFilterRects(nullptr, &rect, snapshotRect, drawRect);

    Drawing::Canvas canvas;
    drawable->CalcFilterRects(&canvas, &rect, snapshotRect, drawRect);
}

/**
 * @tc.name: RSFilterDrawable_SetFilterGeometry
 * @tc.desc: Test RSFilterDrawable SetFilterGeometry
 * @tc.type: FUNC
 */
HWTEST_F(RSFilterDrawableTest, RSFilterDrawable_SetFilterGeometry, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::MockFilterDrawable>();
    ASSERT_NE(drawable, nullptr);
    Drawing::RectI snapshotRect;
    Drawing::RectI drawRect;
    drawable->SetFilterGeometry(nullptr, nullptr, snapshotRect, drawRect);

    Drawing::Canvas canvas;
    drawable->SetFilterGeometry(&canvas, nullptr, snapshotRect, drawRect);

    Drawing::Rect rect;
    EXPECT_EQ(drawable->renderFilter_, nullptr);
    drawable->SetFilterGeometry(&canvas, &rect, snapshotRect, drawRect);
    drawable->renderFilter_ = std::make_shared<RSDrawingFilter>();
    drawable->SetFilterGeometry(&canvas, &rect, snapshotRect, drawRect);

    auto drawingFilter = std::make_shared<RSDrawingFilter>();
    ASSERT_NE(drawingFilter, nullptr);
    drawingFilter->SetNGRenderFilter(RSNGRenderFilterBase::Create(RSNGEffectType::BLUR));
    drawingFilter->GenerateAndUpdateGEVisualEffect();
    drawable->renderFilter_ = drawingFilter;
    drawable->SetFilterGeometry(&canvas, &rect, snapshotRect, drawRect);
}

/**
 * @tc.name: RSFilterDrawable_RenderFilter
 * @tc.desc: Test RSFilterDrawable RenderFilter
 * @tc.type: FUNC
 */
HWTEST_F(RSFilterDrawableTest, RSFilterDrawable_RenderFilter, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::MockFilterDrawable>();
    ASSERT_NE(drawable, nullptr);
    Drawing::RectI snapshotRect;
    Drawing::RectI drawRect;
    drawable->RenderFilter(nullptr, nullptr, snapshotRect, drawRect);

    Drawing::Canvas canvas;
    drawable->RenderFilter(&canvas, nullptr, snapshotRect, drawRect);

    Drawing::Rect rect;
    drawable->RenderFilter(&canvas, &rect, snapshotRect, drawRect);
}

/**
 * @tc.name: RSFilterDrawable_CheckAndEnableEDR
 * @tc.desc: Test RSFilterDrawable CheckAndEnableEDR
 * @tc.type: FUNC
 */
HWTEST_F(RSFilterDrawableTest, RSFilterDrawable_CheckAndEnableEDR, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::MockFilterDrawable>();
    ASSERT_NE(drawable, nullptr);

    drawable->CheckAndEnableEDR(nullptr);
    EXPECT_FALSE(drawable->enableEDREffect_);

    std::shared_ptr<RSFilter> rsFilter = std::make_shared<RSFilter>();
    ASSERT_NE(rsFilter, nullptr);
    drawable->CheckAndEnableEDR(rsFilter);
    EXPECT_FALSE(drawable->enableEDREffect_);

    auto ngFilter = std::make_shared<RSNGRenderEdgeLightFilter>();
    ngFilter->Setter<EdgeLightColorRenderTag>(Vector4f(0.5f, 1.5f, 0.5f, 1.0f));
    auto drawingFilter = std::make_shared<RSDrawingFilter>();
    ASSERT_NE(drawingFilter, nullptr);
    drawingFilter->SetNGRenderFilter(ngFilter);
    drawable->CheckAndEnableEDR(drawingFilter);
    EXPECT_TRUE(drawable->enableEDREffect_);
}

/**
 * @tc.name: RSFilterDrawable_ReportBlur
 * @tc.desc: Test RSFilterDrawable ReportBlur
 * @tc.type: FUNC
 */
HWTEST_F(RSFilterDrawableTest, RSFilterDrawable_ReportBlur, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::MockFilterDrawable>();
    ASSERT_NE(drawable, nullptr);
    drawable->ReportBlur(nullptr, 0);
    Drawing::Rect rect;
    drawable->ReportBlur(&rect, 0);
}

/**
 * @tc.name: RSFilterDrawable_SetBlurIntersectWithDRM
 * @tc.desc: Test RSFilterDrawable SetBlurIntersectWithDRM
 * @tc.type: FUNC
 */
HWTEST_F(RSFilterDrawableTest, RSFilterDrawable_SetBlurIntersectWithDRM, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::MockFilterDrawable>();
    ASSERT_NE(drawable, nullptr);
    bool intersectWithDRM = true;
    bool isDark = true;
    drawable->SetBlurIntersectWithDRM(intersectWithDRM, isDark);
    EXPECT_EQ(intersectWithDRM, drawable->stagingIntersectWithDRM_);
    EXPECT_EQ(isDark, drawable->stagingIsDarkColorMode_);
}

/**
 * @tc.name: RSFilterDrawable_UpdateFilterRectInfo
 * @tc.desc: Test RSFilterDrawable UpdateFilterRectInfo
 * @tc.type: FUNC
 */
HWTEST_F(RSFilterDrawableTest, RSFilterDrawable_UpdateFilterRectInfo, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::MockFilterDrawable>();
    ASSERT_NE(drawable, nullptr);
    RectF emptyBound;
    drawable->UpdateFilterRectInfo(emptyBound, nullptr);

    std::shared_ptr<RSFilter> rsFilter = std::make_shared<RSFilter>();
    drawable->UpdateFilterRectInfo(emptyBound, rsFilter);

    auto renderFilter = RSNGRenderFilterBase::Create(RSNGEffectType::MAGNIFIER);
    auto magnifier = std::static_pointer_cast<RSNGRenderMagnifierFilter>(renderFilter);
    magnifier->Setter<MagnifierOffsetXRenderTag>(10.0f);
    magnifier->Setter<MagnifierOffsetYRenderTag>(10.0f);
    auto drawingFilter = std::make_shared<RSDrawingFilter>();
    drawingFilter->SetNGRenderFilter(renderFilter);
    RectF bound = RectF(0.0f, 0.0f, 100.0f, 100.0f);
    drawable->UpdateFilterRectInfo(bound, drawingFilter);
}

/**
 * @tc.name: RSFilterDrawable_CalVisibleRect
 * @tc.desc: Test RSFilterDrawable CalVisibleRect
 * @tc.type: FUNC
 */
HWTEST_F(RSFilterDrawableTest, RSFilterDrawable_CalVisibleRect, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::MockFilterDrawable>();
    ASSERT_NE(drawable, nullptr);
    Drawing::Matrix absMatrix;
    std::optional<RectI> nullClipRect = std::nullopt;
    RectF relativeRect = RectF(0.0f, 0.0f, 100.0f, 100.0f);

    drawable->stagingRelativeRectInfo_ = nullptr;
    drawable->CalVisibleRect(absMatrix, nullClipRect, relativeRect);

    drawable->stagingRelativeRectInfo_ = std::make_unique<DrawableV2::RSFilterDrawable::FilterRectInfo>();
    drawable->CalVisibleRect(absMatrix, nullClipRect, relativeRect);

    std::optional<RectI> clipRect = RectI(0, 0, 100, 100);
    drawable->CalVisibleRect(absMatrix, clipRect, relativeRect);
}

/**
 * @tc.name: RSFilterDrawable_GetVisibleRegion
 * @tc.desc: Test GetVisibleRegion for total, snapshot, draw and last visible region
 * @tc.type: FUNC
 */
HWTEST_F(RSFilterDrawableTest, RSFilterDrawable_GetVisibleRegion, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::MockFilterDrawable>();
    ASSERT_NE(drawable, nullptr);
    RectI defaultValue(0, 0, 50, 50);
    drawable->stagingVisibleRectInfo_ = nullptr;
    EXPECT_EQ(defaultValue, drawable->GetVisibleSnapshotRegion(defaultValue));
    EXPECT_EQ(defaultValue, drawable->GetVisibleDrawRegion(defaultValue));
    EXPECT_EQ(defaultValue, drawable->GetVisibleTotalRegion(defaultValue));

    RectI snapshotRect(0, 0, 100, 100);
    RectI drawRect(0, 0, 100, 100);
    RectI totalRect = snapshotRect.JoinRect(drawRect);

    drawable->stagingVisibleRectInfo_ = std::make_unique<DrawableV2::RSFilterDrawable::FilterVisibleRectInfo>();
    drawable->stagingVisibleRectInfo_->snapshotRect_ = snapshotRect;
    drawable->stagingVisibleRectInfo_->drawRect_ = drawRect;
    drawable->stagingVisibleRectInfo_->totalRect_ = totalRect;
    EXPECT_EQ(snapshotRect, drawable->GetVisibleSnapshotRegion(defaultValue));
    EXPECT_EQ(drawRect, drawable->GetVisibleDrawRegion(defaultValue));
    EXPECT_EQ(totalRect, drawable->GetVisibleTotalRegion(defaultValue));

    drawable->lastStagingVisibleRectInfo_ = nullptr;
    EXPECT_EQ(defaultValue, drawable->GetLastVisibleSnapshotRegion(defaultValue));
    EXPECT_EQ(defaultValue, drawable->GetLastVisibleDrawRegion(defaultValue));
    drawable->lastStagingVisibleRectInfo_ = std::move(drawable->stagingVisibleRectInfo_);
    EXPECT_EQ(snapshotRect, drawable->GetLastVisibleSnapshotRegion(defaultValue));
    EXPECT_EQ(drawRect, drawable->GetLastVisibleDrawRegion(defaultValue));
}

/**
 * @tc.name: RSFilterDrawable_MarkMethod
 * @tc.desc: Test RSFilterDrawable MarkMethod
 * @tc.type: FUNC
 */
HWTEST_F(RSFilterDrawableTest, RSFilterDrawable_MarkMethod, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::MockFilterDrawable>();
    ASSERT_NE(drawable, nullptr);
    ASSERT_NE(drawable->stagingCacheManager_, nullptr);
    drawable->MarkFilterRegionChanged();
    drawable->MarkFilterRegionInteractWithDirty();
    drawable->MarkFilterRegionIsLargeArea();
    drawable->MarkFilterForceUseCache(true);
    drawable->MarkFilterForceClearCache();
    drawable->MarkEffectNode();
    drawable->MarkRotationChanged();
    drawable->MarkNodeIsOccluded(true);
    drawable->MarkForceClearCacheWithLastFrame();
    drawable->MarkNeedClearFilterCache();
    drawable->MarkInForegroundFilterAndCheckNeedForceClearCache(INVALID_NODEID);
    drawable->MarkDebugEnabled();
    drawable->MarkAIBarCacheStatus(true);
    drawable->stagingCacheManager_->filterType_ = RSFilter::NONE;
    drawable->MarkForceReduceAIBarCacheInterval(true);
    drawable->stagingCacheManager_->filterType_ = RSFilter::AIBAR;
    drawable->MarkForceReduceAIBarCacheInterval(true);
    drawable->MarkForceReduceAIBarCacheInterval(false);

    drawable->stagingCacheManager_ = nullptr;
    drawable->MarkFilterRegionChanged();
    drawable->MarkFilterRegionInteractWithDirty();
    drawable->MarkFilterRegionIsLargeArea();
    drawable->MarkFilterForceUseCache(true);
    drawable->MarkFilterForceClearCache();
    drawable->MarkEffectNode();
    drawable->MarkRotationChanged();
    drawable->MarkNodeIsOccluded(true);
    drawable->MarkForceClearCacheWithLastFrame();
    drawable->MarkNeedClearFilterCache();
    drawable->MarkInForegroundFilterAndCheckNeedForceClearCache(INVALID_NODEID);
    drawable->MarkDebugEnabled();
    drawable->MarkAIBarCacheStatus(true);
    drawable->MarkForceReduceAIBarCacheInterval(true);
}

/**
 * @tc.name: RSFilterDrawable_IsMethodCalledFromMain
 * @tc.desc: Test RSFilterDrawable IsMethodCalledFromMain
 * @tc.type: FUNC
 */
HWTEST_F(RSFilterDrawableTest, RSFilterDrawable_IsMethodCalledFromMain, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::MockFilterDrawable>();
    ASSERT_NE(drawable, nullptr);
    ASSERT_NE(drawable->stagingCacheManager_, nullptr);
    drawable->IsFilterCacheValid();
    drawable->IsForceClearFilterCache();
    drawable->IsForceUseFilterCache();
    drawable->IsLastHpaeClearCache();
    drawable->IsPendingPurge();
    drawable->IsSkippingFrame();
    drawable->IsAIBarFilter();
    drawable->IsDrawLargeAreaBlur();
    drawable->IsDrawLargeAreaBlurPrecisely();
    drawable->IsFilterCacheValidForOcclusion();
    drawable->IsFilterCacheValidForPartialRender();

    drawable->stagingCacheManager_ = nullptr;
    drawable->IsFilterCacheValid();
    drawable->IsForceClearFilterCache();
    drawable->IsForceUseFilterCache();
    drawable->IsLastHpaeClearCache();
    drawable->IsPendingPurge();
    drawable->IsSkippingFrame();
    drawable->IsAIBarFilter();
    drawable->IsDrawLargeAreaBlur();
    drawable->IsDrawLargeAreaBlurPrecisely();
}

/**
 * @tc.name: RSFilterDrawable_IsMethodCalledFromRender
 * @tc.desc: Test RSFilterDrawable IsMethodCalledFromRender
 * @tc.type: FUNC
 */
HWTEST_F(RSFilterDrawableTest, RSFilterDrawable_IsMethodCalledFromRender, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::MockFilterDrawable>();
    ASSERT_NE(drawable, nullptr);
    ASSERT_NE(drawable->cacheManager_, nullptr);
    drawable->IsFilterCacheValidForOcclusion();
    drawable->IsFilterCacheValidForPartialRender();

    drawable->cacheManager_ = nullptr;
    drawable->IsFilterCacheValidForOcclusion();
    drawable->IsFilterCacheValidForPartialRender();
}

/**
 * @tc.name: RSFilterDrawable_GetFilterCachedRegion
 * @tc.desc: Test RSFilterDrawable GetFilterCachedRegion
 * @tc.type: FUNC
 */
HWTEST_F(RSFilterDrawableTest, RSFilterDrawable_GetFilterCachedRegion, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::MockFilterDrawable>();
    ASSERT_NE(drawable, nullptr);
    ASSERT_NE(drawable->cacheManager_, nullptr);
    drawable->GetFilterCachedRegion();

    drawable->cacheManager_ = nullptr;
    drawable->GetFilterCachedRegion();
}

/**
 * @tc.name: RSFilterDrawable_GetRelativeRect
 * @tc.desc: Test RSFilterDrawable GetRelativeRect
 * @tc.type: FUNC
 */
HWTEST_F(RSFilterDrawableTest, RSFilterDrawable_GetRelativeRect, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::MockFilterDrawable>();
    ASSERT_NE(drawable, nullptr);
    RectF defaultValue(0, 0, 50, 50);
    EXPECT_EQ(defaultValue, drawable->GetRelativeRect(nullptr, EffectRectType::TOTAL, defaultValue));

    std::unique_ptr<DrawableV2::RSFilterDrawable::FilterRectInfo> rectInfo =
        std::make_unique<DrawableV2::RSFilterDrawable::FilterRectInfo>();
    RectF snapshotRect = RectF(0, 0, 100, 100);
    RectF drawRect = RectF(0, 0, 100, 100);
    RectF totalRect = drawRect.JoinRect(snapshotRect);
    rectInfo->snapshotRect_ = snapshotRect;
    rectInfo->drawRect_ = drawRect;
    EXPECT_EQ(snapshotRect, drawable->GetRelativeRect(rectInfo, EffectRectType::SNAPSHOT, defaultValue));
    EXPECT_EQ(drawRect, drawable->GetRelativeRect(rectInfo, EffectRectType::DRAW, defaultValue));
    EXPECT_EQ(totalRect, drawable->GetRelativeRect(rectInfo, EffectRectType::TOTAL, defaultValue));
    EXPECT_EQ(defaultValue, drawable->GetRelativeRect(rectInfo, static_cast<EffectRectType>(3), defaultValue));
}

/**
 * @tc.name: RSFilterDrawable_GetClipBound
 * @tc.desc: Test RSFilterDrawable GetClipBound
 * @tc.type: FUNC
 */
HWTEST_F(RSFilterDrawableTest, RSFilterDrawable_GetClipBound, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::MockFilterDrawable>();
    ASSERT_NE(drawable, nullptr);
    Drawing::Canvas canvas1(400, 400);
    canvas1.ClipRect(Drawing::Rect(10, 10, 100, 100));
    EXPECT_EQ(Drawing::Rect(10, 10, 100, 100), drawable->GetClipBound(canvas1));

    Drawing::Canvas canvas2(400, 400);
    Drawing::Matrix matrix;
    matrix.Scale(2.0, 2.0, 0, 0);
    canvas2.ConcatMatrix(matrix);
    canvas2.ClipRect(Drawing::Rect(10, 10, 50, 50));
    EXPECT_EQ(Drawing::Rect(20, 20, 100, 100), drawable->GetClipBound(canvas2));
}

/**
 * @tc.name: RSFilterDrawable_GetAbsRenderEffectRect
 * @tc.desc: Test RSFilterDrawable GetAbsRenderEffectRect
 * @tc.type: FUNC
 */
HWTEST_F(RSFilterDrawableTest, RSFilterDrawable_GetAbsRenderEffectRect, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::MockFilterDrawable>();
    ASSERT_NE(drawable, nullptr);
    drawable->renderRelativeRectInfo_ = nullptr;

    Drawing::Canvas canvas(400, 400);
    Drawing::RectI clipBound(0, 0, 400, 400);
    RectF noIntersectBound(500.0f, 500.0f, 100.0f, 100.0f);
    RectF intersectBound(300.0f, 300.0f, 200.0f, 200.0f);
    EXPECT_EQ(Drawing::RectI(),
        drawable->GetAbsRenderEffectRect(canvas, EffectRectType::SNAPSHOT, noIntersectBound, clipBound));
    EXPECT_EQ(Drawing::RectI(300, 300, 400, 400),
        drawable->GetAbsRenderEffectRect(canvas, EffectRectType::SNAPSHOT, intersectBound, clipBound));

    RectF bound(10.0f, 10.0f, 100.0f, 100.0f);
    EXPECT_EQ(Drawing::RectI(10, 10, 110, 110),
        drawable->GetAbsRenderEffectRect(canvas, EffectRectType::SNAPSHOT, bound, clipBound));
    EXPECT_EQ(Drawing::RectI(10, 10, 110, 110),
        drawable->GetAbsRenderEffectRect(canvas, EffectRectType::DRAW, bound, clipBound));

    drawable->renderRelativeRectInfo_ = std::make_unique<DrawableV2::RSFilterDrawable::FilterRectInfo>();
    drawable->renderRelativeRectInfo_->snapshotRect_ = RectF(0.0f, 0.0f, 50.0f, 50.0f);
    drawable->renderRelativeRectInfo_->drawRect_ = RectF(20.0f, 20.0f, 80.0f, 80.0f);
    EXPECT_EQ(Drawing::RectI(0, 0, 50, 50),
        drawable->GetAbsRenderEffectRect(canvas, EffectRectType::SNAPSHOT, bound, clipBound));
    EXPECT_EQ(Drawing::RectI(20, 20, 100, 100),
        drawable->GetAbsRenderEffectRect(canvas, EffectRectType::DRAW, bound, clipBound));
}

/**
 * @tc.name: RSBackgroundFilterDrawable_OnGenerate
 * @tc.desc: Test RSBackgroundFilterDrawable OnGenerate
 * @tc.type: FUNC
 */
HWTEST_F(RSFilterDrawableTest, RSBackgroundFilterDrawable_OnGenerate, TestSize.Level1)
{
    std::shared_ptr<RSFilter> backgroundFilter =
        std::make_shared<RSDrawingFilter>(std::make_shared<RSRenderFilterParaBase>());

    auto effectNode = std::make_shared<RSEffectRenderNode>(1);
    std::shared_ptr<RSDrawable> bgeDrawable = DrawableV2::RSBackgroundFilterDrawable::OnGenerate(*effectNode);
    EXPECT_EQ(bgeDrawable, nullptr);
    effectNode->GetMutableRenderProperties().GetEffectProperties().SetBackgroundFilter(backgroundFilter);
    bgeDrawable = DrawableV2::RSBackgroundFilterDrawable::OnGenerate(*effectNode);
    EXPECT_NE(bgeDrawable, nullptr);

    auto node = std::make_shared<RSRenderNode>(2);
    std::shared_ptr<RSDrawable> bgfDrawable = DrawableV2::RSBackgroundFilterDrawable::OnGenerate(*node);
    EXPECT_EQ(bgfDrawable, nullptr);
    node->GetMutableRenderProperties().GetEffectProperties().SetBackgroundFilter(backgroundFilter);
    bgfDrawable = DrawableV2::RSBackgroundFilterDrawable::OnGenerate(*node);
    EXPECT_NE(bgfDrawable, nullptr);
}

/**
 * @tc.name: RSBackgroundFilterDrawable_OnUpdate
 * @tc.desc: Test RSBackgroundFilterDrawable OnUpdate
 * @tc.type: FUNC
 */
HWTEST_F(RSFilterDrawableTest, RSBackgroundFilterDrawable_OnUpdate, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSBackgroundFilterDrawable>();
    ASSERT_NE(drawable, nullptr);

    auto rsContext = std::make_shared<RSContext>();
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(0, rsContext);

    EXPECT_FALSE(surfaceNode->NeedDrawBehindWindow());
    drawable->OnUpdate(*surfaceNode);
    EXPECT_FALSE(drawable->stagingNeedDrawBehindWindow_);

    auto modifier = std::make_shared<ModifierNG::RSBehindWindowFilterRenderModifier>();
    surfaceNode->AddModifier(modifier);
    surfaceNode->AddChildBlurBehindWindow(1);
    EXPECT_TRUE(surfaceNode->NeedDrawBehindWindow());
    drawable->OnUpdate(*surfaceNode);
    EXPECT_TRUE(drawable->stagingNeedDrawBehindWindow_);


    std::shared_ptr<RSFilter> backgroundFilter =
        std::make_shared<RSDrawingFilter>(std::make_shared<RSRenderFilterParaBase>());
    surfaceNode->GetMutableRenderProperties().GetEffectProperties().SetBackgroundFilter(backgroundFilter);
    drawable->OnUpdate(*surfaceNode);
    EXPECT_FALSE(drawable->stagingNeedDrawBehindWindow_);
}

/**
 * @tc.name: RSBackgroundFilterDrawable_OnFilterUpdated
 * @tc.desc: Test RSBackgroundFilterDrawable OnFilterUpdated
 * @tc.type: FUNC
 */
HWTEST_F(RSFilterDrawableTest, RSBackgroundFilterDrawable_OnFilterUpdated, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSBackgroundFilterDrawable>();
    ASSERT_NE(drawable, nullptr);
    auto node = std::make_shared<RSRenderNode>(2);
    drawable->OnFilterUpdated(*node, nullptr);
    auto rsFilter = std::make_shared<RSFilter>();
    drawable->OnFilterUpdated(*node, rsFilter);
    auto drawingFilter = std::make_shared<RSDrawingFilter>();
    drawable->OnFilterUpdated(*node, drawingFilter);
}

/**
 * @tc.name: RSBackgroundFilterDrawable_OnSync
 * @tc.desc: Test RSBackgroundFilterDrawable OnSync
 * @tc.type: FUNC
 */
HWTEST_F(RSFilterDrawableTest, RSBackgroundFilterDrawable_OnSync, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSBackgroundFilterDrawable>();
    ASSERT_NE(drawable, nullptr);
    EXPECT_FALSE(drawable->needSync_);
    drawable->OnSync();

    drawable->stagingNeedDrawBehindWindow_ = true;
    drawable->stagingDrawBehindWindowRegion_ = RectI(0, 0, 100, 100);
    drawable->needSync_ = true;
    drawable->OnSync();
    EXPECT_TRUE(drawable->needDrawBehindWindow_);
    EXPECT_EQ(drawable->drawBehindWindowRegion_, RectI(0, 0, 100, 100));

    drawable->needSync_ = true;
    drawable->renderFilter_ = std::make_shared<RSFilter>();
    drawable->OnSync();

    drawable->needSync_ = true;
    drawable->renderFilter_ = std::make_shared<RSDrawingFilter>();
    drawable->OnSync();

    drawable->needSync_ = true;
    drawable->enableEDREffect_ = true;
    drawable->OnSync();
}

/**
 * @tc.name: RSBackgroundFilterDrawable_OnDraw
 * @tc.desc: Test RSBackgroundFilterDrawable OnDraw
 * @tc.type: FUNC
 */
HWTEST_F(RSFilterDrawableTest, RSBackgroundFilterDrawable_OnDraw, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSBackgroundFilterDrawable>();
    ASSERT_NE(drawable, nullptr);
    EXPECT_FALSE(drawable->needDrawBehindWindow_);
    EXPECT_FALSE(drawable->renderIntersectWithDRM_);

    drawable->OnDraw(nullptr, nullptr);
    Drawing::Canvas canvas;
    drawable->OnDraw(&canvas, nullptr);
    Drawing::Rect rect;
    drawable->OnDraw(&canvas, &rect);

    drawable->needDrawBehindWindow_ = true;
    drawable->OnDraw(&canvas, &rect);
    drawable->needDrawBehindWindow_ = false;
    drawable->renderIntersectWithDRM_ = true;
    drawable->OnDraw(&canvas, &rect);
    drawable->renderFilter_ = std::make_shared<RSDrawingFilter>();
    drawable->OnDraw(&canvas, &rect);
}

/**
 * @tc.name: RSBackgroundFilterDrawable_GetBehindWindowFilter
 * @tc.desc: Test RSBackgroundFilterDrawable GetBehindWindowFilter
 * @tc.type: FUNC
 */
HWTEST_F(RSFilterDrawableTest, RSBackgroundFilterDrawable_GetBehindWindowFilter, TestSize.Level1)
{
    // no modifier -> nullptr
    auto nodeWithoutModifier = std::make_shared<RSRenderNode>(1);
    EXPECT_EQ(DrawableV2::RSBackgroundFilterDrawable::GetBehindWindowFilter(*nodeWithoutModifier), nullptr);

    // modifier without properties -> filter generated with default values
    auto nodeWithoutProperty = std::make_shared<RSRenderNode>(2);
    auto emptyModifier = std::make_shared<ModifierNG::RSBehindWindowFilterRenderModifier>();
    nodeWithoutProperty->AddModifier(emptyModifier);
    EXPECT_NE(DrawableV2::RSBackgroundFilterDrawable::GetBehindWindowFilter(*nodeWithoutProperty), nullptr);

    // modifier with partial properties -> filter generated, missing properties fall back to defaults
    auto nodeWithPartialProperty = std::make_shared<RSRenderNode>(3);
    auto partialModifier = std::make_shared<ModifierNG::RSBehindWindowFilterRenderModifier>();
    partialModifier->AttachProperty(ModifierNG::RSPropertyType::BEHIND_WINDOW_FILTER_RADIUS,
        std::make_shared<RSRenderProperty<float>>(10.f, 0));
    nodeWithPartialProperty->AddModifier(partialModifier);
    EXPECT_NE(DrawableV2::RSBackgroundFilterDrawable::GetBehindWindowFilter(*nodeWithPartialProperty), nullptr);

    // modifier with all properties -> filter generated
    auto nodeWithFullProperty = std::make_shared<RSRenderNode>(4);
    auto modifier = std::make_shared<ModifierNG::RSBehindWindowFilterRenderModifier>();
    modifier->AttachProperty(ModifierNG::RSPropertyType::BEHIND_WINDOW_FILTER_RADIUS,
        std::make_shared<RSRenderProperty<float>>(20.f, 0));
    modifier->AttachProperty(ModifierNG::RSPropertyType::BEHIND_WINDOW_FILTER_SATURATION,
        std::make_shared<RSRenderProperty<float>>(0.8f, 0));
    modifier->AttachProperty(ModifierNG::RSPropertyType::BEHIND_WINDOW_FILTER_BRIGHTNESS,
        std::make_shared<RSRenderProperty<float>>(1.2f, 0));
    modifier->AttachProperty(ModifierNG::RSPropertyType::BEHIND_WINDOW_FILTER_MASK_COLOR,
        std::make_shared<RSRenderProperty<RSColor>>(RSColor(255, 255, 255), 0));
    nodeWithFullProperty->AddModifier(modifier);
    auto filter = DrawableV2::RSBackgroundFilterDrawable::GetBehindWindowFilter(*nodeWithFullProperty);
    EXPECT_NE(filter, nullptr);
}

/**
 * @tc.name: RSBackgroundFilterDrawable_GetBehindWindowFilterProperty
 * @tc.desc: Test RSBackgroundFilterDrawable GetBehindWindowFilterProperty
 * @tc.type: FUNC
 */
HWTEST_F(RSFilterDrawableTest, RSBackgroundFilterDrawable_GetBehindWindowFilterProperty, TestSize.Level1)
{
    // node without modifier -> false and default value kept
    auto plainNode = std::make_shared<RSRenderNode>(1);
    float radius = 10.f;
    EXPECT_FALSE(DrawableV2::RSBackgroundFilterDrawable::GetBehindWindowFilterProperty(
    *plainNode, ModifierNG::RSPropertyType::BEHIND_WINDOW_FILTER_RADIUS, radius));
    EXPECT_EQ(radius, 10.f);

    // node with modifier but property missing -> true and value reset to default T()
    auto modifier = std::make_shared<ModifierNG::RSBehindWindowFilterRenderModifier>();
    auto nodeWithModifier = std::make_shared<RSRenderNode>(2);
    nodeWithModifier->AddModifier(modifier);
    float saturation = 2.f;
    EXPECT_TRUE(DrawableV2::RSBackgroundFilterDrawable::GetBehindWindowFilterProperty(
        *nodeWithModifier, ModifierNG::RSPropertyType::BEHIND_WINDOW_FILTER_SATURATION, saturation));
    EXPECT_EQ(saturation, 0.f);

    // node with modifier and attached properties -> true and value fetched, one call per property type
    auto nodeWithProperties = std::make_shared<RSRenderNode>(3);
    auto propertyModifier = std::make_shared<ModifierNG::RSBehindWindowFilterRenderModifier>();
    propertyModifier->AttachProperty(ModifierNG::RSPropertyType::BEHIND_WINDOW_FILTER_RADIUS,
        std::make_shared<RSRenderProperty<float>>(20.f, 0));
    propertyModifier->AttachProperty(ModifierNG::RSPropertyType::BEHIND_WINDOW_FILTER_MASK_COLOR,
        std::make_shared<RSRenderProperty<RSColor>>(RSColor(11, 22, 33, 255), 0));
    nodeWithProperties->AddModifier(propertyModifier);

    float propertyRadius = 0.f;
    EXPECT_TRUE(DrawableV2::RSBackgroundFilterDrawable::GetBehindWindowFilterProperty(
        *nodeWithProperties, ModifierNG::RSPropertyType::BEHIND_WINDOW_FILTER_RADIUS, propertyRadius));
    EXPECT_EQ(propertyRadius, 20.f);

    RSColor maskColor;
    EXPECT_TRUE(DrawableV2::RSBackgroundFilterDrawable::GetBehindWindowFilterProperty(
        *nodeWithProperties, ModifierNG::RSPropertyType::BEHIND_WINDOW_FILTER_MASK_COLOR, maskColor));
    EXPECT_EQ(maskColor.GetRed(), 11);
    EXPECT_EQ(maskColor.GetGreen(), 22);
    EXPECT_EQ(maskColor.GetBlue(), 33);
    EXPECT_EQ(maskColor.GetAlpha(), 255);
}

/**
 * @tc.name: RSBackgroundFilterDrawable_DrawBehindWindow
 * @tc.desc: Test RSBackgroundFilterDrawable DrawBehindWindow
 * @tc.type: FUNC
 */
HWTEST_F(RSFilterDrawableTest, RSBackgroundFilterDrawable_DrawBehindWindow, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSBackgroundFilterDrawable>();
    ASSERT_NE(drawable, nullptr);
    drawable->needDrawBehindWindow_ = true;
    drawable->drawBehindWindowRegion_ = RectI(0, 0, 10, 10);
    drawable->renderNodeId_ = 1;
    drawable->renderFilter_ = std::make_shared<RSDrawingFilter>(std::make_shared<RSRenderFilterParaBase>());
    drawable->cacheManager_ = nullptr;
    DrawableV2::RSEffectDrawableUtils::GetAndResetBlurCnt();

    drawable->OnDraw(nullptr, nullptr);
    EXPECT_EQ(DrawableV2::RSEffectDrawableUtils::GetAndResetBlurCnt(), 0);

    // rect is nullptr -> early return, no blur is counted.
    Drawing::Canvas plainCanvas;
    drawable->OnDraw(&plainCanvas, nullptr);
    EXPECT_EQ(DrawableV2::RSEffectDrawableUtils::GetAndResetBlurCnt(), 0);

    // canvas has no surface -> early return, no blur is counted.
    Drawing::Rect rect(0.0f, 0.0f, 10.0f, 10.0f);
    drawable->OnDraw(&plainCanvas, &rect);
    EXPECT_EQ(DrawableV2::RSEffectDrawableUtils::GetAndResetBlurCnt(), 0);

    auto surface = Drawing::Surface::MakeRasterN32Premul(10, 10);
    ASSERT_NE(surface, nullptr);
    RSPaintFilterCanvas paintFilterCanvas(surface.get());

    // window freeze capture -> early return, no cache data and no blur.
    paintFilterCanvas.SetIsWindowFreezeCapture(true);
    drawable->OnDraw(&paintFilterCanvas, &rect);
    EXPECT_EQ(paintFilterCanvas.GetCacheBehindWindowData(), nullptr);
    EXPECT_EQ(DrawableV2::RSEffectDrawableUtils::GetAndResetBlurCnt(), 0);

    // drawing cache -> cache behind window data is recorded and no blur is counted.
    paintFilterCanvas.SetIsWindowFreezeCapture(false);
    paintFilterCanvas.SetIsDrawingCache(true);
    drawable->OnDraw(&paintFilterCanvas, &rect);
    auto cacheData = paintFilterCanvas.GetCacheBehindWindowData();
    ASSERT_NE(cacheData, nullptr);
    EXPECT_EQ(cacheData->filter_, drawable->renderFilter_);
    EXPECT_EQ(cacheData->rect_, Drawing::Rect(0.0f, 0.0f, 10.0f, 10.0f));
    EXPECT_EQ(DrawableV2::RSEffectDrawableUtils::GetAndResetBlurCnt(), 0);

    // normal path -> background effect is drawn and behind window data is produced.
    paintFilterCanvas.SetIsDrawingCache(false);
    drawable->OnDraw(&paintFilterCanvas, &rect);
    EXPECT_NE(paintFilterCanvas.GetBehindWindowData(), nullptr);
    EXPECT_GT(DrawableV2::RSEffectDrawableUtils::GetAndResetBlurCnt(), 0);
}

/**
 * @tc.name: RSBackgroundFilterDrawable_RenderWithDRM
 * @tc.desc: Test RSBackgroundFilterDrawable RenderWithDRM
 * @tc.type: FUNC
 */
HWTEST_F(RSFilterDrawableTest, RSBackgroundFilterDrawable_RenderWithDRM, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSBackgroundFilterDrawable>();
    ASSERT_NE(drawable, nullptr);
    EXPECT_FALSE(drawable->RenderWithDRM(nullptr, nullptr));
    Drawing::Canvas canvas;
    EXPECT_TRUE(drawable->RenderWithDRM(&canvas, nullptr));
    Drawing::Rect rect;
    EXPECT_TRUE(drawable->RenderWithDRM(&canvas, &rect));
}

/**
 * @tc.name: RSBackgroundEffectDrawable_OnFilterUpdated
 * @tc.desc: Test RSBackgroundEffectDrawable OnFilterUpdated
 * @tc.type: FUNC
 */
HWTEST_F(RSFilterDrawableTest, RSBackgroundEffectDrawable_OnFilterUpdated, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSBackgroundEffectDrawable>();
    ASSERT_NE(drawable, nullptr);
    auto node = std::make_shared<RSRenderNode>(1);
    drawable->OnFilterUpdated(*node, nullptr);
    auto filter = std::make_shared<RSDrawingFilter>(std::make_shared<RSRenderFilterParaBase>());
    filter->renderFilter_ = RSNGRenderFilterBase::Create(RSNGEffectType::BLUR);
    node->GetMutableRenderProperties().GetEffectProperties().SetBackgroundFilter(filter);
    drawable->OnFilterUpdated(*node, filter);
    filter->renderFilter_ = RSNGRenderFilterBase::Create(RSNGEffectType::FROSTED_GLASS_BLUR);
    node->GetMutableRenderProperties().GetEffectProperties().SetBackgroundFilter(filter);
    drawable->OnFilterUpdated(*node, filter);
}

/**
 * @tc.name: RSBackgroundEffectDrawable_RenderFilter
 * @tc.desc: Test RSBackgroundEffectDrawable RenderFilter
 * @tc.type: FUNC
 */
HWTEST_F(RSFilterDrawableTest, RSBackgroundEffectDrawable_RenderFilter, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSBackgroundEffectDrawable>();
    ASSERT_NE(drawable, nullptr);
    Drawing::Canvas canvas;
    Drawing::Rect rect;
    Drawing::RectI snapshotRect;
    Drawing::RectI drawRect;
    drawable->RenderFilter(nullptr, &rect, snapshotRect, drawRect);
    EXPECT_TRUE(snapshotRect.IsEmpty());
    drawable->RenderFilter(&canvas, &rect, snapshotRect, drawRect);
    snapshotRect = Drawing::RectI(0, 0, 100, 100);
    EXPECT_FALSE(snapshotRect.IsEmpty());
    drawable->RenderFilter(&canvas, &rect, snapshotRect, drawRect);
}

/**
 * @tc.name: RSBackgroundEffectDrawable_RenderWithDRM
 * @tc.desc: Test RSBackgroundEffectDrawable RenderWithDRM
 * @tc.type: FUNC
 */
HWTEST_F(RSFilterDrawableTest, RSBackgroundEffectDrawable_RenderWithDRM, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSBackgroundEffectDrawable>();

    ASSERT_NE(drawable, nullptr);
    EXPECT_FALSE(drawable->RenderWithDRM(nullptr, nullptr));
    Drawing::Canvas canvas;
    EXPECT_FALSE(drawable->RenderWithDRM(&canvas, nullptr));
    Drawing::Rect rect;
    EXPECT_FALSE(drawable->RenderWithDRM(&canvas, &rect));
}

/**
 * @tc.name: RSBackgroundEffectDrawable_SetFilterGeometry
 * @tc.desc: Test RSBackgroundEffectDrawable SetFilterGeometry
 * @tc.type: FUNC
 */
HWTEST_F(RSFilterDrawableTest, RSBackgroundEffectDrawable_SetFilterGeometry, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSBackgroundEffectDrawable>();
    ASSERT_NE(drawable, nullptr);

    Drawing::RectI snapshotRect;
    Drawing::RectI drawRect;
    drawable->SetFilterGeometry(nullptr, nullptr, snapshotRect, drawRect);

    Drawing::Canvas canvas;
    drawable->SetFilterGeometry(&canvas, nullptr, snapshotRect, drawRect);

    Drawing::Rect rect;
    EXPECT_EQ(drawable->renderFilter_, nullptr);
    drawable->SetFilterGeometry(&canvas, &rect, snapshotRect, drawRect);
    drawable->renderFilter_ = std::make_shared<RSDrawingFilter>();
    drawable->SetFilterGeometry(&canvas, &rect, snapshotRect, drawRect);
}

/**
 * @tc.name: RSBackgroundEffectDrawable_GetClipBound
 * @tc.desc: Test RSBackgroundEffectDrawable GetClipBound
 * @tc.type: FUNC
 */
HWTEST_F(RSFilterDrawableTest, RSBackgroundEffectDrawable_GetClipBound, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSBackgroundEffectDrawable>();
    ASSERT_NE(drawable, nullptr);

    Drawing::Canvas plainCanvas;
    auto clipBound = drawable->GetClipBound(plainCanvas);
    EXPECT_TRUE(clipBound.IsEmpty());

    auto surface = Drawing::Surface::MakeRasterN32Premul(10, 10);
    ASSERT_NE(surface, nullptr);
    RSPaintFilterCanvas paintFilterCanvas(surface.get());

    paintFilterCanvas.SetFilterClipBounds(Drawing::RectI(0, 0, 5, 5));
    clipBound = drawable->GetClipBound(paintFilterCanvas);
    EXPECT_EQ(clipBound, Drawing::RectI(0, 0, 10, 10));

    drawable->renderRelativeRectInfo_ = std::make_unique<DrawableV2::RSFilterDrawable::FilterRectInfo>();
    paintFilterCanvas.SetFilterClipBounds(Drawing::RectI());
    clipBound = drawable->GetClipBound(paintFilterCanvas);
    EXPECT_EQ(clipBound, Drawing::RectI(0, 0, 10, 10));

    paintFilterCanvas.SetFilterClipBounds(Drawing::RectI(1, 1, 5, 5));
    clipBound = drawable->GetClipBound(paintFilterCanvas);
    EXPECT_EQ(clipBound, Drawing::RectI(1, 1, 5, 5));
}

/**
 * @tc.name: RSMaterialFilterDrawable_OnGenerate
 * @tc.desc: Test RSMaterialFilterDrawable OnGenerate
 * @tc.type: FUNC
 */
HWTEST_F(RSFilterDrawableTest, RSMaterialFilterDrawable_OnGenerate, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(1);
    std::shared_ptr<RSFilter> filter =
        std::make_shared<RSDrawingFilter>(std::make_shared<RSRenderFilterParaBase>());
    std::shared_ptr<RSDrawable> mtfDrawable = DrawableV2::RSMaterialFilterDrawable::OnGenerate(*node);
    EXPECT_EQ(mtfDrawable, nullptr);
    node->GetMutableRenderProperties().GetEffectProperties().SetMaterialFilter(filter);
    mtfDrawable = DrawableV2::RSMaterialFilterDrawable::OnGenerate(*node);
    EXPECT_NE(mtfDrawable, nullptr);
}

/**
 * @tc.name: RSMaterialFilterDrawable_OnUpdate
 * @tc.desc: Test RSMaterialFilterDrawable OnUpdate
 * @tc.type: FUNC
 */
HWTEST_F(RSFilterDrawableTest, RSMaterialFilterDrawable_OnUpdate, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSMaterialFilterDrawable>();
    ASSERT_NE(drawable, nullptr);

    auto node = std::make_shared<RSRenderNode>(1);
    drawable->OnUpdate(*node);
    EXPECT_FALSE(drawable->stagingEmptyShape_);
    EXPECT_EQ(drawable->stagingClipPath_, nullptr);

    auto sdfShape = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_EMPTY_SHAPE);
    node->GetMutableRenderProperties().SetSDFShape(sdfShape);
    Drawing::Path drawingPath;
    drawingPath.AddRect(0, 0, 100, 100);
    node->GetMutableRenderProperties().SetClipBounds(RSPath::CreateRSPath(drawingPath));

    drawable->OnUpdate(*node);
    EXPECT_TRUE(drawable->stagingEmptyShape_);
    EXPECT_NE(drawable->stagingClipPath_, nullptr);
}

/**
 * @tc.name: RSMaterialFilterDrawable_OnSync
 * @tc.desc: Test RSMaterialFilterDrawable OnSync
 * @tc.type: FUNC
 */
HWTEST_F(RSFilterDrawableTest, RSMaterialFilterDrawable_OnSync, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSMaterialFilterDrawable>();
    ASSERT_NE(drawable, nullptr);
    EXPECT_FALSE(drawable->needSync_);
    drawable->OnSync();

    Drawing::Path drawingPath;
    drawingPath.AddRect(0, 0, 100, 100);
    std::shared_ptr<Drawing::Path> clipPath = std::make_shared<Drawing::Path>(drawingPath);
    drawable->stagingEmptyShape_ = true;
    drawable->stagingClipPath_ = clipPath;
    drawable->needSync_ = true;
    drawable->OnSync();
    EXPECT_TRUE(drawable->emptyShape_);
    EXPECT_EQ(drawable->clipPath_, clipPath);
}

/**
 * @tc.name: RSMaterialFilterDrawable_OnDraw
 * @tc.desc: Test RSMaterialFilterDrawable OnDraw
 * @tc.type: FUNC
 */
HWTEST_F(RSFilterDrawableTest, RSMaterialFilterDrawable_OnDraw, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSMaterialFilterDrawable>();
    ASSERT_NE(drawable, nullptr);
    EXPECT_FALSE(drawable->emptyShape_);
    EXPECT_EQ(drawable->clipPath_, nullptr);

    drawable->OnDraw(nullptr, nullptr);
    Drawing::Canvas canvas;
    drawable->OnDraw(&canvas, nullptr);
    Drawing::Rect rect;
    drawable->OnDraw(&canvas, &rect);

    Drawing::Path drawingPath;
    drawingPath.AddRect(0, 0, 100, 100);
    drawable->clipPath_ = std::make_shared<Drawing::Path>(drawingPath);
    drawable->OnDraw(&canvas, &rect);

    drawable->emptyShape_ = true;
    drawable->OnDraw(&canvas, &rect);
}

/**
 * @tc.name: RSMaterialFilterDrawable_OnFilterUpdated
 * @tc.desc: Test RSMaterialFilterDrawable OnFilterUpdated
 * @tc.type: FUNC
 */
HWTEST_F(RSFilterDrawableTest, RSMaterialFilterDrawable_OnFilterUpdated, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSMaterialFilterDrawable>();
    ASSERT_NE(drawable, nullptr);
    auto node = std::make_shared<RSRenderNode>(1);
    drawable->OnFilterUpdated(*node, nullptr);
    auto rsFilter = std::make_shared<RSFilter>();
    drawable->OnFilterUpdated(*node, rsFilter);
    auto drawingFilter = std::make_shared<RSDrawingFilter>();
    drawable->OnFilterUpdated(*node, drawingFilter);
}

/**
 * @tc.name: RSMaterialFilterDrawable_RenderWithDRM
 * @tc.desc: Test RSMaterialFilterDrawable RenderWithDRM
 * @tc.type: FUNC
 */
HWTEST_F(RSFilterDrawableTest, RSMaterialFilterDrawable_RenderWithDRM, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSMaterialFilterDrawable>();
    ASSERT_NE(drawable, nullptr);
    EXPECT_FALSE(drawable->RenderWithDRM(nullptr, nullptr));
    Drawing::Canvas canvas;
    EXPECT_TRUE(drawable->RenderWithDRM(&canvas, nullptr));
    Drawing::Rect rect;
    EXPECT_TRUE(drawable->RenderWithDRM(&canvas, &rect));
}

/**
 * @tc.name: RSMaterialFilterDrawable_GetClipBound
 * @tc.desc: Test RSMaterialFilterDrawable GetClipBound
 * @tc.type: FUNC
 */
HWTEST_F(RSFilterDrawableTest, RSMaterialFilterDrawable_GetClipBound, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSMaterialFilterDrawable>();
    ASSERT_NE(drawable, nullptr);
    Drawing::Canvas plainCanvas;
    auto clipBound = drawable->GetClipBound(plainCanvas);
    EXPECT_TRUE(clipBound.IsEmpty());

    auto surface = Drawing::Surface::MakeRasterN32Premul(10, 10);
    ASSERT_NE(surface, nullptr);
    RSPaintFilterCanvas paintFilterCanvas(surface.get());

    paintFilterCanvas.SetFilterClipBounds(Drawing::RectI());
    clipBound = drawable->GetClipBound(paintFilterCanvas);
    EXPECT_EQ(clipBound, Drawing::RectI(0, 0, 10, 10));

    paintFilterCanvas.SetFilterClipBounds(Drawing::RectI(1, 1, 5, 5));
    clipBound = drawable->GetClipBound(paintFilterCanvas);
    EXPECT_EQ(clipBound, Drawing::RectI(1, 1, 5, 5));
}

/**
 * @tc.name: RSCompositingFilterDrawable_OnGenerate
 * @tc.desc: Test RSCompositingFilterDrawable OnGenerate
 * @tc.type: FUNC
 */
HWTEST_F(RSFilterDrawableTest, RSCompositingFilterDrawable_OnGenerate, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(1);
    std::shared_ptr<RSFilter> filter =
        std::make_shared<RSDrawingFilter>(std::make_shared<RSRenderFilterParaBase>());
    std::shared_ptr<RSDrawable> ctfDrawable = DrawableV2::RSCompositingFilterDrawable::OnGenerate(*node);
    EXPECT_EQ(ctfDrawable, nullptr);
    node->GetMutableRenderProperties().GetEffectProperties().SetCompositingFilter(filter);
    ctfDrawable = DrawableV2::RSCompositingFilterDrawable::OnGenerate(*node);
    EXPECT_NE(ctfDrawable, nullptr);
}

} // namespace OHOS::Rosen