/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "draw/surface.h"
#include "message_parcel.h"
#include "render/rs_image.h"
#include "render/rs_image_cache.h"
#include "pixel_map.h"
#include "transaction/rs_marshalling_helper.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSImageTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSImageTest::SetUpTestCase() {}
void RSImageTest::TearDownTestCase() {}
void RSImageTest::SetUp() {}
void RSImageTest::TearDown() {}

/**
 * @tc.name: IsEqual001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSImageTest, IsEqual001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSMask by Gradient
     */
    RSImage rsImage;
    int res = rsImage.IsEqual(rsImage);
    ASSERT_TRUE(res);
    RSImage other;
    other.SetScale(2.f);
    res = rsImage.IsEqual(other);
    ASSERT_FALSE(res);
}

static std::shared_ptr<Media::PixelMap> CreatePixelMap(int width, int height)
{
    Media::InitializationOptions opts;
    opts.size.width = width;
    opts.size.height = height;
    auto pixelmap = Media::PixelMap::Create(opts);
    auto address = const_cast<uint32_t*>(pixelmap->GetPixel32(0, 0));
    if (address == nullptr) {
        return nullptr;
    }
    Drawing::ImageInfo info =
        Drawing::ImageInfo(pixelmap->GetWidth(), pixelmap->GetHeight(),
        Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL);
    auto surface = Drawing::Surface::MakeRasterDirect(info, address, pixelmap->GetRowBytes());
    auto canvas = surface->GetCanvas();
    canvas->Clear(Drawing::Color::COLOR_YELLOW);
    Drawing::Brush brush;
    brush.SetColor(Drawing::Color::COLOR_RED);
    canvas->AttachBrush(brush);
    canvas->DrawRect(Drawing::Rect(width / 4, height / 4, width / 2, height / 2));
    canvas->DetachBrush();
    return pixelmap;
}

/**
 * @tc.name: LifeCycle001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSImageTest, LifeCycle001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSMask by Gradient
     */
    RSImage rsImage;
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    float fLeft = 1.0f;
    float ftop = 1.0f;
    float fRight = 1.0f;
    float fBottom = 1.0f;
    rsImage.SetImageFit(0);
    Drawing::Rect rect { fLeft, ftop, fRight, fBottom };
    Drawing::Brush brush;
    std::shared_ptr<Media::PixelMap> pixelmap;
    rsImage.SetPixelMap(pixelmap);
    int width = 200;
    int height = 300;
    pixelmap = CreatePixelMap(width, height);
    rsImage.SetPixelMap(pixelmap);
    canvas.AttachBrush(brush);
    rsImage.CanvasDrawImage(canvas, rect, Drawing::SamplingOptions(), false);
    rsImage.CanvasDrawImage(canvas, rect, Drawing::SamplingOptions(), true);
    rsImage.SetImageFit(1);
    rsImage.CanvasDrawImage(canvas, rect, Drawing::SamplingOptions(), true);
    rsImage.SetImageFit(2);
    rsImage.CanvasDrawImage(canvas, rect, Drawing::SamplingOptions(), true);
    rsImage.SetImageFit(3);
    rsImage.CanvasDrawImage(canvas, rect, Drawing::SamplingOptions(), true);
    rsImage.SetImageFit(4);
    rsImage.CanvasDrawImage(canvas, rect, Drawing::SamplingOptions(), true);
    rsImage.SetImageFit(5);
    rsImage.CanvasDrawImage(canvas, rect, Drawing::SamplingOptions(), true);
    rsImage.SetImageFit(6);
    rsImage.CanvasDrawImage(canvas, rect, Drawing::SamplingOptions(), true);
    rsImage.SetImageFit(0);
    rsImage.CanvasDrawImage(canvas, rect, Drawing::SamplingOptions(), true);
    rsImage.SetImageFit(8);
    rsImage.CanvasDrawImage(canvas, rect, Drawing::SamplingOptions(), true);
    canvas.DetachBrush();
}

/**
 * @tc.name: LifeCycle002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSImageTest, LifeCycle002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSMask by Gradient
     */
    Parcel parcel;
    ASSERT_TRUE(RSImage::Unmarshalling(parcel) == nullptr);
    parcel.WriteInt32(1);
    ASSERT_TRUE(RSImage::Unmarshalling(parcel) == nullptr);
}

/**
 * @tc.name: TestRSImage001
 * @tc.desc: IsEqual test.
 * @tc.type: FUNC
 */
HWTEST_F(RSImageTest, TestRSImage001, TestSize.Level1)
{
    RSImage image;
    RSImage other;
    image.IsEqual(other);
}

/**
 * @tc.name: TestRSImage002
 * @tc.desc: CanvasDrawImage test.
 * @tc.type: FUNC
 */
HWTEST_F(RSImageTest, TestRSImage002, TestSize.Level1)
{
    RSImage image;
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    Drawing::Rect rect;
    Drawing::Brush brush;
    bool isBackground = false;
    canvas.AttachBrush(brush);
    image.CanvasDrawImage(canvas, rect, Drawing::SamplingOptions(), isBackground);
    isBackground = true;
    image.CanvasDrawImage(canvas, rect, Drawing::SamplingOptions(), isBackground);
    canvas.DetachBrush();
}

/**
 * @tc.name: TestRSImage003
 * @tc.desc: ApplyImageFit test.
 * @tc.type: FUNC
 */
HWTEST_F(RSImageTest, TestRSImage003, TestSize.Level1)
{
    RSImage image;
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    Drawing::Rect rect;
    Drawing::Brush brush;
    bool isBackground = false;
    image.SetImageFit(0);
    canvas.AttachBrush(brush);
    image.CanvasDrawImage(canvas, rect, Drawing::SamplingOptions(), isBackground);
    image.SetImageFit(5);
    image.CanvasDrawImage(canvas, rect, Drawing::SamplingOptions(), isBackground);
    image.SetImageFit(2);
    image.CanvasDrawImage(canvas, rect, Drawing::SamplingOptions(), isBackground);
    image.SetImageFit(3);
    image.CanvasDrawImage(canvas, rect, Drawing::SamplingOptions(), isBackground);
    image.SetImageFit(4);
    image.CanvasDrawImage(canvas, rect, Drawing::SamplingOptions(), isBackground);
    image.SetImageFit(6);
    image.CanvasDrawImage(canvas, rect, Drawing::SamplingOptions(), isBackground);
    image.SetImageFit(1);
    image.CanvasDrawImage(canvas, rect, Drawing::SamplingOptions(), isBackground);
    image.SetImageFit(7);
    image.CanvasDrawImage(canvas, rect, Drawing::SamplingOptions(), isBackground);
    image.SetImageFit(8);
    image.CanvasDrawImage(canvas, rect, Drawing::SamplingOptions(), isBackground);
    canvas.DetachBrush();
}

/**
 * @tc.name: TestRSImage004
 * @tc.desc: SetImageRepeat test.
 * @tc.type: FUNC
 */
HWTEST_F(RSImageTest, TestRSImage004, TestSize.Level1)
{
    RSImage image;
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    Drawing::Rect rect = Drawing::Rect(0, 0, 100, 100);
    RectF dstRect(10, 10, 80, 80);
    Drawing::Brush brush;
    image.SetDstRect(dstRect);
    image.SetImageRepeat(1);
    canvas.AttachBrush(brush);
    image.CanvasDrawImage(canvas, rect, Drawing::SamplingOptions());
    image.SetImageRepeat(2);
    image.CanvasDrawImage(canvas, rect, Drawing::SamplingOptions());
    image.SetImageRepeat(3);
    image.CanvasDrawImage(canvas, rect, Drawing::SamplingOptions());
    canvas.DetachBrush();
}

/**
 * @tc.name: RSImageBase001
 * @tc.desc: RSImageBase test.
 * @tc.type: FUNC
 */
HWTEST_F(RSImageTest, RSImageBase001, TestSize.Level1)
{
    RSImageBase imageBase;
    RectF rect(0, 0, 100, 100);
    imageBase.SetSrcRect(rect);
    imageBase.SetDstRect(rect);
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    Drawing::Brush brush;
    canvas.AttachBrush(brush);
    imageBase.DrawImage(canvas, Drawing::SamplingOptions());
    canvas.DetachBrush();
}

/**
* @tc.name: RSImageCache001
* @tc.desc: RSImageBase test.
* @tc.type: FUNC
*/
HWTEST_F(RSImageTest, RSImageCache001, TestSize.Level1)
{
    auto rsImage = std::make_shared<RSImage>();
    std::shared_ptr<Media::PixelMap> pixelMap;
    int width = 200;
    int height = 300;
    pixelMap = CreatePixelMap(width, height);
    rsImage->SetPixelMap(pixelMap);

    MessageParcel parcel;
    EXPECT_EQ(RSMarshallingHelper::Marshalling(parcel, rsImage), true);
    std::shared_ptr<RSImage> newImage;
    EXPECT_EQ(RSMarshallingHelper::Unmarshalling(parcel, newImage), true);

    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    Drawing::Brush brush;
    Drawing::Rect rect = Drawing::Rect(0, 0, 100, 100);
    canvas.AttachBrush(brush);
    newImage->CanvasDrawImage(canvas, rect, Drawing::SamplingOptions());
    newImage->CanvasDrawImage(canvas, rect, Drawing::SamplingOptions());
    canvas.DetachBrush();

    MessageParcel parcel2;
    EXPECT_EQ(RSMarshallingHelper::Marshalling(parcel2, rsImage), true);
    std::shared_ptr<RSImage> newImage2;
    EXPECT_EQ(RSMarshallingHelper::Unmarshalling(parcel2, newImage2), true);

    RSImageCache::Instance().CachePixelMap(1, nullptr);
    RSImageCache::Instance().CachePixelMap(0, pixelMap);
    RSImageCache::Instance().CacheRenderDrawingImageByPixelMapId(1, nullptr);
    RSImageCache::Instance().CacheRenderDrawingImageByPixelMapId(0, nullptr);
}
} // namespace OHOS::Rosen
