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

#include "feature/hdr/rs_colorspace_util.h"
#include "gtest/gtest.h"
#include "pipeline/rs_paint_filter_canvas.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSColorSpaceUtilTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSColorSpaceUtilTest::SetUpTestCase() {}
void RSColorSpaceUtilTest::TearDownTestCase() {}
void RSColorSpaceUtilTest::SetUp() {}
void RSColorSpaceUtilTest::TearDown() {}

/**
 * @tc.name: ColorSpaceToDrawingColorSpace001
 * @tc.desc: test ColorSpaceName to DrawingColorSpace case
 * @tc.type:FUNC
 * @tc.require: ICUM49
 */
HWTEST_F(RSColorSpaceUtilTest, ColorSpaceToDrawingColorSpace001, TestSize.Level1)
{
    using OHOS::ColorManager::ColorSpaceName;
    auto colorSpace = RSColorSpaceUtil::ColorSpaceToDrawingColorSpace(ColorSpaceName::DCI_P3);
    ASSERT_NE(colorSpace, nullptr);
    EXPECT_FALSE(colorSpace->IsSRGB());

    colorSpace = RSColorSpaceUtil::ColorSpaceToDrawingColorSpace(ColorSpaceName::DISPLAY_P3);
    ASSERT_NE(colorSpace, nullptr);
    EXPECT_FALSE(colorSpace->IsSRGB());

    colorSpace = RSColorSpaceUtil::ColorSpaceToDrawingColorSpace(ColorSpaceName::LINEAR_SRGB);
    ASSERT_NE(colorSpace, nullptr);
    EXPECT_FALSE(colorSpace->IsSRGB());

    colorSpace = RSColorSpaceUtil::ColorSpaceToDrawingColorSpace(ColorSpaceName::DISPLAY_BT2020_SRGB);
    ASSERT_NE(colorSpace, nullptr);
    EXPECT_FALSE(colorSpace->IsSRGB());

    colorSpace = RSColorSpaceUtil::ColorSpaceToDrawingColorSpace(ColorSpaceName::ADOBE_RGB);
    ASSERT_NE(colorSpace, nullptr);
    EXPECT_FALSE(colorSpace->IsSRGB());

    colorSpace = RSColorSpaceUtil::ColorSpaceToDrawingColorSpace(ColorSpaceName::SRGB);
    ASSERT_NE(colorSpace, nullptr);
    EXPECT_TRUE(colorSpace->IsSRGB());

    colorSpace = RSColorSpaceUtil::ColorSpaceToDrawingColorSpace(ColorSpaceName::NONE);
    ASSERT_NE(colorSpace, nullptr);
    EXPECT_TRUE(colorSpace->IsSRGB());
}

/**
 * @tc.name: DrawingColorSpaceToGraphicColorGamut001
 * @tc.desc: test Drawing::ColorSpace to GraphicColorGamut case
 * @tc.type: FUNC
 * @tc.require: ICUM49
 */
HWTEST_F(RSColorSpaceUtilTest, DrawingColorSpaceToGraphicColorGamut001, TestSize.Level1)
{
    GraphicColorGamut gamut = GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
    gamut = RSColorSpaceUtil::DrawingColorSpaceToGraphicColorGamut(nullptr);
    EXPECT_EQ(gamut, GRAPHIC_COLOR_GAMUT_SRGB);

    auto colorspace = Drawing::ColorSpace::CreateSRGB();
    gamut = RSColorSpaceUtil::DrawingColorSpaceToGraphicColorGamut(colorspace);
    EXPECT_EQ(gamut, GRAPHIC_COLOR_GAMUT_SRGB);

    colorspace = Drawing::ColorSpace::CreateRGB(Drawing::CMSTransferFuncType::SRGB, Drawing::CMSMatrixType::DCIP3);
    gamut = RSColorSpaceUtil::DrawingColorSpaceToGraphicColorGamut(colorspace);
    EXPECT_EQ(gamut, GRAPHIC_COLOR_GAMUT_DISPLAY_P3);

    colorspace = Drawing::ColorSpace::CreateRGB(Drawing::CMSTransferFuncType::SRGB, Drawing::CMSMatrixType::REC2020);
    gamut = RSColorSpaceUtil::DrawingColorSpaceToGraphicColorGamut(colorspace);
    EXPECT_EQ(gamut, GRAPHIC_COLOR_GAMUT_BT2020);

    colorspace = Drawing::ColorSpace::CreateRGB(Drawing::CMSTransferFuncType::SRGB, Drawing::CMSMatrixType::ADOBE_RGB);
    gamut = RSColorSpaceUtil::DrawingColorSpaceToGraphicColorGamut(colorspace);
    EXPECT_EQ(gamut, GRAPHIC_COLOR_GAMUT_SRGB);
}

/**
 * @tc.name: GetColorGamutFromCanvas001
 * @tc.desc: test Drawing::ColorSpace to GraphicColorGamut case
 * @tc.type: FUNC
 * @tc.require: ICUM49
 */
HWTEST_F(RSColorSpaceUtilTest, GetColorGamutFromCanvas001, TestSize.Level1)
{
    Drawing::Canvas drawingCanvas;
    auto gamut = RSColorSpaceUtil::GetColorGamutFromCanvas(drawingCanvas);
    EXPECT_EQ(gamut, GRAPHIC_COLOR_GAMUT_INVALID);

    RSPaintFilterCanvas canvas(&drawingCanvas);
    auto surface = std::make_shared<Drawing::Surface>();
    canvas.surface_ = surface.get();
    gamut = RSColorSpaceUtil::GetColorGamutFromCanvas(canvas);
    EXPECT_EQ(gamut, GRAPHIC_COLOR_GAMUT_SRGB);
}

/**
 * @tc.name: ColorSpaceNameToGraphicGamut001
 * @tc.desc: test ColorSpaceName to GraphicColorGamut case
 * @tc.type:FUNC
 * @tc.require: ICGKPE
 */
HWTEST_F(RSColorSpaceUtilTest, ColorSpaceNameToGraphicGamut001, TestSize.Level1)
{
#ifndef ROSEN_CROSS_PLATFORM
    using OHOS::ColorManager::ColorSpaceName;
    GraphicColorGamut gamut = RSColorSpaceUtil::ColorSpaceNameToGraphicGamut(ColorSpaceName::SRGB);
    EXPECT_EQ(gamut, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);

    gamut = RSColorSpaceUtil::ColorSpaceNameToGraphicGamut(ColorSpaceName::ACES);
    EXPECT_EQ(gamut, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_NATIVE); // unsupport ACES gamut.

    HDIV::CM_ColorPrimaries primary = HDIV::COLORPRIMARIES_ADOBERGB;
    gamut = RSColorSpaceUtil::PrimariesToGraphicGamut(primary);
    EXPECT_EQ(gamut, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);

    primary = HDIV::COLORPRIMARIES_P3_DCI;
    gamut = RSColorSpaceUtil::PrimariesToGraphicGamut(primary);
    EXPECT_EQ(gamut, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);

    primary = HDIV::COLORPRIMARIES_P3_D65;
    gamut = RSColorSpaceUtil::PrimariesToGraphicGamut(primary);
    EXPECT_EQ(gamut, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);

    primary = HDIV::COLORPRIMARIES_BT2020;
    gamut = RSColorSpaceUtil::PrimariesToGraphicGamut(primary);
    EXPECT_EQ(gamut, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020);

    primary = HDIV::COLORPRIMARIES_SRGB;
    gamut = RSColorSpaceUtil::PrimariesToGraphicGamut(primary);
    EXPECT_EQ(gamut, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);

    gamut = RSColorSpaceUtil::SelectBigGamut(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020,
        GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    EXPECT_EQ(gamut, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020);
#endif
}

/**
 * @tc.name: ConvertColorGamutToSpaceType001
 * @tc.desc: Test ConvertColorGamutToSpaceType with standard gamuts
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSColorSpaceUtilTest, ConvertColorGamutToSpaceType001, TestSize.Level1)
{
#ifndef ROSEN_CROSS_PLATFORM
    HDIV::CM_ColorSpaceType colorSpaceType;
    bool ret = RSColorSpaceUtil::ConvertColorGamutToSpaceType(
        GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB, colorSpaceType);
    EXPECT_TRUE(ret);
    EXPECT_EQ(colorSpaceType, HDIV::CM_SRGB_FULL);
 
    ret = RSColorSpaceUtil::ConvertColorGamutToSpaceType(
        GraphicColorGamut::GRAPHIC_COLOR_GAMUT_STANDARD_BT601, colorSpaceType);
    EXPECT_TRUE(ret);
    EXPECT_EQ(colorSpaceType, HDIV::CM_BT601_EBU_FULL);
 
    ret = RSColorSpaceUtil::ConvertColorGamutToSpaceType(
        GraphicColorGamut::GRAPHIC_COLOR_GAMUT_STANDARD_BT709, colorSpaceType);
    EXPECT_TRUE(ret);
    EXPECT_EQ(colorSpaceType, HDIV::CM_BT709_FULL);
#endif
}
 
/**
 * @tc.name: ConvertColorGamutToSpaceType002
 * @tc.desc: Test ConvertColorGamutToSpaceType with wide gamut
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSColorSpaceUtilTest, ConvertColorGamutToSpaceType002, TestSize.Level1)
{
#ifndef ROSEN_CROSS_PLATFORM
    HDIV::CM_ColorSpaceType colorSpaceType;
    bool ret = RSColorSpaceUtil::ConvertColorGamutToSpaceType(
        GraphicColorGamut::GRAPHIC_COLOR_GAMUT_ADOBE_RGB, colorSpaceType);
    EXPECT_TRUE(ret);
    EXPECT_EQ(colorSpaceType, HDIV::CM_ADOBERGB_FULL);
 
    ret = RSColorSpaceUtil::ConvertColorGamutToSpaceType(
        GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3, colorSpaceType);
    EXPECT_TRUE(ret);
    EXPECT_EQ(colorSpaceType, HDIV::CM_P3_FULL);
 
    ret = RSColorSpaceUtil::ConvertColorGamutToSpaceType(
        GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DCI_P3, colorSpaceType);
    EXPECT_TRUE(ret);
    EXPECT_EQ(colorSpaceType, HDIV::CM_P3_FULL);
#endif
}
 
/**
 * @tc.name: ConvertColorGamutToSpaceType003
 * @tc.desc: Test ConvertColorGamutToSpaceType with BT2020 and HDR
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSColorSpaceUtilTest, ConvertColorGamutToSpaceType003, TestSize.Level1)
{
#ifndef ROSEN_CROSS_PLATFORM
    HDIV::CM_ColorSpaceType colorSpaceType;
    bool ret = RSColorSpaceUtil::ConvertColorGamutToSpaceType(
        GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020, colorSpaceType);
    EXPECT_TRUE(ret);
    EXPECT_EQ(colorSpaceType, HDIV::CM_DISPLAY_BT2020_SRGB);
 
    ret = RSColorSpaceUtil::ConvertColorGamutToSpaceType(
        GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2100_PQ, colorSpaceType);
    EXPECT_TRUE(ret);
    EXPECT_EQ(colorSpaceType, HDIV::CM_BT2020_PQ_FULL);
 
    ret = RSColorSpaceUtil::ConvertColorGamutToSpaceType(
        GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2100_HLG, colorSpaceType);
    EXPECT_TRUE(ret);
    EXPECT_EQ(colorSpaceType, HDIV::CM_BT2020_HLG_FULL);
 
    ret = RSColorSpaceUtil::ConvertColorGamutToSpaceType(
        GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_BT2020, colorSpaceType);
    EXPECT_TRUE(ret);
    EXPECT_EQ(colorSpaceType, HDIV::CM_DISPLAY_BT2020_SRGB);
#endif
}
 
/**
 * @tc.name: ConvertColorGamutToSpaceType004
 * @tc.desc: Test ConvertColorGamutToSpaceType with invalid gamut
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSColorSpaceUtilTest, ConvertColorGamutToSpaceType004, TestSize.Level1)
{
#ifndef ROSEN_CROSS_PLATFORM
    HDIV::CM_ColorSpaceType colorSpaceType;
    bool ret = RSColorSpaceUtil::ConvertColorGamutToSpaceType(
        GraphicColorGamut::GRAPHIC_COLOR_GAMUT_NATIVE, colorSpaceType);
    EXPECT_FALSE(ret);
#endif
}

/**
 * @tc.name: ApiColorSpaceNameToCMColorSpaceType001
 * @tc.desc: Test ApiColorSpaceNameToCMColorSpaceType with special/unknown entries
 * @tc.type: FUNC
 */
HWTEST_F(RSColorSpaceUtilTest, ApiColorSpaceNameToCMColorSpaceType001, TestSize.Level1)
{
    auto type = RSColorSpaceUtil::ApiColorSpaceNameToCMColorSpaceType(ApiColorSpaceName::UNKNOWN);
    EXPECT_EQ(type, GraphicCM_ColorSpaceType::GRAPHIC_CM_COLORSPACE_NONE);

    type = RSColorSpaceUtil::ApiColorSpaceNameToCMColorSpaceType(ApiColorSpaceName::CUSTOM);
    EXPECT_EQ(type, GraphicCM_ColorSpaceType::GRAPHIC_CM_SRGB_FULL);

    type = RSColorSpaceUtil::ApiColorSpaceNameToCMColorSpaceType(ApiColorSpaceName::H_LOG);
    EXPECT_EQ(type, GraphicCM_ColorSpaceType::GRAPHIC_CM_SRGB_FULL);

    // Alias of SRGB
    type = RSColorSpaceUtil::ApiColorSpaceNameToCMColorSpaceType(ApiColorSpaceName::DISPLAY_SRGB);
    EXPECT_EQ(type, GraphicCM_ColorSpaceType::GRAPHIC_CM_SRGB_FULL);
}

/**
 * @tc.name: ApiColorSpaceNameToCMColorSpaceType002
 * @tc.desc: Test ApiColorSpaceNameToCMColorSpaceType with full-range SRGB/P3/BT entries
 * @tc.type: FUNC
 */
HWTEST_F(RSColorSpaceUtilTest, ApiColorSpaceNameToCMColorSpaceType002, TestSize.Level1)
{
    auto type = RSColorSpaceUtil::ApiColorSpaceNameToCMColorSpaceType(ApiColorSpaceName::ADOBE_RGB_1998);
    EXPECT_EQ(type, GraphicCM_ColorSpaceType::GRAPHIC_CM_ADOBERGB_FULL);

    type = RSColorSpaceUtil::ApiColorSpaceNameToCMColorSpaceType(ApiColorSpaceName::DCI_P3);
    EXPECT_EQ(type, GraphicCM_ColorSpaceType::GRAPHIC_CM_SRGB_FULL);

    type = RSColorSpaceUtil::ApiColorSpaceNameToCMColorSpaceType(ApiColorSpaceName::DISPLAY_P3);
    EXPECT_EQ(type, GraphicCM_ColorSpaceType::GRAPHIC_CM_DISPLAY_P3_SRGB);

    // Alias of DISPLAY_P3
    type = RSColorSpaceUtil::ApiColorSpaceNameToCMColorSpaceType(ApiColorSpaceName::DISPLAY_P3_SRGB);
    EXPECT_EQ(type, GraphicCM_ColorSpaceType::GRAPHIC_CM_DISPLAY_P3_SRGB);

    type = RSColorSpaceUtil::ApiColorSpaceNameToCMColorSpaceType(ApiColorSpaceName::SRGB);
    EXPECT_EQ(type, GraphicCM_ColorSpaceType::GRAPHIC_CM_SRGB_FULL);

    type = RSColorSpaceUtil::ApiColorSpaceNameToCMColorSpaceType(ApiColorSpaceName::BT709);
    EXPECT_EQ(type, GraphicCM_ColorSpaceType::GRAPHIC_CM_BT709_FULL);

    type = RSColorSpaceUtil::ApiColorSpaceNameToCMColorSpaceType(ApiColorSpaceName::BT601_EBU);
    EXPECT_EQ(type, GraphicCM_ColorSpaceType::GRAPHIC_CM_BT601_EBU_FULL);

    type = RSColorSpaceUtil::ApiColorSpaceNameToCMColorSpaceType(ApiColorSpaceName::BT601_SMPTE_C);
    EXPECT_EQ(type, GraphicCM_ColorSpaceType::GRAPHIC_CM_BT601_SMPTE_C_FULL);
}

/**
 * @tc.name: ApiColorSpaceNameToCMColorSpaceType003
 * @tc.desc: Test ApiColorSpaceNameToCMColorSpaceType with HDR full entries
 * @tc.type: FUNC
 */
HWTEST_F(RSColorSpaceUtilTest, ApiColorSpaceNameToCMColorSpaceType003, TestSize.Level1)
{
    auto type = RSColorSpaceUtil::ApiColorSpaceNameToCMColorSpaceType(ApiColorSpaceName::BT2020_HLG);
    EXPECT_EQ(type, GraphicCM_ColorSpaceType::GRAPHIC_CM_BT2020_HLG_FULL);

    type = RSColorSpaceUtil::ApiColorSpaceNameToCMColorSpaceType(ApiColorSpaceName::BT2020_PQ);
    EXPECT_EQ(type, GraphicCM_ColorSpaceType::GRAPHIC_CM_BT2020_PQ_FULL);

    type = RSColorSpaceUtil::ApiColorSpaceNameToCMColorSpaceType(ApiColorSpaceName::P3_HLG);
    EXPECT_EQ(type, GraphicCM_ColorSpaceType::GRAPHIC_CM_P3_HLG_FULL);

    // Alias of P3_HLG
    type = RSColorSpaceUtil::ApiColorSpaceNameToCMColorSpaceType(ApiColorSpaceName::DISPLAY_P3_HLG);
    EXPECT_EQ(type, GraphicCM_ColorSpaceType::GRAPHIC_CM_P3_HLG_FULL);

    type = RSColorSpaceUtil::ApiColorSpaceNameToCMColorSpaceType(ApiColorSpaceName::P3_PQ);
    EXPECT_EQ(type, GraphicCM_ColorSpaceType::GRAPHIC_CM_P3_PQ_FULL);

    // Alias of P3_PQ
    type = RSColorSpaceUtil::ApiColorSpaceNameToCMColorSpaceType(ApiColorSpaceName::DISPLAY_P3_PQ);
    EXPECT_EQ(type, GraphicCM_ColorSpaceType::GRAPHIC_CM_P3_PQ_FULL);
}

/**
 * @tc.name: ApiColorSpaceNameToCMColorSpaceType004
 * @tc.desc: Test ApiColorSpaceNameToCMColorSpaceType with limit-range SRGB/P3/BT entries
 * @tc.type: FUNC
 */
HWTEST_F(RSColorSpaceUtilTest, ApiColorSpaceNameToCMColorSpaceType004, TestSize.Level1)
{
    auto type = RSColorSpaceUtil::ApiColorSpaceNameToCMColorSpaceType(ApiColorSpaceName::ADOBE_RGB_1998_LIMIT);
    EXPECT_EQ(type, GraphicCM_ColorSpaceType::GRAPHIC_CM_ADOBERGB_LIMIT);

    type = RSColorSpaceUtil::ApiColorSpaceNameToCMColorSpaceType(ApiColorSpaceName::DISPLAY_P3_LIMIT);
    EXPECT_EQ(type, GraphicCM_ColorSpaceType::GRAPHIC_CM_P3_LIMIT);

    type = RSColorSpaceUtil::ApiColorSpaceNameToCMColorSpaceType(ApiColorSpaceName::SRGB_LIMIT);
    EXPECT_EQ(type, GraphicCM_ColorSpaceType::GRAPHIC_CM_SRGB_LIMIT);

    type = RSColorSpaceUtil::ApiColorSpaceNameToCMColorSpaceType(ApiColorSpaceName::BT709_LIMIT);
    EXPECT_EQ(type, GraphicCM_ColorSpaceType::GRAPHIC_CM_BT709_LIMIT);

    type = RSColorSpaceUtil::ApiColorSpaceNameToCMColorSpaceType(ApiColorSpaceName::BT601_EBU_LIMIT);
    EXPECT_EQ(type, GraphicCM_ColorSpaceType::GRAPHIC_CM_BT601_EBU_LIMIT);

    type = RSColorSpaceUtil::ApiColorSpaceNameToCMColorSpaceType(ApiColorSpaceName::BT601_SMPTE_C_LIMIT);
    EXPECT_EQ(type, GraphicCM_ColorSpaceType::GRAPHIC_CM_BT601_SMPTE_C_LIMIT);
}

/**
 * @tc.name: ApiColorSpaceNameToCMColorSpaceType005
 * @tc.desc: Test ApiColorSpaceNameToCMColorSpaceType with limit-range HDR entries
 * @tc.type: FUNC
 */
HWTEST_F(RSColorSpaceUtilTest, ApiColorSpaceNameToCMColorSpaceType005, TestSize.Level1)
{
    auto type = RSColorSpaceUtil::ApiColorSpaceNameToCMColorSpaceType(ApiColorSpaceName::BT2020_HLG_LIMIT);
    EXPECT_EQ(type, GraphicCM_ColorSpaceType::GRAPHIC_CM_BT2020_HLG_LIMIT);

    type = RSColorSpaceUtil::ApiColorSpaceNameToCMColorSpaceType(ApiColorSpaceName::BT2020_PQ_LIMIT);
    EXPECT_EQ(type, GraphicCM_ColorSpaceType::GRAPHIC_CM_BT2020_PQ_LIMIT);

    type = RSColorSpaceUtil::ApiColorSpaceNameToCMColorSpaceType(ApiColorSpaceName::P3_HLG_LIMIT);
    EXPECT_EQ(type, GraphicCM_ColorSpaceType::GRAPHIC_CM_P3_HLG_LIMIT);

    type = RSColorSpaceUtil::ApiColorSpaceNameToCMColorSpaceType(ApiColorSpaceName::P3_PQ_LIMIT);
    EXPECT_EQ(type, GraphicCM_ColorSpaceType::GRAPHIC_CM_P3_PQ_LIMIT);
}

/**
 * @tc.name: ApiColorSpaceNameToCMColorSpaceType006
 * @tc.desc: Test ApiColorSpaceNameToCMColorSpaceType with linear and display BT2020 entries
 * @tc.type: FUNC
 */
HWTEST_F(RSColorSpaceUtilTest, ApiColorSpaceNameToCMColorSpaceType006, TestSize.Level1)
{
    auto type = RSColorSpaceUtil::ApiColorSpaceNameToCMColorSpaceType(ApiColorSpaceName::LINEAR_P3);
    EXPECT_EQ(type, GraphicCM_ColorSpaceType::GRAPHIC_CM_LINEAR_P3);

    type = RSColorSpaceUtil::ApiColorSpaceNameToCMColorSpaceType(ApiColorSpaceName::LINEAR_SRGB);
    EXPECT_EQ(type, GraphicCM_ColorSpaceType::GRAPHIC_CM_LINEAR_SRGB);

    // Alias of LINEAR_SRGB
    type = RSColorSpaceUtil::ApiColorSpaceNameToCMColorSpaceType(ApiColorSpaceName::LINEAR_BT709);
    EXPECT_EQ(type, GraphicCM_ColorSpaceType::GRAPHIC_CM_LINEAR_SRGB);

    type = RSColorSpaceUtil::ApiColorSpaceNameToCMColorSpaceType(ApiColorSpaceName::LINEAR_BT2020);
    EXPECT_EQ(type, GraphicCM_ColorSpaceType::GRAPHIC_CM_LINEAR_BT2020);

    type = RSColorSpaceUtil::ApiColorSpaceNameToCMColorSpaceType(ApiColorSpaceName::DISPLAY_BT2020_SRGB);
    EXPECT_EQ(type, GraphicCM_ColorSpaceType::GRAPHIC_CM_DISPLAY_BT2020_SRGB);
}

/**
 * @tc.name: ApiColorSpaceNameToCMColorSpaceType007
 * @tc.desc: Test ApiColorSpaceNameToCMColorSpaceType fallback branch for unmapped values
 * @tc.type: FUNC
 */
HWTEST_F(RSColorSpaceUtilTest, ApiColorSpaceNameToCMColorSpaceType007, TestSize.Level1)
{
    // TYPE_END is a sentinel value that is not present in the mapping table.
    auto type = RSColorSpaceUtil::ApiColorSpaceNameToCMColorSpaceType(ApiColorSpaceName::TYPE_END);
    EXPECT_EQ(type, GraphicCM_ColorSpaceType::GRAPHIC_CM_SRGB_FULL);

    // Out-of-range raw value cast into the enum (defensive case).
    type = RSColorSpaceUtil::ApiColorSpaceNameToCMColorSpaceType(
        static_cast<ApiColorSpaceName>(static_cast<uint32_t>(ApiColorSpaceName::TYPE_END) + 1u));
    EXPECT_EQ(type, GraphicCM_ColorSpaceType::GRAPHIC_CM_SRGB_FULL);

    // A clearly invalid large value.
    type = RSColorSpaceUtil::ApiColorSpaceNameToCMColorSpaceType(static_cast<ApiColorSpaceName>(0xFFFFFFFFu));
    EXPECT_EQ(type, GraphicCM_ColorSpaceType::GRAPHIC_CM_SRGB_FULL);
}
} // namespace OHOS::Rosen