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
#include "effect/color_filter.h"
#include "image/bitmap.h"
#include "draw/color.h"
#include "include/effects/SkImageFilters.h"
#include "skia_adapter/skia_image_filter.h"
#include "utils/sampling_options.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaImageFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SkiaImageFilterTest::SetUpTestCase() {}
void SkiaImageFilterTest::TearDownTestCase() {}
void SkiaImageFilterTest::SetUp() {}
void SkiaImageFilterTest::TearDown() {}

/**
 * @tc.name: InitWithBlur001
 * @tc.desc: Test InitWithBlur
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaImageFilterTest, InitWithBlur001, TestSize.Level1)
{
    std::shared_ptr<SkiaImageFilter> skiaImageFilter = std::make_shared<SkiaImageFilter>();
    Rect noCropRect = {
        -std::numeric_limits<scalar>::infinity(), -std::numeric_limits<scalar>::infinity(),
        std::numeric_limits<scalar>::infinity(), std::numeric_limits<scalar>::infinity()
    };
    skiaImageFilter->InitWithBlur(
        5.0f, 5.0f, TileMode::REPEAT, nullptr, ImageBlurType::GAUSS, noCropRect); // 5.0f: sigmaX and sigmaY
    EXPECT_TRUE(skiaImageFilter->GetImageFilter() != nullptr);
}

/**
 * @tc.name: InitWithBlur002
 * @tc.desc: Test InitWithBlur
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaImageFilterTest, InitWithBlur002, TestSize.Level1)
{
    std::shared_ptr<SkiaImageFilter> skiaImageFilter = std::make_shared<SkiaImageFilter>();
    Rect noCropRect = {
        -std::numeric_limits<scalar>::infinity(), -std::numeric_limits<scalar>::infinity(),
        std::numeric_limits<scalar>::infinity(), std::numeric_limits<scalar>::infinity()
    };
    skiaImageFilter->InitWithBlur(
        5.0f, 5.0f, TileMode::MIRROR, nullptr, ImageBlurType::GAUSS, noCropRect); // 5.0f: sigmaX and sigmaY
    EXPECT_TRUE(skiaImageFilter->GetImageFilter() != nullptr);
}

/**
 * @tc.name: InitWithBlur003
 * @tc.desc: Test InitWithBlur
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaImageFilterTest, InitWithBlur003, TestSize.Level1)
{
    std::shared_ptr<SkiaImageFilter> skiaImageFilter = std::make_shared<SkiaImageFilter>();
    Rect noCropRect = {
        -std::numeric_limits<scalar>::infinity(), -std::numeric_limits<scalar>::infinity(),
        std::numeric_limits<scalar>::infinity(), std::numeric_limits<scalar>::infinity()
    };
    skiaImageFilter->InitWithBlur(
        5.0f, 5.0f, TileMode::DECAL, nullptr, ImageBlurType::GAUSS, noCropRect); // 5.0f: sigmaX and sigmaY
    EXPECT_TRUE(skiaImageFilter->GetImageFilter() != nullptr);
}

/**
 * @tc.name: Serialize001
 * @tc.desc: Test Serialize
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaImageFilterTest, Serialize001, TestSize.Level1)
{
    std::shared_ptr<SkiaImageFilter> skiaImageFilter = std::make_shared<SkiaImageFilter>();
    EXPECT_TRUE(skiaImageFilter->GetImageFilter() == nullptr);
    skiaImageFilter->Serialize();
    sk_sp<SkImageFilter> blur(SkImageFilters::Blur(20.0f, 20.0f, nullptr)); // 20.0f: sigmaX and sigmaY
    skiaImageFilter->SetSkImageFilter(blur);
    skiaImageFilter->Serialize();
}

/**
 * @tc.name: Deserialize001
 * @tc.desc: Test Deserialize
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaImageFilterTest, Deserialize001, TestSize.Level1)
{
    std::shared_ptr<SkiaImageFilter> skiaImageFilter = std::make_shared<SkiaImageFilter>();
    EXPECT_TRUE(skiaImageFilter->GetImageFilter() == nullptr);
    skiaImageFilter->Deserialize(nullptr);
    std::shared_ptr<Data> data = std::make_shared<Data>();
    skiaImageFilter->Deserialize(data);
}


/**
 * @tc.name: InitWithBitmap001
 * @tc.desc: Test InitWithBitmap
 * @tc.type: FUNC
 * @tc.require: IAZ845
 */
HWTEST_F(SkiaImageFilterTest, InitWithImage001, TestSize.Level1)
{
    int32_t width = 200;
    int32_t height = 200;
    std::shared_ptr<Image> image = nullptr;
    Rect rect(0.0f, 0.0f, width, height);
    std::shared_ptr<SkiaImageFilter> skiaImageFilter = std::make_shared<SkiaImageFilter>();

    BitmapFormat bitmapFormat = { ColorType::COLORTYPE_BGRA_8888, AlphaType::ALPHATYPE_PREMUL };
    Bitmap bitmap;
    bitmap.Build(width, height, bitmapFormat);
    image = bitmap.MakeImage();
    EXPECT_TRUE(image != nullptr);
    SamplingOptions options(FilterMode::LINEAR);
    skiaImageFilter->InitWithImage(image, rect, rect, options);
    EXPECT_TRUE(skiaImageFilter->filter_ != nullptr);
}

/**
 * @tc.name: AllFunc001
 * @tc.desc: Test AllFunc
 * @tc.type: FUNC
 * @tc.require: IAZ845
 */
HWTEST_F(SkiaImageFilterTest, AllFunc001, TestSize.Level1)
{
    int32_t width = 200;
    int32_t height = 200;
    scalar sigmaX = 5.0f;
    scalar sigmaY = 5.0f;
    Rect rect(0.0f, 0.0f, width, height);
    std::shared_ptr<SkiaImageFilter> skiaImageFilter = std::make_shared<SkiaImageFilter>();
    skiaImageFilter->InitWithBlur(
        sigmaX, sigmaX, TileMode::REPEAT, nullptr, ImageBlurType::GAUSS, rect);
    EXPECT_TRUE(skiaImageFilter->GetImageFilter() != nullptr);

    auto colorFilter = ColorFilter::CreateSrgbGammaToLinear();
    skiaImageFilter->InitWithColor(*colorFilter, nullptr, rect);
    EXPECT_TRUE(skiaImageFilter->GetImageFilter() != nullptr);

    skiaImageFilter->InitWithOffset(sigmaX, sigmaY, nullptr, rect);
    EXPECT_TRUE(skiaImageFilter->GetImageFilter() != nullptr);

    skiaImageFilter->InitWithColorBlur(*colorFilter, sigmaX, sigmaY, ImageBlurType::GAUSS, rect);
    EXPECT_TRUE(skiaImageFilter->GetImageFilter() != nullptr);
    
    std::vector<scalar> coefficients = {1.0f, 2.0f, 3.0f, 4.0f};
    skiaImageFilter->InitWithArithmetic(coefficients, true, nullptr, nullptr, rect);
    EXPECT_TRUE(skiaImageFilter->GetImageFilter() != nullptr);

    skiaImageFilter->InitWithBlend(BlendMode::SRC, rect, nullptr, nullptr);
    EXPECT_TRUE(skiaImageFilter->GetImageFilter() != nullptr);

    skiaImageFilter->InitWithShader(nullptr, rect);
    EXPECT_TRUE(skiaImageFilter->GetImageFilter() != nullptr);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS