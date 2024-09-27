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

#include "rs_graphic_test.h"
#include "rs_graphic_test_img.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class BackgroundTest : public RSGraphicTest {
private:
    int screenWidth = 1260;
    int screenHeight = 2720;

public:
    // called before each tests
    void BeforeEach() override
    {
        SetScreenSurfaceBounds({ 0, 0, screenWidth, screenHeight });
    }
};

GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, Background_Blur_Radius_Test)
{
    int columnCount = 3;
    int rowCount = 3;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<float> radiusList = { -1, 0, 10, 20, 30, 50, 100, 150 };
    for (int i = 0; i < radiusList.size(); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));

        testNodeBackGround->SetBackgroundBlurRadius(radiusList[i]);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, Background_Blur_Radius_XY_Test)
{
    int columnCount = 4;
    int rowCount = 4;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<float> radiusList = { -1, 0, 10, 20, 30, 50, 100, 200 };
    for (int i = 0; i < radiusList.size(); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        testNodeBackGround->SetBackgroundBlurRadiusX(radiusList[i]);
        testNodeBackGround->SetBackgroundBlurRadiusY(50);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }

    for (int i = 0; i < radiusList.size(); i++) {
        int x = ((i + radiusList.size()) % columnCount) * sizeX;
        int y = ((i + radiusList.size()) / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        testNodeBackGround->SetBackgroundBlurRadiusX(50);
        testNodeBackGround->SetBackgroundBlurRadiusY(radiusList[i]);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, Background_Blur_Saturation_Test)
{
    int columnCount = 2;
    int rowCount = 3;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<float> saturationList = { -1, 0, 0.5, 10, 200 };
    for (int i = 0; i < saturationList.size(); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        testNodeBackGround->SetBackgroundBlurSaturation(saturationList[i]);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, BackGround_Blur_Brightness_Test)
{
    int columnCount = 2;
    int rowCount = 3;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<float> brightnessList = { -1, 0, 0.5, 10, 200 };
    for (int i = 0; i < brightnessList.size(); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        testNodeBackGround->SetBackgroundBlurBrightness(brightnessList[i]);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, Background_Blur_Mask_Color_Test)
{
    int columnCount = 3;
    int rowCount = 4;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<Color> colorList = {
        Color(0xff000000), // RgbPalette::Black()
        Color(0xffffffff), // RgbPalette::White()
        Color(0x2dff0000), // RgbPalette::Red()
        Color(0x4d00ff00), // RgbPalette::Green()
        Color(0x6d0000ff), // RgbPalette::Blue()
        Color(0x8d00ffff), // RgbPalette::Cyan()
        Color(0xad444444), // RgbPalette::Gray()
        Color(0xcdff00ff), // RgbPalette::Magenta()
        Color(0x00000000), // RgbPalette::Transparent()
        Color(0x10000000), // RgbPalette::Transparent()
        Color(0x20000000), // RgbPalette::Transparent()
        Color(0x30000000), // RgbPalette::Transparent()
    };

    std::vector<int> radiusList = { 10, 20, 30, 40, 50, 60, 70, 80, 90, -100, -10, 0 };
    std::vector<float> saturationList = { 0.2, 0.4, 0.6, 0.8, 1.0, 1.2, 1.4, 1.6, 1.8, 0.6, 1.0, 1.4 };
    std::vector<float> brightnessList = { 0.2, 0.4, 0.6, 0.8, 1.0, 1.2, 1.4, 1.6, 1.8, 0.6, 1.0, 1.4 };

    for (int i = 0; i < colorList.size(); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        testNodeBackGround->SetBackgroundBlurRadius(radiusList[i]);
        testNodeBackGround->SetBackgroundBlurSaturation(saturationList[i]);
        testNodeBackGround->SetBackgroundBlurBrightness(brightnessList[i]);
        testNodeBackGround->SetBackgroundBlurMaskColor(colorList[i]);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, Background_Blur_Color_Mode_Test)
{
    int columnCount = 3;
    int rowCount = 3;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<int> colorModeList = { -100, -10, 0, 1, 2, 3, 10, 100, 10000 };

    for (int i = 0; i < colorModeList.size(); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        testNodeBackGround->SetBackgroundBlurRadius(30);
        testNodeBackGround->SetBackgroundBlurSaturation(1);
        testNodeBackGround->SetBackgroundBlurBrightness(0.5);
        testNodeBackGround->SetBackgroundBlurColorMode(colorModeList[i]);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, SystemBar_Effect_Test)
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
} // namespace OHOS::Rosen