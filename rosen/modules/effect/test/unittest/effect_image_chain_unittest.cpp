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
#include <gmock/gmock.h>
#include <atomic>
#include <future>
#include <thread>
#include <vector>
#include "effect_image_chain.h"
#include "effect_types.h"
#include "ge_linear_gradient_shader_mask.h"
#include "platform/common/rs_system_properties.h"
#include "surface_buffer.h"
#include "render/rs_pixel_map_util.h"
#if defined(RS_ENABLE_VK)
#include "vulkan_context/rs_vulkan_context.h"
#endif

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
static constexpr float TEST_BLUR_RADIUS = 0.5f;
#if defined(RS_ENABLE_VK)
static constexpr int32_t TEST_CONCURRENT_THREAD_COUNT = 4;
#endif

class EffectImageChainUnittest : public testing::Test {
public:
    static void SetUpTestCase() {};
    static void TearDownTestCase() {};
    void SetUp() override {};
    void TearDown() override {};
};

/**
 * @tc.name: Prepare_Test001
 * @tc.desc: test prepare
 */
HWTEST_F(EffectImageChainUnittest, PrepareTest001, TestSize.Level1)
{
    auto image = std::make_shared<EffectImageChain>();
    ASSERT_NE(image, nullptr);

    Media::InitializationOptions opts;
    opts.size = { 1, 1 };
    std::shared_ptr<Media::PixelMap> srcPixelMap(Media::PixelMap::Create(opts));
    ASSERT_NE(srcPixelMap, nullptr);
    auto ret = image->Prepare(srcPixelMap, true);
    ASSERT_EQ(ret, DrawingError::ERR_OK);

    ret = image->Prepare(srcPixelMap, false);
    ASSERT_EQ(ret, DrawingError::ERR_OK);
}

class MockPixelMap : public Media::PixelMap {
public:
    MOCK_METHOD(int32_t, GetWidth, ());
    MOCK_METHOD(int32_t, GetHeight, ());
    MOCK_METHOD(Media::PixelFormat, GetPixelFormat, ());
    MOCK_METHOD(Media::AlphaType, GetAlphaType, ());
};

/**
 * @tc.name: PrepareTest002
 * @tc.desc: test prepare
 */
HWTEST_F(EffectImageChainUnittest, PrepareTest002, TestSize.Level1)
{
    auto image = std::make_shared<EffectImageChain>();
    ASSERT_NE(image, nullptr);

    auto ret = image->Prepare(nullptr, true);
    ASSERT_NE(ret, DrawingError::ERR_OK);

    auto srcPixelMap = std::make_shared<MockPixelMap>();
    ASSERT_NE(srcPixelMap, nullptr);

    EXPECT_CALL(*srcPixelMap, GetWidth()).WillOnce(testing::Return(0));
    ret = image->Prepare(srcPixelMap, false);
    ASSERT_NE(ret, DrawingError::ERR_OK);

    EXPECT_CALL(*srcPixelMap, GetWidth()).WillRepeatedly(testing::Return(1));
    EXPECT_CALL(*srcPixelMap, GetHeight()).WillOnce(testing::Return(0));
    ret = image->Prepare(srcPixelMap, false);
    ASSERT_NE(ret, DrawingError::ERR_OK);

    EXPECT_CALL(*srcPixelMap, GetHeight()).WillRepeatedly(testing::Return(1));
    EXPECT_CALL(*srcPixelMap, GetPixelFormat()).WillOnce(testing::Return(Media::PixelFormat::UNKNOWN));
    ret = image->Prepare(srcPixelMap, false);
    ASSERT_NE(ret, DrawingError::ERR_OK);

    EXPECT_CALL(*srcPixelMap, GetPixelFormat()).WillRepeatedly(testing::Return(Media::PixelFormat::RGBA_8888));
    EXPECT_CALL(*srcPixelMap, GetAlphaType()).WillOnce(testing::Return(Media::AlphaType::IMAGE_ALPHA_TYPE_UNKNOWN));
    ret = image->Prepare(srcPixelMap, false);
    ASSERT_NE(ret, DrawingError::ERR_OK);
}

/**
 * @tc.name: PrepareTest004
 * @tc.desc: test prepare
 */
HWTEST_F(EffectImageChainUnittest, PrepareTest004, TestSize.Level1)
{
    auto image = std::make_shared<EffectImageChain>();
    ASSERT_NE(image, nullptr);
    auto renderContext = RenderContext::Create();
    renderContext->Init(RenderEngineType::BASIC_RENDER);
    image->renderContext_ = renderContext;
    const auto width = 200;
    const auto height = 200;
    auto colorSpace = Drawing::ColorSpace::CreateSRGB();
    Drawing::ImageInfo imageInfo = Drawing::ImageInfo{
        width, height,
        Drawing::ColorType::COLORTYPE_RGBA_8888,
        Drawing::AlphaType::ALPHATYPE_UNPREMUL,
        colorSpace};
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
    ASSERT_NE(srcPixelMap, nullptr);

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

    std::shared_ptr<Media::PixelMap> nullPixelmap = nullptr;
    std::shared_ptr<OH_NativeBuffer> nullBuffer = nullptr;
    auto ret = image->PrepareNativeBuffer(nullPixelmap, dst);
    ASSERT_NE(ret, DrawingError::ERR_OK);
    ret = image->PrepareNativeBuffer(srcPixelMap, nullBuffer);
    ASSERT_NE(ret, DrawingError::ERR_OK);
    ret = image->PrepareNativeBuffer(srcPixelMap, dst);
    ASSERT_EQ(ret, DrawingError::ERR_OK);
    OH_NativeBuffer_Unreference(dstBuffer);
}

/**
 * @tc.name: ApplyDrawTest001
 * @tc.desc: test Apply and Draw
 */
HWTEST_F(EffectImageChainUnittest, ApplyDrawTest001, TestSize.Level1)
{
    auto image = std::make_shared<EffectImageChain>();
    EXPECT_NE(image, nullptr);
    auto ret = image->ApplyDrawingFilter(nullptr);
    EXPECT_NE(ret, DrawingError::ERR_OK);
    ret = image->ApplyBlur(-1, Drawing::TileMode::CLAMP);
    EXPECT_NE(ret, DrawingError::ERR_OK); // invalid radius

    ret = image->ApplyBlur(0.5, Drawing::TileMode::CLAMP);
    EXPECT_NE(ret, DrawingError::ERR_OK); // need prepered first

    Media::InitializationOptions opts;
    opts.size = { 1, 1 };
    std::shared_ptr<Media::PixelMap> srcPixelMap(Media::PixelMap::Create(opts));
    EXPECT_NE(srcPixelMap, nullptr);
    ret = image->Prepare(srcPixelMap, false);
    EXPECT_EQ(ret, DrawingError::ERR_OK);

    ret = image->ApplyBlur(0.5, Drawing::TileMode::CLAMP); // hps
    EXPECT_EQ(ret, DrawingError::ERR_OK);

    ret = image->ApplyBlur(0, Drawing::TileMode::CLAMP); // hps 0
    EXPECT_EQ(ret, DrawingError::ERR_OK);

    RSSystemProperties::SetForceHpsBlurDisabled(true);
    EXPECT_TRUE(!RSSystemProperties::GetHpsBlurEnabled());
    ret = image->ApplyBlur(0.5, Drawing::TileMode::CLAMP); // disable hps
    EXPECT_EQ(ret, DrawingError::ERR_OK);
    RSSystemProperties::SetForceHpsBlurDisabled(false);
    EXPECT_TRUE(RSSystemProperties::GetHpsBlurEnabled());

    ret = image->ApplyBlur(0.5, Drawing::TileMode::DECAL); // mesa
    EXPECT_EQ(ret, DrawingError::ERR_OK);

    auto filterBlur = Drawing::ImageFilter::CreateBlurImageFilter(1, 1, Drawing::TileMode::DECAL, nullptr);
    EXPECT_NE(filterBlur, nullptr);
    ret = image->ApplyDrawingFilter(filterBlur);
    EXPECT_EQ(ret, DrawingError::ERR_OK);
    ret = image->ApplyBlur(0.5, Drawing::TileMode::CLAMP); // has drawing before blur
    EXPECT_EQ(ret, DrawingError::ERR_OK);

    ret = image->Draw();
    EXPECT_EQ(ret, DrawingError::ERR_OK);

    ret = image->Prepare(srcPixelMap, false);
    EXPECT_EQ(ret, DrawingError::ERR_OK);
    ret = image->ApplyBlur(0.5, Drawing::TileMode::MIRROR); // cpu
    EXPECT_EQ(ret, DrawingError::ERR_OK);
}


/**
 * @tc.name: ApplyDrawTest003
 * @tc.desc: test Apply and Draw
 */
HWTEST_F(EffectImageChainUnittest, ApplyDrawTest003, TestSize.Level1)
{
    auto image = std::make_shared<EffectImageChain>();
    ASSERT_NE(image, nullptr);
    const auto width = 200;
    const auto height = 200;
    auto colorSpace = Drawing::ColorSpace::CreateSRGB();
    Drawing::ImageInfo imageInfo = Drawing::ImageInfo{
        width, height,
        Drawing::ColorType::COLORTYPE_RGBA_8888,
        Drawing::AlphaType::ALPHATYPE_UNPREMUL,
        colorSpace};
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
    ASSERT_NE(srcPixelMap, nullptr);

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

    // check proper error return when Prepare(...) not called
    auto ret = image->DrawNativeBuffer();
    ASSERT_NE(ret, DrawingError::ERR_OK);

    ret = image->PrepareNativeBuffer(srcPixelMap, dst);
    ASSERT_EQ(ret, DrawingError::ERR_OK);
    ret = image->DrawNativeBuffer();
    ASSERT_EQ(ret, DrawingError::ERR_OK);
    OH_NativeBuffer_Unreference(dstBuffer);
}

/**
 * @tc.name: ApplySDFTest001
 * @tc.desc: test ApplySDFCreation
 */
HWTEST_F(EffectImageChainUnittest, ApplySDFTest001, TestSize.Level1)
{
    auto image = std::make_shared<EffectImageChain>();
    ASSERT_NE(image, nullptr);

    Media::InitializationOptions opts;
    opts.size = { 1, 1 };
    std::shared_ptr<Media::PixelMap> srcPixelMap(Media::PixelMap::Create(opts));
    ASSERT_NE(srcPixelMap, nullptr);

    auto ret = image->ApplySDFCreation(32, false);
    ASSERT_NE(ret, DrawingError::ERR_OK); // not prepared

    ret = image->Prepare(srcPixelMap, true);
    ASSERT_EQ(ret, DrawingError::ERR_OK);

    ret = image->ApplySDFCreation(8, false);
    EXPECT_NE(ret, DrawingError::ERR_OK); // force cpu unsupported

    ret = image->Prepare(srcPixelMap, false);
    ASSERT_EQ(ret, DrawingError::ERR_OK);

    ret = image->Draw(); // no filter
    ASSERT_EQ(ret, DrawingError::ERR_OK);

    ret = image->ApplySDFCreation(64, true);
    ASSERT_EQ(ret, DrawingError::ERR_OK);

    ret = image->Draw();
    ASSERT_EQ(ret, DrawingError::ERR_OK);

    auto filterBlur = Drawing::ImageFilter::CreateBlurImageFilter(1, 1, Drawing::TileMode::DECAL, nullptr);
    ASSERT_NE(filterBlur, nullptr);
    ret = image->ApplyDrawingFilter(filterBlur);
    ASSERT_EQ(ret, DrawingError::ERR_OK);
    ret = image->ApplySDFCreation(64, true); // has drawing before sdf
    ASSERT_EQ(ret, DrawingError::ERR_OK);

    ret = image->Draw();
    ASSERT_EQ(ret, DrawingError::ERR_OK);
}

/**
 * @tc.name: PixelFormatConvertTest001
 * @tc.desc: test PixelFormatToDrawingColorType
 */
HWTEST_F(EffectImageChainUnittest, PixelFormatConvertTest001, TestSize.Level1)
{
    auto pixelFormat = Media::PixelFormat::RGB_565;
    ASSERT_EQ(ImageUtil::PixelFormatToDrawingColorType(pixelFormat), Drawing::ColorType::COLORTYPE_RGB_565);

    pixelFormat = Media::PixelFormat::RGBA_8888;
    ASSERT_EQ(ImageUtil::PixelFormatToDrawingColorType(pixelFormat), Drawing::ColorType::COLORTYPE_RGBA_8888);

    pixelFormat = Media::PixelFormat::BGRA_8888;
    ASSERT_EQ(ImageUtil::PixelFormatToDrawingColorType(pixelFormat), Drawing::ColorType::COLORTYPE_BGRA_8888);

    pixelFormat = Media::PixelFormat::ALPHA_8;
    ASSERT_EQ(ImageUtil::PixelFormatToDrawingColorType(pixelFormat), Drawing::ColorType::COLORTYPE_ALPHA_8);

    pixelFormat = Media::PixelFormat::RGBA_F16;
    ASSERT_EQ(ImageUtil::PixelFormatToDrawingColorType(pixelFormat), Drawing::ColorType::COLORTYPE_RGBA_F16);

    pixelFormat = Media::PixelFormat::RGBA_1010102;
    ASSERT_EQ(ImageUtil::PixelFormatToDrawingColorType(pixelFormat), Drawing::ColorType::COLORTYPE_RGBA_1010102);

    pixelFormat = Media::PixelFormat::UNKNOWN;
    ASSERT_EQ(ImageUtil::PixelFormatToDrawingColorType(pixelFormat), Drawing::ColorType::COLORTYPE_UNKNOWN);
}

/**
 * @tc.name: AlphaTypeConvertTest001
 * @tc.desc: test AlphaTypeToDrawingAlphaType
 */
HWTEST_F(EffectImageChainUnittest, AlphaTypeConvertTest001, TestSize.Level1)
{
    auto alphaType = Media::AlphaType::IMAGE_ALPHA_TYPE_UNKNOWN;
    ASSERT_EQ(ImageUtil::AlphaTypeToDrawingAlphaType(alphaType), Drawing::AlphaType::ALPHATYPE_UNKNOWN);

    alphaType = Media::AlphaType::IMAGE_ALPHA_TYPE_OPAQUE;
    ASSERT_EQ(ImageUtil::AlphaTypeToDrawingAlphaType(alphaType), Drawing::AlphaType::ALPHATYPE_OPAQUE);

    alphaType = Media::AlphaType::IMAGE_ALPHA_TYPE_PREMUL;
    ASSERT_EQ(ImageUtil::AlphaTypeToDrawingAlphaType(alphaType), Drawing::AlphaType::ALPHATYPE_PREMUL);

    alphaType = Media::AlphaType::IMAGE_ALPHA_TYPE_UNPREMUL;
    ASSERT_EQ(ImageUtil::AlphaTypeToDrawingAlphaType(alphaType), Drawing::AlphaType::ALPHATYPE_UNPREMUL);
}

/**
 * @tc.name  : EffectCanvasTest
 * @tc.desc  : test EffectCanvas
 */
HWTEST_F(EffectImageChainUnittest, EffectCanvasTest, TestSize.Level1) {
    class MockSurface : public Drawing::Surface {
    public:
        std::unique_ptr<Drawing::Canvas> GetCanvas() {
            return nullptr;
        }
    };
    std::shared_ptr<MockSurface> surface = std::make_shared<MockSurface>();
    EffectCanvas effectCanvas(surface.get());

    Drawing::Rect s = { 0.0f, 0.0f, 1.0f, 1.0f };
    Drawing::Rect d = { 0.0f, 0.0f, 1.0f, 1.0f };
    Drawing::HpsBlurParameter blurParams = { s, d, 0.0f, 0.0f, 1.0f };

    auto dimension = effectCanvas.CalcHpsBluredImageDimension(blurParams);
    std::array<int, 2> expected = {0, 0};
    EXPECT_EQ(dimension, expected);

    auto attachBrush = Drawing::Brush();
    auto& attachCanvas = effectCanvas.AttachBrush(attachBrush);
    EXPECT_EQ(&attachCanvas, &effectCanvas);

    auto bounds = Drawing::Rect(0.0f, 0.0f, 100.0f, 100.0f);
    effectCanvas.DrawRect(bounds);

    auto& detachCanvas = effectCanvas.DetachBrush();
    EXPECT_EQ(&detachCanvas, &effectCanvas);
}

/**
 * @tc.name: ApplyMapColorByBrightnessTest
 * @tc.desc: test ApplyMapColorByBrightness
 * @tc.type: FUNC
 */
HWTEST_F(EffectImageChainUnittest, ApplyMapColorByBrightnessTest, TestSize.Level1)
{
    auto image = std::make_shared<EffectImageChain>();
    std::vector<Vector4f> colors = {{1.0f, 0.0f, 0.5f, 1.0f}}; // color rgba
    std::vector<float> positions = {0.5};

    // test not prepare
    auto ret = image->ApplyMapColorByBrightness(colors, positions);
    EXPECT_NE(ret, DrawingError::ERR_OK);

    // test CPU
    Media::InitializationOptions opts;
    opts.size = { 1, 1 };
    std::shared_ptr<Media::PixelMap> srcPixelMap(Media::PixelMap::Create(opts));
    EXPECT_NE(srcPixelMap, nullptr);
    ret = image->Prepare(srcPixelMap, true);
    ret = image->ApplyMapColorByBrightness(colors, positions);
    EXPECT_NE(ret, DrawingError::ERR_OK);

    // test filter is nullptr
    ret = image->Prepare(srcPixelMap, false);
    ret = image->ApplyMapColorByBrightness(colors, positions);
    EXPECT_EQ(ret, DrawingError::ERR_OK);

    // test filter not is nullptr
    auto filterBlur = Drawing::ImageFilter::CreateBlurImageFilter(1, 1, Drawing::TileMode::DECAL, nullptr);
    ASSERT_NE(filterBlur, nullptr);
    ret = image->ApplyDrawingFilter(filterBlur);
    ret = image->ApplyMapColorByBrightness(colors, positions);
    EXPECT_EQ(ret, DrawingError::ERR_OK);
}

/**
 * @tc.name: ApplyGammaCorrectionTest
 * @tc.desc: test ApplyGammaCorrection
 * @tc.type: FUNC
 */
HWTEST_F(EffectImageChainUnittest, ApplyGammaCorrectionTest, TestSize.Level1)
{
    auto image = std::make_shared<EffectImageChain>();
    float gamma = 1.5f; // valid value

    // test not prepare
    auto ret = image->ApplyGammaCorrection(gamma);
    EXPECT_NE(ret, DrawingError::ERR_OK);

    // test CUP
    Media::InitializationOptions opts;
    opts.size = { 1, 1 };
    std::shared_ptr<Media::PixelMap> srcPixelMap(Media::PixelMap::Create(opts));
    EXPECT_NE(srcPixelMap, nullptr);
    ret = image->Prepare(srcPixelMap, true);
    ret = image->ApplyGammaCorrection(gamma);
    EXPECT_NE(ret, DrawingError::ERR_OK);

    // test filter is nullptr
    ret = image->Prepare(srcPixelMap, false);
    ret = image->ApplyGammaCorrection(gamma);
    EXPECT_EQ(ret, DrawingError::ERR_OK);

    // test filter not is nullptr
    auto filterBlur = Drawing::ImageFilter::CreateBlurImageFilter(1, 1, Drawing::TileMode::DECAL, nullptr);
    ASSERT_NE(filterBlur, nullptr);
    ret = image->ApplyDrawingFilter(filterBlur);
    ret = image->ApplyGammaCorrection(gamma);
    EXPECT_EQ(ret, DrawingError::ERR_OK);
}

/**
 * @tc.name: WaterGlassReturnsErrorWhenNotPrepared
 * @tc.desc: test ApplyWaterGlass returns error when not prepared.
 */
HWTEST_F(EffectImageChainUnittest, WaterGlassReturnsErrorWhenNotPrepared, TestSize.Level1)
{
    EffectImageChain chain;
    auto params = std::make_shared<Drawing::GEWaterGlassDataParams>();
    ASSERT_NE(params, nullptr);
    EXPECT_EQ(chain.ApplyWaterGlass(params), DrawingError::ERR_NOT_PREPARED);
}

/**
 * @tc.name: WaterGlassReturnsErrorWhenForceCPU
 * @tc.desc: test ApplyWaterGlass returns error when not ForceCPU.
 */
HWTEST_F(EffectImageChainUnittest, WaterGlassReturnsErrorWhenForceCPU, TestSize.Level1)
{
    EffectImageChain chain;
    chain.prepared_ = true;
    Media::InitializationOptions opts;
    opts.size = { 1, 1 };
    std::shared_ptr<Media::PixelMap> srcPixelMap(Media::PixelMap::Create(opts));
    ASSERT_NE(srcPixelMap, nullptr);
    chain.Prepare(srcPixelMap, true);
    auto params = std::make_shared<Drawing::GEWaterGlassDataParams>();
    EXPECT_EQ(chain.ApplyWaterGlass(params), DrawingError::ERR_ILLEGAL_INPUT);
}

/**
 * @tc.name: WaterGlassReturnsOkWhenApplySuccessfully
 * @tc.desc: test ApplyWaterGlass returns ok when apply successfully.
 */
HWTEST_F(EffectImageChainUnittest, WaterGlassReturnsOkWhenApplySuccessfully, TestSize.Level1)
{
    EffectImageChain chain;
    chain.prepared_ = true;

    Media::InitializationOptions opts;
    opts.size = { 1, 1 };
    std::shared_ptr<Media::PixelMap> srcPixelMap(Media::PixelMap::Create(opts));
    ASSERT_NE(srcPixelMap, nullptr);
    chain.Prepare(srcPixelMap, false);

    auto params = std::make_shared<Drawing::GEWaterGlassDataParams>();
    ASSERT_NE(params, nullptr);
    EXPECT_EQ(chain.ApplyWaterGlass(params), DrawingError::ERR_OK);
}

/**
 * @tc.name: WaterGlassReturnsError
 * @tc.desc: test ApplyWaterGlass returns Error.
 */
HWTEST_F(EffectImageChainUnittest, WaterGlassReturnsError, TestSize.Level1)
{
    EffectImageChain chain;
    chain.prepared_ = true;

    Media::InitializationOptions opts;
    opts.size = { 1, 1 };
    std::shared_ptr<Media::PixelMap> srcPixelMap(Media::PixelMap::Create(opts));
    ASSERT_NE(srcPixelMap, nullptr);
    chain.Prepare(srcPixelMap, false);

    std::shared_ptr<Drawing::GEWaterGlassDataParams> params = nullptr;
    EXPECT_NE(chain.ApplyWaterGlass(params), DrawingError::ERR_OK);
}

/**
 * @tc.name: WaterGlassFilters
 * @tc.desc: test ApplyWaterGlass filters not nullptr.
 */
HWTEST_F(EffectImageChainUnittest, WaterGlassFilters, TestSize.Level1)
{
    EffectImageChain chain;
    chain.prepared_ = true;

    Media::InitializationOptions opts;
    opts.size = { 1, 1 };
    std::shared_ptr<Media::PixelMap> srcPixelMap(Media::PixelMap::Create(opts));
    ASSERT_NE(srcPixelMap, nullptr);
    chain.Prepare(srcPixelMap, false);

    auto filterBlur = Drawing::ImageFilter::CreateBlurImageFilter(1, 1, Drawing::TileMode::DECAL, nullptr);
    EXPECT_NE(filterBlur, nullptr);
    auto ret = chain.ApplyDrawingFilter(filterBlur);
    EXPECT_EQ(ret, DrawingError::ERR_OK);

    auto params = std::make_shared<Drawing::GEWaterGlassDataParams>();
    ASSERT_NE(params, nullptr);
    EXPECT_EQ(chain.ApplyWaterGlass(params), DrawingError::ERR_OK);
}

/**
 * @tc.name: ReededGlassReturnsError
 * @tc.desc: test ApplyReededGlass returns Error.
 */
HWTEST_F(EffectImageChainUnittest, ReededGlassReturnsError, TestSize.Level1)
{
    EffectImageChain chain;
    chain.prepared_ = true;

    Media::InitializationOptions opts;
    opts.size = { 1, 1 };
    std::shared_ptr<Media::PixelMap> srcPixelMap(Media::PixelMap::Create(opts));
    ASSERT_NE(srcPixelMap, nullptr);
    chain.Prepare(srcPixelMap, false);

    std::shared_ptr<Drawing::GEReededGlassDataParams> params = nullptr;
    EXPECT_NE(chain.ApplyReededGlass(params), DrawingError::ERR_OK);
}

/**
 * @tc.name: ReededGlassFilter
 * @tc.desc: test ApplyReededGlass filters not null.
 */
HWTEST_F(EffectImageChainUnittest, ReededGlassFilter, TestSize.Level1)
{
    EffectImageChain chain;
    chain.prepared_ = true;

    Media::InitializationOptions opts;
    opts.size = { 1, 1 };
    std::shared_ptr<Media::PixelMap> srcPixelMap(Media::PixelMap::Create(opts));
    ASSERT_NE(srcPixelMap, nullptr);
    chain.Prepare(srcPixelMap, false);

    auto filterBlur = Drawing::ImageFilter::CreateBlurImageFilter(1, 1, Drawing::TileMode::DECAL, nullptr);
    EXPECT_NE(filterBlur, nullptr);
    auto ret = chain.ApplyDrawingFilter(filterBlur);
    EXPECT_EQ(ret, DrawingError::ERR_OK);

    auto params = std::make_shared<Drawing::GEReededGlassDataParams>();
    ASSERT_NE(params, nullptr);
    EXPECT_EQ(chain.ApplyReededGlass(params), DrawingError::ERR_OK);
}

/**
 * @tc.name: ReededGlassReturnsErrorWhenNotPrepared
 * @tc.desc: test ApplyReededGlass returns error when not prepared.
 */
HWTEST_F(EffectImageChainUnittest, ReededGlassReturnsErrorWhenNotPrepared, TestSize.Level1)
{
    EffectImageChain chain;
    auto params = std::make_shared<Drawing::GEReededGlassDataParams>();
    ASSERT_NE(params, nullptr);
    EXPECT_EQ(chain.ApplyReededGlass(params), DrawingError::ERR_NOT_PREPARED);
}

/**
 * @tc.name: ReededGlassReturnsErrorWhenForceCPU
 * @tc.desc: test ApplyReededGlass returns error when not ForceCPU.
 */
HWTEST_F(EffectImageChainUnittest, ReededGlassReturnsErrorWhenForceCPU, TestSize.Level1)
{
    EffectImageChain chain;
    chain.prepared_ = true;
    Media::InitializationOptions opts;
    opts.size = { 1, 1 };
    std::shared_ptr<Media::PixelMap> srcPixelMap(Media::PixelMap::Create(opts));
    ASSERT_NE(srcPixelMap, nullptr);
    chain.Prepare(srcPixelMap, true);

    auto params = std::make_shared<Drawing::GEReededGlassDataParams>();
    ASSERT_NE(params, nullptr);
    EXPECT_EQ(chain.ApplyReededGlass(params), DrawingError::ERR_ILLEGAL_INPUT);
}

/**
 * @tc.name: ReededGlassReturnsOkWhenApplySuccessfully
 * @tc.desc: test ApplyReededGlass returns ok when apply successfully.
 */
HWTEST_F(EffectImageChainUnittest, ReededGlassReturnsOkWhenApplySuccessfully, TestSize.Level1)
{
    EffectImageChain chain;
    chain.prepared_ = true;

    Media::InitializationOptions opts;
    opts.size = { 1, 1 };
    std::shared_ptr<Media::PixelMap> srcPixelMap(Media::PixelMap::Create(opts));
    ASSERT_NE(srcPixelMap, nullptr);
    chain.Prepare(srcPixelMap, false);

    auto params = std::make_shared<Drawing::GEReededGlassDataParams>();
    ASSERT_NE(params, nullptr);
    EXPECT_EQ(chain.ApplyReededGlass(params), DrawingError::ERR_OK);
}

/**
 * @tc.name: ApplyMaskTransitionFilterTest001
 * @tc.desc: Test ApplyMaskTransitionFilter with null parameters
 */
HWTEST_F(EffectImageChainUnittest, ApplyMaskTransitionFilterTest001, TestSize.Level1)
{
    auto image = std::make_shared<EffectImageChain>();
    std::vector<float> colors = {0.0f, 1.0f};
    std::vector<float> positions = {0.0f, 1.0f};
    Drawing::GELinearGradientShaderMaskParams maskParams{
        {{0.0f, 1.0f}, {0.0f, 1.0f}}, Drawing::Point(0, 0), Drawing::Point(100, 100)};
    auto mask = std::static_pointer_cast<Drawing::GEShaderMask>(
        std::make_shared<Drawing::GELinearGradientShaderMask>(maskParams));

    // Test not prepared
    auto ret = image->ApplyMaskTransitionFilter(nullptr, mask, 0.5f, false);
    EXPECT_NE(ret, DrawingError::ERR_OK);

    // Prepare image
    Media::InitializationOptions opts;
    opts.size = {100, 100};
    std::shared_ptr<Media::PixelMap> srcPixelMap(Media::PixelMap::Create(opts));
    ASSERT_NE(srcPixelMap, nullptr);
    ret = image->Prepare(srcPixelMap, true);
    ASSERT_EQ(ret, DrawingError::ERR_OK);

    // Test with forceCPU = true
    ret = image->ApplyMaskTransitionFilter(nullptr, mask, 0.5f, false);
    EXPECT_NE(ret, DrawingError::ERR_OK);

    ret = image->Prepare(srcPixelMap, false);
    ASSERT_EQ(ret, DrawingError::ERR_OK);

    // Test with null top layer
    ret = image->ApplyMaskTransitionFilter(nullptr, mask, 0.5f, false);
    EXPECT_NE(ret, DrawingError::ERR_OK);
}

/**
 * @tc.name: ApplyMaskTransitionFilterTest002
 * @tc.desc: Test ApplyMaskTransitionFilter with valid parameters
 */
HWTEST_F(EffectImageChainUnittest, ApplyMaskTransitionFilterTest002, TestSize.Level1)
{
    auto image = std::make_shared<EffectImageChain>();
    Media::InitializationOptions opts;
    opts.size = {100, 100};
    std::shared_ptr<Media::PixelMap> srcPixelMap(Media::PixelMap::Create(opts));
    ASSERT_NE(srcPixelMap, nullptr);
    auto ret = image->Prepare(srcPixelMap, false);
    ASSERT_EQ(ret, DrawingError::ERR_OK);

    // Create top layer pixelmap
    std::shared_ptr<Media::PixelMap> topLayerPixelMap(Media::PixelMap::Create(opts));
    ASSERT_NE(topLayerPixelMap, nullptr);

    std::vector<std::pair<float, float>> fractionStops = {{0.0f, 0.0f}, {1.0f, 1.0f}};
    Drawing::GELinearGradientShaderMaskParams maskParams{
        fractionStops, Drawing::Point(0, 0), Drawing::Point(100, 100)};
    auto mask = std::static_pointer_cast<Drawing::GEShaderMask>(
        std::make_shared<Drawing::GELinearGradientShaderMask>(maskParams));

    ret = image->ApplyMaskTransitionFilter(topLayerPixelMap, mask, 0.5f, false);
    EXPECT_EQ(ret, DrawingError::ERR_OK);

    ret = image->Draw();
    EXPECT_EQ(ret, DrawingError::ERR_OK);
}

/**
 * @tc.name: ApplyMaskTransitionFilterTest003
 * @tc.desc: Test ApplyMaskTransitionFilter with filters_ already set (tests cascade path)
 */
HWTEST_F(EffectImageChainUnittest, ApplyMaskTransitionFilterTest003, TestSize.Level1)
{
    auto image = std::make_shared<EffectImageChain>();
    Media::InitializationOptions opts;
    opts.size = {100, 100};
    std::shared_ptr<Media::PixelMap> srcPixelMap(Media::PixelMap::Create(opts));
    ASSERT_NE(srcPixelMap, nullptr);
    auto ret = image->Prepare(srcPixelMap, false);
    ASSERT_EQ(ret, DrawingError::ERR_OK);

    // Add a drawing filter first to test the cascade path
    auto filterBlur = Drawing::ImageFilter::CreateBlurImageFilter(1.0f, 1.0f, Drawing::TileMode::CLAMP, nullptr);
    ASSERT_NE(filterBlur, nullptr);
    ret = image->ApplyDrawingFilter(filterBlur);
    ASSERT_EQ(ret, DrawingError::ERR_OK);

    // Create top layer pixelmap
    std::shared_ptr<Media::PixelMap> topLayerPixelMap(Media::PixelMap::Create(opts));
    ASSERT_NE(topLayerPixelMap, nullptr);

    std::vector<std::pair<float, float>> fractionStops = {{0.0f, 0.0f}, {1.0f, 1.0f}};
    Drawing::GELinearGradientShaderMaskParams maskParams{
        fractionStops, Drawing::Point(0, 0), Drawing::Point(100, 100)};
    auto mask = std::static_pointer_cast<Drawing::GEShaderMask>(
        std::make_shared<Drawing::GELinearGradientShaderMask>(maskParams));

    // This will trigger the cascade path where filters_ != nullptr
    ret = image->ApplyMaskTransitionFilter(topLayerPixelMap, mask, 0.5f, false);
    EXPECT_EQ(ret, DrawingError::ERR_OK);

    ret = image->Draw();
    EXPECT_EQ(ret, DrawingError::ERR_OK);
}

/**
 * @tc.name: ApplyMaskTransitionFilterTest004
 * @tc.desc: Test ApplyMaskTransitionFilter with non-increasing fractionStops y values
 */
HWTEST_F(EffectImageChainUnittest, ApplyMaskTransitionFilterTest004, TestSize.Level1)
{
    auto image = std::make_shared<EffectImageChain>();
    Media::InitializationOptions opts;
    opts.size = {100, 100};
    std::shared_ptr<Media::PixelMap> srcPixelMap(Media::PixelMap::Create(opts));
    ASSERT_NE(srcPixelMap, nullptr);
    auto ret = image->Prepare(srcPixelMap, false);
    ASSERT_EQ(ret, DrawingError::ERR_OK);

    // Create top layer pixelmap
    std::shared_ptr<Media::PixelMap> topLayerPixelMap(Media::PixelMap::Create(opts));
    ASSERT_NE(topLayerPixelMap, nullptr);

    // Test with non-increasing y values (should still work, validation happens in shader)
    std::vector<std::pair<float, float>> fractionStops = {{0.0f, 0.5f}, {1.0f, 0.2f}};
    Drawing::GELinearGradientShaderMaskParams maskParams{
        fractionStops, Drawing::Point(0, 0), Drawing::Point(100, 100)};
    auto mask = std::static_pointer_cast<Drawing::GEShaderMask>(
        std::make_shared<Drawing::GELinearGradientShaderMask>(maskParams));

    ret = image->ApplyMaskTransitionFilter(topLayerPixelMap, mask, 0.5f, false);
    // The API itself doesn't validate the increasing requirement, it happens in the shader
    EXPECT_EQ(ret, DrawingError::ERR_OK);
}

/**
 * @tc.name: ApplyWaterDropletTransitionFilterTest001
 * @tc.desc: Test ApplyWaterDropletTransitionFilter with null parameters
 */
HWTEST_F(EffectImageChainUnittest, ApplyWaterDropletTransitionFilterTest001, TestSize.Level1)
{
    auto image = std::make_shared<EffectImageChain>();
    Drawing::GEWaterDropletTransitionFilterParams params = {
        nullptr, false, 0.5f, 5.0f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f
    };
    auto waterDropletParams = std::make_shared<Drawing::GEWaterDropletTransitionFilterParams>(params);

    // Test not prepared
    auto ret = image->ApplyWaterDropletTransitionFilter(nullptr, waterDropletParams);
    EXPECT_NE(ret, DrawingError::ERR_OK);

    // Prepare image
    Media::InitializationOptions opts;
    opts.size = {100, 100};
    std::shared_ptr<Media::PixelMap> srcPixelMap(Media::PixelMap::Create(opts));
    ASSERT_NE(srcPixelMap, nullptr);
    ret = image->Prepare(srcPixelMap, true);
    ASSERT_EQ(ret, DrawingError::ERR_OK);

    // Test with forceCPU = true (should return ERR_ILLEGAL_INPUT)
    ret = image->ApplyWaterDropletTransitionFilter(nullptr, waterDropletParams);
    EXPECT_NE(ret, DrawingError::ERR_OK);

    ret = image->Prepare(srcPixelMap, false);
    ASSERT_EQ(ret, DrawingError::ERR_OK);

    // Test with null top layer
    ret = image->ApplyWaterDropletTransitionFilter(nullptr, waterDropletParams);
    EXPECT_NE(ret, DrawingError::ERR_OK);

    // Test with null params
    ret = image->ApplyWaterDropletTransitionFilter(srcPixelMap, nullptr);
    EXPECT_NE(ret, DrawingError::ERR_OK);
}

/**
 * @tc.name: ApplyWaterDropletTransitionFilterTest002
 * @tc.desc: Test ApplyWaterDropletTransitionFilter with valid parameters
 */
HWTEST_F(EffectImageChainUnittest, ApplyWaterDropletTransitionFilterTest002, TestSize.Level1)
{
    auto image = std::make_shared<EffectImageChain>();
    Media::InitializationOptions opts;
    opts.size = {100, 100};
    std::shared_ptr<Media::PixelMap> srcPixelMap(Media::PixelMap::Create(opts));
    ASSERT_NE(srcPixelMap, nullptr);
    auto ret = image->Prepare(srcPixelMap, false);
    ASSERT_EQ(ret, DrawingError::ERR_OK);

    // Create top layer pixelmap
    std::shared_ptr<Media::PixelMap> topLayerPixelMap(Media::PixelMap::Create(opts));
    ASSERT_NE(topLayerPixelMap, nullptr);

    Drawing::GEWaterDropletTransitionFilterParams params = {
        nullptr, false, 0.5f, 5.0f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f
    };
    auto waterDropletParams = std::make_shared<Drawing::GEWaterDropletTransitionFilterParams>(params);

    // GE plugin not loaded in test env, GenerateWaterDropletTransitionFilter returns nullptr
    ret = image->ApplyWaterDropletTransitionFilter(topLayerPixelMap, waterDropletParams);
    EXPECT_EQ(ret, DrawingError::ERR_ILLEGAL_INPUT);

    ret = image->Draw();
    EXPECT_EQ(ret, DrawingError::ERR_OK);
}

/**
 * @tc.name: ApplyWaterDropletTransitionFilterTest003
 * @tc.desc: Test ApplyWaterDropletTransitionFilter with filters_ already set (tests cascade path)
 */
HWTEST_F(EffectImageChainUnittest, ApplyWaterDropletTransitionFilterTest003, TestSize.Level1)
{
    auto image = std::make_shared<EffectImageChain>();
    Media::InitializationOptions opts;
    opts.size = {100, 100};
    std::shared_ptr<Media::PixelMap> srcPixelMap(Media::PixelMap::Create(opts));
    ASSERT_NE(srcPixelMap, nullptr);
    auto ret = image->Prepare(srcPixelMap, false);
    ASSERT_EQ(ret, DrawingError::ERR_OK);

    // Add a drawing filter first to test the cascade path
    auto filterBlur = Drawing::ImageFilter::CreateBlurImageFilter(1.0f, 1.0f, Drawing::TileMode::CLAMP, nullptr);
    ASSERT_NE(filterBlur, nullptr);
    ret = image->ApplyDrawingFilter(filterBlur);
    ASSERT_EQ(ret, DrawingError::ERR_OK);

    // Create top layer pixelmap
    std::shared_ptr<Media::PixelMap> topLayerPixelMap(Media::PixelMap::Create(opts));
    ASSERT_NE(topLayerPixelMap, nullptr);

    Drawing::GEWaterDropletTransitionFilterParams params = {
        nullptr, false, 0.5f, 5.0f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f
    };
    auto waterDropletParams = std::make_shared<Drawing::GEWaterDropletTransitionFilterParams>(params);

    // GE plugin not loaded in test env, GenerateWaterDropletTransitionFilter returns nullptr
    ret = image->ApplyWaterDropletTransitionFilter(topLayerPixelMap, waterDropletParams);
    EXPECT_EQ(ret, DrawingError::ERR_ILLEGAL_INPUT);

    ret = image->Draw();
    EXPECT_EQ(ret, DrawingError::ERR_OK);
}

/**
* @tc.name: ApplyScaleWithInvalidScaleX001
* @tc.desc: Test EffectImageChain::ApplyScale with invalid scaleX (zero)
* @tc.type: FUNC
*/
HWTEST_F(EffectImageChainUnittest, ApplyScaleWithInvalidScaleX001, TestSize.Level1)
{
    auto image = std::make_shared<EffectImageChain>();
    ASSERT_NE(image, nullptr);

    Media::InitializationOptions opts;
    opts.size = {100, 100};
    std::shared_ptr<Media::PixelMap> srcPixelMap(Media::PixelMap::Create(opts));
    ASSERT_NE(srcPixelMap, nullptr);
    auto ret = image->Prepare(srcPixelMap, false);
    ASSERT_EQ(ret, DrawingError::ERR_OK);

    // Test with scaleX = 0
    ret = image->ApplyScale(0.0f, 1.0f,
        Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    EXPECT_NE(ret, DrawingError::ERR_OK);
}

/**
* @tc.name: ApplyScaleWithInvalidScaleX002
* @tc.desc: Test EffectImageChain::ApplyScale with invalid scaleX (negative)
* @tc.type: FUNC
*/
HWTEST_F(EffectImageChainUnittest, ApplyScaleWithInvalidScaleX002, TestSize.Level1)
{
    auto image = std::make_shared<EffectImageChain>();
    ASSERT_NE(image, nullptr);

    Media::InitializationOptions opts;
    opts.size = {100, 100};
    std::shared_ptr<Media::PixelMap> srcPixelMap(Media::PixelMap::Create(opts));
    ASSERT_NE(srcPixelMap, nullptr);
    auto ret = image->Prepare(srcPixelMap, false);
    ASSERT_EQ(ret, DrawingError::ERR_OK);

    // Test with negative scaleX
    ret = image->ApplyScale(-0.5f, 1.0f,
        Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    EXPECT_NE(ret, DrawingError::ERR_OK);
}

/**
* @tc.name: ApplyScaleWithInvalidScaleX003
* @tc.desc: Test EffectImageChain::ApplyScale with invalid scaleX (very negative)
* @tc.type: FUNC
*/
HWTEST_F(EffectImageChainUnittest, ApplyScaleWithInvalidScaleX003, TestSize.Level1)
{
    auto image = std::make_shared<EffectImageChain>();
    ASSERT_NE(image, nullptr);

    Media::InitializationOptions opts;
    opts.size = {100, 100};
    std::shared_ptr<Media::PixelMap> srcPixelMap(Media::PixelMap::Create(opts));
    ASSERT_NE(srcPixelMap, nullptr);
    auto ret = image->Prepare(srcPixelMap, false);
    ASSERT_EQ(ret, DrawingError::ERR_OK);

    // Test with very negative scaleX
    ret = image->ApplyScale(-100.0f, 1.0f,
        Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    EXPECT_NE(ret, DrawingError::ERR_OK);
}

/**
* @tc.name: ApplyScaleWithInvalidScaleY001
* @tc.desc: Test EffectImageChain::ApplyScale with invalid scaleY (zero)
* @tc.type: FUNC
*/
HWTEST_F(EffectImageChainUnittest, ApplyScaleWithInvalidScaleY001, TestSize.Level1)
{
    auto image = std::make_shared<EffectImageChain>();
    ASSERT_NE(image, nullptr);

    Media::InitializationOptions opts;
    opts.size = {100, 100};
    std::shared_ptr<Media::PixelMap> srcPixelMap(Media::PixelMap::Create(opts));
    ASSERT_NE(srcPixelMap, nullptr);
    auto ret = image->Prepare(srcPixelMap, false);
    ASSERT_EQ(ret, DrawingError::ERR_OK);

    // Test with scaleY = 0
    ret = image->ApplyScale(1.0f, 0.0f,
        Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    EXPECT_NE(ret, DrawingError::ERR_OK);
}

/**
* @tc.name: ApplyScaleWithInvalidScaleY002
* @tc.desc: Test EffectImageChain::ApplyScale with invalid scaleY (negative)
* @tc.type: FUNC
*/
HWTEST_F(EffectImageChainUnittest, ApplyScaleWithInvalidScaleY002, TestSize.Level1)
{
    auto image = std::make_shared<EffectImageChain>();
    ASSERT_NE(image, nullptr);

    Media::InitializationOptions opts;
    opts.size = {100, 100};
    std::shared_ptr<Media::PixelMap> srcPixelMap(Media::PixelMap::Create(opts));
    ASSERT_NE(srcPixelMap, nullptr);
    auto ret = image->Prepare(srcPixelMap, false);
    ASSERT_EQ(ret, DrawingError::ERR_OK);

    // Test with negative scaleY
    ret = image->ApplyScale(1.0f, -0.5f,
        Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    EXPECT_NE(ret, DrawingError::ERR_OK);
}

/**
* @tc.name: ApplyScaleWithInvalidScaleY003
* @tc.desc: Test EffectImageChain::ApplyScale with invalid scaleY (very negative)
* @tc.type: FUNC
*/
HWTEST_F(EffectImageChainUnittest, ApplyScaleWithInvalidScaleY003, TestSize.Level1)
{
    auto image = std::make_shared<EffectImageChain>();
    ASSERT_NE(image, nullptr);

    Media::InitializationOptions opts;
    opts.size = {100, 100};
    std::shared_ptr<Media::PixelMap> srcPixelMap(Media::PixelMap::Create(opts));
    ASSERT_NE(srcPixelMap, nullptr);
    auto ret = image->Prepare(srcPixelMap, false);
    ASSERT_EQ(ret, DrawingError::ERR_OK);

    // Test with very negative scaleY
    ret = image->ApplyScale(1.0f, -100.0f,
        Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    EXPECT_NE(ret, DrawingError::ERR_OK);
}

/**
* @tc.name: ApplyScaleWithBothInvalidScales001
* @tc.desc: Test EffectImageChain::ApplyScale with both scales zero
* @tc.type: FUNC
*/
HWTEST_F(EffectImageChainUnittest, ApplyScaleWithBothInvalidScales001, TestSize.Level1)
{
    auto image = std::make_shared<EffectImageChain>();
    ASSERT_NE(image, nullptr);

    Media::InitializationOptions opts;
    opts.size = {100, 100};
    std::shared_ptr<Media::PixelMap> srcPixelMap(Media::PixelMap::Create(opts));
    ASSERT_NE(srcPixelMap, nullptr);
    auto ret = image->Prepare(srcPixelMap, false);
    ASSERT_EQ(ret, DrawingError::ERR_OK);

    // Test with both scales = 0
    ret = image->ApplyScale(0.0f, 0.0f,
        Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    EXPECT_NE(ret, DrawingError::ERR_OK);
}

/**
* @tc.name: ApplyScaleWithBothInvalidScales002
* @tc.desc: Test EffectImageChain::ApplyScale with both scales negative
* @tc.type: FUNC
*/
HWTEST_F(EffectImageChainUnittest, ApplyScaleWithBothInvalidScales002, TestSize.Level1)
{
    auto image = std::make_shared<EffectImageChain>();
    ASSERT_NE(image, nullptr);

    Media::InitializationOptions opts;
    opts.size = {100, 100};
    std::shared_ptr<Media::PixelMap> srcPixelMap(Media::PixelMap::Create(opts));
    ASSERT_NE(srcPixelMap, nullptr);
    auto ret = image->Prepare(srcPixelMap, false);
    ASSERT_EQ(ret, DrawingError::ERR_OK);

    // Test with both scales negative
    ret = image->ApplyScale(-1.0f, -1.0f,
        Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    EXPECT_NE(ret, DrawingError::ERR_OK);
}

/**
* @tc.name: ApplyScaleWithoutPrepare001
* @tc.desc: Test EffectImageChain::ApplyScale without prepare
* @tc.type: FUNC
*/
HWTEST_F(EffectImageChainUnittest, ApplyScaleWithoutPrepare001, TestSize.Level1)
{
    auto image = std::make_shared<EffectImageChain>();
    ASSERT_NE(image, nullptr);

    // Test without prepare
    auto ret = image->ApplyScale(1.0f, 1.0f,
        Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    EXPECT_EQ(ret, DrawingError::ERR_OK);
}

/**
* @tc.name: ApplyScaleWithCPU001
* @tc.desc: Test EffectImageChain::ApplyScale with CPU mode (not supported)
* @tc.type: FUNC
*/
HWTEST_F(EffectImageChainUnittest, ApplyScaleWithCPU001, TestSize.Level1)
{
    auto image = std::make_shared<EffectImageChain>();
    ASSERT_NE(image, nullptr);

    Media::InitializationOptions opts;
    opts.size = {100, 100};
    std::shared_ptr<Media::PixelMap> srcPixelMap(Media::PixelMap::Create(opts));
    ASSERT_NE(srcPixelMap, nullptr);
    auto ret = image->Prepare(srcPixelMap, true);
    ASSERT_EQ(ret, DrawingError::ERR_OK);

    // Test with CPU mode (should fail)
    ret = image->ApplyScale(1.5f, 0.8f,
        Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    EXPECT_NE(ret, DrawingError::ERR_OK);
}

/**
* @tc.name: ApplyScaleWithExistingFilters001
* @tc.desc: Test EffectImageChain::ApplyScale when filters already exist
* @tc.type: FUNC
*/
HWTEST_F(EffectImageChainUnittest, ApplyScaleWithExistingFilters001, TestSize.Level1)
{
    auto image = std::make_shared<EffectImageChain>();
    ASSERT_NE(image, nullptr);

    Media::InitializationOptions opts;
    opts.size = {100, 100};
    std::shared_ptr<Media::PixelMap> srcPixelMap(Media::PixelMap::Create(opts));
    ASSERT_NE(srcPixelMap, nullptr);
    auto ret = image->Prepare(srcPixelMap, false);
    ASSERT_EQ(ret, DrawingError::ERR_OK);

    // Apply a drawing filter first
    auto filter = Drawing::ImageFilter::CreateBlurImageFilter(1.0f, 1.0f,
        Drawing::TileMode::CLAMP, nullptr);
    ASSERT_NE(filter, nullptr);
    ret = image->ApplyDrawingFilter(filter);
    ASSERT_EQ(ret, DrawingError::ERR_OK);

    // Now apply scale - should handle existing filters by calling UpdateImage first
    ret = image->ApplyScale(0.5f, 0.5f,
        Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    EXPECT_EQ(ret, DrawingError::ERR_OK);
}

/**
* @tc.name: ApplyScaleSuccess001
* @tc.desc: Test EffectImageChain::ApplyScale success case (GPU mode)
* @tc.type: FUNC
*/
HWTEST_F(EffectImageChainUnittest, ApplyScaleSuccess001, TestSize.Level1)
{
    auto image = std::make_shared<EffectImageChain>();
    ASSERT_NE(image, nullptr);

    Media::InitializationOptions opts;
    opts.size = {100, 100};
    std::shared_ptr<Media::PixelMap> srcPixelMap(Media::PixelMap::Create(opts));
    ASSERT_NE(srcPixelMap, nullptr);
    auto ret = image->Prepare(srcPixelMap, false);
    ASSERT_EQ(ret, DrawingError::ERR_OK);

    // Test with valid scales in GPU mode
    ret = image->ApplyScale(1.5f, 0.8f,
        Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    EXPECT_EQ(ret, DrawingError::ERR_OK);
}

/**
* @tc.name: ApplyScaleSuccess002
* @tc.desc: Test EffectImageChain::ApplyScale with different filter and mipmap modes
* @tc.type: FUNC
*/
HWTEST_F(EffectImageChainUnittest, ApplyScaleSuccess002, TestSize.Level1)
{
    auto image = std::make_shared<EffectImageChain>();
    ASSERT_NE(image, nullptr);

    Media::InitializationOptions opts;
    opts.size = {100, 100};
    std::shared_ptr<Media::PixelMap> srcPixelMap(Media::PixelMap::Create(opts));
    ASSERT_NE(srcPixelMap, nullptr);
    auto ret = image->Prepare(srcPixelMap, false);
    ASSERT_EQ(ret, DrawingError::ERR_OK);

    // Test with NEAREST filter mode
    ret = image->ApplyScale(0.5f, 0.5f,
        Drawing::FilterMode::NEAREST, Drawing::MipmapMode::NONE);
    EXPECT_EQ(ret, DrawingError::ERR_OK);

    // Test with LINEAR mipmap mode
    ret = image->ApplyScale(1.0f, 1.0f,
        Drawing::FilterMode::LINEAR, Drawing::MipmapMode::LINEAR);
    EXPECT_EQ(ret, DrawingError::ERR_OK);
}

/**
* @tc.name: ApplyScaleSuccess003
* @tc.desc: Test EffectImageChain::ApplyScale with upscale
* @tc.type: FUNC
*/
HWTEST_F(EffectImageChainUnittest, ApplyScaleSuccess003, TestSize.Level1)
{
    auto image = std::make_shared<EffectImageChain>();
    ASSERT_NE(image, nullptr);

    Media::InitializationOptions opts;
    opts.size = {100, 100};
    std::shared_ptr<Media::PixelMap> srcPixelMap(Media::PixelMap::Create(opts));
    ASSERT_NE(srcPixelMap, nullptr);
    auto ret = image->Prepare(srcPixelMap, false);
    ASSERT_EQ(ret, DrawingError::ERR_OK);

    // Test with upscale
    ret = image->ApplyScale(2.0f, 2.0f,
        Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    EXPECT_EQ(ret, DrawingError::ERR_OK);
}

/**
* @tc.name: ApplyScaleSuccess004
* @tc.desc: Test EffectImageChain::ApplyScale with downscale
* @tc.type: FUNC
*/
HWTEST_F(EffectImageChainUnittest, ApplyScaleSuccess004, TestSize.Level1)
{
    auto image = std::make_shared<EffectImageChain>();
    ASSERT_NE(image, nullptr);

    Media::InitializationOptions opts;
    opts.size = {100, 100};
    std::shared_ptr<Media::PixelMap> srcPixelMap(Media::PixelMap::Create(opts));
    ASSERT_NE(srcPixelMap, nullptr);
    auto ret = image->Prepare(srcPixelMap, false);
    ASSERT_EQ(ret, DrawingError::ERR_OK);

    // Test with downscale
    ret = image->ApplyScale(0.5f, 0.5f,
        Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    EXPECT_EQ(ret, DrawingError::ERR_OK);
}

/**
* @tc.name: ApplyScaleSuccess005
* @tc.desc: Test EffectImageChain::ApplyScale with very small scales
* @tc.type: FUNC
*/
HWTEST_F(EffectImageChainUnittest, ApplyScaleSuccess005, TestSize.Level1)
{
    auto image = std::make_shared<EffectImageChain>();
    ASSERT_NE(image, nullptr);

    Media::InitializationOptions opts;
    opts.size = {100, 100};
    std::shared_ptr<Media::PixelMap> srcPixelMap(Media::PixelMap::Create(opts));
    ASSERT_NE(srcPixelMap, nullptr);
    auto ret = image->Prepare(srcPixelMap, false);
    ASSERT_EQ(ret, DrawingError::ERR_OK);

    // Test with very small but valid scales
    ret = image->ApplyScale(0.001f, 0.001f,
        Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    EXPECT_EQ(ret, DrawingError::ERR_OK);
}

/**
* @tc.name: ApplyScaleSuccess006
* @tc.desc: Test EffectImageChain::ApplyScale with very large scales
* @tc.type: FUNC
*/
HWTEST_F(EffectImageChainUnittest, ApplyScaleSuccess006, TestSize.Level1)
{
    auto image = std::make_shared<EffectImageChain>();
    ASSERT_NE(image, nullptr);

    Media::InitializationOptions opts;
    opts.size = {100, 100};
    std::shared_ptr<Media::PixelMap> srcPixelMap(Media::PixelMap::Create(opts));
    ASSERT_NE(srcPixelMap, nullptr);
    auto ret = image->Prepare(srcPixelMap, false);
    ASSERT_EQ(ret, DrawingError::ERR_OK);

    // Test with very large scales
    ret = image->ApplyScale(100.0f, 100.0f,
        Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    EXPECT_EQ(ret, DrawingError::ERR_OK);
}

/**
* @tc.name: DrawNativeBufferWithScaling001
* @tc.desc: Test EffectImageChain::DrawNativeBuffer when canvas and surface sizes differ
* @tc.type: FUNC
*/
HWTEST_F(EffectImageChainUnittest, DrawNativeBufferWithScaling001, TestSize.Level1)
{
    auto image = std::make_shared<EffectImageChain>();
    ASSERT_NE(image, nullptr);

    const auto width = 200;
    const auto height = 200;

    Media::InitializationOptions opts = {
        .size = {
            .width = static_cast<int32_t>(width),
            .height = static_cast<int32_t>(height),
        },
        .srcPixelFormat = OHOS::Media::PixelFormat::RGBA_8888,
        .pixelFormat = OHOS::Media::PixelFormat::RGBA_8888,
        .alphaType = OHOS::Media::AlphaType::IMAGE_ALPHA_TYPE_PREMUL,
    };

    std::shared_ptr<Media::PixelMap> srcPixelMap = Media::PixelMap::Create(opts);
    ASSERT_NE(srcPixelMap, nullptr);

    // Create native buffer with different size to trigger scaling
    OH_NativeBuffer_Config config {
        .width = width * 2,  // Different size
        .height = height * 2,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA
    };
    OH_NativeBuffer* dstBuffer = OH_NativeBuffer_Alloc(&config);

    std::shared_ptr<OH_NativeBuffer> dst(
        dstBuffer,
        [](OH_NativeBuffer* buffer) {}
    );

    auto ret = image->PrepareNativeBuffer(srcPixelMap, dst);
    ASSERT_EQ(ret, DrawingError::ERR_OK);

    // This should trigger scaling in DrawNativeBuffer
    ret = image->DrawNativeBuffer();
    EXPECT_EQ(ret, DrawingError::ERR_OK);

    OH_NativeBuffer_Unreference(dstBuffer);
}

/**
 * @tc.name: PrepareStateAfterPrepareTest001
 * @tc.desc: test state after Prepare and ScaleCanvas
 */
HWTEST_F(EffectImageChainUnittest, PrepareStateAfterPrepareTest001, TestSize.Level1)
{
    auto image = std::make_shared<EffectImageChain>();
    ASSERT_NE(image, nullptr);

    Media::InitializationOptions opts;
    opts.size = { 1, 1 };
    std::shared_ptr<Media::PixelMap> srcPixelMap(Media::PixelMap::Create(opts));
    ASSERT_NE(srcPixelMap, nullptr);
    auto ret = image->Prepare(srcPixelMap, true);
    ASSERT_EQ(ret, DrawingError::ERR_OK);
    EXPECT_NE(image->image_, nullptr);
    EXPECT_GT(image->canvasRec_.GetRight(), 0.0f);
    EXPECT_GT(image->canvasRec_.GetBottom(), 0.0f);
    image->ScaleCanvas(0.5f, 0.5f);

    ret = image->Prepare(srcPixelMap, false);
    ASSERT_EQ(ret, DrawingError::ERR_OK);
    EXPECT_NE(image->image_, nullptr);
}

/**
 * @tc.name: PrepareNativeBufferFenceAndGpuContextTest001
 * @tc.desc: test GetfenceId and gpuContext_ after PrepareNativeBuffer
 */
HWTEST_F(EffectImageChainUnittest, PrepareNativeBufferFenceAndGpuContextTest001, TestSize.Level1)
{
    auto image = std::make_shared<EffectImageChain>();
    ASSERT_NE(image, nullptr);
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
    ASSERT_NE(srcPixelMap, nullptr);

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

    auto ret = image->PrepareNativeBuffer(srcPixelMap, dst);
    ASSERT_EQ(ret, DrawingError::ERR_OK);
    EXPECT_EQ(image->GetfenceId(), -1);
    image->fenceId_ = 42;
    EXPECT_EQ(image->GetfenceId(), 42);
    image->gpuContext_ = std::make_shared<Drawing::GPUContext>();
    EXPECT_NE(image->gpuContext_, nullptr);
    OH_NativeBuffer_Unreference(dstBuffer);
}

/**
 * @tc.name: ScaleCanvasWithNullCanvasTest001
 * @tc.desc: test ScaleCanvas with null canvas
 */
HWTEST_F(EffectImageChainUnittest, ScaleCanvasWithNullCanvasTest001, TestSize.Level1)
{
    EffectImageChain nullCanvasChain;
    nullCanvasChain.canvas_ = nullptr;
    nullCanvasChain.canvasRec_ = Drawing::Rect(0, 0, 100.0f, 100.0f);
    nullCanvasChain.ScaleCanvas(0.5f, 0.5f);
    EXPECT_EQ(nullCanvasChain.canvasRec_.GetRight(), 100.0f);
    EXPECT_EQ(nullCanvasChain.canvasRec_.GetBottom(), 100.0f);
}

/**
 * @tc.name: UpdateCanvasWithInvalidRectTest001
 * @tc.desc: test UpdateCanvas with invalid rects
 */
HWTEST_F(EffectImageChainUnittest, UpdateCanvasWithInvalidRectTest001, TestSize.Level1)
{
    EffectImageChain invalidCanvasChain;
    invalidCanvasChain.canvasRec_ = Drawing::Rect(0, 0, 0.0f, 0.0f);
    invalidCanvasChain.canvas_ = nullptr;
    invalidCanvasChain.UpdateCanvas();
    EXPECT_EQ(invalidCanvasChain.canvasRec_.GetRight(), 0.0f);
    invalidCanvasChain.canvasRec_ = Drawing::Rect(0, 0, -1.0f, 100.0f);
    invalidCanvasChain.UpdateCanvas();
    EXPECT_EQ(invalidCanvasChain.canvasRec_.GetRight(), -1.0f);
    invalidCanvasChain.canvasRec_ = Drawing::Rect(0, 0, 100.0f, -1.0f);
    invalidCanvasChain.UpdateCanvas();
    EXPECT_EQ(invalidCanvasChain.canvasRec_.GetBottom(), -1.0f);
    invalidCanvasChain.canvasRec_ = Drawing::Rect(0, 0, 1e-38f, 1e-38f);
    invalidCanvasChain.UpdateCanvas();
    EXPECT_EQ(invalidCanvasChain.canvasRec_.GetRight(), 1e-38f);
}

/**
 * @tc.name: InitWithoutCanvasImageNullTest001
 * @tc.desc: test InitWithoutCanvas returns ERR_IMAGE_NULL when ExtractDrawingImage returns nullptr
 */
HWTEST_F(EffectImageChainUnittest, InitWithoutCanvasImageNullTest001, TestSize.Level1)
{
#if defined(RS_ENABLE_GPU) && !defined(ROSEN_ARKUI_X)
    Media::InitializationOptions opts;
    opts.size = { 1, 1 };
    std::shared_ptr<Media::PixelMap> srcPixelMap(Media::PixelMap::Create(opts));
    ASSERT_NE(srcPixelMap, nullptr);
    srcPixelMap->imageInfo_.pixelFormat = Media::PixelFormat::CMYK;

    EffectImageChain chain;
    chain.srcPixelMap_ = srcPixelMap;
    auto ret = chain.InitWithoutCanvas(srcPixelMap);
    EXPECT_EQ(ret, DrawingError::ERR_IMAGE_NULL);
    EXPECT_EQ(chain.image_, nullptr);
#else
    GTEST_SKIP() << "Skip: InitWithoutCanvas image_==nullptr branch only exists under RS_ENABLE_GPU&&!ROSEN_ARKUI_X";
#endif
}

/**
 * @tc.name: EffectImageChainUnittest.GpuContextThreadLocalHoldsContext
 * @tc.desc: test the thread-local gpuContext_ keeps the GPUContext alive
 */
HWTEST_F(EffectImageChainUnittest, GpuContextThreadLocalHoldsContext, TestSize.Level1)
{
    EffectImageChain::gpuContext_.reset();
    EXPECT_EQ(EffectImageChain::gpuContext_, nullptr);

    auto ctx = std::make_shared<Drawing::GPUContext>();
    EffectImageChain::gpuContext_ = ctx;
    EXPECT_EQ(EffectImageChain::gpuContext_.get(), ctx.get());
    EXPECT_EQ(EffectImageChain::gpuContext_, ctx);

    // thread-local cache holds the GPUContext even after the original holder is gone
    ctx = nullptr;
    EXPECT_NE(EffectImageChain::gpuContext_, nullptr);

    EffectImageChain::gpuContext_.reset();
    EXPECT_EQ(EffectImageChain::gpuContext_, nullptr);
}

/**
 * @tc.name: EffectImageChainUnittest.ProcessVkContextLazyCreateAndClear
 * @tc.desc: test EnsureVkContext lazily creates the process-wide EFFECT_KIT context once
 */
HWTEST_F(EffectImageChainUnittest, ProcessVkContextLazyCreateAndClear, TestSize.Level1)
{
    RsVulkanContext* stale = EffectImageChain::processVkContext_.exchange(nullptr);
    EffectImageChain::ReleaseVkContext(stale);
    EXPECT_EQ(EffectImageChain::processVkContext_.load(), nullptr);

    EffectImageChain::EnsureVkContext();
    RsVulkanContext* created = EffectImageChain::processVkContext_.load();
    ASSERT_NE(created, nullptr);

    EffectImageChain::EnsureVkContext();
    EXPECT_EQ(EffectImageChain::processVkContext_.load(), created);

    EffectImageChain::processVkContext_.store(nullptr);
}

/**
 * @tc.name: EffectImageChainUnittest.ReleaseCachesIdempotentWhenEmpty
 * @tc.desc: test ReleaseCaches on empty
 */
HWTEST_F(EffectImageChainUnittest, ReleaseCachesIdempotentWhenEmpty, TestSize.Level1)
{
    int32_t errorCode = -1;
    EffectImageChain::ReleaseCaches(&errorCode);
    EXPECT_TRUE(errorCode == EFFECT_NO_GPU_ENVIRONMENT || errorCode == 0);

    int32_t errorCodeAgain = -1;
    EffectImageChain::ReleaseCaches(&errorCodeAgain);
    EXPECT_TRUE(errorCodeAgain == EFFECT_NO_GPU_ENVIRONMENT || errorCodeAgain == 0);

    int32_t threadErrorCode = -1;
    EffectImageChain::ReleaseGpuContext(&threadErrorCode);
    EXPECT_TRUE(threadErrorCode == EFFECT_NO_GPU_ENVIRONMENT || threadErrorCode == 0 ||
        threadErrorCode == EFFECT_RESOURCE_RELEASE_ERROR);
}

/**
 * @tc.name: EffectImageChainUnittest.ReleaseCachesZeroesGpuResources
 * @tc.desc: test ReleaseCaches destroys the process device;
 */
HWTEST_F(EffectImageChainUnittest, ReleaseCachesZeroesGpuResources, TestSize.Level1)
{
#if defined(RS_ENABLE_GPU) && !defined(ROSEN_ARKUI_X)
    EffectImageChain::gpuContext_.reset();
    Media::InitializationOptions opts;
    opts.size = { 4, 4 };
    std::shared_ptr<Media::PixelMap> srcPixelMap(Media::PixelMap::Create(opts));
    ASSERT_NE(srcPixelMap, nullptr);
    {
        EffectImageChain chain;
        ASSERT_EQ(chain.Prepare(srcPixelMap, false), DrawingError::ERR_OK);
        ASSERT_EQ(chain.ApplyBlur(TEST_BLUR_RADIUS, Drawing::TileMode::CLAMP), DrawingError::ERR_OK);
        ASSERT_EQ(chain.Draw(), DrawingError::ERR_OK);
        ASSERT_NE(EffectImageChain::gpuContext_, nullptr);
        ASSERT_NE(EffectImageChain::processVkContext_.load(), nullptr);
    }
    // chain destruction must not free the cached GPUContext held by the thread-local
    ASSERT_TRUE(EffectImageChain::gpuEnvInitialized_.load());
    EXPECT_NE(EffectImageChain::gpuContext_, nullptr);
    ASSERT_NE(EffectImageChain::processVkContext_.load(), nullptr);

    int32_t errorCode = -1;
    EffectImageChain::ReleaseCaches(&errorCode);
    ASSERT_TRUE(errorCode == 0 || errorCode == EFFECT_RESOURCE_RELEASE_ERROR);
    EXPECT_EQ(EffectImageChain::gpuContext_, nullptr);
    EXPECT_EQ(EffectImageChain::processVkContext_.load(), nullptr);
#else
    GTEST_SKIP() << "Skip: GPU release path requires RS_ENABLE_GPU && !ROSEN_ARKUI_X";
#endif
}

/**
 * @tc.name: EffectImageChainUnittest.ReleaseGpuContextZeroesThreadResources
 * @tc.desc: test ReleaseGpuContext drops only the calling thread's context and keeps the process device (AC-2.1)
 */
HWTEST_F(EffectImageChainUnittest, ReleaseGpuContextZeroesThreadResources, TestSize.Level1)
{
#if defined(RS_ENABLE_GPU) && !defined(ROSEN_ARKUI_X)
    EffectImageChain::gpuContext_.reset();
    Media::InitializationOptions opts;
    opts.size = { 4, 4 };
    std::shared_ptr<Media::PixelMap> srcPixelMap(Media::PixelMap::Create(opts));
    ASSERT_NE(srcPixelMap, nullptr);
    {
        EffectImageChain chain;
        ASSERT_EQ(chain.Prepare(srcPixelMap, false), DrawingError::ERR_OK);
        ASSERT_EQ(chain.Draw(), DrawingError::ERR_OK);
        ASSERT_NE(EffectImageChain::gpuContext_, nullptr);
    }
    // chain destruction must not free the cached GPUContext held by the thread-local
    ASSERT_TRUE(EffectImageChain::gpuEnvInitialized_.load());
    EXPECT_NE(EffectImageChain::gpuContext_, nullptr);

    int32_t errorCode = -1;
    EffectImageChain::ReleaseGpuContext(&errorCode);
    ASSERT_TRUE(errorCode == 0 || errorCode == EFFECT_RESOURCE_RELEASE_ERROR);
    EXPECT_EQ(EffectImageChain::gpuContext_, nullptr);
    EXPECT_NE(EffectImageChain::processVkContext_.load(), nullptr);
#else
    GTEST_SKIP() << "Skip: GPU release path requires RS_ENABLE_GPU && !ROSEN_ARKUI_X";
#endif
}

/**
 * @tc.name: EffectImageChainUnittest.RenderAfterReleaseProducesSameOutput
 * @tc.desc: test re-rendering after ReleaseCaches produces pixel-identical output (AC-1.3/AC-2.2)
 */
HWTEST_F(EffectImageChainUnittest, RenderAfterReleaseProducesSameOutput, TestSize.Level1)
{
#if defined(RS_ENABLE_GPU) && !defined(ROSEN_ARKUI_X)
    Media::InitializationOptions opts;
    opts.size = { 4, 4 };
    std::shared_ptr<Media::PixelMap> srcPixelMap(Media::PixelMap::Create(opts));
    ASSERT_NE(srcPixelMap, nullptr);

    std::shared_ptr<Media::PixelMap> baseline;
    {
        EffectImageChain chain;
        ASSERT_EQ(chain.Prepare(srcPixelMap, false), DrawingError::ERR_OK);
        ASSERT_EQ(chain.ApplyBlur(TEST_BLUR_RADIUS, Drawing::TileMode::CLAMP), DrawingError::ERR_OK);
        ASSERT_EQ(chain.Draw(), DrawingError::ERR_OK);
        baseline = chain.GetPixelMap();
        ASSERT_NE(baseline, nullptr);
    }

    int32_t errorCode = -1;
    EffectImageChain::ReleaseCaches(&errorCode);
    ASSERT_TRUE(errorCode == 0 || errorCode == EFFECT_NO_GPU_ENVIRONMENT || errorCode == EFFECT_RESOURCE_RELEASE_ERROR);

    std::shared_ptr<Media::PixelMap> afterRelease;
    {
        EffectImageChain chain;
        ASSERT_EQ(chain.Prepare(srcPixelMap, false), DrawingError::ERR_OK);
        ASSERT_EQ(chain.ApplyBlur(TEST_BLUR_RADIUS, Drawing::TileMode::CLAMP), DrawingError::ERR_OK);
        ASSERT_EQ(chain.Draw(), DrawingError::ERR_OK);
        afterRelease = chain.GetPixelMap();
        ASSERT_NE(afterRelease, nullptr);
    }
    ASSERT_EQ(baseline->GetWidth(), afterRelease->GetWidth());
    ASSERT_EQ(baseline->GetHeight(), afterRelease->GetHeight());
    for (int32_t y = 0; y < baseline->GetHeight(); y++) {
        for (int32_t x = 0; x < baseline->GetWidth(); x++) {
            ASSERT_EQ(*baseline->GetPixel32(x, y), *afterRelease->GetPixel32(x, y))
                << "pixel mismatch at (" << x << "," << y << ")";
        }
    }
#else
    GTEST_SKIP() << "Skip: GPU release path requires RS_ENABLE_GPU && !ROSEN_ARKUI_X";
#endif
}

#if defined(RS_ENABLE_GPU) && !defined(ROSEN_ARKUI_X)
static std::shared_ptr<Media::PixelMap> RenderBlurOnThread(
    const std::shared_ptr<Media::PixelMap>& srcPixelMap)
{
    std::promise<std::shared_ptr<Media::PixelMap>> resultPromise;
    auto resultFuture = resultPromise.get_future();
    std::thread worker([&srcPixelMap, &resultPromise]() {
        EffectImageChain chain;
        if (chain.Prepare(srcPixelMap, false) != DrawingError::ERR_OK ||
            chain.ApplyBlur(TEST_BLUR_RADIUS, Drawing::TileMode::CLAMP) != DrawingError::ERR_OK ||
            chain.Draw() != DrawingError::ERR_OK) {
            resultPromise.set_value(nullptr);
            return;
        }
        resultPromise.set_value(chain.GetPixelMap());
    });
    worker.join();
    return resultFuture.get();
}

static void ExpectSamePixels(const std::shared_ptr<Media::PixelMap>& baseline,
    const std::shared_ptr<Media::PixelMap>& result)
{
    ASSERT_EQ(baseline->GetWidth(), result->GetWidth());
    ASSERT_EQ(baseline->GetHeight(), result->GetHeight());
    for (int32_t y = 0; y < baseline->GetHeight(); y++) {
        for (int32_t x = 0; x < baseline->GetWidth(); x++) {
            ASSERT_EQ(*baseline->GetPixel32(x, y), *result->GetPixel32(x, y))
                << "pixel mismatch at (" << x << "," << y << ")";
        }
    }
}

static bool RenderAndReleaseGpuContextOnNewThread()
{
    Media::InitializationOptions opts;
    opts.size = { 4, 4 };
    std::shared_ptr<Media::PixelMap> srcPixelMap(Media::PixelMap::Create(opts));
    if (srcPixelMap == nullptr) {
        return false;
    }
    {
        EffectImageChain chain;
        if (chain.Prepare(srcPixelMap, false) != DrawingError::ERR_OK ||
            chain.Draw() != DrawingError::ERR_OK) {
            return false;
        }
    }
    int32_t errorCode = -1;
    EffectImageChain::ReleaseGpuContext(&errorCode);
    return errorCode == 0 || errorCode == EFFECT_RESOURCE_RELEASE_ERROR;
}
#endif

/**
 * @tc.name: EffectImageChainUnittest.CrossThreadReleaseThenRerender
 * @tc.desc: test ReleaseCaches destroys the device once the worker thread is already clean (L1);
 */
HWTEST_F(EffectImageChainUnittest, CrossThreadReleaseThenRerender, TestSize.Level1)
{
#if defined(RS_ENABLE_GPU) && !defined(ROSEN_ARKUI_X)
    Media::InitializationOptions opts;
    opts.size = { 4, 4 };
    std::shared_ptr<Media::PixelMap> srcPixelMap(Media::PixelMap::Create(opts));
    ASSERT_NE(srcPixelMap, nullptr);

    auto baseline = RenderBlurOnThread(srcPixelMap);
    ASSERT_NE(baseline, nullptr);

    int32_t errorCode = -1;
    EffectImageChain::ReleaseCaches(&errorCode);
    ASSERT_TRUE(errorCode == 0 || errorCode == EFFECT_RESOURCE_RELEASE_ERROR);
    EXPECT_EQ(EffectImageChain::processVkContext_.load(), nullptr);

    auto rerendered = RenderBlurOnThread(srcPixelMap);
    ASSERT_NE(rerendered, nullptr);

    ExpectSamePixels(baseline, rerendered);
#else
    GTEST_SKIP() << "Skip: GPU release path requires RS_ENABLE_GPU && !ROSEN_ARKUI_X";
#endif
}

/**
 * @tc.name: EffectImageChainUnittest.WorkerReleaseDoesNotAffectMainThread
 * @tc.desc: test worker's ReleaseGpuContext keeps main thread context alive and rendering correctly (AC-2.3)
 */
HWTEST_F(EffectImageChainUnittest, WorkerReleaseDoesNotAffectMainThread, TestSize.Level1)
{
#if defined(RS_ENABLE_GPU) && !defined(ROSEN_ARKUI_X)
    Media::InitializationOptions opts;
    opts.size = { 4, 4 };
    std::shared_ptr<Media::PixelMap> srcPixelMap(Media::PixelMap::Create(opts));
    ASSERT_NE(srcPixelMap, nullptr);

    std::shared_ptr<Media::PixelMap> baseline;
    std::unique_ptr<EffectImageChain> mainChain = std::make_unique<EffectImageChain>();
    ASSERT_EQ(mainChain->Prepare(srcPixelMap, false), DrawingError::ERR_OK);
    ASSERT_EQ(mainChain->Draw(), DrawingError::ERR_OK);
    baseline = mainChain->GetPixelMap();
    ASSERT_NE(baseline, nullptr);
    ASSERT_NE(EffectImageChain::gpuContext_, nullptr);

    std::promise<bool> workerDone;
    auto workerFuture = workerDone.get_future();
    std::thread worker([&workerDone]() {
        workerDone.set_value(RenderAndReleaseGpuContextOnNewThread());
    });
    worker.join();
    ASSERT_TRUE(workerFuture.get());

    EXPECT_EQ(mainChain->gpuContextRef_.get(), EffectImageChain::gpuContext_.get());

    mainChain.reset();

    std::shared_ptr<Media::PixelMap> afterWorkerRelease;
    {
        EffectImageChain chain;
        ASSERT_EQ(chain.Prepare(srcPixelMap, false), DrawingError::ERR_OK);
        ASSERT_EQ(chain.Draw(), DrawingError::ERR_OK);
        afterWorkerRelease = chain.GetPixelMap();
        ASSERT_NE(afterWorkerRelease, nullptr);
    }

    ExpectSamePixels(baseline, afterWorkerRelease);
#else
    GTEST_SKIP() << "Skip: GPU release path requires RS_ENABLE_GPU && !ROSEN_ARKUI_X";
#endif
}

/**
 * @tc.name: EffectImageChainUnittest.LiveChainBlocksRelease
 * @tc.desc: test ReleaseCaches is rejected (EFFECT_RESOURCE_RELEASE_ERROR) while any chain is alive
 */
HWTEST_F(EffectImageChainUnittest, LiveChainBlocksRelease, TestSize.Level1)
{
#if defined(RS_ENABLE_GPU) && !defined(ROSEN_ARKUI_X)
    // isolate from the thread-local context left by previous tests on this thread
    EffectImageChain::gpuContext_.reset();
    Media::InitializationOptions opts;
    opts.size = { 4, 4 };
    std::shared_ptr<Media::PixelMap> srcPixelMap(Media::PixelMap::Create(opts));
    ASSERT_NE(srcPixelMap, nullptr);

    std::promise<void> renderStarted;
    std::promise<void> releaseFinished;
    auto startedFuture = renderStarted.get_future();
    auto releaseFuture = releaseFinished.get_future();
    std::thread worker([&srcPixelMap, &renderStarted, &releaseFuture]() {
        EffectImageChain chain;
        if (chain.Prepare(srcPixelMap, false) == DrawingError::ERR_OK) {
            chain.Draw();
        }
        renderStarted.set_value();
        releaseFuture.wait();
    });
    startedFuture.wait();

    int32_t errorCode = -1;
    EffectImageChain::ReleaseCaches(&errorCode);
    ASSERT_TRUE(errorCode == EFFECT_RESOURCE_RELEASE_ERROR);
    EXPECT_NE(EffectImageChain::processVkContext_.load(), nullptr);
    EXPECT_EQ(EffectImageChain::gpuContext_, nullptr);
    releaseFinished.set_value();
    worker.join();
#else
    GTEST_SKIP() << "Skip: GPU release path requires RS_ENABLE_GPU && !ROSEN_ARKUI_X";
#endif
}

/**
 * @tc.name: EffectImageChainUnittest.RepeatedReleaseIdempotent
 * @tc.desc: test repeated ReleaseCaches/ReleaseGpuContext calls after a real release stay idempotent (EX-3)
 */
HWTEST_F(EffectImageChainUnittest, RepeatedReleaseIdempotent, TestSize.Level1)
{
#if defined(RS_ENABLE_GPU) && !defined(ROSEN_ARKUI_X)
    Media::InitializationOptions opts;
    opts.size = { 4, 4 };
    std::shared_ptr<Media::PixelMap> srcPixelMap(Media::PixelMap::Create(opts));
    ASSERT_NE(srcPixelMap, nullptr);
    {
        EffectImageChain chain;
        ASSERT_EQ(chain.Prepare(srcPixelMap, false), DrawingError::ERR_OK);
        ASSERT_EQ(chain.Draw(), DrawingError::ERR_OK);
        ASSERT_NE(EffectImageChain::processVkContext_.load(), nullptr);
    }
    // chain destruction must not free the cached GPUContext held by the thread-local
    ASSERT_TRUE(EffectImageChain::gpuEnvInitialized_.load());
    EXPECT_NE(EffectImageChain::gpuContext_, nullptr);
    ASSERT_NE(EffectImageChain::processVkContext_.load(), nullptr);

    int32_t firstCode = -1;
    EffectImageChain::ReleaseCaches(&firstCode);
    ASSERT_TRUE(firstCode == 0 || firstCode == EFFECT_RESOURCE_RELEASE_ERROR);

    for (int i = 0; i < 2; i++) {
        int32_t repeatCode = -1;
        EffectImageChain::ReleaseCaches(&repeatCode);
        EXPECT_TRUE(repeatCode == 0 || repeatCode == EFFECT_RESOURCE_RELEASE_ERROR);
        int32_t threadCode = -1;
        EffectImageChain::ReleaseGpuContext(&threadCode);
        EXPECT_TRUE(threadCode == 0 || threadCode == EFFECT_RESOURCE_RELEASE_ERROR);
    }
    EXPECT_EQ(EffectImageChain::processVkContext_.load(), nullptr);
    EXPECT_EQ(EffectImageChain::gpuContext_, nullptr);
#else
    GTEST_SKIP() << "Skip: GPU release path requires RS_ENABLE_GPU && !ROSEN_ARKUI_X";
#endif
}

/**
 * @tc.name: EffectImageChainUnittest.ThreadCacheReuseRendersTwice
 * @tc.desc: test two sequential renders on the same thread reuse the cached GPUContext (thread-cache hit branch)
 */
HWTEST_F(EffectImageChainUnittest, ThreadCacheReuseRendersTwice, TestSize.Level1)
{
#if defined(RS_ENABLE_GPU) && !defined(ROSEN_ARKUI_X)
    EffectImageChain::gpuContext_.reset();
    Media::InitializationOptions opts;
    opts.size = { 4, 4 };
    std::shared_ptr<Media::PixelMap> srcPixelMap(Media::PixelMap::Create(opts));
    ASSERT_NE(srcPixelMap, nullptr);

    std::shared_ptr<Drawing::GPUContext> firstContext;
    {
        EffectImageChain chain;
        ASSERT_EQ(chain.Prepare(srcPixelMap, false), DrawingError::ERR_OK);
        ASSERT_EQ(chain.Draw(), DrawingError::ERR_OK);
        firstContext = EffectImageChain::gpuContext_;
        ASSERT_NE(firstContext, nullptr);
    }
    // second render on the same thread must hit the thread cache without recreating the context
    {
        EffectImageChain chain;
        ASSERT_EQ(chain.Prepare(srcPixelMap, false), DrawingError::ERR_OK);
        ASSERT_EQ(chain.Draw(), DrawingError::ERR_OK);
        EXPECT_EQ(EffectImageChain::gpuContext_, firstContext);
    }
    EXPECT_TRUE(EffectImageChain::gpuEnvInitialized_.load());
    // drop the extra holder before ReleaseCaches: the DDGRGPUContext must die while its
    // underlying VkDevice (owned by processVkContext_) is still alive, otherwise the
    // deferred ~TimelineSemaphore runs against a destroyed device
    firstContext = nullptr;
    // call without errorCode to also cover the null error-out defensive branch
    EffectImageChain::ReleaseCaches();
    EXPECT_EQ(EffectImageChain::gpuContext_, nullptr);
#else
    GTEST_SKIP() << "Skip: GPU release path requires RS_ENABLE_GPU && !ROSEN_ARKUI_X";
#endif
}

/**
 * @tc.name: EffectImageChainUnittest.ReleaseApisTolerateNullArgs
 * @tc.desc: test ReleaseContext/ReleaseCaches/ReleaseGpuContext tolerate null inputs (defensive branches)
 */
HWTEST_F(EffectImageChainUnittest, ReleaseApisTolerateNullArgs, TestSize.Level1)
{
    // null context is a no-op; null vkContext is a no-op
    EffectImageChain::ReleaseContext(nullptr);
    EffectImageChain::ReleaseVkContext(nullptr);
    // default parameter errorCode = nullptr must not crash on either release API
    EffectImageChain::ReleaseCaches();
    EffectImageChain::ReleaseGpuContext();
    EXPECT_EQ(EffectImageChain::gpuContext_, nullptr);
}

/**
 * @tc.name: EffectImageChainUnittest.EnsureVkContextConcurrent
 * @tc.desc: test concurrent EnsureVkContext ends up with exactly one process-wide context (double-check branch);
 * the double-check hit path is racy, assertions only require the final invariant
 */
HWTEST_F(EffectImageChainUnittest, EnsureVkContextConcurrent, TestSize.Level1)
{
#if defined(RS_ENABLE_VK)
    // reset so every thread races through the slow path
    RsVulkanContext* stale = EffectImageChain::processVkContext_.exchange(nullptr);
    EffectImageChain::ReleaseVkContext(stale);

    std::atomic<int32_t> readyCount(0);
    std::atomic<bool> goFlag(false);
    std::vector<std::thread> workers;
    for (int32_t i = 0; i < TEST_CONCURRENT_THREAD_COUNT; i++) {
        workers.emplace_back([&readyCount, &goFlag]() {
            readyCount.fetch_add(1, std::memory_order_relaxed);
            while (!goFlag.load(std::memory_order_acquire)) {
            }
            EffectImageChain::EnsureVkContext();
        });
    }
    while (readyCount.load(std::memory_order_acquire) < TEST_CONCURRENT_THREAD_COUNT) {
    }
    goFlag.store(true, std::memory_order_release);
    for (auto& worker : workers) {
        worker.join();
    }

    EXPECT_NE(EffectImageChain::processVkContext_.load(), nullptr);
    // leave a clean state for subsequent tests
    RsVulkanContext* created = EffectImageChain::processVkContext_.exchange(nullptr);
    EffectImageChain::ReleaseVkContext(created);
    EXPECT_EQ(EffectImageChain::processVkContext_.load(), nullptr);
#else
    GTEST_SKIP() << "Skip: concurrent double-check requires RS_ENABLE_VK";
#endif
}

#if defined(RS_ENABLE_VK)
namespace {
std::shared_ptr<Media::PixelMap> CreateRgbaPixelMap(int32_t width, int32_t height)
{
    OHOS::Media::InitializationOptions opts = {
        .size =
            {
                .width = width,
                .height = height,
            },
        .srcPixelFormat = OHOS::Media::PixelFormat::RGBA_8888,
        .pixelFormat = OHOS::Media::PixelFormat::RGBA_8888,
        .alphaType = OHOS::Media::AlphaType::IMAGE_ALPHA_TYPE_PREMUL,
    };
    return Media::PixelMap::Create(opts);
}

OH_NativeBuffer* AllocRgbaNativeBuffer(int32_t width, int32_t height)
{
    OH_NativeBuffer_Config config {
        .width = width,
        .height = height,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA
    };
    return OH_NativeBuffer_Alloc(&config);
}
} // namespace
#endif

/**
 * @tc.name: EffectImageChainUnittest.PrepareNativeBufferFailsWhenVkContextInvalid
 * @tc.desc: test PrepareNativeBuffer/Prepare failure paths when the EFFECT_KIT vulkan context is invalid
 */
HWTEST_F(EffectImageChainUnittest, PrepareNativeBufferFailsWhenVkContextInvalid, TestSize.Level1)
{
#if defined(RS_ENABLE_VK)
    EffectImageChain::gpuContext_.reset();
    EffectImageChain::EnsureVkContext();
    RsVulkanContext* vkCtx = EffectImageChain::processVkContext_.load();
    ASSERT_NE(vkCtx, nullptr);

    std::shared_ptr<RsVulkanInterface> savedIface = vkCtx->GetRsVulkanInterface();
    vkCtx->GetRsVulkanInterface().reset();
    ASSERT_FALSE(vkCtx->IsValid());

    auto srcPixelMap = CreateRgbaPixelMap(200, 200);
    ASSERT_NE(srcPixelMap, nullptr);
    OH_NativeBuffer* dstBuffer = AllocRgbaNativeBuffer(200, 200);
    ASSERT_NE(dstBuffer, nullptr);
    std::shared_ptr<OH_NativeBuffer> dst(dstBuffer, [](OH_NativeBuffer* buffer) {});

    {
        EffectImageChain chain;
        auto ret = chain.PrepareNativeBuffer(srcPixelMap, dst);
        EXPECT_EQ(ret, DrawingError::ERR_ILLEGAL_INPUT);
        EXPECT_FALSE(chain.prepared_);
        EXPECT_EQ(EffectImageChain::gpuContext_, nullptr);
    }
    {
        EffectImageChain chain;
        auto ret = chain.Prepare(srcPixelMap, false);
        EXPECT_EQ(ret, DrawingError::ERR_SURFACE);
        EXPECT_FALSE(chain.prepared_);
    }
    OH_NativeBuffer_Unreference(dstBuffer);
    vkCtx->GetRsVulkanInterface() = savedIface;
    ASSERT_TRUE(vkCtx->IsValid());
    EffectImageChain::gpuContext_.reset();
#else
    GTEST_SKIP() << "Skip: vk-context-invalid injection requires RS_ENABLE_VK";
#endif
}

/**
 * @tc.name: EffectImageChainUnittest.PrepareNativeBufferFailsWhenVkInterfaceMissing
 * @tc.desc: test PrepareNativeBuffer returns ERR_SURFACE when no dedicated vulkan interface exists
 */
HWTEST_F(EffectImageChainUnittest, PrepareNativeBufferFailsWhenVkInterfaceMissing, TestSize.Level1)
{
#if defined(RS_ENABLE_VK)
    EffectImageChain::gpuContext_.reset();
    auto srcPixelMap = CreateRgbaPixelMap(200, 200);
    ASSERT_NE(srcPixelMap, nullptr);
    OH_NativeBuffer* dstBuffer = AllocRgbaNativeBuffer(200, 200);
    ASSERT_NE(dstBuffer, nullptr);
    std::shared_ptr<OH_NativeBuffer> dst(dstBuffer, [](OH_NativeBuffer* buffer) {});

    {
        EffectImageChain warm;
        auto ret = warm.Prepare(srcPixelMap, false);
        ASSERT_EQ(ret, DrawingError::ERR_OK);
    }
    ASSERT_NE(EffectImageChain::gpuContext_, nullptr);

    RsVulkanContext* stale = EffectImageChain::processVkContext_.exchange(nullptr);
    ASSERT_NE(stale, nullptr);

    {
        EffectImageChain chain;
        auto ret = chain.PrepareNativeBuffer(srcPixelMap, dst);
        EXPECT_EQ(ret, DrawingError::ERR_SURFACE);
        EXPECT_FALSE(chain.prepared_);
    }

    OH_NativeBuffer_Unreference(dstBuffer);
    EffectImageChain::processVkContext_.store(stale);
#else
    GTEST_SKIP() << "Skip: vk-interface-missing injection requires RS_ENABLE_VK";
#endif
}

/**
 * @tc.name: EffectImageChainUnittest.DrawNativeBufferFailsWhenVkInterfaceMissing
 * @tc.desc: test DrawNativeBuffer returns ERR_SURFACE and resets fenceId when no dedicated vulkan interface exists
 */
HWTEST_F(EffectImageChainUnittest, DrawNativeBufferFailsWhenVkInterfaceMissing, TestSize.Level1)
{
#if defined(RS_ENABLE_VK)
    EffectImageChain chain;
    auto srcPixelMap = CreateRgbaPixelMap(200, 200);
    ASSERT_NE(srcPixelMap, nullptr);
    OH_NativeBuffer* dstBuffer = AllocRgbaNativeBuffer(200, 200);
    ASSERT_NE(dstBuffer, nullptr);
    std::shared_ptr<OH_NativeBuffer> dst(dstBuffer, [](OH_NativeBuffer* buffer) {});

    auto ret = chain.PrepareNativeBuffer(srcPixelMap, dst);
    ASSERT_EQ(ret, DrawingError::ERR_OK);
    ret = chain.DrawNativeBuffer();
    ASSERT_EQ(ret, DrawingError::ERR_OK);

    RsVulkanContext* stale = EffectImageChain::processVkContext_.exchange(nullptr);
    ASSERT_NE(stale, nullptr);

    ret = chain.DrawNativeBuffer();
    EXPECT_EQ(ret, DrawingError::ERR_SURFACE);
    EXPECT_EQ(chain.GetfenceId(), -1);

    OH_NativeBuffer_Unreference(dstBuffer);
    EffectImageChain::processVkContext_.store(stale);
#else
    GTEST_SKIP() << "Skip: draw vk-interface-missing injection requires RS_ENABLE_VK";
#endif
}
} // namespace Rosen
} // namespace OHOS