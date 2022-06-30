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

#include "effect/color_space.h"
#include "image/image.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class ColorSpaceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void ColorSpaceTest::SetUpTestCase() {}
void ColorSpaceTest::TearDownTestCase() {}
void ColorSpaceTest::SetUp() {}
void ColorSpaceTest::TearDown() {}

/**
 * @tc.name: CreateAndDestroy001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorSpaceTest, CreateAndDestroy001, TestSize.Level1)
{
    auto colorSpace = ColorSpace::CreateSRGB();
    EXPECT_FALSE(colorSpace == nullptr);
}

/**
 * @tc.name: CreatedByStaticMethod001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorSpaceTest, CreatedByStaticMethod001, TestSize.Level1)
{
    auto colorSpace = ColorSpace::CreateSRGBLinear();
    EXPECT_FALSE(colorSpace == nullptr);
}

/**
 * @tc.name: CreatedByStaticMethod003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorSpaceTest, CreatedByStaticMethod003, TestSize.Level1)
{
    Bitmap bmp;
    BitmapFormat format { COLORTYPE_RGBA_8888, ALPHATYPE_OPAQUE };
    bmp.Build(10, 10, format);
    Image image;
    image.BuildFromBitmap(bmp);
    auto colorSpace = ColorSpace::CreateRefImage(image);
    EXPECT_FALSE(colorSpace == nullptr);
}

/**
 * @tc.name: CreateAndGetType001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorSpaceTest, CreateAndGetType001, TestSize.Level1)
{
    auto colorSpace = std::make_unique<ColorSpace>(ColorSpace::ColorSpaceType::NO_TYPE);
    ASSERT_TRUE(colorSpace != nullptr);
    auto type = colorSpace->GetType();
    EXPECT_EQ(type, ColorSpace::ColorSpaceType::NO_TYPE);
}

/**
 * @tc.name: CreateAndGetType002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorSpaceTest, CreateAndGetType002, TestSize.Level1)
{
    auto colorSpace = std::make_unique<ColorSpace>(ColorSpace::ColorSpaceType::REF_IMAGE);
    ASSERT_TRUE(colorSpace != nullptr);
    auto type = colorSpace->GetType();
    EXPECT_EQ(type, ColorSpace::ColorSpaceType::REF_IMAGE);
}

/**
 * @tc.name: CreatedBy2Args001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorSpaceTest, CreatedByArgs001, TestSize.Level1)
{
    auto colorSpace = std::make_unique<ColorSpace>(ColorSpace::ColorSpaceType::REF_IMAGE);
    EXPECT_TRUE(colorSpace != nullptr);
}

/**
 * @tc.name: CreatedBy2Args002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorSpaceTest, CreatedByArgs002, TestSize.Level1)
{
    auto colorSpace = std::make_unique<ColorSpace>(ColorSpace::ColorSpaceType::NO_TYPE);
    EXPECT_TRUE(colorSpace != nullptr);
}

/**
 * @tc.name: CreatedByNoArgs002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorSpaceTest, CreatedByNoArgs003, TestSize.Level1)
{
    Bitmap bmp;
    BitmapFormat format { COLORTYPE_RGBA_8888, ALPHATYPE_OPAQUE };
    bmp.Build(10, 10, format);
    Image image;
    image.BuildFromBitmap(bmp);
    ColorSpace::ColorSpaceType colorSpaceType = ColorSpace::ColorSpaceType::SRGB_LINEAR;
    auto colorSpace = std::make_unique<ColorSpace>(colorSpaceType, image);
    EXPECT_TRUE(colorSpace != nullptr);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
