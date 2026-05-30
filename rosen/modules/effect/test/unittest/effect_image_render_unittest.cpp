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
#include "render/rs_pixel_map_util.h"

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
    int32_t syncFenceFdValue = 0;
    int32_t* syncFenceFd = &syncFenceFdValue;
    auto ret = imageRender.RenderNativeBuffer(srcPixelMap, dst, imageFilter, syncFenceFd, true);
    ASSERT_EQ(ret, DrawingError::ERR_OK);
    ret = imageRender.RenderNativeBuffer(srcPixelMap, dst, imageFilter, syncFenceFd, false);
    ASSERT_EQ(ret, DrawingError::ERR_OK);
    ret = imageRender.RenderNativeBuffer(nullptr, dst, imageFilter, syncFenceFd, true);
    ASSERT_NE(ret, DrawingError::ERR_OK);
    std::shared_ptr<OH_NativeBuffer> nullBuffer = nullptr;
    ret = imageRender.RenderNativeBuffer(srcPixelMap, nullBuffer, imageFilter, syncFenceFd, true);
    ASSERT_NE(ret, DrawingError::ERR_OK);
    ret = imageRender.RenderNativeBuffer(srcPixelMap, nullBuffer, imageFilter, syncFenceFd, true);
    ASSERT_NE(ret, DrawingError::ERR_OK);
    ret = imageRender.RenderNativeBuffer(srcPixelMap, dst, imageFilter, syncFenceFd, true);
    ASSERT_EQ(ret, DrawingError::ERR_OK);
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

    Vector4f color3 = {0.0f, 0.5f, 0.5f, 1.0f}; // color rgba
    std::vector<Vector4f> colors1 = {color1, color2, color3};
    std::vector<float> positions1 = {0.5f, 1.0f};
    flag = EffectImageFilter::MapColorByBrightness(colors1, positions1);
    EXPECT_EQ(flag, nullptr);

    colors1 = {};
    flag = EffectImageFilter::MapColorByBrightness(colors1, positions1);
    EXPECT_EQ(flag, nullptr);

    positions1 = {};
    flag = EffectImageFilter::MapColorByBrightness(colors1, positions1);
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
    DrawingError result = image->Prepare(srcPixelMap, false);
    ASSERT_EQ(result, DrawingError::ERR_OK);

    result = filter->Apply(image);
    ASSERT_EQ(result, DrawingError::ERR_OK);
}

/**
 * @tc.name: ApplyMethodReededGlass
 * @tc.desc: Test EffectImageReededGlassFilter filter application, image is null
 */
HWTEST_F(EffectImageRenderUnittest, ApplyMethodReededGlass, TestSize.Level1)
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
    ASSERT_EQ(result, DrawingError::ERR_OK);
}

/**
 * @tc.name: ApplyMethodWaterGlass
 * @tc.desc: Test EffectImageWaterGlassFilter filter application, image is null
 */
HWTEST_F(EffectImageRenderUnittest, ApplyMethodWaterGlass, TestSize.Level1)
{
    std::shared_ptr<Drawing::GEWaterGlassDataParams> params = std::make_shared<Drawing::GEWaterGlassDataParams>();
    ASSERT_NE(params, nullptr);
    std::shared_ptr<EffectImageWaterGlassFilter> filter = std::make_shared<EffectImageWaterGlassFilter>(params);
    ASSERT_NE(filter, nullptr);

    DrawingError result = filter->Apply(nullptr);
    ASSERT_EQ(result, DrawingError::ERR_IMAGE_NULL);
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
* @tc.name: ScaleFilterWithInvalidScaleX001
* @tc.desc: Test EffectImageFilter::Scale with invalid scaleX (zero)
* @tc.type: FUNC
*/
HWTEST_F(EffectImageFilterUnittest, ScaleFilterWithInvalidScaleX001, TestSize.Level1)
{
    auto filterMode = Drawing::FilterMode::LINEAR;
    auto mipmapMode = Drawing::MipmapMode::LINEAR;
    auto filter = EffectImageFilter::Scale(0.0f, 1.0f, filterMode, mipmapMode);
    EXPECT_EQ(filter, nullptr);
}

/**
* @tc.name: ScaleFilterWithInvalidScaleX002
* @tc.desc: Test EffectImageFilter::Scale with invalid scaleX (negative)
* @tc.type: FUNC
*/
HWTEST_F(EffectImageFilterUnittest, ScaleFilterWithInvalidScaleX002, TestSize.Level1)
{
    auto filterMode = Drawing::FilterMode::LINEAR;
    auto mipmapMode = Drawing::MipmapMode::LINEAR;
    auto filter = EffectImageFilter::Scale(-0.5f, 1.0f, filterMode, mipmapMode);
    EXPECT_EQ(filter, nullptr);
}

/**
* @tc.name: ScaleFilterWithInvalidScaleX003
* @tc.desc: Test EffectImageFilter::Scale with invalid scaleX (very negative)
* @tc.type: FUNC
*/
HWTEST_F(EffectImageFilterUnittest, ScaleFilterWithInvalidScaleX003, TestSize.Level1)
{
    auto filterMode = Drawing::FilterMode::LINEAR;
    auto mipmapMode = Drawing::MipmapMode::LINEAR;
    auto filter = EffectImageFilter::Scale(-100.0f, 1.0f, filterMode, mipmapMode);
    EXPECT_EQ(filter, nullptr);
}

/**
* @tc.name: ScaleFilterWithInvalidScaleY001
* @tc.desc: Test EffectImageFilter::Scale with invalid scaleY (zero)
* @tc.type: FUNC
*/
HWTEST_F(EffectImageFilterUnittest, ScaleFilterWithInvalidScaleY001, TestSize.Level1)
{
    auto filterMode = Drawing::FilterMode::LINEAR;
    auto mipmapMode = Drawing::MipmapMode::LINEAR;
    auto filter = EffectImageFilter::Scale(1.0f, 0.0f, filterMode, mipmapMode);
    EXPECT_EQ(filter, nullptr);
}

/**
* @tc.name: ScaleFilterWithInvalidScaleY002
* @tc.desc: Test EffectImageFilter::Scale with invalid scaleY (negative)
* @tc.type: FUNC
*/
HWTEST_F(EffectImageFilterUnittest, ScaleFilterWithInvalidScaleY002, TestSize.Level1)
{
    auto filterMode = Drawing::FilterMode::LINEAR;
    auto mipmapMode = Drawing::MipmapMode::LINEAR;
    auto filter = EffectImageFilter::Scale(1.0f, -0.5f, filterMode, mipmapMode);
    EXPECT_EQ(filter, nullptr);
}

/**
* @tc.name: ScaleFilterWithInvalidScaleY003
* @tc.desc: Test EffectImageFilter::Scale with invalid scaleY (very negative)
* @tc.type: FUNC
*/
HWTEST_F(EffectImageFilterUnittest, ScaleFilterWithInvalidScaleY003, TestSize.Level1)
{
    auto filterMode = Drawing::FilterMode::LINEAR;
    auto mipmapMode = Drawing::MipmapMode::LINEAR;
    auto filter = EffectImageFilter::Scale(1.0f, -100.0f, filterMode, mipmapMode);
    EXPECT_EQ(filter, nullptr);
}

/**
* @tc.name: ScaleFilterWithBothInvalidScales001
* @tc.desc: Test EffectImageFilter::Scale with both scales zero
* @tc.type: FUNC
*/
HWTEST_F(EffectImageFilterUnittest, ScaleFilterWithBothInvalidScales001, TestSize.Level1)
{
    auto filterMode = Drawing::FilterMode::LINEAR;
    auto mipmapMode = Drawing::MipmapMode::LINEAR;
    auto filter = EffectImageFilter::Scale(0.0f, 0.0f, filterMode, mipmapMode);
    EXPECT_EQ(filter, nullptr);
}

/**
* @tc.name: ScaleFilterWithBothInvalidScales002
* @tc.desc: Test EffectImageFilter::Scale with both scales negative
* @tc.type: FUNC
*/
HWTEST_F(EffectImageFilterUnittest, ScaleFilterWithBothInvalidScales002, TestSize.Level1)
{
    auto filterMode = Drawing::FilterMode::LINEAR;
    auto mipmapMode = Drawing::MipmapMode::LINEAR;
    auto filter = EffectImageFilter::Scale(-1.0f, -1.0f, filterMode, mipmapMode);
    EXPECT_EQ(filter, nullptr);
}

/**
* @tc.name: ScaleFilterWithValidScales001
* @tc.desc: Test EffectImageFilter::Scale with valid scales (equal scaling)
* @tc.type: FUNC
*/
HWTEST_F(EffectImageFilterUnittest, ScaleFilterWithValidScales001, TestSize.Level1)
{
    auto filter = EffectImageFilter::Scale(1.0f, 1.0f,
        Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    EXPECT_NE(filter, nullptr);
}

/**
* @tc.name: ScaleFilterWithValidScales002
* @tc.desc: Test EffectImageFilter::Scale with valid scales (upscale)
* @tc.type: FUNC
*/
HWTEST_F(EffectImageFilterUnittest, ScaleFilterWithValidScales002, TestSize.Level1)
{
    auto filter = EffectImageFilter::Scale(2.0f, 2.0f,
        Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    EXPECT_NE(filter, nullptr);
}

/**
* @tc.name: ScaleFilterWithValidScales003
* @tc.desc: Test EffectImageFilter::Scale with valid scales (downscale)
* @tc.type: FUNC
*/
HWTEST_F(EffectImageFilterUnittest, ScaleFilterWithValidScales003, TestSize.Level1)
{
    auto filter = EffectImageFilter::Scale(0.5f, 0.5f,
        Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    EXPECT_NE(filter, nullptr);
}

/**
* @tc.name: ScaleFilterWithDifferentFilterModes001
* @tc.desc: Test EffectImageFilter::Scale with different filter modes
* @tc.type: FUNC
*/
HWTEST_F(EffectImageFilterUnittest, ScaleFilterWithDifferentFilterModes001, TestSize.Level1)
{
    auto filter1 = EffectImageFilter::Scale(1.0f, 1.0f,
        Drawing::FilterMode::NEAREST, Drawing::MipmapMode::NONE);
        EXPECT_NE(filter1, nullptr);

    auto filter2 = EffectImageFilter::Scale(1.0f, 1.0f,
        Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    EXPECT_NE(filter2, nullptr);
}

/**
* @tc.name: ScaleFilterWithDifferentMipmapModes001
* @tc.desc: Test EffectImageFilter::Scale with different mipmap modes
* @tc.type: FUNC
*/
HWTEST_F(EffectImageFilterUnittest, ScaleFilterWithDifferentMipmapModes001, TestSize.Level1)
{
    auto filter1 = EffectImageFilter::Scale(1.0f, 1.0f,
        Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    EXPECT_NE(filter1, nullptr);

    auto filter2 = EffectImageFilter::Scale(1.0f, 1.0f,
        Drawing::FilterMode::LINEAR, Drawing::MipmapMode::LINEAR);
    EXPECT_NE(filter2, nullptr);
}

/**
* @tc.name: EffectImageScaleFilterApplyWithNullImage001
* @tc.desc: Test EffectImageScaleFilter::Apply with null image
* @tc.type: FUNC
*/
HWTEST_F(EffectImageRenderUnittest, EffectImageScaleFilterApplyWithNullImage001, TestSize.Level1)
{
    auto filter = EffectImageFilter::Scale(1.0f, 1.0f,
        Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    ASSERT_NE(filter, nullptr);

    auto ret = filter->Apply(nullptr);
    EXPECT_EQ(ret, DrawingError::ERR_IMAGE_NULL);
}

/**
* @tc.name: EffectImageScaleFilterApplySuccess001
* @tc.desc: Test EffectImageScaleFilter::Apply with valid image (but not prepared)
* @tc.type: FUNC
*/
HWTEST_F(EffectImageRenderUnittest, EffectImageScaleFilterApplySuccess001, TestSize.Level1)
{
    auto filter = EffectImageFilter::Scale(1.5f, 0.8f,
        Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    ASSERT_NE(filter, nullptr);

    auto image = std::make_shared<EffectImageChain>();
    ASSERT_NE(image, nullptr);

    // Not prepared, so should return error
    auto ret = filter->Apply(image);
    EXPECT_NE(ret, DrawingError::ERR_OK);
}

/**
* @tc.name: EffectImageScaleFilterApplyWithCPU001
* @tc.desc: Test EffectImageScaleFilter::Apply with CPU mode (not supported)
* @tc.type: FUNC
*/
HWTEST_F(EffectImageRenderUnittest, EffectImageScaleFilterApplyWithCPU001, TestSize.Level1)
{
    auto filter = EffectImageFilter::Scale(1.0f, 1.0f,
        Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    ASSERT_NE(filter, nullptr);

    Media::InitializationOptions opts;
    opts.size = {100, 100};
    auto uniPixelMap = Media::PixelMap::Create(opts);
    std::shared_ptr<Media::PixelMap> srcPixelMap(std::move(uniPixelMap));
    ASSERT_NE(srcPixelMap, nullptr);

    auto image = std::make_shared<EffectImageChain>();
    ASSERT_NE(image, nullptr);
    auto ret = image->Prepare(srcPixelMap, true);
    ASSERT_EQ(ret, DrawingError::ERR_OK);

    // CPU mode not supported for scale
    ret = filter->Apply(image);
    EXPECT_EQ(ret, DrawingError::ERR_OK);
}

/**
* @tc.name: EffectImageScaleFilterApplySuccess002
* @tc.desc: Test EffectImageScaleFilter::Apply with valid prepared image (GPU mode)
* @tc.type: FUNC
*/
HWTEST_F(EffectImageRenderUnittest, EffectImageScaleFilterApplySuccess002, TestSize.Level1)
{
    auto filter = EffectImageFilter::Scale(1.5f, 0.8f,
        Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    ASSERT_NE(filter, nullptr);

    Media::InitializationOptions opts;
    opts.size = {100, 100};
    auto uniPixelMap = Media::PixelMap::Create(opts);
    std::shared_ptr<Media::PixelMap> srcPixelMap(std::move(uniPixelMap));
    ASSERT_NE(srcPixelMap, nullptr);

    auto image = std::make_shared<EffectImageChain>();
    ASSERT_NE(image, nullptr);
    auto ret = image->Prepare(srcPixelMap, false);
    ASSERT_EQ(ret, DrawingError::ERR_OK);

    // Should succeed in GPU mode
    ret = filter->Apply(image);
    EXPECT_EQ(ret, DrawingError::ERR_OK);
}

/**
* @tc.name: EffectImageScaleFilterApplyWithExistingFilters001
* @tc.desc: Test EffectImageScaleFilter::Apply when filters already exist
* @tc.type: FUNC
*/
HWTEST_F(EffectImageRenderUnittest, EffectImageScaleFilterApplyWithExistingFilters001, TestSize.Level1)
{
    Media::InitializationOptions opts;
    opts.size = {100, 100};
    auto uniPixelMap = Media::PixelMap::Create(opts);
    std::shared_ptr<Media::PixelMap> srcPixelMap(std::move(uniPixelMap));
    ASSERT_NE(srcPixelMap, nullptr);

    auto image = std::make_shared<EffectImageChain>();
    ASSERT_NE(image, nullptr);
    auto ret = image->Prepare(srcPixelMap, false);
    ASSERT_EQ(ret, DrawingError::ERR_OK);

    // Apply a blur filter first
    auto blurFilter = Drawing::ImageFilter::CreateBlurImageFilter(1.0f, 1.0f,
        Drawing::TileMode::CLAMP, nullptr);
    ASSERT_NE(blurFilter, nullptr);
    ret = image->ApplyDrawingFilter(blurFilter);
    ASSERT_EQ(ret, DrawingError::ERR_OK);

    // Now apply scale - should handle existing filters
    auto scaleFilter = EffectImageFilter::Scale(0.5f, 0.5f,
        Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    ASSERT_NE(scaleFilter, nullptr);
    ret = scaleFilter->Apply(image);
    EXPECT_EQ(ret, DrawingError::ERR_OK);
}

/**
* @tc.name: ScaleFilterWithVerySmallScales001
* @tc.desc: Test EffectImageFilter::Scale with very small but positive scales
* @tc.type: FUNC
*/
HWTEST_F(EffectImageFilterUnittest, ScaleFilterWithVerySmallScales001, TestSize.Level1)
{
    auto filter = EffectImageFilter::Scale(0.001f, 0.001f,
        Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    EXPECT_NE(filter, nullptr);
}
    
/**
* @tc.name: ScaleFilterWithVeryLargeScales001
* @tc.desc: Test EffectImageFilter::Scale with very large scales
* @tc.type: FUNC
*/
HWTEST_F(EffectImageFilterUnittest, ScaleFilterWithVeryLargeScales001, TestSize.Level1)
{
    auto filter = EffectImageFilter::Scale(100.0f, 100.0f,
        Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    EXPECT_NE(filter, nullptr);
}

} // namespace Rosen
} // namespace OHOS