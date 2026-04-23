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

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class AppearanceTest : public RSGraphicTest {
private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;
    static constexpr float lightIntensityDefault = 1.0f;
    static constexpr float lightHeightDefault = 50.0f;
    static constexpr uint32_t colorRed = 0xffff0000;
    static constexpr uint32_t colorGreen = 0xff00ff00;
    static constexpr uint32_t colorBlue = 0xff0000ff;
    static constexpr uint32_t colorYellow = 0xffffff00;
    static constexpr uint32_t colorCyan = 0xff00ffff;
    static constexpr uint32_t colorMagenta = 0xffff00ff;
    static constexpr uint32_t colorGray = 0xff666666;
    static constexpr uint32_t colorDarkGray = 0xff333333;

public:
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }
};

/* Basic Test: single node */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_PointLight_Test_1)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 100, 100, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* Basic Test: different node positions */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_PointLight_Test_2)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 100, 100, 600, 600 });

    std::vector<std::pair<int, int>> positions = {
        { 100, 100 }, { 400, 100 }, { 100, 400 }, { 400, 400 }
    };

    for (const auto& pos : positions) {
        auto childNode = RSCanvasNode::Create();
        childNode->SetBounds({ pos.first, pos.second, 200, 200 });
        childNode->SetBackgroundColor(0xffffffff);
        testNode->AddChild(childNode);
    }
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* Basic Test: different node sizes */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_PointLight_Test_3)
{
    std::vector<int> radii = { 100, 200, 300, 400 };

    for (size_t i = 0; i < radii.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 280 + 50, 100, 250, 250 });
        testNode->SetBorderWidth((int)radii[i] / 20);
        testNode->SetBorderColor(0xff00ff00);
        testNode->SetBackgroundColor(0xffff0000);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* Basic Test: different node colors */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_PointLight_Test_4)
{
    uint32_t colorList[] = { 0xffff0000, 0xff00ff00, 0xff0000ff, 0xffffffff };

    for (int i = 0; i < 4; i++) {
        auto testNode = RSCanvasNode::Create();
        int x = (i % 2) * 450;
        int y = (i / 2) * 450;
        testNode->SetBounds({ x + 100, y + 100, 350, 350 });
        testNode->SetBackgroundColor(colorList[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* Basic Test: multiple nodes */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_PointLight_Test_5)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 100, 100, 800, 800 });

    // Add multiple child nodes
    for (int i = 0; i < 4; i++) {
        auto childNode = RSCanvasNode::Create();
        int x = (i % 2) * 400;
        int y = (i / 2) * 400;
        childNode->SetBounds({ x + 50, y + 50, 300, 300 });
        childNode->SetBackgroundColor(0xffffffff);
        testNode->AddChild(childNode);
    }

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* Basic Test: node with border */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_PointLight_Test_6)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 100, 100, 500, 500 });
    testNode->SetBorderWidth(30);
    testNode->SetBorderColor(0xff00ff00);
    testNode->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* Basic Test: node with shadow */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_PointLight_Test_7)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 100, 100, 500, 500 });
    testNode->SetShadowColor(0xff000000);
    testNode->SetShadowRadius(40);
    testNode->SetShadowOffset(30, 30);
    testNode->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* Basic Test: zero border */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_PointLight_Test_8)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 100, 100, 400, 400 });
    testNode->SetBorderWidth(0);
    testNode->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* Basic Test: extreme border values */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_PointLight_Extreme_Border)
{
    std::vector<int> extremeBorders = { -10, 0, 1, 50, 100, 200, 500 };

    for (size_t i = 0; i < extremeBorders.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 160 + 50, 100, 150, 150 });
        testNode->SetBorderWidth(extremeBorders[i]);
        testNode->SetBorderColor(0xff00ff00);
        testNode->SetBackgroundColor(0xffff0000);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* Basic Test: extreme positions */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_PointLight_Extreme_Position)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 100, 100, 600, 600 });

    // Extreme positions for child nodes
    std::vector<std::pair<int, int>> positions = {
        { -200, -200 }, { 700, 700 }, { -200, 700 }, { 700, -200 }
    };

    for (const auto& pos : positions) {
        auto childNode = RSCanvasNode::Create();
        childNode->SetBounds({ pos.first, pos.second, 100, 100 });
        childNode->SetBackgroundColor(0xffffffff);
        testNode->AddChild(childNode);
    }
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* Basic Test: matrix test - border width x color (5x4) */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_PointLight_Matrix_5x4)
{
    std::vector<int> borderWidths = { 0, 10, 20, 40, 80 };
    uint32_t colors[] = { 0xffff0000, 0xff00ff00, 0xff0000ff, 0xffffffff };

    for (size_t row = 0; row < borderWidths.size(); row++) {
        for (size_t col = 0; col < 4; col++) {
            auto testNode = RSCanvasNode::Create();
            testNode->SetBounds({ (int)col * 280 + 50, (int)row * 180 + 50, 250, 130 });
            testNode->SetBorderWidth(borderWidths[row]);
            testNode->SetBorderColor(colors[col]);
            testNode->SetBackgroundColor(0xffff0000);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/* Basic Test: multiple child nodes at different positions */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_PointLight_Edge_Positions)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 100, 100, 600, 600 });

    // Child nodes at corners and center
    std::vector<std::pair<int, int>> positions = {
        { 0, 0 },         { 400, 0 },       { 0, 400 },       { 400, 400 },     { 200, 200 }
    };

    for (const auto& pos : positions) {
        auto childNode = RSCanvasNode::Create();
        childNode->SetBounds({ pos.first, pos.second, 200, 200 });
        childNode->SetBackgroundColor(0xffffffff);
        testNode->AddChild(childNode);
    }

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* Boundary Test: extreme light positions */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_PointLight_Extreme_Positions)
{
    std::vector<std::tuple<float, float, float>> lightPositions = { { 0.0f, 0.0f, 10.0f }, { 100.0f, 100.0f, 10.0f },
        { 500.0f, 500.0f, lightHeightDefault }, { 1000.0f, 1000.0f, 100.0f },
        { -lightHeightDefault, -lightHeightDefault, 20.0f } };

    for (size_t i = 0; i < lightPositions.size(); i++) {
        auto lightSource = RSCanvasNode::Create();
        lightSource->SetBounds({ 100, (int)i * 350 + 100, 100, 100 });
        lightSource->SetLightPosition(
            std::get<0>(lightPositions[i]), std::get<1>(lightPositions[i]), std::get<2>(lightPositions[i]));
        lightSource->SetLightIntensity(10.f);
        lightSource->SetLightColor(colorYellow);

        lightSource->SetBackgroundColor(colorDarkGray);
        lightSource->SetIlluminatedType(static_cast<uint32_t>(IlluminatedType::CONTENT));
        lightSource->SetIlluminatedBorderWidth(40.0f);
        GetRootNode()->AddChild(lightSource);
        RegisterNode(lightSource);
    }
}

/* Boundary Test: extreme light intensities */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_PointLight_Extreme_Intensities)
{
    std::vector<float> intensities = { 0.0f, 0.1f, 0.5f, lightIntensityDefault, 2.0f, 5.0f };

    for (size_t i = 0; i < intensities.size(); i++) {
        auto lightSource = RSCanvasNode::Create();
        lightSource->SetBounds({ 100, (int)i * 300 + 100, 100, 100 });
        lightSource->SetLightPosition(lightHeightDefault, lightHeightDefault, lightHeightDefault);
        lightSource->SetLightIntensity(intensities[i]);
        lightSource->SetLightColor(colorYellow);

        lightSource->SetBackgroundColor(colorRed);
        lightSource->SetIlluminatedType(static_cast<uint32_t>(IlluminatedType::CONTENT));
        lightSource->SetIlluminatedBorderWidth(20.0f);
        GetRootNode()->AddChild(lightSource);
        RegisterNode(lightSource);
    }
}

/* Test: multiple light sources illuminating single node */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_PointLight_Multiple_Sources)
{
    std::vector<std::tuple<float, float, float, uint32_t>> lightParams = {
        { 0.0f, 0.0f, lightHeightDefault, colorRed }, { 400.0f, 0.0f, lightHeightDefault, colorGreen },
        { 0.0f, 400.0f, lightHeightDefault, colorBlue }, { 400.0f, 400.0f, lightHeightDefault, colorYellow }
    };

    for (const auto& params : lightParams) {
        auto lightSource = RSCanvasNode::Create();
        lightSource->SetBounds({ std::get<0>(params), std::get<1>(params), 100, 100 });
        lightSource->SetBackgroundColor(colorDarkGray);
        lightSource->SetIlluminatedType(static_cast<uint32_t>(IlluminatedType::CONTENT));
        lightSource->SetIlluminatedBorderWidth(30.0f);

        lightSource->SetLightPosition(lightHeightDefault, lightHeightDefault, std::get<2>(params));
        lightSource->SetLightIntensity(lightIntensityDefault);
        lightSource->SetLightColor(std::get<3>(params));
        GetRootNode()->AddChild(lightSource);
        RegisterNode(lightSource);
    }
}

/* Test: single light source illuminating multiple nodes */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_PointLight_Multiple_Illuminated)
{
    std::vector<std::tuple<int, int, uint32_t>> illuminatedParams = { { 100, 100, colorRed },
        { 500, 100, colorGreen }, { 100, 500, colorBlue }, { 500, 500, colorYellow } };

    for (const auto& params : illuminatedParams) {
        auto lightSource = RSCanvasNode::Create();
        lightSource->SetBounds({ std::get<0>(params), std::get<1>(params), 200, 200 });
        lightSource->SetLightPosition(100.0f, 100.0f, 100.0f);
        lightSource->SetLightIntensity(lightIntensityDefault);
        lightSource->SetLightColor(colorYellow);

        lightSource->SetBackgroundColor(std::get<2>(params));
        lightSource->SetIlluminatedType(static_cast<uint32_t>(IlluminatedType::CONTENT));
        lightSource->SetIlluminatedBorderWidth(25.0f);
        GetRootNode()->AddChild(lightSource);
        RegisterNode(lightSource);
    }
}

/* Test: different lightSource types */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_PointLight_Different_Types)
{
    std::vector<uint32_t> illuminatedTypes = { 2, 3, 4, 5, 6 };

    for (size_t i = 0; i < illuminatedTypes.size(); i++) {
        auto lightSource = RSCanvasNode::Create();
        int x = (i % 3) * 350 + 100;
        int y = (i / 3) * 400 + 100;
        lightSource->SetBounds({ x, y, 250, 250 });
        lightSource->SetLightPosition(100.0f, 100.0f, 80.0f);
        lightSource->SetLightIntensity(30.f);
        lightSource->SetLightColor(colorYellow);

        lightSource->SetBackgroundColor(colorDarkGray);
        lightSource->SetIlluminatedType(illuminatedTypes[i]);
        lightSource->SetIlluminatedBorderWidth(20.0f);
        GetRootNode()->AddChild(lightSource);
        RegisterNode(lightSource);
    }
}

/* Test: varying lightSource border widths */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_PointLight_Varying_Border_Widths)
{
    std::vector<float> borderWidths = { 5.0f, 15.0f, 30.0f, lightHeightDefault, 80.0f };

    for (size_t i = 0; i < borderWidths.size(); i++) {
        auto lightSource = RSCanvasNode::Create();
        int x = (i % 3) * 350 + 100;
        int y = (i / 3) * 400 + 100;
        lightSource->SetBounds({ x, y, 250, 250 });
        lightSource->SetLightPosition(100.0f, 100.0f, 80.0f);
        lightSource->SetLightIntensity(lightIntensityDefault);
        lightSource->SetLightColor(colorYellow);

        lightSource->SetBackgroundColor(colorRed);
        lightSource->SetIlluminatedType(static_cast<uint32_t>(IlluminatedType::CONTENT));
        lightSource->SetIlluminatedBorderWidth(borderWidths[i]);
        GetRootNode()->AddChild(lightSource);
        RegisterNode(lightSource);
    }
}

/* Test: light source at different heights */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_PointLight_Different_Heights)
{
    std::vector<float> heights = { 10.0f, 30.0f, lightHeightDefault, 80.0f, 120.0f, 200.0f };

    for (size_t i = 0; i < heights.size(); i++) {
        auto lightSource = RSCanvasNode::Create();
        lightSource->SetBounds({ 100, (int)i * 280 + 100, 100, 100 });
        lightSource->SetLightPosition(lightHeightDefault, lightHeightDefault, heights[i]);
        lightSource->SetLightIntensity(lightIntensityDefault);
        lightSource->SetLightColor(colorYellow);

        lightSource->SetBackgroundColor(colorDarkGray);
        lightSource->SetIlluminatedType(static_cast<uint32_t>(IlluminatedType::CONTENT));
        lightSource->SetIlluminatedBorderWidth(20.0f);
        GetRootNode()->AddChild(lightSource);
        RegisterNode(lightSource);
    }
}

/* Test: different light colors */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_PointLight_Different_Colors)
{
    std::vector<uint32_t> lightColors = { colorRed, colorGreen, colorBlue, colorYellow, colorCyan, colorMagenta };

    for (size_t i = 0; i < lightColors.size(); i++) {
        auto lightSource = RSCanvasNode::Create();
        lightSource->SetBounds({ 100, (int)i * 280 + 100, 100, 100 });
        lightSource->SetLightPosition(lightHeightDefault, lightHeightDefault, 60.0f);
        lightSource->SetLightIntensity(lightIntensityDefault);
        lightSource->SetLightColor(lightColors[i]);

        lightSource->SetBackgroundColor(colorDarkGray);
        lightSource->SetIlluminatedType(static_cast<uint32_t>(IlluminatedType::CONTENT));
        lightSource->SetIlluminatedBorderWidth(25.0f);
        GetRootNode()->AddChild(lightSource);
        RegisterNode(lightSource);
    }
}

/* Test: light source and lightSource node overlap */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_PointLight_Overlap)
{
    auto lightSource = RSCanvasNode::Create();
    lightSource->SetBounds({ 400, 400, 200, 200 });
    lightSource->SetLightPosition(100.0f, 100.0f, 80.0f);
    lightSource->SetLightIntensity(lightIntensityDefault);
    lightSource->SetLightColor(colorYellow);

    lightSource->SetBackgroundColor(colorRed);
    lightSource->SetIlluminatedType(static_cast<uint32_t>(IlluminatedType::CONTENT));
    lightSource->SetIlluminatedBorderWidth(30.0f);
    GetRootNode()->AddChild(lightSource);
    RegisterNode(lightSource);
}

/* Test: complex scene with multiple lights and lightSource nodes */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_PointLight_Complex_Scene)
{
    std::vector<std::tuple<float, float, float, uint32_t>> lightParams = { { 200.0f, 200.0f, 60.0f, colorRed },
        { 800.0f, 200.0f, 60.0f, colorGreen }, { 200.0f, 800.0f, 60.0f, colorBlue },
        { 800.0f, 800.0f, 60.0f, colorYellow } };

    std::vector<std::tuple<int, int, uint32_t>> illuminatedParams = { { 350, 350, colorGray },
        { 650, 350, colorGray }, { 350, 650, colorGray }, { 650, 650, colorGray } };

    for (const auto& params : lightParams) {
        auto lightSource = RSCanvasNode::Create();
        lightSource->SetBounds({ std::get<0>(params) - 50, std::get<1>(params) - 50, 100, 100 });
        lightSource->SetLightPosition(lightHeightDefault, lightHeightDefault, std::get<2>(params));
        lightSource->SetLightIntensity(lightIntensityDefault);
        lightSource->SetLightColor(std::get<3>(params));

        lightSource->SetBackgroundColor(colorDarkGray);
        lightSource->SetIlluminatedType(static_cast<uint32_t>(IlluminatedType::CONTENT));
        lightSource->SetIlluminatedBorderWidth(25.0f);
        GetRootNode()->AddChild(lightSource);
        RegisterNode(lightSource);
    }
}
} // namespace OHOS::Rosen
