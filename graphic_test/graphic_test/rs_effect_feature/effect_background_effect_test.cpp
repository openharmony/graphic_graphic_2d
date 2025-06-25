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

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Water_Ripple_Test_1)
{
    int columnCount = 3;
    int rowCount = 3;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<float> progressList = { 0, 0.1, 0.3, 0.5, 0.7, 0.9, 1.0, 20, 100 };

    for (int i = 0; i < progressList.size(); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        RSWaterRipplePara para = { 3, 0.3, 0.5 };
        testNodeBackGround->SetWaterRippleParams(para, progressList[i]);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Water_Ripple_Test_2)
{
    int columnCount = 4;
    int rowCount = 4;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<float> progressList = { 0, 0.2, 0.4, 0.6, 0.8, 1.0, 5, 50 };

    for (int i = 0; i < progressList.size(); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        RSWaterRipplePara para = { 3, 0.3, 0.5 };
        testNodeBackGround->SetWaterRippleParams(para, progressList[i]);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Water_Ripple_Test_3)
{
    int columnCount = 4;
    int rowCount = 4;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<float> progressList = { 0, 0.1, 0.3, 0.5, 0.7, 0.9, 1.0, 20, 100 };

    for (int i = 0; i < progressList.size(); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        RSWaterRipplePara para = { 2, 0.2, 0.3 };
        testNodeBackGround->SetWaterRippleParams(para, progressList[i]);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Water_Ripple_Test_4)
{
    int columnCount = 2;
    int rowCount = 2;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<float> progressList = { 0, 0.2, 0.4, 0.6, 0.8, 1.0, 5, 50 };

    for (int i = 0; i < progressList.size(); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        RSWaterRipplePara para = { 3, 0.3, 0.5 };
        testNodeBackGround->SetWaterRippleParams(para, progressList[i]);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Water_Ripple_Test_5)
{
    int columnCount = 2;
    int rowCount = 4;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<float> progressList = { 0, 0.1, 0.3, 0.5, 0.7, 0.9, 1.0, 20, 100 };

    for (int i = 0; i < progressList.size(); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        RSWaterRipplePara para = { 3, 0.3, 0.5 };
        testNodeBackGround->SetWaterRippleParams(para, progressList[i]);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Water_Ripple_Test_6)
{
    int columnCount = 3;
    int rowCount = 3;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<float> progressList = { 0, 0.1, 0.3, 0.5, 0.7, 0.9, 1.0, 20, 100 };

    for (int i = 0; i < progressList.size(); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        RSWaterRipplePara para = { 8, 0.8, 1.2 };
        testNodeBackGround->SetWaterRippleParams(para, progressList[i]);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Water_Ripple_Test_7)
{
    int columnCount = 3;
    int rowCount = 3;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<float> progressList = { 0, 0.1, 0.3, 0.5, 0.7, 0.9, 1.0, 20, 100 };

    for (int i = 0; i < progressList.size(); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        RSWaterRipplePara para = { 0, 0, 0 };
        testNodeBackGround->SetWaterRippleParams(para, progressList[i]);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Water_Ripple_Test_8)
{
    int columnCount = -1;
    int rowCount = -1;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<float> progressList = { 0, 0.1, 0.3, 0.5, 0.7, 0.9, 1.0, 20, 100 };

    for (int i = 0; i < progressList.size(); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        RSWaterRipplePara para = { 2, 0.2, 0.3 };
        testNodeBackGround->SetWaterRippleParams(para, progressList[i]);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Water_Ripple_Test_9)
{
    int columnCount = 0;
    int rowCount = 0;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<float> progressList = { 0, 0.1, 0.3, 0.5, 0.7, 0.9, 1.0, 20, 100 };

    for (int i = 0; i < progressList.size(); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        RSWaterRipplePara para = { 6, 0.6, 0.9 };
        testNodeBackGround->SetWaterRippleParams(para, progressList[i]);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Water_Ripple_Test_10)
{
    int columnCount = 10;
    int rowCount = 10;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<float> progressList = { 0, 0.1, 0.3, 0.5, 0.7, 0.9, 1.0, 20, 100 };

    for (int i = 0; i < progressList.size(); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        RSWaterRipplePara para = { 3, 0.3, 0.5 };
        testNodeBackGround->SetWaterRippleParams(para, progressList[i]);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Water_Ripple_Test_11)
{
    int columnCount = 4;
    int rowCount = 2;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<float> progressList = { 0, 0.2, 0.4, 0.6, 0.8, 1.0, 5, 50 };

    for (int i = 0; i < progressList.size(); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        RSWaterRipplePara para = { 10, 1.3, 1.5 };
        testNodeBackGround->SetWaterRippleParams(para, progressList[i]);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Fly_In_Test_1)
{
    int columnCount = 3;
    int rowCount = 3;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<float> progressList = { 0, 0.1, 0.3, 0.5, 0.7, 0.9, 1.0, 20, 100 };

    for (int i = 0; i < progressList.size(); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        RSFlyOutPara para = { i % 2 };
        testNodeBackGround->SetFlyOutParams(para, progressList[i]);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Fly_In_Test_2)
{
    int columnCount = 3;
    int rowCount = 3;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<float> progressList = { 0, 0.1, 0.3, 0.5, 0.7, 0.9, 1.0, 20, 100 };

    for (int i = 0; i < progressList.size(); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        RSFlyOutPara para = { -1 };
        testNodeBackGround->SetFlyOutParams(para, progressList[i]);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Fly_In_Test_3)
{
    int columnCount = 3;
    int rowCount = 3;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<float> progressList = { 0, 0.1, 0.3, 0.5, 0.7, 0.9, 1.0, 20, 100 };

    for (int i = 0; i < progressList.size(); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        RSFlyOutPara para = { 2 };
        testNodeBackGround->SetFlyOutParams(para, progressList[i]);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Fly_In_Test_4)
{
    int columnCount = 3;
    int rowCount = 3;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<float> progressList = { 0, 0.1, 0.3, 0.5, 0.7, 0.9, 1.0, 20, 100 };

    for (int i = 0; i < progressList.size(); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        RSFlyOutPara para = { 10 };
        testNodeBackGround->SetFlyOutParams(para, progressList[i]);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Fly_In_Test_5)
{
    int columnCount = 1000;
    int rowCount = 1000;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<float> progressList = { 0, 0.1, 0.3, 0.5, 0.7, 0.9, 1.0, 20, 100 };

    for (int i = 0; i < progressList.size(); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        RSFlyOutPara para = { i % 2 };
        testNodeBackGround->SetFlyOutParams(para, progressList[i]);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Fly_In_Test_6)
{
    int columnCount = 1;
    int rowCount = 1;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<float> progressList = { 0, 0.1, 0.3, 0.5, 0.7, 0.9, 1.0, 20, 100 };

    for (int i = 0; i < progressList.size(); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        RSFlyOutPara para = { i % 2 };
        testNodeBackGround->SetFlyOutParams(para, progressList[i]);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Fly_In_Test_7)
{
    int columnCount = 0;
    int rowCount = 0;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<float> progressList = { 0, 0.1, 0.3, 0.5, 0.7, 0.9, 1.0, 20, 100 };

    for (int i = 0; i < progressList.size(); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        RSFlyOutPara para = { i % 2 };
        testNodeBackGround->SetFlyOutParams(para, progressList[i]);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Fly_In_Test_8)
{
    int columnCount = 4;
    int rowCount = 6;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<float> progressList = { 0, 0.1, 0.3, 0.5, 0.7, 0.9, 1.0, 20, 100 };

    for (int i = 0; i < progressList.size(); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        RSFlyOutPara para = { i % 2 };
        testNodeBackGround->SetFlyOutParams(para, progressList[i]);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Fly_In_Test_9)
{
    int columnCount = 100;
    int rowCount = 100;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<float> progressList = { 0, 0.1, 0.3, 0.5, 0.7, 0.9, 1.0, 20, 100 };

    for (int i = 0; i < progressList.size(); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        RSFlyOutPara para = { i % 2 };
        testNodeBackGround->SetFlyOutParams(para, progressList[i]);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}
GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Fly_In_Test_10)
{
    int columnCount = -1;
    int rowCount = -1;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<float> progressList = { 0, 0.1, 0.3, 0.5, 0.7, 0.9, 1.0, 20, 100 };

    for (int i = 0; i < progressList.size(); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        RSFlyOutPara para = { i % 2 };
        testNodeBackGround->SetFlyOutParams(para, progressList[i]);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Fly_In_Test_11)
{
    int columnCount = 1;
    int rowCount = 50;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<float> progressList = { 0, 0.1, 0.3, 0.5, 0.7, 0.9, 1.0, 20, 100 };

    for (int i = 0; i < progressList.size(); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        RSFlyOutPara para = { i % 2 };
        testNodeBackGround->SetFlyOutParams(para, progressList[i]);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Distort_Test_1)
{
    int columnCount = 3;
    int rowCount = 3;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<float> valueList = { 1, -10, -1, -0.5, 0, 0.1, 0.3, 0.5, 10 };

    for (int i = 0; i < valueList.size(); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetDistortionK(valueList[i]);
        testNodeBackGround->SetClipToBounds(true);
        testNodeBackGround->SetClipToFrame(true);
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Green()));
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Distort_Test_2)
{
    int columnCount = 3;
    int rowCount = 3;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<float> valueList = { 1, -10, -1, -0.5, 0, 0.1, 0.3, 0.5, 10 };

    for (int i = 0; i < valueList.size(); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetDistortionK(valueList[i]);
        testNodeBackGround->SetClipToBounds(true);
        testNodeBackGround->SetClipToFrame(true);
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Distort_Test_3)
{
    int columnCount = 2;
    int rowCount = 6;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<float> valueList = { 1, -10, -1, -0.5, 0, 0.1, 0.3, 0.5, 10 };

    for (int i = 0; i < valueList.size(); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetDistortionK(valueList[i]);
        testNodeBackGround->SetClipToBounds(true);
        testNodeBackGround->SetClipToFrame(true);
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Green()));
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Distort_Test_4)
{
    int columnCount = 3;
    int rowCount = 3;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<float> valueList = { 1, -10, -1, -0.5, 0, 0.1, 0.3, 0.5, 10 };

    for (int i = 0; i < valueList.size(); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetDistortionK(valueList[i]);
        testNodeBackGround->SetClipToBounds(true);
        testNodeBackGround->SetClipToFrame(true);
        testNodeBackGround->SetBorderStyle(1, 1, 1, 1);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Green()));
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Distort_Test_5)
{
    int columnCount = 7;
    int rowCount = 5;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<float> valueList = { 1, -10, -1, -0.5, 0, 0.1, 0.3, 0.5, 10 };

    for (int i = 0; i < valueList.size(); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetDistortionK(valueList[i]);
        testNodeBackGround->SetClipToBounds(true);
        testNodeBackGround->SetClipToFrame(true);
        testNodeBackGround->SetBorderStyle(3, 3, 3, 3);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Green()));
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Distort_Test_6)
{
    int columnCount = 3;
    int rowCount = 3;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<float> valueList = { 1, -10, -1, -0.5, 0, 0.1, 0.3, 0.5, 10 };

    for (int i = 0; i < valueList.size(); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetDistortionK(valueList[i]);
        testNodeBackGround->SetClipToBounds(true);
        testNodeBackGround->SetClipToFrame(true);
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(0, 0, 0, 0);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Green()));
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Distort_Test_7)
{
    int columnCount = 3;
    int rowCount = 3;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<float> valueList = { 1, -10, -1, -0.5, 0, 0.1, 0.3, 0.5, 10 };

    for (int i = 0; i < valueList.size(); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetDistortionK(valueList[i]);
        testNodeBackGround->SetClipToBounds(true);
        testNodeBackGround->SetClipToFrame(true);
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Black()));
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Distort_Test_8)
{
    int columnCount = 3;
    int rowCount = 3;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<float> valueList = { 1, -10, -1, -0.5, 0, 0.1, 0.3, 0.5, 10 };

    for (int i = 0; i < valueList.size(); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetDistortionK(valueList[i]);
        testNodeBackGround->SetClipToBounds(true);
        testNodeBackGround->SetClipToFrame(true);
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::White()));
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Distort_Test_9)
{
    int columnCount = 3;
    int rowCount = 3;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<float> valueList = { 1, -10, -1, -0.5, 0, 0.1, 0.3, 0.5, 10 };

    for (int i = 0; i < valueList.size(); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetDistortionK(valueList[i]);
        testNodeBackGround->SetClipToBounds(true);
        testNodeBackGround->SetClipToFrame(true);
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(1, 1, 1, 1);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::White()));
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Distort_Test_10)
{
    int columnCount = 3;
    int rowCount = 3;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<float> valueList = { 1, -10, -1, -0.5, 0, 0.1, 0.3, 0.5, 10 };

    for (int i = 0; i < valueList.size(); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetDistortionK(valueList[i]);
        testNodeBackGround->SetClipToBounds(true);
        testNodeBackGround->SetClipToFrame(true);
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(100, 100, 100, 100);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Green()));
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, EFFECT_TEST, Set_Distort_Test_11)
{
    int columnCount = 3;
    int rowCount = 3;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<float> valueList = { 1, -10, -1, -0.5, 0, 0.1, 0.3, 0.5, 10 };

    for (int i = 0; i < valueList.size(); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetDistortionK(valueList[i]);
        testNodeBackGround->SetClipToBounds(true);
        testNodeBackGround->SetClipToFrame(true);
        testNodeBackGround->SetBorderStyle(100, 100, 100, 100);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Green()));
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

} // namespace OHOS::Rosen