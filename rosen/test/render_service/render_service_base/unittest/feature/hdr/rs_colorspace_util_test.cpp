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
} // namespace OHOS::Rosen