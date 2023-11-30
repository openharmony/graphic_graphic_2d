/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef USE_ROSEN_DRAWING
#include <gtest/gtest.h>
#include <securec.h>

#include "include/core/SkPictureRecorder.h"

#include "pipeline/rs_draw_cmd.h"
#include "pipeline/rs_paint_filter_canvas.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSDrawCmdTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

private:
    static sk_sp<SkImage> CreateSkImage();
    static sk_sp<SkPicture> CreateSkPicture();
};

void RSDrawCmdTest::SetUpTestCase() {}
void RSDrawCmdTest::TearDownTestCase() {}
void RSDrawCmdTest::SetUp() {}
void RSDrawCmdTest::TearDown() {}

sk_sp<SkImage> RSDrawCmdTest::CreateSkImage()
{
    const SkImageInfo info = SkImageInfo::MakeN32(200, 200, kOpaque_SkAlphaType);
    auto surface(SkSurface::MakeRaster(info));
    auto canvas = surface->getCanvas();
    canvas->clear(SK_ColorYELLOW);
    SkPaint paint;
    paint.setColor(SK_ColorRED);
    int x = 50;
    int y = 50;
    int width = 100;
    int height = 100;
    canvas->drawRect(SkRect::MakeXYWH(x, y, width, height), paint);
    return surface->makeImageSnapshot();
}

sk_sp<SkPicture> RSDrawCmdTest::CreateSkPicture()
{
    SkPictureRecorder recorder;
    SkCanvas* canvas = recorder.beginRecording(200, 200);
    auto image = CreateSkImage();
    canvas->drawImage(image, 0, 0);
    sk_sp<SkPicture> picture(recorder.finishRecordingAsPicture());
    return picture;
}

/**
 * @tc.name: RectOpItem001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawCmdTest, RectOpItem001, TestSize.Level1)
{
    SkRect rect;
    SkPaint paint;
    auto op = std::make_unique<RectOpItem>(rect, paint);
    Parcel parcel;
    char* buffer = static_cast<char*>(malloc(parcel.GetMaxCapacity()));
    auto bufferSize = parcel.GetMaxCapacity();
    memset_s(buffer, parcel.GetMaxCapacity(), 0, parcel.GetMaxCapacity());
    ASSERT_TRUE(parcel.WriteUnpadBuffer(buffer, parcel.GetMaxCapacity()));
    bool ret = false;
    parcel.SkipBytes(bufferSize);
    while (!ret) {
        size_t position = parcel.GetWritePosition();
        ret = op->Marshalling(parcel) && (RectOpItem::Unmarshalling(parcel) != nullptr);
        parcel.SetMaxCapacity(parcel.GetMaxCapacity() + 1);
        if (!ret) {
            parcel.RewindWrite(position);
            parcel.RewindRead(bufferSize);
        }
    }
    free(buffer);
    ASSERT_TRUE(ret);
}

/**
 * @tc.name: RoundRectOpItem001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawCmdTest, RoundRectOpItem001, TestSize.Level1)
{
    SkRRect rrect;
    SkPaint paint;
    auto op = std::make_unique<RoundRectOpItem>(rrect, paint);
    Parcel parcel;
    char* buffer = static_cast<char*>(malloc(parcel.GetMaxCapacity()));
    auto bufferSize = parcel.GetMaxCapacity();
    memset_s(buffer, parcel.GetMaxCapacity(), 0, parcel.GetMaxCapacity());
    ASSERT_TRUE(parcel.WriteUnpadBuffer(buffer, parcel.GetMaxCapacity()));
    bool ret = false;
    parcel.SkipBytes(bufferSize);
    while (!ret) {
        size_t position = parcel.GetWritePosition();
        ret = op->Marshalling(parcel) && (RoundRectOpItem::Unmarshalling(parcel) != nullptr);
        parcel.SetMaxCapacity(parcel.GetMaxCapacity() + 1);
        if (!ret) {
            parcel.RewindWrite(position);
            parcel.RewindRead(bufferSize);
        }
    }
    free(buffer);
    ASSERT_TRUE(ret);
}

/**
 * @tc.name: ImageWithParmOpItem001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawCmdTest, ImageWithParmOpItem001, TestSize.Level1)
{
    std::shared_ptr<RSImage> rsImage = std::make_shared<RSImage>();
    SkPaint paint;
    SkSamplingOptions samplingOptions;
    auto op = std::make_unique<ImageWithParmOpItem>(rsImage, samplingOptions, paint);
    ASSERT_EQ(paint, op->paint_);
    SkCanvas skCanvas;
    RSPaintFilterCanvas canvas(&skCanvas);
    op->Draw(canvas, nullptr);
    Parcel parcel;
    char* buffer = static_cast<char*>(malloc(parcel.GetMaxCapacity()));
    auto bufferSize = parcel.GetMaxCapacity();
    memset_s(buffer, parcel.GetMaxCapacity(), 0, parcel.GetMaxCapacity());
    ASSERT_TRUE(parcel.WriteUnpadBuffer(buffer, parcel.GetMaxCapacity()));
    bool ret = false;
    parcel.SkipBytes(bufferSize);
    while (!ret) {
        size_t position = parcel.GetWritePosition();
        ret = op->Marshalling(parcel) && (ImageWithParmOpItem::Unmarshalling(parcel) != nullptr);
        parcel.SetMaxCapacity(parcel.GetMaxCapacity() + 1);
        if (!ret) {
            parcel.RewindWrite(position);
            parcel.RewindRead(bufferSize);
        }
    }
    free(buffer);
    ASSERT_TRUE(ret);
}

/**
 * @tc.name: DRRectOpItem001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawCmdTest, DRRectOpItem001, TestSize.Level1)
{
    SkRRect outer;
    SkRRect inner;
    SkPaint paint;
    auto op = std::make_unique<DRRectOpItem>(outer, inner, paint);
    Parcel parcel;
    char* buffer = static_cast<char*>(malloc(parcel.GetMaxCapacity()));
    auto bufferSize = parcel.GetMaxCapacity();
    memset_s(buffer, parcel.GetMaxCapacity(), 0, parcel.GetMaxCapacity());
    ASSERT_TRUE(parcel.WriteUnpadBuffer(buffer, parcel.GetMaxCapacity()));
    bool ret = false;
    parcel.SkipBytes(bufferSize);
    while (!ret) {
        size_t position = parcel.GetWritePosition();
        ret = op->Marshalling(parcel) && (DRRectOpItem::Unmarshalling(parcel) != nullptr);
        parcel.SetMaxCapacity(parcel.GetMaxCapacity() + 1);
        if (!ret) {
            parcel.RewindWrite(position);
            parcel.RewindRead(bufferSize);
        }
    }
    free(buffer);
    ASSERT_TRUE(ret);
}

/**
 * @tc.name: BitmapOpItem001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawCmdTest, BitmapOpItem001, TestSize.Level1)
{
    sk_sp<SkImage> bitmapInfo = CreateSkImage();
    float left = 1.0f;
    float top = 1.0f;
    SkSamplingOptions samplingOptions;
    SkPaint paint;
    auto op = std::make_unique<BitmapOpItem>(bitmapInfo, left, top, samplingOptions, &paint);
    ASSERT_EQ(paint, op->paint_);
    NodeId id = 0;
    op->SetNodeId(id);
    SkCanvas skCanvas;
    RSPaintFilterCanvas canvas(&skCanvas);
    SkRect rect;
    op->Draw(canvas, &rect);
    Parcel parcel;
    char* buffer = static_cast<char*>(malloc(parcel.GetMaxCapacity()));
    auto bufferSize = parcel.GetMaxCapacity();
    memset_s(buffer, parcel.GetMaxCapacity(), 0, parcel.GetMaxCapacity());
    ASSERT_TRUE(parcel.WriteUnpadBuffer(buffer, parcel.GetMaxCapacity()));
    bool ret = false;
    parcel.SkipBytes(bufferSize);
    while (!ret) {
        size_t position = parcel.GetWritePosition();
        ret = op->Marshalling(parcel) && (BitmapOpItem::Unmarshalling(parcel) != nullptr);
        parcel.SetMaxCapacity(parcel.GetMaxCapacity() + 1);
        if (!ret) {
            parcel.RewindWrite(position);
            parcel.RewindRead(bufferSize);
        }
    }
    free(buffer);
    ASSERT_TRUE(ret);
}

/**
 * @tc.name: BitmapRectOpItem001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawCmdTest, BitmapRectOpItem001, TestSize.Level1)
{
    sk_sp<SkImage> bitmapInfo = CreateSkImage();
    SkRect rectSrc = SkRect::MakeLTRB(1, 1, 3, 3);
    SkRect rectDst = SkRect::MakeLTRB(16, 16, 48, 48);
    SkSamplingOptions samplingOptions;
    SkPaint paint;
    auto op = std::make_unique<BitmapRectOpItem>(
        bitmapInfo, &rectSrc, rectDst, samplingOptions, &paint, SkCanvas::kStrict_SrcRectConstraint);
    ASSERT_EQ(paint, op->paint_);
    Parcel parcel;
    char* buffer = static_cast<char*>(malloc(parcel.GetMaxCapacity()));
    auto bufferSize = parcel.GetMaxCapacity();
    memset_s(buffer, parcel.GetMaxCapacity(), 0, parcel.GetMaxCapacity());
    ASSERT_TRUE(parcel.WriteUnpadBuffer(buffer, parcel.GetMaxCapacity()));
    bool ret = false;
    parcel.SkipBytes(bufferSize);
    while (!ret) {
        size_t position = parcel.GetWritePosition();
        ret = op->Marshalling(parcel) && (BitmapRectOpItem::Unmarshalling(parcel) != nullptr);
        parcel.SetMaxCapacity(parcel.GetMaxCapacity() + 1);
        if (!ret) {
            parcel.RewindWrite(position);
            parcel.RewindRead(bufferSize);
        }
    }
    free(buffer);
    ASSERT_TRUE(ret);
}

/**
 * @tc.name: PixelMapOpItem001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawCmdTest, PixelMapOpItem001, TestSize.Level1)
{
    Media::InitializationOptions opts;
    opts.size.width = 512;
    opts.size.height = 512;
    opts.editable = true;
    auto pixelMap = Media::PixelMap::Create(opts);
    auto shpPixelMap = std::shared_ptr<Media::PixelMap>(pixelMap.release());
    float left = 1.0f;
    float top = 1.0f;
    SkSamplingOptions samplingOptions;
    SkPaint paint;
    auto op = std::make_unique<PixelMapOpItem>(shpPixelMap, left, top, samplingOptions, &paint);
    ASSERT_EQ(paint, op->paint_);
    Parcel parcel;
    char* buffer = static_cast<char*>(malloc(parcel.GetMaxCapacity()));
    auto bufferSize = parcel.GetMaxCapacity();
    memset_s(buffer, parcel.GetMaxCapacity(), 0, parcel.GetMaxCapacity());
    ASSERT_TRUE(parcel.WriteUnpadBuffer(buffer, parcel.GetMaxCapacity()));
    bool ret = false;
    parcel.SkipBytes(bufferSize);
    while (!ret) {
        size_t position = parcel.GetWritePosition();
        ret = op->Marshalling(parcel) && (PixelMapOpItem::Unmarshalling(parcel) != nullptr);
        parcel.SetMaxCapacity(parcel.GetMaxCapacity() + 1);
        if (!ret) {
            parcel.RewindWrite(position);
            parcel.RewindRead(bufferSize);
        }
    }
    free(buffer);
    ASSERT_TRUE(ret);
}

/**
 * @tc.name: PixelMapRectOpItem001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawCmdTest, PixelMapRectOpItem001, TestSize.Level1)
{
    Media::InitializationOptions opts;
    opts.size.width = 512;
    opts.size.height = 512;
    opts.editable = true;
    auto pixelMap = Media::PixelMap::Create(opts);
    auto shpPixelMap = std::shared_ptr<Media::PixelMap>(pixelMap.release());
    SkRect src = SkRect::MakeLTRB(0, 0, 128, 128);
    SkRect dst = SkRect::MakeLTRB(0, 0, 512, 512);
    SkSamplingOptions samplingOptions;
    SkPaint paint;
    SkCanvas::SrcRectConstraint constraint = SkCanvas::kStrict_SrcRectConstraint;
    auto op = std::make_unique<PixelMapRectOpItem>(shpPixelMap, src, dst, samplingOptions, &paint, constraint);
    ASSERT_EQ(paint, op->paint_);
    Parcel parcel;
    char* buffer = static_cast<char*>(malloc(parcel.GetMaxCapacity()));
    auto bufferSize = parcel.GetMaxCapacity();
    memset_s(buffer, parcel.GetMaxCapacity(), 0, parcel.GetMaxCapacity());
    ASSERT_TRUE(parcel.WriteUnpadBuffer(buffer, parcel.GetMaxCapacity()));
    bool ret = false;
    parcel.SkipBytes(bufferSize);
    while (!ret) {
        size_t position = parcel.GetWritePosition();
        ret = op->Marshalling(parcel) && (PixelMapRectOpItem::Unmarshalling(parcel) != nullptr);
        parcel.SetMaxCapacity(parcel.GetMaxCapacity() + 1);
        if (!ret) {
            parcel.RewindWrite(position);
            parcel.RewindRead(bufferSize);
        }
    }
    free(buffer);
    ASSERT_TRUE(ret);
}

/**
 * @tc.name: BitmapNineOpItem00
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawCmdTest, BitmapNineOpItem001, TestSize.Level1)
{
    sk_sp<SkImage> bitmapInfo = CreateSkImage();
    SkIRect center = SkIRect::MakeLTRB(0, 0, 128, 128);
    SkRect rectDst = SkRect::MakeLTRB(0, 0, 512, 512);
    SkFilterMode filter = SkFilterMode::kLinear;
    SkPaint paint;
    auto op = std::make_unique<BitmapNineOpItem>(bitmapInfo, center, rectDst, filter, &paint);
    ASSERT_EQ(paint, op->paint_);
    Parcel parcel;
    char* buffer = static_cast<char*>(malloc(parcel.GetMaxCapacity()));
    auto bufferSize = parcel.GetMaxCapacity();
    memset_s(buffer, parcel.GetMaxCapacity(), 0, parcel.GetMaxCapacity());
    ASSERT_TRUE(parcel.WriteUnpadBuffer(buffer, parcel.GetMaxCapacity()));
    bool ret = false;
    parcel.SkipBytes(bufferSize);
    while (!ret) {
        size_t position = parcel.GetWritePosition();
        ret = op->Marshalling(parcel) && (BitmapNineOpItem::Unmarshalling(parcel) != nullptr);
        parcel.SetMaxCapacity(parcel.GetMaxCapacity() + 1);
        if (!ret) {
            parcel.RewindWrite(position);
            parcel.RewindRead(bufferSize);
        }
    }
    free(buffer);
    ASSERT_TRUE(ret);
}

/**
 * @tc.name: AdaptiveRRectOpItem001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawCmdTest, AdaptiveRRectOpItem001, TestSize.Level1)
{
    float radius = 1.0f;
    SkPaint paint;
    auto op = std::make_unique<AdaptiveRRectOpItem>(radius, paint);
    SkCanvas skCanvas;
    RSPaintFilterCanvas canvas(&skCanvas);
    SkRect rect = SkRect::MakeLTRB(0, 0, 512, 512);
    op->Draw(canvas, &rect);
    op->Draw(canvas, nullptr);
    Parcel parcel;
    char* buffer = static_cast<char*>(malloc(parcel.GetMaxCapacity()));
    auto bufferSize = parcel.GetMaxCapacity();
    memset_s(buffer, parcel.GetMaxCapacity(), 0, parcel.GetMaxCapacity());
    ASSERT_TRUE(parcel.WriteUnpadBuffer(buffer, parcel.GetMaxCapacity()));
    bool ret = false;
    parcel.SkipBytes(bufferSize);
    while (!ret) {
        size_t position = parcel.GetWritePosition();
        ret = op->Marshalling(parcel) && (AdaptiveRRectOpItem::Unmarshalling(parcel) != nullptr);
        parcel.SetMaxCapacity(parcel.GetMaxCapacity() + 1);
        if (!ret) {
            parcel.RewindWrite(position);
            parcel.RewindRead(bufferSize);
        }
    }
    free(buffer);
    ASSERT_TRUE(ret);
}

/**
 * @tc.name: AdaptiveRRectScaleOpItem001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawCmdTest, AdaptiveRRectScaleOpItem001, TestSize.Level1)
{
    float radiusRatio = 1.0f;
    SkPaint paint;
    auto op = std::make_unique<AdaptiveRRectScaleOpItem>(radiusRatio, paint);
    SkCanvas skCanvas;
    RSPaintFilterCanvas canvas(&skCanvas);
    SkRect rect = SkRect::MakeLTRB(0, 0, 512, 512);
    op->Draw(canvas, &rect);
    op->Draw(canvas, nullptr);
    Parcel parcel;
    char* buffer = static_cast<char*>(malloc(parcel.GetMaxCapacity()));
    auto bufferSize = parcel.GetMaxCapacity();
    memset_s(buffer, parcel.GetMaxCapacity(), 0, parcel.GetMaxCapacity());
    ASSERT_TRUE(parcel.WriteUnpadBuffer(buffer, parcel.GetMaxCapacity()));
    bool ret = false;
    parcel.SkipBytes(bufferSize);
    while (!ret) {
        size_t position = parcel.GetWritePosition();
        ret = op->Marshalling(parcel) && (AdaptiveRRectScaleOpItem::Unmarshalling(parcel) != nullptr);
        parcel.SetMaxCapacity(parcel.GetMaxCapacity() + 1);
        if (!ret) {
            parcel.RewindWrite(position);
            parcel.RewindRead(bufferSize);
        }
    }
    free(buffer);
    ASSERT_TRUE(ret);
}

/**
 * @tc.name: SaveLayerOpItem001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawCmdTest, SaveLayerOpItem001, TestSize.Level1)
{
    SkCanvas::SaveLayerRec rec;
    auto op = std::make_unique<SaveLayerOpItem>(rec);
    Parcel parcel;
    char* buffer = static_cast<char*>(malloc(parcel.GetMaxCapacity()));
    auto bufferSize = parcel.GetMaxCapacity();
    memset_s(buffer, parcel.GetMaxCapacity(), 0, parcel.GetMaxCapacity());
    ASSERT_TRUE(parcel.WriteUnpadBuffer(buffer, parcel.GetMaxCapacity()));
    bool ret = false;
    parcel.SkipBytes(bufferSize);
    while (!ret) {
        size_t position = parcel.GetWritePosition();
        ret = op->Marshalling(parcel) && (SaveLayerOpItem::Unmarshalling(parcel) != nullptr);
        parcel.SetMaxCapacity(parcel.GetMaxCapacity() + 1);
        if (!ret) {
            parcel.RewindWrite(position);
            parcel.RewindRead(bufferSize);
        }
    }
    free(buffer);
    ASSERT_TRUE(ret);
}

/**
 * @tc.name: DrawableOpItem001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawCmdTest, DrawableOpItem001, TestSize.Level1)
{
    SkMatrix matrix = SkMatrix::I();
    const char data[] = { '0', '0' };
    sk_sp<SkDrawable> inSkDrawable = SkDrawable::Deserialize(data, sizeof(data));
    auto op = std::make_unique<DrawableOpItem>(inSkDrawable.get(), nullptr);
    op = std::make_unique<DrawableOpItem>(inSkDrawable.get(), &matrix);
    Parcel parcel;
    char* buffer = static_cast<char*>(malloc(parcel.GetMaxCapacity()));
    auto bufferSize = parcel.GetMaxCapacity();
    memset_s(buffer, parcel.GetMaxCapacity(), 0, parcel.GetMaxCapacity());
    ASSERT_TRUE(parcel.WriteUnpadBuffer(buffer, parcel.GetMaxCapacity()));
    bool ret = false;
    parcel.SkipBytes(bufferSize);
    while (!ret) {
        size_t position = parcel.GetWritePosition();
        ret = op->Marshalling(parcel) && (DrawableOpItem::Unmarshalling(parcel) != nullptr);
        parcel.SetMaxCapacity(parcel.GetMaxCapacity() + 1);
        if (!ret) {
            parcel.RewindWrite(position);
            parcel.RewindRead(bufferSize);
        }
    }
    free(buffer);
    ASSERT_TRUE(ret);
}

/**
 * @tc.name: PictureOpItem001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawCmdTest, PictureOpItem001, TestSize.Level1)
{
    sk_sp<SkPicture> picture = CreateSkPicture();
    SkMatrix matrix = SkMatrix::I();
    SkPaint paint;
    auto op = std::make_unique<PictureOpItem>(picture, nullptr, nullptr);
    op = std::make_unique<PictureOpItem>(picture, &matrix, &paint);
    Parcel parcel;
    char* buffer = static_cast<char*>(malloc(parcel.GetMaxCapacity()));
    auto bufferSize = parcel.GetMaxCapacity();
    memset_s(buffer, parcel.GetMaxCapacity(), 0, parcel.GetMaxCapacity());
    ASSERT_TRUE(parcel.WriteUnpadBuffer(buffer, parcel.GetMaxCapacity()));
    bool ret = false;
    parcel.SkipBytes(bufferSize);
    while (!ret) {
        size_t position = parcel.GetWritePosition();
        ret = op->Marshalling(parcel) && (PictureOpItem::Unmarshalling(parcel) != nullptr);
        parcel.SetMaxCapacity(parcel.GetMaxCapacity() + 1);
        if (!ret) {
            parcel.RewindWrite(position);
            parcel.RewindRead(bufferSize);
        }
    }
    free(buffer);
    ASSERT_TRUE(ret);
}

/**
 * @tc.name: PointsOpItem001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawCmdTest, PointsOpItem001, TestSize.Level1)
{
    SkCanvas::PointMode mode = SkCanvas::PointMode::kLines_PointMode;
    int count = 3;
    SkPoint processedPoints[] = { { 0, 0 }, { 1, 2 }, { 4, 3 } };
    SkPaint paint;
    auto op = std::make_unique<PointsOpItem>(mode, count, processedPoints, paint);
    Parcel parcel;
    char* buffer = static_cast<char*>(malloc(parcel.GetMaxCapacity()));
    auto bufferSize = parcel.GetMaxCapacity();
    memset_s(buffer, parcel.GetMaxCapacity(), 0, parcel.GetMaxCapacity());
    ASSERT_TRUE(parcel.WriteUnpadBuffer(buffer, parcel.GetMaxCapacity()));
    bool ret = false;
    parcel.SkipBytes(bufferSize);
    while (!ret) {
        size_t position = parcel.GetWritePosition();
        ret = op->Marshalling(parcel) && (PointsOpItem::Unmarshalling(parcel) != nullptr);
        parcel.SetMaxCapacity(parcel.GetMaxCapacity() + 1);
        if (!ret) {
            parcel.RewindWrite(position);
            parcel.RewindRead(bufferSize);
        }
    }
    free(buffer);
    ASSERT_TRUE(ret);
}

/**
 * @tc.name: ShadowRecOpItem001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawCmdTest, ShadowRecOpItem001, TestSize.Level1)
{
    SkPath path;
    SkDrawShadowRec rec;
    auto op = std::make_unique<ShadowRecOpItem>(path, rec);
    SkCanvas skCanvas;
    RSPaintFilterCanvas canvas(&skCanvas);
    SkRect rect = SkRect::MakeLTRB(0, 0, 512, 512);
    op->Draw(canvas, &rect);
    op->Draw(canvas, nullptr);
    Parcel parcel;
    char* buffer = static_cast<char*>(malloc(parcel.GetMaxCapacity()));
    auto bufferSize = parcel.GetMaxCapacity();
    memset_s(buffer, parcel.GetMaxCapacity(), 0, parcel.GetMaxCapacity());
    ASSERT_TRUE(parcel.WriteUnpadBuffer(buffer, parcel.GetMaxCapacity()));
    bool ret = false;
    parcel.SkipBytes(bufferSize);
    while (!ret) {
        size_t position = parcel.GetWritePosition();
        ret = op->Marshalling(parcel) && (ShadowRecOpItem::Unmarshalling(parcel) != nullptr);
        parcel.SetMaxCapacity(parcel.GetMaxCapacity() + 1);
        if (!ret) {
            parcel.RewindWrite(position);
            parcel.RewindRead(bufferSize);
        }
    }
    free(buffer);
    ASSERT_TRUE(ret);
}

/**
 * @tc.name: MultiplyAlphaOpItem001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDrawCmdTest, MultiplyAlphaOpItem001, TestSize.Level1)
{
    float alpha = 1.0f;
    auto op = std::make_unique<MultiplyAlphaOpItem>(alpha);
    Parcel parcel;
    char* buffer = static_cast<char*>(malloc(parcel.GetMaxCapacity()));
    auto bufferSize = parcel.GetMaxCapacity();
    memset_s(buffer, parcel.GetMaxCapacity(), 0, parcel.GetMaxCapacity());
    ASSERT_TRUE(parcel.WriteUnpadBuffer(buffer, parcel.GetMaxCapacity()));
    bool ret = false;
    parcel.SkipBytes(bufferSize);
    while (!ret) {
        size_t position = parcel.GetWritePosition();
        ret = op->Marshalling(parcel) && (MultiplyAlphaOpItem::Unmarshalling(parcel) != nullptr);
        parcel.SetMaxCapacity(parcel.GetMaxCapacity() + 1);
        if (!ret) {
            parcel.RewindWrite(position);
            parcel.RewindRead(bufferSize);
        }
    }
    free(buffer);
    ASSERT_TRUE(ret);
}
} // namespace Rosen
} // namespace OHOS
#endif
