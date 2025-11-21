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
#include "platform/common/rs_system_properties.h"

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
 * @tc.name: Apply_Draw_Test
 * @tc.desc: test Apply and Draw
 */
HWTEST_F(EffectImageChainUnittest, Apply_Draw_Test, TestSize.Level1)
{
    auto image = std::make_shared<EffectImageChain>();
    EXPECT_NE(image, nullptr);

    auto ret = image->ApplyDrawingFilter(nullptr);
    EXPECT_NE(ret, DrawingError::ERR_OK);

    ret = image->ApplyBlur(-1, Drawing::TileMode::CLAMP);
    EXPECT_NE(ret, DrawingError::ERR_OK); // invalid radius

    ret = image->ApplyBlur(0.5, Drawing::TileMode::CLAMP);
    EXPECT_NE(ret, DrawingError::ERR_OK); // need prepered first

    // test not prepared
    std::vector<float> positions = {0.0f, 1.0f};
    std::vector<float> degrees = {0.0f, 1.0f};
    ret = image->ApplyEllipticalGradientBlur(1.0f, 0.0f, 0.0f, 1.0f, 1.0f, positions, degrees);
    EXPECT_EQ(ret, DrawingError::ERR_NOT_PREPARED);
    EXPECT_NE(ret, DrawingError::ERR_OK); // need prepared first

    Media::InitializationOptions opts;
    opts.size = { 1, 1 };
    std::shared_ptr<Media::PixelMap> srcPixelMap(Media::PixelMap::Create(opts));
    EXPECT_NE(srcPixelMap, nullptr);
    ret = image->Prepare(srcPixelMap, false);
    EXPECT_EQ(ret, DrawingError::ERR_OK);

    ret = image->Draw(); // no filter
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
    EXPECT_EQ(filterBlur, nullptr);
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

    // EllipticalGradientBlur
    // test normal condition
    ret = image->ApplyEllipticalGradientBlur(1.0f, 0.0f, 0.0f, 1.0f, 1.0f, positions, degrees);
    EXPECT_EQ(ret, DrawingError::ERR_OK);
    // test illegal blur radius
    ret = image->ApplyEllipticalGradientBlur(-1.0f, 0.0f, 0.0f, 1.0f, 1.0f, positions, degrees);
    EXPECT_EQ(ret, DrawingError::ERR_ILLEGAL_INPUT);

    // test illegal center coordinates
    ret = image->ApplyEllipticalGradientBlur(1.0f, -1.0f, 0.0f, 1.0f, 1.0f, positions, degrees);
    EXPECT_EQ(ret, DrawingError::ERR_ILLEGAL_INPUT);
    ret = image->ApplyEllipticalGradientBlur(1.0f, 0.0f, -1.0f, 1.0f, 1.0f, positions, degrees);
    EXPECT_EQ(ret, DrawingError::ERR_ILLEGAL_INPUT);
    ret = image->ApplyEllipticalGradientBlur(1.0f, -1.0f, -1.0f, 1.0f, 1.0f, positions, degrees);
    EXPECT_EQ(ret, DrawingError::ERR_ILLEGAL_INPUT);

    // test illegal ellipse radius
    ret = image->ApplyEllipticalGradientBlur(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, positions, degrees);
    EXPECT_EQ(ret, DrawingError::ERR_ILLEGAL_INPUT);
    ret = image->ApplyEllipticalGradientBlur(1.0f, 0.0f, 0.0f, 1.0f, 0.0f, positions, degrees);
    EXPECT_EQ(ret, DrawingError::ERR_ILLEGAL_INPUT);
    ret = image->ApplyEllipticalGradientBlur(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, positions, degrees);
    EXPECT_EQ(ret, DrawingError::ERR_ILLEGAL_INPUT);

    // test empty positions and degrees
    ret = image->ApplyEllipticalGradientBlur(1.0f, 0.0f, 0.0f, 1.0f, 1.0f, {}, degrees);
    EXPECT_EQ(ret, DrawingError::ERR_ILLEGAL_INPUT);
    ret = image->ApplyEllipticalGradientBlur(1.0f, 0.0f, 0.0f, 1.0f, 1.0f, positions, {});
    EXPECT_EQ(ret, DrawingError::ERR_ILLEGAL_INPUT);
    ret = image->ApplyEllipticalGradientBlur(1.0f, 0.0f, 0.0f, 1.0f, 1.0f, {}, {});
    EXPECT_EQ(ret, DrawingError::ERR_ILLEGAL_INPUT);

    // test if forceCPU_ is true
    ret = image->Prepare(srcPixelMap, true);
    ret = image->ApplyEllipticalGradientBlur(1.0f, 0.0f, 0.0f, 1.0f, 1.0f, positions, degrees);
    EXPECT_EQ(ret, DrawingError::ERR_ILLEGAL_INPUT);
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
 * @tc.name: EffectCanvasTest
 * @tc.desc: test EffectCanvas
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
} // namespace Rosen
} // namespace OHOS