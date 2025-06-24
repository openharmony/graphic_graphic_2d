/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

#include <cstddef>
#include "gtest/gtest.h"
#include "skia_adapter/skia_canvas_autocache.h"
#include "draw/core_canvas.h"
#include "draw/paint.h"
#include "skia_adapter/skia_convert_utils.h"
#include "skia_adapter/skia_data.h"
#include "skia_adapter/skia_image.h"
#include "skia_adapter/skia_image_info.h"
#include "skia_adapter/skia_matrix.h"
#include "skia_adapter/skia_paint.h"
#include "skia_adapter/skia_path.h"
#include "skia_adapter/skia_region.h"
#include "skia_adapter/skia_text_blob.h"
#include "skia_adapter/skia_vertices.h"
#include "text/rs_xform.h"
#include "utils/rect.h"
#ifdef USE_M133_SKIA
#include "include/core/SkRRect.h"
#include "include/core/SkImage.h"
#endif

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaCanvasAutoCacheTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SkiaCanvasAutoCacheTest::SetUpTestCase() {}
void SkiaCanvasAutoCacheTest::TearDownTestCase() {}
void SkiaCanvasAutoCacheTest::SetUp() {}
void SkiaCanvasAutoCacheTest::TearDown() {}

/**
 * @tc.name: SkiaCanvasAutoCache001
 * @tc.desc: Test SkiaCanvasAutoCache
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaCanvasAutoCacheTest, SkiaCanvasAutoCache001, TestSize.Level1)
{
    SkCanvas canvas;
    std::shared_ptr<SkiaCanvasAutoCache> skiaCanvasAutoCache = std::make_shared<SkiaCanvasAutoCache>(&canvas);
    ASSERT_TRUE(skiaCanvasAutoCache != nullptr);
}

/**
 * @tc.name: Init001
 * @tc.desc: Test SkiaCanvasAutoCache
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaCanvasAutoCacheTest, Init001, TestSize.Level1)
{
    SkCanvas canvas;
    std::shared_ptr<SkiaCanvasAutoCache> skiaCanvasAutoCache = std::make_shared<SkiaCanvasAutoCache>(&canvas);
    auto originMatrix = skiaCanvasAutoCache->nodeMatrix_;
    SkMatrix m;
    skiaCanvasAutoCache->Init(m);
    ASSERT_TRUE(skiaCanvasAutoCache->nodeMatrix_ == originMatrix);
}

/**
 * @tc.name: OpCanCache001
 * @tc.desc: Test SkiaCanvasAutoCache
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaCanvasAutoCacheTest, OpCanCache001, TestSize.Level1)
{
    SkCanvas canvas;
    std::shared_ptr<SkiaCanvasAutoCache> skiaCanvasAutoCache = std::make_shared<SkiaCanvasAutoCache>(&canvas);
    auto v = skiaCanvasAutoCache->GetOpListDrawArea();
    ASSERT_TRUE(v.size() == 0);
}

/**
 * @tc.name: GetOpUnionRect001
 * @tc.desc: Test SkiaCanvasAutoCache
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaCanvasAutoCacheTest, GetOpUnionRect001, TestSize.Level1)
{
    SkCanvas canvas;
    std::shared_ptr<SkiaCanvasAutoCache> skiaCanvasAutoCache = std::make_shared<SkiaCanvasAutoCache>(&canvas);
    ASSERT_TRUE(skiaCanvasAutoCache->GetOpUnionRect().isEmpty());
}

/**
 * @tc.name: GetOpsNum001
 * @tc.desc: Test SkiaCanvasAutoCache
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaCanvasAutoCacheTest, GetOpsNum001, TestSize.Level1)
{
    SkCanvas canvas;
    std::shared_ptr<SkiaCanvasAutoCache> skiaCanvasAutoCache = std::make_shared<SkiaCanvasAutoCache>(&canvas);
    ASSERT_TRUE(skiaCanvasAutoCache->GetOpsNum() == 0);
}

/**
 * @tc.name: GetOpsPercent001
 * @tc.desc: Test SkiaCanvasAutoCache
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaCanvasAutoCacheTest, GetOpsPercent001, TestSize.Level1)
{
    SkCanvas canvas;
    std::shared_ptr<SkiaCanvasAutoCache> skiaCanvasAutoCache = std::make_shared<SkiaCanvasAutoCache>(&canvas);
    ASSERT_TRUE(skiaCanvasAutoCache->GetOpsPercent() == 0);
}

/**
 * @tc.name: ShowDrawResult001
 * @tc.desc: Test SkiaCanvasAutoCache
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaCanvasAutoCacheTest, ShowDrawResult001, TestSize.Level1)
{
    SkCanvas canvas;
    std::shared_ptr<SkiaCanvasAutoCache> skiaCanvasAutoCache = std::make_shared<SkiaCanvasAutoCache>(&canvas);
    ASSERT_TRUE(skiaCanvasAutoCache->GetOpsPercent() == 0);
    SkRect bound;
    skiaCanvasAutoCache->ShowDrawResult(bound);
}

/**
 * @tc.name: getBaseLayerSize001
 * @tc.desc: Test SkiaCanvasAutoCache
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaCanvasAutoCacheTest, getBaseLayerSize001, TestSize.Level1)
{
    SkCanvas canvas;
    std::shared_ptr<SkiaCanvasAutoCache> skiaCanvasAutoCache = std::make_shared<SkiaCanvasAutoCache>(&canvas);
    SkISize size = skiaCanvasAutoCache->getBaseLayerSize();
    ASSERT_TRUE(size.area() == 0);
}

/**
 * @tc.name: recordingContext001
 * @tc.desc: Test SkiaCanvasAutoCache
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaCanvasAutoCacheTest, recordingContext001, TestSize.Level1)
{
    SkCanvas canvas;
    std::shared_ptr<SkiaCanvasAutoCache> skiaCanvasAutoCache = std::make_shared<SkiaCanvasAutoCache>(&canvas);
    ASSERT_TRUE(skiaCanvasAutoCache->recordingContext() == nullptr);
}

/**
 * @tc.name: RecordDrawArea001
 * @tc.desc: Test SkiaCanvasAutoCache
 * @tc.type: FUNC
 * @tc.require: IAKGJ7
 */
HWTEST_F(SkiaCanvasAutoCacheTest, RecordDrawArea001, TestSize.Level1)
{
    SkCanvas canvas;
    SkiaCanvasAutoCache skiaCanvasAutoCache(&canvas);
    const SkRect bounds = SkRect::MakeLTRB(100, 100, 400, 300); // rect.Left, rect.Top, rect.Right, rect.Bottom
    Paint paint;
    paint.SetAntiAlias(true);
    SkPaint skPaint;
    SkiaPaint::PaintToSkPaint(paint, skPaint);
    SkiaMatrix skiaMatrix;
    skiaMatrix.Scale(2, 2, 2, 2); // 2: sx, sy, px, py
    SkMatrix skMatrix = skiaMatrix.ExportSkiaMatrix();
    bool isRecordDrawArea = false;
    isRecordDrawArea = skiaCanvasAutoCache.RecordDrawArea(bounds, skPaint, &skMatrix);
    ASSERT_TRUE(isRecordDrawArea);

    SkiaImage skiaImage;
    const SamplingOptions sampling;
    const SkSamplingOptions* samplingOptions = reinterpret_cast<const SkSamplingOptions*>(&sampling);
    SkRect dstBounds = SkRect::MakeEmpty();
    skiaCanvasAutoCache.onDrawImageRect2(skiaImage.GetImage().get(), bounds, dstBounds, *samplingOptions, &skPaint,
        static_cast<SkCanvas::SrcRectConstraint>(SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT));
}

/**
 * @tc.name: OpCanCache002
 * @tc.desc: Test SkiaCanvasAutoCache
 * @tc.type: FUNC
 * @tc.require: IAKGJ7
 */
HWTEST_F(SkiaCanvasAutoCacheTest, OpCanCache002, TestSize.Level1)
{
    SkCanvas canvas;
    SkiaCanvasAutoCache skiaCanvasAutoCache(&canvas);
    skiaCanvasAutoCache.opCanCache_ = true;
    skiaCanvasAutoCache.totalOpNums_ = 2;
    const SkRect boundsFirst = SkRect::MakeLTRB(100, 100, 400, 300); // rect.Left, rect.Top, rect.Right, rect.Bottom
    skiaCanvasAutoCache.drawAreaRects_.emplace_back(boundsFirst);
    const SkRect boundsSecond = SkRect::MakeLTRB(150, 150, 450, 350); // rect.Left, rect.Top, rect.Right, rect.Bottom
    skiaCanvasAutoCache.drawAreaRects_.emplace_back(boundsSecond);
    ASSERT_TRUE(skiaCanvasAutoCache.drawAreaRects_.size() != 0);

    skiaCanvasAutoCache.MergeDrawAreaRects();
    bool isOpCanCache = false;
    isOpCanCache = skiaCanvasAutoCache.OpCanCache(boundsFirst);
    ASSERT_TRUE(!isOpCanCache);
}

/**
 * @tc.name: onDrawVerticesObject001
 * @tc.desc: Test SkiaCanvasAutoCache
 * @tc.type: FUNC
 * @tc.require: IAKGJ7
 */
HWTEST_F(SkiaCanvasAutoCacheTest, onDrawVerticesObject001, TestSize.Level1)
{
    SkCanvas canvas;
    SkiaCanvasAutoCache skiaCanvasAutoCache(&canvas);
    SkiaVertices vertices;
    Paint paint;
    paint.SetAntiAlias(true);
    SkPaint skPaint;
    SkiaPaint::PaintToSkPaint(paint, skPaint);
    skiaCanvasAutoCache.onDrawVerticesObject(vertices.GetVertices().get(),
        static_cast<SkBlendMode>(BlendMode::CLEAR), skPaint);

    const SkRect skBounds = SkRect::MakeLTRB(100, 100, 400, 300); // rect.Left, rect.Top, rect.Right, rect.Bottom
    SkCanvas::SaveLayerRec slr(&skBounds, &skPaint);
    SkCanvas::SaveLayerStrategy saveLayerStrategy = SkCanvas::SaveLayerStrategy::kFullLayer_SaveLayerStrategy;
    saveLayerStrategy = skiaCanvasAutoCache.getSaveLayerStrategy(slr);
    ASSERT_TRUE(saveLayerStrategy == SkCanvas::SaveLayerStrategy::kNoLayer_SaveLayerStrategy);
    bool isOnDoSaveBehind = true;
    isOnDoSaveBehind = skiaCanvasAutoCache.onDoSaveBehind(&skBounds);
    ASSERT_TRUE(!isOnDoSaveBehind);
}

/**
 * @tc.name: onDrawPaint001
 * @tc.desc: Test SkiaCanvasAutoCache
 * @tc.type: FUNC
 * @tc.require: IAKGJ7
 */
HWTEST_F(SkiaCanvasAutoCacheTest, onDrawPaint001, TestSize.Level1)
{
    SkCanvas canvas;
    SkiaCanvasAutoCache skiaCanvasAutoCache(&canvas);
    Paint paint;
    paint.SetAntiAlias(true);
    SkPaint skPaint;
    SkiaPaint::PaintToSkPaint(paint, skPaint);
    skiaCanvasAutoCache.onDrawPaint(skPaint);
    skiaCanvasAutoCache.onDrawBehind(skPaint);

    SkPoint point1;
    point1.Make(100.f, 100.f); // 100.f, 100.f means coordinates of points
    SkPoint point2;
    point2.Make(200.f, 200.f); // 200.f, 200.f means coordinates of points
    SkPoint pts[] = {point1, point2};
    skiaCanvasAutoCache.onDrawPoints(static_cast<SkCanvas::PointMode>(PointMode::POINTS_POINTMODE),
        2, pts, skPaint); //2 is pts count

    const SkRect skBounds = SkRect::MakeLTRB(100, 100, 400, 300); // rect.Left, rect.Top, rect.Right, rect.Bottom
    skiaCanvasAutoCache.onDrawRect(skBounds, skPaint);

    SkRRect skRRect;
    skRRect = SkRRect::MakeRect(skBounds);
    skiaCanvasAutoCache.onDrawRRect(skRRect, skPaint);

    SkRRect skRRectOuter;
    skiaCanvasAutoCache.onDrawDRRect(skRRectOuter, skRRect, skPaint);

    SkIRect skIRect;
    skIRect = SkIRect::MakeWH(200, 300); // 200, 300 means width and height of constructed SkIRect
    SkRegion skRegion;
    bool isRegionSetRect = false;
    isRegionSetRect = skRegion.setRect(skIRect);
    ASSERT_TRUE(isRegionSetRect);
    skiaCanvasAutoCache.onDrawRegion(skRegion, skPaint);
}

/**
 * @tc.name: onDrawOval001
 * @tc.desc: Test SkiaCanvasAutoCache
 * @tc.type: FUNC
 * @tc.require: IAKGJ7
 */
HWTEST_F(SkiaCanvasAutoCacheTest, onDrawOval001, TestSize.Level1)
{
    SkCanvas canvas;
    SkiaCanvasAutoCache skiaCanvasAutoCache(&canvas);
    Paint paint;
    paint.SetAntiAlias(true);
    SkPaint skPaint;
    SkiaPaint::PaintToSkPaint(paint, skPaint);

    const SkRect skRect = SkRect::MakeLTRB(100, 100, 400, 300); // rect.Left, rect.Top, rect.Right, rect.Bottom
    skiaCanvasAutoCache.onDrawOval(skRect, skPaint);
    ASSERT_TRUE(skiaCanvasAutoCache.OpShouldRecord());
    skiaCanvasAutoCache.onDrawArc(skRect, 90.f, 360.f, false, skPaint); // 90.f 360.f means startAngle and sweepAngle
    ASSERT_TRUE(skiaCanvasAutoCache.OpShouldRecord());
    SkiaPath skiaPath;
    skiaPath.AddRoundRect(0, 0,
        100, 100, 100, 100, PathDirection::CW_DIRECTION); // 100: right, bottom, xRadius, yRadius
    skiaCanvasAutoCache.onDrawPath(skiaPath.GetPath(), skPaint);
}

/**
 * @tc.name: onDrawImage2001
 * @tc.desc: Test SkiaCanvasAutoCache
 * @tc.type: FUNC
 * @tc.require: IAKGJ7
 */
HWTEST_F(SkiaCanvasAutoCacheTest, onDrawImage2001, TestSize.Level1)
{
    SkCanvas canvas;
    SkiaCanvasAutoCache skiaCanvasAutoCache(&canvas);
    Paint paint;
    paint.SetAntiAlias(true);
    SkPaint skPaint;
    SkiaPaint::PaintToSkPaint(paint, skPaint);
#ifdef USE_M133_SKIA
    auto info = SkImageInfo::Make(300, 500, SkiaImageInfo::ConvertToSkColorType(ColorType::COLORTYPE_RGBA_8888),
        SkiaImageInfo::ConvertToSkAlphaType(AlphaType::ALPHATYPE_PREMUL));
    SkBitmap bitmap;
    bool isBuildSuccess = bitmap.setInfo(info) && bitmap.tryAllocPixels();
    ASSERT_TRUE(isBuildSuccess);
    sk_sp<SkImage> image = SkImages::RasterFromBitmap(bitmap);
#else
    SkImageInfo info;
    info.makeWH(300, 500); // 300, 500 means newWidth and newHeight
    SkImage image(info, 1); // 1 means uniqueID
#endif
    const SamplingOptions sampling;
    const SkSamplingOptions* samplingOptions = reinterpret_cast<const SkSamplingOptions*>(&sampling);
    ASSERT_TRUE(samplingOptions != nullptr);
#ifdef USE_M133_SKIA
    skiaCanvasAutoCache.onDrawImage2(image.get(), 100.f, 200.f, *samplingOptions,
        &skPaint); // 100.f, 200.f means left and top
#else
    skiaCanvasAutoCache.onDrawImage2(&image, 100.f, 200.f, *samplingOptions,
        &skPaint); // 100.f, 200.f means left and top
#endif

    const SkRect skRect = SkRect::MakeLTRB(100, 100, 400, 300); // rect.Left, rect.Top, rect.Right, rect.Bottom
    SkCanvas::Lattice lattice;
#ifdef USE_M133_SKIA
    skiaCanvasAutoCache.onDrawImageLattice2(image.get(), lattice, skRect,
        static_cast<SkFilterMode>(FilterMode::LINEAR), &skPaint);
#else
    skiaCanvasAutoCache.onDrawImageLattice2(&image, lattice, skRect,
        static_cast<SkFilterMode>(FilterMode::LINEAR), &skPaint);
#endif
}

/**
 * @tc.name: onDrawTextBlob001
 * @tc.desc: Test SkiaCanvasAutoCache
 * @tc.type: FUNC
 * @tc.require: IAKGJ7
 */
HWTEST_F(SkiaCanvasAutoCacheTest, onDrawTextBlob001, TestSize.Level1)
{
    SkCanvas canvas;
    SkiaCanvasAutoCache skiaCanvasAutoCache(&canvas);
    Paint paint;
    paint.SetAntiAlias(true);
    SkPaint skPaint;
    SkiaPaint::PaintToSkPaint(paint, skPaint);
    const char* str = "asdf";
    Font font;
    font.SetSize(100);
    auto textBlob = SkiaTextBlob::MakeFromText(str, strlen(str), font, TextEncoding::UTF8);
    ASSERT_TRUE(textBlob != nullptr);
    auto skiaBlobImpl = textBlob->GetImpl<SkiaTextBlob>();
    ASSERT_TRUE(skiaBlobImpl != nullptr);

    skiaCanvasAutoCache.onDrawTextBlob(skiaBlobImpl->GetTextBlob().get(), 100.f, 100.f, skPaint); // 100 means location
}

/**
 * @tc.name: onDrawAtlas2001
 * @tc.desc: Test SkiaCanvasAutoCache
 * @tc.type: FUNC
 * @tc.require: IAKGJ7
 */
HWTEST_F(SkiaCanvasAutoCacheTest, onDrawAtlas2001, TestSize.Level1)
{
    SkCanvas canvas;
    SkiaCanvasAutoCache skiaCanvasAutoCache(&canvas);
    Paint paint;
    paint.SetAntiAlias(true);
    SkPaint skPaint;
    SkiaPaint::PaintToSkPaint(paint, skPaint);
    std::shared_ptr<SkiaImage> skiaImage = std::make_shared<SkiaImage>();
    RSXform xform = RSXform::Make(10, 10, 10, 10);
    SkRSXform& rSXform = reinterpret_cast<SkRSXform&>(xform);
    SkRSXform rSXformArray[2] = {rSXform, rSXform}; // 2 means array size
    Rect rect(100, 100, 400, 300); // 100, 100, 400, 300 means rect.Left, rect.Top, rect.Right, rect.Bottom
    SkRect* skBounds = reinterpret_cast<SkRect*>(&rect);
    ASSERT_TRUE(skBounds != nullptr);
    SkRect skBoundsArray[2] = {*skBounds, *skBounds}; // 2 means array size
    Color color(123, 33, 50, 1);
    SkColor* skColor = reinterpret_cast<SkColor*>(&color);
    SkColor skColorArray[2] = {*skColor, *skColor}; // 2 means array size
    const SamplingOptions sampling;
    const SkSamplingOptions* samplingOptions = reinterpret_cast<const SkSamplingOptions*>(&sampling);
    ASSERT_TRUE(samplingOptions != nullptr);
    const SkRect cull = SkRect::MakeLTRB(100, 100, 400, 300); // rect.Left, rect.Top, rect.Right, rect.Bottom

    skiaCanvasAutoCache.onDrawAtlas2(skiaImage->GetImage().get(), rSXformArray, skBoundsArray, skColorArray,
        2, static_cast<SkBlendMode>(BlendMode::CLEAR), *samplingOptions, &cull, &skPaint);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS