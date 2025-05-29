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

#include "property/rs_properties.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class AppearanceTest : public RSGraphicTest {
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

GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Visible_Test)
{
    int columnCount = 1;
    int rowCount = 2;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<bool> visibleList = { true, false };

    for (auto i = 0; i < 2; i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Green()));
        testNodeBackGround->SetVisible(visibleList[i]);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_PixelStretch_Test)
{
    int columnCount = 2;
    int rowCount = 3;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<Vector4f> pixelStretchList = { { 0, 0, 0, 0 }, { 0, 20, 0, 0 }, { 0, 0, 40, 0 }, { 0, 0, 0, 60 },
        { 30, 30, 30, 30 } };
    std::vector<Drawing::TileMode> modeList = { Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP,
        Drawing::TileMode::REPEAT, Drawing::TileMode::MIRROR, Drawing::TileMode::DECAL };

    for (auto i = 0; i < 5; i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x + 50, y + 50, 500, 300 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Green()));
        testNodeBackGround->SetPixelStretch(pixelStretchList[i], modeList[i]);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_PixelStretchPercent_Test)
{
    int columnCount = 2;
    int rowCount = 3;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<Vector4f> pixelStretchList = { { 0, 0, 0, 0 }, { 0, 0.10, 0, 0 }, { 0, 0, 0.5, 0 }, { 0, 0, 0, 1.0 },
        { 0.1, 0.2, 0.3, 0.4 } };
    Vector4f midVal = { 0.5, 0.5, 0.5, 0.5 };
    std::vector<Drawing::TileMode> modeList = { Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP,
        Drawing::TileMode::REPEAT, Drawing::TileMode::MIRROR, Drawing::TileMode::DECAL };

    for (auto i = 0; i < 5; i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x + 50, y + 50, 500, 300 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Green()));
        // Set PixelStretch to a middle value, DTS2024092542149
        testNodeBackGround->SetPixelStretchPercent(midVal, modeList[i]);
        testNodeBackGround->SetPixelStretchPercent(pixelStretchList[i], modeList[i]);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_MotionBlur_Test)
{
    int columnCount = 2;
    int rowCount = 2;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<float> radiusList = { 0, 10, 0, 10 };
    std::vector<Vector2f> modeList = { { 0, 0 }, { 0, 0 }, { 500, 500 }, { 500, 500 } };

    for (auto i = 0; i < 4; i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Green()));
        testNodeBackGround->SetMotionBlurPara(radiusList[i], modeList[i]);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_MagnifierParams_Test)
{
    int columnCount = 1;
    int rowCount = 2;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    int x = (0 % columnCount) * sizeX;
    int y = (0 / columnCount) * sizeY;
    std::string testImagePath = "/data/local/tmp/Images/3200x2000.jpg";
    auto testNodeBackGround1 = SetUpNodeBgImage(testImagePath, { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround1->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround1->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround1->SetBorderColor(Vector4<Color>(RgbPalette::Green()));
    auto magnifierParams1 = std::make_shared<Rosen::RSMagnifierParams>();
    magnifierParams1->factor_ = 3;
    magnifierParams1->width_ = 200;
    magnifierParams1->height_ = 200;
    magnifierParams1->cornerRadius_ = 20;
    magnifierParams1->borderWidth_ = 5;
    magnifierParams1->offsetX_ = 50;
    magnifierParams1->offsetY_ = 50;
    testNodeBackGround1->SetMagnifierParams(magnifierParams1);
    GetRootNode()->AddChild(testNodeBackGround1);
    RegisterNode(testNodeBackGround1);

    x = (1 % columnCount) * sizeX;
    y = (1 / columnCount) * sizeY;
    auto testNodeBackGround2 = SetUpNodeBgImage(testImagePath, { x, y, sizeX - 10, sizeY - 10 });
    testNodeBackGround2->SetBorderStyle(0, 0, 0, 0);
    testNodeBackGround2->SetBorderWidth(5, 5, 5, 5);
    testNodeBackGround2->SetBorderColor(Vector4<Color>(RgbPalette::Green()));
    auto magnifierParams2 = std::make_shared<Rosen::RSMagnifierParams>();
    magnifierParams2->factor_ = 10;
    magnifierParams2->width_ = 250;
    magnifierParams2->height_ = 250;
    magnifierParams2->cornerRadius_ = 50;
    magnifierParams2->borderWidth_ = 15;
    magnifierParams2->offsetX_ = 30;
    magnifierParams2->offsetY_ = 60;
    testNodeBackGround2->SetMagnifierParams(magnifierParams2);
    GetRootNode()->AddChild(testNodeBackGround2);
    RegisterNode(testNodeBackGround2);
}

GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_FrameGravity_Test)
{
    int columnCount = 4;
    int rowCount = 4;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    for (int i = static_cast<int>(Gravity::CENTER); i <= static_cast<int>(Gravity::RESIZE_ASPECT_FILL_BOTTOM_RIGHT);
         i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Green()));
        testNodeBackGround->SetFrameGravity(static_cast<Gravity>(i));
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

} // namespace OHOS::Rosen