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

#include "effect/shader_effect.h"

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

GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, BackGround_Color_Test)
{
    int columnCount = 3;
    int rowCount = 3;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<uint32_t> colorList = { 0xffffffff, 0xff000000, 0xffff0000, 0xff00ff00, 0xff0000ff, 0x7f000000,
        0x3f000000, 0x00000000 };
    for (int i = 0; i < colorList.size(); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround = RSCanvasNode::Create();
        testNodeBackGround->SetBackgroundColor(colorList[i]);
        testNodeBackGround->SetBounds({ x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBorderWidth(5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    };
}

GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, BackGround_Brightness_Parameter_Test_1)
{
    float rateList[] = { -0.05, 0.0, 1.0, 20.0 };
    float saturationList[] = { 0.0, 5.0, 10.0, 20.0 };
    std::array<float, 3> RGB[] = { { 2.3, 4.5, 2 }, { 0.5, 2, 0.5 } };
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 2; k++) {
                RSDynamicBrightnessPara params = RSDynamicBrightnessPara(rateList[i], rateList[(i + 3) % 4],
                    saturationList[j], saturationList[(j + 1) % 4], saturationList[j], RGB[k], RGB[(k + 1) % 2]);
                params.fraction_ = 0.5;
                int x = i * 310;
                int y = (k + j * 2) * 310;
                auto testFaNode = SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, 300, 300 });
                auto testNode = RSCanvasNode::Create();
                testNode->SetBounds({ 0, 0, 300, 300 });
                testNode->SetBackgroundColor(0xff7d112c);
                testNode->SetBgBrightnessParams(params);
                testNode->SetBgBrightnessFract(params.fraction_);
                GetRootNode()->AddChild(testFaNode);
                testFaNode->AddChild(testNode);
                RegisterNode(testFaNode);
                RegisterNode(testNode);
            }
        }
    }
}

GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, BackGround_Brightness_Parameter_Test_2)
{
    float rateList[] = { -0.05, 0.0, 1.0, 20.0 };
    float saturationList[] = { 0.0, 5.0, 10.0, 20.0 };
    std::array<float, 3> RGB[] = { { 2.3, 4.5, 2 }, { 0.5, 2, 0.5 } };
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 2; k++) {
                RSDynamicBrightnessPara params = RSDynamicBrightnessPara(rateList[i], rateList[(i + 3) % 4],
                    saturationList[j], saturationList[(j + 1) % 4], saturationList[j], RGB[k], RGB[(k + 1) % 2]);
                params.fraction_ = 1.0;
                int x = i * 310;
                int y = (k + j * 2) * 310;
                auto testFaNode = SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, 300, 300 });
                auto testNode = RSCanvasNode::Create();
                testNode->SetBounds({ 0, 0, 300, 300 });
                testNode->SetBackgroundColor(0xff7d112c);
                testNode->SetBgBrightnessParams(params);
                testNode->SetBgBrightnessFract(params.fraction_);
                GetRootNode()->AddChild(testFaNode);
                testFaNode->AddChild(testNode);
                RegisterNode(testFaNode);
                RegisterNode(testNode);
            }
        }
    }
}

GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, BackGround_Brightness_Parameter_Test_3)
{
    float rateList[] = { -0.05, 0.0, 1.0, 20.0 };
    float saturationList[] = { 0.0, 5.0, 10.0, 20.0 };
    std::array<float, 3> RGB[] = { { 2.3, 4.5, 2 }, { 0.5, 2, 0.5 } };
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 2; k++) {
                RSDynamicBrightnessPara params = RSDynamicBrightnessPara(rateList[i], rateList[(i + 3) % 4],
                    saturationList[j], saturationList[(j + 1) % 4], saturationList[j], RGB[k], RGB[(k + 1) % 2]);
                params.fraction_ = 0.0;
                int x = i * 310;
                int y = (k + j * 2) * 310;
                auto testFaNode = SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, 300, 300 });
                auto testNode = RSCanvasNode::Create();
                testNode->SetBounds({ 0, 0, 300, 300 });
                testNode->SetBackgroundColor(0xff7d112c);
                testNode->SetBgBrightnessParams(params);
                testNode->SetBgBrightnessFract(params.fraction_);
                GetRootNode()->AddChild(testFaNode);
                testFaNode->AddChild(testNode);
                RegisterNode(testFaNode);
                RegisterNode(testNode);
            }
        }
    }
}

GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, BackGround_Brightness_Parameter_Test_4)
{
    float rateList[] = { -0.05, 0.0, 1.0, 20.0 };
    float saturationList[] = { 0.0, 5.0, 10.0, 20.0 };
    std::array<float, 3> RGB[] = { { 2.3, 4.5, 2 }, { 0.5, 2, 0.5 } };
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 2; k++) {
                RSDynamicBrightnessPara params = RSDynamicBrightnessPara(
                    rateList[i], rateList[(i + 3) % 4], 0.0, 0.0, saturationList[j], RGB[k], RGB[(k + 1) % 2]);
                params.fraction_ = 0.0;
                int x = i * 310;
                int y = (k + j * 2) * 310;
                auto testFaNode = SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, 300, 300 });
                auto testNode = RSCanvasNode::Create();
                testNode->SetBounds({ 0, 0, 300, 300 });
                testNode->SetBackgroundColor(0xff7d112c);
                testNode->SetBgBrightnessParams(params);
                testNode->SetBgBrightnessFract(params.fraction_);
                GetRootNode()->AddChild(testFaNode);
                testFaNode->AddChild(testNode);
                RegisterNode(testFaNode);
                RegisterNode(testNode);
            }
        }
    }
}

GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, BackGround_VisualEffect_Test)
{
    float rateList[] = { -0.05, 0.0, 1.0, 20.0 };
    float saturationList[] = { 0.0, 5.0, 10.0, 20.0 };
    Vector3f RGB[] = { { 2.3, 4.5, 2 }, { 0.5, 2, 0.5 } };
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 2; k++) {
                float fraction = 0.0;
                auto brightnessBlender = std::make_shared<BrightnessBlender>();
                brightnessBlender->SetLinearRate(rateList[i]);
                brightnessBlender->SetDegree(rateList[(i + 3) % 4]);
                brightnessBlender->SetCubicRate(saturationList[j]);
                brightnessBlender->SetQuadRate(saturationList[(j + 1) % 4]);
                brightnessBlender->SetSaturation(saturationList[j]);
                brightnessBlender->SetPositiveCoeff(RGB[k]);
                brightnessBlender->SetNegativeCoeff(RGB[(k + 1) % 2]);
                brightnessBlender->SetFraction(fraction);

                auto backgroundColorEffectPara = std::make_shared<BackgroundColorEffectPara>();
                backgroundColorEffectPara->SetBlender(brightnessBlender);
                VisualEffect effect;
                effect.AddPara(backgroundColorEffectPara);

                int x = i * 310;           // start position x of the image
                int y = (k + j * 2) * 310; // calculate the positon y of the image
                auto testFaNode = SetUpNodeBgImage(
                    "/data/local/tmp/Images/backGroundImage.jpg", { x, y, 300, 300 }); // 300 is image height and width
                auto testNode = RSCanvasNode::Create();
                testNode->SetBounds({ 0, 0, 300, 300 });  // Set the bounds height and width to 300
                testNode->SetBackgroundColor(0xff7d112c); // Set the background color
                testNode->SetVisualEffect(&effect);
                GetRootNode()->AddChild(testFaNode);
                testFaNode->AddChild(testNode);
                RegisterNode(testFaNode);
                RegisterNode(testNode);
            }
        }
    }
}

GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, BackGround_Brightness_Fract_Test)
{
    int columnCount = 2;
    int rowCount = 2;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<float> brightnessFractList = { 0, 0.5, 1, 200 };
    for (int i = 0; i < brightnessFractList.size(); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBgBrightnessFract(brightnessFractList[i]);
        testNodeBackGround->SetBgBrightnessRates({ 10, 0, 10, 0 });
        testNodeBackGround->SetBgBrightnessSaturation(10);
        testNodeBackGround->SetBgBrightnessPosCoeff({ 4, 5, 6, 0 });
        testNodeBackGround->SetBgBrightnessNegCoeff({ 2, 3, 4, 0 });
        testNodeBackGround->SetBorderWidth(5);
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, Grey_Coef_Test)
{
    int columnCount = 2;
    int rowCount = 3;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<Vector2f> greyCoefList = { { -10, -10 }, { -1, -1 }, { 0, 0 }, { 0.5, 0.5 }, { 1, 1 }, { 10, 10 } };

    for (int i = 0; i < greyCoefList.size(); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetGreyCoef(greyCoefList[i]);
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, Background_Color_Test001)
{
    auto testNode1 = RSCanvasNode::Create();
    testNode1->SetBounds({ 0, 0, 500, 500 });
    testNode1->SetBackgroundColor(0xff000000);
    GetRootNode()->AddChild(testNode1);
    RegisterNode(testNode1);
}

GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, Background_Color_Test002)
{
    auto testNode1 = RSCanvasNode::Create();
    testNode1->SetBounds({ -100, 0, 500, 500 });
    testNode1->SetBackgroundColor(0xf0f00000);
    GetRootNode()->AddChild(testNode1);
    RegisterNode(testNode1);
}

GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, Background_Color_Test003)
{
    auto testNode1 = RSCanvasNode::Create();
    testNode1->SetBounds({ 0, -100, 500, 500 });
    testNode1->SetBackgroundColor(0xf00f0000);
    GetRootNode()->AddChild(testNode1);
    RegisterNode(testNode1);
}

GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, Background_Color_Test004)
{
    auto testNode1 = RSCanvasNode::Create();
    testNode1->SetBounds({ 0, 0, -500, 500 });
    testNode1->SetBackgroundColor(0xf00f0000);
    GetRootNode()->AddChild(testNode1);
    RegisterNode(testNode1);
}

GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, Background_Color_Test005)
{
    auto testNode1 = RSCanvasNode::Create();
    testNode1->SetBounds({ 0, 0, 500, -500 });
    testNode1->SetBackgroundColor(0xf000f000);
    GetRootNode()->AddChild(testNode1);
    RegisterNode(testNode1);
}

GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, Background_Color_Test006)
{
    auto testNode1 = RSCanvasNode::Create();
    testNode1->SetBounds({ 0, 0, -500, -500 });
    testNode1->SetBackgroundColor(0xf0000f00);
    GetRootNode()->AddChild(testNode1);
    RegisterNode(testNode1);
}

GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, Background_Color_Test007)
{
    auto testNode1 = RSCanvasNode::Create();
    testNode1->SetBounds({ -100, -100, 500, 500 });
    testNode1->SetBackgroundColor(0xf00000f0);
    GetRootNode()->AddChild(testNode1);
    RegisterNode(testNode1);
}

GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, Background_Color_Test008)
{
    auto testNode1 = RSCanvasNode::Create();
    testNode1->SetBounds({ -100, -100, -500, 500 });
    testNode1->SetBackgroundColor(0xf000000f);
    GetRootNode()->AddChild(testNode1);
    RegisterNode(testNode1);
}

GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, Background_Color_Test009)
{
    auto testNode1 = RSCanvasNode::Create();
    testNode1->SetBounds({ -100, -100, -500, -500 });
    testNode1->SetBackgroundColor(0x000ff000);
    GetRootNode()->AddChild(testNode1);
    RegisterNode(testNode1);
}

GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, Background_Color_Test010)
{
    auto testNode1 = RSCanvasNode::Create();
    testNode1->SetBounds({ 100, 100, 500, 500 });
    testNode1->SetBackgroundColor(0x000000ff);
    GetRootNode()->AddChild(testNode1);
    RegisterNode(testNode1);
}
} // namespace OHOS::Rosen