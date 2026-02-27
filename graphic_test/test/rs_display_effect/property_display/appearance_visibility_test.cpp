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

/* SetVisible: visible = true */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Visible_Test_1)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 100, 100, 300, 300 });
    testNode->SetVisible(true);
    testNode->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetVisible: visible = false */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Visible_Test_2)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 100, 100, 300, 300 });
    testNode->SetVisible(false); // node not visible
    testNode->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetVisible: multiple nodes with different visibility */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Visible_Test_3)
{
    // Visible node
    auto testNode1 = RSCanvasNode::Create();
    testNode1->SetBounds({ 100, 100, 300, 300 });
    testNode1->SetVisible(true);
    testNode1->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(testNode1);
    RegisterNode(testNode1);

    // Invisible node
    auto testNode2 = RSCanvasNode::Create();
    testNode2->SetBounds({ 500, 100, 300, 300 });
    testNode2->SetVisible(false);
    testNode2->SetBackgroundColor(0xff00ff00);
    GetRootNode()->AddChild(testNode2);
    RegisterNode(testNode2);
}

/* SetVisible: toggle visibility */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Visible_Test_4)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
        { 100, 100, 400, 400 });
    testNode->SetVisible(true);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    // Toggle to invisible
    testNode->SetVisible(false);
}

/* SetVisible: child visibility when parent is visible */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Visible_Test_5)
{
    auto parentNode = RSCanvasNode::Create();
    parentNode->SetBounds({ 100, 100, 600, 600 });
    parentNode->SetVisible(true);
    parentNode->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(parentNode);

    // Child node
    auto childNode = RSCanvasNode::Create();
    childNode->SetBounds({ 50, 50, 200, 200 });
    childNode->SetVisible(true);
    childNode->SetBackgroundColor(0xff0000ff);
    parentNode->AddChild(childNode);
    RegisterNode(parentNode);
}

/* SetVisible: child visibility when parent is invisible */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Visible_Test_6)
{
    auto parentNode = RSCanvasNode::Create();
    parentNode->SetBounds({ 100, 100, 600, 600 });
    parentNode->SetVisible(false); // parent invisible
    parentNode->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(parentNode);

    // Child node (even if visible, won't show because parent is invisible)
    auto childNode = RSCanvasNode::Create();
    childNode->SetBounds({ 50, 50, 200, 200 });
    childNode->SetVisible(true);
    childNode->SetBackgroundColor(0xff0000ff);
    parentNode->AddChild(childNode);
    RegisterNode(parentNode);
}

/* SetVisible: image node visibility */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Visible_Test_7)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
        { 100, 100, 400, 400 });
    testNode->SetVisible(true);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetVisible: multiple children with mixed visibility */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Visible_Test_8)
{
    auto parentNode = RSCanvasNode::Create();
    parentNode->SetBounds({ 100, 100, 600, 600 });
    parentNode->SetVisible(true);
    parentNode->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(parentNode);

    // Create 4 children with alternating visibility
    for (int i = 0; i < 4; i++) {
        auto childNode = RSCanvasNode::Create();
        int x = (i % 2) * 250;
        int y = (i / 2) * 250;
        childNode->SetBounds({ x, y, 200, 200 });
        childNode->SetVisible(i % 2 == 0); // alternating visibility
        childNode->SetBackgroundColor(0xff0000ff);
        parentNode->AddChild(childNode);
    }
    RegisterNode(parentNode);
}

/* SetVisible: node with border and visibility */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Visible_Test_9)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 100, 100, 400, 400 });
    testNode->SetVisible(true);
    testNode->SetBorderWidth(20);
    testNode->SetBorderColor(0xff00ff00);
    testNode->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetVisible: node with shadow and visibility */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Visible_Test_10)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 100, 100, 400, 400 });
    testNode->SetVisible(true);
    testNode->SetShadowColor(0xff000000);
    testNode->SetShadowRadius(30);
    testNode->SetShadowOffset(20, 20);
    testNode->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* SetVisible: toggle visibility multiple times */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Visible_Toggle_Test)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
        { 100, 100, 400, 400 });
    testNode->SetVisible(true);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    // Toggle multiple times
    testNode->SetVisible(false);
    testNode->SetVisible(true);
    testNode->SetVisible(false);
}

/* SetVisible: nested visibility hierarchy */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Visible_Nested_Test)
{
    // Root - visible
    auto root = RSCanvasNode::Create();
    root->SetBounds({ 100, 100, 900, 900 });
    root->SetVisible(true);
    root->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(root);

    // Level 1 - mixed visibility
    for (int i = 0; i < 2; i++) {
        auto l1 = RSCanvasNode::Create();
        l1->SetBounds({ i * 400, 50, 350, 350 });
        l1->SetVisible(i == 0); // first visible, second invisible
        l1->SetBackgroundColor(0xff00ff00);
        root->AddChild(l1);

        // Level 2 - under both
        for (int j = 0; j < 2; j++) {
            auto l2 = RSCanvasNode::Create();
            l2->SetBounds({ j * 150, 50, 100, 100 });
            l2->SetVisible(true);
            l2->SetBackgroundColor(0xff0000ff);
            l1->AddChild(l2);
        }
    }
    RegisterNode(root);
}

/* SetVisible: visibility with different content types */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Visible_Content_Types_Test)
{
    // Image node
    auto imgNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
        { 100, 100, 300, 300 });
    imgNode->SetVisible(true);
    GetRootNode()->AddChild(imgNode);
    RegisterNode(imgNode);

    // Color node
    auto colorNode = RSCanvasNode::Create();
    colorNode->SetBounds({ 450, 100, 300, 300 });
    colorNode->SetVisible(true);
    colorNode->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(colorNode);
    RegisterNode(colorNode);

    // Border node
    auto borderNode = RSCanvasNode::Create();
    borderNode->SetBounds({ 100, 450, 300, 300 });
    borderNode->SetVisible(true);
    borderNode->SetBorderWidth(20);
    borderNode->SetBorderColor(0xff00ff00);
    borderNode->SetBackgroundColor(0xff0000ff);
    GetRootNode()->AddChild(borderNode);
    RegisterNode(borderNode);

    // Invisible node
    auto invNode = RSCanvasNode::Create();
    invNode->SetBounds({ 450, 450, 300, 300 });
    invNode->SetVisible(false);
    invNode->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(invNode);
    RegisterNode(invNode);
}

/* SetVisible: extreme bounds with visibility */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Visible_Extreme_Bounds_Test)
{
    Vector4f extremeBounds[] = {
        { 0, 0, 1, 1 },           // 最小尺寸
        { 0, 0, 1200, 2000 },      // 全屏
        { -100, -100, 200, 200 },  // 负位置
        { 1100, 1900, 200, 200 }   // 边缘位置
    };

    for (int i = 0; i < 4; i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ extremeBounds[i].x_, extremeBounds[i].y_,
            extremeBounds[i].z_, extremeBounds[i].w_ });
        testNode->SetVisible(i < 3); // 最后一个不可见
        testNode->SetBackgroundColor(0xffff0000);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetVisible: rapid visibility changes */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Visible_Rapid_Change_Test)
{
    std::vector<std::shared_ptr<RSCanvasNode>> nodes;
    for (int i = 0; i < 10; i++) {
        auto node = RSCanvasNode::Create();
        node->SetBounds({ i * 110 + 50, 100, 100, 100 });
        node->SetVisible(true);
        node->SetBackgroundColor(0xffff0000);
        GetRootNode()->AddChild(node);
        RegisterNode(node);
        nodes.push_back(node);
    }

    // Rapid toggle
    for (auto& node : nodes) {
        node->SetVisible(false);
    }
    for (auto& node : nodes) {
        node->SetVisible(true);
    }
}

} // namespace OHOS::Rosen
