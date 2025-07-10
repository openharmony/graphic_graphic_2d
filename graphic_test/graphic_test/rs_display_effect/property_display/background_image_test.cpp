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
    const int screenWidth = 1200;
    const int screenHeight = 2000;

public:
    // called before each tests
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }
};

/* SetBgImage：Pure_Color, AllBlack */
GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, Background_Pure_Color_BackgroundImage_Test1)
{
    auto testNodeBackGround = SetUpNodeBgImage("/data/local/tmp/Images/AllBlack.jpg", { 500, 500, 300, 300 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

/* SetBgImage：Pure_Color, AllWhite */
GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, Background_Pure_Color_BackgroundImage_Test2)
{
    auto testNodeBackGround = SetUpNodeBgImage("/data/local/tmp/Images/AllWhite.jpg", { 100, 100, 500, 500 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

/* SetBgImage：Images1 */
GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, Background_BackgroundImage_Test1)
{
    auto testNodeBackGround = SetUpNodeBgImage("/data/local/tmp/Images/475x327.jpg", { 100, 100, 500, 500 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

/* SetBgImage：Images2 */
GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, Background_BackgroundImage_Test2)
{
    auto testNodeBackGround = SetUpNodeBgImage("/data/local/tmp/Images/450x311.jpg", { 100, 100, 500, 500 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

/* SetBgImage：Images3 */
GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, Background_BackgroundImage_Test3)
{
    auto testNodeBackGround = SetUpNodeBgImage("/data/local/tmp/Images/940x650.jpg", { 100, 100, 500, 500 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

/* SetBgImage：Images4 */
GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, Background_BackgroundImage_Test4)
{
    auto testNodeBackGround = SetUpNodeBgImage("/data/local/tmp/Images/3200x2000.jpg", { 100, 100, 500, 500 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

/* SetBgImageInnerRect：abnormal1 */
GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, Background_BackgroundImage_InnerRect_Test1)
{
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { 100, 100, 500, 500 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBgImageInnerRect({ 100, 100, -200, -200 });
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

/* SetBgImageInnerRect：abnormal2 */
GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, Background_BackgroundImage_InnerRect_Test2)
{
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { 100, 100, 500, 500 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBgImageInnerRect({ 100, 100, 600, 600 });
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

/* SetBgImageInnerRect：normal1 */
GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, Background_BackgroundImage_InnerRect_Test3)
{
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { 100, 100, 500, 500 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBgImageInnerRect({ 100, 100, 100, 100 });
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

/* SetBgImageInnerRect：normal2 */
GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, Background_BackgroundImage_InnerRect_Test4)
{
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { 100, 100, 500, 500 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBgImageInnerRect({ 100, 100, 500, 500 });
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

/* SetBgImageInnerRect：random */
GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, Background_BackgroundImage_InnerRect_Test5)
{
    int columnCount = 2;
    int rowCount = 3;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<Vector2f> positionList = {{-200, -200}, {20, 20}, {100, 100}, {300, 300}, {500, 500}};
    for (int i = 0; i < positionList.size(); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        testNodeBackGround->SetBgImageInnerRect({x + positionList[i][0], y + positionList[i][1], sizeX, sizeY});
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

/* SetBgImageWidth: abnormal1 */
GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, Background_BackgroundImage_Width_Test1)
{
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { 100, 100, 500, 500 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBgImageWidth(-100);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

/* SetBgImageWidth: abnormal2 */
GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, Background_BackgroundImage_Width_Test2)
{
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { 100, 100, 500, 500 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBgImageWidth(0);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

/* SetBgImageWidth: abnormal3 */
GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, Background_BackgroundImage_Width_Test3)
{
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { 100, 100, 500, 500 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBgImageWidth(600);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

/* SetBgImageWidth: normal1 */
GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, Background_BackgroundImage_Width_Test4)
{
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { 100, 100, 500, 500 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBgImageWidth(200);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

/* SetBgImageWidth: normal2 */
GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, Background_BackgroundImage_Width_Test5)
{
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { 100, 100, 500, 500 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBgImageWidth(500);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

/* SetBgImageHeight: abnormal1 */
GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, Background_BackgroundImage_Height_Test1)
{
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { 100, 100, 500, 500 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBgImageHeight(-100);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

/* SetBgImageHeight: abnormal2 */
GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, Background_BackgroundImage_Height_Test2)
{
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { 100, 100, 500, 500 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBgImageHeight(0);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

/* SetBgImageHeight: abnormal3 */
GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, Background_BackgroundImage_Height_Test3)
{
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { 100, 100, 500, 500 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBgImageHeight(600);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

/* SetBgImageHeight: normal1 */
GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, Background_BackgroundImage_Height_Test4)
{
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { 100, 100, 500, 500 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBgImageHeight(200);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

/* SetBgImageSize: normal2 */
GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, Background_BackgroundImage_Height_Test5)
{
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { 100, 100, 500, 500 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBgImageHeight(500);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

/* SetBgImageSize: abnormal1 */
GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, Background_BackgroundImage_Size_Test1)
{
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { 100, 100, 500, 500 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBgImageSize(-200, -200);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

/* SetBgImageSize: normal */
GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, Background_BackgroundImage_Size_Test2)
{
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { 100, 100, 500, 500 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBgImageSize(200, 200);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

/* SetBgImagePositionX: abnormal1 */
GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, Background_BackgroundImage_PositionX_Test1)
{
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { 100, 100, 500, 500 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBgImagePositionX(-100);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

/* SetBgImagePositionX: normal1 */
GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, Background_BackgroundImage_PositionX_Test2)
{
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { 100, 100, 500, 500 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBgImagePositionX(0);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

/* SetBgImagePositionY: abnormal1 */
GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, Background_BackgroundImage_PositionY_Test1)
{
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { 100, 100, 500, 500 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBgImagePositionY(-100);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

/* SetBgImagePositionY: normal1 */
GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, Background_BackgroundImage_PositionY_Test2)
{
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { 100, 100, 500, 500 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBgImagePositionY(0);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

/* SetBgImagePosition: abnormal1 */
GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, Background_BackgroundImage_Position_Test1)
{
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { 100, 100, 500, 500 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBgImagePosition(-200, -200);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

/* SetBgImagePosition: normal1 */
GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, Background_BackgroundImage_Position_Test2)
{
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { 100, 100, 500, 500 });
    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBgImagePosition(200, 200);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

/* SetBgImagePosition、SetBgImageSize： case1 */
GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, Background_BackgroundImage_Comp_Test1)
{
    int columnCount = 2;
    int rowCount = 2;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    int x = (0 % columnCount) * sizeX;
    int y = (0 / columnCount) * sizeY;
    int borderWidth = sizeX - 10;
    int borderHeight = sizeY - 10;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, borderWidth, borderHeight });

    testNodeBackGround->SetBgImagePosition(20, 20);
    testNodeBackGround->SetBgImageSize(borderWidth * 0.8, borderHeight * 0.8);
    testNodeBackGround->SetCornerRadius(sizeX * 0.2);

    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));

    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

/* SetBgImagePosition、SetBgImageSize： case2 */
GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, Background_BackgroundImage_Comp_Test2)
{
    int columnCount = 2;
    int rowCount = 2;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    int x = (3 % columnCount) * sizeX;
    int y = (3 / columnCount) * sizeY;
    int borderWidth = sizeX - 10;
    int borderHeight = sizeY - 10;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, borderWidth, borderHeight });

    testNodeBackGround->SetBgImagePosition(20, borderHeight * 0.3);
    testNodeBackGround->SetBgImageSize(borderWidth * 0.8, borderHeight * 0.8);

    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));

    testNodeBackGround->SetCornerRadius(sizeX * 0.2);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, Background_BackgroundImage_Test)
{
    int columnCount = 2;
    int rowCount = 2;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<std::string> pathList = { "/data/local/tmp/Images/475x327.jpg", "/data/local/tmp/Images/450x311.jpg",
        "/data/local/tmp/Images/940x650.jpg", "/data/local/tmp/Images/3200x2000.jpg" };

    for (int i = 0; i < pathList.size(); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround = SetUpNodeBgImage(pathList[i], { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, Background_BackgroundImage_InnerRect_Test)
{
    int columnCount = 2;
    int rowCount = 3;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<Vector2f> sizeList = {  { -200, -200 }, { sizeX - 10, sizeY - 10 }, { sizeX - 100, sizeY - 100 },
        { 445, 445 }, { 500, 500 } };

    for (int i = 0; i < sizeList.size(); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        testNodeBackGround->SetBgImageInnerRect({ x, y, sizeList[i][0], sizeList[i][1] });
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, Background_BackgroundImage_Size_Test)
{
    std::vector<float> factorList = { -0.5, 0, 0.233, 0.5, 2 };

    int columnCount = 2;
    int rowCount = factorList.size();
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;


    for (int i = 0; i < rowCount; i++) {
        int x = 0;
        int y = i * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        testNodeBackGround->SetBgImageWidth((sizeX - 10) * factorList[i]);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }

    for (int i = 0; i < rowCount; i++) {
        int x = sizeX;
        int y = i * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        testNodeBackGround->SetBgImageHeight((sizeY - 10) * factorList[i]);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, Background_BackgroundImage_Position_Test)
{
    std::vector<float> factorList = { -0.5, 0, 0.233, 0.5, 2 };

    int columnCount = 2;
    int rowCount = factorList.size();
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    for (int i = 0; i < rowCount; i++) {
        int x = 0;
        int y = i * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        testNodeBackGround->SetBgImagePositionX((sizeX - 10) * factorList[i]);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }

    for (int i = 0; i < rowCount; i++) {
        int x = sizeX;
        int y = i * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        testNodeBackGround->SetBgImagePositionY((sizeY - 10) * factorList[i]);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, Background_BackgroundImage_Corner_Radius_Test)
{
    int columnCount = 2;
    int rowCount = 2;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    int x = (0 % columnCount) * sizeX;
    int y = (0 / columnCount) * sizeY;
    int borderWidth = sizeX - 10;
    int borderHeight = sizeY - 10;
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, borderWidth, borderHeight });

    testNodeBackGround->SetBgImagePosition(20, 20);
    testNodeBackGround->SetBgImageWidth(borderWidth * 0.8);
    testNodeBackGround->SetBgImageHeight(borderHeight * 0.8);
    testNodeBackGround->SetCornerRadius(sizeX * 0.2);

    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));

    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);

    x = (3 % columnCount) * sizeX;
    y = (3 / columnCount) * sizeY;
    testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, borderWidth, borderHeight });

    testNodeBackGround->SetBgImagePosition(20, borderHeight * 0.3);
    testNodeBackGround->SetBgImageWidth(borderWidth * 0.8);
    testNodeBackGround->SetBgImageHeight(borderHeight * 0.8);

    testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));

    testNodeBackGround->SetCornerRadius(sizeX * 0.2);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, Background_BackgroundImage_Alpha_Test)
{
    int columnCount = 2;
    int rowCount = 2;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<float> alphaList = { 0, 0.5, 1, 200 };

    for (int i = 0; i < alphaList.size(); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        testNodeBackGround->SetAlpha(alphaList[i]);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, Background_Pure_Color_BackgroundImage_Test)
{
    int columnCount = 1;
    int rowCount = 2;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<std::string> pathList = { "/data/local/tmp/Images/AllBlack.jpg",
        "/data/local/tmp/Images/AllWhite.jpg" };

    for (int i = 0; i < pathList.size(); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround = SetUpNodeBgImage(pathList[i], { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}


GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, Background_BackgroundImage_InnerRect_Test_1)
{
    int columnCount = 2;
    int rowCount = 3;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<Vector2f> positionList = {{-200, -200}, {20, 20}, {100, 100}, {300, 300}, {500, 500}};
    for (int i = 0; i < positionList.size(); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        testNodeBackGround->SetBgImageInnerRect({x + positionList[i][0], y + positionList[i][1], sizeX, sizeY});
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}
} // namespace OHOS::Rosen