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
#include "surface_buffer.h"
#include "effect_image_render.h"
#include "ge_linear_gradient_shader_mask.h"

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
 * @tc.name: RenderTest002
 * @tc.desc: Test Render
 */
HWTEST_F(EffectImageRenderUnittest, RenderTest002, TestSize.Level1)
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
 * @tc.name: RenderTest003
 * @tc.desc: Test Render
 */
HWTEST_F(EffectImageRenderUnittest, RenderTest003, TestSize.Level1)
{
    std::vector<std::shared_ptr<EffectImageFilter>> imageFilter;
    imageFilter.emplace_back(nullptr);
    auto filterBlur = EffectImageFilter::Blur(0.5);
    EXPECT_TRUE(filterBlur != nullptr);
    imageFilter.emplace_back(filterBlur);

    const auto width = 200;
    const auto height = 200;

    OHOS::Media::InitializationOptions opts = {
        .size =
            {
                .width = static_cast<int32_t>(width),
                .height = static_cast<int32_t>(height),
            },
        .srcPixelFormat = OHOS::Media::PixelFormat::RGBA_8888,
        .pixelFormat = OHOS::Media::PixelFormat::RGBA_8888,
        .alphaType = OHOS::Media::AlphaType::IMAGE_ALPHA_TYPE_PREMUL,
    };
    std::shared_ptr<Media::PixelMap> srcPixelMap = Media::PixelMap::Create(opts);
    OH_NativeBuffer_Config config {
        .width = width,
        .height = height,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA
    };
    OH_NativeBuffer* dstBuffer = OH_NativeBuffer_Alloc(&config);
    std::shared_ptr<OH_NativeBuffer> dst(
        dstBuffer,
        [](OH_NativeBuffer* buffer) {}
    );
    EffectImageRender imageRender;
    auto ret = imageRender.RenderDstNative(srcPixelMap, dst, imageFilter, false);
    ASSERT_EQ(ret, DrawingError::ERR_OK);
    ret = imageRender.RenderDstNative(nullptr, dst, imageFilter, false);
    ASSERT_NE(ret, DrawingError::ERR_OK);
    std::shared_ptr<OH_NativeBuffer> nullBuffer = nullptr;
    ret = imageRender.RenderDstNative(srcPixelMap, nullBuffer, imageFilter, false);
    ASSERT_NE(ret, DrawingError::ERR_OK);
    ret = imageRender.RenderDstNative(srcPixelMap, nullBuffer, imageFilter, true);
    ASSERT_NE(ret, DrawingError::ERR_OK);
    ret = imageRender.RenderDstNative(srcPixelMap, dst, imageFilter, true);
    ASSERT_NE(ret, DrawingError::ERR_OK);
    OH_NativeBuffer_Unreference(dstBuffer);
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
 * @tc.name: MapColorByBrightnessTest
 * @tc.desc: Test MapColorByBrightness by invalid params
 * @tc.type: FUNC
 */
HWTEST_F(EffectImageRenderUnittest, MapColorByBrightnessTest, TestSize.Level1)
{
    std::vector<Vector4f> colors = {};
    std::vector<float> positions = {};
    auto flag = EffectImageFilter::MapColorByBrightness(colors, positions);
    EXPECT_EQ(flag, nullptr);

    Vector4f color1 = {1.0f, 0.0f, 0.5f, 1.0f}; // color rgba
    Vector4f color2 = {1.0f, 0.5f, 0.5f, 1.0f}; // color rgba
    Vector4f color3 = {0.0f, 0.5f, 0.5f, 1.0f}; // color rgba
    colors = {color1, color2, color3};
    flag = EffectImageFilter::MapColorByBrightness(colors, positions);
    EXPECT_EQ(flag, nullptr);

    positions = {0.2f, -0.5f, 1.5f};
    flag = EffectImageFilter::MapColorByBrightness(colors, positions);
    EXPECT_NE(flag, nullptr);

    colors = {};
    flag = EffectImageFilter::MapColorByBrightness(colors, positions);
    EXPECT_EQ(flag, nullptr);
}

/**
 * @tc.name: MapColorByBrightnessTestNotEqual
 * @tc.desc: Test MapColorByBrightness by colors and positions are not equal
 * @tc.type: FUNC
 */
HWTEST_F(EffectImageRenderUnittest, MapColorByBrightnessTestNotEqual, TestSize.Level1)
{
    Vector4f color1 = {1.0f, 0.0f, 0.5f, 1.0f}; // color rgba
    Vector4f color2 = {1.0f, 0.5f, 0.5f, 1.0f}; // color rgba
    std::vector<Vector4f> colors = {color1, color2};
    std::vector<float> positions = {0.2f, 0.5f, 1.0f};
    auto flag = EffectImageFilter::MapColorByBrightness(colors, positions);
    EXPECT_EQ(flag, nullptr);
}

/**
 * @tc.name: EffectImageMapColorByBrightnessFilterTest
 * @tc.desc: Test EffectImageMapColorByBrightnessFilter
 * @tc.type: FUNC
 */
HWTEST_F(EffectImageRenderUnittest, EffectImageMapColorByBrightnessFilterTest, TestSize.Level1)
{
    std::vector<Vector4f> colors = {{1.0f, 0.0f, 0.5f, 1.0f}}; // color rgba
    std::vector<float> positions = {0.5};
    auto filter = EffectImageFilter::MapColorByBrightness(colors, positions);
    ASSERT_NE(filter, nullptr);

    std::shared_ptr<EffectImageChain> image = nullptr;
    auto ret = filter->Apply(nullptr);
    EXPECT_NE(ret, DrawingError::ERR_OK);

    image = std::make_shared<EffectImageChain>();
    ret = filter->Apply(image);
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

/**
 * @tc.name: MaskTransitionFilterTest001
 * @tc.desc: Test EffectImageFilter::MaskTransition factory method
 */
HWTEST_F(EffectImageFilterUnittest, MaskTransitionFilterTest001, TestSize.Level1)
{
    Media::InitializationOptions opts;
    opts.size = {100, 100};
    auto uniPixelMap = Media::PixelMap::Create(opts);
    std::shared_ptr<Media::PixelMap> topLayerMap(std::move(uniPixelMap));
    ASSERT_NE(topLayerMap, nullptr);

    std::vector<std::pair<float, float>> fractionStops = {{0.0f, 0.0f}, {1.0f, 1.0f}};
    Drawing::GELinearGradientShaderMaskParams maskParams{
        fractionStops, Drawing::Point(0, 0), Drawing::Point(100, 100)};
    auto mask = std::static_pointer_cast<Drawing::GEShaderMask>(
        std::make_shared<Drawing::GELinearGradientShaderMask>(maskParams));

    auto filter = EffectImageFilter::MaskTransition(topLayerMap, mask, 0.5f, false);
    EXPECT_TRUE(filter != nullptr);

    // Test with null image
    auto ret = filter->Apply(nullptr);
    EXPECT_TRUE(ret != DrawingError::ERR_OK);

    // Test with valid image (but not prepared)
    auto image = std::make_shared<EffectImageChain>();
    ret = filter->Apply(image);
    // Not prepared, so should rteurn error
    EXPECT_TRUE(ret != DrawingError::ERR_OK);
}

/**
 * @tc.name: MaskTransitionFilterTest002
 * @tc.desc: Test EffectImageFilter::MaskTransition with different parameters
 */
HWTEST_F(EffectImageFilterUnittest, MaskTransitionFilterTest002, TestSize.Level1)
{
    Media::InitializationOptions opts;
    opts.size = {100, 100};
    auto uniPixelMap = Media::PixelMap::Create(opts);
    std::shared_ptr<Media::PixelMap> topLayerMap(std::move(uniPixelMap));
    ASSERT_NE(topLayerMap, nullptr);

    std::vector<std::pair<float, float>> fractionStops = {{0.0f, 0.0f}, {1.0f, 1.0f}};
    Drawing::GELinearGradientShaderMaskParams maskParams{
        fractionStops, Drawing::Point(0, 0), Drawing::Point(100, 100)};
    auto mask = std::static_pointer_cast<Drawing::GEShaderMask>(
        std::make_shared<Drawing::GELinearGradientShaderMask>(maskParams));

    // Test with inverse = true
    auto filter = EffectImageFilter::MaskTransition(topLayerMap, mask, 0.8f, true);
    EXPECT_TRUE(filter != nullptr);

    // Test with factor at boundaries
    filter = EffectImageFilter::MaskTransition(topLayerMap, mask, 0.0f, false);
    EXPECT_TRUE(filter != nullptr);

    filter = EffectImageFilter::MaskTransition(topLayerMap, mask, 1.0f, false);
    EXPECT_TRUE(filter != nullptr);
}

/**
 * @tc.name: WaterDropletTransitionFilterTest001
 * @tc.desc: Test EffectImageFilter::WaterDropletTransition factory method
 */
HWTEST_F(EffectImageFilterUnittest, WaterDropletTransitionFilterTest001, TestSize.Level1)
{
    Media::InitializationOptions opts;
    opts.size = {100, 100};
    auto uniPixelMap = Media::PixelMap::Create(opts);
    std::shared_ptr<Media::PixelMap> topLayerMap(std::move(uniPixelMap));
    ASSERT_NE(topLayerMap, nullptr);

    Drawing::GEWaterDropletTransitionFilterParams params = {
        nullptr, false, 0.5f, 5.0f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f
    };
    auto waterDropletParams = std::make_shared<Drawing::GEWaterDropletTransitionFilterParams>(params);

    auto filter = EffectImageFilter::WaterDropletTransition(topLayerMap, waterDropletParams);
    EXPECT_TRUE(filter != nullptr);

    // Test with null image
    auto ret = filter->Apply(nullptr);
    EXPECT_TRUE(ret != DrawingError::ERR_OK);

    // Test with valid image (but not prepared)
    auto image = std::make_shared<EffectImageChain>();
    ret = filter->Apply(image);
    // Not prepared, so should return error
    EXPECT_TRUE(ret != DrawingError::ERR_OK);
}

/**
 * @tc.name: WaterDropletTransitionFilterTest002
 * @tc.desc: Test EffectImageFilter::WaterDropletTransition with different parameters
 */
HWTEST_F(EffectImageFilterUnittest, WaterDropletTransitionFilterTest002, TestSize.Level1)
{
    Media::InitializationOptions opts;
    opts.size = {100, 100};
    auto uniPixelMap = Media::PixelMap::Create(opts);
    std::shared_ptr<Media::PixelMap> topLayerMap(std::move(uniPixelMap));
    ASSERT_NE(topLayerMap, nullptr);

    // Test with inverse = true
    Drawing::GEWaterDropletTransitionFilterParams params1 = {
        nullptr, true, 0.3f, 3.0f, 0.3f, 0.3f, 0.3f, 0.3f, 0.5f, 0.5f, 0.5f, 0.5f
    };
    auto waterDropletParams1 = std::make_shared<Drawing::GEWaterDropletTransitionFilterParams>(params1);
    auto filter = EffectImageFilter::WaterDropletTransition(topLayerMap, waterDropletParams1);
    EXPECT_TRUE(filter != nullptr);

    // Test with different progress values
    Drawing::GEWaterDropletTransitionFilterParams params2 = {
        nullptr, false, 0.0f, 5.0f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f
    };
    auto waterDropletParams2 = std::make_shared<Drawing::GEWaterDropletTransitionFilterParams>(params2);
    filter = EffectImageFilter::WaterDropletTransition(topLayerMap, waterDropletParams2);
    EXPECT_TRUE(filter != nullptr);

    Drawing::GEWaterDropletTransitionFilterParams params3 = {
        nullptr, false, 1.0f, 5.0f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f
    };
    auto waterDropletParams3 = std::make_shared<Drawing::GEWaterDropletTransitionFilterParams>(params3);
    filter = EffectImageFilter::WaterDropletTransition(topLayerMap, waterDropletParams3);
    EXPECT_TRUE(filter != nullptr);
}

/**
 * @tc.name: ReededGlassMethod
 * @tc.desc: Test ReededGlass
 */
HWTEST_F(EffectImageRenderUnittest, ReededGlassMethod, TestSize.Level1)
{
    auto params = std::make_shared<Drawing::GEReededGlassDataParams>();
    ASSERT_NE(params, nullptr);
    auto filter = EffectImageFilter::ReededGlass(params);
    ASSERT_NE(filter, nullptr);

    // Verify that the filter is of the correct type
    ASSERT_NE(std::static_pointer_cast<EffectImageReededGlassFilter>(filter), nullptr);
}

/**
 * @tc.name: WaterGlassMethod
 * @tc.desc: Test WaterGlass
 */
HWTEST_F(EffectImageRenderUnittest, WaterGlassMethod, TestSize.Level1)
{
    auto params = std::make_shared<Drawing::GEWaterGlassDataParams>();
    ASSERT_NE(params, nullptr);
    auto filter = EffectImageFilter::WaterGlass(params);
    ASSERT_NE(filter, nullptr);

    // Verify that the filter is of the correct type
    ASSERT_NE(std::static_pointer_cast<EffectImageWaterGlassFilter>(filter), nullptr);
}

/**
 * @tc.name: ApplyrReededGlassMethod
 * @tc.desc: Test EffectImageReededGlassFilter filter application
 */
HWTEST_F(EffectImageRenderUnittest, ApplyrReededGlassMethod, TestSize.Level1)
{
    std::shared_ptr<Drawing::GEReededGlassDataParams> params = std::make_shared<Drawing::GEReededGlassDataParams>();
    ASSERT_NE(params, nullptr);
    std::shared_ptr<EffectImageReededGlassFilter> filter = std::make_shared<EffectImageReededGlassFilter>(params);
    ASSERT_NE(filter, nullptr);

    auto image = std::make_shared<EffectImageChain>();
    image->prepared_ = true;
    Media::InitializationOptions opts;
    opts.size = { 1, 1 };
    std::shared_ptr<Media::PixelMap> srcPixelMap(Media::PixelMap::Create(opts));
    ASSERT_NE(srcPixelMap, nullptr);
    image->Prepare(srcPixelMap, false);

    DrawingError result = filter->Apply(image);
    ASSERT_NE(result, DrawingError::ERR_OK);
}

/**
 * @tc.name: ApplyMethod
 * @tc.desc: Test EffectImageReededGlassFilter filter application, image is null
 */
HWTEST_F(EffectImageRenderUnittest, ApplyrMethod, TestSize.Level1)
{
    std::shared_ptr<Drawing::GEReededGlassDataParams> params = std::make_shared<Drawing::GEReededGlassDataParams>();
    ASSERT_NE(params, nullptr);
    std::shared_ptr<EffectImageReededGlassFilter> filter = std::make_shared<EffectImageReededGlassFilter>(params);
    ASSERT_NE(filter, nullptr);

    DrawingError result = filter->Apply(nullptr);
    ASSERT_EQ(result, DrawingError::ERR_IMAGE_NULL);
}

/**
 * @tc.name: ApplyWaterGlassMethod
 * @tc.desc: Test EffectImageWaterGlassFilter filter application
 */
HWTEST_F(EffectImageRenderUnittest, ApplyWaterGlassMethod, TestSize.Level1)
{
    std::shared_ptr<Drawing::GEWaterGlassDataParams> params = std::make_shared<Drawing::GEWaterGlassDataParams>();
    ASSERT_NE(params, nullptr);

    std::shared_ptr<EffectImageWaterGlassFilter> filter = std::make_shared<EffectImageWaterGlassFilter>(params);
    ASSERT_NE(filter, nullptr);

    auto image = std::make_shared<EffectImageChain>();
    image->prepared_ = true;
    Media::InitializationOptions opts;
    opts.size = { 1, 1 };
    std::shared_ptr<Media::PixelMap> srcPixelMap(Media::PixelMap::Create(opts));
    ASSERT_NE(srcPixelMap, nullptr);
    image->Prepare(srcPixelMap, false);
    DrawingError result = filter->Apply(image);
    ASSERT_NE(result, DrawingError::ERR_OK);
}

/**
 * @tc.name: ApplyMethod
 * @tc.desc: Test EffectImageWaterGlassFilter filter application, image is null
 */
HWTEST_F(EffectImageRenderUnittest, ApplyMethod, TestSize.Level1)
{
    std::shared_ptr<Drawing::GEWaterGlassDataParams> params = std::make_shared<Drawing::GEWaterGlassDataParams>();
    ASSERT_NE(params, nullptr);
    std::shared_ptr<EffectImageWaterGlassFilter> filter = std::make_shared<EffectImageWaterGlassFilter>(params);
    ASSERT_NE(filter, nullptr);

    DrawingError result = filter->Apply(nullptr);
    ASSERT_EQ(result, DrawingError::ERR_IMAGE_NULL);
}

/**
* @tc.desc: Test Blur filter with boundary values
* @tc.type: FUNC
*/
HWTEST_F(EffectImageFilterUnittest, BlurFilterBoundaryTest, TestSize.Level1)
{
    // Test with zero radius
    auto filter = EffectImageFilter::Blur(0.0f);
    EXPECT_NE(filter, nullptr);

    // Test with very small radius
    filter = EffectImageFilter::Blur(0.001f);
    EXPECT_NE(filter, nullptr);

    // Test with large radius
    filter = EffectImageFilter::Blur(100.0f);
    EXPECT_NE(filter, nullptr);

    // Test with negative radius
    filter = EffectImageFilter::Blur(-0.5f);
    EXPECT_EQ(filter, nullptr);

    // Test with very large radius
    filter = EffectImageFilter::Blur(10000.0f);
    EXPECT_NE(filter, nullptr);
}

/**
* @tc.name: BrightnessFilterBoundaryTest
* @tc.desc: Test Brightness filter with boundary values
* @tc.type: FUNC
*/
HWTEST_F(EffectImageFilterUnittest, BrightnessFilterBoundaryTest, TestSize.Level1)
{
    // Test with zero brightness
    auto filter = EffectImageFilter::Brightness(0.0f);
    EXPECT_NE(filter, nullptr);

    // Test with minimum valid value (just above 0)
    filter = EffectImageFilter::Brightness(0.001f);
    EXPECT_NE(filter, nullptr);

    // Test with maximum valid value
    filter = EffectImageFilter::Brightness(9.999f);
    EXPECT_EQ(filter, nullptr);

    // Test with invalid negative value
    filter = EffectImageFilter::Brightness(-0.1f);
    EXPECT_EQ(filter, nullptr);

    // Test with invalid value exceeding maximum
    filter = EffectImageFilter::Brightness(10.1f);
    EXPECT_EQ(filter, nullptr);

    // Test with boundary value 1.0
    filter = EffectImageFilter::Brightness(1.0f);
    EXPECT_NE(filter, nullptr);
}

/**
* @tc.name: MultipleFiltersSequentialApplyTest
* @tc.desc: Test applying multiple filters sequentially
* @tc.type: FUNC
*/
HWTEST_F(EffectImageRenderUnittest, MultipleFiltersSequentialApplyTest, TestSize.Level1)
{
    Media::InitializationOptions opts;
    opts.size = {100, 100};
    auto uniPixelMap = Media::PixelMap::Create(opts);
    std::shared_ptr<Media::PixelMap> srcPixelMap(std::move(uniPixelMap));
    ASSERT_NE(srcPixelMap, nullptr);

    std::vector<std::shared_ptr<EffectImageFilter>> filters;
    filters.emplace_back(EffectImageFilter::Brightness(0.5f));
    filters.emplace_back(EffectImageFilter::Blur(1.0f));
    filters.emplace_back(EffectImageFilter::Grayscale());
    filters.emplace_back(EffectImageFilter::Invert());

    std::shared_ptr<Media::PixelMap> dstPixelMap = nullptr;
    EffectImageRender imageRender;
    auto ret = imageRender.Render(srcPixelMap, filters, false, dstPixelMap);
    ASSERT_EQ(ret, DrawingError::ERR_OK);
}

/**
* @tc.name: GraphicsEngineFilterTest
* @tc.desc: Test various graphics engine filters
* @tc.type: FUNC
*/
HWTEST_F(EffectImageFilterUnittest, GraphicsEngineFilterTest, TestSize.Level1)
{
    // Test Invert filter
    auto invertFilter = EffectImageFilter::Invert();
    EXPECT_NE(invertFilter, nullptr);

    // Test Grayscale filter
    auto grayscaleFilter = EffectImageFilter::Grayscale();
    EXPECT_NE(grayscaleFilter, nullptr);

    // Test ColorMatrix with identity matrix
    Drawing::ColorMatrix identityMatrix;
    auto colorMatrixFilter = EffectImageFilter::ApplyColorMatrix(identityMatrix);
    EXPECT_NE(colorMatrixFilter, nullptr);

    // Test SDF creation with various sizes
    auto sdfFilter = EffectImageFilter::CreateSDF(32, false);
    EXPECT_NE(sdfFilter, nullptr);

    sdfFilter = EffectImageFilter::CreateSDF(128, true);
    EXPECT_NE(sdfFilter, nullptr);

    sdfFilter = EffectImageFilter::CreateSDF(256, false);
    EXPECT_NE(sdfFilter, nullptr);
}

/**
* @tc.name: LargeImageRenderTest
* @tc.desc: Test rendering with large image dimensions
* @tc.type: FUNC
*/
HWTEST_F(EffectImageRenderUnittest, LargeImageRenderTest, TestSize.Level1)
{
    const int32_t width = 2048;
    const int32_t height = 2048;

    Media::InitializationOptions opts = {
        .size = {.width = width, .height = height},
        .srcPixelFormat = OHOS::Media::PixelFormat::RGBA_8888,
        .pixelFormat = OHOS::Media::PixelFormat::RGBA_8888,
    };
    std::shared_ptr<Media::PixelMap> srcPixelMap = Media::PixelMap::Create(opts);
    ASSERT_NE(srcPixelMap, nullptr);

    std::vector<std::shared_ptr<EffectImageFilter>> filters;
    filters.emplace_back(EffectImageFilter::Blur(2.0f));
    filters.emplace_back(EffectImageFilter::Brightness(0.7f));

    std::shared_ptr<Media::PixelMap> dstPixelMap = nullptr;
    EffectImageRender imageRender;
    auto ret = imageRender.Render(srcPixelMap, filters, false, dstPixelMap);
    ASSERT_EQ(ret, DrawingError::ERR_OK);
}

/**
* @tc.name: SmallImageRenderTest
* @tc.desc: Test rendering with very small image dimensions
* @tc.type: FUNC
*/
HWTEST_F(EffectImageRenderUnittest, SmallImageRenderTest, TestSize.Level1)
{
    Media::InitializationOptions opts;
    opts.size = {1, 1};
    auto uniPixelMap = Media::PixelMap::Create(opts);
    std::shared_ptr<Media::PixelMap> srcPixelMap(std::move(uniPixelMap));
    ASSERT_NE(srcPixelMap, nullptr);

    std::vector<std::shared_ptr<EffectImageFilter>> filters;
    filters.emplace_back(EffectImageFilter::Grayscale());
    filters.emplace_back(EffectImageFilter::Invert());

    std::shared_ptr<Media::PixelMap> dstPixelMap = nullptr;
    EffectImageRender imageRender;
    auto ret = imageRender.Render(srcPixelMap, filters, false, dstPixelMap);
    ASSERT_EQ(ret, DrawingError::ERR_OK);
}

/**
* @tc.name: EmptyFilterListTest
* @tc.desc: Test rendering with empty filter list
* @tc.type: FUNC
*/
HWTEST_F(EffectImageRenderUnittest, EmptyFilterListTest, TestSize.Level1)
{
    Media::InitializationOptions opts;
    opts.size = {100, 100};
    auto uniPixelMap = Media::PixelMap::Create(opts);
    std::shared_ptr<Media::PixelMap> srcPixelMap(std::move(uniPixelMap));
    ASSERT_NE(srcPixelMap, nullptr);

    std::vector<std::shared_ptr<EffectImageFilter>> emptyFilters;
    std::shared_ptr<Media::PixelMap> dstPixelMap = nullptr;
    EffectImageRender imageRender;
    auto ret = imageRender.Render(srcPixelMap, emptyFilters, false, dstPixelMap);
    ASSERT_EQ(ret, DrawingError::ERR_OK);
}

/**
* @tc.name: AllNullFiltersTest
* @tc.desc: Test rendering with all null filters
* @tc.type: FUNC
*/
HWTEST_F(EffectImageRenderUnittest, AllNullFiltersTest, TestSize.Level1)
{
    Media::InitializationOptions opts;
    opts.size = {100, 100};
    auto uniPixelMap = Media::PixelMap::Create(opts);
    std::shared_ptr<Media::PixelMap> srcPixelMap(std::move(uniPixelMap));
    ASSERT_NE(srcPixelMap, nullptr);

    std::vector<std::shared_ptr<EffectImageFilter>> filters;
    filters.emplace_back(nullptr);
    filters.emplace_back(nullptr);
    filters.emplace_back(nullptr);

    std::shared_ptr<Media::PixelMap> dstPixelMap = nullptr;
    EffectImageRender imageRender;
    auto ret = imageRender.Render(srcPixelMap, filters, false, dstPixelMap);
    ASSERT_EQ(ret, DrawingError::ERR_OK);
}

/**
* @tc.name: MixedNullAndValidFiltersTest
* @tc.desc: Test rendering with mix of null and valid filters
* @tc.type: FUNC
*/
HWTEST_F(EffectImageRenderUnittest, MixedNullAndValidFiltersTest, TestSize.Level1)
{
    Media::InitializationOptions opts;
    opts.size = {100, 100};
    auto uniPixelMap = Media::PixelMap::Create(opts);
    std::shared_ptr<Media::PixelMap> srcPixelMap(std::move(uniPixelMap));
    ASSERT_NE(srcPixelMap, nullptr);

    std::vector<std::shared_ptr<EffectImageFilter>> filters;
    filters.emplace_back(nullptr);
    filters.emplace_back(EffectImageFilter::Blur(1.0f));
    filters.emplace_back(nullptr);
    filters.emplace_back(EffectImageFilter::Brightness(0.5f));
    filters.emplace_back(nullptr);

    std::shared_ptr<Media::PixelMap> dstPixelMap = nullptr;
    EffectImageRender imageRender;
    auto ret = imageRender.Render(srcPixelMap, filters, false, dstPixelMap);
    ASSERT_EQ(ret, DrawingError::ERR_OK);
}

/**
* @tc.name: DirectionalBlurEdgeCaseTest
* @tc.desc: Test directional blur with edge case angles
* @tc.type: FUNC
*/
HWTEST_F(EffectImageFilterUnittest, DirectionalBlurEdgeCaseTest, TestSize.Level1)
{
    // Test with angle 0
    auto filter = EffectImageFilter::Blur(1.0f, 0.0f);
    EXPECT_NE(filter, nullptr);

    // Test with angle 90
    filter = EffectImageFilter::Blur(1.0f, 90.0f);
    EXPECT_NE(filter, nullptr);

    // Test with angle 180
    filter = EffectImageFilter::Blur(1.0f, 180.0f);
    EXPECT_NE(filter, nullptr);

    // Test with angle 360
    filter = EffectImageFilter::Blur(1.0f, 360.0f);
    EXPECT_NE(filter, nullptr);

    // Test with negative angle
    filter = EffectImageFilter::Blur(1.0f, -45.0f);
    EXPECT_NE(filter, nullptr);

    // Test with large angle
    filter = EffectImageFilter::Blur(1.0f, 720.0f);
    EXPECT_NE(filter, nullptr);
}

/**
* @tc.name: NativeBufferRenderTest001
* @tc.desc: Test RenderDstNative with different pixel formats
* @tc.type: FUNC
*/
HWTEST_F(EffectImageRenderUnittest, NativeBufferRenderTest001, TestSize.Level1)
{
    const int32_t width = 256;
    const int32_t height = 256;

    Media::InitializationOptions opts = {
        .size = {.width = width, .height = height},
        .srcPixelFormat = OHOS::Media::PixelFormat::RGBA_8888,
        .pixelFormat = OHOS::Media::PixelFormat::RGBA_8888,
    };
    std::shared_ptr<Media::PixelMap> srcPixelMap = Media::PixelMap::Create(opts);
    ASSERT_NE(srcPixelMap, nullptr);

    OH_NativeBuffer_Config config{
        .width = width,
        .height = height,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA};
    OH_NativeBuffer* dstBuffer = OH_NativeBuffer_Alloc(&config);
    std::shared_ptr<OH_NativeBuffer> dst(dstBuffer, [](OH_NativeBuffer* buffer) {});

    std::vector<std::shared_ptr<EffectImageFilter>> filters;
    filters.emplace_back(EffectImageFilter::Brightness(0.5f));

    EffectImageRender imageRender;
    auto ret = imageRender.RenderDstNative(srcPixelMap, dst, filters, false);
    ASSERT_EQ(ret, DrawingError::ERR_OK);

    OH_NativeBuffer_Unreference(dstBuffer);
}

/**
* @tc.name: RectangularImageTest
* @tc.desc: Test rendering with rectangular (non-square) images
* @tc.type: FUNC
*/
HWTEST_F(EffectImageRenderUnittest, RectangularImageTest, TestSize.Level1)
{
    // Wide image
    Media::InitializationOptions opts1;
    opts1.size = {800, 200};
    auto uniPixelMap1 = Media::PixelMap::Create(opts1);
    std::shared_ptr<Media::PixelMap> widePixelMap(std::move(uniPixelMap1));
    ASSERT_NE(widePixelMap, nullptr);

    std::vector<std::shared_ptr<EffectImageFilter>> filters;
    filters.emplace_back(EffectImageFilter::Blur(2.0f));

    std::shared_ptr<Media::PixelMap> dstPixelMap1 = nullptr;
    EffectImageRender imageRender;
    auto ret = imageRender.Render(widePixelMap, filters, false, dstPixelMap1);
    ASSERT_EQ(ret, DrawingError::ERR_OK);

    // Tall image
    Media::InitializationOptions opts2;
    opts2.size = {200, 800};
    auto uniPixelMap2 = Media::PixelMap::Create(opts2);
    std::shared_ptr<Media::PixelMap> tallPixelMap(std::move(uniPixelMap2));
    ASSERT_NE(tallPixelMap, nullptr);

    std::shared_ptr<Media::PixelMap> dstPixelMap2 = nullptr;
    ret = imageRender.Render(tallPixelMap, filters, false, dstPixelMap2);
    ASSERT_EQ(ret, DrawingError::ERR_OK);
}

/**
* @tc.name: ComplexFilterChainTest
* @tc.desc: Test complex filter chain with many filters
* @tc.type: FUNC
*/
HWTEST_F(EffectImageRenderUnittest, ComplexFilterChainTest, TestSize.Level1)
{
    Media::InitializationOptions opts;
    opts.size = {512, 512};
    auto uniPixelMap = Media::PixelMap::Create(opts);
    std::shared_ptr<Media::PixelMap> srcPixelMap(std::move(uniPixelMap));
    ASSERT_NE(srcPixelMap, nullptr);

    std::vector<std::shared_ptr<EffectImageFilter>> complexFilters;
    complexFilters.emplace_back(EffectImageFilter::Blur(0.5f));
    complexFilters.emplace_back(EffectImageFilter::Brightness(0.3f));
    complexFilters.emplace_back(EffectImageFilter::Grayscale());
    complexFilters.emplace_back(EffectImageFilter::Invert());
    complexFilters.emplace_back(EffectImageFilter::Blur(1.0f, 45.0f));
    complexFilters.emplace_back(EffectImageFilter::Brightness(0.8f));

    std::shared_ptr<Media::PixelMap> dstPixelMap = nullptr;
    EffectImageRender imageRender;
    auto ret = imageRender.Render(srcPixelMap, complexFilters, true, dstPixelMap);
    ASSERT_EQ(ret, DrawingError::ERR_OK);
}

/**
* @tc.name: ConsecutiveRenderCallsTest
* @tc.desc: Test multiple consecutive render calls
* @tc.type: FUNC
*/
HWTEST_F(EffectImageRenderUnittest, ConsecutiveRenderCallsTest, TestSize.Level1)
{
    EffectImageRender imageRender;

    for (int i = 0; i < 5; i++) {
        Media::InitializationOptions opts;
        opts.size = {100 + i * 10, 100 + i * 10};
        auto uniPixelMap = Media::PixelMap::Create(opts);
        std::shared_ptr<Media::PixelMap> srcPixelMap(std::move(uniPixelMap));
        ASSERT_NE(srcPixelMap, nullptr);

        std::vector<std::shared_ptr<EffectImageFilter>> filters;
        filters.emplace_back(EffectImageFilter::Blur(0.5f + i * 0.1f));
        filters.emplace_back(EffectImageFilter::Brightness(0.4f + i * 0.1f));

        std::shared_ptr<Media::PixelMap> dstPixelMap = nullptr;
        auto ret = imageRender.Render(srcPixelMap, filters, false, dstPixelMap);
        ASSERT_EQ(ret, DrawingError::ERR_OK);
    }
}

/**
* @tc.name: EllipticalGradientBlurParameterVariationTest
* @tc.desc: Test EllipticalGradientBlur with various parameter combinations
* @tc.type: FUNC
*/
HWTEST_F(EffectImageFilterUnittest, EllipticalGradientBlurParameterVariationTest, TestSize.Level1)
{
    std::vector<float> positions = {0.0f, 0.5f, 1.0f};
    std::vector<float> degrees = {0.2f, 0.6f, 1.0f};

    // Test with different center points
    auto filter1 = EffectImageFilter::EllipticalGradientBlur(
        1.0f, 0.0f, 0.0f, 1.0f, 1.0f, positions, degrees);
    EXPECT_NE(filter1, nullptr);

    // Test with different ellipse parameters
    auto filter2 = EffectImageFilter::EllipticalGradientBlur(
        2.0f, 10.0f, 10.0f, 2.0f, 0.5f, positions, degrees);
    EXPECT_NE(filter2, nullptr);

    // Test with extreme positions
    std::vector<float> extremePositions = {0.0f, 1.0f};
    std::vector<float> extremeDegrees = {0.1f, 1.0f};
    auto filter3 = EffectImageFilter::EllipticalGradientBlur(
        0.5f, 5.0f, 5.0f, 0.5f, 2.0f, extremePositions, extremeDegrees);
    EXPECT_NE(filter3, nullptr);
}

/**
* @tc.name: ColorMatrixVariationTest
* @tc.desc: Test ApplyColorMatrix with different matrix values
* @tc.type: FUNC
*/
HWTEST_F(EffectImageFilterUnittest, ColorMatrixVariationTest, TestSize.Level1)
{
    // Test with identity-like matrix
    Drawing::ColorMatrix matrix1;
    auto filter1 = EffectImageFilter::ApplyColorMatrix(matrix1);
    EXPECT_NE(filter1, nullptr);

    // Test applying the same matrix multiple times
    Drawing::ColorMatrix matrix2;
    auto filter2 = EffectImageFilter::ApplyColorMatrix(matrix2);
    EXPECT_NE(filter2, nullptr);

    auto filter3 = EffectImageFilter::ApplyColorMatrix(matrix2);
    EXPECT_NE(filter3, nullptr);

    // Verify they're different instances
    EXPECT_NE(filter2.get(), filter3.get());
}

/**
* @tc.name: SDFFilterVariationTest
* @tc.desc: Test CreateSDF with various parameters
* @tc.type: FUNC
*/
HWTEST_F(EffectImageFilterUnittest, SDFFilterVariationTest, TestSize.Level1)
{
    // Test with small size and invert false
    auto filter1 = EffectImageFilter::CreateSDF(16, false);
    EXPECT_NE(filter1, nullptr);

    // Test with medium size and invert true
    auto filter2 = EffectImageFilter::CreateSDF(64, true);
    EXPECT_NE(filter2, nullptr);

    // Test with large size and invert false
    auto filter3 = EffectImageFilter::CreateSDF(512, false);
    EXPECT_NE(filter3, nullptr);

    // Test with boundary size
    auto filter4 = EffectImageFilter::CreateSDF(1, false);
    EXPECT_NE(filter4, nullptr);
}

/**
* @tc.name: MultiPixelFormatRenderTest
* @tc.desc: Test rendering with different pixel formats
* @tc.type: FUNC
*/
HWTEST_F(EffectImageRenderUnittest, MultiPixelFormatRenderTest, TestSize.Level1)
{
    std::vector<OHOS::Media::PixelFormat> pixelFormats = {
        OHOS::Media::PixelFormat::RGBA_8888,
        OHOS::Media::PixelFormat::RGB_565,
    };

    EffectImageRender imageRender;
    std::vector<std::shared_ptr<EffectImageFilter>> filters;
    filters.emplace_back(EffectImageFilter::Blur(1.0f));

    for (auto format : pixelFormats) {
        Media::InitializationOptions opts = {
            .size = {.width = 64, .height = 64},
            .srcPixelFormat = format,
            .pixelFormat = format,
        };
        std::shared_ptr<Media::PixelMap> srcPixelMap = Media::PixelMap::Create(opts);
        if (srcPixelMap != nullptr) {
            std::shared_ptr<Media::PixelMap> dstPixelMap = nullptr;
            auto ret = imageRender.Render(srcPixelMap, filters, false, dstPixelMap);
            ASSERT_EQ(ret, DrawingError::ERR_OK);
        }
    }
}
} // namespace Rosen
} // namespace OHOS