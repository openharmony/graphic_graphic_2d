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

#include "rs_graphic_test.h"
#include "rs_graphic_test_img.h"

#include "render/rs_blur_filter.h"
#include "render/rs_material_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class BackgroundBlurTest : public RSGraphicTest {
private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;

public:
    // called before each tests
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }
};

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_SystemBar_Effect_Test)
{
    int columnCount = 1;
    int rowCount = 2;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;
    int x = (0 % columnCount) * sizeX;
    int y = (0 / columnCount) * sizeY;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetSystemBarEffect();
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
    x = (1 % columnCount) * sizeX;
    y = (1 / columnCount) * sizeY;
    auto testNodeBackGround2 =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    GetRootNode()->AddChild(testNodeBackGround2);
    RegisterNode(testNodeBackGround2);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Brightness_Test1)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float brightness = -1;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Brightness_Test2)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float brightness = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Brightness_Test3)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float brightness = 0.5;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Brightness_Test4)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float brightness = 1;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Brightness_Test5)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    float brightness = 200;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Color_Mode_Test1)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Color color = Color(0x2dff0000); // RgbPalette::Red()
    int radius = 30;
    float saturation = 0.6;
    float brightness = 0.6;
    int colorMode = -1;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    testNodeBackGround->SetBackgroundBlurColorMode(colorMode);
    testNodeBackGround->SetBackgroundBlurMaskColor(color);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Color_Mode_Test2)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Color color = Color(0x2dff0000); // RgbPalette::Red()
    int radius = 30;
    float saturation = 0.6;
    float brightness = 0.6;
    int colorMode = 0;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    testNodeBackGround->SetBackgroundBlurColorMode(colorMode);
    testNodeBackGround->SetBackgroundBlurMaskColor(color);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Color_Mode_Test3)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Color color = Color(0x2dff0000); // RgbPalette::Red()
    int radius = 30;
    float saturation = 0.6;
    float brightness = 0.6;
    int colorMode = 1;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    testNodeBackGround->SetBackgroundBlurColorMode(colorMode);
    testNodeBackGround->SetBackgroundBlurMaskColor(color);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Color_Mode_Test4)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Color color = Color(0x2dff0000); // RgbPalette::Red()
    int radius = 30;
    float saturation = 0.6;
    float brightness = 0.6;
    int colorMode = 2;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    testNodeBackGround->SetBackgroundBlurColorMode(colorMode);
    testNodeBackGround->SetBackgroundBlurMaskColor(color);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Color_Mode_Test5)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Color color = Color(0x2dff0000); // RgbPalette::Red()
    int radius = 30;
    float saturation = 0.6;
    float brightness = 0.6;
    int colorMode = 3;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    testNodeBackGround->SetBackgroundBlurColorMode(colorMode);
    testNodeBackGround->SetBackgroundBlurMaskColor(color);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Color_Mode_Test6)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Color color = Color(0x2dff0000); // RgbPalette::Red()
    int radius = 30;
    float saturation = 0.6;
    float brightness = 0.6;
    int colorMode = 100;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    testNodeBackGround->SetBackgroundBlurColorMode(colorMode);
    testNodeBackGround->SetBackgroundBlurMaskColor(color);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Color_Mode_Test7)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Color color = Color(0x4d00ff00); // RgbPalette::Green()
    int radius = 30;
    float saturation = 0.6;
    float brightness = 0.6;
    int colorMode = -1;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    testNodeBackGround->SetBackgroundBlurColorMode(colorMode);
    testNodeBackGround->SetBackgroundBlurMaskColor(color);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Color_Mode_Test8)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Color color = Color(0x4d00ff00); // RgbPalette::Green()
    int radius = 30;
    float saturation = 0.6;
    float brightness = 0.6;
    int colorMode = 0;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    testNodeBackGround->SetBackgroundBlurColorMode(colorMode);
    testNodeBackGround->SetBackgroundBlurMaskColor(color);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Color_Mode_Test9)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Color color = Color(0x4d00ff00); // RgbPalette::Green()
    int radius = 30;
    float saturation = 0.6;
    float brightness = 0.6;
    int colorMode = 1;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    testNodeBackGround->SetBackgroundBlurColorMode(colorMode);
    testNodeBackGround->SetBackgroundBlurMaskColor(color);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Color_Mode_Test10)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Color color = Color(0x4d00ff00); // RgbPalette::Green()
    int radius = 30;
    float saturation = 0.6;
    float brightness = 0.6;
    int colorMode = 2;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    testNodeBackGround->SetBackgroundBlurColorMode(colorMode);
    testNodeBackGround->SetBackgroundBlurMaskColor(color);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Color_Mode_Test11)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Color color = Color(0x4d00ff00); // RgbPalette::Green()
    int radius = 30;
    float saturation = 0.6;
    float brightness = 0.6;
    int colorMode = 3;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    testNodeBackGround->SetBackgroundBlurColorMode(colorMode);
    testNodeBackGround->SetBackgroundBlurMaskColor(color);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Color_Mode_Test12)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Color color = Color(0x4d00ff00); // RgbPalette::Green()
    int radius = 30;
    float saturation = 0.6;
    float brightness = 0.6;
    int colorMode = 100;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    testNodeBackGround->SetBackgroundBlurColorMode(colorMode);
    testNodeBackGround->SetBackgroundBlurMaskColor(color);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Color_Mode_Test13)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Color color = Color(0x6d0000ff); // RgbPalette::Blue()
    int radius = 30;
    float saturation = 0.6;
    float brightness = 0.6;
    int colorMode = -1;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    testNodeBackGround->SetBackgroundBlurColorMode(colorMode);
    testNodeBackGround->SetBackgroundBlurMaskColor(color);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Color_Mode_Test14)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Color color = Color(0x6d0000ff); // RgbPalette::Blue()
    int radius = 30;
    float saturation = 0.6;
    float brightness = 0.6;
    int colorMode = 0;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    testNodeBackGround->SetBackgroundBlurColorMode(colorMode);
    testNodeBackGround->SetBackgroundBlurMaskColor(color);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Color_Mode_Test15)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Color color = Color(0x6d0000ff); // RgbPalette::Blue()
    int radius = 30;
    float saturation = 0.6;
    float brightness = 0.6;
    int colorMode = 1;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    testNodeBackGround->SetBackgroundBlurColorMode(colorMode);
    testNodeBackGround->SetBackgroundBlurMaskColor(color);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Color_Mode_Test16)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Color color = Color(0x6d0000ff); // RgbPalette::Blue()
    int radius = 30;
    float saturation = 0.6;
    float brightness = 0.6;
    int colorMode = 2;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    testNodeBackGround->SetBackgroundBlurColorMode(colorMode);
    testNodeBackGround->SetBackgroundBlurMaskColor(color);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Color_Mode_Test17)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Color color = Color(0x6d0000ff); // RgbPalette::Blue()
    int radius = 30;
    float saturation = 0.6;
    float brightness = 0.6;
    int colorMode = 3;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    testNodeBackGround->SetBackgroundBlurColorMode(colorMode);
    testNodeBackGround->SetBackgroundBlurMaskColor(color);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Color_Mode_Test18)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Color color = Color(0x6d0000ff); // RgbPalette::Blue()
    int radius = 30;
    float saturation = 0.6;
    float brightness = 0.6;
    int colorMode = 100;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    testNodeBackGround->SetBackgroundBlurColorMode(colorMode);
    testNodeBackGround->SetBackgroundBlurMaskColor(color);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Mask_Color_Test1)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Color color = Color(0xff000000); // RgbPalette::Black()
    int radius = 10;
    float saturation = 0.2;
    float brightness = 0.2;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    testNodeBackGround->SetBackgroundBlurMaskColor(color);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Mask_Color_Test2)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Color color = Color(0xffffffff); // RgbPalette::White()
    int radius = 20;
    float saturation = 0.4;
    float brightness = 0.4;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    testNodeBackGround->SetBackgroundBlurMaskColor(color);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Mask_Color_Test3)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Color color = Color(0x2dff0000); // RgbPalette::Red()
    int radius = 30;
    float saturation = 0.6;
    float brightness = 0.6;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    testNodeBackGround->SetBackgroundBlurMaskColor(color);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Mask_Color_Test4)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Color color = Color(0x4d00ff00); // RgbPalette::Green()
    int radius = 40;
    float saturation = 0.8;
    float brightness = 0.8;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    testNodeBackGround->SetBackgroundBlurMaskColor(color);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Mask_Color_Test5)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Color color = Color(0x6d0000ff); // RgbPalette::Blue()
    int radius = 50;
    float saturation = 1.0;
    float brightness = 1.0;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    testNodeBackGround->SetBackgroundBlurMaskColor(color);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}
  
GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Mask_Color_Test6)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Color color = Color(0x8d00ffff); // RgbPalette::Cyan()
    int radius = 60;
    float saturation = 1.2;
    float brightness = 1.2;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    testNodeBackGround->SetBackgroundBlurMaskColor(color);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Mask_Color_Test7)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Color color = Color(0xad444444); // RgbPalette::Gray()
    int radius = 70;
    float saturation = 1.4;
    float brightness = 1.4;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    testNodeBackGround->SetBackgroundBlurMaskColor(color);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}
 
GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Mask_Color_Test8)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Color color = Color(0xcdff00ff); // RgbPalette::Magenta()
    int radius = 80;
    float saturation = 1.6;
    float brightness = 1.6;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    testNodeBackGround->SetBackgroundBlurMaskColor(color);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Mask_Color_Test9)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Color color = Color(0x00000000); // RgbPalette::Transparent()
    int radius = 90;
    float saturation = 1.8;
    float brightness = 1.8;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    testNodeBackGround->SetBackgroundBlurMaskColor(color);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Mask_Color_Test10)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Color color = Color(0x10000000); // RgbPalette::Transparent()
    int radius = -100;
    float saturation = 0.6;
    float brightness = 0.6;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    testNodeBackGround->SetBackgroundBlurMaskColor(color);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Mask_Color_Test11)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Color color = Color(0x20000000); // RgbPalette::Transparent()
    int radius = -10;
    float saturation = 1.0;
    float brightness = 1.0;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    testNodeBackGround->SetBackgroundBlurMaskColor(color);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Mask_Color_Test12)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Color color = Color(0xff00ff00); // RgbPalette::Green()
    int radius = -10;
    float saturation = -1.4;
    float brightness = -1.4;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    testNodeBackGround->SetBackgroundBlurMaskColor(color);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Mask_Color_Alpha_Test1)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Color color = Color(0xff000000); // RgbPalette::Black()
    int radius = 10;
    float saturation = 0.2;
    float brightness = 0.2;
    float alpha = 0.1;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    testNodeBackGround->SetBackgroundBlurMaskColor(color);
    testNodeBackGround->SetAlpha(alpha);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Mask_Color_Alpha_Test2)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Color color = Color(0xffffffff); // RgbPalette::White()
    int radius = 20;
    float saturation = 0.4;
    float brightness = 0.4;
    float alpha = 0.2;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    testNodeBackGround->SetBackgroundBlurMaskColor(color);
    testNodeBackGround->SetAlpha(alpha);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Mask_Color_Alpha_Test3)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Color color = Color(0x2dff0000); // RgbPalette::Red()
    int radius = 30;
    float saturation = 0.6;
    float brightness = 0.6;
    float alpha = 0.3;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    testNodeBackGround->SetBackgroundBlurMaskColor(color);
    testNodeBackGround->SetAlpha(alpha);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Mask_Color_Alpha_Test4)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Color color = Color(0x4d00ff00); // RgbPalette::Green()
    int radius = 40;
    float saturation = 0.8;
    float brightness = 0.8;
    float alpha = 0.4;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    testNodeBackGround->SetBackgroundBlurMaskColor(color);
    testNodeBackGround->SetAlpha(alpha);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Mask_Color_Alpha_Test5)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Color color = Color(0x6d0000ff); // RgbPalette::Blue()
    int radius = 50;
    float saturation = 1.0;
    float brightness = 1.0;
    float alpha = 0.5;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    testNodeBackGround->SetBackgroundBlurMaskColor(color);
    testNodeBackGround->SetAlpha(alpha);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}
  
GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Mask_Color_Alpha_Test6)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Color color = Color(0x8d00ffff); // RgbPalette::Cyan()
    int radius = 60;
    float saturation = 1.2;
    float brightness = 1.2;
    float alpha = 0.6;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    testNodeBackGround->SetBackgroundBlurMaskColor(color);
    testNodeBackGround->SetAlpha(alpha);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Mask_Color_Alpha_Test7)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Color color = Color(0xad444444); // RgbPalette::Gray()
    int radius = 70;
    float saturation = 1.4;
    float brightness = 1.4;
    float alpha = 0.7;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    testNodeBackGround->SetBackgroundBlurMaskColor(color);
    testNodeBackGround->SetAlpha(alpha);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}
 
GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Mask_Color_Alpha_Test8)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Color color = Color(0xcdff00ff); // RgbPalette::Magenta()
    int radius = 80;
    float saturation = 1.6;
    float brightness = 1.6;
    float alpha = 0.8;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    testNodeBackGround->SetBackgroundBlurMaskColor(color);
    testNodeBackGround->SetAlpha(alpha);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Mask_Color_Alpha_Test9)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Color color = Color(0x00000000); // RgbPalette::Transparent()
    int radius = 90;
    float saturation = 1.8;
    float brightness = 1.8;
    float alpha = 0.9;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    testNodeBackGround->SetBackgroundBlurMaskColor(color);
    testNodeBackGround->SetAlpha(alpha);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Mask_Color_Alpha_Test10)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Color color = Color(0x10000000); // RgbPalette::Transparent()
    int radius = -100;
    float saturation = 0.6;
    float brightness = 0.6;
    float alpha = 1.0;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    testNodeBackGround->SetBackgroundBlurMaskColor(color);
    testNodeBackGround->SetAlpha(alpha);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Mask_Color_Alpha_Test11)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Color color = Color(0x20000000); // RgbPalette::Transparent()
    int radius = -10;
    float saturation = 1.0;
    float brightness = 1.0;
    float alpha = 1.1;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    testNodeBackGround->SetBackgroundBlurMaskColor(color);
    testNodeBackGround->SetAlpha(alpha);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Mask_Color_Alpha_Test12)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Color color = Color(0xff00ff00); // RgbPalette::Green()
    int radius = -10;
    float saturation = -1.4;
    float brightness = -1.4;
    float alpha = 1.2;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    testNodeBackGround->SetBackgroundBlurBrightness(brightness);
    testNodeBackGround->SetBackgroundBlurMaskColor(color);
    testNodeBackGround->SetAlpha(alpha);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Radius_Test1)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    float radius = -1;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Radius_Test2)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    float radius = 0;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Radius_Test3)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    float radius = 10;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Radius_Test4)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    float radius = 20;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Radius_Test5)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    float radius = 30;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Radius_Test6)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    float radius = 50;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Radius_Test7)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    float radius = 100;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Radius_Test8)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    float radius = 150;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(radius);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Radius_X_Test1)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    float radius = -1;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadiusX(radius);
    testNodeBackGround->SetBackgroundBlurRadiusY(50);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Radius_X_Test2)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    float radius = 0;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadiusX(radius);
    testNodeBackGround->SetBackgroundBlurRadiusY(50);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Radius_X_Test3)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    float radius = 10;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadiusX(radius);
    testNodeBackGround->SetBackgroundBlurRadiusY(50);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Radius_X_Test4)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    float radius = 20;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadiusX(radius);
    testNodeBackGround->SetBackgroundBlurRadiusY(50);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Radius_X_Test5)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    float radius = 30;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadiusX(radius);
    testNodeBackGround->SetBackgroundBlurRadiusY(50);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Radius_X_Test6)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    float radius = 50;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadiusX(radius);
    testNodeBackGround->SetBackgroundBlurRadiusY(50);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Radius_X_Test7)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    float radius = 100;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadiusX(radius);
    testNodeBackGround->SetBackgroundBlurRadiusY(50);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Radius_X_Test8)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    float radius = 200;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadiusX(radius);
    testNodeBackGround->SetBackgroundBlurRadiusY(50);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Radius_Y_Test1)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    float radius = -1;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadiusX(50);
    testNodeBackGround->SetBackgroundBlurRadiusY(radius);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Radius_Y_Test2)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    float radius = 0;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadiusX(50);
    testNodeBackGround->SetBackgroundBlurRadiusY(radius);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Radius_Y_Test3)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    float radius = 10;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadiusX(50);
    testNodeBackGround->SetBackgroundBlurRadiusY(radius);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Radius_Y_Test4)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    float radius = 20;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadiusX(50);
    testNodeBackGround->SetBackgroundBlurRadiusY(radius);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Radius_Y_Test5)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    float radius = 30;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadiusX(50);
    testNodeBackGround->SetBackgroundBlurRadiusY(radius);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Radius_Y_Test6)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    float radius = 50;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadiusX(50);
    testNodeBackGround->SetBackgroundBlurRadiusY(radius);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Radius_Y_Test7)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    float radius = 100;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadiusX(50);
    testNodeBackGround->SetBackgroundBlurRadiusY(radius);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Radius_Y_Test8)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    float radius = 200;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadiusX(50);
    testNodeBackGround->SetBackgroundBlurRadiusY(radius);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Saturation_Test1)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    float saturation = -1;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Saturation_Test2)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    float saturation = 0;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Saturation_Test3)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    float saturation = 0.5;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Saturation_Test4)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    float saturation = 10;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Blur_Saturation_Test5)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    float saturation = 200;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurSaturation(saturation);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Grey_Coef_Test1)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Vector2f greyCoef(-1.f, -1.f); // range：[0, 127]
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(50);
    testNodeBackGround->SetGreyCoef(greyCoef);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Grey_Coef_Test2)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Vector2f greyCoef(0.f, 0.f); // range：[0, 127]
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(50);
    testNodeBackGround->SetGreyCoef(greyCoef);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Grey_Coef_Test3)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Vector2f greyCoef(20.f, 20.f); // range：[0, 127]
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(50);
    testNodeBackGround->SetGreyCoef(greyCoef);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Grey_Coef_Test4)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Vector2f greyCoef(50.f, 50.f); // range：[0, 127]
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(50);
    testNodeBackGround->SetGreyCoef(greyCoef);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Grey_Coef_Test5)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Vector2f greyCoef(80.f, 80.f); // range：[0, 127]
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(50);
    testNodeBackGround->SetGreyCoef(greyCoef);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Grey_Coef_Test6)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Vector2f greyCoef(127.f, 127.f); // range：[0, 127]
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(50);
    testNodeBackGround->SetGreyCoef(greyCoef);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Grey_Coef_Test7)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Vector2f greyCoef(128.f, 128.f); // range：[0, 127]
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(50);
    testNodeBackGround->SetGreyCoef(greyCoef);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Grey_Coef_Test8)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    Vector2f greyCoef(200.f, 200.f); // range：[0, 127]
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundBlurRadius(50);
    testNodeBackGround->SetGreyCoef(greyCoef);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Filter_Test1)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundFilter(nullptr);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Filter_Test2)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    MaterialParam materialParam = { 50, 0.8, 0.8, Color(0x2dff0000), false };
    std::shared_ptr<RSMaterialFilter> matFilter =
        std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::PRE_DEFINED);
    testNodeBackGround->SetBackgroundFilter(matFilter);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Background_Filter_Test3)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    std::shared_ptr<RSBlurFilter> blurFilter = std::make_shared<RSBlurFilter>(30, 30, false);
    testNodeBackGround->SetBackgroundFilter(blurFilter);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Bg_Blur_Disable_System_Adaptation_Test1)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundFilter(nullptr);
    testNodeBackGround->SetBgBlurDisableSystemAdaptation(false);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Bg_Blur_Disable_System_Adaptation_Test2)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    MaterialParam materialParam = { 50, 0.8, 0.8, Color(0x2dff0000), false };
    std::shared_ptr<RSMaterialFilter> matFilter =
        std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::PRE_DEFINED);
    testNodeBackGround->SetBackgroundFilter(matFilter);
    testNodeBackGround->SetBgBlurDisableSystemAdaptation(false);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Bg_Blur_Disable_System_Adaptation_Test3)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    std::shared_ptr<RSBlurFilter> blurFilter = std::make_shared<RSBlurFilter>(30, 30, false);
    testNodeBackGround->SetBackgroundFilter(blurFilter);
    testNodeBackGround->SetBgBlurDisableSystemAdaptation(false);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Bg_Blur_Disable_System_Adaptation_Test4)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBackgroundFilter(nullptr);
    testNodeBackGround->SetBgBlurDisableSystemAdaptation(true);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Bg_Blur_Disable_System_Adaptation_Test5)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    MaterialParam materialParam = { 50, 0.8, 0.8, Color(0x2dff0000), false };
    std::shared_ptr<RSMaterialFilter> matFilter =
        std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::PRE_DEFINED);
    testNodeBackGround->SetBackgroundFilter(matFilter);
    testNodeBackGround->SetBgBlurDisableSystemAdaptation(true);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundBlurTest, EFFECT_TEST, Set_Bg_Blur_Disable_System_Adaptation_Test6)
{
    auto sizeX = screenWidth;
    auto sizeY = screenHeight;
    int x = 0;
    int y = 0;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    std::shared_ptr<RSBlurFilter> blurFilter = std::make_shared<RSBlurFilter>(30, 30, false);
    testNodeBackGround->SetBackgroundFilter(blurFilter);
    testNodeBackGround->SetBgBlurDisableSystemAdaptation(true);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

} // namespace OHOS::Rosen
