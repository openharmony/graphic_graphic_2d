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

#include "render/rs_colorspace_convert.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "display_engine/rs_luminance_control.h"
#include "effect/image_filter.h"
#include "metadata_helper.h"
#include "platform/common/rs_log.h"
#include "surface_buffer_impl.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class RSColorspaceConvertTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

class MockRSColorspaceConvert : public RSColorSpaceConvert {
public:
    MOCK_METHOD6(SetColorSpaceConverterDisplayParameter, bool(const sptr<SurfaceBuffer>& surfaceBuffer,
        VPEParameter& parameter, GraphicColorGamut targetColorSpace, ScreenId screenId, uint32_t dynamicRangeMode,
        float hdrBrightness));
};

void RSColorspaceConvertTest::SetUpTestCase() {}
void RSColorspaceConvertTest::TearDownTestCase() {}
void RSColorspaceConvertTest::SetUp() {}
void RSColorspaceConvertTest::TearDown() {}

/**
 * @tc.name: ColorSpaceConvertor001
 * @tc.desc: test inputShader == nullptr && surfaceBuffer == nullptr
 * @tc.type:FUNC
 * @tc.require: issueI9NLRF
 */
HWTEST_F(RSColorspaceConvertTest, ColorSpaceConvertor001, TestSize.Level1)
{
    Drawing::Paint paint;
    GraphicColorGamut targetColorSpace = GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
    ScreenId screenId = 0;
    uint32_t dynamicRangeMode = 1;
    float hdrBrightness = 1.0f;

    bool ret = RSColorSpaceConvert::Instance().ColorSpaceConvertor(nullptr, nullptr, paint, targetColorSpace,
        screenId, dynamicRangeMode, hdrBrightness);
    ASSERT_TRUE(ret == false);
}

/**
 * @tc.name: ColorSpaceConvertor002
 * @tc.desc: test inputShader != nullptr && surfaceBuffer == nullptr
 * @tc.type:FUNC
 * @tc.require: issueI9NLRF
 */
HWTEST_F(RSColorspaceConvertTest, ColorSpaceConvertor002, TestSize.Level1)
{
    Drawing::Paint paint;
    GraphicColorGamut targetColorSpace = GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
    ScreenId screenId = 0;
    uint32_t dynamicRangeMode = 1;
    float hdrBrightness = 1.0f;

    Drawing::SamplingOptions sampling;
    Drawing::Matrix matrix;  //Identity Matrix
    std::shared_ptr<Drawing::Image> img = std::make_shared<Drawing::Image>();
    ASSERT_TRUE(img != nullptr);

    auto imageShader = Drawing::ShaderEffect::CreateImageShader(
        *img, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, sampling, matrix);
    ASSERT_TRUE(imageShader != nullptr);

    bool ret = RSColorSpaceConvert::Instance().ColorSpaceConvertor(imageShader, nullptr, paint, targetColorSpace,
        screenId, dynamicRangeMode, hdrBrightness);
    ASSERT_TRUE(ret == false);
}

/**
 * @tc.name: ColorSpaceConvertor201
 * @tc.desc: tilex TileMode is REPEAT
 * @tc.type:FUNC
 * @tc.require: issueI9NLRF
 */
HWTEST_F(RSColorspaceConvertTest, ColorSpaceConvertor201, TestSize.Level1)
{
    Drawing::Paint paint;
    GraphicColorGamut targetColorSpace = GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
    ScreenId screenId = 0;
    uint32_t dynamicRangeMode = 1;
    float hdrBrightness = 1.0f;

    Drawing::SamplingOptions sampling;
    Drawing::Matrix matrix;  //Identity Matrix
    std::shared_ptr<Drawing::Image> img = std::make_shared<Drawing::Image>();
    ASSERT_TRUE(img != nullptr);

    auto imageShader = Drawing::ShaderEffect::CreateImageShader(
        *img, Drawing::TileMode::REPEAT, Drawing::TileMode::CLAMP, sampling, matrix);
    ASSERT_TRUE(imageShader != nullptr);

    bool ret = RSColorSpaceConvert::Instance().ColorSpaceConvertor(imageShader, nullptr, paint, targetColorSpace,
        screenId, dynamicRangeMode, hdrBrightness);
    ASSERT_TRUE(ret == false);
}

/**
 * @tc.name: ColorSpaceConvertor202
 * @tc.desc: TileMode is MIRROR
 * @tc.type:FUNC
 * @tc.require: issueI9NLRF
 */
HWTEST_F(RSColorspaceConvertTest, ColorSpaceConvertor202, TestSize.Level1)
{
    Drawing::Paint paint;
    GraphicColorGamut targetColorSpace = GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
    ScreenId screenId = 0;
    uint32_t dynamicRangeMode = 1;
    float hdrBrightness = 1.0f;

    Drawing::SamplingOptions sampling;
    Drawing::Matrix matrix;  //Identity Matrix
    std::shared_ptr<Drawing::Image> img = std::make_shared<Drawing::Image>();
    ASSERT_TRUE(img != nullptr);

    auto imageShader = Drawing::ShaderEffect::CreateImageShader(
        *img, Drawing::TileMode::MIRROR, Drawing::TileMode::CLAMP, sampling, matrix);
    ASSERT_TRUE(imageShader != nullptr);

    bool ret = RSColorSpaceConvert::Instance().ColorSpaceConvertor(imageShader, nullptr, paint, targetColorSpace,
        screenId, dynamicRangeMode, hdrBrightness);
    ASSERT_TRUE(ret == false);
}

/**
 * @tc.name: ColorSpaceConvertor203
 * @tc.desc: tilex TileMode is DECAL
 * @tc.type:FUNC
 * @tc.require: issueI9NLRF
 */
HWTEST_F(RSColorspaceConvertTest, ColorSpaceConvertor203, TestSize.Level1)
{
    Drawing::Paint paint;
    GraphicColorGamut targetColorSpace = GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
    ScreenId screenId = 0;
    uint32_t dynamicRangeMode = 1;
    float hdrBrightness = 1.0f;

    Drawing::SamplingOptions sampling;
    Drawing::Matrix matrix;  //Identity Matrix
    std::shared_ptr<Drawing::Image> img = std::make_shared<Drawing::Image>();
    ASSERT_TRUE(img != nullptr);

    auto imageShader = Drawing::ShaderEffect::CreateImageShader(
        *img, Drawing::TileMode::DECAL, Drawing::TileMode::CLAMP, sampling, matrix);
    ASSERT_TRUE(imageShader != nullptr);

    bool ret = RSColorSpaceConvert::Instance().ColorSpaceConvertor(imageShader, nullptr, paint, targetColorSpace,
        screenId, dynamicRangeMode, hdrBrightness);
    ASSERT_TRUE(ret == false);
}

/**
 * @tc.name: ColorSpaceConvertor204
 * @tc.desc: tileY TileMode is REPEAT
 * @tc.type:FUNC
 * @tc.require: issueI9NLRF
 */
HWTEST_F(RSColorspaceConvertTest, ColorSpaceConvertor204, TestSize.Level1)
{
    Drawing::Paint paint;
    GraphicColorGamut targetColorSpace = GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
    ScreenId screenId = 0;
    uint32_t dynamicRangeMode = 1;
    float hdrBrightness = 1.0f;

    Drawing::SamplingOptions sampling;
    Drawing::Matrix matrix;  //Identity Matrix
    std::shared_ptr<Drawing::Image> img = std::make_shared<Drawing::Image>();
    ASSERT_TRUE(img != nullptr);

    auto imageShader = Drawing::ShaderEffect::CreateImageShader(
        *img, Drawing::TileMode::REPEAT, Drawing::TileMode::REPEAT, sampling, matrix);
    ASSERT_TRUE(imageShader != nullptr);

    bool ret = RSColorSpaceConvert::Instance().ColorSpaceConvertor(imageShader, nullptr, paint, targetColorSpace,
        screenId, dynamicRangeMode, hdrBrightness);
    ASSERT_TRUE(ret == false);
}

/**
 * @tc.name: ColorSpaceConvertor205
 * @tc.desc: tileY TileMode is MIRROR
 * @tc.type:FUNC
 * @tc.require: issueI9NLRF
 */
HWTEST_F(RSColorspaceConvertTest, ColorSpaceConvertor205, TestSize.Level1)
{
    Drawing::Paint paint;
    GraphicColorGamut targetColorSpace = GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
    ScreenId screenId = 0;
    uint32_t dynamicRangeMode = 1;
    float hdrBrightness = 1.0f;

    Drawing::SamplingOptions sampling;
    Drawing::Matrix matrix;  //Identity Matrix
    std::shared_ptr<Drawing::Image> img = std::make_shared<Drawing::Image>();
    ASSERT_TRUE(img != nullptr);

    auto imageShader = Drawing::ShaderEffect::CreateImageShader(
        *img, Drawing::TileMode::REPEAT, Drawing::TileMode::MIRROR, sampling, matrix);
    ASSERT_TRUE(imageShader != nullptr);

    bool ret = RSColorSpaceConvert::Instance().ColorSpaceConvertor(imageShader, nullptr, paint, targetColorSpace,
        screenId, dynamicRangeMode, hdrBrightness);
    ASSERT_TRUE(ret == false);
}

/**
 * @tc.name: ColorSpaceConvertor206
 * @tc.desc: tileY TileMode is MIRROR
 * @tc.type:FUNC
 * @tc.require: issueI9NLRF
 */
HWTEST_F(RSColorspaceConvertTest, ColorSpaceConvertor206, TestSize.Level1)
{
    Drawing::Paint paint;
    GraphicColorGamut targetColorSpace = GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
    ScreenId screenId = 0;
    uint32_t dynamicRangeMode = 1;
    float hdrBrightness = 1.0f;

    Drawing::SamplingOptions sampling;
    Drawing::Matrix matrix;  //Identity Matrix
    std::shared_ptr<Drawing::Image> img = std::make_shared<Drawing::Image>();
    ASSERT_TRUE(img != nullptr);

    auto imageShader = Drawing::ShaderEffect::CreateImageShader(
        *img, Drawing::TileMode::REPEAT, Drawing::TileMode::DECAL, sampling, matrix);
    ASSERT_TRUE(imageShader != nullptr);

    bool ret = RSColorSpaceConvert::Instance().ColorSpaceConvertor(imageShader, nullptr, paint, targetColorSpace,
        screenId, dynamicRangeMode, hdrBrightness);
    ASSERT_TRUE(ret == false);
}

/**
 * @tc.name: ColorSpaceConvertor003
 * @tc.desc: test inputShader != nullptr && surfaceBuffer != nullptr
 * @tc.type:FUNC
 * @tc.require: issueI9NLRF
 */
HWTEST_F(RSColorspaceConvertTest, ColorSpaceConvertor003, TestSize.Level1)
{
    Drawing::Paint paint;
    GraphicColorGamut targetColorSpace = GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
    ScreenId screenId = 0;
    uint32_t dynamicRangeMode = 1;
    float hdrBrightness = 1.0f;

    Drawing::SamplingOptions sampling;
    Drawing::Matrix matrix;  //Identity Matrix
    std::shared_ptr<Drawing::Image> img = std::make_shared<Drawing::Image>();
    ASSERT_TRUE(img != nullptr);

    sptr<SurfaceBuffer> surfaceBuffer = SurfaceBuffer::Create().GetRefPtr();
    ASSERT_TRUE(surfaceBuffer != nullptr);
    auto imageShader = Drawing::ShaderEffect::CreateImageShader(
        *img, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, sampling, matrix);
    ASSERT_TRUE(imageShader != nullptr);

    bool ret = RSColorSpaceConvert::Instance().ColorSpaceConvertor(imageShader, surfaceBuffer, paint,
        targetColorSpace, screenId, dynamicRangeMode, hdrBrightness);
    ASSERT_TRUE(ret == false);
}

/**
 * @tc.name: ColorSpaceConvertor004
 * @tc.desc: test inputShader != nullptr && surfaceBuffer != nullptr
 * @tc.type:FUNC
 * @tc.require: IAJ26A
 */
HWTEST_F(RSColorspaceConvertTest, ColorSpaceConvertor004, TestSize.Level1)
{
    std::shared_ptr<MockRSColorspaceConvert> mockRSColorspaceConvert = nullptr;

    mockRSColorspaceConvert = std::make_shared<MockRSColorspaceConvert>();

    sptr<SurfaceBuffer> surfaceBuffer = SurfaceBuffer::Create().GetRefPtr();
    VPEParameter parameter;
    GraphicColorGamut targetColorSpace = GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
    ScreenId screenId = 0;
    uint32_t dynamicRangeMode = 1;
    float hdrBrightness = 1.0f;
    Drawing::Paint paint;

    EXPECT_CALL(*mockRSColorspaceConvert, SetColorSpaceConverterDisplayParameter(_, _, _, _, _, _))
        .WillRepeatedly(testing::Return(true));

    Drawing::SamplingOptions sampling;
    Drawing::Matrix matrix;  //Identity Matrix
    std::shared_ptr<Drawing::Image> img = std::make_shared<Drawing::Image>();
    ASSERT_TRUE(img != nullptr);
    auto imageShader = Drawing::ShaderEffect::CreateImageShader(
        *img, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, sampling, matrix);
    ASSERT_TRUE(imageShader != nullptr);

    bool ret = mockRSColorspaceConvert->ColorSpaceConvertor(imageShader, surfaceBuffer, paint,
        targetColorSpace, screenId, dynamicRangeMode, hdrBrightness);
    ASSERT_TRUE(ret == false);
}

/**
 * @tc.name: SetColorSpaceConverterDisplayParameter001
 * @tc.desc: test surfaceBuffer != nullptr
 * @tc.type:FUNC
 * @tc.require: issueI9NLRF
 */
HWTEST_F(RSColorspaceConvertTest, SetColorSpaceConverterDisplayParameter001, TestSize.Level1)
{
    GraphicColorGamut targetColorSpace = GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
    ScreenId screenId = 0;
    uint32_t dynamicRangeMode = 1;
    float hdrBrightness = 1.0f;

    sptr<SurfaceBuffer> surfaceBuffer = SurfaceBuffer::Create().GetRefPtr();
    ASSERT_TRUE(surfaceBuffer != nullptr);
    VPEParameter parameter;

    bool ret = RSColorSpaceConvert::Instance().SetColorSpaceConverterDisplayParameter(surfaceBuffer, parameter,
        targetColorSpace, screenId, dynamicRangeMode, hdrBrightness);
    ASSERT_TRUE(ret == false);
}

/**
 * @tc.name: SetColorSpaceConverterDisplayParameter002
 * @tc.desc: test surfaceBuffer == nullptr
 * @tc.type:FUNC
 * @tc.require: issueI9NLRF
 */
HWTEST_F(RSColorspaceConvertTest, SetColorSpaceConverterDisplayParameter002, TestSize.Level1)
{
    GraphicColorGamut targetColorSpace = GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
    ScreenId screenId = 0;
    uint32_t dynamicRangeMode = 1;
    float hdrBrightness = 1.0f;

    sptr<SurfaceBuffer> surfaceBuffer;
    VPEParameter parameter;

    bool ret = RSColorSpaceConvert::Instance().SetColorSpaceConverterDisplayParameter(nullptr, parameter,
        targetColorSpace, screenId, dynamicRangeMode, hdrBrightness);
    ASSERT_TRUE(ret == false);
}

/**
 * @tc.name: ConvertColorGamutToSpaceInfo001
 * @tc.desc: test targetColorSpace == GRAPHIC_COLOR_GAMUT_DISPLAY_P3
 * @tc.type:FUNC
 * @tc.require: IAJ26A
 */
HWTEST_F(RSColorspaceConvertTest, ConvertColorGamutToSpaceInfo001, TestSize.Level1)
{
    GraphicColorGamut targetColorSpace = GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
    HDIV::CM_ColorSpaceInfo colorSpaceInfo;

    bool ret = RSColorSpaceConvert::Instance().ConvertColorGamutToSpaceInfo(targetColorSpace, colorSpaceInfo);
    ASSERT_TRUE(ret == true);
}

/**
 * @tc.name: ConvertColorGamutToSpaceInfo002
 * @tc.desc: test targetColorSpace == GRAPHIC_COLOR_GAMUT_INVALID
 * @tc.type:FUNC
 * @tc.require: IAJ26A
 */
HWTEST_F(RSColorspaceConvertTest, ConvertColorGamutToSpaceInfo002, TestSize.Level1)
{
    GraphicColorGamut targetColorSpace = GRAPHIC_COLOR_GAMUT_INVALID;
    HDIV::CM_ColorSpaceInfo colorSpaceInfo;

    bool ret = RSColorSpaceConvert::Instance().ConvertColorGamutToSpaceInfo(targetColorSpace, colorSpaceInfo);
    ASSERT_TRUE(ret == true);
}

/**
 * @tc.name: ConvertColorGamutToSpaceInfo003
 * @tc.desc: test targetColorSpace == GRAPHIC_COLOR_GAMUT_NATIVE
 * @tc.type:FUNC
 * @tc.require: IAJ26A
 */
HWTEST_F(RSColorspaceConvertTest, ConvertColorGamutToSpaceInfo003, TestSize.Level1)
{
    GraphicColorGamut targetColorSpace = GRAPHIC_COLOR_GAMUT_NATIVE;
    HDIV::CM_ColorSpaceInfo colorSpaceInfo;

    bool ret = RSColorSpaceConvert::Instance().ConvertColorGamutToSpaceInfo(targetColorSpace, colorSpaceInfo);
    ASSERT_TRUE(ret == true);
}

/**
 * @tc.name: ConvertColorGamutToSpaceInfo004
 * @tc.desc: test targetColorSpace == GRAPHIC_COLOR_GAMUT_STANDARD_BT601
 * @tc.type:FUNC
 * @tc.require: IAJ26A
 */
HWTEST_F(RSColorspaceConvertTest, ConvertColorGamutToSpaceInfo004, TestSize.Level1)
{
    GraphicColorGamut targetColorSpace = GRAPHIC_COLOR_GAMUT_STANDARD_BT601;
    HDIV::CM_ColorSpaceInfo colorSpaceInfo;

    bool ret = RSColorSpaceConvert::Instance().ConvertColorGamutToSpaceInfo(targetColorSpace, colorSpaceInfo);
    ASSERT_TRUE(ret == true);
}


/**
 * @tc.name: ConvertColorGamutToSpaceInfo005
 * @tc.desc: test targetColorSpace == GRAPHIC_COLOR_GAMUT_STANDARD_BT709
 * @tc.type:FUNC
 * @tc.require: IAJ26A
 */
HWTEST_F(RSColorspaceConvertTest, ConvertColorGamutToSpaceInfo005, TestSize.Level1)
{
    GraphicColorGamut targetColorSpace = GRAPHIC_COLOR_GAMUT_STANDARD_BT709;
    HDIV::CM_ColorSpaceInfo colorSpaceInfo;

    bool ret = RSColorSpaceConvert::Instance().ConvertColorGamutToSpaceInfo(targetColorSpace, colorSpaceInfo);
    ASSERT_TRUE(ret == true);
}


/**
 * @tc.name: ConvertColorGamutToSpaceInfo006
 * @tc.desc: test targetColorSpace == GRAPHIC_COLOR_GAMUT_SRGB
 * @tc.type:FUNC
 * @tc.require: IAJ26A
 */
HWTEST_F(RSColorspaceConvertTest, ConvertColorGamutToSpaceInfo006, TestSize.Level1)
{
    GraphicColorGamut targetColorSpace = GRAPHIC_COLOR_GAMUT_SRGB;
    HDIV::CM_ColorSpaceInfo colorSpaceInfo;

    bool ret = RSColorSpaceConvert::Instance().ConvertColorGamutToSpaceInfo(targetColorSpace, colorSpaceInfo);
    ASSERT_TRUE(ret == true);
}

/**
 * @tc.name: ConvertColorGamutToSpaceInfo007
 * @tc.desc: test targetColorSpace == GRAPHIC_COLOR_GAMUT_ADOBE_RGB
 * @tc.type:FUNC
 * @tc.require: IAJ26A
 */
HWTEST_F(RSColorspaceConvertTest, ConvertColorGamutToSpaceInfo007, TestSize.Level1)
{
    GraphicColorGamut targetColorSpace = GRAPHIC_COLOR_GAMUT_ADOBE_RGB;
    HDIV::CM_ColorSpaceInfo colorSpaceInfo;

    bool ret = RSColorSpaceConvert::Instance().ConvertColorGamutToSpaceInfo(targetColorSpace, colorSpaceInfo);
    ASSERT_TRUE(ret == true);
}

/**
 * @tc.name: GetFovMetadata001
 * @tc.desc: test get fov metadata normal case
 * @tc.type:FUNC
 * @tc.require: IAJ26A
 */
HWTEST_F(RSColorspaceConvertTest, GetFovMetadata002, TestSize.Level1)
{
    BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
        .colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB,
    };
    sptr<SurfaceBuffer> surfaceBuffer = new SurfaceBufferImpl(0);
    GSError ret = surfaceBuffer->Alloc(requestConfig);
    ASSERT_TRUE(ret == GSERROR_OK);
    ASSERT_TRUE(surfaceBuffer != nullptr);
    std::vector<uint8_t> metadataSet{1, 18, 119, 33, 196, 253, 112, 171, 74, 230, 99, 23, 0, 244, 82,
        138, 13, 158, 100, 41, 50, 189, 111, 144, 3, 153, 75, 210, 243, 237, 19, 12, 128};
    ret = MetadataHelper::SetAdaptiveFOVMetadata(surfaceBuffer, metadataSet);
    EXPECT_TRUE(ret != GSERROR_OK);
}

/**
 * @tc.name: GetVideoDynamicMetadata001
 * @tc.desc: test get video dynamic metadata normal case
 * @tc.type:FUNC
 * @tc.require: IBYX6N
 */
HWTEST_F(RSColorspaceConvertTest, GetVideoDynamicMetadata001, TestSize.Level1)
{
    BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
        .colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB,
    };
    sptr<SurfaceBuffer> surfaceBuffer = new SurfaceBufferImpl(0);
    GSError ret = surfaceBuffer->Alloc(requestConfig);
    ASSERT_TRUE(ret == GSERROR_OK);
    ASSERT_TRUE(surfaceBuffer != nullptr);
    std::vector<uint8_t> metadataSet{};
    ret = MetadataHelper::GetVideoDynamicMetadata(surfaceBuffer, metadataSet);
    ASSERT_TRUE(ret == GSERROR_HDI_ERROR);
}
} // namespace OHOS::Rosen
