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
#include "effect_image_chain.h"
#include "ge_linear_gradient_shader_mask.h"
#include "platform/common/rs_system_properties.h"
#include "surface_buffer.h"
#include "render/rs_pixel_map_util.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
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

} // namespace Rosen
} // namespace OHOS