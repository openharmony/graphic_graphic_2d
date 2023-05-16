/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"

#include "draw/brush.h"
#include "effect/color_space.h"
#include "drawing/engine_adapter/impl_factory.h"
#include "image/bitmap.h"
#include "image/gpu_context.h"
#include "image/image.h"
#include "image/picture.h"
#include "utils/matrix.h"
#include "utils/size.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class ImageTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void ImageTest::SetUpTestCase() {}
void ImageTest::TearDownTestCase() {}
void ImageTest::SetUp() {}
void ImageTest::TearDown() {}

/**
 * @tc.name: CreateAndDestroy001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ImageTest, CreateAndDestroy001, TestSize.Level1)
{
    // The best way to create Image.
    std::unique_ptr<Image> image = std::make_unique<Image>();
    ASSERT_TRUE(image != nullptr);
}

/**
 * @tc.name: BuildFromBitmap001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ImageTest, BuildFromBitmap001, TestSize.Level1)
{
    std::unique_ptr<Image> image = std::make_unique<Image>();
    ASSERT_TRUE(image != nullptr);
    Bitmap bitmap;
    image->BuildFromBitmap(bitmap);
}

/**
 * @tc.name: BuildFromPicture001
 * @tc.desc: test for creating Image from Picture.
 * @tc.type: FUNC
 * @tc.require: I70OWN
 */
HWTEST_F(ImageTest, BuildFromPicture001, TestSize.Level1)
{
    std::unique_ptr<Image> image = std::make_unique<Image>();
    ASSERT_TRUE(image != nullptr);
    Picture picture;
    SizeI dimensions;
    Matrix matrix;
    Brush brush;
    BitDepth bitDepth = BitDepth::KU8;
    std::shared_ptr<ColorSpace> colorSpace(new ColorSpace(ColorSpace::ColorSpaceType::NO_TYPE));
    image->BuildFromPicture(picture, dimensions, matrix, brush, bitDepth, colorSpace);
}

/**
 * @tc.name: ImageGetWidthTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ImageTest, ImageGetWidthTest001, TestSize.Level1)
{
    Bitmap bitmap;
    BitmapFormat bitmapFormat { COLORTYPE_RGBA_8888, ALPHATYPE_OPAQUE };
    bitmap.Build(10, 10, bitmapFormat);
    Image image;
    image.BuildFromBitmap(bitmap);
    ASSERT_EQ(10, image.GetWidth());
}

/**
 * @tc.name: ImageGetWidthTest002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ImageTest, ImageGetWidthTest002, TestSize.Level1)
{
    Bitmap bitmap;
    BitmapFormat bitmapFormat { COLORTYPE_RGBA_8888, ALPHATYPE_OPAQUE };
    bitmap.Build(15, 15, bitmapFormat);
    Image image;
    image.BuildFromBitmap(bitmap);
    ASSERT_EQ(15, image.GetWidth());
}

/**
 * @tc.name: ImageGetHeightTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ImageTest, ImageGetHeightTest001, TestSize.Level1)
{
    Bitmap bitmap;
    BitmapFormat bitmapFormat { COLORTYPE_RGBA_8888, ALPHATYPE_OPAQUE };
    bitmap.Build(10, 10, bitmapFormat);
    Image image;
    image.BuildFromBitmap(bitmap);
    ASSERT_EQ(10, image.GetHeight());
}

/**
 * @tc.name: ImageGetHeightTest002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ImageTest, ImageGetHeightTest002, TestSize.Level1)
{
    Bitmap bitmap;
    BitmapFormat bitmapFormat { COLORTYPE_RGBA_8888, ALPHATYPE_OPAQUE };
    bitmap.Build(15, 15, bitmapFormat);
    Image image;
    image.BuildFromBitmap(bitmap);
    ASSERT_EQ(15, image.GetHeight());
}

/**
 * @tc.name: ImageGetUniqueIDTest001
 * @tc.desc: test for geting the unique Id of Image.
 * @tc.type: FUNC
 * @tc.require: I70OWN
 */
HWTEST_F(ImageTest, ImageGetUniqueIDTest001, TestSize.Level1)
{
    Bitmap bitmap;
    BitmapFormat bitmapFormat { COLORTYPE_RGBA_8888, ALPHATYPE_OPAQUE };
    bitmap.Build(15, 15, bitmapFormat);
    Image image;
    image.BuildFromBitmap(bitmap);
    ASSERT_EQ(10, image.GetUniqueID());
}

/**
 * @tc.name: ReadPixelsTest001
 * @tc.desc: test for Coping a Rect of pixels from Image to Bitmap.
 * @tc.type: FUNC
 * @tc.require: I70OWN
 */
HWTEST_F(ImageTest, ReadPixelsTest001, TestSize.Level1)
{
    Bitmap bitmap;
    Image image;
    int x = 0;
    int y = 0;
    EXPECT_FALSE(image.ReadPixels(bitmap, x, y));
}

/**
 * @tc.name: ReadPixelsTest002
 * @tc.desc: test for Coping a Rect of pixels from Image to Bitmap.
 * @tc.type: FUNC
 * @tc.require: I70OWN
 */
HWTEST_F(ImageTest, ReadPixelsTest002, TestSize.Level1)
{
    Bitmap bitmap;
    BitmapFormat bitmapFormat { COLORTYPE_RGBA_8888, ALPHATYPE_OPAQUE };
    bitmap.Build(15, 15, bitmapFormat);
    Image image;
    image.BuildFromBitmap(bitmap);
    int x = 0;
    int y = 0;
    EXPECT_TRUE(image.ReadPixels(bitmap, x, y));
}

/**
 * @tc.name: IsTextureBackedTest001
 * @tc.desc: test for IsTextureBacked function.
 * @tc.type: FUNC
 * @tc.require: I70OWN
 */
HWTEST_F(ImageTest, IsTextureBackedTest001, TestSize.Level1)
{
    Bitmap bitmap;
    BitmapFormat bitmapFormat { COLORTYPE_RGBA_8888, ALPHATYPE_OPAQUE };
    bitmap.Build(15, 15, bitmapFormat);
    Image image;
    image.BuildFromBitmap(bitmap);
    EXPECT_FALSE(image.IsTextureBacked());
}

#ifdef ACE_ENABLE_GPU
/**
 * @tc.name: BuildFromCompressedTest001
 * @tc.desc: test for creating a GPU-backed Image from compressed data.
 * @tc.type: FUNC
 * @tc.require: I70OWN
 */
HWTEST_F(ImageTest, BuildFromCompressedTest001, TestSize.Level1)
{
    GPUContext gpuContext;
    const std::shared_ptr<Data> data = nullptr;
    std::unique_ptr<Image> image = std::make_unique<Image>();
    ASSERT_TRUE(image != nullptr);
    image->BuildFromCompressed(gpuContext, data, 15, 15, CompressedType::ASTC);
}

/**
 * @tc.name: BuildFromCompressedTest002
 * @tc.desc: test for creating a GPU-backed Image from compressed data.
 * @tc.type: FUNC
 * @tc.require: I70OWN
 */
HWTEST_F(ImageTest, BuildFromCompressedTest002, TestSize.Level1)
{
    GPUContext gpuContext;
    std::shared_ptr<Data> data = std::make_shared<Data>();
    ASSERT_TRUE(data != nullptr);
    std::unique_ptr<Image> image = std::make_unique<Image>();
    ASSERT_TRUE(image != nullptr);
    image->BuildFromCompressed(gpuContext, data, 15, 15, CompressedType::ASTC);
}

/**
 * @tc.name: BuildFromCompressedTest003
 * @tc.desc: test for creating a GPU-backed Image from compressed data.
 * @tc.type: FUNC
 * @tc.require: I70OWN
 */
HWTEST_F(ImageTest, BuildFromCompressedTest003, TestSize.Level1)
{
    GPUContext gpuContext;
    std::shared_ptr<Data> data = std::make_shared<Data>();
    ASSERT_TRUE(data != nullptr);
    std::unique_ptr<Image> image = std::make_unique<Image>();
    ASSERT_TRUE(image != nullptr);
    image->BuildFromCompressed(gpuContext, data, 15, 15, CompressedType::ETC1);
}

/**
 * @tc.name: BuildFromBitmapTest001
 * @tc.desc: test for creating Image from Bitmap. Image is uploaded to GPU back-end using context.
 * @tc.type: FUNC
 * @tc.require: I70OWN
 */
HWTEST_F(ImageTest, BuildFromBitmapTest001, TestSize.Level1)
{
    GPUContext gpuContext;
    Bitmap bitmap;
    BitmapFormat bitmapFormat { COLORTYPE_RGBA_8888, ALPHATYPE_OPAQUE };
    bitmap.Build(15, 15, bitmapFormat);
    std::unique_ptr<Image> image = std::make_unique<Image>();
    ASSERT_TRUE(image != nullptr);
    image->BuildFromBitmap(gpuContext, bitmap);
}

/**
 * @tc.name: BuildFromTextureTest001
 * @tc.desc: test for creating Image from GPU texture associated with context.
 * @tc.type: FUNC
 * @tc.require: I70OWN
 */
HWTEST_F(ImageTest, BuildFromTextureTest001, TestSize.Level1)
{
    GPUContext gpuContext;
    TextureInfo info;
    info.SetWidth(10);
    BitmapFormat bitmapFormat { COLORTYPE_RGBA_8888, ALPHATYPE_OPAQUE };
    std::shared_ptr<ColorSpace> colorSpace(new ColorSpace(ColorSpace::ColorSpaceType::NO_TYPE));
    std::unique_ptr<Image> image = std::make_unique<Image>();
    ASSERT_TRUE(image != nullptr);
    image->BuildFromTexture(gpuContext, info, TextureOrigin::TOP_LEFT, bitmapFormat, colorSpace);
}
#endif
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS