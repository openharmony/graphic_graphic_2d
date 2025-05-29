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

#include <cstddef>
#include "gtest/gtest.h"
#include "skia_adapter/skia_color_space.h"
#include "effect/color_space.h"
#include "image/bitmap.h"
#include "image/image.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaColorSpaceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SkiaColorSpaceTest::SetUpTestCase() {}
void SkiaColorSpaceTest::TearDownTestCase() {}
void SkiaColorSpaceTest::SetUp() {}
void SkiaColorSpaceTest::TearDown() {}

/**
 * @tc.name: InitWithSRGB001
 * @tc.desc: Test InitWithSRGB
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaColorSpaceTest, InitWithSRGB001, TestSize.Level1)
{
    SkiaColorSpace skiaColorSpace;
    skiaColorSpace.InitWithSRGB();
    EXPECT_TRUE(skiaColorSpace.GetColorSpace() != nullptr);
}

/**
 * @tc.name: InitWithSRGBLinear001
 * @tc.desc: Test InitWithSRGBLinear
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaColorSpaceTest, InitWithSRGBLinear001, TestSize.Level1)
{
    SkiaColorSpace skiaColorSpace;
    skiaColorSpace.InitWithSRGBLinear();
    EXPECT_TRUE(skiaColorSpace.GetColorSpace() != nullptr);
}

/**
 * @tc.name: InitWithImage001
 * @tc.desc: Test InitWithImage
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaColorSpaceTest, InitWithImage001, TestSize.Level1)
{
    Bitmap bmp;
    BitmapFormat format { COLORTYPE_RGBA_8888, ALPHATYPE_OPAQUE };
    bmp.Build(10, 10, format); // 10: width, height
    Image image;
    image.BuildFromBitmap(bmp);
    SkiaColorSpace skiaColorSpace;
    skiaColorSpace.InitWithImage(image);
    EXPECT_TRUE(skiaColorSpace.GetColorSpace() == nullptr);
}

/**
 * @tc.name: GetColorSpace001
 * @tc.desc: Test GetColorSpace
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaColorSpaceTest, GetColorSpace001, TestSize.Level1)
{
    SkiaColorSpace skiaColorSpace;
    EXPECT_TRUE(skiaColorSpace.GetColorSpace() == nullptr);
}

/**
 * @tc.name: GetSkColorSpace001
 * @tc.desc: Test GetSkColorSpace
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaColorSpaceTest, GetSkColorSpace001, TestSize.Level1)
{
    SkiaColorSpace skiaColorSpace;
    EXPECT_TRUE(skiaColorSpace.GetSkColorSpace() == nullptr);
}

/**
 * @tc.name: Deserialize001
 * @tc.desc: Test Deserialize
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaColorSpaceTest, Deserialize001, TestSize.Level1)
{
    SkiaColorSpace skiaColorSpace;
    EXPECT_FALSE(skiaColorSpace.Deserialize(nullptr));
}

/**
 * @tc.name: IsSRGB001
 * @tc.desc: Test IsSRGB
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorSpaceTest, IsSRGB001, TestSize.Level1)
{
    SkiaColorSpace skiaColorSpace;
    skiaColorSpace.InitWithSRGB();
    EXPECT_TRUE(skiaColorSpace.IsSRGB());
}

/**
 * @tc.name: Equals001
 * @tc.desc: Test Equals
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SkiaColorSpaceTest, Equals001, TestSize.Level1)
{
    std::shared_ptr<ColorSpaceImpl> colorSpaceImpl = std::make_shared<SkiaColorSpace>();
    colorSpaceImpl->InitWithSRGB();
    std::shared_ptr<ColorSpace> colorSpace = std::make_shared<ColorSpace>();
    colorSpace->GetImpl<SkiaColorSpace>()->SetColorSpace(colorSpaceImpl->GetSkColorSpace());

    SkiaColorSpace skiaColorSpace;
    skiaColorSpace.InitWithSRGB();
    EXPECT_TRUE(skiaColorSpace.Equals(colorSpace));
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS