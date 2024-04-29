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

#include "gtest/gtest.h"

#include "draw/color.h"
#include "image/bitmap.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class BitmapTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void BitmapTest::SetUpTestCase() {}
void BitmapTest::TearDownTestCase() {}
void BitmapTest::SetUp() {}
void BitmapTest::TearDown() {}

/**
 * @tc.name: BitmapCreateAndDestroy001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BitmapTest, BitmapCreateAndDestroy001, TestSize.Level1)
{
    // The best way to create Bitmap.
    std::unique_ptr<Bitmap> bitmap = std::make_unique<Bitmap>();
    ASSERT_TRUE(bitmap != nullptr);
}

/**
 * @tc.name: BitmapBuildTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BitmapTest, BitmapBuildTest001, TestSize.Level1)
{
    // The best way to Build Bitmap.
    std::unique_ptr<Bitmap> bitmap = std::make_unique<Bitmap>();
    ASSERT_TRUE(bitmap != nullptr);
    BitmapFormat bitmapFormat = { ColorType::COLORTYPE_ALPHA_8, AlphaType::ALPHATYPE_OPAQUE };
    EXPECT_TRUE(bitmap->Build(100, 200, bitmapFormat));

    bitmapFormat = { COLORTYPE_RGB_565, ALPHATYPE_PREMUL };
    EXPECT_TRUE(bitmap->Build(100, 200, bitmapFormat));

    bitmapFormat = { COLORTYPE_ARGB_4444, ALPHATYPE_UNPREMUL };
    EXPECT_TRUE(bitmap->Build(100, 200, bitmapFormat));

    bitmapFormat.colorType = COLORTYPE_RGBA_8888;
    EXPECT_TRUE(bitmap->Build(100, 200, bitmapFormat));

    bitmapFormat.colorType = COLORTYPE_BGRA_8888;
    EXPECT_TRUE(bitmap->Build(100, 200, bitmapFormat));

    bitmapFormat.colorType = COLORTYPE_N32;
    EXPECT_TRUE(bitmap->Build(100, 200, bitmapFormat));
}

/**
 * @tc.name: BitmapBuildTest002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BitmapTest, BitmapBuildTest002, TestSize.Level1)
{
    // The best way to Build Bitmap.
    std::unique_ptr<Bitmap> bitmap = std::make_unique<Bitmap>();
    ASSERT_TRUE(bitmap != nullptr);
    BitmapFormat bitmapFormat = { ColorType::COLORTYPE_ALPHA_8, AlphaType::ALPHATYPE_OPAQUE };
    EXPECT_TRUE(bitmap->Build(150, 99, bitmapFormat));

    bitmapFormat = { COLORTYPE_UNKNOWN, ALPHATYPE_OPAQUE };
    EXPECT_FALSE(bitmap->Build(100, 200, bitmapFormat));

    bitmapFormat = { COLORTYPE_ALPHA_8, ALPHATYPE_UNKNOWN };
    EXPECT_FALSE(bitmap->Build(100, 200, bitmapFormat));
}

/**
 * @tc.name: BitmapBuildTest003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BitmapTest, BitmapBuildTest003, TestSize.Level1)
{
    // The best way to Build Bitmap.
    std::unique_ptr<Bitmap> bitmap = std::make_unique<Bitmap>();
    ASSERT_TRUE(bitmap != nullptr);
    ImageInfo imageInfo = {111, 450, ColorType::COLORTYPE_ALPHA_8, AlphaType::ALPHATYPE_OPAQUE};
    EXPECT_TRUE(bitmap->Build(imageInfo));

    imageInfo = {111, 450, ColorType::COLORTYPE_UNKNOWN, AlphaType::ALPHATYPE_OPAQUE};
    EXPECT_FALSE(bitmap->Build(imageInfo));

    imageInfo = {111, 450, ColorType::COLORTYPE_ALPHA_8, AlphaType::ALPHATYPE_UNKNOWN};
    EXPECT_FALSE(bitmap->Build(imageInfo));
}

/**
 * @tc.name: BitmapGetWidthTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BitmapTest, BitmapGetWidthTest001, TestSize.Level1)
{
    // The best way to get width.
    std::unique_ptr<Bitmap> bitmap = std::make_unique<Bitmap>();
    ASSERT_TRUE(bitmap != nullptr);
    BitmapFormat bitmapFormat = { ColorType::COLORTYPE_ALPHA_8, AlphaType::ALPHATYPE_OPAQUE };
    EXPECT_TRUE(bitmap->Build(111, 450, bitmapFormat));
    ASSERT_EQ(111, bitmap->GetWidth());
}

/**
 * @tc.name: BitmapGetWidthTest002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BitmapTest, BitmapGetWidthTest002, TestSize.Level1)
{
    // The best way to get width.
    std::unique_ptr<Bitmap> bitmap = std::make_unique<Bitmap>();
    ASSERT_TRUE(bitmap != nullptr);
    BitmapFormat bitmapFormat = { ColorType::COLORTYPE_ALPHA_8, AlphaType::ALPHATYPE_OPAQUE };
    EXPECT_TRUE(bitmap->Build(151, 150, bitmapFormat));
    ASSERT_EQ(151, bitmap->GetWidth());
}

/**
 * @tc.name: BitmapGetHeightTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BitmapTest, BitmapGetHeightTest001, TestSize.Level1)
{
    // The best way to get height.
    std::unique_ptr<Bitmap> bitmap = std::make_unique<Bitmap>();
    ASSERT_TRUE(bitmap != nullptr);
    BitmapFormat bitmapFormat = { ColorType::COLORTYPE_ALPHA_8, AlphaType::ALPHATYPE_OPAQUE };
    EXPECT_TRUE(bitmap->Build(111, 450, bitmapFormat));
    ASSERT_EQ(450, bitmap->GetHeight());
}

/**
 * @tc.name: BitmapGetHeightTest002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BitmapTest, BitmapGetHeightTest002, TestSize.Level1)
{
    // The best way to get height.
    std::unique_ptr<Bitmap> bitmap = std::make_unique<Bitmap>();
    ASSERT_TRUE(bitmap != nullptr);
    BitmapFormat bitmapFormat = { ColorType::COLORTYPE_ALPHA_8, AlphaType::ALPHATYPE_OPAQUE };
    EXPECT_TRUE(bitmap->Build(151, 150, bitmapFormat));
    ASSERT_EQ(150, bitmap->GetHeight());
}

/**
 * @tc.name: BitmapSetAndPixelsTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BitmapTest, BitmapSetAndPixelsTest001, TestSize.Level1)
{
    std::unique_ptr<Bitmap> bitmap = std::make_unique<Bitmap>();
    ASSERT_TRUE(bitmap != nullptr);
    BitmapFormat* bitmapFormat1 = nullptr;
    bitmap->SetPixels(bitmapFormat1);
    EXPECT_EQ(bitmapFormat1, bitmap->GetPixels());
}

/**
 * @tc.name: BitmapSetAndPixelsTest002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BitmapTest, BitmapSetAndPixelsTest002, TestSize.Level1)
{
    std::unique_ptr<Bitmap> bitmap = std::make_unique<Bitmap>();
    ASSERT_TRUE(bitmap != nullptr);
    BitmapFormat* bitmapFormat2 = nullptr;
    bitmap->SetPixels(bitmapFormat2);
    EXPECT_EQ(bitmapFormat2, bitmap->GetPixels());
}

/**
 * @tc.name: BitmapCopyPixelsTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BitmapTest, BitmapCopyPixelsTest001, TestSize.Level1)
{
    std::unique_ptr<Bitmap> bitmap = std::make_unique<Bitmap>();
    ASSERT_TRUE(bitmap != nullptr);
    Bitmap bitmap1;
    bitmap->CopyPixels(bitmap1, 100, 105);
}

/**
 * @tc.name: BitmapCopyPixelsTest002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BitmapTest, BitmapCopyPixelsTest002, TestSize.Level1)
{
    std::unique_ptr<Bitmap> bitmap = std::make_unique<Bitmap>();
    ASSERT_TRUE(bitmap != nullptr);
    Bitmap bitmap1;
    bitmap->CopyPixels(bitmap1, 66, 5);
}

/**
 * @tc.name: BitmapClearWithColor001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BitmapTest, BitmapClearWithColor001, TestSize.Level1)
{
    std::unique_ptr<Bitmap> bitmap = std::make_unique<Bitmap>();
    ASSERT_TRUE(bitmap != nullptr);
    bitmap->ClearWithColor(COLORTYPE_UNKNOWN);
}

/**
 * @tc.name: BitmapClearWithColor002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BitmapTest, BitmapClearWithColor002, TestSize.Level1)
{
    std::unique_ptr<Bitmap> bitmap = std::make_unique<Bitmap>();
    ASSERT_TRUE(bitmap != nullptr);
    bitmap->ClearWithColor(COLORTYPE_ALPHA_8);
}

/**
 * @tc.name: BitmapIsValid001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BitmapTest, BitmapIsValid001, TestSize.Level1)
{
    std::unique_ptr<Bitmap> bitmap = std::make_unique<Bitmap>();
    ASSERT_TRUE(bitmap != nullptr);
    ASSERT_TRUE(bitmap->IsValid());
}

/**
 * @tc.name: BitmapIsValid002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BitmapTest, BitmapIsValid002, TestSize.Level1)
{
    std::unique_ptr<Bitmap> bitmap = std::make_unique<Bitmap>();
    ASSERT_TRUE(bitmap != nullptr);
    ASSERT_FALSE(!bitmap->IsValid());
}

/**
 * @tc.name: BitmapBitmapGetColorTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BitmapTest, BitmapBitmapGetColorTest001, TestSize.Level1)
{
    std::unique_ptr<Bitmap> bitmap = std::make_unique<Bitmap>();
    ASSERT_TRUE(bitmap != nullptr);
    ASSERT_EQ(Color::COLOR_TRANSPARENT, bitmap->GetColor(0, 0));
}

/**
 * @tc.name: BitmapGetColorTest002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BitmapTest, BitmapGetColorTest002, TestSize.Level1)
{
    std::unique_ptr<Bitmap> bitmap = std::make_unique<Bitmap>();
    ASSERT_TRUE(bitmap != nullptr);
    ASSERT_EQ(Color::COLOR_TRANSPARENT, bitmap->GetColor(1, 2));
}

/**
 * @tc.name: BitmapGetFormatTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BitmapTest, BitmapGetFormatTest001, TestSize.Level1)
{
    std::unique_ptr<Bitmap> bitmap = std::make_unique<Bitmap>();
    ASSERT_TRUE(bitmap != nullptr);
    BitmapFormat bitmapFormat = { ColorType::COLORTYPE_ALPHA_8, AlphaType::ALPHATYPE_OPAQUE };
    EXPECT_TRUE(bitmap->Build(111, 450, bitmapFormat));
    ASSERT_EQ(ColorType::COLORTYPE_ALPHA_8, bitmap->GetFormat().colorType);
    ASSERT_EQ(AlphaType::ALPHATYPE_OPAQUE, bitmap->GetFormat().alphaType);
    bitmap->Free();
}

/**
 * @tc.name: BitmapGetFormatTest002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BitmapTest, BitmapGetFormatTest002, TestSize.Level1)
{
    std::unique_ptr<Bitmap> bitmap = std::make_unique<Bitmap>();
    ASSERT_TRUE(bitmap != nullptr);
    BitmapFormat bitmapFormat = { ColorType::COLORTYPE_ALPHA_8, AlphaType::ALPHATYPE_OPAQUE };
    EXPECT_TRUE(bitmap->Build(151, 150, bitmapFormat));
    ASSERT_EQ(ColorType::COLORTYPE_ALPHA_8, bitmap->GetFormat().colorType);
    ASSERT_EQ(AlphaType::ALPHATYPE_OPAQUE, bitmap->GetFormat().alphaType);
    bitmap->Free();
}

/*
 * @tc.name: BitmapGetImageInfoTest001
 * @tc.desc: test for bitmap GetImageInfo.
 * @tc.type: FUNC
 * @tc.require: AR20240104201189
 */
HWTEST_F(BitmapTest, BitmapGetImageInfoTest001, TestSize.Level1)
{
    std::unique_ptr<Bitmap> bitmap = std::make_unique<Bitmap>();
    ASSERT_TRUE(bitmap != nullptr);
    BitmapFormat bitmapFormat = { ColorType::COLORTYPE_ALPHA_8, AlphaType::ALPHATYPE_OPAQUE };
    const unsigned int width = 500;
    const unsigned int height = 500;
    EXPECT_TRUE(bitmap->Build(width, height, bitmapFormat));

    auto imageInfo = bitmap->GetImageInfo();
    EXPECT_EQ(width, imageInfo.GetWidth());
    EXPECT_EQ(height, imageInfo.GetHeight());
    
    ImageInfo info{width, height, ColorType::COLORTYPE_ALPHA_8, AlphaType::ALPHATYPE_OPAQUE};
    bitmap->SetInfo(info);
    auto info2 = bitmap->GetImageInfo();
    EXPECT_EQ(width, info2.GetWidth());
    EXPECT_EQ(height, info2.GetHeight());

    bitmap->Free();
}

/*
 * @tc.name: BitmapTryAllocPixelsTest001
 * @tc.desc: test for bitmap TryAllocPixels.
 * @tc.type: FUNC
 * @tc.require: AR20240104201189
 */
HWTEST_F(BitmapTest, BitmapTryAllocPixelsTest001, TestSize.Level1)
{
    std::unique_ptr<Bitmap> bitmap = std::make_unique<Bitmap>();
    ASSERT_TRUE(bitmap != nullptr);
    ImageInfo imageInfo = {500, 500, ColorType::COLORTYPE_ALPHA_8, AlphaType::ALPHATYPE_OPAQUE};
    EXPECT_TRUE(bitmap->TryAllocPixels(imageInfo));

    imageInfo = {500, 500, ColorType::COLORTYPE_UNKNOWN, AlphaType::ALPHATYPE_OPAQUE};
    EXPECT_TRUE(bitmap->TryAllocPixels(imageInfo));

    imageInfo = {500, 500, ColorType::COLORTYPE_ALPHA_8, AlphaType::ALPHATYPE_UNKNOWN};
    EXPECT_FALSE(bitmap->TryAllocPixels(imageInfo));
}

/**
 * @tc.name: ExtractSubset001
 * @tc.desc: Test ExtractSubset
 * @tc.type: FUNC
 * @tc.require: AR20240104201189
 */
HWTEST_F(BitmapTest, ExtractSubset001, TestSize.Level1)
{
    Bitmap left;
    Rect subset = Rect(0, 0, 50, 50); // 50: right, bottom
    Bitmap bitmap;
    ASSERT_TRUE(!bitmap.ExtractSubset(left, subset));
    bitmap.Free();
}

/**
 * @tc.name: ReadPixels001
 * @tc.desc: Test ReadPixels
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BitmapTest, ReadPixels001, TestSize.Level1)
{
    std::unique_ptr<Bitmap> bitmap = std::make_unique<Bitmap>();
    ASSERT_TRUE(bitmap != nullptr);
    BitmapFormat bitmapFormat = { ColorType::COLORTYPE_ALPHA_8, AlphaType::ALPHATYPE_OPAQUE };
    EXPECT_TRUE(bitmap->Build(200, 200, bitmapFormat)); // 200: width height
    ImageInfo imageInfo{50, 50, ColorType::COLORTYPE_ALPHA_8, AlphaType::ALPHATYPE_OPAQUE}; // 50: width height
    void* dstPixels = nullptr;
    ASSERT_TRUE(!bitmap->ReadPixels(imageInfo, dstPixels, 200, 0, 0)); // 200: dstRowBytes
    bitmap->Free();
}

/**
 * @tc.name: ComputeByteSize001
 * @tc.desc: Test ComputeByteSize
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BitmapTest, ComputeByteSize001, TestSize.Level1)
{
    std::unique_ptr<Bitmap> bitmap = std::make_unique<Bitmap>();
    ASSERT_TRUE(bitmap != nullptr);
    BitmapFormat bitmapFormat = { ColorType::COLORTYPE_ALPHA_8, AlphaType::ALPHATYPE_OPAQUE };
    EXPECT_TRUE(bitmap->Build(200, 200, bitmapFormat)); // 200: width height
    ASSERT_TRUE(bitmap->ComputeByteSize() >= 0);
    bitmap->Free();
}

/**
 * @tc.name: Immutable001
 * @tc.desc: Test IsImmutable and SetImmutable
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BitmapTest, Immutable001, TestSize.Level1)
{
    std::unique_ptr<Bitmap> bitmap = std::make_unique<Bitmap>();
    ASSERT_TRUE(bitmap != nullptr);
    BitmapFormat bitmapFormat = { ColorType::COLORTYPE_ALPHA_8, AlphaType::ALPHATYPE_OPAQUE };
    EXPECT_TRUE(bitmap->Build(200, 200, bitmapFormat)); // 200: width height
    bitmap->SetImmutable();
    ASSERT_TRUE(bitmap->IsImmutable());
    bitmap->Free();
}

/**
 * @tc.name: IsEmpty001
 * @tc.desc: Test IsEmpty
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BitmapTest, IsEmpty001, TestSize.Level1)
{
    std::unique_ptr<Bitmap> bitmap = std::make_unique<Bitmap>();
    ASSERT_TRUE(bitmap != nullptr);
    ASSERT_TRUE(bitmap->IsEmpty());
    BitmapFormat bitmapFormat = { ColorType::COLORTYPE_ALPHA_8, AlphaType::ALPHATYPE_OPAQUE };
    EXPECT_TRUE(bitmap->Build(200, 200, bitmapFormat)); // 200: width height
    ASSERT_TRUE(!bitmap->IsEmpty());
    bitmap->Free();
}

/**
 * @tc.name: GetPixmap001
 * @tc.desc: Test GetPixmap
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BitmapTest, GetPixmap001, TestSize.Level1)
{
    std::unique_ptr<Bitmap> bitmap = std::make_unique<Bitmap>();
    ASSERT_TRUE(bitmap != nullptr);
    const unsigned int width = 500;
    const unsigned int height = 500;
    BitmapFormat bitmapFormat = { ColorType::COLORTYPE_ALPHA_8, AlphaType::ALPHATYPE_OPAQUE };
    EXPECT_TRUE(bitmap->Build(width, height, bitmapFormat));
    ASSERT_TRUE(width == bitmap->GetPixmap().GetWidth());
    bitmap->Free();
}

/**
 * @tc.name: Serialize001
 * @tc.desc: Test Serialize
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BitmapTest, Serialize001, TestSize.Level1)
{
    std::unique_ptr<Bitmap> bitmap = std::make_unique<Bitmap>();
    ASSERT_TRUE(bitmap != nullptr);
    const unsigned int width = 500;
    const unsigned int height = 500;
    BitmapFormat bitmapFormat = { ColorType::COLORTYPE_ALPHA_8, AlphaType::ALPHATYPE_OPAQUE };
    EXPECT_TRUE(bitmap->Build(width, height, bitmapFormat));
    ASSERT_TRUE(bitmap->Serialize() != nullptr);
    bitmap->Free();
}

/**
 * @tc.name: SetFormat001
 * @tc.desc: Test SetFormat
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BitmapTest, SetFormat001, TestSize.Level1)
{
    std::unique_ptr<Bitmap> bitmap = std::make_unique<Bitmap>();
    ASSERT_TRUE(bitmap != nullptr);
    BitmapFormat bitmapFormat = { ColorType::COLORTYPE_ALPHA_8, AlphaType::ALPHATYPE_OPAQUE };
    bitmap->SetFormat(bitmapFormat);
    ASSERT_EQ(ColorType::COLORTYPE_ALPHA_8, bitmap->GetFormat().colorType);
    ASSERT_EQ(AlphaType::ALPHATYPE_OPAQUE, bitmap->GetFormat().alphaType);
    bitmap->Free();
}

/**
 * @tc.name: GetColorType001
 * @tc.desc: Test GetColorType
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BitmapTest, GetColorType001, TestSize.Level1)
{
    // The best way to Build Bitmap.
    std::unique_ptr<Bitmap> bitmap = std::make_unique<Bitmap>();
    ASSERT_TRUE(bitmap != nullptr);
    BitmapFormat bitmapFormat = { ColorType::COLORTYPE_ALPHA_8, AlphaType::ALPHATYPE_OPAQUE };
    EXPECT_TRUE(bitmap->Build(200, 200, bitmapFormat)); // 200: width height
    EXPECT_TRUE(bitmap->GetColorType() == ColorType::COLORTYPE_ALPHA_8);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS