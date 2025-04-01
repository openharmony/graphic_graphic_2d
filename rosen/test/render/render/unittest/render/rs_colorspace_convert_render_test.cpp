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

#include "render/rs_colorspace_convert.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "display_engine/rs_luminance_control.h"
#include "effect/image_filter.h"
#include "metadata_helper.h"
#include "platform/common/rs_log.h"
#include "surface_buffer_impl.h"

using namespace testing;
using namespace testing::text;

namespace OHOS::Rosen {

class RSColorspaceConvertRenderTest : public testing::Test {
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

void RSColorspaceConvertRenderTest::SetUpTestCase() {}
void RSColorspaceConvertRenderTest::TearDownTestCase() {}
void RSColorspaceConvertRenderTest::SetUp() {}
void RSColorspaceConvertRenderTest::TearDown() {}

/**
 * @tc.name: TestColorSpaceConvertor01
 * @tc.desc: Verify function inputShader == nullptr && surfaceBuffer == nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSColorspaceConvertRenderTest, TestColorSpaceConvertor01, TestSize.Level1)
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
 * @tc.name: TestColorSpaceConvertor02
 * @tc.desc: Verify function inputShader != nullptr && surfaceBuffer == nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSColorspaceConvertRenderTest, TestColorSpaceConvertor02, TestSize.Level1)
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
 * @tc.name: TestColorSpaceConvertor01
 * @tc.desc: tilex TileMode is REPEAT
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSColorspaceConvertRenderTest, TestColorSpaceConvertor01, TestSize.Level1)
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
 * @tc.name: TestColorSpaceConvertor02
 * @tc.desc: TileMode is MIRROR
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSColorspaceConvertRenderTest, TestColorSpaceConvertor02, TestSize.Level1)
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
 * @tc.name: TestColorSpaceConvertor03
 * @tc.desc: tilex TileMode is DECAL
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSColorspaceConvertRenderTest, TestColorSpaceConvertor03, TestSize.Level1)
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
 * @tc.name: TestColorSpaceConvertor04
 * @tc.desc: tileY TileMode is REPEAT
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSColorspaceConvertRenderTest, TestColorSpaceConvertor04, TestSize.Level1)
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
 * @tc.name: TestColorSpaceConvertor05
 * @tc.desc: tileY TileMode is MIRROR
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSColorspaceConvertRenderTest, TestColorSpaceConvertor05, TestSize.Level1)
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
 * @tc.name: TestColorSpaceConvertor06
 * @tc.desc: tileY TileMode is MIRROR
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSColorspaceConvertRenderTest, TestColorSpaceConvertor06, TestSize.Level1)
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
 * @tc.name: TestColorSpaceConvertor03
 * @tc.desc: Verify function inputShader != nullptr && surfaceBuffer != nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSColorspaceConvertRenderTest, TestColorSpaceConvertor03, TestSize.Level1)
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
 * @tc.name: TestColorSpaceConvertor04
 * @tc.desc: Verify function inputShader != nullptr && surfaceBuffer != nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSColorspaceConvertRenderTest, TestColorSpaceConvertor04, TestSize.Level1)
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
 * @tc.name: TestSetColorSpaceConverterDisplayParameter01
 * @tc.desc: Verify function surfaceBuffer != nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSColorspaceConvertRenderTest, TestSetColorSpaceConverterDisplayParameter01, TestSize.Level1)
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
 * @tc.name: TestSetColorSpaceConverterDisplayParameter02
 * @tc.desc: Verify function surfaceBuffer == nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSColorspaceConvertRenderTest, TestSetColorSpaceConverterDisplayParameter02, TestSize.Level1)
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
 * @tc.name: TestConvertColorGamutToSpaceInfo01
 * @tc.desc: Verify function targetColorSpace == GRAPHIC_COLOR_GAMUT_DISPLAY_P3
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSColorspaceConvertRenderTest, TestConvertColorGamutToSpaceInfo01, TestSize.Level1)
{
    GraphicColorGamut targetColorSpace = GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
    HDIV::CM_ColorSpaceInfo colorSpaceInfo;

    bool ret = RSColorSpaceConvert::Instance().ConvertColorGamutToSpaceInfo(targetColorSpace, colorSpaceInfo);
    ASSERT_TRUE(ret == true);
}

/**
 * @tc.name: TestConvertColorGamutToSpaceInfo02
 * @tc.desc: Verify function targetColorSpace == GRAPHIC_COLOR_GAMUT_INVALID
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSColorspaceConvertRenderTest, TestConvertColorGamutToSpaceInfo02, TestSize.Level1)
{
    GraphicColorGamut targetColorSpace = GRAPHIC_COLOR_GAMUT_INVALID;
    HDIV::CM_ColorSpaceInfo colorSpaceInfo;

    bool ret = RSColorSpaceConvert::Instance().ConvertColorGamutToSpaceInfo(targetColorSpace, colorSpaceInfo);
    ASSERT_TRUE(ret == true);
}

/**
 * @tc.name: TestConvertColorGamutToSpaceInfo03
 * @tc.desc: Verify function targetColorSpace == GRAPHIC_COLOR_GAMUT_NATIVE
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSColorspaceConvertRenderTest, TestConvertColorGamutToSpaceInfo03, TestSize.Level1)
{
    GraphicColorGamut targetColorSpace = GRAPHIC_COLOR_GAMUT_NATIVE;
    HDIV::CM_ColorSpaceInfo colorSpaceInfo;

    bool ret = RSColorSpaceConvert::Instance().ConvertColorGamutToSpaceInfo(targetColorSpace, colorSpaceInfo);
    ASSERT_TRUE(ret == true);
}

/**
 * @tc.name: TestConvertColorGamutToSpaceInfo04
 * @tc.desc: Verify function targetColorSpace == GRAPHIC_COLOR_GAMUT_STANDARD_BT601
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSColorspaceConvertRenderTest, TestConvertColorGamutToSpaceInfo04, TestSize.Level1)
{
    GraphicColorGamut targetColorSpace = GRAPHIC_COLOR_GAMUT_STANDARD_BT601;
    HDIV::CM_ColorSpaceInfo colorSpaceInfo;

    bool ret = RSColorSpaceConvert::Instance().ConvertColorGamutToSpaceInfo(targetColorSpace, colorSpaceInfo);
    ASSERT_TRUE(ret == true);
}


/**
 * @tc.name: TestConvertColorGamutToSpaceInfo05
 * @tc.desc: Verify function targetColorSpace == GRAPHIC_COLOR_GAMUT_STANDARD_BT709
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSColorspaceConvertRenderTest, TestConvertColorGamutToSpaceInfo05, TestSize.Level1)
{
    GraphicColorGamut targetColorSpace = GRAPHIC_COLOR_GAMUT_STANDARD_BT709;
    HDIV::CM_ColorSpaceInfo colorSpaceInfo;

    bool ret = RSColorSpaceConvert::Instance().ConvertColorGamutToSpaceInfo(targetColorSpace, colorSpaceInfo);
    ASSERT_TRUE(ret == true);
}


/**
 * @tc.name: TestConvertColorGamutToSpaceInfo06
 * @tc.desc: Verify function targetColorSpace == GRAPHIC_COLOR_GAMUT_SRGB
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSColorspaceConvertRenderTest, TestConvertColorGamutToSpaceInfo06, TestSize.Level1)
{
    GraphicColorGamut targetColorSpace = GRAPHIC_COLOR_GAMUT_SRGB;
    HDIV::CM_ColorSpaceInfo colorSpaceInfo;

    bool ret = RSColorSpaceConvert::Instance().ConvertColorGamutToSpaceInfo(targetColorSpace, colorSpaceInfo);
    ASSERT_TRUE(ret == true);
}

/**
 * @tc.name: TestConvertColorGamutToSpaceInfo07
 * @tc.desc: Verify function targetColorSpace == GRAPHIC_COLOR_GAMUT_ADOBE_RGB
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSColorspaceConvertRenderTest, TestConvertColorGamutToSpaceInfo07, TestSize.Level1)
{
    GraphicColorGamut targetColorSpace = GRAPHIC_COLOR_GAMUT_ADOBE_RGB;
    HDIV::CM_ColorSpaceInfo colorSpaceInfo;

    bool ret = RSColorSpaceConvert::Instance().ConvertColorGamutToSpaceInfo(targetColorSpace, colorSpaceInfo);
    ASSERT_TRUE(ret == true);
}

/**
 * @tc.name: TestGetFovMetadata01
 * @tc.desc: Verify function get fov metadata with nullptr buffer
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSColorspaceConvertRenderTest, TestGetFovMetadata01, TestSize.Level1)
{
    VPEParameter parameter;
    sptr<SurfaceBuffer> surfaceBuffer = nullptr;
    GSError ret = GSERROR_OK;
    RSColorSpaceConvert::Instance().GetFOVMetadata(surfaceBuffer, parameter.adaptiveFOVMetadata, ret);
    ASSERT_TRUE(ret != GSERROR_OK); // buffer is nullptr
}

/**
 * @tc.name: TestGetFovMetadata02
 * @tc.desc: Verify function get fov metadata normal case
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSColorspaceConvertRenderTest, TestGetFovMetadata02, TestSize.Level1)
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
    ASSERT_TRUE(ret, GSERROR_OK);
    ASSERT_TRUE(surfaceBuffer != nullptr);
    std::vector<uint8_t> metadataSet{1, 18, 119, 33, 196, 253, 112, 171, 74, 230, 99, 23, 0, 244, 82,
        138, 13, 158, 100, 41, 50, 189, 111, 144, 3, 153, 75, 210, 243, 237, 19, 12, 128};
    ret = MetadataHelper::SetAdaptiveFOVMetadata(surfaceBuffer, metadataSet);
    ASSERT_TRUE(ret == GSERROR_OK || ret == GSERROR_HDI_ERROR);
    VPEParameter parameter;
    RSColorSpaceConvert::Instance().GetFOVMetadata(surfaceBuffer, parameter.adaptiveFOVMetadata, ret);
    ASSERT_TRUE(ret == GSERROR_OK || ret == GSERROR_HDI_ERROR);
}
} // namespace OHOS::Rosen
