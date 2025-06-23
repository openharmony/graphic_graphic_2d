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

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Color_Blend_Mode_1)
{
    int columnCount = 6;
    int rowCount = 5;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    for (int i = static_cast<int>(RSColorBlendMode::NONE); i <= static_cast<int>(RSColorBlendMode::MAX); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Green()));

        auto childNode1 =
            SetUpNodeBgImage("/data/local/tmp/Images/475x327.jpg", { sizeX / 4, sizeY / 4, sizeX / 3, sizeY / 3 });
        childNode1->SetBackgroundBlurRadius(30);
        childNode1->SetBorderStyle(0, 0, 0, 0);
        childNode1->SetBorderWidth(5, 5, 5, 5);
        childNode1->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        childNode1->SetAlpha(0.3);
        RegisterNode(childNode1);
        auto childNode2 = RSCanvasNode::Create();
        childNode2->SetBounds({ sizeX / 2, sizeY / 2, sizeX / 3, sizeY / 3 });
        childNode2->SetBackgroundColor(0xff00ff00);
        childNode2->SetBackgroundBlurRadius(30);
        childNode2->SetBorderStyle(0, 0, 0, 0);
        childNode2->SetBorderWidth(5, 5, 5, 5);
        childNode2->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        childNode2->SetAlpha(0.5);
        RegisterNode(childNode2);
        auto childNode3 = RSCanvasNode::Create();
        childNode3->SetBounds({ sizeX * 3 / 4, sizeY * 3 / 4, sizeX / 3, sizeY / 3 });
        childNode3->SetBackgroundColor(0xff00ffff);
        childNode3->SetBackgroundBlurRadius(30);
        childNode3->SetBorderStyle(0, 0, 0, 0);
        childNode3->SetBorderWidth(5, 5, 5, 5);
        childNode3->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        childNode3->SetAlpha(0.7);
        RegisterNode(childNode3);
        testNodeBackGround->SetBounds({ x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->AddChild(childNode1);
        testNodeBackGround->AddChild(childNode2);
        testNodeBackGround->AddChild(childNode3);

        testNodeBackGround->SetColorBlendMode(static_cast<RSColorBlendMode>(i));
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Color_Blend_Mode_2)
{
    int columnCount = 6;
    int rowCount = 5;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    for (int i = static_cast<int>(RSColorBlendMode::NONE); i <= static_cast<int>(RSColorBlendMode::HUE); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Green()));

        auto childNode1 =
            SetUpNodeBgImage("/data/local/tmp/Images/475x327.jpg", { sizeX / 4, sizeY / 4, sizeX / 3, sizeY / 3 });
        childNode1->SetBackgroundBlurRadius(30);
        childNode1->SetBorderStyle(0, 0, 0, 0);
        childNode1->SetBorderWidth(5, 5, 5, 5);
        childNode1->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        childNode1->SetAlpha(0.3);
        RegisterNode(childNode1);
        auto childNode2 = RSCanvasNode::Create();
        childNode2->SetBounds({ sizeX / 2, sizeY / 2, sizeX / 3, sizeY / 3 });
        childNode2->SetBackgroundColor(0xff00ff00);
        childNode2->SetBackgroundBlurRadius(30);
        childNode2->SetBorderStyle(0, 0, 0, 0);
        childNode2->SetBorderWidth(5, 5, 5, 5);
        childNode2->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        childNode2->SetAlpha(0.5);
        RegisterNode(childNode2);
        auto childNode3 = RSCanvasNode::Create();
        childNode3->SetBounds({ sizeX * 3 / 4, sizeY * 3 / 4, sizeX / 3, sizeY / 3 });
        childNode3->SetBackgroundColor(0xff00ffff);
        childNode3->SetBackgroundBlurRadius(30);
        childNode3->SetBorderStyle(0, 0, 0, 0);
        childNode3->SetBorderWidth(5, 5, 5, 5);
        childNode3->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        childNode3->SetAlpha(0.7);
        RegisterNode(childNode3);
        testNodeBackGround->SetBounds({ x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->AddChild(childNode1);
        testNodeBackGround->AddChild(childNode2);
        testNodeBackGround->AddChild(childNode3);

        testNodeBackGround->SetColorBlendMode(static_cast<RSColorBlendMode>(i));
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Color_Blend_Mode_3)
{
    int columnCount = 2;
    int rowCount = 2;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    for (int i = static_cast<int>(RSColorBlendMode::NONE); i <= static_cast<int>(RSColorBlendMode::MAX); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Green()));

        auto childNode1 =
            SetUpNodeBgImage("/data/local/tmp/Images/475x327.jpg", { sizeX / 4, sizeY / 4, sizeX / 3, sizeY / 3 });
        childNode1->SetBackgroundBlurRadius(30);
        childNode1->SetBorderStyle(0, 0, 0, 0);
        childNode1->SetBorderWidth(5, 5, 5, 5);
        childNode1->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        childNode1->SetAlpha(0.3);
        RegisterNode(childNode1);
        auto childNode2 = RSCanvasNode::Create();
        childNode2->SetBounds({ sizeX / 2, sizeY / 2, sizeX / 3, sizeY / 3 });
        childNode2->SetBackgroundColor(0xff00ff00);
        childNode2->SetBackgroundBlurRadius(30);
        childNode2->SetBorderStyle(0, 0, 0, 0);
        childNode2->SetBorderWidth(5, 5, 5, 5);
        childNode2->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        childNode2->SetAlpha(0.5);
        RegisterNode(childNode2);
        auto childNode3 = RSCanvasNode::Create();
        childNode3->SetBounds({ sizeX * 3 / 4, sizeY * 3 / 4, sizeX / 3, sizeY / 3 });
        childNode3->SetBackgroundColor(0xff00ffff);
        childNode3->SetBackgroundBlurRadius(30);
        childNode3->SetBorderStyle(0, 0, 0, 0);
        childNode3->SetBorderWidth(5, 5, 5, 5);
        childNode3->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        childNode3->SetAlpha(0.7);
        RegisterNode(childNode3);
        testNodeBackGround->SetBounds({ x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->AddChild(childNode1);
        testNodeBackGround->AddChild(childNode2);
        testNodeBackGround->AddChild(childNode3);

        testNodeBackGround->SetColorBlendMode(static_cast<RSColorBlendMode>(i));
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Color_Blend_Mode_4)
{
    int columnCount = 6;
    int rowCount = 5;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    for (int i = static_cast<int>(RSColorBlendMode::NONE); i <= static_cast<int>(RSColorBlendMode::MAX); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Black()));

        auto childNode1 =
            SetUpNodeBgImage("/data/local/tmp/Images/475x327.jpg", { sizeX / 4, sizeY / 4, sizeX / 3, sizeY / 3 });
        childNode1->SetBackgroundBlurRadius(30);
        childNode1->SetBorderStyle(0, 0, 0, 0);
        childNode1->SetBorderWidth(5, 5, 5, 5);
        childNode1->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        childNode1->SetAlpha(0.3);
        RegisterNode(childNode1);
        auto childNode2 = RSCanvasNode::Create();
        childNode2->SetBounds({ sizeX / 2, sizeY / 2, sizeX / 3, sizeY / 3 });
        childNode2->SetBackgroundColor(0xff00ff00);
        childNode2->SetBackgroundBlurRadius(30);
        childNode2->SetBorderStyle(0, 0, 0, 0);
        childNode2->SetBorderWidth(5, 5, 5, 5);
        childNode2->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        childNode2->SetAlpha(0.5);
        RegisterNode(childNode2);
        auto childNode3 = RSCanvasNode::Create();
        childNode3->SetBounds({ sizeX * 3 / 4, sizeY * 3 / 4, sizeX / 3, sizeY / 3 });
        childNode3->SetBackgroundColor(0xff00ffff);
        childNode3->SetBackgroundBlurRadius(30);
        childNode3->SetBorderStyle(0, 0, 0, 0);
        childNode3->SetBorderWidth(5, 5, 5, 5);
        childNode3->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        childNode3->SetAlpha(0.7);
        RegisterNode(childNode3);
        testNodeBackGround->SetBounds({ x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->AddChild(childNode1);
        testNodeBackGround->AddChild(childNode2);
        testNodeBackGround->AddChild(childNode3);

        testNodeBackGround->SetColorBlendMode(static_cast<RSColorBlendMode>(i));
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Color_Blend_Mode_5)
{
    int columnCount = 6;
    int rowCount = 5;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    for (int i = static_cast<int>(RSColorBlendMode::NONE); i <= static_cast<int>(RSColorBlendMode::MAX); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBorderStyle(5, 5, 5, 5);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Green()));

        auto childNode1 =
            SetUpNodeBgImage("/data/local/tmp/Images/475x327.jpg", { sizeX / 4, sizeY / 4, sizeX / 3, sizeY / 3 });
        childNode1->SetBackgroundBlurRadius(30);
        childNode1->SetBorderStyle(0, 0, 0, 0);
        childNode1->SetBorderWidth(5, 5, 5, 5);
        childNode1->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        childNode1->SetAlpha(0.3);
        RegisterNode(childNode1);
        auto childNode2 = RSCanvasNode::Create();
        childNode2->SetBounds({ sizeX / 2, sizeY / 2, sizeX / 3, sizeY / 3 });
        childNode2->SetBackgroundColor(0xff00ff00);
        childNode2->SetBackgroundBlurRadius(30);
        childNode2->SetBorderStyle(0, 0, 0, 0);
        childNode2->SetBorderWidth(5, 5, 5, 5);
        childNode2->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        childNode2->SetAlpha(0.5);
        RegisterNode(childNode2);
        auto childNode3 = RSCanvasNode::Create();
        childNode3->SetBounds({ sizeX * 3 / 4, sizeY * 3 / 4, sizeX / 3, sizeY / 3 });
        childNode3->SetBackgroundColor(0xff00ffff);
        childNode3->SetBackgroundBlurRadius(30);
        childNode3->SetBorderStyle(0, 0, 0, 0);
        childNode3->SetBorderWidth(5, 5, 5, 5);
        childNode3->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        childNode3->SetAlpha(0.7);
        RegisterNode(childNode3);
        testNodeBackGround->SetBounds({ x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->AddChild(childNode1);
        testNodeBackGround->AddChild(childNode2);
        testNodeBackGround->AddChild(childNode3);

        testNodeBackGround->SetColorBlendMode(static_cast<RSColorBlendMode>(i));
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Color_Blend_Mode_6)
{
    int columnCount = 6;
    int rowCount = 5;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    for (int i = static_cast<int>(RSColorBlendMode::NONE); i <= static_cast<int>(RSColorBlendMode::MAX); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Green()));

        auto childNode1 =
            SetUpNodeBgImage("/data/local/tmp/Images/475x327.jpg", { sizeX / 4, sizeY / 4, sizeX / 3, sizeY / 3 });
        childNode1->SetBackgroundBlurRadius(0);
        childNode1->SetBorderStyle(0, 0, 0, 0);
        childNode1->SetBorderWidth(5, 5, 5, 5);
        childNode1->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        childNode1->SetAlpha(0.3);
        RegisterNode(childNode1);
        auto childNode2 = RSCanvasNode::Create();
        childNode2->SetBounds({ sizeX / 2, sizeY / 2, sizeX / 3, sizeY / 3 });
        childNode2->SetBackgroundColor(0xff00ff00);
        childNode2->SetBackgroundBlurRadius(30);
        childNode2->SetBorderStyle(0, 0, 0, 0);
        childNode2->SetBorderWidth(5, 5, 5, 5);
        childNode2->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        childNode2->SetAlpha(0.5);
        RegisterNode(childNode2);
        auto childNode3 = RSCanvasNode::Create();
        childNode3->SetBounds({ sizeX * 3 / 4, sizeY * 3 / 4, sizeX / 3, sizeY / 3 });
        childNode3->SetBackgroundColor(0xff00ffff);
        childNode3->SetBackgroundBlurRadius(30);
        childNode3->SetBorderStyle(0, 0, 0, 0);
        childNode3->SetBorderWidth(5, 5, 5, 5);
        childNode3->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        childNode3->SetAlpha(0.7);
        RegisterNode(childNode3);
        testNodeBackGround->SetBounds({ x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->AddChild(childNode1);
        testNodeBackGround->AddChild(childNode2);
        testNodeBackGround->AddChild(childNode3);

        testNodeBackGround->SetColorBlendMode(static_cast<RSColorBlendMode>(i));
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Color_Blend_Mode_7)
{
    int columnCount = 6;
    int rowCount = 5;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    for (int i = static_cast<int>(RSColorBlendMode::NONE); i <= static_cast<int>(RSColorBlendMode::MAX); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Green()));

        auto childNode1 =
            SetUpNodeBgImage("/data/local/tmp/Images/475x327.jpg", { sizeX / 4, sizeY / 4, sizeX / 3, sizeY / 3 });
        childNode1->SetBackgroundBlurRadius(30);
        childNode1->SetBorderStyle(0, 0, 0, 0);
        childNode1->SetBorderWidth(5, 5, 5, 5);
        childNode1->SetBorderColor(Vector4<Color>(RgbPalette::White()));
        childNode1->SetAlpha(0.3);
        RegisterNode(childNode1);
        auto childNode2 = RSCanvasNode::Create();
        childNode2->SetBounds({ sizeX / 2, sizeY / 2, sizeX / 3, sizeY / 3 });
        childNode2->SetBackgroundColor(0xff00ff00);
        childNode2->SetBackgroundBlurRadius(30);
        childNode2->SetBorderStyle(0, 0, 0, 0);
        childNode2->SetBorderWidth(5, 5, 5, 5);
        childNode2->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        childNode2->SetAlpha(0.5);
        RegisterNode(childNode2);
        auto childNode3 = RSCanvasNode::Create();
        childNode3->SetBounds({ sizeX * 3 / 4, sizeY * 3 / 4, sizeX / 3, sizeY / 3 });
        childNode3->SetBackgroundColor(0xff00ffff);
        childNode3->SetBackgroundBlurRadius(30);
        childNode3->SetBorderStyle(0, 0, 0, 0);
        childNode3->SetBorderWidth(5, 5, 5, 5);
        childNode3->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        childNode3->SetAlpha(0.7);
        RegisterNode(childNode3);
        testNodeBackGround->SetBounds({ x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->AddChild(childNode1);
        testNodeBackGround->AddChild(childNode2);
        testNodeBackGround->AddChild(childNode3);

        testNodeBackGround->SetColorBlendMode(static_cast<RSColorBlendMode>(i));
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Color_Blend_Mode_8)
{
    int columnCount = 6;
    int rowCount = 5;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    for (int i = static_cast<int>(RSColorBlendMode::NONE); i <= static_cast<int>(RSColorBlendMode::MAX); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Green()));

        auto childNode1 =
            SetUpNodeBgImage("/data/local/tmp/Images/475x327.jpg", { sizeX / 4, sizeY / 4, sizeX / 3, sizeY / 3 });
        childNode1->SetBackgroundBlurRadius(30);
        childNode1->SetBorderStyle(0, 0, 0, 0);
        childNode1->SetBorderWidth(5, 5, 5, 5);
        childNode1->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        childNode1->SetAlpha(0.3);
        RegisterNode(childNode1);
        auto childNode2 = RSCanvasNode::Create();
        childNode2->SetBounds({ sizeX / 2, sizeY / 2, sizeX / 3, sizeY / 3 });
        childNode2->SetBackgroundColor(0xff00ff00);
        childNode2->SetBackgroundBlurRadius(30);
        childNode2->SetBorderStyle(0, 0, 0, 0);
        childNode2->SetBorderWidth(5, 5, 5, 5);
        childNode2->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        childNode2->SetAlpha(0.5);
        RegisterNode(childNode2);
        auto childNode3 = RSCanvasNode::Create();
        childNode3->SetBounds({ sizeX * 3 / 4, sizeY * 3 / 4, sizeX / 3, sizeY / 3 });
        childNode3->SetBackgroundColor(0xff00ffff);
        childNode3->SetBackgroundBlurRadius(0);
        childNode3->SetBorderStyle(0, 0, 0, 0);
        childNode3->SetBorderWidth(5, 5, 5, 5);
        childNode3->SetBorderColor(Vector4<Color>(RgbPalette::Black()));
        childNode3->SetAlpha(0.7);
        RegisterNode(childNode3);
        testNodeBackGround->SetBounds({ x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->AddChild(childNode1);
        testNodeBackGround->AddChild(childNode2);
        testNodeBackGround->AddChild(childNode3);

        testNodeBackGround->SetColorBlendMode(static_cast<RSColorBlendMode>(i));
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Color_Blend_Mode_9)
{
    int columnCount = 6;
    int rowCount = 5;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    for (int i = static_cast<int>(RSColorBlendMode::NONE); i <= static_cast<int>(RSColorBlendMode::MAX); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Green()));

        auto childNode1 =
            SetUpNodeBgImage("/data/local/tmp/Images/475x327.jpg", { sizeX / 4, sizeY / 4, sizeX / 3, sizeY / 3 });
        childNode1->SetBackgroundBlurRadius(30);
        childNode1->SetBorderStyle(0, 0, 0, 0);
        childNode1->SetBorderWidth(5, 5, 5, 5);
        childNode1->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        childNode1->SetAlpha(0.3);
        RegisterNode(childNode1);
        auto childNode2 = RSCanvasNode::Create();
        childNode2->SetBounds({ sizeX / 2, sizeY / 2, sizeX / 3, sizeY / 3 });
        childNode2->SetBackgroundColor(0xff00ff00);
        childNode2->SetBackgroundBlurRadius(30);
        childNode2->SetBorderStyle(0, 0, 0, 0);
        childNode2->SetBorderWidth(5, 5, 5, 5);
        childNode2->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        childNode2->SetAlpha(0.5);
        RegisterNode(childNode2);
        auto childNode3 = RSCanvasNode::Create();
        childNode3->SetBounds({ sizeX * 3 / 4, sizeY * 3 / 4, sizeX / 3, sizeY / 3 });
        childNode3->SetBackgroundColor(0xff00ffff);
        childNode3->SetBackgroundBlurRadius(30);
        childNode3->SetBorderStyle(0, 0, 0, 0);
        childNode3->SetBorderWidth(5, 5, 5, 5);
        childNode3->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        childNode3->SetAlpha(0);
        RegisterNode(childNode3);
        testNodeBackGround->SetBounds({ x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->AddChild(childNode1);
        testNodeBackGround->AddChild(childNode2);
        testNodeBackGround->AddChild(childNode3);

        testNodeBackGround->SetColorBlendMode(static_cast<RSColorBlendMode>(i));
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Color_Blend_Mode_10)
{
    int columnCount = 6;
    int rowCount = 5;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    for (int i = static_cast<int>(RSColorBlendMode::NONE); i <= static_cast<int>(RSColorBlendMode::MAX); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Green()));

        auto childNode1 =
            SetUpNodeBgImage("/data/local/tmp/Images/475x327.jpg", { sizeX / 4, sizeY / 4, sizeX / 3, sizeY / 3 });
        childNode1->SetBackgroundBlurRadius(30);
        childNode1->SetBorderStyle(0, 0, 0, 0);
        childNode1->SetBorderWidth(5, 5, 5, 5);
        childNode1->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        childNode1->SetAlpha(0);
        RegisterNode(childNode1);
        auto childNode2 = RSCanvasNode::Create();
        childNode2->SetBounds({ sizeX / 2, sizeY / 2, sizeX / 3, sizeY / 3 });
        childNode2->SetBackgroundColor(0xff00ff00);
        childNode2->SetBackgroundBlurRadius(30);
        childNode2->SetBorderStyle(0, 0, 0, 0);
        childNode2->SetBorderWidth(5, 5, 5, 5);
        childNode2->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        childNode2->SetAlpha(0);
        RegisterNode(childNode2);
        auto childNode3 = RSCanvasNode::Create();
        childNode3->SetBounds({ sizeX * 3 / 4, sizeY * 3 / 4, sizeX / 3, sizeY / 3 });
        childNode3->SetBackgroundColor(0xff00ffff);
        childNode3->SetBackgroundBlurRadius(30);
        childNode3->SetBorderStyle(0, 0, 0, 0);
        childNode3->SetBorderWidth(5, 5, 5, 5);
        childNode3->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        childNode3->SetAlpha(0);
        RegisterNode(childNode3);
        testNodeBackGround->SetBounds({ x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->AddChild(childNode1);
        testNodeBackGround->AddChild(childNode2);
        testNodeBackGround->AddChild(childNode3);

        testNodeBackGround->SetColorBlendMode(static_cast<RSColorBlendMode>(i));
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Color_Blend_Mode_11)
{
    int columnCount = 6;
    int rowCount = 5;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    for (int i = static_cast<int>(RSColorBlendMode::NONE); i <= static_cast<int>(RSColorBlendMode::MAX); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Green()));

        auto childNode1 =
            SetUpNodeBgImage("/data/local/tmp/Images/475x327.jpg", { sizeX / 4, sizeY / 4, sizeX / 3, sizeY / 3 });
        childNode1->SetBackgroundBlurRadius(30);
        childNode1->SetBorderStyle(0, 0, 0, 0);
        childNode1->SetBorderWidth(5, 5, 5, 5);
        childNode1->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        childNode1->SetAlpha(1.0);
        RegisterNode(childNode1);
        auto childNode2 = RSCanvasNode::Create();
        childNode2->SetBounds({ sizeX / 2, sizeY / 2, sizeX / 3, sizeY / 3 });
        childNode2->SetBackgroundColor(0xff00ff00);
        childNode2->SetBackgroundBlurRadius(30);
        childNode2->SetBorderStyle(0, 0, 0, 0);
        childNode2->SetBorderWidth(5, 5, 5, 5);
        childNode2->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        childNode2->SetAlpha(1.0);
        RegisterNode(childNode2);
        auto childNode3 = RSCanvasNode::Create();
        childNode3->SetBounds({ sizeX * 3 / 4, sizeY * 3 / 4, sizeX / 3, sizeY / 3 });
        childNode3->SetBackgroundColor(0xff00ffff);
        childNode3->SetBackgroundBlurRadius(30);
        childNode3->SetBorderStyle(0, 0, 0, 0);
        childNode3->SetBorderWidth(5, 5, 5, 5);
        childNode3->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        childNode3->SetAlpha(1.0);
        RegisterNode(childNode3);
        testNodeBackGround->SetBounds({ x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->AddChild(childNode1);
        testNodeBackGround->AddChild(childNode2);
        testNodeBackGround->AddChild(childNode3);

        testNodeBackGround->SetColorBlendMode(static_cast<RSColorBlendMode>(i));
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Color_Blend_Mode_12)
{
    int columnCount = 6;
    int rowCount = 5;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    for (int i = static_cast<int>(RSColorBlendMode::NONE); i <= static_cast<int>(RSColorBlendMode::MAX); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Green()));

        auto childNode1 =
            SetUpNodeBgImage("/data/local/tmp/Images/475x327.jpg", { sizeX / 4, sizeY / 4, sizeX / 3, sizeY / 3 });
        childNode1->SetBackgroundBlurRadius(30);
        childNode1->SetBorderStyle(3, 3, 3, 3);
        childNode1->SetBorderWidth(5, 5, 5, 5);
        childNode1->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        childNode1->SetAlpha(0.3);
        RegisterNode(childNode1);
        auto childNode2 = RSCanvasNode::Create();
        childNode2->SetBounds({ sizeX / 2, sizeY / 2, sizeX / 3, sizeY / 3 });
        childNode2->SetBackgroundColor(0xff00ff00);
        childNode2->SetBackgroundBlurRadius(30);
        childNode2->SetBorderStyle(3, 3, 3, 3);
        childNode2->SetBorderWidth(5, 5, 5, 5);
        childNode2->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        childNode2->SetAlpha(0.5);
        RegisterNode(childNode2);
        auto childNode3 = RSCanvasNode::Create();
        childNode3->SetBounds({ sizeX * 3 / 4, sizeY * 3 / 4, sizeX / 3, sizeY / 3 });
        childNode3->SetBackgroundColor(0xff00ffff);
        childNode3->SetBackgroundBlurRadius(30);
        childNode3->SetBorderStyle(3, 3, 3, 3);
        childNode3->SetBorderWidth(5, 5, 5, 5);
        childNode3->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        childNode3->SetAlpha(0.7);
        RegisterNode(childNode3);
        testNodeBackGround->SetBounds({ x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->AddChild(childNode1);
        testNodeBackGround->AddChild(childNode2);
        testNodeBackGround->AddChild(childNode3);

        testNodeBackGround->SetColorBlendMode(static_cast<RSColorBlendMode>(i));
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Color_Blend_Mode_13)
{
    int columnCount = 10;
    int rowCount = 10;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    for (int i = static_cast<int>(RSColorBlendMode::NONE); i <= static_cast<int>(RSColorBlendMode::MAX); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Green()));

        auto childNode1 =
            SetUpNodeBgImage("/data/local/tmp/Images/475x327.jpg", { sizeX / 4, sizeY / 4, sizeX / 3, sizeY / 3 });
        childNode1->SetBackgroundBlurRadius(30);
        childNode1->SetBorderStyle(0, 0, 0, 0);
        childNode1->SetBorderWidth(5, 5, 5, 5);
        childNode1->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        childNode1->SetAlpha(0.3);
        RegisterNode(childNode1);
        auto childNode2 = RSCanvasNode::Create();
        childNode2->SetBounds({ sizeX / 2, sizeY / 2, sizeX / 3, sizeY / 3 });
        childNode2->SetBackgroundColor(0xff00ff00);
        childNode2->SetBackgroundBlurRadius(30);
        childNode2->SetBorderStyle(0, 0, 0, 0);
        childNode2->SetBorderWidth(5, 5, 5, 5);
        childNode2->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        childNode2->SetAlpha(0.5);
        RegisterNode(childNode2);
        auto childNode3 = RSCanvasNode::Create();
        childNode3->SetBounds({ sizeX * 3 / 4, sizeY * 3 / 4, sizeX / 3, sizeY / 3 });
        childNode3->SetBackgroundColor(0xff00ffff);
        childNode3->SetBackgroundBlurRadius(30);
        childNode3->SetBorderStyle(0, 0, 0, 0);
        childNode3->SetBorderWidth(5, 5, 5, 5);
        childNode3->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        childNode3->SetAlpha(0.7);
        RegisterNode(childNode3);
        testNodeBackGround->SetBounds({ x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->AddChild(childNode1);
        testNodeBackGround->AddChild(childNode2);
        testNodeBackGround->AddChild(childNode3);

        testNodeBackGround->SetColorBlendMode(static_cast<RSColorBlendMode>(i));
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Color_Blend_Mode_14)
{
    int columnCount = 6;
    int rowCount = 5;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    for (int i = static_cast<int>(RSColorBlendMode::NONE); i <= static_cast<int>(RSColorBlendMode::MAX); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Green()));

        auto childNode1 =
            SetUpNodeBgImage("/data/local/tmp/Images/475x327.jpg", { sizeX / 4, sizeY / 4, sizeX / 3, sizeY / 3 });
        childNode1->SetBackgroundBlurRadius(30);
        childNode1->SetBorderStyle(0, 0, 0, 0);
        childNode1->SetBorderWidth(5, 5, 5, 5);
        childNode1->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        childNode1->SetAlpha(0);
        RegisterNode(childNode1);
        auto childNode2 = RSCanvasNode::Create();
        childNode2->SetBounds({ sizeX / 2, sizeY / 2, sizeX / 3, sizeY / 3 });
        childNode2->SetBackgroundColor(0xff00ff00);
        childNode2->SetBackgroundBlurRadius(30);
        childNode2->SetBorderStyle(0, 0, 0, 0);
        childNode2->SetBorderWidth(5, 5, 5, 5);
        childNode2->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        childNode2->SetAlpha(0.5);
        RegisterNode(childNode2);
        auto childNode3 = RSCanvasNode::Create();
        childNode3->SetBounds({ sizeX * 3 / 4, sizeY * 3 / 4, sizeX / 3, sizeY / 3 });
        childNode3->SetBackgroundColor(0xff00ffff);
        childNode3->SetBackgroundBlurRadius(30);
        childNode3->SetBorderStyle(0, 0, 0, 0);
        childNode3->SetBorderWidth(5, 5, 5, 5);
        childNode3->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        childNode3->SetAlpha(1.0);
        RegisterNode(childNode3);
        testNodeBackGround->SetBounds({ x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->AddChild(childNode1);
        testNodeBackGround->AddChild(childNode2);
        testNodeBackGround->AddChild(childNode3);

        testNodeBackGround->SetColorBlendMode(static_cast<RSColorBlendMode>(i));
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Color_Blend_Mode_15)
{
    int columnCount = 6;
    int rowCount = 5;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    for (int i = static_cast<int>(RSColorBlendMode::NONE); i <= static_cast<int>(RSColorBlendMode::MAX); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));

        auto childNode1 =
            SetUpNodeBgImage("/data/local/tmp/Images/475x327.jpg", { sizeX / 4, sizeY / 4, sizeX / 3, sizeY / 3 });
        childNode1->SetBackgroundBlurRadius(30);
        childNode1->SetBorderStyle(0, 0, 0, 0);
        childNode1->SetBorderWidth(5, 5, 5, 5);
        childNode1->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        childNode1->SetAlpha(0.3);
        RegisterNode(childNode1);
        auto childNode2 = RSCanvasNode::Create();
        childNode2->SetBounds({ sizeX / 2, sizeY / 2, sizeX / 3, sizeY / 3 });
        childNode2->SetBackgroundColor(0xff00ff00);
        childNode2->SetBackgroundBlurRadius(30);
        childNode2->SetBorderStyle(0, 0, 0, 0);
        childNode2->SetBorderWidth(5, 5, 5, 5);
        childNode2->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        childNode2->SetAlpha(0.5);
        RegisterNode(childNode2);
        auto childNode3 = RSCanvasNode::Create();
        childNode3->SetBounds({ sizeX * 3 / 4, sizeY * 3 / 4, sizeX / 3, sizeY / 3 });
        childNode3->SetBackgroundColor(0xff00ffff);
        childNode3->SetBackgroundBlurRadius(30);
        childNode3->SetBorderStyle(0, 0, 0, 0);
        childNode3->SetBorderWidth(5, 5, 5, 5);
        childNode3->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        childNode3->SetAlpha(0.7);
        RegisterNode(childNode3);
        testNodeBackGround->SetBounds({ x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->AddChild(childNode1);
        testNodeBackGround->AddChild(childNode2);
        testNodeBackGround->AddChild(childNode3);

        testNodeBackGround->SetColorBlendMode(static_cast<RSColorBlendMode>(i));
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Color_Blend_Mode_16)
{
    int columnCount = 6;
    int rowCount = 5;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    for (int i = static_cast<int>(RSColorBlendMode::NONE); i <= static_cast<int>(RSColorBlendMode::MAX); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Gray()));

        auto childNode1 =
            SetUpNodeBgImage("/data/local/tmp/Images/475x327.jpg", { sizeX / 4, sizeY / 4, sizeX / 3, sizeY / 3 });
        childNode1->SetBackgroundBlurRadius(30);
        childNode1->SetBorderStyle(0, 0, 0, 0);
        childNode1->SetBorderWidth(5, 5, 5, 5);
        childNode1->SetBorderColor(Vector4<Color>(RgbPalette::Gray()));
        childNode1->SetAlpha(0.3);
        RegisterNode(childNode1);
        auto childNode2 = RSCanvasNode::Create();
        childNode2->SetBounds({ sizeX / 2, sizeY / 2, sizeX / 3, sizeY / 3 });
        childNode2->SetBackgroundColor(0xff00ff00);
        childNode2->SetBackgroundBlurRadius(30);
        childNode2->SetBorderStyle(0, 0, 0, 0);
        childNode2->SetBorderWidth(5, 5, 5, 5);
        childNode2->SetBorderColor(Vector4<Color>(RgbPalette::Gray()));
        childNode2->SetAlpha(0.5);
        RegisterNode(childNode2);
        auto childNode3 = RSCanvasNode::Create();
        childNode3->SetBounds({ sizeX * 3 / 4, sizeY * 3 / 4, sizeX / 3, sizeY / 3 });
        childNode3->SetBackgroundColor(0xff00ffff);
        childNode3->SetBackgroundBlurRadius(30);
        childNode3->SetBorderStyle(0, 0, 0, 0);
        childNode3->SetBorderWidth(5, 5, 5, 5);
        childNode3->SetBorderColor(Vector4<Color>(RgbPalette::Gray()));
        childNode3->SetAlpha(0.7);
        RegisterNode(childNode3);
        testNodeBackGround->SetBounds({ x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->AddChild(childNode1);
        testNodeBackGround->AddChild(childNode2);
        testNodeBackGround->AddChild(childNode3);

        testNodeBackGround->SetColorBlendMode(static_cast<RSColorBlendMode>(i));
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Color_Blend_Apply_Type1)
{
    int columnCount = 1;
    int rowCount = 3;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    for (auto i = static_cast<int>(RSColorBlendApplyType::FAST); i <= static_cast<int>(RSColorBlendApplyType::MAX);
         i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Green()));
        testNodeBackGround->SetColorBlendApplyType(static_cast<RSColorBlendApplyType>(i));
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Color_Blend_Apply_Type2)
{
    int columnCount = 2;
    int rowCount = 2;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    for (auto i = static_cast<int>(RSColorBlendApplyType::FAST); i <= static_cast<int>(RSColorBlendApplyType::MAX);
         i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Green()));
        testNodeBackGround->SetColorBlendApplyType(static_cast<RSColorBlendApplyType>(i));
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Color_Blend_Apply_Type3)
{
    int columnCount = 10;
    int rowCount = 10;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    for (auto i = static_cast<int>(RSColorBlendApplyType::FAST); i <= static_cast<int>(RSColorBlendApplyType::MAX);
         i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Green()));
        testNodeBackGround->SetColorBlendApplyType(static_cast<RSColorBlendApplyType>(i));
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Color_Blend_Apply_Type4)
{
    int columnCount = -1;
    int rowCount = -1;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    for (auto i = static_cast<int>(RSColorBlendApplyType::FAST); i <= static_cast<int>(RSColorBlendApplyType::MAX);
         i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Green()));
        testNodeBackGround->SetColorBlendApplyType(static_cast<RSColorBlendApplyType>(i));
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Color_Blend_Apply_Type5)
{
    int columnCount = 1;
    int rowCount = 3;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    for (auto i = static_cast<int>(RSColorBlendApplyType::FAST); i <= static_cast<int>(RSColorBlendApplyType::MAX);
         i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::White()));
        testNodeBackGround->SetColorBlendApplyType(static_cast<RSColorBlendApplyType>(i));
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Color_Blend_Apply_Type6)
{
    int columnCount = 1;
    int rowCount = 3;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    for (auto i = static_cast<int>(RSColorBlendApplyType::FAST); i <= static_cast<int>(RSColorBlendApplyType::MAX);
         i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Black()));
        testNodeBackGround->SetColorBlendApplyType(static_cast<RSColorBlendApplyType>(i));
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Color_Blend_Apply_Type7)
{
    int columnCount = 1;
    int rowCount = 3;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    for (auto i = static_cast<int>(RSColorBlendApplyType::FAST); i <= static_cast<int>(RSColorBlendApplyType::MAX);
         i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBorderStyle(3, 3, 3, 3);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Green()));
        testNodeBackGround->SetColorBlendApplyType(static_cast<RSColorBlendApplyType>(i));
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Color_Blend_Apply_Type8)
{
    int columnCount = 5;
    int rowCount = 5;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    for (auto i = static_cast<int>(RSColorBlendApplyType::FAST); i <= static_cast<int>(RSColorBlendApplyType::MAX);
         i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Black()));
        testNodeBackGround->SetColorBlendApplyType(static_cast<RSColorBlendApplyType>(i));
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Color_Blend_Mode__With_Alpha1)
{
    int columnCount = 1;
    int rowCount = 3;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    for (auto i = static_cast<int>(RSColorBlendApplyType::FAST); i <= static_cast<int>(RSColorBlendApplyType::MAX);
         i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetAlpha(0.5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Green()));
        testNodeBackGround->SetColorBlendApplyType(static_cast<RSColorBlendApplyType>(i));
        testNodeBackGround->SetColorBlendMode(RSColorBlendMode::SRC_OVER);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Color_Blend_Mode__With_Alpha2)
{
    int columnCount = 5;
    int rowCount = 5;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    for (auto i = static_cast<int>(RSColorBlendApplyType::FAST); i <= static_cast<int>(RSColorBlendApplyType::MAX);
         i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetAlpha(0.5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Green()));
        testNodeBackGround->SetColorBlendApplyType(static_cast<RSColorBlendApplyType>(i));
        testNodeBackGround->SetColorBlendMode(RSColorBlendMode::SRC_OVER);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Color_Blend_Mode__With_Alpha3)
{
    int columnCount = 1;
    int rowCount = 3;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    for (auto i = static_cast<int>(RSColorBlendApplyType::FAST); i <= static_cast<int>(RSColorBlendApplyType::MAX);
         i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetAlpha(0.5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        testNodeBackGround->SetColorBlendApplyType(static_cast<RSColorBlendApplyType>(i));
        testNodeBackGround->SetColorBlendMode(RSColorBlendMode::SRC_OVER);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Color_Blend_Mode__With_Alpha4)
{
    int columnCount = 1;
    int rowCount = 3;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    for (auto i = static_cast<int>(RSColorBlendApplyType::FAST); i <= static_cast<int>(RSColorBlendApplyType::MAX);
         i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetAlpha(0.5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Gray()));
        testNodeBackGround->SetColorBlendApplyType(static_cast<RSColorBlendApplyType>(i));
        testNodeBackGround->SetColorBlendMode(RSColorBlendMode::SRC_OVER);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Color_Blend_Mode__With_Alpha5)
{
    int columnCount = 1;
    int rowCount = 3;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    for (auto i = static_cast<int>(RSColorBlendApplyType::FAST); i <= static_cast<int>(RSColorBlendApplyType::MAX);
         i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetAlpha(0);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Green()));
        testNodeBackGround->SetColorBlendApplyType(static_cast<RSColorBlendApplyType>(i));
        testNodeBackGround->SetColorBlendMode(RSColorBlendMode::SRC_OVER);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Color_Blend_Mode__With_Alpha6)
{
    int columnCount = 1;
    int rowCount = 3;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    for (auto i = static_cast<int>(RSColorBlendApplyType::FAST); i <= static_cast<int>(RSColorBlendApplyType::MAX);
         i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetAlpha(-1.0);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Green()));
        testNodeBackGround->SetColorBlendApplyType(static_cast<RSColorBlendApplyType>(i));
        testNodeBackGround->SetColorBlendMode(RSColorBlendMode::SRC_OVER);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Color_Blend_Mode__With_Alpha7)
{
    int columnCount = 1;
    int rowCount = 3;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    for (auto i = static_cast<int>(RSColorBlendApplyType::FAST); i <= static_cast<int>(RSColorBlendApplyType::MAX);
         i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetAlpha(1.0);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Green()));
        testNodeBackGround->SetColorBlendApplyType(static_cast<RSColorBlendApplyType>(i));
        testNodeBackGround->SetColorBlendMode(RSColorBlendMode::SRC_OVER);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Color_Blend_Mode__With_Alpha8)
{
    int columnCount = 2;
    int rowCount = 2;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    for (auto i = static_cast<int>(RSColorBlendApplyType::FAST); i <= static_cast<int>(RSColorBlendApplyType::MAX);
         i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBorderStyle(10, 10, 10, 10);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetAlpha(0.5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Green()));
        testNodeBackGround->SetColorBlendApplyType(static_cast<RSColorBlendApplyType>(i));
        testNodeBackGround->SetColorBlendMode(RSColorBlendMode::SRC_OVER);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Color_Blend_Mode__With_Alpha9)
{
    int columnCount = 1;
    int rowCount = 3;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    for (auto i = static_cast<int>(RSColorBlendApplyType::FAST); i <= static_cast<int>(RSColorBlendApplyType::MAX);
         i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetAlpha(0.2);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        testNodeBackGround->SetColorBlendApplyType(static_cast<RSColorBlendApplyType>(i));
        testNodeBackGround->SetColorBlendMode(RSColorBlendMode::SRC_OVER);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Color_Blend_Mode__With_Alpha10)
{
    int columnCount = 1;
    int rowCount = 10;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    for (auto i = static_cast<int>(RSColorBlendApplyType::FAST); i <= static_cast<int>(RSColorBlendApplyType::MAX);
         i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBorderStyle(3, 3, 3, 3);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetAlpha(0);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Gray()));
        testNodeBackGround->SetColorBlendApplyType(static_cast<RSColorBlendApplyType>(i));
        testNodeBackGround->SetColorBlendMode(RSColorBlendMode::SRC_OVER);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

} // namespace OHOS::Rosen