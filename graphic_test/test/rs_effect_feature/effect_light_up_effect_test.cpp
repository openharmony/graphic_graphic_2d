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
};

/*
 * Test lightUpEffectDegree sweep: negative value
 * Verifies that a negative degree dims the content significantly
 */
GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Degree_Negative_Test)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float lightUpDegree = LIGHT_UP_DEGREE_NEGATIVE;
    auto testNode =
        SetUpNodeBgImage(BG_IMAGE_PATH, { x, y, sizeX - SCREEN_MARGIN, sizeY - SCREEN_MARGIN });
    testNode->SetLightUpEffectDegree(lightUpDegree);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * Test lightUpEffectDegree sweep: zero value
 * Verifies that zero degree makes content fully dark
 */
GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Degree_Zero_Test)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float lightUpDegree = LIGHT_UP_DEGREE_ZERO;
    auto testNode =
        SetUpNodeBgImage(BG_IMAGE_PATH, { x, y, sizeX - SCREEN_MARGIN, sizeY - SCREEN_MARGIN });
    testNode->SetLightUpEffectDegree(lightUpDegree);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * Test lightUpEffectDegree sweep: 0.25 (quarter brightness)
 * Verifies reduced brightness with small positive degree
 */
GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Degree_Quarter_Test)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float lightUpDegree = LIGHT_UP_DEGREE_QUARTER;
    auto testNode =
        SetUpNodeBgImage(BG_IMAGE_PATH, { x, y, sizeX - SCREEN_MARGIN, sizeY - SCREEN_MARGIN });
    testNode->SetLightUpEffectDegree(lightUpDegree);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * Test lightUpEffectDegree sweep: 0.5 (half brightness)
 * Verifies moderate dimming with half degree
 */
GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Degree_Half_Test)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float lightUpDegree = LIGHT_UP_DEGREE_HALF;
    auto testNode =
        SetUpNodeBgImage(BG_IMAGE_PATH, { x, y, sizeX - SCREEN_MARGIN, sizeY - SCREEN_MARGIN });
    testNode->SetLightUpEffectDegree(lightUpDegree);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * Test lightUpEffectDegree sweep: 1.0 (default, no effect)
 * Verifies that default degree produces no visual change
 */
GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Degree_Default_Test)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float lightUpDegree = LIGHT_UP_DEGREE_DEFAULT;
    auto testNode =
        SetUpNodeBgImage(BG_IMAGE_PATH, { x, y, sizeX - SCREEN_MARGIN, sizeY - SCREEN_MARGIN });
    testNode->SetLightUpEffectDegree(lightUpDegree);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * Test lightUpEffectDegree sweep: 1.5 (moderate light up)
 * Verifies moderate brightness increase
 */
GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Degree_OneAndHalf_Test)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float lightUpDegree = LIGHT_UP_DEGREE_ONE_AND_HALF;
    auto testNode =
        SetUpNodeBgImage(BG_IMAGE_PATH, { x, y, sizeX - SCREEN_MARGIN, sizeY - SCREEN_MARGIN });
    testNode->SetLightUpEffectDegree(lightUpDegree);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * Test lightUpEffectDegree sweep: 2.0 (double light up)
 * Verifies significant brightness increase
 */
GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Degree_Double_Test)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float lightUpDegree = LIGHT_UP_DEGREE_DOUBLE;
    auto testNode =
        SetUpNodeBgImage(BG_IMAGE_PATH, { x, y, sizeX - SCREEN_MARGIN, sizeY - SCREEN_MARGIN });
    testNode->SetLightUpEffectDegree(lightUpDegree);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * Test lightUpEffectDegree sweep: 5.0 (large degree)
 * Verifies behavior with a large brightness boost
 */
GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Degree_Large_Test)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float lightUpDegree = LIGHT_UP_DEGREE_LARGE;
    auto testNode =
        SetUpNodeBgImage(BG_IMAGE_PATH, { x, y, sizeX - SCREEN_MARGIN, sizeY - SCREEN_MARGIN });
    testNode->SetLightUpEffectDegree(lightUpDegree);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * Test lightUpEffectDegree sweep: 200.0 (extreme degree)
 * Verifies behavior with an extreme brightness boost (robustness)
 */
GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Degree_Extreme_Test)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float lightUpDegree = LIGHT_UP_DEGREE_EXTREME;
    auto testNode =
        SetUpNodeBgImage(BG_IMAGE_PATH, { x, y, sizeX - SCREEN_MARGIN, sizeY - SCREEN_MARGIN });
    testNode->SetLightUpEffectDegree(lightUpDegree);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * Test lightUpEffect combined with background blur, mask color red, and color mode variations
 * Row layout: each row tests a different colorMode value with fixed lightUp + blur
 */
GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Blur_Red_ColorMode_Test1)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Color color = Color(COLOR_MASK_RED);
    int radius = BLUR_RADIUS_MEDIUM;
    float saturation = SATURATION_MID;
    float brightness = BRIGHTNESS_MID;
    int colorMode = COLOR_MODE_INVALID;
    float lightUpDegree = LIGHT_UP_DEGREE_DEFAULT;
    int x = 0;
    int y = 0;
    auto testNode =
        SetUpNodeBgImage(BG_IMAGE_PATH, { x, y, sizeX - SCREEN_MARGIN, sizeY - SCREEN_MARGIN });
    testNode->SetBorderStyle(0, 0, 0, 0);
    testNode->SetBorderWidth(BORDER_WIDTH, BORDER_WIDTH, BORDER_WIDTH, BORDER_WIDTH);
    testNode->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNode->SetBackgroundBlurRadius(radius);
    testNode->SetBackgroundBlurSaturation(saturation);
    testNode->SetBackgroundBlurBrightness(brightness);
    testNode->SetBackgroundBlurColorMode(colorMode);
    testNode->SetBackgroundBlurMaskColor(color);
    testNode->SetLightUpEffectDegree(lightUpDegree);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Blur_Red_ColorMode_Test2)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Color color = Color(COLOR_MASK_RED);
    int radius = BLUR_RADIUS_MEDIUM;
    float saturation = SATURATION_MID;
    float brightness = BRIGHTNESS_MID;
    int colorMode = COLOR_MODE_DEFAULT;
    float lightUpDegree = LIGHT_UP_DEGREE_DEFAULT;
    int x = 0;
    int y = 0;
    auto testNode =
        SetUpNodeBgImage(BG_IMAGE_PATH, { x, y, sizeX - SCREEN_MARGIN, sizeY - SCREEN_MARGIN });
    testNode->SetBorderStyle(0, 0, 0, 0);
    testNode->SetBorderWidth(BORDER_WIDTH, BORDER_WIDTH, BORDER_WIDTH, BORDER_WIDTH);
    testNode->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNode->SetBackgroundBlurRadius(radius);
    testNode->SetBackgroundBlurSaturation(saturation);
    testNode->SetBackgroundBlurBrightness(brightness);
    testNode->SetBackgroundBlurColorMode(colorMode);
    testNode->SetBackgroundBlurMaskColor(color);
    testNode->SetLightUpEffectDegree(lightUpDegree);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Blur_Red_ColorMode_Test3)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Color color = Color(COLOR_MASK_RED);
    int radius = BLUR_RADIUS_MEDIUM;
    float saturation = SATURATION_MID;
    float brightness = BRIGHTNESS_MID;
    int colorMode = COLOR_MODE_ALT;
    float lightUpDegree = LIGHT_UP_DEGREE_DEFAULT;
    int x = 0;
    int y = 0;
    auto testNode =
        SetUpNodeBgImage(BG_IMAGE_PATH, { x, y, sizeX - SCREEN_MARGIN, sizeY - SCREEN_MARGIN });
    testNode->SetBorderStyle(0, 0, 0, 0);
    testNode->SetBorderWidth(BORDER_WIDTH, BORDER_WIDTH, BORDER_WIDTH, BORDER_WIDTH);
    testNode->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNode->SetBackgroundBlurRadius(radius);
    testNode->SetBackgroundBlurSaturation(saturation);
    testNode->SetBackgroundBlurBrightness(brightness);
    testNode->SetBackgroundBlurColorMode(colorMode);
    testNode->SetBackgroundBlurMaskColor(color);
    testNode->SetLightUpEffectDegree(lightUpDegree);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Blur_Red_ColorMode_Test4)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Color color = Color(COLOR_MASK_RED);
    int radius = BLUR_RADIUS_MEDIUM;
    float saturation = SATURATION_MID;
    float brightness = BRIGHTNESS_MID;
    int colorMode = COLOR_MODE_EXTENDED;
    float lightUpDegree = LIGHT_UP_DEGREE_DEFAULT;
    int x = 0;
    int y = 0;
    auto testNode =
        SetUpNodeBgImage(BG_IMAGE_PATH, { x, y, sizeX - SCREEN_MARGIN, sizeY - SCREEN_MARGIN });
    testNode->SetBorderStyle(0, 0, 0, 0);
    testNode->SetBorderWidth(BORDER_WIDTH, BORDER_WIDTH, BORDER_WIDTH, BORDER_WIDTH);
    testNode->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNode->SetBackgroundBlurRadius(radius);
    testNode->SetBackgroundBlurSaturation(saturation);
    testNode->SetBackgroundBlurBrightness(brightness);
    testNode->SetBackgroundBlurColorMode(colorMode);
    testNode->SetBackgroundBlurMaskColor(color);
    testNode->SetLightUpEffectDegree(lightUpDegree);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * Test lightUpEffect combined with background blur and green mask color
 * Verifies lightUp with green overlay mask and different blur radii
 */
GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Blur_Green_ColorMode_Test1)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Color color = Color(COLOR_MASK_GREEN);
    int radius = BLUR_RADIUS_SMALL;
    float saturation = SATURATION_MIN;
    float brightness = BRIGHTNESS_MIN;
    int colorMode = COLOR_MODE_DEFAULT;
    float lightUpDegree = LIGHT_UP_DEGREE_HALF;
    int x = 0;
    int y = 0;
    auto testNode =
        SetUpNodeBgImage(BG_IMAGE_PATH, { x, y, sizeX - SCREEN_MARGIN, sizeY - SCREEN_MARGIN });
    testNode->SetBorderStyle(0, 0, 0, 0);
    testNode->SetBorderWidth(BORDER_WIDTH, BORDER_WIDTH, BORDER_WIDTH, BORDER_WIDTH);
    testNode->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNode->SetBackgroundBlurRadius(radius);
    testNode->SetBackgroundBlurSaturation(saturation);
    testNode->SetBackgroundBlurBrightness(brightness);
    testNode->SetBackgroundBlurColorMode(colorMode);
    testNode->SetBackgroundBlurMaskColor(color);
    testNode->SetLightUpEffectDegree(lightUpDegree);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Blur_Green_ColorMode_Test2)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Color color = Color(COLOR_MASK_GREEN);
    int radius = BLUR_RADIUS_MEDIUM;
    float saturation = SATURATION_MID;
    float brightness = BRIGHTNESS_MID;
    int colorMode = COLOR_MODE_ALT;
    float lightUpDegree = LIGHT_UP_DEGREE_DEFAULT;
    int x = 0;
    int y = 0;
    auto testNode =
        SetUpNodeBgImage(BG_IMAGE_PATH, { x, y, sizeX - SCREEN_MARGIN, sizeY - SCREEN_MARGIN });
    testNode->SetBorderStyle(0, 0, 0, 0);
    testNode->SetBorderWidth(BORDER_WIDTH, BORDER_WIDTH, BORDER_WIDTH, BORDER_WIDTH);
    testNode->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNode->SetBackgroundBlurRadius(radius);
    testNode->SetBackgroundBlurSaturation(saturation);
    testNode->SetBackgroundBlurBrightness(brightness);
    testNode->SetBackgroundBlurColorMode(colorMode);
    testNode->SetBackgroundBlurMaskColor(color);
    testNode->SetLightUpEffectDegree(lightUpDegree);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Blur_Green_ColorMode_Test3)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Color color = Color(COLOR_MASK_GREEN);
    int radius = BLUR_RADIUS_LARGE;
    float saturation = SATURATION_MAX;
    float brightness = BRIGHTNESS_MAX;
    int colorMode = COLOR_MODE_DEFAULT;
    float lightUpDegree = LIGHT_UP_DEGREE_DOUBLE;
    int x = 0;
    int y = 0;
    auto testNode =
        SetUpNodeBgImage(BG_IMAGE_PATH, { x, y, sizeX - SCREEN_MARGIN, sizeY - SCREEN_MARGIN });
    testNode->SetBorderStyle(0, 0, 0, 0);
    testNode->SetBorderWidth(BORDER_WIDTH, BORDER_WIDTH, BORDER_WIDTH, BORDER_WIDTH);
    testNode->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNode->SetBackgroundBlurRadius(radius);
    testNode->SetBackgroundBlurSaturation(saturation);
    testNode->SetBackgroundBlurBrightness(brightness);
    testNode->SetBackgroundBlurColorMode(colorMode);
    testNode->SetBackgroundBlurMaskColor(color);
    testNode->SetLightUpEffectDegree(lightUpDegree);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * Test lightUpEffect combined with background blur and blue mask color
 * Verifies lightUp with blue overlay mask and varying saturation/brightness
 */
GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Blur_Blue_ColorMode_Test1)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Color color = Color(COLOR_MASK_BLUE);
    int radius = BLUR_RADIUS_MEDIUM;
    float saturation = SATURATION_MID;
    float brightness = BRIGHTNESS_MID;
    int colorMode = COLOR_MODE_INVALID;
    float lightUpDegree = LIGHT_UP_DEGREE_ONE_AND_HALF;
    int x = 0;
    int y = 0;
    auto testNode =
        SetUpNodeBgImage(BG_IMAGE_PATH, { x, y, sizeX - SCREEN_MARGIN, sizeY - SCREEN_MARGIN });
    testNode->SetBorderStyle(0, 0, 0, 0);
    testNode->SetBorderWidth(BORDER_WIDTH, BORDER_WIDTH, BORDER_WIDTH, BORDER_WIDTH);
    testNode->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNode->SetBackgroundBlurRadius(radius);
    testNode->SetBackgroundBlurSaturation(saturation);
    testNode->SetBackgroundBlurBrightness(brightness);
    testNode->SetBackgroundBlurColorMode(colorMode);
    testNode->SetBackgroundBlurMaskColor(color);
    testNode->SetLightUpEffectDegree(lightUpDegree);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Blur_Blue_ColorMode_Test2)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Color color = Color(COLOR_MASK_BLUE);
    int radius = BLUR_RADIUS_MEDIUM;
    float saturation = SATURATION_MID;
    float brightness = BRIGHTNESS_MID;
    int colorMode = COLOR_MODE_DEFAULT;
    float lightUpDegree = LIGHT_UP_DEGREE_ONE_AND_HALF;
    int x = 0;
    int y = 0;
    auto testNode =
        SetUpNodeBgImage(BG_IMAGE_PATH, { x, y, sizeX - SCREEN_MARGIN, sizeY - SCREEN_MARGIN });
    testNode->SetBorderStyle(0, 0, 0, 0);
    testNode->SetBorderWidth(BORDER_WIDTH, BORDER_WIDTH, BORDER_WIDTH, BORDER_WIDTH);
    testNode->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNode->SetBackgroundBlurRadius(radius);
    testNode->SetBackgroundBlurSaturation(saturation);
    testNode->SetBackgroundBlurBrightness(brightness);
    testNode->SetBackgroundBlurColorMode(colorMode);
    testNode->SetBackgroundBlurMaskColor(color);
    testNode->SetLightUpEffectDegree(lightUpDegree);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Blur_Blue_ColorMode_Test3)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Color color = Color(COLOR_MASK_BLUE);
    int radius = BLUR_RADIUS_MEDIUM;
    float saturation = SATURATION_MID;
    float brightness = BRIGHTNESS_MID;
    int colorMode = COLOR_MODE_ALT;
    float lightUpDegree = LIGHT_UP_DEGREE_ONE_AND_HALF;
    int x = 0;
    int y = 0;
    auto testNode =
        SetUpNodeBgImage(BG_IMAGE_PATH, { x, y, sizeX - SCREEN_MARGIN, sizeY - SCREEN_MARGIN });
    testNode->SetBorderStyle(0, 0, 0, 0);
    testNode->SetBorderWidth(BORDER_WIDTH, BORDER_WIDTH, BORDER_WIDTH, BORDER_WIDTH);
    testNode->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNode->SetBackgroundBlurRadius(radius);
    testNode->SetBackgroundBlurSaturation(saturation);
    testNode->SetBackgroundBlurBrightness(brightness);
    testNode->SetBackgroundBlurColorMode(colorMode);
    testNode->SetBackgroundBlurMaskColor(color);
    testNode->SetLightUpEffectDegree(lightUpDegree);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * Test lightUpEffect with background blur mask color sweep (black to transparent)
 * Combines increasing blur radius/saturation/brightness with varying mask colors
 */
GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Blur_MaskColor_Black_Test)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Color color = Color(COLOR_MASK_BLACK);
    int radius = BLUR_RADIUS_SMALL;
    float saturation = SATURATION_MIN;
    float brightness = BRIGHTNESS_MIN;
    float lightUpDegree = LIGHT_UP_DEGREE_HALF;
    int x = 0;
    int y = 0;
    auto testNode =
        SetUpNodeBgImage(BG_IMAGE_PATH, { x, y, sizeX - SCREEN_MARGIN, sizeY - SCREEN_MARGIN });
    testNode->SetBorderStyle(0, 0, 0, 0);
    testNode->SetBorderWidth(BORDER_WIDTH, BORDER_WIDTH, BORDER_WIDTH, BORDER_WIDTH);
    testNode->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNode->SetBackgroundBlurRadius(radius);
    testNode->SetBackgroundBlurSaturation(saturation);
    testNode->SetBackgroundBlurBrightness(brightness);
    testNode->SetBackgroundBlurMaskColor(color);
    testNode->SetLightUpEffectDegree(lightUpDegree);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Blur_MaskColor_White_Test)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Color color = Color(COLOR_MASK_WHITE);
    int radius = BLUR_RADIUS_MEDIUM;
    float saturation = SATURATION_LOW;
    float brightness = BRIGHTNESS_LOW;
    float lightUpDegree = LIGHT_UP_DEGREE_DEFAULT;
    int x = 0;
    int y = 0;
    auto testNode =
        SetUpNodeBgImage(BG_IMAGE_PATH, { x, y, sizeX - SCREEN_MARGIN, sizeY - SCREEN_MARGIN });
    testNode->SetBorderStyle(0, 0, 0, 0);
    testNode->SetBorderWidth(BORDER_WIDTH, BORDER_WIDTH, BORDER_WIDTH, BORDER_WIDTH);
    testNode->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNode->SetBackgroundBlurRadius(radius);
    testNode->SetBackgroundBlurSaturation(saturation);
    testNode->SetBackgroundBlurBrightness(brightness);
    testNode->SetBackgroundBlurMaskColor(color);
    testNode->SetLightUpEffectDegree(lightUpDegree);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Blur_MaskColor_Red_Test)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Color color = Color(COLOR_MASK_RED);
    int radius = BLUR_RADIUS_MEDIUM;
    float saturation = SATURATION_MID;
    float brightness = BRIGHTNESS_MID;
    float lightUpDegree = LIGHT_UP_DEGREE_ONE_AND_HALF;
    int x = 0;
    int y = 0;
    auto testNode =
        SetUpNodeBgImage(BG_IMAGE_PATH, { x, y, sizeX - SCREEN_MARGIN, sizeY - SCREEN_MARGIN });
    testNode->SetBorderStyle(0, 0, 0, 0);
    testNode->SetBorderWidth(BORDER_WIDTH, BORDER_WIDTH, BORDER_WIDTH, BORDER_WIDTH);
    testNode->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNode->SetBackgroundBlurRadius(radius);
    testNode->SetBackgroundBlurSaturation(saturation);
    testNode->SetBackgroundBlurBrightness(brightness);
    testNode->SetBackgroundBlurMaskColor(color);
    testNode->SetLightUpEffectDegree(lightUpDegree);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Blur_MaskColor_Transparent_Test)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Color color = Color(COLOR_MASK_TRANSPARENT);
    int radius = BLUR_RADIUS_LARGE;
    float saturation = SATURATION_MAX;
    float brightness = BRIGHTNESS_MAX;
    float lightUpDegree = LIGHT_UP_DEGREE_DOUBLE;
    int x = 0;
    int y = 0;
    auto testNode =
        SetUpNodeBgImage(BG_IMAGE_PATH, { x, y, sizeX - SCREEN_MARGIN, sizeY - SCREEN_MARGIN });
    testNode->SetBorderStyle(0, 0, 0, 0);
    testNode->SetBorderWidth(BORDER_WIDTH, BORDER_WIDTH, BORDER_WIDTH, BORDER_WIDTH);
    testNode->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNode->SetBackgroundBlurRadius(radius);
    testNode->SetBackgroundBlurSaturation(saturation);
    testNode->SetBackgroundBlurBrightness(brightness);
    testNode->SetBackgroundBlurMaskColor(color);
    testNode->SetLightUpEffectDegree(lightUpDegree);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * Test lightUpEffect with background blur and alpha sweep
 * Verifies interaction between lightUp effect and node alpha
 */
GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Blur_Alpha_Min_Test)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Color color = Color(COLOR_MASK_RED);
    int radius = BLUR_RADIUS_SMALL;
    float saturation = SATURATION_MIN;
    float brightness = BRIGHTNESS_MIN;
    float alpha = ALPHA_MIN;
    float lightUpDegree = LIGHT_UP_DEGREE_HALF;
    int x = 0;
    int y = 0;
    auto testNode =
        SetUpNodeBgImage(BG_IMAGE_PATH, { x, y, sizeX - SCREEN_MARGIN, sizeY - SCREEN_MARGIN });
    testNode->SetBorderStyle(0, 0, 0, 0);
    testNode->SetBorderWidth(BORDER_WIDTH, BORDER_WIDTH, BORDER_WIDTH, BORDER_WIDTH);
    testNode->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNode->SetBackgroundBlurRadius(radius);
    testNode->SetBackgroundBlurSaturation(saturation);
    testNode->SetBackgroundBlurBrightness(brightness);
    testNode->SetBackgroundBlurMaskColor(color);
    testNode->SetAlpha(alpha);
    testNode->SetLightUpEffectDegree(lightUpDegree);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Blur_Alpha_Low_Test)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Color color = Color(COLOR_MASK_GREEN);
    int radius = BLUR_RADIUS_MEDIUM;
    float saturation = SATURATION_LOW;
    float brightness = BRIGHTNESS_LOW;
    float alpha = ALPHA_LOW;
    float lightUpDegree = LIGHT_UP_DEGREE_DEFAULT;
    int x = 0;
    int y = 0;
    auto testNode =
        SetUpNodeBgImage(BG_IMAGE_PATH, { x, y, sizeX - SCREEN_MARGIN, sizeY - SCREEN_MARGIN });
    testNode->SetBorderStyle(0, 0, 0, 0);
    testNode->SetBorderWidth(BORDER_WIDTH, BORDER_WIDTH, BORDER_WIDTH, BORDER_WIDTH);
    testNode->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNode->SetBackgroundBlurRadius(radius);
    testNode->SetBackgroundBlurSaturation(saturation);
    testNode->SetBackgroundBlurBrightness(brightness);
    testNode->SetBackgroundBlurMaskColor(color);
    testNode->SetAlpha(alpha);
    testNode->SetLightUpEffectDegree(lightUpDegree);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Blur_Alpha_Mid_Test)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Color color = Color(COLOR_MASK_BLUE);
    int radius = BLUR_RADIUS_MEDIUM;
    float saturation = SATURATION_MID;
    float brightness = BRIGHTNESS_MID;
    float alpha = ALPHA_MID;
    float lightUpDegree = LIGHT_UP_DEGREE_ONE_AND_HALF;
    int x = 0;
    int y = 0;
    auto testNode =
        SetUpNodeBgImage(BG_IMAGE_PATH, { x, y, sizeX - SCREEN_MARGIN, sizeY - SCREEN_MARGIN });
    testNode->SetBorderStyle(0, 0, 0, 0);
    testNode->SetBorderWidth(BORDER_WIDTH, BORDER_WIDTH, BORDER_WIDTH, BORDER_WIDTH);
    testNode->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNode->SetBackgroundBlurRadius(radius);
    testNode->SetBackgroundBlurSaturation(saturation);
    testNode->SetBackgroundBlurBrightness(brightness);
    testNode->SetBackgroundBlurMaskColor(color);
    testNode->SetAlpha(alpha);
    testNode->SetLightUpEffectDegree(lightUpDegree);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Blur_Alpha_High_Test)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Color color = Color(COLOR_MASK_RED);
    int radius = BLUR_RADIUS_LARGE;
    float saturation = SATURATION_HIGH;
    float brightness = BRIGHTNESS_HIGH;
    float alpha = ALPHA_HIGH;
    float lightUpDegree = LIGHT_UP_DEGREE_DOUBLE;
    int x = 0;
    int y = 0;
    auto testNode =
        SetUpNodeBgImage(BG_IMAGE_PATH, { x, y, sizeX - SCREEN_MARGIN, sizeY - SCREEN_MARGIN });
    testNode->SetBorderStyle(0, 0, 0, 0);
    testNode->SetBorderWidth(BORDER_WIDTH, BORDER_WIDTH, BORDER_WIDTH, BORDER_WIDTH);
    testNode->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNode->SetBackgroundBlurRadius(radius);
    testNode->SetBackgroundBlurSaturation(saturation);
    testNode->SetBackgroundBlurBrightness(brightness);
    testNode->SetBackgroundBlurMaskColor(color);
    testNode->SetAlpha(alpha);
    testNode->SetLightUpEffectDegree(lightUpDegree);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Blur_Alpha_Max_Test)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Color color = Color(COLOR_MASK_GREEN);
    int radius = BLUR_RADIUS_LARGE;
    float saturation = SATURATION_MAX;
    float brightness = BRIGHTNESS_MAX;
    float alpha = ALPHA_MAX;
    float lightUpDegree = LIGHT_UP_DEGREE_LARGE;
    int x = 0;
    int y = 0;
    auto testNode =
        SetUpNodeBgImage(BG_IMAGE_PATH, { x, y, sizeX - SCREEN_MARGIN, sizeY - SCREEN_MARGIN });
    testNode->SetBorderStyle(0, 0, 0, 0);
    testNode->SetBorderWidth(BORDER_WIDTH, BORDER_WIDTH, BORDER_WIDTH, BORDER_WIDTH);
    testNode->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNode->SetBackgroundBlurRadius(radius);
    testNode->SetBackgroundBlurSaturation(saturation);
    testNode->SetBackgroundBlurBrightness(brightness);
    testNode->SetBackgroundBlurMaskColor(color);
    testNode->SetAlpha(alpha);
    testNode->SetLightUpEffectDegree(lightUpDegree);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * Test lightUpEffect with over-range alpha (robustness)
 * Verifies behavior when alpha exceeds the normal 0-1 range
 */
GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Blur_Alpha_Over_Test)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Color color = Color(COLOR_MASK_BLUE);
    int radius = BLUR_RADIUS_MEDIUM;
    float saturation = SATURATION_MID;
    float brightness = BRIGHTNESS_MID;
    float alpha = ALPHA_OVER;
    float lightUpDegree = LIGHT_UP_DEGREE_ONE_AND_HALF;
    int x = 0;
    int y = 0;
    auto testNode =
        SetUpNodeBgImage(BG_IMAGE_PATH, { x, y, sizeX - SCREEN_MARGIN, sizeY - SCREEN_MARGIN });
    testNode->SetBorderStyle(0, 0, 0, 0);
    testNode->SetBorderWidth(BORDER_WIDTH, BORDER_WIDTH, BORDER_WIDTH, BORDER_WIDTH);
    testNode->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNode->SetBackgroundBlurRadius(radius);
    testNode->SetBackgroundBlurSaturation(saturation);
    testNode->SetBackgroundBlurBrightness(brightness);
    testNode->SetBackgroundBlurMaskColor(color);
    testNode->SetAlpha(alpha);
    testNode->SetLightUpEffectDegree(lightUpDegree);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * Test RSLightUpEffectFilter Compose functionality
 * Verifies that composing two lightUp filters produces a valid result
 */
GRAPHIC_TEST(LightUpEffectTest, EFFECT_TEST, Set_Light_Up_Effect_Compose_Test)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    float lightUpDegree1 = LIGHT_UP_DEGREE_HALF;
    float lightUpDegree2 = LIGHT_UP_DEGREE_ONE_AND_HALF;
    auto filter1 = std::make_shared<RSLightUpEffectFilter>(lightUpDegree1);
    auto filter2 = std::make_shared<RSLightUpEffectFilter>(lightUpDegree2);
    auto composedFilter = filter1->Compose(filter2);
    int x = 0;
    int y = 0;
    auto testNode =
        SetUpNodeBgImage(BG_IMAGE_PATH, { x, y, sizeX - SCREEN_MARGIN, sizeY - SCREEN_MARGIN });
    testNode->SetBackgroundFilter(composedFilter);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * Test lightUpEffect grid layout: multiple degrees in a grid
 * Verifies different lightUp degrees side by side for visual comparison
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
 * Test lightUpEffect grid layout: degrees combined with background blur
 * Verifies lightUp + blur combination in a multi-cell grid
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
