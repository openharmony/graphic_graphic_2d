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
#include "include/core/SkSurface.h"
#include "include/render/rs_image.h"
#include "pixel_map.h"

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
    SkImageInfo info =
        SkImageInfo::Make(pixelmap->GetWidth(), pixelmap->GetHeight(), kRGBA_8888_SkColorType, kPremul_SkAlphaType);
    auto surface = SkSurface::MakeRasterDirect(info, address, pixelmap->GetRowBytes());
    auto canvas = surface->getCanvas();
    canvas->clear(SK_ColorYELLOW);
    SkPaint paint;
    paint.setColor(SK_ColorRED);
    canvas->drawRect(SkRect::MakeXYWH(width / 4, height / 4, width / 2, height / 2), paint);
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
    SkCanvas canvas;
    float fLeft = 1.0f;
    float ftop = 1.0f;
    float fRight = 1.0f;
    float fBottom = 1.0f;
    rsImage.SetImageFit(0);
    SkRect rect { fLeft, ftop, fRight, fBottom };
    SkPaint paint;
    std::shared_ptr<Media::PixelMap>pixelmap;
    rsImage.SetPixelMap(pixelmap);
    int width = 200;
    int height = 300;
    pixelmap = CreatePixelMap(width, height);
    rsImage.SetPixelMap(pixelmap);
    rsImage.CanvasDrawImage(canvas, rect, paint, false);
    rsImage.CanvasDrawImage(canvas, rect, paint, true);
    rsImage.SetImageFit(1);
    rsImage.CanvasDrawImage(canvas, rect, paint, true);
    rsImage.SetImageFit(2);
    rsImage.CanvasDrawImage(canvas, rect, paint, true);
    rsImage.SetImageFit(3);
    rsImage.CanvasDrawImage(canvas, rect, paint, true);
    rsImage.SetImageFit(4);
    rsImage.CanvasDrawImage(canvas, rect, paint, true);
    rsImage.SetImageFit(5);
    rsImage.CanvasDrawImage(canvas, rect, paint, true);
    rsImage.SetImageFit(6);
    rsImage.CanvasDrawImage(canvas, rect, paint, true);
    rsImage.SetImageFit(0);
    rsImage.CanvasDrawImage(canvas, rect, paint, true);
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
    SkCanvas canvas;
    SkRect rect;
    SkPaint paint;
    bool isBackground = false;
    image.CanvasDrawImage(canvas, rect, paint, isBackground);
    isBackground = true;
    image.CanvasDrawImage(canvas, rect, paint, isBackground);
}

/**
 * @tc.name: TestRSImage003
 * @tc.desc: ApplyImageFit test.
 * @tc.type: FUNC
 */
HWTEST_F(RSImageTest, TestRSImage003, TestSize.Level1)
{
    RSImage image;
    SkCanvas canvas;
    SkRect rect;
    SkPaint paint;
    bool isBackground = false;
    image.SetImageFit(0);
    image.CanvasDrawImage(canvas, rect, paint, isBackground);
    image.SetImageFit(5);
    image.CanvasDrawImage(canvas, rect, paint, isBackground);
    image.SetImageFit(2);
    image.CanvasDrawImage(canvas, rect, paint, isBackground);
    image.SetImageFit(3);
    image.CanvasDrawImage(canvas, rect, paint, isBackground);
    image.SetImageFit(4);
    image.CanvasDrawImage(canvas, rect, paint, isBackground);
    image.SetImageFit(6);
    image.CanvasDrawImage(canvas, rect, paint, isBackground);
    image.SetImageFit(1);
    image.CanvasDrawImage(canvas, rect, paint, isBackground);
}
} // namespace OHOS::Rosen
