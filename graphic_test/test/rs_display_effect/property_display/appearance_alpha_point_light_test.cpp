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

/* Alpha Test: alpha = 0.5 */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Alpha_PointLight_Test_1)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 100, 100, 400, 400 });
    testNode->SetAlpha(0.5f);
    testNode->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* Alpha Test: different alpha values */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Alpha_PointLight_Test_2)
{
    std::vector<float> alphaList = { 0.3f, 0.5f, 0.7f, 0.9f };

    for (size_t i = 0; i < alphaList.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            { (int)i * 260 + 50, 100, 250, 250 });
        testNode->SetAlpha(alphaList[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* Alpha Test: with border */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Alpha_PointLight_Test_3)
{
    std::vector<float> radii = { 150, 250, 350 };

    for (size_t i = 0; i < radii.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 350 + 50, 100, 300, 300 });
        testNode->SetAlpha(0.6f);
        testNode->SetBorderWidth((int)radii[i] / 10);
        testNode->SetBorderColor(0xff00ff00);
        testNode->SetBackgroundColor(0xffff0000);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* Alpha Test: multiple nodes with alpha */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Alpha_PointLight_Test_4)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 100, 100, 600, 600 });
    testNode->SetAlpha(0.7f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    // Add multiple child nodes to simulate multiple light sources
    for (int i = 0; i < 4; i++) {
        auto childNode = RSCanvasNode::Create();
        int x = (i % 2) * 300;
        int y = (i / 2) * 300;
        childNode->SetBounds({ x, y, 200, 200 });
        childNode->SetAlpha(0.8f);
        childNode->SetBackgroundColor(0xffffffff);
        testNode->AddChild(childNode);
    }
    RegisterNode(testNode);
}

/* Alpha Test: change alpha dynamically */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Alpha_PointLight_Test_5)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 100, 100, 500, 500 });
    testNode->SetAlpha(0.8f);
    testNode->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    testNode->SetAlpha(0.4f);
}

/* Alpha Test: boundary values - alpha = 0 */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Alpha_PointLight_Boundary_01)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 100, 100, 400, 400 });
    testNode->SetAlpha(0.0f);
    testNode->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* Alpha Test: boundary values - alpha = 1 */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Alpha_PointLight_Boundary_02)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 100, 100, 400, 400 });
    testNode->SetAlpha(1.0f);
    testNode->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* Alpha Test: extreme radius values (using border width) */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Alpha_PointLight_Extreme_Radius)
{
    std::vector<int> borderWidths = { 0, 10, 20, 40, 80, 160 };

    for (size_t i = 0; i < borderWidths.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 180 + 50, 400, 150, 150 });
        testNode->SetAlpha(0.7f);
        testNode->SetBorderWidth(borderWidths[i]);
        testNode->SetBorderColor(0xff00ff00);
        testNode->SetBackgroundColor(0xffff0000);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* Alpha Test: matrix test - alpha x border (4x3) */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Alpha_PointLight_Matrix_4x3)
{
    std::vector<float> alphaList = { 0.3f, 0.5f, 0.7f, 1.0f };
    std::vector<int> borderWidths = { 0, 20, 40 };

    for (size_t row = 0; row < alphaList.size(); row++) {
        for (size_t col = 0; col < borderWidths.size(); col++) {
            auto testNode = RSCanvasNode::Create();
            testNode->SetBounds({ (int)col * 380 + 50, (int)row * 280 + 50, 330, 230 });
            testNode->SetAlpha(alphaList[row]);
            testNode->SetBorderWidth(borderWidths[col]);
            testNode->SetBorderColor(0xff00ff00);
            testNode->SetBackgroundColor(0xffff0000);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/* Alpha Test: light position matrix (using child nodes) */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Alpha_PointLight_Position_Matrix)
{
    auto parentNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 100, 100, 800, 800 });
    parentNode->SetAlpha(0.8f);
    GetRootNode()->AddChild(parentNode);

    // 3x3 grid of child nodes to simulate light positions
    for (int x = 0; x < 3; x++) {
        for (int y = 0; y < 3; y++) {
            auto childNode = RSCanvasNode::Create();
            childNode->SetBounds({ 200 + x * 200, 200 + y * 200, 150, 150 });
            childNode->SetAlpha(0.9f);
            childNode->SetBackgroundColor(0xffffffff);
            parentNode->AddChild(childNode);
        }
    }
    RegisterNode(parentNode);
}

/* Alpha Test: abnormal alpha values */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Alpha_PointLight_Abnormal_Alpha)
{
    std::vector<float> abnormalAlphas = { -1.0f, -0.5f, 0.0f, 1.0f, 1.5f, 2.0f };

    for (size_t i = 0; i < abnormalAlphas.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 190 + 50, 700, 180, 180 });
        testNode->SetAlpha(abnormalAlphas[i]);
        testNode->SetBackgroundColor(0xffff0000);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* Alpha Test: zero border with various alpha */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Alpha_PointLight_Zero_Radius)
{
    std::vector<float> alphaList = { 0.2f, 0.5f, 0.8f };

    for (size_t i = 0; i < alphaList.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 350 + 50, 1000, 300, 300 });
        testNode->SetAlpha(alphaList[i]);
        testNode->SetBorderWidth(0);
        testNode->SetBackgroundColor(0xffff0000);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

} // namespace OHOS::Rosen
