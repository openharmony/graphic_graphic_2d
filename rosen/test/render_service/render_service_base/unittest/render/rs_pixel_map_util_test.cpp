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

#include "render/rs_pixel_map_util.h"
#include "render/rs_image.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class RSPixelMapUtilTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSPixelMapUtilTest::SetUpTestCase() {}
void RSPixelMapUtilTest::TearDownTestCase() {}
void RSPixelMapUtilTest::SetUp() {}
void RSPixelMapUtilTest::TearDown() {}

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
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bitmap.Build(pixelmap->GetWidth(), pixelmap->GetHeight(), format);
    auto surface = std::make_shared<Drawing::Surface>();
    surface->Bind(bitmap);
    auto canvas = surface->GetCanvas();
    canvas->Clear(Drawing::Color::COLOR_YELLOW);
    Drawing::Brush brush;
    brush.SetColor(Drawing::Color::COLOR_RED);
    int w = 50;
    int h = 50;
    int half = 25;
    canvas->AttachBrush(brush);
    canvas->DrawRect(Drawing::Rect(w, h, half, half));
    canvas->DetachBrush();
    return pixelmap;
}

/**
 * @tc.name: ExtractDrawingImage
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSPixelMapUtilTest, ExtractDrawingImage, TestSize.Level1)
{
    std::shared_ptr<Media::PixelMap> pixelMap;
    int width = 200;
    int height = 300;
    pixelMap = CreatePixelMap(width, height);
    pixelMap->imageInfo_.alphaType = OHOS::Media::AlphaType::IMAGE_ALPHA_TYPE_OPAQUE;
    pixelMap->imageInfo_.pixelFormat = Media::PixelFormat::RGB_565;
    EXPECT_NE(nullptr, RSPixelMapUtil::ExtractDrawingImage(pixelMap));
    pixelMap->imageInfo_.alphaType = OHOS::Media::AlphaType::IMAGE_ALPHA_TYPE_OPAQUE;
    pixelMap->imageInfo_.pixelFormat = Media::PixelFormat::RGBA_8888;
    EXPECT_NE(nullptr, RSPixelMapUtil::ExtractDrawingImage(pixelMap));
    pixelMap->imageInfo_.alphaType = OHOS::Media::AlphaType::IMAGE_ALPHA_TYPE_PREMUL;
    pixelMap->imageInfo_.pixelFormat = Media::PixelFormat::BGRA_8888;
    EXPECT_NE(nullptr, RSPixelMapUtil::ExtractDrawingImage(pixelMap));
    pixelMap->imageInfo_.alphaType = OHOS::Media::AlphaType::IMAGE_ALPHA_TYPE_UNPREMUL;
    pixelMap->imageInfo_.pixelFormat = Media::PixelFormat::ALPHA_8;
    EXPECT_NE(nullptr, RSPixelMapUtil::ExtractDrawingImage(pixelMap));
    pixelMap->imageInfo_.pixelFormat = Media::PixelFormat::RGBA_F16;
    EXPECT_EQ(nullptr, RSPixelMapUtil::ExtractDrawingImage(pixelMap));
    pixelMap->imageInfo_.pixelFormat = Media::PixelFormat::RGBA_1010102;
    EXPECT_NE(nullptr, RSPixelMapUtil::ExtractDrawingImage(pixelMap));
    pixelMap->imageInfo_.pixelFormat = Media::PixelFormat::UNKNOWN;
    EXPECT_EQ(nullptr, RSPixelMapUtil::ExtractDrawingImage(pixelMap));
    pixelMap->imageInfo_.pixelFormat = Media::PixelFormat::ARGB_8888;
    EXPECT_EQ(nullptr, RSPixelMapUtil::ExtractDrawingImage(pixelMap));
    pixelMap->imageInfo_.pixelFormat = Media::PixelFormat::RGB_888;
    EXPECT_EQ(nullptr, RSPixelMapUtil::ExtractDrawingImage(pixelMap));
    pixelMap->imageInfo_.pixelFormat = Media::PixelFormat::NV21;
    EXPECT_EQ(nullptr, RSPixelMapUtil::ExtractDrawingImage(pixelMap));
    pixelMap->imageInfo_.pixelFormat = Media::PixelFormat::NV12;
    EXPECT_EQ(nullptr, RSPixelMapUtil::ExtractDrawingImage(pixelMap));
    pixelMap->imageInfo_.pixelFormat = Media::PixelFormat::CMYK;
    EXPECT_EQ(nullptr, RSPixelMapUtil::ExtractDrawingImage(pixelMap));
}

/**
 * @tc.name: TransformDataSetForAstcnTest001
 * @tc.desc: Verify function TransformDataSetForAstc
 * @tc.type:FUNC
 */
HWTEST_F(RSPixelMapUtilTest, TransformDataSetForAstcnTest001, TestSize.Level1)
{
    std::shared_ptr<Media::PixelMap> pixelMap;
    int width = 200;
    int height = 300;
    pixelMap = CreatePixelMap(width, height);
    Drawing::Rect src;
    Drawing::Rect dst;
    Drawing::Canvas canvas;
    pixelMap->transformData_.flipX = true;
    pixelMap->transformData_.flipY = true;
    pixelMap->transformData_.cropLeft = 0;
    pixelMap->transformData_.cropTop = 0;
    pixelMap->transformData_.cropWidth = 1;
    pixelMap->transformData_.cropHeight = 1;
    pixelMap->astcrealSize_.width = 1;
    pixelMap->astcrealSize_.height = 1;
    pixelMap->transformData_.scaleX = 1;
    pixelMap->transformData_.scaleY = 1;
    RSPixelMapUtil::TransformDataSetForAstc(pixelMap, src, dst, canvas, ImageFit::NONE);
    RSPixelMapUtil::TransformDataSetForAstc(pixelMap, src, dst, canvas, ImageFit::CONTAIN);
    EXPECT_NE(pixelMap, nullptr);
}

/**
 * @tc.name: TransformDataSetForAstcnTest002
 * @tc.desc: Verify function TransformDataSetForAstc
 * @tc.type:FUNC
 */
HWTEST_F(RSPixelMapUtilTest, TransformDataSetForAstcnTest002, TestSize.Level1)
{
    std::shared_ptr<Media::PixelMap> pixelMap;
    int width = 200;
    int height = 300;
    pixelMap = CreatePixelMap(width, height);
    Drawing::Rect src;
    Drawing::Rect dst;
    Drawing::Canvas canvas;
    pixelMap->transformData_.flipX = false;
    pixelMap->transformData_.flipY = true;
    pixelMap->transformData_.cropLeft = 0;
    pixelMap->transformData_.cropTop = 0;
    pixelMap->transformData_.cropWidth = 1;
    pixelMap->transformData_.cropHeight = 1;
    pixelMap->astcrealSize_.width = 1;
    pixelMap->astcrealSize_.height = 1;
    pixelMap->transformData_.scaleX = 1;
    pixelMap->transformData_.scaleY = 1;
    RSPixelMapUtil::TransformDataSetForAstc(pixelMap, src, dst, canvas, ImageFit::NONE);
    EXPECT_NE(pixelMap, nullptr);
}

/**
 * @tc.name: TransformDataSetForAstcnTest003
 * @tc.desc: Verify function TransformDataSetForAstc
 * @tc.type:FUNC
 */
HWTEST_F(RSPixelMapUtilTest, TransformDataSetForAstcnTest003, TestSize.Level1)
{
    std::shared_ptr<Media::PixelMap> pixelMap;
    int width = 200;
    int height = 300;
    pixelMap = CreatePixelMap(width, height);
    Drawing::Rect src;
    Drawing::Rect dst;
    Drawing::Canvas canvas;
    pixelMap->transformData_.flipX = false;
    pixelMap->transformData_.flipY = false;
    pixelMap->transformData_.cropLeft = 0;
    pixelMap->transformData_.cropTop = 0;
    pixelMap->transformData_.cropWidth = 1;
    pixelMap->transformData_.cropHeight = 1;
    pixelMap->astcrealSize_.width = 1;
    pixelMap->astcrealSize_.height = 1;
    pixelMap->transformData_.scaleX = 1;
    pixelMap->transformData_.scaleY = 1;
    RSPixelMapUtil::TransformDataSetForAstc(pixelMap, src, dst, canvas, ImageFit::NONE);
    EXPECT_NE(pixelMap, nullptr);
}

/**
 * @tc.name: TransformDataSetForAstcnTest004
 * @tc.desc: Verify function TransformDataSetForAstc
 * @tc.type:FUNC
 */
HWTEST_F(RSPixelMapUtilTest, TransformDataSetForAstcnTest004, TestSize.Level1)
{
    std::shared_ptr<Media::PixelMap> pixelMap;
    int width = 200;
    int height = 300;
    pixelMap = CreatePixelMap(width, height);
    Drawing::Rect src;
    Drawing::Rect dst;
    Drawing::Canvas canvas;
    pixelMap->transformData_.flipX = true;
    pixelMap->transformData_.flipY = true;
    pixelMap->transformData_.cropLeft = -1;
    pixelMap->transformData_.cropTop = -1;
    pixelMap->transformData_.cropWidth = 1;
    pixelMap->transformData_.cropHeight = 1;
    pixelMap->astcrealSize_.width = 1;
    pixelMap->astcrealSize_.height = 1;
    pixelMap->transformData_.scaleX = 1;
    pixelMap->transformData_.scaleY = 1;
    RSPixelMapUtil::TransformDataSetForAstc(pixelMap, src, dst, canvas, ImageFit::NONE);
    EXPECT_NE(pixelMap, nullptr);
}

/**
 * @tc.name: TransformDataSetForAstcnTest005
 * @tc.desc: Verify function TransformDataSetForAstc
 * @tc.type:FUNC
 */
HWTEST_F(RSPixelMapUtilTest, TransformDataSetForAstcnTest005, TestSize.Level1)
{
    std::shared_ptr<Media::PixelMap> pixelMap;
    int width = 200;
    int height = 300;
    pixelMap = CreatePixelMap(width, height);
    Drawing::Rect src;
    Drawing::Rect dst;
    Drawing::Canvas canvas;
    pixelMap->transformData_.flipX = true;
    pixelMap->transformData_.flipY = true;
    pixelMap->transformData_.cropLeft = 0;
    pixelMap->transformData_.cropTop = 0;
    pixelMap->transformData_.cropWidth = 0;
    pixelMap->transformData_.cropHeight = 0;
    pixelMap->astcrealSize_.width = 1;
    pixelMap->astcrealSize_.height = 1;
    pixelMap->transformData_.scaleX = 1;
    pixelMap->transformData_.scaleY = 1;
    RSPixelMapUtil::TransformDataSetForAstc(pixelMap, src, dst, canvas, ImageFit::NONE);
    EXPECT_NE(pixelMap, nullptr);
}

/**
 * @tc.name: TransformDataSetForAstcnTest006
 * @tc.desc: Verify function TransformDataSetForAstc
 * @tc.type:FUNC
 */
HWTEST_F(RSPixelMapUtilTest, TransformDataSetForAstcnTest006, TestSize.Level1)
{
    std::shared_ptr<Media::PixelMap> pixelMap;
    int width = 200;
    int height = 300;
    pixelMap = CreatePixelMap(width, height);
    Drawing::Rect src;
    Drawing::Rect dst;
    Drawing::Canvas canvas;
    pixelMap->transformData_.flipX = true;
    pixelMap->transformData_.flipY = true;
    pixelMap->transformData_.cropLeft = 0;
    pixelMap->transformData_.cropTop = 0;
    pixelMap->transformData_.cropWidth = 1;
    pixelMap->transformData_.cropHeight = 1;
    pixelMap->astcrealSize_.width = 1;
    pixelMap->astcrealSize_.height = 1;
    pixelMap->transformData_.scaleX = 0;
    pixelMap->transformData_.scaleY = 0;
    RSPixelMapUtil::TransformDataSetForAstc(pixelMap, src, dst, canvas, ImageFit::NONE);
    EXPECT_NE(pixelMap, nullptr);
}

/**
 * @tc.name: IsYUVFormat
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSPixelMapUtilTest, IsYUVFormat, TestSize.Level1)
{
    std::shared_ptr<Media::PixelMap> pixelmap;
    int width = 200;
    int height = 300;
    pixelmap = CreatePixelMap(width, height);

    EXPECT_FALSE(RSPixelMapUtil::IsYUVFormat(pixelmap));
}

/**
 * @tc.name: ConvertYUVPixelMapToDrawingImage
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSPixelMapUtilTest, ConvertYUVPixelMapToDrawingImage, TestSize.Level1)
{
    std::shared_ptr<Media::PixelMap> pixelmap;
    int width = 200;
    int height = 300;
    pixelmap = CreatePixelMap(width, height);

    auto gpuContext = std::make_shared<Drawing::GPUContext>();
    EXPECT_EQ(RSPixelMapUtil::ConvertYUVPixelMapToDrawingImage(gpuContext, pixelmap), nullptr);
    pixelmap->imageInfo_.pixelFormat = Media::PixelFormat::NV12;
    EXPECT_EQ(RSPixelMapUtil::ConvertYUVPixelMapToDrawingImage(gpuContext, pixelmap), nullptr);
    pixelmap->imageInfo_.pixelFormat = Media::PixelFormat::NV21;
    EXPECT_EQ(RSPixelMapUtil::ConvertYUVPixelMapToDrawingImage(gpuContext, pixelmap), nullptr);
}

/**
 * @tc.name: DrawPixelMapTest001
 * @tc.desc: Verify function DrawPixelMap
 * @tc.type:FUNC
 */
HWTEST_F(RSPixelMapUtilTest, DrawPixelMapTest001, TestSize.Level1)
{
    Media::PixelMap pixelMap;
    Drawing::Canvas canvas;
    RSPixelMapUtil::DrawPixelMap(canvas, pixelMap, 1.0f, 1.0f);
    EXPECT_EQ(pixelMap.rowDataSize_, 0);
}

/**
 * @tc.name: DrawPixelMap
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSPixelMapUtilTest, DrawPixelMap, TestSize.Level1)
{
    std::shared_ptr<Media::PixelMap> pixelmap;
    int width = 200;
    int height = 300;
    pixelmap = CreatePixelMap(width, height);

    auto canvas = std::make_unique<Drawing::Canvas>();
    EXPECT_NE(nullptr, canvas);

    RSPixelMapUtil::DrawPixelMap(*canvas, *pixelmap, 0.0f, 0.0f);
}

/**
 * @tc.name: ConvertYUVPixelMapToDrawingImageTest
 * @tc.desc: Verify function ConvertYUVPixelMapToDrawingImage
 * @tc.type: FUNC
 * @tc.require: issuesI9MO9U
 */
HWTEST_F(RSPixelMapUtilTest, ConvertYUVPixelMapToDrawingImageTest, TestSize.Level1)
{
    std::shared_ptr<Drawing::GPUContext> gpuContext;
    std::shared_ptr<Media::PixelMap> pixelmap;
    RSPixelMapUtil::ConvertYUVPixelMapToDrawingImage(gpuContext, pixelmap);
    gpuContext = std::make_shared<Drawing::GPUContext>();
    EXPECT_NE(nullptr, gpuContext);
    RSPixelMapUtil::ConvertYUVPixelMapToDrawingImage(gpuContext, pixelmap);
    int width = 200;
    int height = 300;
    pixelmap = CreatePixelMap(width, height);
    auto image = RSPixelMapUtil::ConvertYUVPixelMapToDrawingImage(gpuContext, pixelmap);
    EXPECT_EQ(image, nullptr);
}

/**
 * @tc.name: ConvertYUVPixelMapToDrawingImage001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSPixelMapUtilTest, ConvertYUVPixelMapToDrawingImage001, TestSize.Level1)
{
    std::shared_ptr<Media::PixelMap> pixelmap;
    int width = 20;
    int height = 300;
    pixelmap = CreatePixelMap(width, height);
 
    auto gpuContext = std::make_shared<Drawing::GPUContext>();
    EXPECT_EQ(RSPixelMapUtil::ConvertYUVPixelMapToDrawingImage(gpuContext, pixelmap), nullptr);
    pixelmap->imageInfo_.pixelFormat = Media::PixelFormat::ARGB_8888;
    EXPECT_EQ(RSPixelMapUtil::ConvertYUVPixelMapToDrawingImage(gpuContext, pixelmap), nullptr);
    pixelmap->imageInfo_.pixelFormat = Media::PixelFormat::ARGB_8888;
    EXPECT_EQ(RSPixelMapUtil::ConvertYUVPixelMapToDrawingImage(gpuContext, pixelmap), nullptr);
}

/**
 * @tc.name: ConvertYUVPixelMapToDrawingImage002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSPixelMapUtilTest, ConvertYUVPixelMapToDrawingImage002, TestSize.Level1)
{
    std::shared_ptr<Media::PixelMap> pixelmap;
    int width = 2;
    int height = 3000;
    pixelmap = CreatePixelMap(width, height);
 
    auto gpuContext = std::make_shared<Drawing::GPUContext>();
    EXPECT_EQ(RSPixelMapUtil::ConvertYUVPixelMapToDrawingImage(gpuContext, pixelmap), nullptr);
    pixelmap->imageInfo_.pixelFormat = Media::PixelFormat::RGBA_1010102;
    EXPECT_EQ(RSPixelMapUtil::ConvertYUVPixelMapToDrawingImage(gpuContext, pixelmap), nullptr);
    pixelmap->imageInfo_.pixelFormat = Media::PixelFormat::RGBA_1010102;
    EXPECT_EQ(RSPixelMapUtil::ConvertYUVPixelMapToDrawingImage(gpuContext, pixelmap), nullptr);
}

/**
 * @tc.name: ConvertYUVPixelMapToDrawingImage003
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSPixelMapUtilTest, ConvertYUVPixelMapToDrawingImage003, TestSize.Level1)
{
    std::shared_ptr<Media::PixelMap> pixelmap;
    int width = 2;
    int height = 3000;
    pixelmap = CreatePixelMap(width, height);
 
    auto gpuContext = std::make_shared<Drawing::GPUContext>();
    EXPECT_EQ(RSPixelMapUtil::ConvertYUVPixelMapToDrawingImage(gpuContext, pixelmap), nullptr);
    pixelmap->imageInfo_.pixelFormat = Media::PixelFormat::ALPHA_8;
    EXPECT_EQ(RSPixelMapUtil::ConvertYUVPixelMapToDrawingImage(gpuContext, pixelmap), nullptr);
    pixelmap->imageInfo_.pixelFormat = Media::PixelFormat::ALPHA_8;
    EXPECT_EQ(RSPixelMapUtil::ConvertYUVPixelMapToDrawingImage(gpuContext, pixelmap), nullptr);
}

/**
 * @tc.name: ConvertYUVPixelMapToDrawingImage004
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSPixelMapUtilTest, ConvertYUVPixelMapToDrawingImage004, TestSize.Level1)
{
    std::shared_ptr<Media::PixelMap> pixelmap;
    int width = 2;
    int height = 3000;
    pixelmap = CreatePixelMap(width, height);
 
    auto gpuContext = std::make_shared<Drawing::GPUContext>();
    EXPECT_EQ(RSPixelMapUtil::ConvertYUVPixelMapToDrawingImage(gpuContext, pixelmap), nullptr);
    pixelmap->imageInfo_.pixelFormat = Media::PixelFormat::RGBA_F16;
    EXPECT_EQ(RSPixelMapUtil::ConvertYUVPixelMapToDrawingImage(gpuContext, pixelmap), nullptr);
    pixelmap->imageInfo_.pixelFormat = Media::PixelFormat::RGBA_F16;
    EXPECT_EQ(RSPixelMapUtil::ConvertYUVPixelMapToDrawingImage(gpuContext, pixelmap), nullptr);
}

/**
 * @tc.name: IsSupportZeroCopyTest
 * @tc.desc: Verify function IsSupportZeroCopy
 * @tc.type: FUNC
 */
HWTEST_F(RSPixelMapUtilTest, IsSupportZeroCopyTest, TestSize.Level1)
{
    std::shared_ptr<Media::PixelMap> pixelmap;
    int width = 200;
    int height = 300;
    pixelmap = CreatePixelMap(width, height);
    Drawing::SamplingOptions sampling;

    EXPECT_FALSE(RSPixelMapUtil::IsSupportZeroCopy(pixelmap, sampling));
}

/**
 * @tc.name: GetPixelmapColorSpaceTest
 * @tc.desc: Verify function GetPixelmapColorSpace
 * @tc.type: FUNC
 */
HWTEST_F(RSPixelMapUtilTest, GetPixelmapColorSpaceTest, TestSize.Level1)
{
    std::shared_ptr<Media::PixelMap> pixelmap;
    int width = 200;
    int height = 300;
    pixelmap = CreatePixelMap(width, height);

    auto colorSpace = RSPixelMapUtil::GetPixelmapColorSpace(pixelmap);
    EXPECT_NE(colorSpace, nullptr);
}
} // namespace Rosen
} // namespace OHOS