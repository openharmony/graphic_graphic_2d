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

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class AppearanceTest03 : public RSGraphicTest {
private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;

public:
    // called before each tests
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }

    void setNode(std::shared_ptr<RSCanvasNode>& node, const Vector4f& bounds, Vector4<Color> outLineColor)
    {
        Vector4<BorderStyle> style = Vector4<BorderStyle>(BorderStyle::SOLID);
        node->SetBounds(bounds);
        int transXY = 20;
        int transZ = 0;
        node->SetTranslate(transXY, transXY, transZ);
        node->SetOutlineStyle(style);
        Vector4f borderWidth = { 5, 5, 5, 5 };
        node->SetOutlineWidth(borderWidth);
        // set radius
        Vector4f radius = { 50.0, 50.0, 50.0, 50.0 };
        node->SetOutlineRadius(radius);
        node->SetOutlineColor(outLineColor);
    };
};

// color
GRAPHIC_TEST(AppearanceTest03, CONTENT_DISPLAY_TEST, Appearance_OutlineBorder_Radius_Color_Test_1)
{
    Color colorList[] = { Color(0, 0, 0), Color(255, 0, 0), Color(0, 255, 0), Color(0, 0, 255) };

    const int nodeCount = 4;
    const int columnCount = 2;
    const int nodeSize = 400;
    const int nodeHalfSize = 200;
    const int nodePos = 420;

    for (int i = 0; i < nodeCount; i++) {
        int x = (i % columnCount) * nodePos;
        int y = (i / columnCount) * nodePos;
        auto testNodeColor = RSCanvasNode::Create();
        setNode(testNodeColor, { x, y, nodeSize, nodeSize }, Vector4<Color>(colorList[i]));
        GetRootNode()->AddChild(testNodeColor);
        RegisterNode(testNodeColor);
    }

    // parent black, child red color, white border
    auto testNodeParent = RSCanvasNode::Create();
    setNode(testNodeParent, { 0, nodePos * 2, nodeSize, nodeSize }, Vector4<Color>(colorList[0]));
    testNodeParent->SetBackgroundColor(0xff000000);
    GetRootNode()->AddChild(testNodeParent);
    RegisterNode(testNodeParent);

    const int testPos = 50;
    auto testNodeChild = RSCanvasNode::Create();
    setNode(testNodeChild, { testPos, testPos, nodeHalfSize, nodeHalfSize }, Vector4<Color>(Color(0xffffffff)));
    testNodeChild->SetForegroundColor(0xffff0000);
    testNodeParent->AddChild(testNodeChild);
    RegisterNode(testNodeChild);

    // alpha border
    auto testNodeAlphaColor = RSCanvasNode::Create();
    setNode(testNodeAlphaColor, { nodePos, nodePos * 2, nodeSize, nodeSize }, Vector4<Color>(Color(0x7dffffff)));
    GetRootNode()->AddChild(testNodeAlphaColor);
    RegisterNode(testNodeAlphaColor);

    // four different color
    auto testNodeFourColor = RSCanvasNode::Create();
    setNode(testNodeFourColor, { 0, nodePos * 3, nodeSize, nodeSize },
        Vector4<Color>(colorList[0], colorList[1], colorList[2], colorList[3]));
    GetRootNode()->AddChild(testNodeFourColor);
    RegisterNode(testNodeFourColor);
}

// width
GRAPHIC_TEST(AppearanceTest03, CONTENT_DISPLAY_TEST, Appearance_OutlineBorder_Radius_Width_Test_1)
{
    uint32_t widthList[] = { 0, 5, 120 };
    Vector4<BorderStyle> style = Vector4<BorderStyle>(BorderStyle::SOLID);
    Color color(0, 0, 0);
    Vector4<Color> outLineColor = { color, color, color, color };

    const int nodeCount = 3;
    const int columnCount = 2;
    const int nodeSize = 400;
    const int nodePos = 420;
    const float radiusVal = 50.0f;
    const int offsetVal = 20;
    const int offsetVal2 = 400;

    for (int i = 0; i < nodeCount; i++) {
        int x = (i % columnCount) * nodePos;
        int y = (i / columnCount) * nodePos;
        auto testNodeWidth = RSCanvasNode::Create();
        testNodeWidth->SetBounds({ x, y, nodeSize, nodeSize });
        testNodeWidth->SetTranslate(widthList[i] + offsetVal, widthList[i] + radiusVal, 0);
        testNodeWidth->SetOutlineStyle(style);
        testNodeWidth->SetOutlineWidth({ widthList[i], widthList[i], widthList[i], widthList[i] });
        testNodeWidth->SetOutlineRadius({ radiusVal, radiusVal, radiusVal, radiusVal });
        testNodeWidth->SetOutlineColor(outLineColor);
        GetRootNode()->AddChild(testNodeWidth);
        RegisterNode(testNodeWidth);
    }

    // four different width
    auto testNodeFourWidth = RSCanvasNode::Create();
    testNodeFourWidth->SetBounds({ 0, nodePos * 2, nodeSize, nodeSize });
    testNodeFourWidth->SetTranslate(offsetVal, offsetVal2, 0);
    testNodeFourWidth->SetOutlineStyle(style);
    testNodeFourWidth->SetOutlineWidth({ widthList[1] * 0, widthList[1] * 2, widthList[1] * 4, widthList[1] * 8 });
    testNodeFourWidth->SetOutlineRadius({ radiusVal, radiusVal, radiusVal, radiusVal });
    testNodeFourWidth->SetOutlineColor(outLineColor);
    GetRootNode()->AddChild(testNodeFourWidth);
    RegisterNode(testNodeFourWidth);
}

GRAPHIC_TEST(AppearanceTest03, CONTENT_DISPLAY_TEST, Appearance_OutlineBorder_Radius_Width_Test_2)
{
    std::vector<float> widthList = { -5, 5, 500 };
    Vector4<BorderStyle> style = Vector4<BorderStyle>(BorderStyle::SOLID);
    Color color(0, 0, 0);
    Vector4<Color> outLineColor = { color, color, color, color };

    const int nodeCount = 3;
    const int columnCount = 2;
    const int nodeSize = 400;
    const int nodePos = 420;
    const float radiusVal = 50.0f;
    const int offsetVal = 20;
    const int offsetVal2 = 400;

    for (int i = 0; i < nodeCount; i++) {
        int x = (i % columnCount) * nodePos;
        int y = (i / columnCount) * nodePos;
        auto testNodeWidth = RSCanvasNode::Create();
        testNodeWidth->SetBounds({ x, y, nodeSize, nodeSize });
        testNodeWidth->SetTranslate(widthList[i] + offsetVal, widthList[i] + radiusVal, 0);
        testNodeWidth->SetOutlineStyle(style);
        testNodeWidth->SetOutlineWidth({ widthList[i], widthList[i], widthList[i], widthList[i] });
        testNodeWidth->SetOutlineRadius({ radiusVal, radiusVal, radiusVal, radiusVal });
        testNodeWidth->SetOutlineColor(outLineColor);
        GetRootNode()->AddChild(testNodeWidth);
        RegisterNode(testNodeWidth);
    }

    // four different width
    auto testNodeFourWidth = RSCanvasNode::Create();
    testNodeFourWidth->SetBounds({ 0, nodePos * 2, nodeSize, nodeSize });
    testNodeFourWidth->SetTranslate(offsetVal, offsetVal2, 0);
    testNodeFourWidth->SetOutlineStyle(style);
    testNodeFourWidth->SetOutlineWidth({ widthList[1] * 0, widthList[1] * 2, widthList[1] * 4, widthList[1] * 8 });
    testNodeFourWidth->SetOutlineRadius({ radiusVal, radiusVal, radiusVal, radiusVal });
    testNodeFourWidth->SetOutlineColor(outLineColor);
    GetRootNode()->AddChild(testNodeFourWidth);
    RegisterNode(testNodeFourWidth);
}

// Style
// 0 -- Solid
// 1 -- Dash
// 2 -- Dot
// 3 -- None
GRAPHIC_TEST(AppearanceTest03, CONTENT_DISPLAY_TEST, Appearance_OutlineBorder_Radius_Style_Test_1)
{
    Color color(0, 0, 0);
    Vector4<Color> outLineColor = { color, color, color, color };

    const int nodeCount = 4;
    const int columnCount = 2;
    const int nodeSize = 500;
    const int nodePos = 520;
    const float radiusVal = 50.0f;
    const int borderWidth = 5;
    const int tranlateVal = 20;

    for (int i = 0; i < nodeCount; i++) {
        int x = (i % columnCount) * nodePos;
        int y = (i / columnCount) * nodePos;
        auto testNodeStyle = RSCanvasNode::Create();
        testNodeStyle->SetBounds({ x, y, nodeSize, nodeSize });
        testNodeStyle->SetTranslate(tranlateVal, tranlateVal, 0);
        Vector4<BorderStyle> style = Vector4<BorderStyle>((BorderStyle)i);
        testNodeStyle->SetOutlineStyle(style);
        testNodeStyle->SetOutlineWidth({ borderWidth, borderWidth, borderWidth, borderWidth });
        testNodeStyle->SetOutlineRadius({ radiusVal, radiusVal, radiusVal, radiusVal });
        testNodeStyle->SetOutlineColor(outLineColor);
        GetRootNode()->AddChild(testNodeStyle);
        RegisterNode(testNodeStyle);
    }

    // four different style
    auto testNodeFourStyle = RSCanvasNode::Create();
    testNodeFourStyle->SetBounds({ 0, nodePos * 2, nodeSize, nodeSize });
    testNodeFourStyle->SetTranslate(tranlateVal, tranlateVal, 0);
    Vector4<BorderStyle> style2 = Vector4<BorderStyle>((BorderStyle)0, (BorderStyle)1, (BorderStyle)2, (BorderStyle)3);
    testNodeFourStyle->SetOutlineStyle(style2);
    testNodeFourStyle->SetOutlineWidth({ borderWidth, borderWidth, borderWidth, borderWidth });
    testNodeFourStyle->SetOutlineRadius({ radiusVal, radiusVal, radiusVal, radiusVal });
    testNodeFourStyle->SetOutlineColor(outLineColor);
    GetRootNode()->AddChild(testNodeFourStyle);
    RegisterNode(testNodeFourStyle);
}

// Dash Width
GRAPHIC_TEST(AppearanceTest03, CONTENT_DISPLAY_TEST, Appearance_OutlineBorder_Radius_DashWidth_Test_1)
{
    uint32_t widthList[] = { 0, 20, 120, 250 };
    Vector4<BorderStyle> style = Vector4<BorderStyle>((BorderStyle)1);
    Color color(0, 0, 0);
    Vector4<Color> outLineColor = { color, color, color, color };

    for (int i = 0; i < 4; i++) {
        int x = (i % 2) * 520;
        int y = (i / 2) * 520;
        auto testNodeDashWidth = RSCanvasNode::Create();
        testNodeDashWidth->SetBounds({ x, y, 500, 500 });
        testNodeDashWidth->SetTranslate(20, 20, 0);
        // dash style
        testNodeDashWidth->SetOutlineStyle(style);
        testNodeDashWidth->SetBorderDashWidth({ widthList[i], widthList[i], widthList[i], widthList[i] });
        testNodeDashWidth->SetOutlineWidth({ 5, 5, 5, 5 });
        testNodeDashWidth->SetOutlineRadius({ 50.0, 50.0, 50.0, 50.0 });
        testNodeDashWidth->SetOutlineColor(outLineColor);
        GetRootNode()->AddChild(testNodeDashWidth);
        RegisterNode(testNodeDashWidth);
    }

    // four different dash width
    auto testNodeFourDashWidth = RSCanvasNode::Create();
    testNodeFourDashWidth->SetBounds({ 0, 520 * 2, 500, 500 });
    testNodeFourDashWidth->SetTranslate(20, 20, 0);
    testNodeFourDashWidth->SetOutlineStyle(style);
    testNodeFourDashWidth->SetBorderDashWidth(
        { widthList[1] * 0, widthList[1] * 2, widthList[1] * 4, widthList[1] * 8 });
    testNodeFourDashWidth->SetOutlineWidth({ 5, 5, 5, 5 });
    testNodeFourDashWidth->SetOutlineRadius({ 50.0, 50.0, 50.0, 50.0 });
    testNodeFourDashWidth->SetOutlineColor(outLineColor);
    GetRootNode()->AddChild(testNodeFourDashWidth);
    RegisterNode(testNodeFourDashWidth);

    // not dash style, set dash width
    auto testNodeSolid = RSCanvasNode::Create();
    testNodeSolid->SetBounds({ 520, 520 * 2, 500, 500 });
    testNodeSolid->SetTranslate(20, 20, 0);
    // solid style
    testNodeSolid->SetOutlineStyle(style);
    testNodeSolid->SetBorderDashWidth({ widthList[1], widthList[1], widthList[1], widthList[1] });
    testNodeSolid->SetOutlineWidth({ 5, 5, 5, 5 });
    testNodeSolid->SetOutlineRadius({ 50.0, 50.0, 50.0, 50.0 });
    testNodeSolid->SetOutlineColor(outLineColor);
    GetRootNode()->AddChild(testNodeSolid);
    RegisterNode(testNodeSolid);
}

GRAPHIC_TEST(AppearanceTest03, CONTENT_DISPLAY_TEST, Appearance_OutlineBorder_Radius_OutlineDashWidth_Test_1)
{
    uint32_t widthList[] = { 0, 20, 120, 250, -20 };
    Vector4<BorderStyle> style = Vector4<BorderStyle>((BorderStyle)1);
    Color color(0, 0, 0);
    Vector4<Color> outLineColor = { color, color, color, color };

    for (int i = 0; i < 5; i++) {
        int x = (i % 2) * 520;
        int y = (i / 2) * 520;
        auto testNodeDashWidth = RSCanvasNode::Create();
        testNodeDashWidth->SetBounds({ x, y, 500, 500 });
        testNodeDashWidth->SetTranslate(20, 20, 0);
        // dash style
        testNodeDashWidth->SetOutlineStyle(style);
        testNodeDashWidth->SetOutlineDashWidth({ widthList[i], widthList[i], widthList[i], widthList[i] });
        testNodeDashWidth->SetOutlineWidth({ 5, 5, 5, 5 });
        testNodeDashWidth->SetOutlineRadius({ 50.0, 50.0, 50.0, 50.0 });
        testNodeDashWidth->SetOutlineColor(outLineColor);
        GetRootNode()->AddChild(testNodeDashWidth);
        RegisterNode(testNodeDashWidth);
    }

    // four different dash width
    auto testNodeFourDashWidth = RSCanvasNode::Create();
    testNodeFourDashWidth->SetBounds({ 0, 520 * 3, 500, 500 });
    testNodeFourDashWidth->SetTranslate(20, 20, 0);
    testNodeFourDashWidth->SetOutlineStyle(style);
    testNodeFourDashWidth->SetOutlineDashWidth(
        { widthList[1] * 0, widthList[1] * 2, widthList[1] * 4, widthList[1] * 8 });
    testNodeFourDashWidth->SetOutlineWidth({ 5, 5, 5, 5 });
    testNodeFourDashWidth->SetOutlineRadius({ 50.0, 50.0, 50.0, 50.0 });
    testNodeFourDashWidth->SetOutlineColor(outLineColor);
    GetRootNode()->AddChild(testNodeFourDashWidth);
    RegisterNode(testNodeFourDashWidth);

    // not dash style, set dash width
    auto testNodeSolid = RSCanvasNode::Create();
    testNodeSolid->SetBounds({ 520, 520 * 3, 500, 500 });
    testNodeSolid->SetTranslate(20, 20, 0);
    // solid style
    testNodeSolid->SetOutlineStyle(style);
    testNodeSolid->SetOutlineDashWidth({ widthList[1], widthList[1], widthList[1], widthList[1] });
    testNodeSolid->SetOutlineWidth({ 5, 5, 5, 5 });
    testNodeSolid->SetOutlineRadius({ 50.0, 50.0, 50.0, 50.0 });
    testNodeSolid->SetOutlineColor(outLineColor);
    GetRootNode()->AddChild(testNodeSolid);
    RegisterNode(testNodeSolid);
}

// Dash Gap
GRAPHIC_TEST(AppearanceTest03, CONTENT_DISPLAY_TEST, Appearance_OutlineBorder_Radius_DashGap_Test_1)
{
    uint32_t gapList[] = { 0, 20, 120, 250 };
    Vector4<BorderStyle> style = Vector4<BorderStyle>((BorderStyle)1);
    Color color(0, 0, 0);
    Vector4<Color> outLineColor = { color, color, color, color };

    for (int i = 0; i < 4; i++) {
        int x = (i % 2) * 520;
        int y = (i / 2) * 520;
        auto testNodeDashGap = RSCanvasNode::Create();
        testNodeDashGap->SetBounds({ x, y, 500, 500 });
        testNodeDashGap->SetTranslate(20, 20, 0);
        // dash style
        testNodeDashGap->SetOutlineStyle(style);
        testNodeDashGap->SetBorderDashGap({ gapList[i], gapList[i], gapList[i], gapList[i] });
        testNodeDashGap->SetOutlineWidth({ 5, 5, 5, 5 });
        testNodeDashGap->SetOutlineRadius({ 50.0, 50.0, 50.0, 50.0 });
        testNodeDashGap->SetOutlineColor(outLineColor);
        GetRootNode()->AddChild(testNodeDashGap);
        RegisterNode(testNodeDashGap);
    }

    // four different dash width
    auto testNodeFourDashGap = RSCanvasNode::Create();
    testNodeFourDashGap->SetBounds({ 0, 520 * 2, 500, 500 });
    testNodeFourDashGap->SetTranslate(20, 20, 0);
    testNodeFourDashGap->SetOutlineStyle(style);
    testNodeFourDashGap->SetBorderDashGap({ gapList[1] * 0, gapList[1] * 2, gapList[1] * 4, gapList[1] * 8 });
    testNodeFourDashGap->SetOutlineWidth({ 5, 5, 5, 5 });
    testNodeFourDashGap->SetOutlineRadius({ 50.0, 50.0, 50.0, 50.0 });
    testNodeFourDashGap->SetOutlineColor(outLineColor);
    GetRootNode()->AddChild(testNodeFourDashGap);
    RegisterNode(testNodeFourDashGap);

    // not dash style, set dash width
    auto testNodeSolid = RSCanvasNode::Create();
    testNodeSolid->SetBounds({ 520, 520 * 2, 500, 500 });
    testNodeSolid->SetTranslate(20, 20, 0);
    // solid style
    testNodeSolid->SetOutlineStyle(style);
    testNodeSolid->SetBorderDashGap({ gapList[1], gapList[1], gapList[1], gapList[1] });
    testNodeSolid->SetOutlineWidth({ 5, 5, 5, 5 });
    testNodeSolid->SetOutlineRadius({ 50.0, 50.0, 50.0, 50.0 });
    testNodeSolid->SetOutlineColor(outLineColor);
    GetRootNode()->AddChild(testNodeSolid);
    RegisterNode(testNodeSolid);
}

GRAPHIC_TEST(AppearanceTest03, CONTENT_DISPLAY_TEST, Appearance_OutlineBorder_Radius_OutlineDashGap_Test_1)
{
    uint32_t gapList[] = { 0, 20, 120, 250, -20 };
    Vector4<BorderStyle> style = Vector4<BorderStyle>((BorderStyle)1);
    Color color(0, 0, 0);
    Vector4<Color> outLineColor = { color, color, color, color };

    for (int i = 0; i < 5; i++) {
        int x = (i % 2) * 520;
        int y = (i / 2) * 520;
        auto testNodeDashGap = RSCanvasNode::Create();
        testNodeDashGap->SetBounds({ x, y, 500, 500 });
        testNodeDashGap->SetTranslate(20, 20, 0);
        // dash style
        testNodeDashGap->SetOutlineStyle(style);
        testNodeDashGap->SetOutlineDashGap({ gapList[i], gapList[i], gapList[i], gapList[i] });
        testNodeDashGap->SetOutlineWidth({ 5, 5, 5, 5 });
        testNodeDashGap->SetOutlineRadius({ 50.0, 50.0, 50.0, 50.0 });
        testNodeDashGap->SetOutlineColor(outLineColor);
        GetRootNode()->AddChild(testNodeDashGap);
        RegisterNode(testNodeDashGap);
    }

    // four different dash width
    auto testNodeFourDashGap = RSCanvasNode::Create();
    testNodeFourDashGap->SetBounds({ 0, 520 * 3, 500, 500 });
    testNodeFourDashGap->SetTranslate(20, 20, 0);
    testNodeFourDashGap->SetOutlineStyle(style);
    testNodeFourDashGap->SetOutlineDashGap({ gapList[1] * 0, gapList[1] * 2, gapList[1] * 4, gapList[1] * 8 });
    testNodeFourDashGap->SetOutlineWidth({ 5, 5, 5, 5 });
    testNodeFourDashGap->SetOutlineRadius({ 50.0, 50.0, 50.0, 50.0 });
    testNodeFourDashGap->SetOutlineColor(outLineColor);
    GetRootNode()->AddChild(testNodeFourDashGap);
    RegisterNode(testNodeFourDashGap);

    // not dash style, set dash width
    auto testNodeSolid = RSCanvasNode::Create();
    testNodeSolid->SetBounds({ 520, 520 * 3, 500, 500 });
    testNodeSolid->SetTranslate(20, 20, 0);
    // solid style
    testNodeSolid->SetOutlineStyle(style);
    testNodeSolid->SetOutlineDashGap({ gapList[1], gapList[1], gapList[1], gapList[1] });
    testNodeSolid->SetOutlineWidth({ 5, 5, 5, 5 });
    testNodeSolid->SetOutlineRadius({ 50.0, 50.0, 50.0, 50.0 });
    testNodeSolid->SetOutlineColor(outLineColor);
    GetRootNode()->AddChild(testNodeSolid);
    RegisterNode(testNodeSolid);
}

// two node eoverlay & touch
GRAPHIC_TEST(AppearanceTest03, CONTENT_DISPLAY_TEST, Appearance_OutlineBorder_Radius_Touch_Test_1)
{
    Color color1(255, 0, 0);
    Vector4<Color> outLineColor1 = { color1, color1, color1, color1 };
    Color color2(0, 0, 255);
    Vector4<Color> outLineColor2 = { color2, color2, color2, color2 };

    uint32_t styleList[] = { 0, 1, 0, 1 };
    float xList[] = { 50, 50, 525, 525 };
    float yList[] = { 50, 50, 20, 20 };
    // overlay
    for (int i = 0; i < 4; i++) {
        int x = (i % 2) * 560;
        int y = (i / 2) * 510;
        if (i == 3) {
            x -= 560;
            y += 510;
        }
        auto testNode1 = RSCanvasNode::Create();
        testNode1->SetBounds({ x, y, 500, 500 });
        testNode1->SetTranslate(20, 20, 0);
        testNode1->SetBackgroundColor(0xff000000);
        Vector4<BorderStyle> style = Vector4<BorderStyle>((BorderStyle)styleList[i]);
        testNode1->SetOutlineStyle(style);
        testNode1->SetBorderDashGap({ 25, 25, 25, 25 });
        testNode1->SetOutlineWidth({ 10, 10, 10, 10 });
        testNode1->SetOutlineRadius({ 50.0, 50.0, 50.0, 50.0 });
        testNode1->SetOutlineColor(outLineColor1);
        GetRootNode()->AddChild(testNode1);
        RegisterNode(testNode1);

        auto testNode2 = RSCanvasNode::Create();
        testNode2->SetBounds({ x, y, 500, 500 });
        testNode2->SetTranslate(xList[i], yList[i], 0);
        testNode2->SetBackgroundColor(0xff000000);
        testNode2->SetOutlineStyle(style);
        testNode2->SetBorderDashGap({ 25, 25, 25, 25 });
        testNode2->SetOutlineWidth({ 10, 10, 10, 10 });
        testNode2->SetOutlineRadius({ 50.0, 50.0, 50.0, 50.0 });
        testNode2->SetOutlineColor(outLineColor2);
        GetRootNode()->AddChild(testNode2);
        RegisterNode(testNode2);
    }
}

GRAPHIC_TEST(AppearanceTest03, CONTENT_DISPLAY_TEST, Appearance_Outline_Radius_Test_1)
{
    vector<float> vecs = {-20, 0, 20, 150, 500};
    for (int i = 0; i < vecs.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({380, i * 350 + 20, 400, 300});
        testNode->SetOutlineRadius(vecs[i]);
        testNode->SetOutlineWidth(5);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(AppearanceTest03, CONTENT_DISPLAY_TEST, Appearance_Outline_Radius_Test_2)
{
    vector<Vector4f> vecs = {{0, 20, 50, 80}, {50, 100, 50, 80}};
    for (int i = 0; i < vecs.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({380, i * 350 + 20, 400, 300});
        testNode->SetOutlineRadius(vecs[i]);
        testNode->SetOutlineWidth(5);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(AppearanceTest03, CONTENT_DISPLAY_TEST, Appearance_Outline_BackgroundColor_Test_1)
{
    int columnCount = 2;
    int rowCount = 3;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;
    vector<float> vecs1 = {0, 0, 1, 1, 2, 2};
    vector<float> vecs2 = {0, 0, 20, 20, 20, 20};
    vector<float> vecs3 = {0, 0, 20, 20, 20, 20};
    vector<float> vecs4 = {0, 40, 0, 40, 0, 40};
    vector<float> vecs5 = {0, 40, 0, 40, 0, 40};
    Vector4<Color> color(Color::FromArgbInt(0xFF00FF00), Color::FromArgbInt(0xFF00FF00),
        Color::FromArgbInt(0xFF00FF00), Color::FromArgbInt(0xFF00FF00));
    for (int i = 0; i < vecs1.size(); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({x, y, sizeX - 10, sizeY - 10});
        testNode->SetBackgroundColor(0x8F0000FF);
        testNode->SetOutlineColor(color);
        testNode->SetOutlineWidth(20);
        Vector4<BorderStyle> style = Vector4<BorderStyle>((BorderStyle)vecs1[i]);
        testNode->SetOutlineStyle(style);
        testNode->SetOutlineDashWidth(vecs2[i]);
        testNode->SetOutlineDashGap(vecs3[i]);
        testNode->SetOutlineRadius(vecs4[i]);
        testNode->SetCornerRadius(vecs5[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(AppearanceTest03, CONTENT_DISPLAY_TEST, Appearance_CornerRadius_Test_1)
{
    vector<float> vecs = {-20, 0, 20, 150, 160};
    for (int i = 0; i < vecs.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({380, i * 350 + 20, 400, 300});
        testNode->SetBackgroundColor(0x8F0000FF);
        testNode->SetCornerRadius(vecs[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(AppearanceTest03, CONTENT_DISPLAY_TEST, Appearance_CornerRadius_Test_2)
{
    vector<Vector4f> vecs = {{0, 20, 50, 80}, {50, 100, 50, 80}};
    for (int i = 0; i < vecs.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({380, i * 350 + 20, 400, 300});
        testNode->SetBackgroundColor(0x8F0000FF);
        testNode->SetCornerRadius(vecs[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(AppearanceTest03, CONTENT_DISPLAY_TEST, Appearance_CornerRadius_OutlineRadius_Test_1)
{
    vector<float> vecs1 = {20, 150, 500};
    vector<float> vecs2 = {20, 500, 150};
    for (int i = 0; i < vecs1.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({380, i * 350 + 20, 400, 300});
        testNode->SetOutlineRadius(vecs1[i]);
        testNode->SetCornerRadius(vecs2[i]);
        testNode->SetOutlineWidth(5);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(AppearanceTest03, CONTENT_DISPLAY_TEST, Appearance_CornerRadius_OutlineRadius_Test_2)
{
    vector<Vector4f> vecs1 = {{0, 20, 50, 80}, {50, 100, 50, 80}};
    vector<Vector4f> vecs2 = {{0, 20, 50, 80}, {100, 50, 80, 50}};
    for (int i = 0; i < vecs1.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({380, i * 350 + 20, 400, 300});
        testNode->SetOutlineRadius(vecs1[i]);
        testNode->SetCornerRadius(vecs2[i]);
        testNode->SetOutlineWidth(5);
        testNode->SetPivot(Vector2f(0.5, 0.5));
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}
} // namespace OHOS::Rosen