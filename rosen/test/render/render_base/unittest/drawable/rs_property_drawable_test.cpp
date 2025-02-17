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
#include "render/rs_shader_filter.h"
#include "render/rs_aibar_shader_filter.h"
#include "render/rs_linear_gradient_blur_shader_filter.h"
#include "drawable/rs_property_drawable_utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSPropertyDrawableUnitTest : public testing::Test {
public:
    void SetUp() override;
    void TearDown() override;
    static void SetUpTestCase();
    static void TearDownTestCase();
};

void RSPropertyDrawableUnitTest::SetUpTestCase() {}
void RSPropertyDrawableUnitTest::TearDownTestCase() {}
void RSPropertyDrawableUnitTest::SetUp() {}
void RSPropertyDrawableUnitTest::TearDown() {}

/*
 * @tc.name: MarkBlurIntersectWithDRM001
 * @tc.desc: class RSFilterDrawable MarkBlurIntersectWithDRM test
 * @tc.type: FUNC
 * @tc.require: issuesIAQZ4I
 */
HWTEST_F(RSPropertyDrawableUnitTest, MarkBlurIntersectWithDRM001, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSFilterDrawable> filterDrawable = std::make_shared<DrawableV2::RSFilterDrawable>();
    EXPECT_NE(filterDrawable, nullptr);
    filterDrawable->MarkBlurIntersectWithDRM(true, true);
}

/**
 * @tc.name: OnSyncAndCreateDrawFuncTest001
 * @tc.desc: class RSPropertyDrawable OnSync and CreateDrawFunc test
 * @tc.type:FUNC
 * @tc.require: issueI9VSPU
 */
HWTEST_F(RSPropertyDrawableUnitTest, OnSyncAndCreateDrawFuncTest001, TestSize.Level1)
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
HWTEST_F(RSPropertyDrawableUnitTest, RSPropertyDrawCmdListUpdaterTest002, TestSize.Level1)
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
HWTEST_F(RSPropertyDrawableUnitTest, OnGenerateAndOnUpdateTest003, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSFrameOffsetDrawable> frameOffsetDrawable =
        std::make_shared<DrawableV2::RSFrameOffsetDrawable>();
    EXPECT_NE(frameOffsetDrawable, nullptr);

    RSRenderNode node(0);
    node.renderContent_->renderProperties_.frameOffsetX_ = 0.0f;
    node.renderContent_->renderProperties_.frameOffsetY_ = 0.0f;
    EXPECT_EQ(frameOffsetDrawable->OnGenerate(node), nullptr);

    node.renderContent_->renderProperties_.frameOffsetX_ = 1.0f;
    EXPECT_NE(frameOffsetDrawable->OnGenerate(node), nullptr);
}

/**
 * @tc.name: OnUpdateTest004
 * @tc.desc: class RSClipToBoundsDrawable OnUpdate test
 * @tc.type:FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSPropertyDrawableUnitTest, OnUpdateTest004, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSClipToBoundsDrawable> clipToBoundsDrawable =
        std::make_shared<DrawableV2::RSClipToBoundsDrawable>();
    EXPECT_NE(clipToBoundsDrawable, nullptr);

    RSRenderNode nodeTest1(0);
    nodeTest1.renderContent_->renderProperties_.clipPath_ = std::make_shared<RSPath>();
    EXPECT_NE(nodeTest1.renderContent_->renderProperties_.clipPath_, nullptr);
    EXPECT_TRUE(clipToBoundsDrawable->OnUpdate(nodeTest1));

    RSRenderNode nodeTest2(0);
    RectT<float> rect(1.0f, 1.0f, 1.0f, 1.0f);
    RRectT<float> rectt(rect, 1.0f, 1.0f);
    nodeTest2.renderContent_->renderProperties_.clipRRect_ = rectt;
    EXPECT_TRUE(clipToBoundsDrawable->OnUpdate(nodeTest2));

    RSRenderNode nodeTest3(0);
    nodeTest3.renderContent_->renderProperties_.cornerRadius_ = 1.0f;
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
HWTEST_F(RSPropertyDrawableUnitTest, OnGenerateAndOnUpdateTest005, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSClipToFrameDrawable> clipToFrameDrawable =
        std::make_shared<DrawableV2::RSClipToFrameDrawable>();
    EXPECT_NE(clipToFrameDrawable, nullptr);

    RSRenderNode node(0);
    node.renderContent_->renderProperties_.clipToFrame_ = false;
    EXPECT_EQ(clipToFrameDrawable->OnGenerate(node), nullptr);

    RSObjGeometry geometry;

    geometry.width_ = 1.0f;
    geometry.height_ = 1.0f;
    node.renderContent_->renderProperties_.frameGeo_ = geometry;
    node.renderContent_->renderProperties_.clipToFrame_ = true;
    EXPECT_NE(clipToFrameDrawable->OnGenerate(node), nullptr);
}

/**
 * @tc.name: RSFilterDrawableTest006
 * @tc.desc: class RSFilterDrawable and OnSync test
 * @tc.type:FUNC
 * @tc.require: issueI9VSPU
 */
HWTEST_F(RSPropertyDrawableUnitTest, RSFilterDrawableTest006, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSFilterDrawable> filterDrawable = std::make_shared<DrawableV2::RSFilterDrawable>();
    EXPECT_NE(filterDrawable, nullptr);

    filterDrawable->needSync_ = true;
    filterDrawable->stagingFilter_ = nullptr;
    filterDrawable->isFilterCacheValid_ = true;
    filterDrawable->OnSync();
    EXPECT_FALSE(filterDrawable->needSync_);
    EXPECT_FALSE(filterDrawable->isFilterCacheValid_);

    std::shared_ptr<RSShaderFilter> shaderFilter = std::make_shared<RSShaderFilter>();
    EXPECT_NE(shaderFilter, nullptr);
    shaderFilter->type_ = RSShaderFilter::LINEAR_GRADIENT_BLUR;
    filterDrawable->filter_ = std::make_shared<RSDrawingFilter>(shaderFilter);
    EXPECT_NE(filterDrawable->filter_, nullptr);
    filterDrawable->filter_->type_ = RSFilter::LINEAR_GRADIENT_BLUR;
    std::unique_ptr<RSFilterCacheManager> cacheManager = std::make_unique<RSFilterCacheManager>();
    EXPECT_NE(cacheManager, nullptr);
    filterDrawable->cacheManager_ = std::move(cacheManager);
    Drawing::Canvas canvas;
    Drawing::Rect rect(0.0f, 0.0f, 1.0f, 1.0f);
    filterDrawable->CreateDrawFunc()(&canvas, &rect);
    filterDrawable->MarkEffectNode();
    EXPECT_TRUE(filterDrawable->stagingIsEffectNode_);
}

/**
 * @tc.name: MarkNeedClearFilterCacheTest007
 * @tc.desc: class RSFilterDrawable MarkNeedClearFilterCache test
 * @tc.type:FUNC
 * @tc.require: issueI9VSPU
 */
HWTEST_F(RSPropertyDrawableUnitTest, MarkNeedClearFilterCacheTest007, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSFilterDrawable> filterDrawable = std::make_shared<DrawableV2::RSFilterDrawable>();
    EXPECT_NE(filterDrawable, nullptr);
    filterDrawable->cacheManager_ = nullptr;
    filterDrawable->MarkNeedClearFilterCache();
    std::unique_ptr<RSFilterCacheManager> cacheManager = std::make_unique<RSFilterCacheManager>();
    EXPECT_NE(cacheManager, nullptr);
    filterDrawable->cacheManager_ = std::move(cacheManager);
    filterDrawable->isFilterCacheValid_ = true;
    filterDrawable->stagingForceClearCacheForLastFrame_ = true;
    filterDrawable->MarkNeedClearFilterCache();
    EXPECT_FALSE(filterDrawable->isFilterCacheValid_);
    filterDrawable->stagingForceClearCacheForLastFrame_ = false;
    filterDrawable->lastCacheType_ = FilterCacheType::NONE;
    filterDrawable->MarkNeedClearFilterCache();
    filterDrawable->lastCacheType_ = FilterCacheType::SNAPSHOT;
    filterDrawable->stagingForceUseCache_ = true;
    filterDrawable->MarkNeedClearFilterCache();
    filterDrawable->stagingForceUseCache_ = false;
    filterDrawable->stagingForceClearCache_ = true;
    filterDrawable->MarkNeedClearFilterCache();
    filterDrawable->stagingForceClearCache_ = false;
    filterDrawable->stagingFilterRegionChanged_ = true;
    filterDrawable->stagingRotationChanged_ = false;
    filterDrawable->MarkNeedClearFilterCache();
    filterDrawable->stagingFilterRegionChanged_ = false;
    filterDrawable->stagingFilterInteractWithDirty_ = false;
    filterDrawable->pendingPurge_ = true;
    filterDrawable->MarkNeedClearFilterCache();
    filterDrawable->stagingFilterInteractWithDirty_ = true;
    filterDrawable->cacheUpdateInterval_ = 0;
    filterDrawable->MarkNeedClearFilterCache();
    filterDrawable->stagingFilterInteractWithDirty_ = false;
    filterDrawable->pendingPurge_ = false;
    filterDrawable->stagingRotationChanged_ = true;
    filterDrawable->cacheUpdateInterval_ = 0;
    filterDrawable->MarkNeedClearFilterCache();
    filterDrawable->cacheUpdateInterval_ = 1;
    filterDrawable->MarkNeedClearFilterCache();
}

/**
 * @tc.name: RecordFilterInfosTest008
 * @tc.desc: class RSFilterDrawable RecordFilterInfos ClearFilterCache UpdateFlags test
 * @tc.type:FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSPropertyDrawableUnitTest, RecordFilterInfosTest008, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSFilterDrawable> filterDrawable = std::make_shared<DrawableV2::RSFilterDrawable>();
    EXPECT_NE(filterDrawable, nullptr);

    std::shared_ptr<RSDrawingFilter> rsFilter = nullptr;
    filterDrawable->RecordFilterInfos(rsFilter);
    std::shared_ptr<RSShaderFilter> shaderFilter = std::make_shared<RSShaderFilter>();
    EXPECT_NE(shaderFilter, nullptr);
    filterDrawable->stagingCachedFilterHash_ = 1;
    rsFilter = std::make_shared<RSDrawingFilter>(shaderFilter);
    filterDrawable->RecordFilterInfos(rsFilter);
    EXPECT_EQ(filterDrawable->stagingCachedFilterHash_, rsFilter->Hash());

    // RSProperties::FilterCacheEnabled is true
    filterDrawable->ClearFilterCache();
    filterDrawable->filterType_ = RSFilter::AIBAR;
    filterDrawable->stagingIsOccluded_ = true;
    filterDrawable->cacheManager_ = std::make_unique<RSFilterCacheManager>();
    filterDrawable->filter_ = std::make_shared<RSFilter>();
    filterDrawable->ClearFilterCache();
    filterDrawable->renderIsEffectNode_ = true;
    filterDrawable->ClearFilterCache();
    filterDrawable->stagingIsOccluded_ = false;
    filterDrawable->stagingFilterRegionChanged_ = true;
    filterDrawable->stagingClearType_ = FilterCacheType::FILTERED_SNAPSHOT;
    filterDrawable->cacheManager_->cachedFilteredSnapshot_ = std::make_shared<RSPaintFilterCanvas::CachedEffectData>();
    filterDrawable->ClearFilterCache();

    filterDrawable->pendingPurge_ = true;
    filterDrawable->UpdateFlags(FilterCacheType::NONE, false);
    EXPECT_FALSE(filterDrawable->pendingPurge_);
    filterDrawable->stagingFilterInteractWithDirty_ = true;
    filterDrawable->cacheUpdateInterval_ = 3;
    filterDrawable->UpdateFlags(FilterCacheType::NONE, true);
    EXPECT_EQ(filterDrawable->cacheUpdateInterval_, 2);
    filterDrawable->stagingFilterInteractWithDirty_ = false;
    filterDrawable->stagingRotationChanged_ = true;
    filterDrawable->UpdateFlags(FilterCacheType::NONE, true);
    EXPECT_EQ(filterDrawable->cacheUpdateInterval_, 1);
    filterDrawable->stagingRotationChanged_ = false;
    filterDrawable->filterType_ = RSFilter::AIBAR;
    filterDrawable->UpdateFlags(FilterCacheType::NONE, true);
    EXPECT_EQ(filterDrawable->cacheUpdateInterval_, 1);
    filterDrawable->stagingIsAIBarInteractWithHWC_ = true;
    filterDrawable->cacheUpdateInterval_ = 0;
    filterDrawable->UpdateFlags(FilterCacheType::NONE, true);
    EXPECT_EQ(filterDrawable->cacheUpdateInterval_, 0);
    filterDrawable->cacheUpdateInterval_ = 3;
    filterDrawable->stagingIsAIBarInteractWithHWC_ = true;
    filterDrawable->UpdateFlags(FilterCacheType::NONE, true);
    EXPECT_EQ(filterDrawable->cacheUpdateInterval_, 2);
}

/**
 * @tc.name: IsAIBarCacheValidTest009
 * @tc.desc: class RSFilterDrawable IsAIBarCacheValid test
 * @tc.type:FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSPropertyDrawableUnitTest, IsAIBarCacheValidTest009, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSFilterDrawable> filterDrawable = std::make_shared<DrawableV2::RSFilterDrawable>();
    EXPECT_NE(filterDrawable, nullptr);

    EXPECT_FALSE(filterDrawable->IsAIBarCacheValid());
    filterDrawable->filterType_ = RSFilter::AIBAR;
    EXPECT_FALSE(filterDrawable->IsAIBarCacheValid());
    filterDrawable->cacheUpdateInterval_ = 1;
    filterDrawable->stagingForceClearCacheForLastFrame_ = true;
    EXPECT_FALSE(filterDrawable->IsAIBarCacheValid());
    filterDrawable->stagingForceClearCacheForLastFrame_ = false;
    EXPECT_TRUE(filterDrawable->IsAIBarCacheValid());
}

/**
 * @tc.name: RSFilterDrawableTest010
 * @tc.desc: CreateDrawFunc
 * @tc.type:FUNC
 * @tc.require: issueI9VSPU
 */
HWTEST_F(RSPropertyDrawableUnitTest, RSFilterDrawableTest010, TestSize.Level1)
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
}

/**
 * @tc.name: RSFilterDrawableTest011
 * @tc.desc: IsFilterCacheValidForOcclusion
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableUnitTest, RSFilterDrawableTest011, TestSize.Level1)
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

    filterDrawable->renderClearType_ = FilterCacheType::SNAPSHOT;
    cacheManager->InvalidateFilterCache(filterDrawable->renderClearType_);
    EXPECT_FALSE(filterDrawable->IsFilterCacheValidForOcclusion());

    // cacheType: FilterCacheType::BOTH
    cacheManager->cachedSnapshot_ = std::make_shared<RSPaintFilterCanvas::CachedEffectData>();
    ASSERT_NE(cacheManager->cachedSnapshot_, nullptr);
    cacheManager->cachedFilteredSnapshot_ = std::make_shared<RSPaintFilterCanvas::CachedEffectData>();
    ASSERT_NE(cacheManager->cachedFilteredSnapshot_, nullptr);
    EXPECT_TRUE(filterDrawable->IsFilterCacheValidForOcclusion());

    filterDrawable->renderClearType_ = FilterCacheType::BOTH;
    cacheManager->InvalidateFilterCache(filterDrawable->renderClearType_);
    EXPECT_FALSE(filterDrawable->IsFilterCacheValidForOcclusion());

    // cacheType: FilterCacheType::FILTERED_SNAPSHOT
    cacheManager->cachedSnapshot_ = nullptr;
    cacheManager->cachedFilteredSnapshot_ = std::make_shared<RSPaintFilterCanvas::CachedEffectData>();
    ASSERT_NE(cacheManager->cachedFilteredSnapshot_, nullptr);
    EXPECT_TRUE(filterDrawable->IsFilterCacheValidForOcclusion());

    filterDrawable->renderClearType_ = FilterCacheType::FILTERED_SNAPSHOT;
    cacheManager->InvalidateFilterCache(filterDrawable->renderClearType_);
    EXPECT_FALSE(filterDrawable->IsFilterCacheValidForOcclusion());
}

/**
 * @tc.name: RSFilterDrawableTest012
 * @tc.desc: Test RSFilterDrawable needDrawBehindWindow branch
 * @tc.type: FUNC
 * @tc.require: issueIB0UQV
 */
HWTEST_F(RSPropertyDrawableUnitTest, RSFilterDrawableTest012, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSFilterDrawable>();
    ASSERT_NE(drawable, nullptr);
    Drawing::Canvas canvas;
    Drawing::Rect rect(0.0f, 0.0f, 1.0f, 1.0f);
    auto drawFunc = drawable->CreateDrawFunc();
    drawFunc(&canvas, &rect);
    drawable->filter_ = RSPropertyDrawableUtils::GenerateBehindWindowFilter(80.0f, 1.9f, 1.0f, RSColor(0xFFFFFFE5));
    EXPECT_NE(drawable->filter_, nullptr);
    drawFunc(&canvas, &rect);
    ASSERT_TRUE(true);
}
} // namespace OHOS::Rosen
