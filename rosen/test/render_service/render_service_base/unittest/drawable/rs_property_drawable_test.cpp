/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "common/rs_obj_geometry.h"
#include "drawable/rs_property_drawable.h"
#include "effect/rs_render_shader_base.h"
#include "effect/rs_render_shape_base.h"
#include "pipeline/rs_recording_canvas.h"
#include "pipeline/rs_render_node.h"
#include "render/rs_drawing_filter.h"
#include "render/rs_render_aibar_filter.h"
#include "render/rs_render_linear_gradient_blur_filter.h"
#include "drawable/rs_property_drawable_utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSPropertyDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSPropertyDrawableTest::SetUpTestCase() {}
void RSPropertyDrawableTest::TearDownTestCase() {}
void RSPropertyDrawableTest::SetUp() {}
void RSPropertyDrawableTest::TearDown() {}

/**
 * @tc.name: OnSyncAndOnDrawFuncTest001
 * @tc.desc: class RSPropertyDrawable OnSync and OnDraw test
 * @tc.type:FUNC
 * @tc.require: issueI9VSPU
 */
HWTEST_F(RSPropertyDrawableTest, OnSyncAndOnDrawFuncTest001, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSPropertyDrawable> propertyDrawable =
        std::make_shared<DrawableV2::RSPropertyDrawable>();
    EXPECT_NE(propertyDrawable, nullptr);

    propertyDrawable->needSync_ = false;
    propertyDrawable->OnSync();

    propertyDrawable->needSync_ = true;
    propertyDrawable->drawCmdList_ = nullptr;
    propertyDrawable->stagingDrawCmdList_ = nullptr;
    propertyDrawable->OnSync();
    EXPECT_FALSE(propertyDrawable->needSync_);

    std::shared_ptr<Drawing::DrawCmdList> drawCmdList = std::make_shared<Drawing::DrawCmdList>();
    EXPECT_NE(drawCmdList, nullptr);
    propertyDrawable->drawCmdList_ = drawCmdList;
    Drawing::Canvas canvas;
    Drawing::Rect rect(0.0f, 0.0f, 1.0f, 1.0f);
    propertyDrawable->OnDraw(&canvas, &rect);
    propertyDrawable->propertyDescription_ = "RSPropertyDrawable test";
    propertyDrawable->OnDraw(&canvas, &rect);
}

/**
 * @tc.name: RSPropertyDrawCmdListUpdaterTest002
 * @tc.desc: ~RSPropertyDrawCmdListUpdater test
 * @tc.type:FUNC
 * @tc.require: issueI9VSPU
 */
HWTEST_F(RSPropertyDrawableTest, RSPropertyDrawCmdListUpdaterTest002, TestSize.Level1)
{
    DrawableV2::RSPropertyDrawable targetTest1;
    std::shared_ptr<DrawableV2::RSPropertyDrawCmdListUpdater> propertyDrawCmdListUpdaterTest1 =
        std::make_shared<DrawableV2::RSPropertyDrawCmdListUpdater>(0, 0, &targetTest1);
    EXPECT_NE(propertyDrawCmdListUpdaterTest1, nullptr);

    std::unique_ptr<ExtendRecordingCanvas> recordingCanvas = std::make_unique<ExtendRecordingCanvas>(0, 0);
    EXPECT_NE(recordingCanvas, nullptr);
    propertyDrawCmdListUpdaterTest1->recordingCanvas_ = std::move(recordingCanvas);
    DrawableV2::RSPropertyDrawable target;
    propertyDrawCmdListUpdaterTest1->target_ = &target;

    DrawableV2::RSPropertyDrawable targetTest2;
    std::shared_ptr<DrawableV2::RSPropertyDrawCmdListUpdater> propertyDrawCmdListUpdaterTest2 =
        std::make_shared<DrawableV2::RSPropertyDrawCmdListUpdater>(0, 0, &targetTest2);
    propertyDrawCmdListUpdaterTest2->recordingCanvas_ = nullptr;
}

/**
 * @tc.name: OnGenerateAndOnUpdateTest003
 * @tc.desc: class RSFrameOffsetDrawable OnGenerate and OnUpdate test
 * @tc.type:FUNC
 * @tc.require: issueI9VSPU
 */
HWTEST_F(RSPropertyDrawableTest, OnGenerateAndOnUpdateTest003, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSFrameOffsetDrawable> frameOffsetDrawable =
        std::make_shared<DrawableV2::RSFrameOffsetDrawable>();
    EXPECT_NE(frameOffsetDrawable, nullptr);

    RSRenderNode node(0);
    node.renderProperties_.frameOffsetX_ = 0.0f;
    node.renderProperties_.frameOffsetY_ = 0.0f;
    EXPECT_EQ(frameOffsetDrawable->OnGenerate(node), nullptr);

    node.renderProperties_.frameOffsetX_ = 1.0f;
    EXPECT_NE(frameOffsetDrawable->OnGenerate(node), nullptr);
}

/**
 * @tc.name: OnUpdateTest004
 * @tc.desc: class RSClipToBoundsDrawable OnUpdate test
 * @tc.type:FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSPropertyDrawableTest, OnUpdateTest004, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSClipToBoundsDrawable> clipToBoundsDrawable =
        std::make_shared<DrawableV2::RSClipToBoundsDrawable>();
    EXPECT_NE(clipToBoundsDrawable, nullptr);

    RSRenderNode nodeTest1(0);
    nodeTest1.renderProperties_.clipPath_ = std::make_shared<RSPath>();
    EXPECT_NE(nodeTest1.renderProperties_.clipPath_, nullptr);
    EXPECT_TRUE(clipToBoundsDrawable->OnUpdate(nodeTest1));

    RSRenderNode nodeTest2(0);
    RectT<float> rect(1.0f, 1.0f, 1.0f, 1.0f);
    RRectT<float> rectt(rect, 1.0f, 1.0f);
    nodeTest2.renderProperties_.clipRRect_ = rectt;
    EXPECT_TRUE(clipToBoundsDrawable->OnUpdate(nodeTest2));

    RSRenderNode nodeTest3(0);
    nodeTest3.renderProperties_.cornerRadius_ = 1.0f;
    EXPECT_TRUE(clipToBoundsDrawable->OnUpdate(nodeTest3));

    RSRenderNode nodeTest4(0);
    EXPECT_TRUE(clipToBoundsDrawable->OnUpdate(nodeTest4));
}

/**
 * @tc.name: RSClipToBoundsDrawableOnSyncTest
 * @tc.desc: class RSClipToBoundsDrawable OnSync test
 * @tc.type: FUNC
 * @tc.require: issue20176
 */
HWTEST_F(RSPropertyDrawableTest, RSClipToBoundsDrawableOnSyncTest, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSClipToBoundsDrawable> clipToBoundsDrawable =
        std::make_shared<DrawableV2::RSClipToBoundsDrawable>();
    EXPECT_NE(clipToBoundsDrawable, nullptr);
    clipToBoundsDrawable->needSync_ = false;
    clipToBoundsDrawable->OnSync();

    clipToBoundsDrawable->needSync_ = true;
    clipToBoundsDrawable->OnSync();
    EXPECT_FALSE(clipToBoundsDrawable->needSync_);
}

/**
 * @tc.name: RSClipToBoundsDrawableOnDrawFuncTest
 * @tc.desc: class RSClipToBoundsDrawable OnDraw test
 * @tc.type: FUNC
 * @tc.require: issue20176
 */
HWTEST_F(RSPropertyDrawableTest, RSClipToBoundsDrawableOnDrawFuncTest, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSClipToBoundsDrawable> clipToBoundsDrawable =
        std::make_shared<DrawableV2::RSClipToBoundsDrawable>();
    EXPECT_NE(clipToBoundsDrawable, nullptr);
    Drawing::Canvas canvas;
    Drawing::Rect rect(0.0f, 0.0f, 1.0f, 1.0f);
    clipToBoundsDrawable->OnDraw(&canvas, &rect);

    clipToBoundsDrawable->type_ = RSClipToBoundsType::CLIP_PATH;
    clipToBoundsDrawable->OnDraw(&canvas, &rect);

    clipToBoundsDrawable->type_ = RSClipToBoundsType::CLIP_RRECT;
    clipToBoundsDrawable->isClipRRectOptimization_ = true;
    clipToBoundsDrawable->OnDraw(&canvas, &rect);

    clipToBoundsDrawable->type_ = RSClipToBoundsType::CLIP_RRECT;
    clipToBoundsDrawable->isClipRRectOptimization_ = false;
    clipToBoundsDrawable->OnDraw(&canvas, &rect);

    clipToBoundsDrawable->type_ = RSClipToBoundsType::CLIP_IRECT;
    clipToBoundsDrawable->OnDraw(&canvas, &rect);

    clipToBoundsDrawable->type_ = RSClipToBoundsType::CLIP_RECT;
    clipToBoundsDrawable->OnDraw(&canvas, &rect);

    clipToBoundsDrawable->type_ = RSClipToBoundsType::INVALID;
    clipToBoundsDrawable->OnDraw(&canvas, &rect);
}

/**
 * @tc.name: RSClipToBoundsDrawableTestSDF
 * @tc.desc: Test RSClipToBoundsDrawable with sdf
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableTest, RSClipToBoundsDrawableTestSDF, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSClipToBoundsDrawable>();
    drawable->type_ = RSClipToBoundsType::CLIP_SDF;
    Drawing::Canvas canvas;
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect rect { 1.0f, 1.0f, 2.0f, 2.0f };
    drawable->OnDraw(&canvas, &rect);
    EXPECT_EQ(drawable->geContainer_, nullptr);

    NodeId id = 3;
    RSRenderNode node(id);
    auto sdfShape = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_UNION_OP_SHAPE);
    node.GetMutableRenderProperties().SetSDFShape(sdfShape);
    drawable->OnUpdate(node);
    drawable->OnSync();
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    drawable->OnDraw(&paintFilterCanvas, &rect);
    EXPECT_NE(drawable->geContainer_, nullptr);
}

/**
 * @tc.name: OnGenerateAndOnUpdateTest005
 * @tc.desc: class RSClipToFrameDrawable OnGenerate and OnUpdate test
 * @tc.type:FUNC
 * @tc.require: issueI9VSPU
 */
HWTEST_F(RSPropertyDrawableTest, OnGenerateAndOnUpdateTest005, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSClipToFrameDrawable> clipToFrameDrawable =
        std::make_shared<DrawableV2::RSClipToFrameDrawable>();
    EXPECT_NE(clipToFrameDrawable, nullptr);

    RSRenderNode node(0);
    node.renderProperties_.clipToFrame_ = false;
    EXPECT_EQ(clipToFrameDrawable->OnGenerate(node), nullptr);

    RSObjGeometry geometry;

    geometry.width_ = 1.0f;
    geometry.height_ = 1.0f;
    node.renderProperties_.frameGeo_ = geometry;
    node.renderProperties_.clipToFrame_ = true;
    EXPECT_NE(clipToFrameDrawable->OnGenerate(node), nullptr);
}

/**
 * @tc.name: RSFilterDrawableTest006
 * @tc.desc: class RSFilterDrawable and OnSync test
 * @tc.type:FUNC
 * @tc.require: issueI9VSPU
 */
HWTEST_F(RSPropertyDrawableTest, RSFilterDrawableTest006, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSFilterDrawable> filterDrawable = std::make_shared<DrawableV2::RSFilterDrawable>();
    EXPECT_NE(filterDrawable, nullptr);

    filterDrawable->needSync_ = true;
    filterDrawable->stagingFilter_ = std::make_shared<RSFilter>();
    filterDrawable->stagingIntersectWithDRM_=true;
    filterDrawable->stagingIsDarkColorMode_=true;
    filterDrawable->OnSync();
    EXPECT_FALSE(filterDrawable->needSync_);
    EXPECT_FALSE(filterDrawable->stagingIntersectWithDRM_);
    EXPECT_FALSE(filterDrawable->stagingIsDarkColorMode_);

    Drawing::Canvas canvas;
    Drawing::Rect rect(0.0f, 0.0f, 1.0f, 1.0f);
    filterDrawable->OnDraw(&canvas, &rect);
    filterDrawable->MarkEffectNode();
}

/**
 * @tc.name: MarkNeedClearFilterCacheTest007
 * @tc.desc: class RSFilterDrawable MarkNeedClearFilterCache test
 * @tc.type:FUNC
 * @tc.require: issueI9VSPU
 */
HWTEST_F(RSPropertyDrawableTest, MarkNeedClearFilterCacheTest007, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSFilterDrawable> filterDrawable = std::make_shared<DrawableV2::RSFilterDrawable>();
    EXPECT_NE(filterDrawable, nullptr);
    filterDrawable->MarkNeedClearFilterCache();
    filterDrawable->stagingCacheManager_ = nullptr;
    filterDrawable->MarkNeedClearFilterCache();
}

/**
 * @tc.name: RecordFilterInfosTest008
 * @tc.desc: class RSFilterDrawable RecordFilterInfos ClearFilterCache UpdateFlags test
 * @tc.type:FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSPropertyDrawableTest, RecordFilterInfosTest008, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSFilterDrawable> filterDrawable = std::make_shared<DrawableV2::RSFilterDrawable>();
    EXPECT_NE(filterDrawable, nullptr);

    std::shared_ptr<RSDrawingFilter> rsFilter = nullptr;
    filterDrawable->RecordFilterInfos(rsFilter);
}

/**
 * @tc.name: CheckAndUpdateAIBarCacheStatusTest009
 * @tc.desc: class RSFilterDrawable CheckAndUpdateAIBarCacheStatus test
 * @tc.type:FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSPropertyDrawableTest, CheckAndUpdateAIBarCacheStatusTest009, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSFilterDrawable> filterDrawable = std::make_shared<DrawableV2::RSFilterDrawable>();
    EXPECT_NE(filterDrawable, nullptr);
    EXPECT_FALSE(filterDrawable->CheckAndUpdateAIBarCacheStatus(false));

    if (filterDrawable->stagingCacheManager_ != nullptr) {
        filterDrawable->stagingCacheManager_->filterType_ = RSFilter::AIBAR;
        filterDrawable->stagingCacheManager_->cacheUpdateInterval_ = 1;
        filterDrawable->stagingCacheManager_->stagingForceClearCacheForLastFrame_ = false;
        EXPECT_TRUE(filterDrawable->CheckAndUpdateAIBarCacheStatus(false));
    }

    filterDrawable->stagingCacheManager_ = nullptr;
    EXPECT_FALSE(filterDrawable->CheckAndUpdateAIBarCacheStatus(false));
}

/**
 * @tc.name: ForceReduceAIBarCacheIntervalTest
 * @tc.desc: class RSFilterDrawable ForceReduceAIBarCacheInterval test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertyDrawableTest, ForceReduceAIBarCacheInterval, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSFilterDrawable> filterDrawable = std::make_shared<DrawableV2::RSFilterDrawable>();
    EXPECT_NE(filterDrawable, nullptr);
    EXPECT_FALSE(filterDrawable->CheckAndUpdateAIBarCacheStatus(false));

    if (filterDrawable->stagingCacheManager_ != nullptr) {
        filterDrawable->stagingCacheManager_->filterType_ = RSFilter::AIBAR;
        filterDrawable->stagingCacheManager_->cacheUpdateInterval_ = 1;
        EXPECT_TRUE(filterDrawable->ForceReduceAIBarCacheInterval(true));
        EXPECT_FALSE(filterDrawable->ForceReduceAIBarCacheInterval(false));
    }
}

/**
 * @tc.name: RSFilterDrawableTest010
 * @tc.desc: OnDraw
 * @tc.type:FUNC
 * @tc.require: issueI9VSPU
 */
HWTEST_F(RSPropertyDrawableTest, RSFilterDrawableTest010, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSFilterDrawable>();
    EXPECT_NE(drawable, nullptr);
    Drawing::Canvas canvas;
    Drawing::Rect rect(0.0f, 0.0f, 1.0f, 1.0f);
    drawable->OnDraw(&canvas, &rect);
    drawable->OnDraw(nullptr, &rect);
    std::vector<std::pair<float, float>> fractionStops;
    auto para = std::make_shared<RSLinearGradientBlurPara>(1.f, fractionStops, GradientDirection::LEFT);
    auto shaderFilter = std::make_shared<RSLinearGradientBlurShaderFilter>(para, 1.f, 1.f);
    drawable->filter_ = std::make_shared<RSDrawingFilter>(shaderFilter);;
    drawable->filter_->SetFilterType(RSFilter::LINEAR_GRADIENT_BLUR);
    EXPECT_NE(drawable->filter_, nullptr);
    drawable->OnDraw(&canvas, &rect);
    EXPECT_TRUE(true);
    auto aiBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    drawable->filter_ = std::make_shared<RSDrawingFilter>(aiBarShaderFilter);
    drawable->filter_->SetFilterType(RSFilter::LINEAR_GRADIENT_BLUR);
    EXPECT_NE(drawable->filter_, nullptr);
    drawable->OnDraw(&canvas, &rect);
    EXPECT_TRUE(true);
    drawable->filter_->SetFilterType(RSFilter::AIBAR);
    EXPECT_NE(drawable->filter_, nullptr);
    drawable->OnDraw(&canvas, &rect);
    EXPECT_TRUE(true);
    auto filterCanvas = std::make_shared<RSPaintFilterCanvas>(&canvas);
    drawable->OnDraw(filterCanvas.get(), &rect);
    EXPECT_NE(drawable->filter_, nullptr);

    drawable->renderRelativeRectInfo_ = nullptr;
    drawable->OnDraw(filterCanvas.get(), &rect);
    EXPECT_TRUE(true);

    auto bound = RectF(-5.0f, -5.0f, 8.0f, 8.0f);
    auto drawable2 = std::make_shared<DrawableV2::RSFilterDrawable>();
    auto aiBarShaderFilter2 = std::make_shared<RSAIBarShaderFilter>();
    drawable2->filter_ = std::make_shared<RSDrawingFilter>(aiBarShaderFilter2);
    drawable2->filter_->SetFilterType(RSFilter::AIBAR);
    drawable2->renderRelativeRectInfo_ = std::make_unique<DrawableV2::RSFilterDrawable::FilterRectInfo>();
    drawable2->renderRelativeRectInfo_->snapshotRect_ = bound;
    drawable2->renderRelativeRectInfo_->drawRect_ = bound;
    drawable2->OnDraw(filterCanvas.get(), &rect);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: RSFilterDrawableTest011
 * @tc.desc: IsFilterCacheValidForOcclusion
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableTest, RSFilterDrawableTest011, TestSize.Level1)
{
    auto filterDrawable = std::make_shared<DrawableV2::RSFilterDrawable>();
    ASSERT_NE(filterDrawable, nullptr);

    auto &cacheManager = filterDrawable->cacheManager_;

    cacheManager = nullptr;
    EXPECT_FALSE(filterDrawable->IsFilterCacheValidForOcclusion());

    cacheManager = std::make_unique<RSFilterCacheManager>();
    ASSERT_NE(cacheManager, nullptr);

    // cacheType: FilterCacheType::NONE
    cacheManager->cachedSnapshot_ = nullptr;
    cacheManager->cachedFilteredSnapshot_ = nullptr;
    EXPECT_FALSE(filterDrawable->IsFilterCacheValidForOcclusion());

    // cacheType: FilterCacheType::SNAPSHOT
    cacheManager->cachedSnapshot_ = std::make_shared<RSPaintFilterCanvas::CachedEffectData>();
    ASSERT_NE(cacheManager->cachedSnapshot_, nullptr);
    cacheManager->cachedFilteredSnapshot_ = nullptr;
    EXPECT_TRUE(filterDrawable->IsFilterCacheValidForOcclusion());

    // cacheType: FilterCacheType::BOTH
    cacheManager->cachedSnapshot_ = std::make_shared<RSPaintFilterCanvas::CachedEffectData>();
    ASSERT_NE(cacheManager->cachedSnapshot_, nullptr);
    cacheManager->cachedFilteredSnapshot_ = std::make_shared<RSPaintFilterCanvas::CachedEffectData>();
    ASSERT_NE(cacheManager->cachedFilteredSnapshot_, nullptr);
    EXPECT_TRUE(filterDrawable->IsFilterCacheValidForOcclusion());

    // cacheType: FilterCacheType::FILTERED_SNAPSHOT
    cacheManager->cachedSnapshot_ = nullptr;
    cacheManager->cachedFilteredSnapshot_ = std::make_shared<RSPaintFilterCanvas::CachedEffectData>();
    ASSERT_NE(cacheManager->cachedFilteredSnapshot_, nullptr);
    EXPECT_TRUE(filterDrawable->IsFilterCacheValidForOcclusion());
}

/*
 * @tc.name: MarkBlurIntersectWithDRM001
 * @tc.desc: class RSFilterDrawable MarkBlurIntersectWithDRM test
 * @tc.type: FUNC
 * @tc.require: issuesIAQZ4I
 */
HWTEST_F(RSPropertyDrawableTest, MarkBlurIntersectWithDRM001, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSFilterDrawable> filterDrawable = std::make_shared<DrawableV2::RSFilterDrawable>();
    EXPECT_NE(filterDrawable, nullptr);
    filterDrawable->MarkBlurIntersectWithDRM(true, true);
}

/**
 * @tc.name: RSFilterDrawableTest012
 * @tc.desc: Test RSFilterDrawable needDrawBehindWindow branch
 * @tc.type: FUNC
 * @tc.require: issueIB0UQV
 */
HWTEST_F(RSPropertyDrawableTest, RSFilterDrawableTest012, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSFilterDrawable>();
    ASSERT_NE(drawable, nullptr);
    auto canvas = std::make_shared<Drawing::Canvas>();
    auto filterCanvas = std::make_shared<RSPaintFilterCanvas>(canvas.get());
    Drawing::Surface surface;
    filterCanvas->surface_ = &surface;
    drawable->needDrawBehindWindow_ = true;
    Drawing::Rect rect(0.0f, 0.0f, 1.0f, 1.0f);
    drawable->OnDraw(filterCanvas.get(), &rect);

    drawable->filter_ = RSPropertyDrawableUtils::GenerateBehindWindowFilter(80.0f, 1.9f, 1.0f, RSColor(0xFFFFFFE5));
    EXPECT_NE(drawable->filter_, nullptr);
    drawable->OnDraw(filterCanvas.get(), &rect);

    drawable->OnDraw(canvas.get(), &rect);
    ASSERT_TRUE(true);

    filterCanvas->isWindowFreezeCapture_ = true;
    drawable->OnDraw(filterCanvas.get(), &rect);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: RSFilterDrawableTest013
 * @tc.desc: Test RSFilterDrawable SetDrawBehindWindowRegion
 * @tc.type: FUNC
 * @tc.require: issueIBCIDQ
 */
HWTEST_F(RSPropertyDrawableTest, RSFilterDrawableTest013, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSFilterDrawable>();
    ASSERT_NE(drawable, nullptr);
    RectI region(0, 0, 1, 1);
    drawable->SetDrawBehindWindowRegion(region);
    ASSERT_TRUE(drawable->stagingDrawBehindWindowRegion_ == region);
}

/**
 * @tc.name: RSFilterDrawableTest014
 * @tc.desc: Test RSFilterDrawable needDrawBehindWindow DrawCache
 * @tc.type:FUNC
 * @tc.require:issuesIC0HM8
 */
HWTEST_F(RSPropertyDrawableTest, RSFilterDrawableTest014, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSFilterDrawable>();
    ASSERT_NE(drawable, nullptr);
    auto canvas = std::make_shared<Drawing::Canvas>();
    auto filterCanvas = std::make_shared<RSPaintFilterCanvas>(canvas.get());
    Drawing::Surface surface;
    filterCanvas->surface_ = &surface;
    drawable->needDrawBehindWindow_ = true;
    filterCanvas->SetIsDrawingCache(true);
    Drawing::Rect rect(0.0f, 0.0f, 1.0f, 1.0f);
    drawable->filter_ = RSPropertyDrawableUtils::GenerateBehindWindowFilter(80.0f, 1.9f, 1.0f, RSColor(0xFFFFFFE5));
    EXPECT_NE(drawable->filter_, nullptr);
    drawable->OnDraw(filterCanvas.get(), &rect);
    EXPECT_NE(filterCanvas->cacheBehindWindowData_, nullptr);

    filterCanvas->isWindowFreezeCapture_ = true;
    drawable->OnDraw(filterCanvas.get(), &rect);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: RSFilterDrawableTest015
 * @tc.desc: Test RSFilterDrawable OnDraw
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyDrawableTest, RSFilterDrawableTest015, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSFilterDrawable>();
    Drawing::Canvas canvas;
    std::vector<std::pair<float, float>> fractionStops;
    auto para = std::make_shared<RSLinearGradientBlurPara>(1.f, fractionStops, GradientDirection::LEFT);
    auto shaderFilter = std::make_shared<RSLinearGradientBlurShaderFilter>(para, 1.f, 1.f);
    drawable->filter_ = std::make_shared<RSDrawingFilter>(shaderFilter);
    drawable->filter_->SetFilterType(RSFilter::LINEAR_GRADIENT_BLUR);

    // Test rect == nullptr
    drawable->OnDraw(&canvas, nullptr);
    auto drawingFilter = std::static_pointer_cast<RSDrawingFilter>(drawable->filter_);
    auto shaderLinearGradientBlurFilter = drawingFilter->GetShaderFilterWithType(RSUIFilterType::LINEAR_GRADIENT_BLUR);
    EXPECT_EQ(shaderLinearGradientBlurFilter->geoHeight_, 1.0f);
}

/**
 * @tc.name: RSFilterDrawableTest018
 * @tc.desc: Test RSFilterDrawable CalVisibleRect
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyDrawableTest, RSFilterDrawableTest018, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSFilterDrawable>();
    Drawing::Canvas canvas;

    RectF defaultRelativeRect(0.0f, 0.0f, 10.0f, 10.0f);
    drawable->CalVisibleRect(canvas.GetTotalMatrix(), std::nullopt, defaultRelativeRect);
    EXPECT_EQ(drawable->stagingVisibleRectInfo_, nullptr);

    RectF snapshotRect = RectF(-5.0f, -5.0f, 15.0f, 15.0f);
    RectF drawRect = RectF(-2.0f, -2.0f, 18.0f, 18.0f);
    RectF totalRect = snapshotRect.JoinRect(drawRect);
    drawable->stagingRelativeRectInfo_ = std::make_unique<DrawableV2::RSFilterDrawable::FilterRectInfo>();
    drawable->stagingRelativeRectInfo_->snapshotRect_ = snapshotRect;
    drawable->stagingRelativeRectInfo_->drawRect_ = drawRect;
    drawable->CalVisibleRect(canvas.GetTotalMatrix(), std::nullopt, defaultRelativeRect);
    ASSERT_NE(drawable->stagingVisibleRectInfo_, nullptr);
    EXPECT_EQ(drawable->stagingVisibleRectInfo_->snapshotRect_, snapshotRect.ConvertTo<int>());
    EXPECT_EQ(drawable->stagingVisibleRectInfo_->totalRect_, totalRect.ConvertTo<int>());

    RectI clipRect = RectI(-4, -4, 17, 17);
    drawable->CalVisibleRect(canvas.GetTotalMatrix(), clipRect, defaultRelativeRect);
    ASSERT_NE(drawable->stagingVisibleRectInfo_, nullptr);
    EXPECT_EQ(drawable->stagingVisibleRectInfo_->snapshotRect_, snapshotRect.ConvertTo<int>().IntersectRect(clipRect));
    EXPECT_EQ(drawable->stagingVisibleRectInfo_->totalRect_, totalRect.ConvertTo<int>().IntersectRect(clipRect));
}

/**
 * @tc.name: RSFilterDrawableTest019
 * @tc.desc: Test RSFilterDrawable GetRelativeRect
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyDrawableTest, RSFilterDrawableTest019, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSFilterDrawable>();
    RectF defaultRelativeRect(0.0f, 0.0f, 10.0f, 10.0f);
    RectF snapshotRect = RectF(-5.0f, -5.0f, 15.0f, 15.0f);
    RectF drawRect = RectF(-2.0f, -2.0f, 18.0f, 18.0f);
    RectF totalRect = snapshotRect.JoinRect(drawRect);
    auto rectInfo = std::make_unique<DrawableV2::RSFilterDrawable::FilterRectInfo>();
    rectInfo->snapshotRect_ = snapshotRect;
    rectInfo->drawRect_ = drawRect;

    EXPECT_EQ(DrawableV2::RSFilterDrawable::GetRelativeRect(rectInfo, EffectRectType::SNAPSHOT, defaultRelativeRect),
        snapshotRect);
    EXPECT_EQ(DrawableV2::RSFilterDrawable::GetRelativeRect(rectInfo, EffectRectType::DRAW, defaultRelativeRect),
        drawRect);
    EXPECT_EQ(DrawableV2::RSFilterDrawable::GetRelativeRect(rectInfo, EffectRectType::TOTAL, defaultRelativeRect),
        totalRect);
    EXPECT_EQ(DrawableV2::RSFilterDrawable::GetRelativeRect(rectInfo, EffectRectType(UINT8_MAX), defaultRelativeRect),
        defaultRelativeRect);
}

/**
 * @tc.name: RSFilterDrawableTest020
 * @tc.desc: Test RSFilterDrawable UpdateFilterRectInfo
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyDrawableTest, RSFilterDrawableTest020, TestSize.Level1)
{
    RectF bound(0.0f, 0.0f, 10.0f, 10.0f);
    auto drawable = std::make_shared<DrawableV2::RSFilterDrawable>();
    drawable->UpdateFilterRectInfo(bound, nullptr);
    ASSERT_EQ(drawable->stagingRelativeRectInfo_, nullptr);
    
    auto drawingFilter = std::make_shared<RSDrawingFilter>();
    drawingFilter->imageFilter_ = std::make_shared<Drawing::ImageFilter>();
    drawable->UpdateFilterRectInfo(bound, drawingFilter);
    ASSERT_EQ(drawable->stagingRelativeRectInfo_, nullptr);

    drawingFilter->imageFilter_ = nullptr;
    drawable->UpdateFilterRectInfo(bound, drawingFilter);
    ASSERT_NE(drawable->stagingRelativeRectInfo_, nullptr);
    EXPECT_EQ(drawable->stagingRelativeRectInfo_->snapshotRect_,
        drawingFilter->GetRect(bound, EffectRectType::SNAPSHOT));
    EXPECT_EQ(drawable->stagingRelativeRectInfo_->drawRect_, drawingFilter->GetRect(bound, EffectRectType::DRAW));
}

/**
 * @tc.name: GetAbsRenderEffectRect001
 * @tc.desc: Test RSFilterDrawable GetAbsRenderEffectRect
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertyDrawableTest, GetAbsRenderEffectRect001, TestSize.Level1)
{
    Drawing::Canvas canvas;
    auto bound = RectF(-5.0f, -5.0f, 8.0f, 8.0f);
    auto drawable = std::make_shared<DrawableV2::RSFilterDrawable>();
    drawable->renderRelativeRectInfo_ = std::make_unique<DrawableV2::RSFilterDrawable::FilterRectInfo>();
    drawable->renderRelativeRectInfo_->snapshotRect_ = bound;
    drawable->renderRelativeRectInfo_->drawRect_ = bound;
    auto absRenderEffectRect = drawable->GetAbsRenderEffectRect(canvas, EffectRectType::SNAPSHOT, bound);
    auto drawingClipBound = canvas.GetDeviceClipBounds();
    auto effectRect = RectI(std::floor(bound.GetLeft()), std::floor(bound.GetTop()),
        std::ceil(bound.GetWidth()), std::ceil(bound.GetHeight()));
    auto result = effectRect.IntersectRect(RectI(
        drawingClipBound.GetLeft(), drawingClipBound.GetTop(),
        drawingClipBound.GetWidth(), drawingClipBound.GetHeight()));
    EXPECT_EQ(absRenderEffectRect,
        Drawing::RectI(result.GetLeft(), result.GetTop(), result.GetRight(), result.GetBottom()));
}
} // namespace OHOS::Rosen
