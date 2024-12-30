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

#include "render/rs_pixel_map_util.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class RSPixelMapUtilUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSPixelMapUtilUnitTest::SetUpTestCase() {}
void RSPixelMapUtilUnitTest::TearDownTestCase() {}
void RSPixelMapUtilUnitTest::SetUp() {}
void RSPixelMapUtilUnitTest::TearDown() {}

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
 * @tc.name: TestExtractDrawingImage01
 * @tc.desc: Verify function ExtractDrawingImage
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPixelMapUtilUnitTest, TestExtractDrawingImage01, TestSize.Level1)
{
    std::shared_ptr<Media::PixelMap> testPixelMap;
    int width = 200;
    int height = 300;
    testPixelMap = CreatePixelMap(width, height);
    testPixelMap->imageInfo_.alphaType = OHOS::Media::AlphaType::IMAGE_ALPHA_TYPE_OPAQUE;
    testPixelMap->imageInfo_.pixelFormat = Media::PixelFormat::RGB_565;
    EXPECT_NE(nullptr, RSPixelMapUtil::ExtractDrawingImage(testPixelMap));
    testPixelMap->imageInfo_.alphaType = OHOS::Media::AlphaType::IMAGE_ALPHA_TYPE_OPAQUE;
    testPixelMap->imageInfo_.pixelFormat = Media::PixelFormat::RGBA_8888;
    EXPECT_NE(nullptr, RSPixelMapUtil::ExtractDrawingImage(testPixelMap));
    testPixelMap->imageInfo_.alphaType = OHOS::Media::AlphaType::IMAGE_ALPHA_TYPE_PREMUL;
    testPixelMap->imageInfo_.pixelFormat = Media::PixelFormat::BGRA_8888;
    EXPECT_NE(nullptr, RSPixelMapUtil::ExtractDrawingImage(testPixelMap));
    testPixelMap->imageInfo_.alphaType = OHOS::Media::AlphaType::IMAGE_ALPHA_TYPE_UNPREMUL;
    testPixelMap->imageInfo_.pixelFormat = Media::PixelFormat::ALPHA_8;
    EXPECT_NE(nullptr, RSPixelMapUtil::ExtractDrawingImage(testPixelMap));
    testPixelMap->imageInfo_.pixelFormat = Media::PixelFormat::RGBA_F16;
    EXPECT_EQ(nullptr, RSPixelMapUtil::ExtractDrawingImage(testPixelMap));
    testPixelMap->imageInfo_.pixelFormat = Media::PixelFormat::RGBA_1010102;
    EXPECT_NE(nullptr, RSPixelMapUtil::ExtractDrawingImage(testPixelMap));
    testPixelMap->imageInfo_.pixelFormat = Media::PixelFormat::UNKNOWN;
    EXPECT_EQ(nullptr, RSPixelMapUtil::ExtractDrawingImage(testPixelMap));
    testPixelMap->imageInfo_.pixelFormat = Media::PixelFormat::ARGB_8888;
    EXPECT_EQ(nullptr, RSPixelMapUtil::ExtractDrawingImage(testPixelMap));
    testPixelMap->imageInfo_.pixelFormat = Media::PixelFormat::RGB_888;
    EXPECT_EQ(nullptr, RSPixelMapUtil::ExtractDrawingImage(testPixelMap));
    testPixelMap->imageInfo_.pixelFormat = Media::PixelFormat::NV21;
    EXPECT_EQ(nullptr, RSPixelMapUtil::ExtractDrawingImage(testPixelMap));
    testPixelMap->imageInfo_.pixelFormat = Media::PixelFormat::NV12;
    EXPECT_EQ(nullptr, RSPixelMapUtil::ExtractDrawingImage(testPixelMap));
    testPixelMap->imageInfo_.pixelFormat = Media::PixelFormat::CMYK;
    EXPECT_EQ(nullptr, RSPixelMapUtil::ExtractDrawingImage(testPixelMap));
}

/**
 * @tc.name: TestTransformDataSetForAstcn01
 * @tc.desc: Verify function TransformDataSetForAstc
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPixelMapUtilUnitTest, TestTransformDataSetForAstcn01, TestSize.Level1)
{
    std::shared_ptr<Media::PixelMap> testPixelMap;
    int width = 200;
    int height = 300;
    testPixelMap = CreatePixelMap(width, height);
    Drawing::Rect src;
    Drawing::Rect dst;
    Drawing::Canvas canvas;
    testPixelMap->transformData_.flipX = true;
    testPixelMap->transformData_.flipY = true;
    testPixelMap->transformData_.cropLeft = 0;
    testPixelMap->transformData_.cropTop = 0;
    testPixelMap->transformData_.cropWidth = 1;
    testPixelMap->transformData_.cropHeight = 1;
    testPixelMap->astcrealSize_.width = 1;
    testPixelMap->astcrealSize_.height = 1;
    testPixelMap->transformData_.scaleX = 1;
    testPixelMap->transformData_.scaleY = 1;
    RSPixelMapUtil::TransformDataSetForAstc(testPixelMap, src, dst, canvas);
    EXPECT_NE(testPixelMap, nullptr);
}

/**
 * @tc.name: TestIsYUVFormat01
 * @tc.desc: Verify function IsYUVFormat
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPixelMapUtilUnitTest, TestIsYUVFormat01, TestSize.Level1)
{
    std::shared_ptr<Media::PixelMap> testPixelMap;
    int width = 200;
    int height = 300;
    testPixelMap = CreatePixelMap(width, height);

    EXPECT_FALSE(RSPixelMapUtil::IsYUVFormat(testPixelMap));
}

/**
 * @tc.name: TestConvertYUVPixelMapToDrawingImage01
 * @tc.desc: Verify function ConvertYUVPixelMapToDrawingImage
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPixelMapUtilUnitTest, TestConvertYUVPixelMapToDrawingImage01, TestSize.Level1)
{
    std::shared_ptr<Media::PixelMap> testPixelMap;
    int width = 200;
    int height = 300;
    testPixelMap = CreatePixelMap(width, height);

    auto gpuContext = std::make_shared<Drawing::GPUContext>();
    EXPECT_EQ(RSPixelMapUtil::ConvertYUVPixelMapToDrawingImage(gpuContext, testPixelMap), nullptr);
    testPixelMap->imageInfo_.pixelFormat = Media::PixelFormat::NV12;
    EXPECT_EQ(RSPixelMapUtil::ConvertYUVPixelMapToDrawingImage(gpuContext, testPixelMap), nullptr);
    testPixelMap->imageInfo_.pixelFormat = Media::PixelFormat::NV21;
    EXPECT_EQ(RSPixelMapUtil::ConvertYUVPixelMapToDrawingImage(gpuContext, testPixelMap), nullptr);
}

/**
 * @tc.name: TestDrawPixelMap01
 * @tc.desc: Verify function DrawPixelMap
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPixelMapUtilUnitTest, TestDrawPixelMap01, TestSize.Level1)
{
    Media::PixelMap testPixelMap;
    Drawing::Canvas canvas;
    RSPixelMapUtil::DrawPixelMap(canvas, testPixelMap, 1.0f, 1.0f);
    EXPECT_EQ(testPixelMap.rowDataSize_, 0);
}

/**
 * @tc.name: TestDrawPixelMap02
 * @tc.desc: Verify function DrawPixelMap
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPixelMapUtilUnitTest, TestDrawPixelMap02, TestSize.Level1)
{
    std::shared_ptr<Media::PixelMap> testPixelMap;
    int width = 200;
    int height = 300;
    testPixelMap = CreatePixelMap(width, height);

    auto canvas = std::make_unique<Drawing::Canvas>();
    EXPECT_NE(nullptr, canvas);

    RSPixelMapUtil::DrawPixelMap(*canvas, *testPixelMap, 0.0f, 0.0f);
}

/**
 * @tc.name: TestConvertYUVPixelMapToDrawingImage02
 * @tc.desc: Verify function ConvertYUVPixelMapToDrawingImage
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPixelMapUtilUnitTest, TestConvertYUVPixelMapToDrawingImage02, TestSize.Level1)
{
    std::shared_ptr<Drawing::GPUContext> gpuContext;
    std::shared_ptr<Media::PixelMap> testPixelMap;
    RSPixelMapUtil::ConvertYUVPixelMapToDrawingImage(gpuContext, testPixelMap);
    gpuContext = std::make_shared<Drawing::GPUContext>();
    EXPECT_NE(nullptr, gpuContext);
    RSPixelMapUtil::ConvertYUVPixelMapToDrawingImage(gpuContext, testPixelMap);
    int width = 200;
    int height = 300;
    testPixelMap = CreatePixelMap(width, height);
    auto image = RSPixelMapUtil::ConvertYUVPixelMapToDrawingImage(gpuContext, testPixelMap);
    EXPECT_EQ(image, nullptr);
}

/**
 * @tc.name: TestIsSupportZeroCopy01
 * @tc.desc: Verify function IsSupportZeroCopy
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPixelMapUtilUnitTest, TestIsSupportZeroCopy01, TestSize.Level1)
{
    std::shared_ptr<Media::PixelMap> testPixelMap;
    int width = 200;
    int height = 300;
    testPixelMap = CreatePixelMap(width, height);
    Drawing::SamplingOptions sampling;

    EXPECT_FALSE(RSPixelMapUtil::IsSupportZeroCopy(testPixelMap, sampling));
}

/**
 * @tc.name: TestGetPixelmapColorSpace01
 * @tc.desc: Verify function GetPixelmapColorSpace
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPixelMapUtilUnitTest, TestGetPixelmapColorSpace01, TestSize.Level1)
{
    std::shared_ptr<Media::PixelMap> testPixelMap;
    int width = 200;
    int height = 300;
    testPixelMap = CreatePixelMap(width, height);

    auto colorSpace = RSPixelMapUtil::GetPixelmapColorSpace(testPixelMap);
    EXPECT_NE(colorSpace, nullptr);
}
} // namespace Rosen
} // namespace OHOS