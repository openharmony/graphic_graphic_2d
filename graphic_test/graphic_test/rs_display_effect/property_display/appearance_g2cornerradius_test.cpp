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

GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, G2_CornerRadius_DrawRoundRect)
{
    int columnCount = 2;
    int rowCount = 4;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;
    int i = 0;
    int j = 0;

    // Normal Radius
    std::vector<float> normalRadius = { 50, 100 };
    for (; i < 2; i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround = RSCanvasNode::Create();
        testNodeBackGround->SetBackgroundColor(0xff0000ff);
        testNodeBackGround->SetBounds({ x, y, 400, 300 });
        testNodeBackGround->SetCornerRadius(normalRadius[i]);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }

    // Asymmetry Radius
    std::vector<Vector4f> asymmetryRadius = { { 20, 60, 100, 160 }, { 0, 0, 150, 150 } };
    for (; i < 4; i++, j++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround = RSCanvasNode::Create();
        testNodeBackGround->SetBackgroundColor(0xff0000ff);
        testNodeBackGround->SetBounds({ x, y, 400, 300 });
        testNodeBackGround->SetCornerRadius(asymmetryRadius[j]);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }

    // Capsule Radius
    std::vector<Vector4f> capsuleRadius = { Vector4f(150), Vector4f(200), Vector4f(260), { 160, 180, 200, 220 } };
    j = 0;
    for (; i < 8; i++, j++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround = RSCanvasNode::Create();
        testNodeBackGround->SetBackgroundColor(0xff0000ff);
        testNodeBackGround->SetBounds({ x, y, 400, 300 });
        testNodeBackGround->SetCornerRadius(capsuleRadius[j]);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, G2_CornerRadius_ClipRoundRect)
{
    int columnCount = 2;
    int rowCount = 4;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    int i = 0;
    int j = 0;

    // Normal Radius
    std::vector<float> normalRadius = { 50, 100 };
    for (; i < 2; i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround = SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, 400, 300 });
        testNodeBackGround->SetCornerRadius(normalRadius[i]);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }

    // Asymmetry Radius
    std::vector<Vector4f> asymmetryRadius = { { 20, 60, 100, 160 }, { 0, 0, 150, 150 } };
    j = 0;
    for (; i < 4; i++, j++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround = SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, 400, 300 });
        testNodeBackGround->SetCornerRadius(asymmetryRadius[j]);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }

    // Capsule Radius
    std::vector<float> capsuleRadius = { 150, 200, 260 };
    j = 0;
    for (; i < 7; i++, j++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround = SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, 400, 300 });
        testNodeBackGround->SetCornerRadius(capsuleRadius[j]);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }

    i = 7;
    int x = (i % columnCount) * sizeX;
    int y = (i / columnCount) * sizeY;
    auto testNodeBackGround = SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, 400, 300 });
    testNodeBackGround->SetCornerRadius({ 160, 180, 200, 220 });
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, G2_CornerRadius_FromDTS_1)
{
    // DTS DTS2024011614283
    int columnCount = 2;
    int rowCount = 2;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<Vector4f> radiusList = { { 10, 10, 0, 0 }, { 10, 10, 10, 10 } };
    for (int i = 0; i < 2; i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround1 = RSCanvasNode::Create();
        testNodeBackGround1->SetBackgroundColor(0xffffffff);
        testNodeBackGround1->SetBounds({ x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround1->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround1->SetBorderWidth(1, 1, 1, 1);
        testNodeBackGround1->SetBorderColor(Vector4<Color>(RgbPalette::Blue()));
        RegisterNode(testNodeBackGround1);
        auto testNodeBackGround2 = RSCanvasNode::Create();
        testNodeBackGround2->SetBackgroundColor(0xffffffff);
        testNodeBackGround2->SetBounds({ 0, 0, sizeX - 10, sizeY - 10 });
        testNodeBackGround2->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround2->SetBorderWidth(7.5, 1, 1, 1);
        testNodeBackGround2->SetBorderColor(Vector4<Color>(RgbPalette::Green()));
        testNodeBackGround2->SetCornerRadius(radiusList[i]);
        RegisterNode(testNodeBackGround2);
        testNodeBackGround1->AddChild(testNodeBackGround2);
        GetRootNode()->AddChild(testNodeBackGround1);
    }

    for (int i = 2; i < 4; i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround1 = RSCanvasNode::Create();
        testNodeBackGround1->SetBackgroundColor(0xffffffff);
        testNodeBackGround1->SetBounds({ x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround1->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround1->SetBorderWidth(10, 10, 10, 10);
        testNodeBackGround1->SetBorderColor(Vector4<Color>(RgbPalette::Blue()));
        RegisterNode(testNodeBackGround1);
        auto testNodeBackGround2 = RSCanvasNode::Create();
        testNodeBackGround2->SetBackgroundColor(0xffffffff);
        testNodeBackGround2->SetBounds({ 0, 0, sizeX - 10, sizeY - 10 });
        testNodeBackGround2->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround2->SetBorderWidth(10, 10, 10, 10);
        testNodeBackGround2->SetBorderColor(Vector4<Color>(RgbPalette::Green()));
        testNodeBackGround2->SetCornerRadius(radiusList[i - 2]);
        RegisterNode(testNodeBackGround2);
        testNodeBackGround2->AddChild(testNodeBackGround1);
        GetRootNode()->AddChild(testNodeBackGround2);
    }
}

GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, G1_To_G2_DrawRoundRect_Test_01)
{
    int columnCount = 4;
    int rowCount = 4;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    // get the smaller one between node's width and height
    int nodeSize = sizeX < sizeY ? sizeX - 10 : sizeY - 10;
    float radiusMultiplier[] = { 0.9, 1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9, 2.0, 2.5, 3.0, 3.5, 4.0 };

    for (int i = 0; i < columnCount * rowCount; i++) {
        Vector4f cornerRadiusVal = { nodeSize / radiusMultiplier[i], 0, 0, 0 };
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround = RSCanvasNode::Create();
        testNodeBackGround->SetBackgroundColor(0xff0000ff);
        testNodeBackGround->SetBounds({ x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetCornerRadius(cornerRadiusVal);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, G1_To_G2_DrawRoundRect_Test_02)
{
    int columnCount = 4;
    int rowCount = 4;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    // get the smaller one between node's width and height
    int nodeSize = sizeX < sizeY ? sizeX - 10 : sizeY - 10;
    float radiusMultiplier[] = { 0.9, 1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9, 2.0, 2.5, 3.0, 3.5, 4.0 };

    for (int i = 0; i < columnCount * rowCount; i++) {
        Vector4f cornerRadiusVal = { nodeSize / (2 * radiusMultiplier[i]), nodeSize / (2 * radiusMultiplier[i]), 0, 0 };
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround = RSCanvasNode::Create();
        testNodeBackGround->SetBackgroundColor(0xff0000ff);
        testNodeBackGround->SetBounds({ x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetCornerRadius(cornerRadiusVal);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, G1_To_G2_DrawRoundRect_Test_03)
{
    int columnCount = 4;
    int rowCount = 4;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    // get the smaller one between node's width and height
    int nodeSize = sizeX < sizeY ? sizeX - 10 : sizeY - 10;
    float radiusMultiplier[] = { 0.9, 1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9, 2.0, 2.5, 3.0, 3.5, 4.0 };

    for (int i = 0; i < columnCount * rowCount; i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround = RSCanvasNode::Create();
        testNodeBackGround->SetBackgroundColor(0xff0000ff);
        testNodeBackGround->SetBounds({ x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetCornerRadius(nodeSize / (2 * radiusMultiplier[i]));
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, G1_To_G2_ClipRoundRect_Test_01)
{
    int columnCount = 4;
    int rowCount = 4;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    // get the smaller one between node's width and height
    int nodeSize = sizeX < sizeY ? sizeX - 10 : sizeY - 10;
    float radiusMultiplier[] = { 0.9, 1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9, 2.0, 2.5, 3.0, 3.5, 4.0 };

    for (int i = 0; i < columnCount * rowCount; i++) {
        Vector4f cornerRadiusVal = { nodeSize / radiusMultiplier[i], 0, 0, 0 };
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBounds({ x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetCornerRadius(cornerRadiusVal);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, G1_To_G2_ClipRoundRect_Test_02)
{
    int columnCount = 4;
    int rowCount = 4;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    // get the smaller one between node's width and height
    int nodeSize = sizeX < sizeY ? sizeX - 10 : sizeY - 10;
    float radiusMultiplier[] = { 0.9, 1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9, 2.0, 2.5, 3.0, 3.5, 4.0 };

    for (int i = 0; i < columnCount * rowCount; i++) {
        Vector4f cornerRadiusVal = { nodeSize / (2 * radiusMultiplier[i]), nodeSize / (2 * radiusMultiplier[i]), 0, 0 };
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBounds({ x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetCornerRadius(cornerRadiusVal);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, G1_To_G2_ClipRoundRect_Test_03)
{
    int columnCount = 4;
    int rowCount = 4;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    // get the smaller one between node's width and height
    int nodeSize = sizeX < sizeY ? sizeX - 10 : sizeY - 10;
    float radiusMultiplier[] = { 0.9, 1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9, 2.0, 2.5, 3.0, 3.5, 4.0 };

    for (int i = 0; i < columnCount * rowCount; i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBounds({ x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetCornerRadius(nodeSize / (2 * radiusMultiplier[i]));
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, G2_CornerRadius_DrawNestedRoundRect)
{
    int columnCount = 2;
    int rowCount = 4;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;
    int i = 0;
    int j = 0;

    // Normal Radius
    std::vector<float> normalRadius = { 50, 100 };
    for (; i < 2; i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround = RSCanvasNode::Create();
        testNodeBackGround->SetBackgroundColor(0xff0000ff);
        testNodeBackGround->SetBounds({ x, y, 400, 300 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(20, 25, 10, 30);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        testNodeBackGround->SetCornerRadius(normalRadius[i]);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }

    // Asymmetry Radius and Capsule Radius
    std::vector<Vector4f> asymmetryRadius = { { 20, 60, 100, 160 }, { 0, 0, 150, 150 }, Vector4f(150), Vector4f(200),
        Vector4f(260), { 160, 180, 200, 220 } };
    for (; i < 8; i++, j++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround = RSCanvasNode::Create();
        testNodeBackGround->SetBackgroundColor(0xff0000ff);
        testNodeBackGround->SetBounds({ x, y, 400, 300 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(50, 20, 50, 20);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        testNodeBackGround->SetCornerRadius(asymmetryRadius[j]);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

} // namespace OHOS::Rosen