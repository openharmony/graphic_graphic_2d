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

} // namespace OHOS::Rosen
