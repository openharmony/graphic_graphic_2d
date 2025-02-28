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
#include "render/rs_kawase_blur_shader_filter.h"
#include "render/rs_magnifier_shader_filter.h"
#include "skia_adapter/skia_surface.h"
#include "skia_canvas.h"
#include "skia_surface.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSFilterCacheManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSFilterCacheManagerTest::SetUpTestCase() {}
void RSFilterCacheManagerTest::TearDownTestCase() {}
void RSFilterCacheManagerTest::SetUp() {}
void RSFilterCacheManagerTest::TearDown() {}

/**
 * @tc.name: GetCacheStateTest
 * @tc.desc: test results of GetCacheState
 * @tc.type: FUNC
 * @tc.require: issueIA5FLZ
 */
HWTEST_F(RSFilterCacheManagerTest, GetCacheStateTest, TestSize.Level1)
{
    auto rsFilterCacheManager = std::make_shared<RSFilterCacheManager>();
    EXPECT_EQ(rsFilterCacheManager->GetCacheState(), "No valid cache found.");
    rsFilterCacheManager->cachedSnapshot_ = std::make_shared<RSPaintFilterCanvas::CachedEffectData>();
    EXPECT_EQ(
        rsFilterCacheManager->GetCacheState(), "Snapshot found in cache. Generating filtered image using cached data.");
    rsFilterCacheManager->cachedFilteredSnapshot_ = std::make_shared<RSPaintFilterCanvas::CachedEffectData>();
    EXPECT_EQ(rsFilterCacheManager->GetCacheState(), "Filtered image found in cache. Reusing cached result.");
}

/**
 * @tc.name: UpdateCacheStateWithFilterHashTest
 * @tc.desc: test results of UpdateCacheStateWithFilterHash
 * @tc.type: FUNC
 * @tc.require: issueIA5FLZ
 */
HWTEST_F(RSFilterCacheManagerTest, UpdateCacheStateWithFilterHashTest, TestSize.Level1)
{
    auto rsFilterCacheManager = std::make_shared<RSFilterCacheManager>();
    auto filter = std::make_shared<RSFilter>();
    rsFilterCacheManager->UpdateCacheStateWithFilterHash(filter);
    EXPECT_EQ(rsFilterCacheManager->cachedFilteredSnapshot_, nullptr);
    rsFilterCacheManager->cachedFilteredSnapshot_ = std::make_shared<RSPaintFilterCanvas::CachedEffectData>();
    rsFilterCacheManager->UpdateCacheStateWithFilterHash(filter);
    EXPECT_NE(rsFilterCacheManager->cachedFilteredSnapshot_, nullptr);
    auto filterHash = filter->Hash();
    rsFilterCacheManager->UpdateCacheStateWithFilterHash(filter);
    EXPECT_EQ(rsFilterCacheManager->cachedFilterHash_, filterHash);
    rsFilterCacheManager->cachedFilterHash_ = 1;
    rsFilterCacheManager->UpdateCacheStateWithFilterHash(filter);
    EXPECT_NE(rsFilterCacheManager->cachedFilterHash_, filterHash);
}

/**
 * @tc.name: UpdateCacheStateWithFilterRegionTest
 * @tc.desc: test results of UpdateCacheStateWithFilterRegion
 * @tc.type: FUNC
 * @tc.require: issueIA5FLZ
 */
HWTEST_F(RSFilterCacheManagerTest, UpdateCacheStateWithFilterRegionTest, TestSize.Level1)
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
 * @tc.name: UpdateCacheStateWithDirtyRegionTest001
 * @tc.desc: test results of UpdateCacheStateWithDirtyRegion
 * @tc.type: FUNC
 * @tc.require: issueIA5FLZ
 */
HWTEST_F(RSFilterCacheManagerTest, UpdateCacheStateWithDirtyRegionTest001, TestSize.Level1)
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
 * @tc.name: UpdateCacheStateWithDirtyRegionTest002
 * @tc.desc: test results of UpdateCacheStateWithDirtyRegion
 * @tc.type: FUNC
 * @tc.require: issueIA5FLZ
 */
HWTEST_F(RSFilterCacheManagerTest, UpdateCacheStateWithDirtyRegionTest002, TestSize.Level1)
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
 * @tc.name: DrawFilterWithoutSnapshotTest
 * @tc.desc: test results of DrawFilterWithoutSnapshot
 * @tc.type: FUNC
 * @tc.require: issueIA5FLZ
 */
HWTEST_F(RSFilterCacheManagerTest, DrawFilterWithoutSnapshotTest, TestSize.Level1)
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
 * @tc.name: DrawFilterWithoutSnapshotTest002
 * @tc.desc: test results of DrawFilterWithoutSnapshot
 * @tc.type: FUNC
 * @tc.require: issueIBE049
 */
HWTEST_F(RSFilterCacheManagerTest, DrawFilterWithoutSnapshotTest002, TestSize.Level1)
{
    auto rsFilterCacheManager = std::make_shared<RSFilterCacheManager>();
    Drawing::Canvas canvas;
    RSPaintFilterCanvas filterCanvas(&canvas);
    
    float blurRadius = 10.0f;
    auto kawaseBlurShaderFilter = std::make_shared<RSKawaseBlurShaderFilter>(blurRadius);
    auto drawingFilter = std::make_shared<RSDrawingFilter>(kawaseBlurShaderFilter);
    drawingFilter->SetFilterType(RSFilter::MATERIAL);

    Drawing::RectI srcRect{ 0, 0, 100, 100 };
    Drawing::RectI dstRect{ 0, 0, 100, 100 };
    int width = 100;
    int height = 100;
    Drawing::ImageInfo imageInfo{ width, height, Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    std::shared_ptr<Drawing::Surface> surface = Drawing::Surface::MakeRaster(imageInfo);
    filterCanvas.surface_ = surface.get();
    EXPECT_NE(filterCanvas.GetSurface(), nullptr);
    rsFilterCacheManager->TakeSnapshot(filterCanvas, drawingFilter, srcRect);
    EXPECT_NE(rsFilterCacheManager->cachedSnapshot_, nullptr);
    EXPECT_NE(rsFilterCacheManager->cachedSnapshot_->cachedImage_, nullptr);
    EXPECT_TRUE(rsFilterCacheManager->DrawFilterWithoutSnapshot(filterCanvas, drawingFilter, srcRect, dstRect, true));
}

/**
 * @tc.name: DrawFilterTest
 * @tc.desc: test results of DrawFilter
 * @tc.type: FUNC
 * @tc.require: issueIA5FLZ
 */
HWTEST_F(RSFilterCacheManagerTest, DrawFilterTest, TestSize.Level1)
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
    rsFilterCacheManager->DrawFilter(filterCanvas, filter, true, shouldClearFilteredCache, srcRect, dstRect);
    EXPECT_TRUE(filterCanvas.GetDeviceClipBounds().IsEmpty());
}

/**
 * @tc.name: DrawFilterTest002
 * @tc.desc: test results of DrawFilter
 * @tc.type: FUNC
 * @tc.require: issueIBE049
 */
HWTEST_F(RSFilterCacheManagerTest, DrawFilterTest002, TestSize.Level1)
{
    auto rsFilterCacheManager = std::make_shared<RSFilterCacheManager>();
    float blurRadius = 10.0f;
    auto kawaseBlurShaderFilter = std::make_shared<RSKawaseBlurShaderFilter>(blurRadius);
    auto drawingFilter = std::make_shared<RSDrawingFilter>(kawaseBlurShaderFilter);
    drawingFilter->SetFilterType(RSFilter::MATERIAL);
    std::optional<Drawing::RectI> srcRect(Drawing::RectI { 0, 0, 100, 100 });
    std::optional<Drawing::RectI> dstRect(Drawing::RectI { 0, 0, 100, 100 });
    int width = 100;
    int height = 100;
    Drawing::ImageInfo imageInfo{ width, height, Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    std::shared_ptr<Drawing::Surface> surface = Drawing::Surface::MakeRaster(imageInfo);
    RSPaintFilterCanvas filterCanvas(surface.get());
    EXPECT_FALSE(filterCanvas.GetDeviceClipBounds().IsEmpty());
    bool shouldClearFilteredCache = false;
    rsFilterCacheManager->DrawFilter(filterCanvas, drawingFilter, true, shouldClearFilteredCache, srcRect, dstRect);
    EXPECT_NE(rsFilterCacheManager->cachedSnapshot_, nullptr);
    EXPECT_NE(rsFilterCacheManager->cachedFilteredSnapshot_, nullptr);
    rsFilterCacheManager->renderClearFilteredCacheAfterDrawing_ = false;
    rsFilterCacheManager->CompactFilterCache();
    rsFilterCacheManager->renderClearFilteredCacheAfterDrawing_ = true;
    rsFilterCacheManager->CompactFilterCache();
    EXPECT_EQ(rsFilterCacheManager->cachedSnapshot_, nullptr);
    EXPECT_EQ(rsFilterCacheManager->cachedFilteredSnapshot_, nullptr);
}

/**
 * @tc.name: GeneratedCachedEffectDataTest
 * @tc.desc: test results of GeneratedCachedEffectData
 * @tc.type: FUNC
 * @tc.require: issueIA5FLZ
 */
HWTEST_F(RSFilterCacheManagerTest, GeneratedCachedEffectDataTest, TestSize.Level1)
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
 * @tc.name: GeneratedCachedEffectDataTest002
 * @tc.desc: test results of GeneratedCachedEffectData
 * @tc.type: FUNC
 * @tc.require: issueIBE049
 */
HWTEST_F(RSFilterCacheManagerTest, GeneratedCachedEffectDataTest002, TestSize.Level1)
{
    auto rsFilterCacheManager = std::make_shared<RSFilterCacheManager>();
    float blurRadius = 10.0f;
    auto kawaseBlurShaderFilter = std::make_shared<RSKawaseBlurShaderFilter>(blurRadius);
    auto drawingFilter = std::make_shared<RSDrawingFilter>(kawaseBlurShaderFilter);
    drawingFilter->SetFilterType(RSFilter::MATERIAL);
    std::optional<Drawing::RectI> srcRect(Drawing::RectI { 0, 0, 100, 100 });
    std::optional<Drawing::RectI> dstRect(Drawing::RectI { 0, 0, 100, 100 });
    int width = 100;
    int height = 100;
    Drawing::ImageInfo imageInfo{ width, height, Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    std::shared_ptr<Drawing::Surface> surface = Drawing::Surface::MakeRaster(imageInfo);
    RSPaintFilterCanvas filterCanvas(surface.get());
    EXPECT_FALSE(filterCanvas.GetDeviceClipBounds().IsEmpty());
    const auto& [src, dst] = rsFilterCacheManager->ValidateParams(filterCanvas, srcRect, dstRect);
    EXPECT_FALSE(src.IsEmpty() || dst.IsEmpty());
    EXPECT_EQ(rsFilterCacheManager->cachedSnapshot_, nullptr);
    EXPECT_EQ(rsFilterCacheManager->cachedFilteredSnapshot_, nullptr);
    auto cachedFilteredSnapshot =
        rsFilterCacheManager->GeneratedCachedEffectData(filterCanvas, drawingFilter, srcRect, dstRect);
    EXPECT_NE(rsFilterCacheManager->cachedSnapshot_, nullptr);
    EXPECT_NE(cachedFilteredSnapshot, nullptr);
}

/**
 * @tc.name: TakeSnapshotTest
 * @tc.desc: test results of TakeSnapshot
 * @tc.type: FUNC
 * @tc.require: issueIA5FLZ
 */
HWTEST_F(RSFilterCacheManagerTest, TakeSnapshotTest, TestSize.Level1)
{
    auto rsFilterCacheManager = std::make_shared<RSFilterCacheManager>();
    Drawing::Canvas canvas;
    RSPaintFilterCanvas filterCanvas(&canvas);
    auto shaderFilter = std::make_shared<RSShaderFilter>();
    auto filter = std::make_shared<RSDrawingFilter>(shaderFilter);
    // for test
    Drawing::RectI srcRect(0, 0, 100, 100);
    rsFilterCacheManager->TakeSnapshot(filterCanvas, filter, srcRect);
    EXPECT_EQ(filterCanvas.GetSurface(), nullptr);
    filterCanvas.surface_ = new Drawing::Surface();
    rsFilterCacheManager->TakeSnapshot(filterCanvas, filter, srcRect);
    EXPECT_NE(filterCanvas.GetSurface(), nullptr);
    auto para = std::make_shared<RSMagnifierParams>();
    auto rsMagnifierShaderFilter = std::make_shared<RSMagnifierShaderFilter>(para);
    filter = std::make_shared<RSDrawingFilter>(rsMagnifierShaderFilter);
    rsFilterCacheManager->TakeSnapshot(filterCanvas, filter, srcRect);
    EXPECT_NE(filter->GetShaderFilterWithType(RSShaderFilter::MAGNIFIER), nullptr);
}

/**
 * @tc.name: TakeSnapshotTest002
 * @tc.desc: test results of TakeSnapshot
 * @tc.type: FUNC
 * @tc.require: issueIBE049
 */
HWTEST_F(RSFilterCacheManagerTest, TakeSnapshotTest002, TestSize.Level1)
{
    auto rsFilterCacheManager = std::make_shared<RSFilterCacheManager>();
    Drawing::Canvas canvas;
    RSPaintFilterCanvas filterCanvas(&canvas);
    auto shaderFilter = std::make_shared<RSShaderFilter>();
    auto filter = std::make_shared<RSDrawingFilter>(shaderFilter);

    Drawing::RectI srcRect(0, 0, 100, 100);
    int width = 100;
    int height = 100;
    Drawing::ImageInfo imageInfo{ width, height, Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    std::shared_ptr<Drawing::Surface> surface = Drawing::Surface::MakeRaster(imageInfo);
    filterCanvas.surface_ = surface.get();
    EXPECT_NE(filterCanvas.GetSurface(), nullptr);
    std::shared_ptr<Drawing::Image> snapShot = filterCanvas.GetSurface()->GetImageSnapshot(srcRect);
    EXPECT_NE(snapShot, nullptr);
    rsFilterCacheManager->TakeSnapshot(filterCanvas, filter, srcRect);
    EXPECT_NE(rsFilterCacheManager->cachedSnapshot_, nullptr);
}

/**
 * @tc.name: GenerateFilteredSnapshotTest
 * @tc.desc: test results of GenerateFilteredSnapshot
 * @tc.type: FUNC
 * @tc.require: issueIA5FLZ
 */
HWTEST_F(RSFilterCacheManagerTest, GenerateFilteredSnapshotTest, TestSize.Level1)
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
 * @tc.name: GenerateFilteredSnapshotTest002
 * @tc.desc: test results of GenerateFilteredSnapshot
 * @tc.type: FUNC
 * @tc.require: issueIBE049
 */
HWTEST_F(RSFilterCacheManagerTest, GenerateFilteredSnapshotTest002, TestSize.Level1)
{
    auto rsFilterCacheManager = std::make_shared<RSFilterCacheManager>();
    Drawing::Canvas canvas;
    RSPaintFilterCanvas filterCanvas(&canvas);
    
    float blurRadius = 10.0f;
    auto kawaseBlurShaderFilter = std::make_shared<RSKawaseBlurShaderFilter>(blurRadius);
    auto drawingFilter = std::make_shared<RSDrawingFilter>(kawaseBlurShaderFilter);
    drawingFilter->SetFilterType(RSFilter::MATERIAL);

    Drawing::RectI srcRect{ 0, 0, 100, 100 };
    Drawing::RectI dstRect{ 0, 0, 100, 100 };
    int width = 100;
    int height = 100;
    Drawing::ImageInfo imageInfo{ width, height, Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    std::shared_ptr<Drawing::Surface> surface = Drawing::Surface::MakeRaster(imageInfo);
    filterCanvas.surface_ = surface.get();
    EXPECT_NE(filterCanvas.GetSurface(), nullptr);
    auto offscreenSurface = filterCanvas.GetSurface()->MakeSurface(dstRect.GetWidth(), dstRect.GetHeight());
    EXPECT_NE(offscreenSurface, nullptr);
    rsFilterCacheManager->TakeSnapshot(filterCanvas, drawingFilter, srcRect);
    EXPECT_NE(rsFilterCacheManager->cachedSnapshot_, nullptr);
    rsFilterCacheManager->GenerateFilteredSnapshot(filterCanvas, drawingFilter, dstRect);
    EXPECT_NE(rsFilterCacheManager->cachedFilteredSnapshot_, nullptr);
}

/**
 * @tc.name: DrawCachedFilteredSnapshotTest
 * @tc.desc: test results of DrawCachedFilteredSnapshot
 * @tc.type: FUNC
 * @tc.require: issueIA5FLZ
 */
HWTEST_F(RSFilterCacheManagerTest, DrawCachedFilteredSnapshotTest, TestSize.Level1)
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
 * @tc.name: InvalidateFilterCacheTest
 * @tc.desc: test results of InvalidateFilterCache
 * @tc.type: FUNC
 * @tc.require: issueIA5FLZ
 */
HWTEST_F(RSFilterCacheManagerTest, InvalidateFilterCacheTest, TestSize.Level1)
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
 * @tc.name: SetAndGetFilterInvalidTest
 * @tc.desc: test results of SetAndGetFilterInvalid
 * @tc.type: FUNC
 * @tc.require: issueIA5FLZ
 */
HWTEST_F(RSFilterCacheManagerTest, SetAndGetFilterInvalidTest, TestSize.Level1)
{
    auto manager = std::make_shared<RSFilterCacheManager>();
    manager->SetFilterInvalid(true);
    EXPECT_TRUE(manager->GetFilterInvalid());

    manager->SetFilterInvalid(false);
    EXPECT_FALSE(manager->GetFilterInvalid());
}

/**
 * @tc.name: GetCachedTypeTest
 * @tc.desc: test results of GetCachedType
 * @tc.type: FUNC
 * @tc.require: issueIA5FLZ
 */
HWTEST_F(RSFilterCacheManagerTest, GetCachedTypeTest, TestSize.Level1)
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

/**
 * @tc.name: CheckCachedImagesTest
 * @tc.desc: test results of CheckCachedImages
 * @tc.type:FUNC
 * @tc.require: issueIA5FLZ
 */
HWTEST_F(RSFilterCacheManagerTest, CheckCachedImagesTest, TestSize.Level1)
{
    auto rsFilterCacheManager = std::make_shared<RSFilterCacheManager>();
    Drawing::Canvas canvas;
    RSPaintFilterCanvas filterCanvas(&canvas);
    rsFilterCacheManager->CheckCachedImages(filterCanvas);
    EXPECT_EQ(rsFilterCacheManager->cachedSnapshot_, nullptr);
    EXPECT_EQ(rsFilterCacheManager->cachedFilteredSnapshot_, nullptr);

    rsFilterCacheManager->cachedSnapshot_ = std::make_shared<RSPaintFilterCanvas::CachedEffectData>();
    rsFilterCacheManager->cachedFilteredSnapshot_ = std::make_shared<RSPaintFilterCanvas::CachedEffectData>();
    rsFilterCacheManager->CheckCachedImages(filterCanvas);
    EXPECT_EQ(rsFilterCacheManager->cachedSnapshot_, nullptr);
    EXPECT_EQ(rsFilterCacheManager->cachedFilteredSnapshot_, nullptr);
}

/**
 * @tc.name: ValidateParamsTest
 * @tc.desc: test results of ValidateParams
 * @tc.type: FUNC
 * @tc.require: issueIA5FLZ
 */
HWTEST_F(RSFilterCacheManagerTest, ValidateParamsTest, TestSize.Level1)
{
    auto rsFilterCacheManager = std::make_shared<RSFilterCacheManager>();
    Drawing::Canvas canvas;
    RSPaintFilterCanvas filterCanvas(&canvas);
    std::optional<Drawing::RectI> srcRect;
    std::optional<Drawing::RectI> dstRect;
    auto [src1, dst1] = rsFilterCacheManager->ValidateParams(filterCanvas, srcRect, dstRect);
    EXPECT_EQ(src1, filterCanvas.GetRoundInDeviceClipBounds());
    EXPECT_EQ(dst1, filterCanvas.GetRoundInDeviceClipBounds());
    // for test
    srcRect = Drawing::RectI { 0, 0, 100, 100 };
    dstRect = Drawing::RectI { 0, 0, 100, 100 };
    auto [src2, dst2] = rsFilterCacheManager->ValidateParams(filterCanvas, srcRect, dstRect);
    EXPECT_EQ(src2, srcRect);
    EXPECT_EQ(dst2, dstRect);
}
} // namespace Rosen
} // namespace OHOS
