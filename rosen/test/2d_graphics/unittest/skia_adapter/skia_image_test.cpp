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

#include <cstddef>
#include "draw/surface.h"
#include "gtest/gtest.h"
#include "image/image.h"
#include "skia_adapter/skia_image.h"
#include "skia_adapter/skia_canvas.h"
#include "skia_adapter/skia_gpu_context.h"
#include "skia_adapter/skia_surface.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaImageTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SkiaImageTest::SetUpTestCase() {}
void SkiaImageTest::TearDownTestCase() {}
void SkiaImageTest::SetUp() {}
void SkiaImageTest::TearDown() {}

/**
 * @tc.name: BuildFromBitmap001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaImageTest, BuildFromBitmap001, TestSize.Level1)
{
    Bitmap bitmap;
    std::shared_ptr<SkiaImage> skiaImage = std::make_shared<SkiaImage>();
    auto image = skiaImage->BuildFromBitmap(bitmap);
    EXPECT_EQ(image, false);
}

/**
 * @tc.name: MakeRasterData001
 * @tc.desc: Test MakeRasterData
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaImageTest, MakeRasterData001, TestSize.Level1)
{
    std::shared_ptr<SkiaImage> skiaImage = std::make_shared<SkiaImage>();
    ImageInfo imageInfo;
    auto image = skiaImage->MakeRasterData(imageInfo, nullptr, 0);
    EXPECT_TRUE(image == nullptr);
    std::shared_ptr<Data> data = std::make_shared<Data>();
    data->BuildUninitialized(100);
    auto image2 = skiaImage->MakeRasterData(imageInfo, data, 100);
    EXPECT_TRUE(image2 == nullptr);
}

#ifdef ACE_ENABLE_GPU
/**
 * @tc.name: MakeFromEncoded001
 * @tc.desc: Test MakeFromEncoded
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaImageTest, MakeFromEncoded001, TestSize.Level1)
{
    std::shared_ptr<SkiaImage> skiaImage = std::make_shared<SkiaImage>();
    auto ret = skiaImage->MakeFromEncoded(nullptr);
    EXPECT_TRUE(!ret);
}

/**
 * @tc.name: BuildSubset001
 * @tc.desc: Test BuildSubset
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaImageTest, BuildSubset001, TestSize.Level1)
{
    auto surface = std::make_unique<SkiaSurface>();
    surface->FlushAndSubmit(true);
    Bitmap bitmap;
    BitmapFormat bitmapFormat { COLORTYPE_RGBA_8888, ALPHATYPE_OPAQUE };
    bitmap.Build(100, 100, bitmapFormat);
    ASSERT_TRUE(surface->Bind(bitmap));

    auto image = surface->GetImageSnapshot();
    GPUContext context;
#ifdef NEW_SKIA
    GrMockOptions options;
    context.GetImpl<SkiaGPUContext>()->SetGrContext(GrDirectContext::MakeMock(&options));
#endif
    auto skiaImage = image->GetImpl<SkiaImage>();
    if (skiaImage) {
        RectI rect;
        GPUContext context;
        skiaImage->BuildSubset(nullptr, rect, context);
    }
}

/**
 * @tc.name: BuildFromTexture001
 * @tc.desc: Test BuildFromTexture
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaImageTest, BuildFromTexture001, TestSize.Level1)
{
    std::shared_ptr<SkiaImage> skiaImage = std::make_shared<SkiaImage>();
    GPUContext context;
#ifdef NEW_SKIA
    GrMockOptions options;
    context.GetImpl<SkiaGPUContext>()->SetGrContext(GrDirectContext::MakeMock(&options));
#endif
    TextureInfo textureInfo;
    BitmapFormat bitmapFormat;
    skiaImage->BuildFromTexture(context, textureInfo, TextureOrigin::TOP_LEFT, bitmapFormat,
        nullptr, nullptr, nullptr);
    auto colorSpace = std::make_shared<ColorSpace>(ColorSpace::ColorSpaceType::NO_TYPE);
    skiaImage->BuildFromTexture(context, textureInfo, TextureOrigin::TOP_LEFT, bitmapFormat,
        colorSpace, nullptr, nullptr);
}

/**
 * @tc.name: BuildFromSurface001
 * @tc.desc: Test BuildFromSurface
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaImageTest, BuildFromSurface001, TestSize.Level1)
{
    std::shared_ptr<SkiaImage> skiaImage = std::make_shared<SkiaImage>();
    auto skiaCanvas = std::make_shared<SkiaCanvas>();
    auto gpuContext = skiaCanvas->GetGPUContext();
    auto surface = Surface::MakeRasterN32Premul(800, 800);
    BitmapFormat bitmapFormat { COLORTYPE_RGBA_8888, ALPHATYPE_OPAQUE };
    auto colorSpace = std::make_shared<ColorSpace>(ColorSpace::ColorSpaceType::NO_TYPE);
    auto ret = skiaImage->BuildFromSurface(*gpuContext, *surface, TextureOrigin::TOP_LEFT, bitmapFormat, colorSpace);
    ASSERT_TRUE(!ret);
    ASSERT_TRUE(skiaImage->GetColorSpace() == nullptr);
}

/**
 * @tc.name: IsTextureBacked002
 * @tc.desc: Test GetBackendTexture
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaImageTest, IsTextureBacked002, TestSize.Level1)
{
    auto surface = std::make_unique<SkiaSurface>();
    surface->FlushAndSubmit(true);
    Bitmap bitmap;
    BitmapFormat bitmapFormat { COLORTYPE_RGBA_8888, ALPHATYPE_OPAQUE };
    bitmap.Build(100, 100, bitmapFormat);
    ASSERT_TRUE(surface->Bind(bitmap));

    auto image = surface->GetImageSnapshot();
    if (image) {
        auto skiaImage = image->GetImpl<SkiaImage>();
        if (skiaImage) {
            ASSERT_TRUE(skiaImage->IsValid(nullptr));
            ASSERT_TRUE(!skiaImage->IsTextureBacked());
            ASSERT_TRUE(!skiaImage->IsLazyGenerated());
            ASSERT_TRUE(skiaImage->CanPeekPixels());
            ASSERT_TRUE(skiaImage->IsOpaque());
        }
    }
}

/**
 * @tc.name: IsValid001
 * @tc.desc: Test IsValid
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaImageTest, IsValid001, TestSize.Level1)
{
    Bitmap bitmap;
    std::shared_ptr<SkiaImage> skiaImage = std::make_shared<SkiaImage>();
    ASSERT_TRUE(!skiaImage->IsValid(nullptr));
    auto ret = skiaImage->BuildFromBitmap(bitmap);
    if (ret) {
        ASSERT_TRUE(!skiaImage->IsValid(nullptr));
        GPUContext context;
        ASSERT_TRUE(skiaImage->IsValid(&context));
    }
}
#endif

/**
 * @tc.name: IsLazyGenerated001
 * @tc.desc: Test IsLazyGenerated
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaImageTest, IsLazyGenerated001, TestSize.Level1)
{
    auto surface = std::make_unique<SkiaSurface>();
    surface->FlushAndSubmit(true);
    Bitmap bitmap;
    BitmapFormat bitmapFormat { COLORTYPE_RGBA_8888, ALPHATYPE_OPAQUE };
    bitmap.Build(100, 100, bitmapFormat);
    ASSERT_TRUE(surface->Bind(bitmap));

    auto image = surface->GetImageSnapshot();
    auto skiaImage = image->GetImpl<SkiaImage>();
    ASSERT_TRUE(skiaImage->GetImage() != nullptr);
    skiaImage->GetColorType();
    skiaImage->GetAlphaType();
    ASSERT_TRUE(skiaImage->EncodeToData(EncodedImageFormat::JPEG, 1) != nullptr);
    ASSERT_TRUE(skiaImage->GetColorSpace() == nullptr);
    ASSERT_TRUE(skiaImage->MakeRasterImage() != nullptr);
    ASSERT_TRUE(!skiaImage->IsTextureBacked());
    ASSERT_TRUE(!skiaImage->IsLazyGenerated());
    ASSERT_TRUE(skiaImage->CanPeekPixels());
    ASSERT_TRUE(skiaImage->IsOpaque());
    Bitmap bitmap2;
    ASSERT_TRUE(skiaImage->GetROPixels(bitmap2));
    Pixmap pixmap;
    skiaImage->ReadPixels(pixmap, 100, 100);
    Bitmap bitmap3;
    CubicResampler cubicResampler;
    SamplingOptions options1{cubicResampler};
    ASSERT_TRUE(!skiaImage->ScalePixels(bitmap3, options1, false));
}

/**
 * @tc.name: IsLazyGenerated002
 * @tc.desc: Test IsLazyGenerated
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaImageTest, IsLazyGenerated002, TestSize.Level1)
{
    std::shared_ptr<SkiaImage> skiaImage = std::make_shared<SkiaImage>();
    ASSERT_TRUE(!skiaImage->IsLazyGenerated());
}

/**
 * @tc.name: AsLegacyBitmap001
 * @tc.desc: Test AsLegacyBitmap
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaImageTest, AsLegacyBitmap001, TestSize.Level1)
{
    std::shared_ptr<SkiaImage> skiaImage = std::make_shared<SkiaImage>();
    Bitmap bitmap;
    ASSERT_TRUE(!skiaImage->AsLegacyBitmap(bitmap));
}

/**
 * @tc.name: IsOpaque001
 * @tc.desc: Test IsOpaque
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaImageTest, IsOpaque001, TestSize.Level1)
{
    std::shared_ptr<SkiaImage> skiaImage = std::make_shared<SkiaImage>();
    ASSERT_TRUE(!skiaImage->IsOpaque());
}

/**
 * @tc.name: GetAlphaType001
 * @tc.desc: Test GetAlphaType
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaImageTest, GetAlphaType001, TestSize.Level1)
{
    std::shared_ptr<SkiaImage> skiaImage = std::make_shared<SkiaImage>();
    skiaImage->GetAlphaType();
}

/**
 * @tc.name: GetColorType001
 * @tc.desc: Test GetColorType
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaImageTest, GetColorType001, TestSize.Level1)
{
    std::shared_ptr<SkiaImage> skiaImage = std::make_shared<SkiaImage>();
    skiaImage->GetColorType();
}

/**
 * @tc.name: GetColorSpace001
 * @tc.desc: Test GetColorSpace
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaImageTest, GetColorSpace001, TestSize.Level1)
{
    std::shared_ptr<SkiaImage> skiaImage = std::make_shared<SkiaImage>();
    ASSERT_TRUE(skiaImage->GetColorSpace() == nullptr);
}

/**
 * @tc.name: GetImageInfo001
 * @tc.desc: Test GetImageInfo
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaImageTest, GetImageInfo001, TestSize.Level1)
{
    std::shared_ptr<SkiaImage> skiaImage = std::make_shared<SkiaImage>();
    skiaImage->GetImageInfo();
}

/**
 * @tc.name: IsTextureBacked001
 * @tc.desc: Test IsTextureBacked
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaImageTest, IsTextureBacked001, TestSize.Level1)
{
    std::shared_ptr<SkiaImage> skiaImage = std::make_shared<SkiaImage>();
    ASSERT_TRUE(!skiaImage->IsTextureBacked());
}

/**
 * @tc.name: ReadPixels001
 * @tc.desc: Test ReadPixels
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaImageTest, ReadPixels001, TestSize.Level1)
{
    std::shared_ptr<SkiaImage> skiaImage = std::make_shared<SkiaImage>();
    Pixmap pixmap;
    skiaImage->ReadPixels(pixmap, 0, 0);
    ImageInfo dstInfo = ImageInfo::MakeN32Premul(100, 100); // 100: width, height
    skiaImage->ReadPixels(dstInfo, nullptr, 100, 100, 100); // 100: dstRowBytes, srcX, srcY
}

/**
 * @tc.name: ScalePixels001
 * @tc.desc: Test ScalePixels
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaImageTest, ScalePixels001, TestSize.Level1)
{
    std::shared_ptr<SkiaImage> skiaImage = std::make_shared<SkiaImage>();
    Bitmap bitmap;
    CubicResampler cubicResampler;
    SamplingOptions options1{cubicResampler};
    ASSERT_TRUE(!skiaImage->ScalePixels(bitmap, options1, false));
    SamplingOptions options2;
    ASSERT_TRUE(!skiaImage->ScalePixels(bitmap, options2, true));
}

/**
 * @tc.name: EncodeToData001
 * @tc.desc: Test EncodeToData
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaImageTest, EncodeToData001, TestSize.Level1)
{
    std::shared_ptr<SkiaImage> skiaImage = std::make_shared<SkiaImage>();
    ASSERT_TRUE(skiaImage->EncodeToData(EncodedImageFormat::JPEG, 1) == nullptr);
}

/**
 * @tc.name: MakeRasterImage001
 * @tc.desc: Test MakeRasterImage
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaImageTest, MakeRasterImage001, TestSize.Level1)
{
    std::shared_ptr<SkiaImage> skiaImage = std::make_shared<SkiaImage>();
    ASSERT_TRUE(skiaImage->MakeRasterImage() == nullptr);
}

/**
 * @tc.name: GetROPixels001
 * @tc.desc: Test GetROPixels
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaImageTest, GetROPixels001, TestSize.Level1)
{
    std::shared_ptr<SkiaImage> skiaImage = std::make_shared<SkiaImage>();
    Bitmap bitmap;
    ASSERT_TRUE(!skiaImage->GetROPixels(bitmap));
}

/**
 * @tc.name: CanPeekPixels001
 * @tc.desc: Test CanPeekPixels
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaImageTest, CanPeekPixels001, TestSize.Level1)
{
    std::shared_ptr<SkiaImage> skiaImage = std::make_shared<SkiaImage>();
    ASSERT_TRUE(!skiaImage->CanPeekPixels());
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS