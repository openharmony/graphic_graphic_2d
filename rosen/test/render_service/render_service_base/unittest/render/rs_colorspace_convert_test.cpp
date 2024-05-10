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

#include "gtest/gtest.h"
#include "effect/image_filter.h"
#include "luminance/rs_luminance_control.h"
#include "metadata_helper.h"
#include "platform/common/rs_log.h"

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

    bool ret = RSColorSpaceConvert::Instance().ColorSpaceConvertor(nullptr, nullptr, paint, targetColorSpace,
        screenId, dynamicRangeMode);
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

    Drawing::SamplingOptions sampling;
    Drawing::Matrix matrix;  //Identity Matrix
    std::shared_ptr<Drawing::Image> img = std::make_shared<Drawing::Image>();
    ASSERT_TRUE(img != nullptr);

    auto imageShader = Drawing::ShaderEffect::CreateImageShader(
        *img, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, sampling, matrix);
    ASSERT_TRUE(imageShader != nullptr);

    bool ret = RSColorSpaceConvert::Instance().ColorSpaceConvertor(imageShader, nullptr, paint, targetColorSpace,
        screenId, dynamicRangeMode);
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
        targetColorSpace, screenId, dynamicRangeMode);
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

    sptr<SurfaceBuffer> surfaceBuffer = SurfaceBuffer::Create().GetRefPtr();
    ASSERT_TRUE(surfaceBuffer != nullptr);
    VPEParameter parameter;

    bool ret = RSColorSpaceConvert::Instance().SetColorSpaceConverterDisplayParameter(surfaceBuffer, parameter,
        targetColorSpace, screenId, dynamicRangeMode);
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

    sptr<SurfaceBuffer> surfaceBuffer;
    VPEParameter parameter;

    bool ret = RSColorSpaceConvert::Instance().SetColorSpaceConverterDisplayParameter(nullptr, parameter,
        targetColorSpace, screenId, dynamicRangeMode);
    ASSERT_TRUE(ret == false);
}

} // namespace OHOS::Rosen
