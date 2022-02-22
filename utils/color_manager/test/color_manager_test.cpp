/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <array>
#include <gtest/gtest.h>
#include <hilog/log.h>

#include "color.h"
#include "color_space.h"
#include "color_space_convertor.h"

using namespace testing::ext;

namespace OHOS {
namespace ColorManager {
class ColorManagerTest : public testing::Test {
public:
    static constexpr HiviewDFX::HiLogLabel LOG_LABEL = {LOG_CORE, 0, "ColorManagerTest"};

    static void SetUpTestCase()
    {}

    static void TearDownTestCase()
    {}
};

/*
* Function: ColorManagerTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: sRGB convert to sRGB
*/
HWTEST_F(ColorManagerTest, sRGBTosRGB, Function | SmallTest | Level2)
{
    auto color = Color(0.1, 0.2, 0.3, 0.4);
    Color result = color.Convert(ColorSpace(SRGB));
    ASSERT_EQ(result.r, 0.1);
    ASSERT_EQ(result.g, 0.2);
    ASSERT_EQ(result.b, 0.3);
    ASSERT_EQ(result.a, 0.4);
}

/*
* Function: ColorManagerTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: sRGB convert to Display_P3
*/
HWTEST_F(ColorManagerTest, sRGBToDisplay_P3, Function | SmallTest | Level2)
{
    auto color = Color(0.1, 0.2, 0.3, 0.4);
    Color result = color.Convert(ColorSpace(DISPLAY_P3));
    ASSERT_EQ(result.r, 0.1237);
    ASSERT_EQ(result.g, 0.1975);
    ASSERT_EQ(result.b, 0.2918);
    ASSERT_EQ(result.a, 0.4);
}

/*
* Function: ColorManagerTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: sRGB convert to Adobe
*/
HWTEST_F(ColorManagerTest, sRGBToAdobe, Function | SmallTest | Level2)
{
    auto color = Color(0.1, 0.2, 0.3, 0.4);
    Color result = color.Convert(ColorSpace(ADOBE_RGB));
    ASSERT_EQ(result.r, 0.1234);
    ASSERT_EQ(result.g, 0.2124);
    ASSERT_EQ(result.b, 0.3047);
    ASSERT_EQ(result.a, 0.4);
}

/*
* Function: ColorManagerTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Display_P3 convert to sRGB
*/
HWTEST_F(ColorManagerTest, Display_P3TosRGB, Function | SmallTest | Level2)
{
    auto convertor = ColorSpaceConvertor(ColorSpace(DISPLAY_P3), ColorSpace(SRGB), GAMUT_MAP_CONSTANT);
    std::array<float, 3> rgb = {0.1, 0.2, 0.3};
    auto result = convertor.Convert(rgb);
    ASSERT_EQ(result[0], 0.0594);
    ASSERT_EQ(result[1], 0.2031);
    ASSERT_EQ(result[2], 0.3087);
}

/*
* Function: ColorManagerTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: sRGB convert to sRGB
*/
HWTEST_F(ColorManagerTest, sRGBTosRGBConvertor, Function | SmallTest | Level2)
{
    auto convertor = ColorSpaceConvertor(ColorSpace(SRGB), ColorSpace(SRGB), GAMUT_MAP_CONSTANT);
    std::array<float, 3> rgb = {0.1, 0.2, 0.3};
    auto result = convertor.Convert(rgb);
    ASSERT_EQ(result[0], 0.1);
    ASSERT_EQ(result[1], 0.2);
    ASSERT_EQ(result[2], 0.3);
}

/*
* Function: ColorManagerTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Adobe convert to Display_P3
*/
HWTEST_F(ColorManagerTest, AdobeToDisplay_P3, Function | SmallTest | Level2)
{
    auto convertor = ColorSpaceConvertor(ColorSpace(ADOBE_RGB), ColorSpace(DISPLAY_P3), GAMUT_MAP_CONSTANT);
    std::array<float, 3> rgb = {0.1, 0.2, 0.3};
    auto result = convertor.Convert(rgb);
    ASSERT_EQ(result[0], 0.1020);
    ASSERT_EQ(result[1], 0.1837);
    ASSERT_EQ(result[2], 0.2863);
}

/*
* Function: ColorManagerTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Display_P3 convert to Adobe
*/
HWTEST_F(ColorManagerTest, Display_P3ToAdobe, Function | SmallTest | Level2)
{
    auto convertor = ColorSpaceConvertor(ColorSpace(DISPLAY_P3), ColorSpace(ADOBE_RGB), GAMUT_MAP_CONSTANT);
    std::array<float, 3> rgb = {0.1, 0.2, 0.3};
    auto result = convertor.Convert(rgb);
    ASSERT_EQ(result[0], 0.0886);
    ASSERT_EQ(result[1], 0.2152);
    ASSERT_EQ(result[2], 0.3130);
}

/*
* Function: ColorManagerTest
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Adobe convert to sRGB
*/
HWTEST_F(ColorManagerTest, AdobeToSRGB, Function | SmallTest | Level2)
{
    auto convertor = ColorSpaceConvertor(ColorSpace(ADOBE_RGB), ColorSpace(SRGB), GAMUT_MAP_CONSTANT);
    std::array<float, 3> rgb = {0.1, 0.2, 0.3};
    auto result = convertor.Convert(rgb);
    ASSERT_EQ(result[0], 0.0728);
    ASSERT_EQ(result[1], 0.1862);
    ASSERT_EQ(result[2], 0.2949);
}
}
}