/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>
#include "effect_image_render.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class EffectImageFilterUnittest : public testing::Test {
public:
    static void SetUpTestCase() {};
    static void TearDownTestCase() {};
    void SetUp() override {};
    void TearDown() override {};
};

class EffectImageRenderUnittest : public testing::Test {
public:
    static void SetUpTestCase() {};
    static void TearDownTestCase() {};
    void SetUp() override {};
    void TearDown() override {};
};

/**
 * @tc.name: FilterTest001
 * @tc.desc: create a filter
 */
HWTEST_F(EffectImageFilterUnittest, FilterTest001, TestSize.Level1)
{
    auto filter = EffectImageFilter::Blur(0.5);
    EXPECT_TRUE(filter != nullptr);

    filter = EffectImageFilter::Blur(-1);
    EXPECT_TRUE(filter == nullptr);

    filter = EffectImageFilter::Brightness(0.5);
    EXPECT_TRUE(filter != nullptr);

    filter = EffectImageFilter::Brightness(-1);
    EXPECT_TRUE(filter == nullptr);

    filter = EffectImageFilter::Brightness(10);
    EXPECT_TRUE(filter == nullptr);

    filter = EffectImageFilter::Grayscale();
    EXPECT_TRUE(filter != nullptr);

    filter = EffectImageFilter::CreateSDF(64, true);
    EXPECT_TRUE(filter != nullptr);

    filter = EffectImageFilter::CreateSDF(0, false);
    EXPECT_TRUE(filter != nullptr);

    filter = EffectImageFilter::Invert();
    EXPECT_TRUE(filter != nullptr);

    Drawing::ColorMatrix matrix;
    filter = EffectImageFilter::ApplyColorMatrix(matrix);
    EXPECT_TRUE(filter != nullptr);
}

/**
 * @tc.name: ApplyTest001
 * @tc.desc: apply drawing
 */
HWTEST_F(EffectImageFilterUnittest, ApplyTest001, TestSize.Level1)
{
    auto filter = EffectImageFilter::Brightness(0.5);
    EXPECT_TRUE(filter != nullptr);
    auto ret = filter->Apply(nullptr);
    EXPECT_TRUE(ret != DrawingError::ERR_OK);

    auto image = std::make_shared<EffectImageChain>();
    EXPECT_TRUE(image != nullptr);
    ret = filter->Apply(image);
    EXPECT_TRUE(ret == DrawingError::ERR_OK);

    auto filterBlur = EffectImageFilter::Blur(0.5);
    EXPECT_TRUE(filterBlur != nullptr);
    ret = filterBlur->Apply(nullptr);
    EXPECT_TRUE(ret != DrawingError::ERR_OK);

    std::vector<float> positions = {0.0f, 0.5f};
    std::vector<float> degrees = {1.0f, 1.0f};
    auto filterEllipticalGradientBlur =
        EffectImageFilter::EllipticalGradientBlur(1.0f, 0.0f, 0.0f, 1.0f, 1.0f, positions, degrees);
    EXPECT_TRUE(filterEllipticalGradientBlur != nullptr);
    ret = filterEllipticalGradientBlur->Apply(nullptr);
    EXPECT_TRUE(ret != DrawingError::ERR_OK);
    image = std::make_shared<EffectImageChain>();
    EXPECT_TRUE(image != nullptr);
}

/**
 * @tc.name: ApplyTest002
 * @tc.desc: apply sdf creation
 */
HWTEST_F(EffectImageFilterUnittest, ApplyTest002, TestSize.Level1)
{
    auto filter = EffectImageFilter::CreateSDF(64, true);
    EXPECT_TRUE(filter != nullptr);
    auto ret = filter->Apply(nullptr);
    EXPECT_TRUE(ret != DrawingError::ERR_OK);

    auto image = std::make_shared<EffectImageChain>();
    EXPECT_TRUE(image != nullptr);
    ret = filter->Apply(image);
    EXPECT_TRUE(ret != DrawingError::ERR_OK); // image not prepared
}

/**
 * @tc.name: RenderTest001
 * @tc.desc: Test Render
 */
HWTEST_F(EffectImageRenderUnittest, RenderTest001, TestSize.Level1)
{
    Media::InitializationOptions opts;
    opts.size ={ 1, 1 };
    auto uniPixelMap = Media::PixelMap::Create(opts);
    std::shared_ptr<Media::PixelMap> srcPixelMap(std::move(uniPixelMap));

    std::vector<std::shared_ptr<EffectImageFilter>> imageFilter;
    imageFilter.emplace_back(nullptr);
    auto filterBlur = EffectImageFilter::Blur(0.5);
    EXPECT_TRUE(filterBlur != nullptr);
    imageFilter.emplace_back(filterBlur);

    std::shared_ptr<Media::PixelMap> dstPixelMap = nullptr;
    EffectImageRender imageRender;
    auto ret = imageRender.Render(srcPixelMap, imageFilter, true, dstPixelMap);
    ASSERT_EQ(ret, DrawingError::ERR_OK);

    ret = imageRender.Render(nullptr, imageFilter, true, dstPixelMap);
    ASSERT_NE(ret, DrawingError::ERR_OK);
}

/**
 * @tc.name: EllipticalGradientBlurApplyTest
 * @tc.desc: Test EllipticalGradientBlur filter application
 */
HWTEST_F(EffectImageRenderUnittest, EllipticalGradientBlurApplyTest, TestSize.Level1)
{
    std::vector<float> positions = {0.0f, 1.0f};
    std::vector<float> degrees = {0.5f, 1.0f};

    Media::InitializationOptions opts;
    opts.size = {1, 1};
    auto uniPixelMap = Media::PixelMap::Create(opts);
    std::shared_ptr<Media::PixelMap> srcPixelMap(std::move(uniPixelMap));

    std::shared_ptr<Media::PixelMap> dstPixelMap = nullptr;
    std::vector<std::shared_ptr<EffectImageFilter>> imageFilter;
    imageFilter.emplace_back(nullptr);
    EffectImageRender imageRender;
    auto ret = imageRender.Render(srcPixelMap, imageFilter, false, dstPixelMap);
    EXPECT_EQ(ret, DrawingError::ERR_OK);

    auto filterBlur = EffectImageFilter::EllipticalGradientBlur(
        1.0f, 0.0f, 0.0f, 1.0f, 1.0f, positions, degrees);
    EXPECT_TRUE(filterBlur != nullptr);
    imageFilter.emplace_back(filterBlur);

    ret = imageRender.Render(nullptr, imageFilter, false, dstPixelMap);
    ASSERT_NE(ret, DrawingError::ERR_OK);
}

/**
 * @tc.name: DirectionBlurTest
 * @tc.desc: Test Render widht direction blur
* @tc.type: FUNC
 */
HWTEST_F(EffectImageRenderUnittest, DirectionBlurTest, TestSize.Level1)
{
    float radius = -1.5f; // invalid value
    float angle = 0.0;
    auto flag = EffectImageFilter::Blur(radius, angle);
    EXPECT_EQ(flag, nullptr);

    radius = 1.5f; // valid value
    flag = EffectImageFilter::Blur(radius, angle);
    EXPECT_NE(flag, nullptr);
}


/**
 * @tc.name: ColorRemapTest
 * @tc.desc: Test ColorRemap by invalid params
 * @tc.type: FUNC
 */
HWTEST_F(EffectImageRenderUnittest, ColorRemapTest, TestSize.Level1)
{
    std::vector<Vector4f> colors = {};
    std::vector<float> positions = {};
    auto flag = EffectImageFilter::ColorRemap(colors, positions);
    EXPECT_EQ(flag, nullptr);

    Vector4f color1 = {1.0f, 0.0f, 0.5f, 1.0f}; // color rgba
    Vector4f color2 = {1.0f, 0.5f, 0.5f, 1.0f}; // color rgba
    Vector4f color3 = {0.0f, 0.5f, 0.5f, 1.0f}; // color rgba
    colors = {color1, color3, color3};
    flag = EffectImageFilter::ColorRemap(colors, positions);
    EXPECT_EQ(flag, nullptr);

    positions = {0.2f, -0.5f, 1.5f};
    flag = EffectImageFilter::ColorRemap(colors, positions);
    EXPECT_NE(flag, nullptr);

    colors = {};
    flag = EffectImageFilter::ColorRemap(colors, positions);
    EXPECT_EQ(flag, nullptr);
}

/**
 * @tc.name: EffectImageColorRemapFilterTest
 * @tc.desc: Test EffectImageColorRemapFilter
 * @tc.type: FUNC
 */
HWTEST_F(EffectImageRenderUnittest, EffectImageColorRemapFilterTest, TestSize.Level1)
{
    std::vector<Vector4f> colors = {{1.0f, 0.0f, 0.5f, 1.0f}}; // color rgba
    std::vector<float> positions = {0.5};
    auto filterColorRemap = EffectImageFilter::ColorRemap(colors, positions);
    ASSERT_NE(filterColorRemap, nullptr);

    std::shared_ptr<EffectImageChain> image = nullptr;
    auto ret = filterColorRemap->Apply(nullptr);
    EXPECT_NE(ret, DrawingError::ERR_OK);

    image = std::make_shared<EffectImageChain>();
    ret = filterColorRemap->Apply(image);
    EXPECT_NE(ret, DrawingError::ERR_OK);
}

/**
 * @tc.name: GammaCorrectionTest
 * @tc.desc: Test GammaCorrection
 * @tc.type: FUNC
 */
HWTEST_F(EffectImageRenderUnittest, GammaCorrectionTest, TestSize.Level1)
{
    float gamma = -1.5f; // invalid value
    auto flag = EffectImageFilter::GammaCorrection(gamma);
    EXPECT_EQ(flag, nullptr);

    gamma = 0.0f; // boundary value
    flag = EffectImageFilter::GammaCorrection(gamma);
    EXPECT_EQ(flag, nullptr);

    gamma = 1.5f; // valid value
    flag = EffectImageFilter::GammaCorrection(gamma);
    EXPECT_NE(flag, nullptr);
}

/**
 * @tc.name: EffectImageGammaCorrectionFilterTest
 * @tc.desc: Test EffectImageGammaCorrectionFilter
 * @tc.type: FUNC
 */
HWTEST_F(EffectImageRenderUnittest, EffectImageGammaCorrectionFilterTest, TestSize.Level1)
{
    float gamma = 1.5f; // valid value
    auto filter = EffectImageFilter::GammaCorrection(gamma);
    ASSERT_NE(filter, nullptr);

    std::shared_ptr<EffectImageChain> image = nullptr;
    auto ret = filter->Apply(nullptr);
    EXPECT_NE(ret, DrawingError::ERR_OK);

    image = std::make_shared<EffectImageChain>();
    ret = filter->Apply(image);
    EXPECT_NE(ret, DrawingError::ERR_OK);
}
} // namespace Rosen
} // namespace OHOS