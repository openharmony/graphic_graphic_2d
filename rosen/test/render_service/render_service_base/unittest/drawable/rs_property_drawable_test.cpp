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
 * @tc.name: OnSyncAndCreateDrawFuncTest001
 * @tc.desc: class RSPropertyDrawable OnSync and CreateDrawFunc test
 * @tc.type:FUNC
 * @tc.require: issueI9VSPU
 */
HWTEST_F(RSPropertyDrawableTest, OnSyncAndCreateDrawFuncTest001, TestSize.Level1)
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
    propertyDrawable->propertyDescription_ = "RSPropertyDrawable test";
    propertyDrawable->CreateDrawFunc()(&canvas, &rect);
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
    filterDrawable->CreateDrawFunc()(&canvas, &rect);
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

    filterDrawable->stagingCacheManager_->filterType_ = RSFilter::AIBAR;
    filterDrawable->stagingCacheManager_->cacheUpdateInterval_ = 1;
    filterDrawable->stagingCacheManager_->stagingForceClearCacheForLastFrame_ = false;
    EXPECT_TRUE(filterDrawable->CheckAndUpdateAIBarCacheStatus(false));

    filterDrawable->stagingCacheManager_ = nullptr;
    EXPECT_FALSE(filterDrawable->CheckAndUpdateAIBarCacheStatus(false));
}

/**
 * @tc.name: RSFilterDrawableTest010
 * @tc.desc: CreateDrawFunc
 * @tc.type:FUNC
 * @tc.require: issueI9VSPU
 */
HWTEST_F(RSPropertyDrawableTest, RSFilterDrawableTest010, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSFilterDrawable>();
    EXPECT_NE(drawable, nullptr);
    Drawing::Canvas canvas;
    Drawing::Rect rect(0.0f, 0.0f, 1.0f, 1.0f);
    drawable->CreateDrawFunc()(&canvas, &rect);
    std::vector<std::pair<float, float>> fractionStops;
    auto para = std::make_shared<RSLinearGradientBlurPara>(1.f, fractionStops, GradientDirection::LEFT);
    auto shaderFilter = std::make_shared<RSLinearGradientBlurShaderFilter>(para, 1.f, 1.f);
    drawable->filter_ = std::make_shared<RSDrawingFilter>(shaderFilter);;
    drawable->filter_->SetFilterType(RSFilter::LINEAR_GRADIENT_BLUR);
    EXPECT_NE(drawable->filter_, nullptr);
    drawable->CreateDrawFunc()(&canvas, &rect);
    EXPECT_TRUE(true);
    auto aiBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    drawable->filter_ = std::make_shared<RSDrawingFilter>(aiBarShaderFilter);
    drawable->filter_->SetFilterType(RSFilter::LINEAR_GRADIENT_BLUR);
    EXPECT_NE(drawable->filter_, nullptr);
    drawable->CreateDrawFunc()(&canvas, &rect);
    EXPECT_TRUE(true);
    drawable->filter_->SetFilterType(RSFilter::AIBAR);
    EXPECT_NE(drawable->filter_, nullptr);
    drawable->CreateDrawFunc()(&canvas, &rect);
    EXPECT_TRUE(true);
    auto filterCanvas = std::make_shared<RSPaintFilterCanvas>(&canvas);
    drawable->CreateDrawFunc()(filterCanvas.get(), &rect);
    EXPECT_NE(drawable->filter_, nullptr);
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
    auto drawFunc = drawable->CreateDrawFunc();
    drawFunc(filterCanvas.get(), &rect);
    drawable->filter_ = RSPropertyDrawableUtils::GenerateBehindWindowFilter(80.0f, 1.9f, 1.0f, RSColor(0xFFFFFFE5));
    EXPECT_NE(drawable->filter_, nullptr);
    drawFunc(filterCanvas.get(), &rect);
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
    auto drawFunc = drawable->CreateDrawFunc();
    drawFunc(filterCanvas.get(), &rect);
    EXPECT_NE(filterCanvas->cacheBehindWindowData_, nullptr);
}

/**
 * @tc.name: RSFilterDrawableTest015
 * @tc.desc: Test RSFilterDrawable CreateDrawFunc
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
    drawable->CreateDrawFunc()(&canvas, nullptr);
    auto drawingFilter = std::static_pointer_cast<RSDrawingFilter>(drawable->filter_);
    auto shaderLinearGradientBlurFilter = drawingFilter->GetShaderFilterWithType(RSUIFilterType::LINEAR_GRADIENT_BLUR);
    EXPECT_EQ(shaderLinearGradientBlurFilter->geoHeight_, 1.0f);

    // Test rect != nullptr
    Drawing::Rect rect(0.0f, 0.0f, 10.0f, 10.0f);
    drawable->CreateDrawFunc()(&canvas, &rect);
    EXPECT_EQ(shaderLinearGradientBlurFilter->geoHeight_, 10.0f);
}

} // namespace OHOS::Rosen
