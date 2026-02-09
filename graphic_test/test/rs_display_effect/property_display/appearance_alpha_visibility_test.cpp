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
    // called before each tests
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }
};

/* SetVisible + Alpha: visible + alpha */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Alpha_Visible_Test_1)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 100, 100, 300, 300 });
    testNode->SetVisible(true);
    testNode->SetAlpha(0.5f);
    testNode->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetVisible + Alpha: invisible + alpha (no effect) */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Alpha_Visible_Test_2)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 100, 100, 300, 300 });
    testNode->SetVisible(false);
    testNode->SetAlpha(0.5f); // alpha has no effect when invisible
    testNode->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetVisible + Alpha: visible + different alpha values */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Alpha_Visible_Test_3)
{
    std::vector<float> alphaList = { 0.0f, 0.3f, 0.6f, 1.0f };

    for (size_t i = 0; i < alphaList.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 280 + 50, 100, 250, 250 });
        testNode->SetVisible(true);
        testNode->SetAlpha(alphaList[i]);
        testNode->SetBackgroundColor(0xff0000ff);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetVisible + Alpha: toggle both properties */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Alpha_Visible_Test_4)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
        { 100, 100, 400, 400 });
    testNode->SetVisible(true);
    testNode->SetAlpha(0.7f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    // Change to invisible
    testNode->SetVisible(false);
}

/* SetVisible + Alpha: parent with alpha, child visibility */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Alpha_Visible_Test_5)
{
    auto parentNode = RSCanvasNode::Create();
    parentNode->SetBounds({ 100, 100, 600, 600 });
    parentNode->SetVisible(true);
    parentNode->SetAlpha(0.5f);
    parentNode->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(parentNode);

    // Child node
    auto childNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
        { 100, 100, 400, 400 });
    childNode->SetVisible(true);
    parentNode->AddChild(childNode);
    RegisterNode(parentNode);
}

/* SetVisible + Alpha: invisible parent with alpha, visible child */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Alpha_Visible_Test_6)
{
    auto parentNode = RSCanvasNode::Create();
    parentNode->SetBounds({ 100, 100, 600, 600 });
    parentNode->SetVisible(false); // parent invisible
    parentNode->SetAlpha(0.5f);
    parentNode->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(parentNode);

    // Child node (won't show even with alpha)
    auto childNode = RSCanvasNode::Create();
    childNode->SetBounds({ 100, 100, 400, 400 });
    childNode->SetVisible(true);
    childNode->SetAlpha(1.0f);
    childNode->SetBackgroundColor(0xff0000ff);
    parentNode->AddChild(childNode);
    RegisterNode(parentNode);
}

/* SetVisible + Alpha: visible node with alpha gradient */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Alpha_Visible_Test_7)
{
    for (int i = 0; i < 4; i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            { (i % 2) * 500 + 100, (i / 2) * 500 + 100, 400, 400 });
        testNode->SetVisible(true);
        testNode->SetAlpha(0.25f * (i + 1));
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetVisible + Alpha: border with visibility and alpha */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Alpha_Visible_Test_8)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 100, 100, 400, 400 });
    testNode->SetVisible(true);
    testNode->SetAlpha(0.7f);
    testNode->SetBorderWidth(30);
    testNode->SetBorderColor(0xff00ff00);
    testNode->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetVisible + Alpha: shadow with visibility and alpha */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Alpha_Visible_Test_9)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 100, 100, 400, 400 });
    testNode->SetVisible(true);
    testNode->SetAlpha(0.8f);
    testNode->SetShadowColor(0xff000000);
    testNode->SetShadowRadius(40);
    testNode->SetShadowOffset(30, 30);
    testNode->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetVisible + Alpha: change alpha while visible */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Alpha_Visible_Test_10)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
        { 100, 100, 400, 400 });
    testNode->SetVisible(true);
    testNode->SetAlpha(0.9f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    // Update alpha
    testNode->SetAlpha(0.3f);
}

/* SetVisible + Alpha: change visibility with alpha */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Alpha_Visible_Test_11)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 100, 100, 400, 400 });
    testNode->SetVisible(true);
    testNode->SetAlpha(0.6f);
    testNode->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    // Change to invisible
    testNode->SetVisible(false);
}

/* SetVisible + Alpha: multiple children with mixed properties */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Alpha_Visible_Test_12)
{
    auto parentNode = RSCanvasNode::Create();
    parentNode->SetBounds({ 100, 100, 800, 800 });
    parentNode->SetVisible(true);
    parentNode->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(parentNode);

    // 4 children with different combinations
    for (int i = 0; i < 4; i++) {
        auto childNode = RSCanvasNode::Create();
        int x = (i % 2) * 350;
        int y = (i / 2) * 350;
        childNode->SetBounds({ x, y, 300, 300 });
        childNode->SetVisible(i < 3); // last one invisible
        childNode->SetAlpha(0.3f + i * 0.2f);
        childNode->SetBackgroundColor(0xff0000ff);
        parentNode->AddChild(childNode);
    }
    RegisterNode(parentNode);
}

/* SetVisible + Alpha: boundary values - alpha = 0 (完全透明) */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Alpha_Visible_Boundary_Test_01)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 100, 100, 400, 400 });
    testNode->SetVisible(true);
    testNode->SetAlpha(0.0f); // 边界值：完全透明
    testNode->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetVisible + Alpha: boundary values - alpha = 1 (完全不透明) */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Alpha_Visible_Boundary_Test_02)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 100, 100, 400, 400 });
    testNode->SetVisible(true);
    testNode->SetAlpha(1.0f); // 边界值：完全不透明
    testNode->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetVisible + Alpha: abnormal values - alpha < 0 (负值) */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Alpha_Visible_Abnormal_Test_01)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 100, 100, 400, 400 });
    testNode->SetVisible(true);
    testNode->SetAlpha(-0.5f); // 异常值：负值alpha
    testNode->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetVisible + Alpha: abnormal values - alpha > 1 (超限值) */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Alpha_Visible_Abnormal_Test_02)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 100, 100, 400, 400 });
    testNode->SetVisible(true);
    testNode->SetAlpha(1.5f); // 异常值：超限值alpha
    testNode->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetVisible + Alpha: matrix test 4x4 (visibility x alpha) */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Alpha_Visible_Matrix_4x4)
{
    std::vector<bool> visibilityList = { true, false, true, false };
    std::vector<float> alphaList = { 0.0f, 0.3f, 0.7f, 1.0f };

    for (size_t row = 0; row < visibilityList.size(); row++) {
        for (size_t col = 0; col < alphaList.size(); col++) {
            auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
                { (int)col * 280 + 50, (int)row * 280 + 50, 250, 250 });
            testNode->SetVisible(visibilityList[row]);
            testNode->SetAlpha(alphaList[col]);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/* SetVisible + Alpha: conflict scenario - invisible with high alpha */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Alpha_Visible_Conflict_Test_01)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 100, 100, 400, 400 });
    testNode->SetVisible(false); // 冲突：不可见但设置了高alpha
    testNode->SetAlpha(0.9f); // alpha应该无效
    testNode->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetVisible + Alpha: conflict scenario - visible with zero alpha */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Alpha_Visible_Conflict_Test_02)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 100, 100, 400, 400 });
    testNode->SetVisible(true); // 冲突：可见但完全透明
    testNode->SetAlpha(0.0f); // 完全透明，内容不可见
    testNode->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetVisible + Alpha: extreme value combination */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Alpha_Visible_Extreme_Test)
{
    std::vector<float> extremeAlphas = { -10.0f, -1.0f, 0.0f, 0.001f, 0.999f, 1.0f, 1.001f, 10.0f };

    for (size_t i = 0; i < extremeAlphas.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 140 + 50, 100, 130, 130 });
        testNode->SetVisible(true);
        testNode->SetAlpha(extremeAlphas[i]);
        testNode->SetBackgroundColor(0xffff0000);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetVisible + Alpha: parent-child alpha blend */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Alpha_Visible_ParentChild_Test)
{
    // 4x4 matrix: parent alpha x child alpha
    std::vector<float> parentAlphas = { 0.25f, 0.5f, 0.75f, 1.0f };
    std::vector<float> childAlphas = { 0.25f, 0.5f, 0.75f, 1.0f };

    for (size_t row = 0; row < parentAlphas.size(); row++) {
        for (size_t col = 0; col < childAlphas.size(); col++) {
            auto parentNode = RSCanvasNode::Create();
            parentNode->SetBounds({ (int)col * 280 + 50, (int)row * 280 + 50, 250, 250 });
            parentNode->SetVisible(true);
            parentNode->SetAlpha(parentAlphas[row]);
            parentNode->SetBackgroundColor(0xffff0000);
            GetRootNode()->AddChild(parentNode);

            auto childNode = RSCanvasNode::Create();
            childNode->SetBounds({ 50, 50, 150, 150 });
            childNode->SetVisible(true);
            childNode->SetAlpha(childAlphas[col]);
            childNode->SetBackgroundColor(0xff0000ff);
            parentNode->AddChild(childNode);
            RegisterNode(parentNode);
        }
    }
}

} // namespace OHOS::Rosen
