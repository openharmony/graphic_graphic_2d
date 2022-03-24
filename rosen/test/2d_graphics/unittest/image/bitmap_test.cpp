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
#include "image/bitmap.h"
#include "draw/color.h"
 
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
 * @tc.name: BitmapCreateAndDestory001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BitmapTest, BitmapCreateAndDestory001, TestSize.Level1)
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
    BitmapFormat bitmapFormat = {ColorType::COLORTYPE_ALPHA_8, AlphaType::ALPHATYPE_OPAQUYE};
    bitmap->Build(100, 200, bitmapFormat);
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
    BitmapFormat bitmapFormat = {ColorType::COLORTYPE_ALPHA_8, AlphaType::ALPHATYPE_OPAQUYE};
    bitmap->Build(150, 99, bitmapFormat);
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
    BitmapFormat bitmapFormat = {ColorType::COLORTYPE_ALPHA_8, AlphaType::ALPHATYPE_OPAQUYE};
    bitmap->Build(111, 450, bitmapFormat);
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
    BitmapFormat bitmapFormat = {ColorType::COLORTYPE_ALPHA_8, AlphaType::ALPHATYPE_OPAQUYE};
    bitmap->Build(111, 450, bitmapFormat);
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
    BitmapFormat bitmapFormat = {ColorType::COLORTYPE_ALPHA_8, AlphaType::ALPHATYPE_OPAQUYE};
    bitmap->Build(151, 150, bitmapFormat);
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
    BitmapFormat bitmapFormat = {ColorType::COLORTYPE_ALPHA_8, AlphaType::ALPHATYPE_OPAQUYE};
    bitmap->Build(111, 450, bitmapFormat);
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
    BitmapFormat bitmapFormat = {ColorType::COLORTYPE_ALPHA_8, AlphaType::ALPHATYPE_OPAQUYE};
    bitmap->Build(151, 150, bitmapFormat);
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
    BitmapFormat *bitmapFormat1= nullptr;
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
    BitmapFormat * bitmapFormat2= nullptr;
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
    bitmap->CopyPixels(bitmap1, 100, 105, 201, 845);
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
    bitmap->CopyPixels(bitmap1, 66, 5, 99, 320);
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
    ASSERT_FALSE(! bitmap->IsValid());
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
    BitmapFormat bitmapFormat = {ColorType::COLORTYPE_ALPHA_8, AlphaType::ALPHATYPE_OPAQUYE};
    bitmap->Build(111, 450, bitmapFormat);
    ASSERT_EQ(ColorType::COLORTYPE_UNKNOWN, bitmap->GetFormat().colorType);
    ASSERT_EQ(AlphaType::ALPHATYPE_UNKNOWN, bitmap->GetFormat().alphaType);
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
    BitmapFormat bitmapFormat = {ColorType::COLORTYPE_ALPHA_8, AlphaType::ALPHATYPE_OPAQUYE};
    bitmap->Build(151, 150, bitmapFormat);
    ASSERT_EQ(ColorType::COLORTYPE_UNKNOWN, bitmap->GetFormat().colorType);
    ASSERT_EQ(AlphaType::ALPHATYPE_UNKNOWN, bitmap->GetFormat().alphaType);
    bitmap->Free();
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS