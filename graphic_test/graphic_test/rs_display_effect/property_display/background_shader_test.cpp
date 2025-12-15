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
#include "render/rs_flow_light_sweep_shader.h"
#include "ui_effect/property/include/rs_ui_shader_base.h"

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

struct DotMatrixTestParam {
public:
    std::vector<uint32_t> dotColorList;
    std::vector<float> dotRadiusList;
    std::vector<float> dotSpacingList;
    std::vector<uint32_t> bgColorList;
    std::vector<float> progressList;
    std::vector<std::vector<Drawing::Color>> effectColorList;
    std::vector<Vector2f> colorFractionsList;
    std::vector<std::vector<Drawing::Point>> startPointsList;
    std::vector<float> pathWidthList;
    std::vector<bool> inverseEffectList;

    void InitRippleEffectParam()
    {
        dotColorList = { 0xffff0000, 0xffff0000, 0xffff0000, 0xff00ff00, 0xff00ff00, 0xff00ff00, 0xff0000ff, 0xff0000ff,
            0xff0000ff };
        dotRadiusList = { -0.7, 0.02, 0.05, 0.1, 0.2, 0.4, 0.6, 1.0, 10.0 };
        dotSpacingList = { -10.0, 10.0, 50.0, 20.0, 40.0, 60.0, 10.0, 50.0, 100.0 };
        bgColorList = { 0xffffffff, 0xff888888, 0xff000000, 0xffffffff, 0xff888888, 0xff000000, 0xffffffff, 0xff888888,
            0xff000000 };
        effectColorList = { { 0xffffff00, 0xffffff00, 0xff00ffff, 0xff00ffff },
            { 0xffff00ff, 0xffff00ff, 0xffffff00, 0xffffff00 }, { 0xff00ffff, 0xff00ffff, 0xff0000ff, 0xff0000ff },
            { 0xffff0000, 0xffff0000, 0xffffff00, 0xffffff00 }, { 0xff0000ff, 0xff0000ff, 0xff00ffff, 0xff00ffff },
            { 0xff00ffff, 0xff00ffff, 0xffff00ff, 0xffff00ff }, { 0xffff0000, 0xff00ff00, 0xff0000ff, 0xffffffff },
            { 0xffffffff, 0xff888888, 0xff000000, 0xff888888 }, { 0xff0000ff, 0xff00ff00, 0xff0000ff, 0xffff0000 } };
        progressList = { -18.6, 0.0, 0.1, 0.3, 0.5, 0.7, 0.9, 2.0, 20.0 };
        colorFractionsList = { { -0.2, -0.2 }, { 0.0, 0.0 }, { 0.5, 1.2 }, { 0.2, 0.3 }, { 0.8, 0.1 }, { 0.5, 1.0 },
            { 1.0, 0.2 }, { 0.5, 0.5 }, { 0.1, 0.9 } };
        startPointsList = {
            {},
            { Drawing::PointF(0.1, 0.9) },
            { Drawing::PointF(0.5, 0.5), Drawing::PointF(0.2, 0.2) },
            { Drawing::PointF(-0.5, 0.5), Drawing::PointF(1.2, 0.2) },
            { Drawing::PointF(0, 1), Drawing::PointF(1, 0) },
            { Drawing::PointF(0.2, 0.8), Drawing::PointF(0.8, 0.2) },
            { Drawing::PointF(0, 0), Drawing::PointF(1, 1) },
            { Drawing::PointF(0.2, 0.8), Drawing::PointF(0.8, 0.2) },
            { Drawing::PointF(0, 0), Drawing::PointF(1, 1) },
        };
        pathWidthList = { -0.5, 0.0, 0.1, 0.3, 0.5, 0.7, 0.9, 1.0, 5.0 };
        inverseEffectList = { false, true, false, true, false, true, false, true, false };
    }
};

GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, BackGround_Shader_Test)
{
    int columnCount = 2;
    int rowCount = 2;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<std::shared_ptr<OHOS::Rosen::Drawing::ShaderEffect>> shaderEffectList = {
        OHOS::Rosen::Drawing::ShaderEffect::CreateColorShader(Drawing::Color::COLOR_CYAN),
        OHOS::Rosen::Drawing::ShaderEffect::CreateLinearGradient({ 10, 10 }, { 100, 100 },
            { Drawing::Color::COLOR_GREEN, Drawing::Color::COLOR_BLUE, Drawing::Color::COLOR_RED }, { 0.0, 0.5, 1.0 },
            Drawing::TileMode::MIRROR)
    };
    for (int i = 0; i < shaderEffectList.size(); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround = RSCanvasNode::Create();
        testNodeBackGround->SetBounds({ x, y, sizeX - 10, sizeY - 10 });
        auto currentShader = RSShader::CreateRSShader(shaderEffectList[i]);
        testNodeBackGround->SetBackgroundShader(currentShader);
        testNodeBackGround->SetBorderWidth(5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

void SetNormalParams(Rosen::RSNGDotMatrixShader& shader, Drawing::Color dotColor,
    float dotSpacing, float dotRadius, Drawing::Color bgColor)
{
    Vector4f DotColor = {dotColor.GetRedF(), dotColor.GetGreenF(), dotColor.GetBlueF(), dotColor.GetAlphaF()};
    Vector4f BgColor = {bgColor.GetRedF(), bgColor.GetGreenF(), bgColor.GetBlueF(), bgColor.GetAlphaF()};
    shader.Setter<Rosen::DotMatrixShaderDotColorTag>(DotColor);
    shader.Setter<Rosen::DotMatrixShaderDotSpacingTag>(dotSpacing);
    shader.Setter<Rosen::DotMatrixShaderDotRadiusTag>(dotRadius);
    shader.Setter<Rosen::DotMatrixShaderBgColorTag>(BgColor);
    shader.Setter<Rosen::DotMatrixShaderEffectTypeTag>(0);
}

void IniDotMatrixData(std::vector<Drawing::Color>& dotColorList, std::vector<float>& dotRadiusList,
    std::vector<float>& dotSpacingList, std::vector<Drawing::Color>& bgColorList)
{
    dotColorList = { Drawing::Color::COLOR_RED, Drawing::Color::COLOR_RED,
        Drawing::Color::COLOR_RED, Drawing::Color::COLOR_GREEN, Drawing::Color::COLOR_GREEN,
        Drawing::Color::COLOR_GREEN, Drawing::Color::COLOR_BLUE, Drawing::Color::COLOR_BLUE,
        Drawing::Color::COLOR_BLUE };
    dotRadiusList = { -0.7, 0.02, 0.05, 0.1, 0.2, 0.4, 0.6, 1.0, 10.0 };
    dotSpacingList = { -10.0, 10.0, 50.0, 20.0, 40.0, 60.0, 10.0, 50.0, 100.0 };
    bgColorList = { Drawing::Color::COLOR_WHITE, Drawing::Color::COLOR_GRAY,
        Drawing::Color::COLOR_BLACK, Drawing::Color::COLOR_WHITE, Drawing::Color::COLOR_GRAY,
        Drawing::Color::COLOR_BLACK, Drawing::Color::COLOR_WHITE, Drawing::Color::COLOR_GRAY,
        Drawing::Color::COLOR_BLACK };
}

GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, BackGround_DotMatrix_Shader_NoEffect_Test)
{
    int columnCount = 3;
    int rowCount = 3;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<Drawing::Color> dotColorList = {};
    std::vector<float> dotRadiusList = {};
    std::vector<float> dotSpacingList = {};
    std::vector<Drawing::Color> bgColorList = {};
    IniDotMatrixData(dotColorList, dotRadiusList, dotSpacingList, bgColorList);

    for (int i = 0; i < dotColorList.size(); i++) {
        auto dotMatShader = std::make_shared<Rosen::RSNGDotMatrixShader>();
        if (dotMatShader == nullptr) {
            return;
        }
        SetNormalParams(*dotMatShader, dotColorList[i], dotSpacingList[i], dotRadiusList[i], bgColorList[i]);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround = RSCanvasNode::Create();
        testNodeBackGround->SetBounds({ x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetFrame({ x, y, sizeX - 10, sizeY - 10 });
        dotMatShader->Setter<Rosen::DotMatrixShaderProgressTag>(1.0f);
        testNodeBackGround->SetBackgroundNGShader(dotMatShader);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

void SetRotateEffect(Rosen::RSNGDotMatrixShader& shader,
    Rosen::Drawing::DotMatrixDirection pathDirection, std::vector<Drawing::Color> effectColors)
{
    std::vector<Vector4f> EffectColors;
    for (uint32_t i = 0; i < effectColors.size(); i++) {
        Vector4f color = {effectColors[i].GetRedF(), effectColors[i].GetGreenF(),
            effectColors[i].GetBlueF(), effectColors[i].GetAlphaF()};
        EffectColors.emplace_back(color);
    }
    shader.Setter<Rosen::DotMatrixShaderPathDirectionTag>((int)pathDirection);
    shader.Setter<Rosen::DotMatrixShaderEffectColorsTag>(EffectColors);
    shader.Setter<Rosen::DotMatrixShaderEffectTypeTag>(1);
}


GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, BackGround_DotMatrix_Shader_RotateEffect_Test)
{
    int columnCount = 3;
    int rowCount = 3;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<Drawing::Color> dotColorList = {};
    std::vector<float> dotRadiusList = {};
    std::vector<float> dotSpacingList = {};
    std::vector<Drawing::Color> bgColorList = {};
    IniDotMatrixData(dotColorList, dotRadiusList, dotSpacingList, bgColorList);
    std::vector<std::vector<Drawing::Color>> effectColorList = {
        { Drawing::Color::COLOR_YELLOW, Drawing::Color::COLOR_YELLOW, Drawing::Color::COLOR_CYAN,
        Drawing::Color::COLOR_CYAN }, { Drawing::Color::COLOR_MAGENTA, Drawing::Color::COLOR_MAGENTA,
        Drawing::Color::COLOR_YELLOW, Drawing::Color::COLOR_YELLOW }, { Drawing::Color::COLOR_CYAN,
        Drawing::Color::COLOR_CYAN, Drawing::Color::COLOR_BLUE, Drawing::Color::COLOR_BLUE },
        { Drawing::Color::COLOR_RED, Drawing::Color::COLOR_RED, Drawing::Color::COLOR_YELLOW,
        Drawing::Color::COLOR_YELLOW }, { Drawing::Color::COLOR_BLUE, Drawing::Color::COLOR_BLUE,
        Drawing::Color::COLOR_CYAN, Drawing::Color::COLOR_CYAN }, { Drawing::Color::COLOR_CYAN,
        Drawing::Color::COLOR_CYAN, Drawing::Color::COLOR_MAGENTA, Drawing::Color::COLOR_MAGENTA },
        { Drawing::Color::COLOR_RED, Drawing::Color::COLOR_GREEN, Drawing::Color::COLOR_BLUE,
        Drawing::Color::COLOR_WHITE }, { Drawing::Color::COLOR_WHITE, Drawing::Color::COLOR_GRAY,
        Drawing::Color::COLOR_BLACK, Drawing::Color::COLOR_GRAY }, { Drawing::Color::COLOR_BLUE,
        Drawing::Color::COLOR_GREEN, Drawing::Color::COLOR_BLUE, Drawing::Color::COLOR_RED } };
    std::vector<float> progressList = { -18.6, 0.0, 0.1, 0.3, 0.5, 0.7, 0.9, 2.0, 20.0 };

    for (int i = 0; i < dotColorList.size(); i++) {
        auto dotMatShader = std::make_shared<Rosen::RSNGDotMatrixShader>();
        if (dotMatShader == nullptr) {
            return;
        }
        SetNormalParams(*dotMatShader, dotColorList[i], dotSpacingList[i], dotRadiusList[i], bgColorList[i]);
        SetRotateEffect(*dotMatShader, static_cast<Drawing::DotMatrixDirection>(i), effectColorList[i]);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround = RSCanvasNode::Create();
        testNodeBackGround->SetBounds({ x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetFrame({ x, y, sizeX - 10, sizeY - 10 });
        dotMatShader->Setter<Rosen::DotMatrixShaderProgressTag>(progressList[i]);
        testNodeBackGround->SetBackgroundNGShader(dotMatShader);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

void SetRippleEffect(Rosen::RSNGDotMatrixShader& shader,
    std::vector<Drawing::Color> effectColors, Vector2f colorFractions,
    std::vector<Drawing::Point> startPoints, float pathWidth, bool inverseEffect)
{
    std::vector<Vector4f> EffectColors;
    for (uint32_t i = 0; i < effectColors.size(); i++) {
        Vector4f color = {effectColors[i].GetRedF(), effectColors[i].GetGreenF(),
            effectColors[i].GetBlueF(), effectColors[i].GetAlphaF()};
        EffectColors.emplace_back(color);
    }
    std::vector<Vector2f> StartPoints;
    for (uint32_t i = 0; i < startPoints.size(); i++) {
        Vector2f point = {startPoints[i].GetX(), startPoints[i].GetY()};
        StartPoints.emplace_back(point);
    }
    shader.Setter<Rosen::DotMatrixShaderEffectColorsTag>(EffectColors);
    shader.Setter<Rosen::DotMatrixShaderColorFractionsTag>(colorFractions);
    shader.Setter<Rosen::DotMatrixShaderStartPointsTag>(StartPoints);
    shader.Setter<Rosen::DotMatrixShaderPathWidthTag>(pathWidth);
    shader.Setter<Rosen::DotMatrixShaderInverseEffectTag>(inverseEffect);
    shader.Setter<Rosen::DotMatrixShaderEffectTypeTag>(2.0f);
}

GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, BackGround_DotMatrix_Shader_RippleEffect_Test)
{
    int columnCount = 3;
    int rowCount = 3;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<Drawing::Color> dotColorList = {};
    std::vector<float> dotRadiusList = {};
    std::vector<float> dotSpacingList = {};
    std::vector<Drawing::Color> bgColorList = {};
    IniDotMatrixData(dotColorList, dotRadiusList, dotSpacingList, bgColorList);
    std::vector<std::vector<Drawing::Color>> effectColorList = {
        { Drawing::Color::COLOR_YELLOW, Drawing::Color::COLOR_YELLOW, Drawing::Color::COLOR_CYAN,
        Drawing::Color::COLOR_CYAN }, { Drawing::Color::COLOR_MAGENTA, Drawing::Color::COLOR_MAGENTA,
        Drawing::Color::COLOR_YELLOW, Drawing::Color::COLOR_YELLOW }, { Drawing::Color::COLOR_CYAN,
        Drawing::Color::COLOR_CYAN, Drawing::Color::COLOR_BLUE, Drawing::Color::COLOR_BLUE },
        { Drawing::Color::COLOR_RED, Drawing::Color::COLOR_RED, Drawing::Color::COLOR_YELLOW,
        Drawing::Color::COLOR_YELLOW }, { Drawing::Color::COLOR_BLUE, Drawing::Color::COLOR_BLUE,
        Drawing::Color::COLOR_CYAN, Drawing::Color::COLOR_CYAN }, { Drawing::Color::COLOR_CYAN,
        Drawing::Color::COLOR_CYAN, Drawing::Color::COLOR_MAGENTA, Drawing::Color::COLOR_MAGENTA },
        { Drawing::Color::COLOR_RED, Drawing::Color::COLOR_GREEN, Drawing::Color::COLOR_BLUE,
        Drawing::Color::COLOR_WHITE }, { Drawing::Color::COLOR_WHITE, Drawing::Color::COLOR_GRAY,
        Drawing::Color::COLOR_BLACK, Drawing::Color::COLOR_GRAY }, { Drawing::Color::COLOR_BLUE,
        Drawing::Color::COLOR_GREEN, Drawing::Color::COLOR_BLUE, Drawing::Color::COLOR_RED } };
    std::vector<float> progressList = { -18.6, 0.0, 0.1, 0.3, 0.5, 0.7, 0.9, 2.0, 20.0 };

    DotMatrixTestParam testParam;
    testParam.InitRippleEffectParam();
    for (int i = 0; i < columnCount * rowCount; i++) {
        auto dotMatShader = std::make_shared<Rosen::RSNGDotMatrixShader>();
        if (dotMatShader == nullptr) {
            return;
        }
        SetNormalParams(*dotMatShader, dotColorList[i], dotSpacingList[i],
            dotRadiusList[i], bgColorList[i]);
        SetRippleEffect(*dotMatShader, effectColorList[i], testParam.colorFractionsList[i],
            testParam.startPointsList[i], testParam.pathWidthList[i], testParam.inverseEffectList[i]);
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround = RSCanvasNode::Create();
        testNodeBackGround->SetBounds({ x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetFrame({x, y, sizeX - 10, sizeY - 10});
        dotMatShader->Setter<Rosen::DotMatrixShaderProgressTag>(progressList[i]);
        testNodeBackGround->SetBackgroundNGShader(dotMatShader);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}

GRAPHIC_TEST(BackgroundTest, CONTENT_DISPLAY_TEST, BackGround_FlowLightSweep_Shader_Test)
{
    int columnCount = 4;
    int rowCount = 4;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    // create dot color list, the float value range is [0, 1]
    std::vector<std::pair<Drawing::Color, float>> effectColor_1 = { { Drawing::Color::COLOR_RED, 0.0 },
        { Drawing::Color::COLOR_GREEN, 0.2 }, { Drawing::Color::COLOR_BLUE, 0.4 },
        { Drawing::Color::COLOR_YELLOW, 0.6 }, { Drawing::Color::COLOR_CYAN, 0.8 },
        { Drawing::Color::COLOR_MAGENTA, 1.0 } };

    std::vector<std::pair<Drawing::Color, float>> effectColor_2 = { { Drawing::Color::COLOR_RED, -1.0 },
        { Drawing::Color::COLOR_GREEN, 0.2 }, { Drawing::Color::COLOR_BLUE, 0.2 },
        { Drawing::Color::COLOR_YELLOW, 0.5 }, { Drawing::Color::COLOR_CYAN, 0.5 },
        { Drawing::Color::COLOR_MAGENTA, 2.0 } };

    std::vector<std::pair<Drawing::Color, float>> effectColor_3 = { { Drawing::Color::COLOR_BLACK, 1.0 },
        { Drawing::Color::COLOR_GREEN, 0.2 }, { Drawing::Color::COLOR_GRAY, 0.8 },
        { Drawing::Color::COLOR_YELLOW, 0.5 }, { Drawing::Color::COLOR_WHITE, 0.3 },
        { Drawing::Color::COLOR_MAGENTA, 0.7 } };

    std::vector<std::pair<Drawing::Color, float>> effectColor_4 = { { Drawing::Color::COLOR_RED, -10.0 },
        { Drawing::Color::COLOR_GREEN, 0.3 }, { Drawing::Color::COLOR_BLUE, 10.0 } };

    std::vector<std::vector<std::pair<Drawing::Color, float>>> effectColorList = { effectColor_1, effectColor_2,
        effectColor_3, effectColor_4 };

    std::vector<float> progressList = { -10, 0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 10, 0.25, 0.75,
        0.66 };

    for (int i = 0; i < columnCount * rowCount; i++) {
        auto flowLightShader = std::make_shared<Rosen::RSFlowLightSweepShader>(effectColorList[i / columnCount]);
        if (flowLightShader == nullptr) {
            return;
        }
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround = RSCanvasNode::Create();
        testNodeBackGround->SetBounds({ x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBackgroundShader(flowLightShader);
        testNodeBackGround->SetBackgroundShaderProgress(progressList[i % progressList.size()]);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);
    }
}
} // namespace OHOS::Rosen