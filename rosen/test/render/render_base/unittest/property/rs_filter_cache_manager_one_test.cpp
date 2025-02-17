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
#include <parameters.h>

#include "property/rs_filter_cache_manager.h"
#include "render/rs_drawing_filter.h"
#include "render/rs_filter.h"
#include "render/rs_magnifier_shader_filter.h"
#include "skia_adapter/skia_surface.h"
#include "skia_canvas.h"
#include "skia_surface.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSFilterCacheManagerOneTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSFilterCacheManagerOneTest::SetUpTestCase() {}
void RSFilterCacheManagerOneTest::TearDownTestCase() {}
void RSFilterCacheManagerOneTest::SetUp() {}
void RSFilterCacheManagerOneTest::TearDown() {}

/**
 * @tc.name: UpdateCacheStateWithDirtyRegion001
 * @tc.desc: test results of UpdateCacheStateWithDirtyRegion
 * @tc.type: FUNC
 * @tc.require: issueIB7RF8
 */
HWTEST_F(RSFilterCacheManagerOneTest, UpdateCacheStateWithDirtyRegion001, TestSize.Level1)
{
    auto rsFilterCacheManager = std::make_shared<RSFilterCacheManager>();
    rsFilterCacheManager->UpdateCacheStateWithDirtyRegion();
    EXPECT_FALSE(rsFilterCacheManager->IsCacheValid());
    rsFilterCacheManager->cachedSnapshot_ = std::make_shared<RSPaintFilterCanvas::CachedEffectData>();
    rsFilterCacheManager->cachedFilteredSnapshot_ = std::make_shared<RSPaintFilterCanvas::CachedEffectData>();
    rsFilterCacheManager->cacheUpdateInterval_ = 1;
    rsFilterCacheManager->UpdateCacheStateWithDirtyRegion();
    EXPECT_TRUE(rsFilterCacheManager->pendingPurge_);
    rsFilterCacheManager->cacheUpdateInterval_ = -1;
    rsFilterCacheManager->UpdateCacheStateWithDirtyRegion();
    EXPECT_FALSE(rsFilterCacheManager->IsCacheValid());
}

/**
 * @tc.name: UpdateCacheStateWithFilterRegion
 * @tc.desc: test results of UpdateCacheStateWithFilterRegion
 * @tc.type: FUNC
 * @tc.require: issueIB7RF8
 */
HWTEST_F(RSFilterCacheManagerOneTest, UpdateCacheStateWithFilterRegion, TestSize.Level1)
{
    auto rsFilterCacheManager = std::make_shared<RSFilterCacheManager>();
    rsFilterCacheManager->UpdateCacheStateWithFilterRegion();
    EXPECT_FALSE(rsFilterCacheManager->IsCacheValid());
    rsFilterCacheManager->cachedSnapshot_ = std::make_shared<RSPaintFilterCanvas::CachedEffectData>();
    rsFilterCacheManager->cachedFilteredSnapshot_ = std::make_shared<RSPaintFilterCanvas::CachedEffectData>();
    EXPECT_TRUE(rsFilterCacheManager->IsCacheValid());
    rsFilterCacheManager->UpdateCacheStateWithFilterRegion();
    EXPECT_FALSE(rsFilterCacheManager->IsCacheValid());
}

/**
 * @tc.name: DrawFilterWithoutSnapshot
 * @tc.desc: test results of DrawFilterWithoutSnapshot
 * @tc.type: FUNC
 * @tc.require: issueIB7RF8
 */
HWTEST_F(RSFilterCacheManagerOneTest, DrawFilterWithoutSnapshot, TestSize.Level1)
{
    auto rsFilterCacheManager = std::make_shared<RSFilterCacheManager>();
    Drawing::Canvas canvas;
    RSPaintFilterCanvas filterCanvas(&canvas);
    auto shaderFilter = std::make_shared<RSShaderFilter>();
    auto filter = std::make_shared<RSDrawingFilter>(shaderFilter);
    Drawing::RectI src;
    Drawing::RectI dst;
    EXPECT_FALSE(rsFilterCacheManager->DrawFilterWithoutSnapshot(filterCanvas, filter, src, dst, false));
}

/**
 * @tc.name: UpdateCacheStateWithDirtyRegion002
 * @tc.desc: test results of UpdateCacheStateWithDirtyRegion
 * @tc.type: FUNC
 * @tc.require: issueIB7RF8
 */
HWTEST_F(RSFilterCacheManagerOneTest, UpdateCacheStateWithDirtyRegion002, TestSize.Level1)
{
    auto rsFilterCacheManager = std::make_shared<RSFilterCacheManager>();
    RSDirtyRegionManager dirtyManager;
    EXPECT_FALSE(rsFilterCacheManager->UpdateCacheStateWithDirtyRegion(dirtyManager));
    rsFilterCacheManager->cachedSnapshot_ = std::make_shared<RSPaintFilterCanvas::CachedEffectData>();
    rsFilterCacheManager->cachedFilteredSnapshot_ = std::make_shared<RSPaintFilterCanvas::CachedEffectData>();
    EXPECT_FALSE(rsFilterCacheManager->UpdateCacheStateWithDirtyRegion(dirtyManager));
    rsFilterCacheManager->pendingPurge_ = true;
    EXPECT_TRUE(rsFilterCacheManager->UpdateCacheStateWithDirtyRegion(dirtyManager));
    rsFilterCacheManager->cachedSnapshot_ = std::make_shared<RSPaintFilterCanvas::CachedEffectData>();
    rsFilterCacheManager->cachedFilteredSnapshot_ = std::make_shared<RSPaintFilterCanvas::CachedEffectData>();
    // for test
    rsFilterCacheManager->snapshotRegion_.SetAll(5, 5, 10, 10);
    dirtyManager.currentFrameDirtyRegion_.SetAll(0, 0, 10, 10);
    rsFilterCacheManager->cacheUpdateInterval_ = 1;
    EXPECT_FALSE(rsFilterCacheManager->UpdateCacheStateWithDirtyRegion(dirtyManager));
    EXPECT_TRUE(rsFilterCacheManager->pendingPurge_);
    rsFilterCacheManager->cacheUpdateInterval_ = -1;
    EXPECT_FALSE(rsFilterCacheManager->UpdateCacheStateWithDirtyRegion(dirtyManager));
    EXPECT_FALSE(rsFilterCacheManager->IsCacheValid());
}

/**
 * @tc.name: GeneratedCachedEffectData
 * @tc.desc: test results of GeneratedCachedEffectData
 * @tc.type: FUNC
 * @tc.require: issueIB7RF8
 */
HWTEST_F(RSFilterCacheManagerOneTest, GeneratedCachedEffectData, TestSize.Level1)
{
    auto rsFilterCacheManager = std::make_shared<RSFilterCacheManager>();
    Drawing::Canvas canvas;
    RSPaintFilterCanvas filterCanvas(&canvas);
    auto shaderFilter = std::make_shared<RSShaderFilter>();
    auto filter = std::make_shared<RSDrawingFilter>(shaderFilter);
    // for test
    std::optional<Drawing::RectI> srcRect(Drawing::RectI { 0, 0, 100, 100 });
    std::optional<Drawing::RectI> dstRect(Drawing::RectI { 0, 0, 100, 100 });
    rsFilterCacheManager->GeneratedCachedEffectData(filterCanvas, filter, srcRect, dstRect);
    EXPECT_TRUE(filterCanvas.GetDeviceClipBounds().IsEmpty());
}

/**
 * @tc.name: DrawFilter
 * @tc.desc: test results of DrawFilter
 * @tc.type: FUNC
 * @tc.require: issueIB7RF8
 */
HWTEST_F(RSFilterCacheManagerOneTest, DrawFilter, TestSize.Level1)
{
    auto rsFilterCacheManager = std::make_shared<RSFilterCacheManager>();
    Drawing::Canvas canvas;
    RSPaintFilterCanvas filterCanvas(&canvas);
    auto shaderFilter = std::make_shared<RSShaderFilter>();
    auto filter = std::make_shared<RSDrawingFilter>(shaderFilter);
    bool shouldClearFilteredCache = false;
    // for test
    std::optional<Drawing::RectI> srcRect(Drawing::RectI { 0, 0, 100, 100 });
    std::optional<Drawing::RectI> dstRect(Drawing::RectI { 0, 0, 100, 100 });
    rsFilterCacheManager->DrawFilter(filterCanvas, filter, shouldClearFilteredCache, srcRect, dstRect);
    EXPECT_TRUE(filterCanvas.GetDeviceClipBounds().IsEmpty());
}

/**
 * @tc.name: GenerateFilteredSnapshot
 * @tc.desc: test results of GenerateFilteredSnapshot
 * @tc.type: FUNC
 * @tc.require: issueIB7RF8
 */
HWTEST_F(RSFilterCacheManagerOneTest, GenerateFilteredSnapshot, TestSize.Level1)
{
    auto manager = std::make_shared<RSFilterCacheManager>();
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    std::shared_ptr<Drawing::ImageFilter> imageFilter = std::make_shared<Drawing::ImageFilter>();
    std::shared_ptr<RSDrawingFilter> filter = std::make_shared<RSDrawingFilter>(imageFilter, 0);
    Drawing::RectI dstRect;
    manager->GenerateFilteredSnapshot(canvas, filter, dstRect);
    EXPECT_TRUE(canvas.GetSurface() == nullptr);

    auto surfacePtr = std::make_shared<Drawing::Surface>();
    surfacePtr->impl_ = std::make_shared<Drawing::SkiaSurface>();
    canvas.surface_ = surfacePtr.get();
    manager->GenerateFilteredSnapshot(canvas, filter, dstRect);
    EXPECT_TRUE(canvas.GetSurface() != nullptr);
    EXPECT_TRUE(manager->cachedSnapshot_ == nullptr);

    manager->cachedSnapshot_ = std::make_shared<RSPaintFilterCanvas::CachedEffectData>();
    manager->GenerateFilteredSnapshot(canvas, filter, dstRect);
    EXPECT_TRUE(manager->cachedSnapshot_ != nullptr);
    EXPECT_TRUE(manager->cachedSnapshot_->cachedImage_ == nullptr);

    manager->cachedSnapshot_->cachedImage_ = std::make_shared<Drawing::Image>();
    manager->GenerateFilteredSnapshot(canvas, filter, dstRect);
    EXPECT_TRUE(manager->cachedSnapshot_->cachedImage_ != nullptr);
}

/**
 * @tc.name: InvalidateFilterCache
 * @tc.desc: test results of InvalidateFilterCache
 * @tc.type: FUNC
 * @tc.require: issueIB7RF8
 */
HWTEST_F(RSFilterCacheManagerOneTest, InvalidateFilterCache, TestSize.Level1)
{
    auto manager = std::make_shared<RSFilterCacheManager>();
    manager->InvalidateFilterCache(FilterCacheType::BOTH);
    EXPECT_TRUE(manager->cachedFilteredSnapshot_ == nullptr);

    manager->InvalidateFilterCache(FilterCacheType::SNAPSHOT);
    EXPECT_TRUE(manager->cachedFilteredSnapshot_ == nullptr);

    manager->InvalidateFilterCache(FilterCacheType::FILTERED_SNAPSHOT);
    EXPECT_TRUE(manager->cachedFilteredSnapshot_ == nullptr);
}

/**
 * @tc.name: DrawCachedFilteredSnapshot
 * @tc.desc: test results of DrawCachedFilteredSnapshot
 * @tc.type: FUNC
 * @tc.require: issueIB7RF8
 */
HWTEST_F(RSFilterCacheManagerOneTest, DrawCachedFilteredSnapshot, TestSize.Level1)
{
    auto manager = std::make_shared<RSFilterCacheManager>();
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    auto shaderFilter = std::make_shared<RSShaderFilter>();
    auto filter = std::make_shared<RSDrawingFilter>(shaderFilter);
    std::shared_ptr<Drawing::ImageFilter> imageFilter = std::make_shared<Drawing::ImageFilter>();
    Drawing::RectI dstRect;
    manager->DrawCachedFilteredSnapshot(canvas, dstRect, filter);
    EXPECT_TRUE(manager->cachedFilteredSnapshot_ == nullptr);

    manager->cachedFilteredSnapshot_ = std::make_shared<RSPaintFilterCanvas::CachedEffectData>();
    manager->DrawCachedFilteredSnapshot(canvas, dstRect, filter);
    EXPECT_TRUE(manager->cachedFilteredSnapshot_ != nullptr);
    EXPECT_TRUE(manager->cachedFilteredSnapshot_->cachedImage_ == nullptr);

    manager->cachedFilteredSnapshot_->cachedImage_ = std::make_shared<Drawing::Image>();
    manager->DrawCachedFilteredSnapshot(canvas, dstRect, filter);
    EXPECT_TRUE(manager->cachedFilteredSnapshot_->cachedImage_ != nullptr);

    canvas.visibleRect_.right_ = 10;
    canvas.visibleRect_.bottom_ = 10;
    manager->DrawCachedFilteredSnapshot(canvas, dstRect, filter);
    EXPECT_TRUE(manager->cachedFilteredSnapshot_->cachedImage_ != nullptr);
}

/**
 * @tc.name: SetAndGetFilterInvalid
 * @tc.desc: test results of SetAndGetFilterInvalid
 * @tc.type: FUNC
 * @tc.require: issueIB7RF8
 */
HWTEST_F(RSFilterCacheManagerOneTest, SetAndGetFilterInvalid, TestSize.Level1)
{
    auto manager = std::make_shared<RSFilterCacheManager>();
    manager->SetFilterInvalid(true);
    EXPECT_TRUE(manager->GetFilterInvalid());

    manager->SetFilterInvalid(false);
    EXPECT_FALSE(manager->GetFilterInvalid());
}

/**
 * @tc.name: GetCachedType
 * @tc.desc: test results of GetCachedType
 * @tc.type: FUNC
 * @tc.require: issueIB7RF8
 */
HWTEST_F(RSFilterCacheManagerOneTest, GetCachedType, TestSize.Level1)
{
    auto manager = std::make_shared<RSFilterCacheManager>();
    manager->GetCachedType();
    EXPECT_TRUE(manager->cachedSnapshot_ == nullptr);
    EXPECT_TRUE(manager->cachedFilteredSnapshot_ == nullptr);

    manager->cachedFilteredSnapshot_ = std::make_shared<RSPaintFilterCanvas::CachedEffectData>();
    manager->GetCachedType();
    EXPECT_TRUE(manager->cachedSnapshot_ == nullptr);
    EXPECT_TRUE(manager->cachedFilteredSnapshot_);

    manager->cachedSnapshot_ = std::make_shared<RSPaintFilterCanvas::CachedEffectData>();
    manager->cachedFilteredSnapshot_ = nullptr;
    manager->GetCachedType();
    EXPECT_TRUE(manager->cachedSnapshot_);
    EXPECT_TRUE(manager->cachedFilteredSnapshot_ == nullptr);

    manager->cachedFilteredSnapshot_ = std::make_shared<RSPaintFilterCanvas::CachedEffectData>();
    manager->GetCachedType();
    EXPECT_TRUE(manager->cachedSnapshot_);
    EXPECT_TRUE(manager->cachedFilteredSnapshot_);
}
} // namespace Rosen
} // namespace OHOS
