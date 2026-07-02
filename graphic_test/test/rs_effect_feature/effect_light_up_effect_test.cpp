/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "rs_graphic_test.h"
#include "rs_graphic_test_img.h"

#include "render/rs_light_up_effect_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

namespace {
constexpr int SCREEN_WIDTH = 1200;
constexpr int SCREEN_HEIGHT = 2000;
constexpr int SCREEN_MARGIN = 10;
constexpr int BORDER_WIDTH = 5;
constexpr float LIGHT_UP_DEGREE_NEGATIVE = -1.0f;
constexpr float LIGHT_UP_DEGREE_ZERO = 0.0f;
constexpr float LIGHT_UP_DEGREE_QUARTER = 0.25f;
constexpr float LIGHT_UP_DEGREE_HALF = 0.5f;
constexpr float LIGHT_UP_DEGREE_DEFAULT = 1.0f;
constexpr float LIGHT_UP_DEGREE_ONE_AND_HALF = 1.5f;
constexpr float LIGHT_UP_DEGREE_DOUBLE = 2.0f;
constexpr float LIGHT_UP_DEGREE_LARGE = 5.0f;
constexpr float LIGHT_UP_DEGREE_EXTREME = 200.0f;
constexpr float SATURATION_MIN = 0.2f;
constexpr float SATURATION_LOW = 0.4f;
constexpr float SATURATION_MID = 0.6f;
constexpr float SATURATION_HIGH = 0.8f;
constexpr float SATURATION_MAX = 1.0f;
constexpr float BRIGHTNESS_MIN = 0.2f;
constexpr float BRIGHTNESS_LOW = 0.4f;
constexpr float BRIGHTNESS_MID = 0.6f;
constexpr float BRIGHTNESS_HIGH = 0.8f;
constexpr float BRIGHTNESS_MAX = 1.0f;
constexpr int BLUR_RADIUS_SMALL = 10;
constexpr int BLUR_RADIUS_MEDIUM = 30;
constexpr int BLUR_RADIUS_LARGE = 60;
constexpr int COLOR_MODE_INVALID = -1;
constexpr int COLOR_MODE_DEFAULT = 0;
constexpr int COLOR_MODE_ALT = 1;
constexpr int COLOR_MODE_EXTENDED = 100;
constexpr float ALPHA_MIN = 0.1f;
constexpr float ALPHA_LOW = 0.3f;
constexpr float ALPHA_MID = 0.5f;
constexpr float ALPHA_HIGH = 0.7f;
constexpr float ALPHA_MAX = 1.0f;
constexpr float ALPHA_OVER = 1.1f;
constexpr uint32_t COLOR_MASK_RED = 0x2dff0000;
constexpr uint32_t COLOR_MASK_GREEN = 0x4d00ff00;
constexpr uint32_t COLOR_MASK_BLUE = 0x6d0000ff;
constexpr uint32_t COLOR_MASK_BLACK = 0xff000000;
constexpr uint32_t COLOR_MASK_WHITE = 0xffffffff;
constexpr uint32_t COLOR_MASK_TRANSPARENT = 0x00000000;
constexpr int COLUMN_COUNT_2 = 2;
constexpr int COLUMN_COUNT_3 = 3;

const std::string BG_IMAGE_PATH = "/data/local/tmp/Images/backGroundImage.jpg";
}

class LightUpEffectTest : public RSGraphicTest {
private:
    const int screenWidth = SCREEN_WIDTH;
    const int screenHeight = SCREEN_HEIGHT;

public:
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }

    auto SetUpDegreeNode(float lightUpDegree)
    {
        auto testNode =
            SetUpNodeBgImage(BG_IMAGE_PATH, { 0, 0, screenWidth - SCREEN_MARGIN, screenHeight - SCREEN_MARGIN });
        testNode->SetLightUpEffectDegree(lightUpDegree);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
        return testNode;
    }

    auto SetUpBlurDegreeNode(float lightUpDegree, uint32_t maskColor, int radius,
        float saturation, float brightness, int colorMode)
    {
        auto testNode =
            SetUpNodeBgImage(BG_IMAGE_PATH, { 0, 0, screenWidth - SCREEN_MARGIN, screenHeight - SCREEN_MARGIN });
        testNode->SetBorderStyle(0, 0, 0, 0);
        testNode->SetBorderWidth(BORDER_WIDTH, BORDER_WIDTH, BORDER_WIDTH, BORDER_WIDTH);
        testNode->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        testNode->SetBackgroundBlurRadius(radius);
        testNode->SetBackgroundBlurSaturation(saturation);
        testNode->SetBackgroundBlurBrightness(brightness);
        testNode->SetBackgroundBlurColorMode(colorMode);
        testNode->SetBackgroundBlurMaskColor(Color(maskColor));
        testNode->SetLightUpEffectDegree(lightUpDegree);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
        return testNode;
    }

    auto SetUpBlurMaskDegreeNode(float lightUpDegree, uint32_t maskColor, int radius,
        float saturation, float brightness)
    {
        auto testNode =
            SetUpNodeBgImage(BG_IMAGE_PATH, { 0, 0, screenWidth - SCREEN_MARGIN, screenHeight - SCREEN_MARGIN });
        testNode->SetBorderStyle(0, 0, 0, 0);
        testNode->SetBorderWidth(BORDER_WIDTH, BORDER_WIDTH, BORDER_WIDTH, BORDER_WIDTH);
        testNode->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        testNode->SetBackgroundBlurRadius(radius);
        testNode->SetBackgroundBlurSaturation(saturation);
        testNode->SetBackgroundBlurBrightness(brightness);
        testNode->SetBackgroundBlurMaskColor(Color(maskColor));
        testNode->SetLightUpEffectDegree(lightUpDegree);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
        return testNode;
    }

    auto SetUpBlurAlphaDegreeNode(float lightUpDegree, uint32_t maskColor, int radius,
        float saturation, float brightness, float alpha)
    {
        auto testNode =
            SetUpNodeBgImage(BG_IMAGE_PATH, { 0, 0, screenWidth - SCREEN_MARGIN, screenHeight - SCREEN_MARGIN });
        testNode->SetBorderStyle(0, 0, 0, 0);
        testNode->SetBorderWidth(BORDER_WIDTH, BORDER_WIDTH, BORDER_WIDTH, BORDER_WIDTH);
        testNode->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        testNode->SetBackgroundBlurRadius(radius);
        testNode->SetBackgroundBlurSaturation(saturation);
        testNode->SetBackgroundBlurBrightness(brightness);
        testNode->SetBackgroundBlurMaskColor(Color(maskColor));
        testNode->SetAlpha(alpha);
        testNode->SetLightUpEffectDegree(lightUpDegree);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
        return testNode;
    }
};

/*
 * Degree sweep: each value rendered at full-screen for isolated visual comparison
 */
GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Degree_Negative_Test)
{
    SetUpDegreeNode(LIGHT_UP_DEGREE_NEGATIVE);
}

GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Degree_Zero_Test)
{
    SetUpDegreeNode(LIGHT_UP_DEGREE_ZERO);
}

GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Degree_Quarter_Test)
{
    SetUpDegreeNode(LIGHT_UP_DEGREE_QUARTER);
}

GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Degree_Half_Test)
{
    SetUpDegreeNode(LIGHT_UP_DEGREE_HALF);
}

GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Degree_Default_Test)
{
    SetUpDegreeNode(LIGHT_UP_DEGREE_DEFAULT);
}

GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Degree_OneAndHalf_Test)
{
    SetUpDegreeNode(LIGHT_UP_DEGREE_ONE_AND_HALF);
}

GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Degree_Double_Test)
{
    SetUpDegreeNode(LIGHT_UP_DEGREE_DOUBLE);
}

GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Degree_Large_Test)
{
    SetUpDegreeNode(LIGHT_UP_DEGREE_LARGE);
}

GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Degree_Extreme_Test)
{
    SetUpDegreeNode(LIGHT_UP_DEGREE_EXTREME);
}

/*
 * Degree + blur + red mask with varying degree and colorMode
 */
GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Blur_Red_ColorMode_Test1)
{
    SetUpBlurDegreeNode(LIGHT_UP_DEGREE_HALF, COLOR_MASK_RED, BLUR_RADIUS_MEDIUM,
        SATURATION_MID, BRIGHTNESS_MID, COLOR_MODE_INVALID);
}

GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Blur_Red_ColorMode_Test2)
{
    SetUpBlurDegreeNode(LIGHT_UP_DEGREE_DEFAULT, COLOR_MASK_RED, BLUR_RADIUS_MEDIUM,
        SATURATION_MID, BRIGHTNESS_MID, COLOR_MODE_DEFAULT);
}

GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Blur_Red_ColorMode_Test3)
{
    SetUpBlurDegreeNode(LIGHT_UP_DEGREE_ONE_AND_HALF, COLOR_MASK_RED, BLUR_RADIUS_MEDIUM,
        SATURATION_MID, BRIGHTNESS_MID, COLOR_MODE_ALT);
}

GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Blur_Red_ColorMode_Test4)
{
    SetUpBlurDegreeNode(LIGHT_UP_DEGREE_DOUBLE, COLOR_MASK_RED, BLUR_RADIUS_MEDIUM,
        SATURATION_MID, BRIGHTNESS_MID, COLOR_MODE_EXTENDED);
}

/*
 * Degree + blur + green mask with varying degree and colorMode
 */
GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Blur_Green_ColorMode_Test1)
{
    SetUpBlurDegreeNode(LIGHT_UP_DEGREE_HALF, COLOR_MASK_GREEN, BLUR_RADIUS_SMALL,
        SATURATION_MIN, BRIGHTNESS_MIN, COLOR_MODE_DEFAULT);
}

GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Blur_Green_ColorMode_Test2)
{
    SetUpBlurDegreeNode(LIGHT_UP_DEGREE_DEFAULT, COLOR_MASK_GREEN, BLUR_RADIUS_MEDIUM,
        SATURATION_MID, BRIGHTNESS_MID, COLOR_MODE_ALT);
}

GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Blur_Green_ColorMode_Test3)
{
    SetUpBlurDegreeNode(LIGHT_UP_DEGREE_DOUBLE, COLOR_MASK_GREEN, BLUR_RADIUS_LARGE,
        SATURATION_MAX, BRIGHTNESS_MAX, COLOR_MODE_DEFAULT);
}

/*
 * Degree + blur + blue mask with varying degree and colorMode
 */
GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Blur_Blue_ColorMode_Test1)
{
    SetUpBlurDegreeNode(LIGHT_UP_DEGREE_HALF, COLOR_MASK_BLUE, BLUR_RADIUS_MEDIUM,
        SATURATION_MID, BRIGHTNESS_MID, COLOR_MODE_INVALID);
}

GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Blur_Blue_ColorMode_Test2)
{
    SetUpBlurDegreeNode(LIGHT_UP_DEGREE_DEFAULT, COLOR_MASK_BLUE, BLUR_RADIUS_MEDIUM,
        SATURATION_MID, BRIGHTNESS_MID, COLOR_MODE_DEFAULT);
}

GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Blur_Blue_ColorMode_Test3)
{
    SetUpBlurDegreeNode(LIGHT_UP_DEGREE_ONE_AND_HALF, COLOR_MASK_BLUE, BLUR_RADIUS_MEDIUM,
        SATURATION_MID, BRIGHTNESS_MID, COLOR_MODE_ALT);
}

/*
 * Degree + blur + mask color sweep (black to transparent)
 */
GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Blur_MaskColor_Black_Test)
{
    SetUpBlurMaskDegreeNode(LIGHT_UP_DEGREE_HALF, COLOR_MASK_BLACK,
        BLUR_RADIUS_SMALL, SATURATION_MIN, BRIGHTNESS_MIN);
}

GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Blur_MaskColor_White_Test)
{
    SetUpBlurMaskDegreeNode(LIGHT_UP_DEGREE_DEFAULT, COLOR_MASK_WHITE,
        BLUR_RADIUS_MEDIUM, SATURATION_LOW, BRIGHTNESS_LOW);
}

GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Blur_MaskColor_Red_Test)
{
    SetUpBlurMaskDegreeNode(LIGHT_UP_DEGREE_ONE_AND_HALF, COLOR_MASK_RED,
        BLUR_RADIUS_MEDIUM, SATURATION_MID, BRIGHTNESS_MID);
}

GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Blur_MaskColor_Transparent_Test)
{
    SetUpBlurMaskDegreeNode(LIGHT_UP_DEGREE_DOUBLE, COLOR_MASK_TRANSPARENT,
        BLUR_RADIUS_LARGE, SATURATION_MAX, BRIGHTNESS_MAX);
}

/*
 * Degree + blur + alpha sweep
 */
GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Blur_Alpha_Min_Test)
{
    SetUpBlurAlphaDegreeNode(LIGHT_UP_DEGREE_HALF, COLOR_MASK_RED,
        BLUR_RADIUS_SMALL, SATURATION_MIN, BRIGHTNESS_MIN, ALPHA_MIN);
}

GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Blur_Alpha_Low_Test)
{
    SetUpBlurAlphaDegreeNode(LIGHT_UP_DEGREE_DEFAULT, COLOR_MASK_GREEN,
        BLUR_RADIUS_MEDIUM, SATURATION_LOW, BRIGHTNESS_LOW, ALPHA_LOW);
}

GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Blur_Alpha_Mid_Test)
{
    SetUpBlurAlphaDegreeNode(LIGHT_UP_DEGREE_ONE_AND_HALF, COLOR_MASK_BLUE,
        BLUR_RADIUS_MEDIUM, SATURATION_MID, BRIGHTNESS_MID, ALPHA_MID);
}

GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Blur_Alpha_High_Test)
{
    SetUpBlurAlphaDegreeNode(LIGHT_UP_DEGREE_DOUBLE, COLOR_MASK_RED,
        BLUR_RADIUS_LARGE, SATURATION_HIGH, BRIGHTNESS_HIGH, ALPHA_HIGH);
}

GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Blur_Alpha_Max_Test)
{
    SetUpBlurAlphaDegreeNode(LIGHT_UP_DEGREE_LARGE, COLOR_MASK_GREEN,
        BLUR_RADIUS_LARGE, SATURATION_MAX, BRIGHTNESS_MAX, ALPHA_MAX);
}

/*
 * Degree + blur + alpha: over-range alpha (robustness)
 */
GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Blur_Alpha_Over_Test)
{
    SetUpBlurAlphaDegreeNode(LIGHT_UP_DEGREE_ONE_AND_HALF, COLOR_MASK_BLUE,
        BLUR_RADIUS_MEDIUM, SATURATION_MID, BRIGHTNESS_MID, ALPHA_OVER);
}

/*
 * Degree + blur + alpha: zero and negative degree combined with alpha
 */
GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Blur_Alpha_ZeroDegree_Test)
{
    SetUpBlurAlphaDegreeNode(LIGHT_UP_DEGREE_ZERO, COLOR_MASK_RED,
        BLUR_RADIUS_MEDIUM, SATURATION_MID, BRIGHTNESS_MID, ALPHA_MID);
}

GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Blur_Alpha_QuarterDegree_Test)
{
    SetUpBlurAlphaDegreeNode(LIGHT_UP_DEGREE_QUARTER, COLOR_MASK_GREEN,
        BLUR_RADIUS_SMALL, SATURATION_LOW, BRIGHTNESS_LOW, ALPHA_LOW);
}

GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Blur_Alpha_NegativeDegree_Test)
{
    SetUpBlurAlphaDegreeNode(LIGHT_UP_DEGREE_NEGATIVE, COLOR_MASK_BLUE,
        BLUR_RADIUS_LARGE, SATURATION_HIGH, BRIGHTNESS_HIGH, ALPHA_HIGH);
}

/*
 * Compose: two lightUp filters with different degree combinations
 */
GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Compose_Half_And_OneAndHalf_Test)
{
    auto filter1 = std::make_shared<RSLightUpEffectFilter>(LIGHT_UP_DEGREE_HALF);
    auto filter2 = std::make_shared<RSLightUpEffectFilter>(LIGHT_UP_DEGREE_ONE_AND_HALF);
    auto composedFilter = filter1->Compose(filter2);
    auto testNode =
        SetUpNodeBgImage(BG_IMAGE_PATH, { 0, 0, screenWidth - SCREEN_MARGIN, screenHeight - SCREEN_MARGIN });
    testNode->SetBackgroundFilter(composedFilter);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Compose_Quarter_And_Double_Test)
{
    auto filter1 = std::make_shared<RSLightUpEffectFilter>(LIGHT_UP_DEGREE_QUARTER);
    auto filter2 = std::make_shared<RSLightUpEffectFilter>(LIGHT_UP_DEGREE_DOUBLE);
    auto composedFilter = filter1->Compose(filter2);
    auto testNode =
        SetUpNodeBgImage(BG_IMAGE_PATH, { 0, 0, screenWidth - SCREEN_MARGIN, screenHeight - SCREEN_MARGIN });
    testNode->SetBackgroundFilter(composedFilter);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Compose_Double_And_Quarter_Test)
{
    auto filter1 = std::make_shared<RSLightUpEffectFilter>(LIGHT_UP_DEGREE_DOUBLE);
    auto filter2 = std::make_shared<RSLightUpEffectFilter>(LIGHT_UP_DEGREE_QUARTER);
    auto composedFilter = filter1->Compose(filter2);
    auto testNode =
        SetUpNodeBgImage(BG_IMAGE_PATH, { 0, 0, screenWidth - SCREEN_MARGIN, screenHeight - SCREEN_MARGIN });
    testNode->SetBackgroundFilter(composedFilter);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Compose_Large_And_Negative_Test)
{
    auto filter1 = std::make_shared<RSLightUpEffectFilter>(LIGHT_UP_DEGREE_LARGE);
    auto filter2 = std::make_shared<RSLightUpEffectFilter>(LIGHT_UP_DEGREE_NEGATIVE);
    auto composedFilter = filter1->Compose(filter2);
    auto testNode =
        SetUpNodeBgImage(BG_IMAGE_PATH, { 0, 0, screenWidth - SCREEN_MARGIN, screenHeight - SCREEN_MARGIN });
    testNode->SetBackgroundFilter(composedFilter);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * Grid layout: all degree values side by side for visual comparison
 */
GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Grid_Degree_Test)
{
    int columnCount = COLUMN_COUNT_2;
    const std::vector<float> degrees = {
        LIGHT_UP_DEGREE_NEGATIVE, LIGHT_UP_DEGREE_ZERO,
        LIGHT_UP_DEGREE_QUARTER, LIGHT_UP_DEGREE_HALF,
        LIGHT_UP_DEGREE_DEFAULT, LIGHT_UP_DEGREE_ONE_AND_HALF,
        LIGHT_UP_DEGREE_DOUBLE, LIGHT_UP_DEGREE_LARGE,
        LIGHT_UP_DEGREE_EXTREME
    };
    int rowCount = static_cast<int>(degrees.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNode =
            SetUpNodeBgImage(BG_IMAGE_PATH, { x, y, sizeX - SCREEN_MARGIN, sizeY - SCREEN_MARGIN });
        testNode->SetLightUpEffectDegree(degrees[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * Grid layout: degrees combined with background blur
 */
GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Grid_Blur_Test)
{
    int columnCount = COLUMN_COUNT_3;
    const std::vector<float> degrees = {
        LIGHT_UP_DEGREE_HALF, LIGHT_UP_DEGREE_DEFAULT,
        LIGHT_UP_DEGREE_ONE_AND_HALF, LIGHT_UP_DEGREE_DOUBLE,
        LIGHT_UP_DEGREE_LARGE, LIGHT_UP_DEGREE_NEGATIVE
    };
    const std::vector<int> radii = {
        BLUR_RADIUS_SMALL, BLUR_RADIUS_MEDIUM,
        BLUR_RADIUS_LARGE, BLUR_RADIUS_SMALL,
        BLUR_RADIUS_MEDIUM, BLUR_RADIUS_LARGE
    };
    int rowCount = static_cast<int>(degrees.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNode =
            SetUpNodeBgImage(BG_IMAGE_PATH, { x, y, sizeX - SCREEN_MARGIN, sizeY - SCREEN_MARGIN });
        testNode->SetBackgroundBlurRadius(radii[i]);
        testNode->SetBackgroundBlurSaturation(SATURATION_MID);
        testNode->SetBackgroundBlurBrightness(BRIGHTNESS_MID);
        testNode->SetBackgroundBlurMaskColor(Color(COLOR_MASK_RED));
        testNode->SetLightUpEffectDegree(degrees[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

} // namespace OHOS::Rosen
