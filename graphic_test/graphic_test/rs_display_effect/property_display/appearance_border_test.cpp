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

class AppearanceTest02 : public RSGraphicTest {
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
        if (!node) {
            node = RSCanvasNode::Create();
        }
        node->SetBounds(bounds);
        int transXY = 20;
        int transZ = 0;
        node->SetTranslate(transXY, transXY, transZ);
        node->SetBorderStyle(style);
        Vector4f borderWidth = { 5, 5, 5, 5 };
        node->SetBorderWidth(borderWidth);
        node->SetBorderColor(outLineColor);
    };

    void setNode(std::shared_ptr<RSCanvasNode>& node, const Vector4f bounds, Vector4f translate)
    {
        if (!node) {
            node = RSCanvasNode::Create();
        }
        node->SetBounds(bounds);
        node->SetTranslate(translate[0], translate[1], translate[2]);
        Vector4<BorderStyle> style = Vector4<BorderStyle>(BorderStyle::SOLID);
        node->SetBorderStyle(style);
        Vector4f borderWidth = { 50, 50, 50, 50 };
        node->SetBorderWidth(borderWidth);
        node->SetBorderColor({ Color(0, 0, 0), Color(255, 0, 0), Color(0, 255, 0), Color(0, 0, 255) });
        node->SetBackgroundColor(0xff000000);
    };
};

// color
GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Border_Color_Test_1)
{
    Color colorList[] = { Color(0, 0, 0), Color(255, 0, 0), Color(0, 255, 0), Color(0, 0, 255) };

    const int nodeCount = 4;
    const int columnCount = 2;
    const int nodeSize = 400;
    const int nodeHalfSize = 200;
    const int nodePos = 420;
    const int testPos = 50;

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
GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Border_Width_Test_1)
{
    std::vector<float> widthList = { 0, 5, 250, 500, -5 };

    const int nodeCount = widthList.size();
    const int columnCount = 2;
    const int nodeSize = 500;
    const int nodePos = 510;

    for (int i = 0; i < nodeCount; i++) {
        int x = (i % columnCount) * nodePos;
        int y = (i / columnCount) * nodePos;
        auto testNodeWidth = RSCanvasNode::Create();
        testNodeWidth->SetBounds({ x, y, nodeSize, nodeSize });
        testNodeWidth->SetBorderStyle(0, 0, 0, 0);
        testNodeWidth->SetBorderWidth(widthList[i], widthList[i], widthList[i], widthList[i]);
        testNodeWidth->SetBorderColor(0xff000000, 0xff000000, 0xff000000, 0xff000000);
        GetRootNode()->AddChild(testNodeWidth);
        RegisterNode(testNodeWidth);
    }

    // four different width
    auto testNodeFourWidth = RSCanvasNode::Create();
    testNodeFourWidth->SetBounds({ 0, nodePos * 3, nodeSize, nodeSize });
    testNodeFourWidth->SetBorderStyle(0, 0, 0, 0);
    testNodeFourWidth->SetBorderWidth(widthList[1] * 0, widthList[1] * 2, widthList[1] * 4, widthList[1] * 8);
    testNodeFourWidth->SetBorderColor(0xff000000, 0xff000000, 0xff000000, 0xff000000);
    GetRootNode()->AddChild(testNodeFourWidth);
    RegisterNode(testNodeFourWidth);
}

// Style
// 0 -- Solid
// 1 -- Dash
// 2 -- Dot
// 3 -- None
GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Border_Style_Test_1)
{
    const int nodeCount = 4;
    const int columnCount = 2;
    const int nodeSize = 500;
    const int nodePos = 510;
    const int borderWidth = 5;

    for (int i = 0; i < nodeCount; i++) {
        int x = (i % columnCount) * nodePos;
        int y = (i / columnCount) * nodePos;
        auto testNodeStyle = RSCanvasNode::Create();
        testNodeStyle->SetBounds({ x, y, nodeSize, nodeSize });
        testNodeStyle->SetBorderStyle(i, i, i, i);
        testNodeStyle->SetBorderWidth(borderWidth, borderWidth, borderWidth, borderWidth);
        testNodeStyle->SetBorderColor(0xff000000, 0xff000000, 0xff000000, 0xff000000);
        GetRootNode()->AddChild(testNodeStyle);
        RegisterNode(testNodeStyle);
    }

    // four different style
    auto testNodeFourStyle = RSCanvasNode::Create();
    testNodeFourStyle->SetBounds({ 0, nodePos * 2, nodeSize, nodeSize });
    testNodeFourStyle->SetBorderStyle(0, 1, 2, 3);
    testNodeFourStyle->SetBorderWidth(borderWidth, borderWidth, borderWidth, borderWidth);
    testNodeFourStyle->SetBorderColor(0xff000000, 0xff000000, 0xff000000, 0xff000000);
    GetRootNode()->AddChild(testNodeFourStyle);
    RegisterNode(testNodeFourStyle);
}

// Dash Width
GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Border_DashWidth_Test_1)
{
    std::vector<float> widthList = { 0, 20, 120, 250, -20 };

    const int nodeCount = widthList.size();
    const int columnCount = 2;
    const int nodeSize = 500;
    const int nodePos = 510;
    const int borderWidth = 5;

    for (int i = 0; i < nodeCount; i++) {
        int x = (i % columnCount) * nodePos;
        int y = (i / columnCount) * nodePos;
        auto testNodeDashWidth = RSCanvasNode::Create();
        testNodeDashWidth->SetBounds({ x, y, nodeSize, nodeSize });
        // dash style
        testNodeDashWidth->SetBorderStyle(1, 1, 1, 1);
        testNodeDashWidth->SetBorderDashWidth({ widthList[i], widthList[i], widthList[i], widthList[i] });
        testNodeDashWidth->SetBorderWidth(borderWidth, borderWidth, borderWidth, borderWidth);
        testNodeDashWidth->SetBorderColor(0xff000000, 0xff000000, 0xff000000, 0xff000000);
        GetRootNode()->AddChild(testNodeDashWidth);
        RegisterNode(testNodeDashWidth);
    }

    // four different dash width
    auto testNodeFourDashWidth = RSCanvasNode::Create();
    testNodeFourDashWidth->SetBounds({ 0, nodePos * 3, nodeSize, nodeSize });
    testNodeFourDashWidth->SetBorderStyle(1, 1, 1, 1);
    testNodeFourDashWidth->SetBorderDashWidth(
        { widthList[1] * 0, widthList[1] * 2, widthList[1] * 4, widthList[1] * 8 });
    testNodeFourDashWidth->SetBorderWidth(borderWidth, borderWidth, borderWidth, borderWidth);
    testNodeFourDashWidth->SetBorderColor(0xff000000, 0xff000000, 0xff000000, 0xff000000);
    GetRootNode()->AddChild(testNodeFourDashWidth);
    RegisterNode(testNodeFourDashWidth);

    // not dash style, set dash width
    auto testNodeSolid = RSCanvasNode::Create();
    testNodeSolid->SetBounds({ nodePos, nodePos * 3, nodeSize, nodeSize });
    // solid style
    testNodeSolid->SetBorderStyle(0, 0, 0, 0);
    testNodeSolid->SetBorderDashWidth({ widthList[1], widthList[1], widthList[1], widthList[1] });
    testNodeSolid->SetBorderWidth(borderWidth, borderWidth, borderWidth, borderWidth);
    testNodeSolid->SetBorderColor(0xff000000, 0xff000000, 0xff000000, 0xff000000);
    GetRootNode()->AddChild(testNodeSolid);
    RegisterNode(testNodeSolid);
}

// Dash Gap
GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Border_DashGap_Test_1)
{
    std::vector<float> gapList = { 0, 20, 120, 250, -20 };

    const int nodeCount = gapList.size();
    const int columnCount = 2;
    const int nodeSize = 500;
    const int nodePos = 510;
    const int borderWidth = 5;

    for (int i = 0; i < nodeCount; i++) {
        int x = (i % columnCount) * nodePos;
        int y = (i / columnCount) * nodePos;
        auto testNodeDashGap = RSCanvasNode::Create();
        testNodeDashGap->SetBounds({ x, y, nodeSize, nodeSize });
        // dash style
        testNodeDashGap->SetBorderStyle(1, 1, 1, 1);
        testNodeDashGap->SetBorderDashGap({ gapList[i], gapList[i], gapList[i], gapList[i] });
        testNodeDashGap->SetBorderWidth(borderWidth, borderWidth, borderWidth, borderWidth);
        testNodeDashGap->SetBorderColor(0xff000000, 0xff000000, 0xff000000, 0xff000000);
        GetRootNode()->AddChild(testNodeDashGap);
        RegisterNode(testNodeDashGap);
    }

    // four different dash width
    auto testNodeFourDashGap = RSCanvasNode::Create();
    testNodeFourDashGap->SetBounds({ 0, nodePos * 3, nodeSize, nodeSize });
    testNodeFourDashGap->SetBorderStyle(1, 1, 1, 1);
    testNodeFourDashGap->SetBorderDashGap({ gapList[1] * 0, gapList[1] * 2, gapList[1] * 4, gapList[1] * 8 });
    testNodeFourDashGap->SetBorderWidth(borderWidth, borderWidth, borderWidth, borderWidth);
    testNodeFourDashGap->SetBorderColor(0xff000000, 0xff000000, 0xff000000, 0xff000000);
    GetRootNode()->AddChild(testNodeFourDashGap);
    RegisterNode(testNodeFourDashGap);

    // not dash style, set dash width
    auto testNodeSolid = RSCanvasNode::Create();
    testNodeSolid->SetBounds({ nodePos, nodePos * 3, nodeSize, nodeSize });
    // solid style
    testNodeSolid->SetBorderStyle(0, 0, 0, 0);
    testNodeSolid->SetBorderDashGap({ gapList[1], gapList[1], gapList[1], gapList[1] });
    testNodeSolid->SetBorderWidth(borderWidth, borderWidth, borderWidth, borderWidth);
    testNodeSolid->SetBorderColor(0xff000000, 0xff000000, 0xff000000, 0xff000000);
    GetRootNode()->AddChild(testNodeSolid);
    RegisterNode(testNodeSolid);
}

// two node eoverlay & touch
GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Border_Touch_Test_1)
{
    uint32_t styleList[] = { 0, 1, 0, 1 };
    float xList[] = { 50, 50, 505, 505 };
    float yList[] = { 50, 50, 0, 0 };

    const int nodeCount = 4;
    const int columnCount = 2;
    const int nodeSize = 500;
    const int nodePos = 510;
    const int nodePosL = 560;
    const int dashGap = 25;
    const int borderWidth = 10;

    // overlay
    for (int i = 0; i < nodeCount; i++) {
        int x = (i % columnCount) * nodePosL;
        int y = (i / columnCount) * nodePos;
        if (i == nodeCount - 1) {
            x -= nodePosL;
            y += nodePos;
        }
        auto testNode1 = RSCanvasNode::Create();
        testNode1->SetBounds({ x, y, nodeSize, nodeSize });
        testNode1->SetBackgroundColor(0xff000000);
        testNode1->SetBorderStyle(Vector4<BorderStyle>(static_cast<BorderStyle>(styleList[i])));
        testNode1->SetBorderDashGap({ dashGap, dashGap, dashGap, dashGap });
        testNode1->SetBorderWidth({ borderWidth, borderWidth, borderWidth, borderWidth });
        testNode1->SetBorderColor(0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000);
        GetRootNode()->AddChild(testNode1);
        RegisterNode(testNode1);

        auto testNode2 = RSCanvasNode::Create();
        testNode2->SetBounds({ x, y, nodeSize, nodeSize });
        testNode2->SetTranslate(xList[i], yList[i], 0);
        testNode2->SetBackgroundColor(0xff000000);
        testNode2->SetBorderStyle(Vector4<BorderStyle>(static_cast<BorderStyle>(styleList[i])));
        testNode2->SetBorderDashGap({ dashGap, dashGap, dashGap, dashGap });
        testNode2->SetBorderWidth({ borderWidth, borderWidth, borderWidth, borderWidth });
        testNode2->SetBorderColor(0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff);
        GetRootNode()->AddChild(testNode2);
        RegisterNode(testNode2);
    }
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Bounds_Test)
{
    int columnCount = 2;
    int rowCount = 4;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<uint32_t> valueList = { -10, 0, 10, 100 };

    // each row we draw a node with bounds width, and a node with bounds height
    for (int i = 0; i < rowCount; i++) {
        // node with bounds width
        int y = (i % rowCount) * sizeY;
        auto testNodeBackGround = RSCanvasNode::Create();
        testNodeBackGround->SetBackgroundColor(0xff00ffff); // Node BgColor set to Blue
        testNodeBackGround->SetBounds({ 0, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBoundsWidth(valueList[i]);
        GetRootNode()->AddChild(testNodeBackGround);
        RegisterNode(testNodeBackGround);

        // node with bounds height
        auto testNodeBackGround2 = RSCanvasNode::Create();
        testNodeBackGround2->SetBackgroundColor(0xffff0000); // Node BgColor set to Red
        testNodeBackGround2->SetBounds({ sizeX, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround2->SetBoundsHeight(valueList[i]);
        GetRootNode()->AddChild(testNodeBackGround2);
        RegisterNode(testNodeBackGround2);
    };
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_OuterBorder_Style_Test_1)
{
    const int nodeCount = 4;
    const int columnCount = 2;
    const int nodeSize = 450;
    const int nodePos = 510;
    const int borderWidth = 10;

    for (int i = 0; i < nodeCount; i++) {
        int x = (i % columnCount) * nodePos;
        int y = (i / columnCount) * nodePos;
        auto testNodeStyle = RSCanvasNode::Create();
        testNodeStyle->SetBounds({ x, y, nodeSize, nodeSize });
        testNodeStyle->SetOuterBorderStyle(Vector4<BorderStyle>(static_cast<BorderStyle>(i)));
        testNodeStyle->SetOuterBorderWidth({ borderWidth, borderWidth, borderWidth, borderWidth });
        testNodeStyle->SetOuterBorderColor(Vector4<Color>(Color::FromArgbInt(0xff000000)));
        testNodeStyle->SetOuterBorderRadius({ 50.0, 50.0, 50.0, 50.0 });
        GetRootNode()->AddChild(testNodeStyle);
        RegisterNode(testNodeStyle);
    }

    // four different style
    auto testNodeFourStyle = RSCanvasNode::Create();
    testNodeFourStyle->SetBounds({ 0, nodePos * 2, nodeSize, nodeSize });
    testNodeFourStyle->SetOuterBorderStyle(
        { BorderStyle::SOLID, BorderStyle::DASHED, BorderStyle::DOTTED, BorderStyle::NONE });
    testNodeFourStyle->SetOuterBorderWidth({ borderWidth, borderWidth, borderWidth, borderWidth });
    testNodeFourStyle->SetOuterBorderColor(Vector4<Color>(Color::FromArgbInt(0xff000000)));
    testNodeFourStyle->SetOuterBorderRadius({ 50.0, 50.0, 50.0, 50.0 });
    GetRootNode()->AddChild(testNodeFourStyle);
    RegisterNode(testNodeFourStyle);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_BorderWidth_OutlineWidth_Test_1)
{
    vector<float> vecs1 = {0, 5, 20, 500, 120};
    vector<float> vecs2 = {5, 0, 20, 120, 500};
    Vector4<Color> color(Color::FromArgbInt(0xFF00FF00), Color::FromArgbInt(0xFF00FF00),
        Color::FromArgbInt(0xFF00FF00), Color::FromArgbInt(0xFF00FF00));
    for (int i = 0; i < vecs1.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({380, i * 350 + 20, 400, 300});
        testNode->SetBorderColor(0xFFFF0000);
        testNode->SetOutlineColor(color);
        testNode->SetBorderWidth(vecs1[i]);
        testNode->SetOutlineWidth(vecs2[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Border_BackgroundColor_Test_1)
{
    int columnCount = 2;
    int rowCount = 3;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;
    vector<float> vecs1 = {0, 0, 1, 1, 2, 2};
    vector<float> vecs2 = {0, 0, 20, 20, 20, 20};
    vector<float> vecs3 = {0, 0, 20, 20, 20, 20};
    vector<float> vecs4 = {0, 40, 0, 40, 0, 40};
    for (int i = 0; i < vecs1.size(); i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({x, y, sizeX - 10, sizeY - 10});
        testNode->SetBackgroundColor(0x8F0000FF);
        testNode->SetBorderColor(0x8F00FF00);
        testNode->SetBorderWidth(20);
        testNode->SetBorderStyle(vecs1[i]);
        testNode->SetBorderDashWidth(vecs2[i]);
        testNode->SetBorderDashGap(vecs3[i]);
        testNode->SetCornerRadius(vecs4[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

enum {
    BOUNDS = 0,
    COLOR = 1,
    WIDTH = 2,
    RADIUS = 3,
    STYLE = 4,
    DASHGAP = 5,
    DASHWIDTH = 6,
};

static RSCanvasNode::SharedPtr BorderCreate(vector<vector<float>> &vecs)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds(vecs[BOUNDS][0], vecs[BOUNDS][1], vecs[BOUNDS][2], vecs[BOUNDS][3]);
    testNode->SetBackgroundColor(0x8FFF00FF);
    testNode->SetBorderColor(vecs[COLOR][0], vecs[COLOR][1], vecs[COLOR][2], vecs[COLOR][3]);
    testNode->SetBorderWidth({vecs[WIDTH][0], vecs[WIDTH][1], vecs[WIDTH][2], vecs[WIDTH][3]});
    testNode->SetCornerRadius({vecs[RADIUS][0], vecs[RADIUS][1], vecs[RADIUS][2], vecs[RADIUS][3]});
    testNode->SetBorderStyle(vecs[STYLE][0], vecs[STYLE][1], vecs[STYLE][2], vecs[STYLE][3]);
    testNode->SetBorderDashGap({vecs[DASHGAP][0], vecs[DASHGAP][1], vecs[DASHGAP][2], vecs[DASHGAP][3]});
    testNode->SetBorderDashWidth({vecs[DASHWIDTH][0], vecs[DASHWIDTH][1], vecs[DASHWIDTH][2], vecs[DASHWIDTH][3]});
    return testNode;
}

static RSCanvasNode::SharedPtr BorderCreate2(vector<vector<float>> &vecs)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds(vecs[BOUNDS][0], vecs[BOUNDS][1], vecs[BOUNDS][2], vecs[BOUNDS][3]);
    testNode->SetBackgroundColor(0x12FF0000);
    testNode->SetBorderColor(vecs[COLOR][0]);
    testNode->SetBorderWidth(vecs[WIDTH][0]);
    testNode->SetCornerRadius({vecs[RADIUS][0], vecs[RADIUS][1], vecs[RADIUS][2], vecs[RADIUS][3]});
    testNode->SetBorderStyle(vecs[STYLE][0]);
    testNode->SetBorderDashGap({vecs[DASHGAP][0], vecs[DASHGAP][1], vecs[DASHGAP][2], vecs[DASHGAP][3]});
    testNode->SetBorderDashWidth({vecs[DASHWIDTH][0], vecs[DASHWIDTH][1], vecs[DASHWIDTH][2], vecs[DASHWIDTH][3]});
    return testNode;
}

static RSCanvasNode::SharedPtr BorderCreate3(vector<vector<float>> &vecs)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds(vecs[BOUNDS][0], vecs[BOUNDS][1], vecs[BOUNDS][2], vecs[BOUNDS][3]);
    testNode->SetBackgroundColor(0x12FF0000);
    Vector4<Color> outLineColor = {
        {vecs[COLOR][0], vecs[COLOR][1], vecs[COLOR][2]},
        {vecs[COLOR][1], vecs[COLOR][2], vecs[COLOR][3]},
        {vecs[COLOR][2], vecs[COLOR][3], vecs[COLOR][4]},
        {vecs[COLOR][4], vecs[COLOR][3], vecs[COLOR][1]}
    };
    testNode->SetBorderColor(outLineColor);
    testNode->SetBorderWidth(vecs[WIDTH][0], vecs[WIDTH][1], vecs[WIDTH][2], vecs[WIDTH][3]);
    testNode->SetCornerRadius({vecs[RADIUS][0], vecs[RADIUS][1], vecs[RADIUS][2], vecs[RADIUS][3]});
    Vector4<BorderStyle> style = {
        (BorderStyle)vecs[STYLE][0],
        (BorderStyle)vecs[STYLE][1],
        (BorderStyle)vecs[STYLE][2],
        (BorderStyle)vecs[STYLE][3],
    };
    testNode->SetBorderStyle(style);
    testNode->SetBorderDashGap({vecs[DASHGAP][0], vecs[DASHGAP][1], vecs[DASHGAP][2], vecs[DASHGAP][3]});
    testNode->SetBorderDashWidth({vecs[DASHWIDTH][0], vecs[DASHWIDTH][1], vecs[DASHWIDTH][2], vecs[DASHWIDTH][3]});
    return testNode;
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Border_Test01)
{
    vector<vector<float>> vecs = {
        {0, 0, 500, 500}, // rect of bound
        {0x8F00001F, 0x8F00002F, 0x8F00003F, 0x8F00004F}, // color of border
        {20, 20, 10, 30}, // width of border
        {20, 30, 40, 50}, // radius of border
        {0, 2, 1, 1}, // style of border
        {50, 40, 30, 10}, // dashGap of border
        {10, 20, 30, 40} // dashWidth of border
    };
    auto testNode = BorderCreate(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Border_Test02)
{
    vector<vector<float>> vecs = {
        {0, 0, 500, 500}, // rect of bound
        {0x8F00001F, 0x8F00002F, 0x8F00003F, 0x8F00004F}, // color of border
        {10, 20, 30, 40}, // width of border
        {10, 10, 10, 10}, // radius of border
        {0, 0, 0, 0}, // style of border
        {50, 40, 30, 10}, // dashGap of border
        {10, 20, 30, 40} // dashWidth of border
    };
    auto testNode = BorderCreate(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Border_Test03)
{
    vector<vector<float>> vecs = {
        {0, 0, 500, 500}, // rect of bound
        {0x8F00001F, 0x8F00002F, 0x8F00003F, 0x8F00004F}, // color of border
        {10, 20, 30, 40}, // width of border
        {10, 10, 10, 10}, // radius of border
        {1, 1, 1, 1}, // style of border
        {50, 40, 30, 10}, // dashGap of border
        {10, 20, 30, 40} // dashWidth of border
    };
    auto testNode = BorderCreate(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Border_Test04)
{
    vector<vector<float>> vecs = {
        {0, 0, 500, 500}, // rect of bound
        {0x8F00001F, 0x8F00002F, 0x8F00003F, 0x8F00004F}, // color of border
        {10, 20, 30, 40}, // width of border
        {10, 10, 10, 10}, // radius of border
        {2, 2, 2, 2}, // style of border
        {50, 40, 30, 10}, // dashGap of border
        {10, 20, 30, 40} // dashWidth of border
    };
    auto testNode = BorderCreate(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Border_Test05)
{
    vector<vector<float>> vecs = {
        {0, 0, 500, 500}, // rect of bound
        {0x8F00001F, 0x8F00002F, 0x8F00003F, 0x8F00004F}, // color of border
        {10, 20, 30, 40}, // width of border
        {10, 10, 10, 10}, // radius of border
        {0, 0, 1, 1}, // style of border
        {50, 40, 30, 10}, // dashGap of border
        {10, 20, 30, 40} // dashWidth of border
    };
    auto testNode = BorderCreate(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Border_Test06)
{
    vector<vector<float>> vecs = {
        {0, 0, 500, 500}, // rect of bound
        {0x8F00001F, 0x8F00002F, 0x8F00003F, 0x8F00004F}, // color of border
        {10, 20, 30, 40}, // width of border
        {10, 10, 10, 10}, // radius of border
        {1, 1, 2, 2}, // style of border
        {100, 100, 30, 10}, // dashGap of border
        {100, 100, 30, 40} // dashWidth of border
    };
    auto testNode = BorderCreate(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Border_Test07)
{
    vector<vector<float>> vecs = {
        {0, 0, 500, 500}, // rect of bound
        {0x8F97001F, 0x8F98002F, 0x8F99003F, 0x8Fa0004F}, // color of border
        {10, 20, 30, 40}, // width of border
        {10, 10, 10, 10}, // radius of border
        {0, 0, 2, 2}, // style of border
        {0, 0, 0, 0}, // dashGap of border
        {0, 0, 0, 0} // dashWidth of border
    };
    auto testNode = BorderCreate(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Border_Test08)
{
    vector<vector<float>> vecs = {
        {0, 0, 500, 500}, // rect of bound
        {0x8F93001F, 0x8F94002F, 0x8F95003F, 0x8F96004F}, // color of border
        {100, 20, 30, 40}, // width of border
        {100, 20, 10, 10}, // radius of border
        {1, 1, 0, 2}, // style of border
        {50, 40, 0, 0}, // dashGap of border
        {10, 20, 0, 0} // dashWidth of border
    };
    auto testNode = BorderCreate(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Border_Test09)
{
    vector<vector<float>> vecs = {
        {0, 0, 500, 500}, // rect of bound
        {0x8F89001F, 0x8F90002F, 0x8F91003F, 0x8F92004F}, // color of border
        {10, 20, 30, 40}, // width of border
        {10, 10, 10, 10}, // radius of border
        {1, 1, 1, 2}, // style of border
        {100, 50, 30, 0}, // dashGap of border
        {100, 50, 30, 0} // dashWidth of border
    };
    auto testNode = BorderCreate(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Border_Test010)
{
    vector<vector<float>> vecs = {
        {0, 0, 500, 500}, // rect of bound
        {0x8F85001F, 0x8F86002F, 0x8F87003F, 0x8F88004F}, // color of border
        {10, 20, 30, 40}, // width of border
        {10, 10, 10, 10}, // radius of border
        {1, 1, 1, 1}, // style of border
        {50, 40, 100, 10}, // dashGap of border
        {50, 20, 100, 40} // dashWidth of border
    };
    auto testNode = BorderCreate(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Border_Test011)
{
    vector<vector<float>> vecs = {
        {0, 0, 500, 500}, // rect of bound
        {0x8F81001F, 0x8F82002F, 0x8F83003F, 0x8F84004F}, // color of border
        {10, 20, 30, 40}, // width of border
        {10, 10, 10, 10}, // radius of border
        {1, 2, 1, 2}, // style of border
        {100, 0, 10, 0}, // dashGap of border
        {100, 0, 10, 0} // dashWidth of border
    };
    auto testNode = BorderCreate(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Border_Test012)
{
    vector<vector<float>> vecs = {
        {0, 500, 400, 400}, // rect of bound
        {0x8F77001F, 0x8F78002F, 0x8F79003F, 0x8F80004F}, // color of border
        {20, 20, 10, 30}, // width of border
        {20, 30, 40, 50}, // radius of border
        {0, 2, 1, 1}, // style of border
        {50, 40, 30, 10}, // dashGap of border
        {10, 20, 30, 40} // dashWidth of border
    };
    auto testNode = BorderCreate(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Border_Test013)
{
    vector<vector<float>> vecs = {
        {0, 500, 400, 400}, // rect of bound
        {0x8F73001F, 0x8F74002F, 0x8F75003F, 0x8F76004F}, // color of border
        {10, 20, 30, 40}, // width of border
        {10, 10, 10, 10}, // radius of border
        {0, 0, 0, 0}, // style of border
        {50, 40, 30, 10}, // dashGap of border
        {10, 20, 30, 40} // dashWidth of border
    };
    auto testNode = BorderCreate(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Border_Test014)
{
    vector<vector<float>> vecs = {
        {0, 500, 400, 400}, // rect of bound
        {0x8F69001F, 0x8F70002F, 0x8F71003F, 0x8F72004F}, // color of border
        {10, 20, 30, 40}, // width of border
        {10, 10, 10, 10}, // radius of border
        {2, 2, 2, 2}, // style of border
        {50, 40, 30, 10}, // dashGap of border
        {10, 20, 30, 40} // dashWidth of border
    };
    auto testNode = BorderCreate(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Border_Test015)
{
    vector<vector<float>> vecs = {
        {0, 500, 400, 400}, // rect of bound
        {0x8F65001F, 0x8F66002F, 0x8F67003F, 0x8F68004F}, // color of border
        {10, 20, 30, 40}, // width of border
        {10, 10, 10, 10}, // radius of border
        {0, 0, 1, 1}, // style of border
        {50, 40, 30, 10}, // dashGap of border
        {10, 20, 30, 40} // dashWidth of border
    };
    auto testNode = BorderCreate(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Border_Test016)
{
    vector<vector<float>> vecs = {
        {0, 500, 400, 400}, // rect of bound
        {0x8F61001F, 0x8F62002F, 0x8F63003F, 0x8F64004F}, // color of border
        {10, 20, 30, 40}, // width of border
        {10, 10, 10, 10}, // radius of border
        {1, 1, 2, 2}, // style of border
        {100, 100, 30, 10}, // dashGap of border
        {100, 100, 30, 40} // dashWidth of border
    };
    auto testNode = BorderCreate(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Border_Test017)
{
    vector<vector<float>> vecs = {
        {0, 500, 400, 400}, // rect of bound
        {0x8F57001F, 0x8F58002F, 0x8F59003F, 0x8F60004F}, // color of border
        {10, 20, 30, 40}, // width of border
        {10, 10, 10, 10}, // radius of border
        {0, 0, 2, 2}, // style of border
        {0, 0, 0, 0}, // dashGap of border
        {0, 0, 0, 0} // dashWidth of border
    };
    auto testNode = BorderCreate(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Border_Test018)
{
    vector<vector<float>> vecs = {
        {0, 500, 400, 400}, // rect of bound
        {0x8F53001F, 0x8F54002F, 0x8F55003F, 0x8F56004F}, // color of border
        {100, 20, 30, 40}, // width of border
        {100, 20, 10, 10}, // radius of border
        {1, 1, 0, 2}, // style of border
        {50, 40, 0, 0}, // dashGap of border
        {10, 20, 0, 0} // dashWidth of border
    };
    auto testNode = BorderCreate(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Border_Test019)
{
    vector<vector<float>> vecs = {
        {0, 500, 400, 400}, // rect of bound
        {0x8F49001F, 0x8F50002F, 0x8F51003F, 0x8F52004F}, // color of border
        {10, 20, 30, 40}, // width of border
        {10, 10, 10, 10}, // radius of border
        {1, 1, 1, 2}, // style of border
        {100, 50, 30, 0}, // dashGap of border
        {100, 50, 30, 0} // dashWidth of border
    };
    auto testNode = BorderCreate(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Border_Test020)
{
    vector<vector<float>> vecs = {
        {0, 500, 400, 400}, // rect of bound
        {0x8F45001F, 0x8F46002F, 0x8F47003F, 0x8F48004F}, // color of border
        {10, 20, 30, 40}, // width of border
        {10, 10, 10, 10}, // radius of border
        {1, 1, 1, 1}, // style of border
        {50, 40, 100, 10}, // dashGap of border
        {50, 20, 100, 40} // dashWidth of border
    };
    auto testNode = BorderCreate(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Border_Test021)
{
    vector<vector<float>> vecs = {
        {0, 500, 400, 400}, // rect of bound
        {0x8F41001F, 0x8F42002F, 0x8F43003F, 0x8F44004F}, // color of border
        {10, 20, 30, 40}, // width of border
        {10, 10, 10, 10}, // radius of border
        {1, 2, 1, 2}, // style of border
        {100, 0, 10, 0}, // dashGap of border
        {100, 0, 10, 0} // dashWidth of border
    };
    auto testNode = BorderCreate(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Border_Test022)
{
    vector<vector<float>> vecs = {
        {0, 500, 400, 400}, // rect of bound
        {0x8F37001F, 0x8F38002F, 0x8F39003F, 0x8F40004F}, // color of border
        {50, 50, 50, 50}, // width of border
        {0, 0, 10, 80}, // radius of border
        {0, 0, 0, 0}, // style of border
        {50, 40, 30, 10}, // dashGap of border
        {10, 20, 30, 40} // dashWidth of border
    };
    auto testNode = BorderCreate(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Border_Test023)
{
    vector<vector<float>> vecs = {
        {0, 500, 400, 400}, // rect of bound
        {0x8F33001F, 0x8F34002F, 0x8F35003F, 0x8F36004F}, // color of border
        {100, 100, 100, 100}, // width of border
        {80, 0, 10, 80}, // radius of border
        {0, 0, 0, 0}, // style of border
        {50, 40, 30, 10}, // dashGap of border
        {10, 20, 30, 40} // dashWidth of border
    };
    auto testNode = BorderCreate(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Border_Test024)
{
    vector<vector<float>> vecs = {
        {0, 500, 400, 400}, // rect of bound
        {0x8F29001F, 0x8F30002F, 0x8F31003F, 0x8F30004F}, // color of border
        {100, 100, 100, 100}, // width of border
        {80, 0, 10, 80}, // radius of border
        {2, 2, 2, 2}, // style of border
        {50, 40, 30, 10}, // dashGap of border
        {10, 20, 30, 40} // dashWidth of border
    };
    auto testNode = BorderCreate(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Border_Test025)
{
    vector<vector<float>> vecs = {
        {0, 500, 400, 400}, // rect of bound
        {0x8F25001F, 0x8F26002F, 0x8F27003F, 0x8F28004F}, // color of border
        {100, 100, 100, 100}, // width of border
        {80, 0, 10, 80}, // radius of border
        {0, 0, 2, 2}, // style of border
        {50, 40, 30, 10}, // dashGap of border
        {10, 20, 30, 40} // dashWidth of border
    };
    auto testNode = BorderCreate(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Border_Test026)
{
    vector<vector<float>> vecs = {
        {0, 500, 400, 400}, // rect of bound
        {0x8F21001F, 0x8F22002F, 0x8F23003F, 0x8F24004F}, // color of border
        {100, 100, 100, 100}, // width of border
        {80, 0, 10, 80}, // radius of border
        {0, 1, 1, 2}, // style of border
        {50, 0, 0, 10}, // dashGap of border
        {10, 0, 0, 40} // dashWidth of border
    };
    auto testNode = BorderCreate(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Border_Test027)
{
    vector<vector<float>> vecs = {
        {0, 500, 400, 400}, // rect of bound
        {0x8F17001F, 0x8F18002F, 0x8F19003F, 0x8F20004F}, // color of border
        {100, 100, 100, 100}, // width of border
        {80, 0, 10, 80}, // radius of border
        {1, 1, 1, 2}, // style of border
        {0, 0, 0, 0}, // dashGap of border
        {0, 0, 0, 0} // dashWidth of border
    };
    auto testNode = BorderCreate(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Border_Test028)
{
    vector<vector<float>> vecs = {
        {0, 500, 400, 400}, // rect of bound
        {0x8F13001F, 0x8F14002F, 0x8F15003F, 0x8F16004F}, // color of border
        {100, 100, 100, 100}, // width of border
        {80, 0, 10, 80}, // radius of border
        {1, 1, 1, 1}, // style of border
        {0, 0, 0, 0}, // dashGap of border
        {0, 0, 0, 0} // dashWidth of border
    };
    auto testNode = BorderCreate(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Border_Test029)
{
    vector<vector<float>> vecs = {
        {0, 500, 400, 400}, // rect of bound
        {0x8F09001F, 0x8F10002F, 0x8F11003F, 0x8F12004F}, // color of border
        {100, 100, 100, 100}, // width of border
        {80, 0, 10, 80}, // radius of border
        {1, 1, 1, 1}, // style of border
        {80, 80, 80, 80}, // dashGap of border
        {80, 80, 80, 80} // dashWidth of border
    };
    auto testNode = BorderCreate(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Border_Test030)
{
    vector<vector<float>> vecs = {
        {0, 500, 400, 400}, // rect of bound
        {0x8F05001F, 0x8F06002F, 0x8F07003F, 0x8F08004F}, // color of border
        {100, 100, 100, 100}, // width of border
        {80, 0, 10, 80}, // radius of border
        {1, 2, 0, 1}, // style of border
        {80, 80, 80, 80}, // dashGap of border
        {80, 80, 80, 80} // dashWidth of border
    };
    auto testNode = BorderCreate(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Border_Test031)
{
    vector<vector<float>> vecs = {
        {0, 500, 400, 400}, // rect of bound
        {0x8F01001F, 0x8F02002F, 0x8F03003F, 0x8F04004F}, // color of border
        {100, 100, 100, 100}, // width of border
        {100, 100, 100, 100}, // radius of border
        {1, 2, 0, 1}, // style of border
        {100, 100, 100, 100}, // dashGap of border
        {100, 100, 100, 100} // dashWidth of border
    };
    auto testNode = BorderCreate(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Border_Test032)
{
    vector<vector<float>> vecs = {
        {0, 500, 400, 400}, // rect of bound
        {0xff000000, 0x8F38002F, 0x8F39003F, 0x8F40004F}, // color of border
        {0, 50, 50, 50}, // width of border
        {0, 0, 10, 80}, // radius of border
        {0, 0, 0, 0}, // style of border
        {50, 40, 30, 10}, // dashGap of border
        {10, 20, 30, 40} // dashWidth of border
    };
    auto testNode = BorderCreate2(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Border_Test033)
{
    vector<vector<float>> vecs = {
        {0, 500, 400, 400}, // rect of bound
        {0xff000000, 0x8F34002F, 0x8F35003F, 0x8F36004F}, // color of border
        {100, 100, 100, 100}, // width of border
        {80, 0, 10, 80}, // radius of border
        {0, 0, 0, 0}, // style of border
        {50, 40, 30, 10}, // dashGap of border
        {10, 20, 30, 40} // dashWidth of border
    };
    auto testNode = BorderCreate2(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Border_Test034)
{
    vector<vector<float>> vecs = {
        {0, 500, 400, 400}, // rect of bound
        {0xff000000, 0x8F30002F, 0x8F31003F, 0x8F30004F}, // color of border
        {100, 100, 100, 100}, // width of border
        {0, 0, 10, 80}, // radius of border
        {2, 2, 2, 2}, // style of border
        {50, 40, 30, 10}, // dashGap of border
        {10, 20, 30, 40} // dashWidth of border
    };
    auto testNode = BorderCreate2(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Border_Test035)
{
    vector<vector<float>> vecs = {
        {0, 500, 400, 400}, // rect of bound
        {0xff000000, 0x8F26002F, 0x8F27003F, 0x8F28004F}, // color of border
        {100, 100, 100, 100}, // width of border
        {0, 0, 10, 80}, // radius of border
        {1, 0, 2, 2}, // style of border
        {50, 40, 30, 10}, // dashGap of border
        {10, 20, 30, 40} // dashWidth of border
    };
    auto testNode = BorderCreate2(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Border_Test036)
{
    vector<vector<float>> vecs = {
        {0, 500, 400, 400}, // rect of bound
        {0xff000000, 0x8F22002F, 0x8F23003F, 0x8F24004F}, // color of border
        {10, 100, 100, 100}, // width of border
        {80, 0, 10, 80}, // radius of border
        {0, 1, 1, 2}, // style of border
        {50, 0, 0, 10}, // dashGap of border
        {10, 0, 0, 40} // dashWidth of border
    };
    auto testNode = BorderCreate2(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Border_Test037)
{
    vector<vector<float>> vecs = {
        {0, 500, 400, 400}, // rect of bound
        {0xff000000, 0x8F18002F, 0x8F19003F, 0x8F20004F}, // color of border
        {100, 100, 100, 100}, // width of border
        {80, 0, 10, 80}, // radius of border
        {1, 1, 1, 2}, // style of border
        {0, 0, 0, 0}, // dashGap of border
        {0, 0, 0, 0} // dashWidth of border
    };
    auto testNode = BorderCreate2(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Border_Test038)
{
    vector<vector<float>> vecs = {
        {0, 500, 400, 400}, // rect of bound
        {0xff000000, 0x8F14002F, 0x8F15003F, 0x8F16004F}, // color of border
        {100, 100, 100, 100}, // width of border
        {80, 0, 10, 80}, // radius of border
        {1, 1, 1, 1}, // style of border
        {100, 0, 0, 0}, // dashGap of border
        {100, 0, 0, 0} // dashWidth of border
    };
    auto testNode = BorderCreate2(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Border_Test039)
{
    vector<vector<float>> vecs = {
        {0, 500, 400, 400}, // rect of bound
        {0xff000000, 0x8F38002F, 0x8F39003F, 0x8F40004F}, // color of border
        {0, 50, 50, 50}, // width of border
        {0, 0, 10, 80}, // radius of border
        {0, 0, 0, 0}, // style of border
        {50, 40, 30, 10}, // dashGap of border
        {10, 20, 30, 40} // dashWidth of border
    };
    auto testNode = BorderCreate3(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Bound_Test001)
{
    auto testNodeParent = RSCanvasNode::Create();
    setNode(testNodeParent, { 0, 0, 400, 400 }, {0, 0, 0, 0});
    GetRootNode()->AddChild(testNodeParent);
    RegisterNode(testNodeParent);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Bound_Test002)
{
    auto testNodeParent = RSCanvasNode::Create();
    setNode(testNodeParent, { -50, 0, 400, 400 }, {0, 0, 0, 0});
    GetRootNode()->AddChild(testNodeParent);
    RegisterNode(testNodeParent);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Bound_Test003)
{
    auto testNodeParent = RSCanvasNode::Create();
    setNode(testNodeParent, { -50,  -100, 400, 400 }, {0, 0, 0, 0});
    GetRootNode()->AddChild(testNodeParent);
    RegisterNode(testNodeParent);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Bound_Test004)
{
    auto testNodeParent = RSCanvasNode::Create();
    setNode(testNodeParent, { -50,  -100, -400, 400 }, {0, 0, 0, 0});
    GetRootNode()->AddChild(testNodeParent);
    RegisterNode(testNodeParent);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Bound_Test005)
{
    auto testNodeParent = RSCanvasNode::Create();
    setNode(testNodeParent, { -50,  -100, -400, -400 }, {0, 0, 0, 0});
    GetRootNode()->AddChild(testNodeParent);
    RegisterNode(testNodeParent);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Bound_Test006)
{
    auto testNodeParent = RSCanvasNode::Create();
    setNode(testNodeParent, { 100,  -100, -400, -400 }, {0, 0, 0, 0});
    GetRootNode()->AddChild(testNodeParent);
    RegisterNode(testNodeParent);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Bound_Test007)
{
    auto testNodeParent = RSCanvasNode::Create();
    setNode(testNodeParent, { 100,  100, -400, -400 }, {0, 0, 0, 0});
    GetRootNode()->AddChild(testNodeParent);
    RegisterNode(testNodeParent);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Bound_Test008)
{
    auto testNodeParent = RSCanvasNode::Create();
    setNode(testNodeParent, { 100,  100, 400, -400 }, {0, 0, 0, 0});
    GetRootNode()->AddChild(testNodeParent);
    RegisterNode(testNodeParent);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Bound_Test009)
{
    auto testNodeParent = RSCanvasNode::Create();
    setNode(testNodeParent, { 100,  100, 400, 400 }, {0, 0, 0, 0});
    GetRootNode()->AddChild(testNodeParent);
    RegisterNode(testNodeParent);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Bound_Test010)
{
    auto testNodeParent = RSCanvasNode::Create();
    setNode(testNodeParent, { 0, 0, 400, 400 }, {100, 100, 100, 0});
    GetRootNode()->AddChild(testNodeParent);
    RegisterNode(testNodeParent);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Bound_Test011)
{
    auto testNodeParent = RSCanvasNode::Create();
    setNode(testNodeParent, { -50, 0, 400, 400 }, {100, 100, 100, 0});
    GetRootNode()->AddChild(testNodeParent);
    RegisterNode(testNodeParent);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Bound_Test012)
{
    auto testNodeParent = RSCanvasNode::Create();
    setNode(testNodeParent, { -50,  -100, 400, 400 }, {100, 100, 100, 0});
    GetRootNode()->AddChild(testNodeParent);
    RegisterNode(testNodeParent);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Bound_Test013)
{
    auto testNodeParent = RSCanvasNode::Create();
    setNode(testNodeParent, { -50,  -100, -400, 400 }, {100, 100, 100, 0});
    GetRootNode()->AddChild(testNodeParent);
    RegisterNode(testNodeParent);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Bound_Test014)
{
    auto testNodeParent = RSCanvasNode::Create();
    setNode(testNodeParent, { -50,  -100, -400, -400 }, {100, 100, 100, 0});
    GetRootNode()->AddChild(testNodeParent);
    RegisterNode(testNodeParent);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Bound_Test015)
{
    auto testNodeParent = RSCanvasNode::Create();
    setNode(testNodeParent, { 100,  -100, -400, -400 }, {100, 100, 100, 0});
    GetRootNode()->AddChild(testNodeParent);
    RegisterNode(testNodeParent);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Bound_Test016)
{
    auto testNodeParent = RSCanvasNode::Create();
    setNode(testNodeParent, { 100,  100, -400, -400 }, {100, 100, 100, 0});
    GetRootNode()->AddChild(testNodeParent);
    RegisterNode(testNodeParent);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Bound_Test017)
{
    auto testNodeParent = RSCanvasNode::Create();
    setNode(testNodeParent, { 100,  100, 400, -400 }, {100, 100, 100, 0});
    GetRootNode()->AddChild(testNodeParent);
    RegisterNode(testNodeParent);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Bound_Test018)
{
    auto testNodeParent = RSCanvasNode::Create();
    setNode(testNodeParent, { 100,  100, 400, 400 }, {100, 100, 100, 0});
    GetRootNode()->AddChild(testNodeParent);
    RegisterNode(testNodeParent);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Bound_Test019)
{
    auto testNodeParent = RSCanvasNode::Create();
    setNode(testNodeParent, { 0, 0, 400, 400 }, {100, 100, -100, 0});
    GetRootNode()->AddChild(testNodeParent);
    RegisterNode(testNodeParent);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Bound_Test020)
{
    auto testNodeParent = RSCanvasNode::Create();
    setNode(testNodeParent, { -50, 0, 400, 400 }, {100, 100, -100, 0});
    GetRootNode()->AddChild(testNodeParent);
    RegisterNode(testNodeParent);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Bound_Test021)
{
    auto testNodeParent = RSCanvasNode::Create();
    setNode(testNodeParent, { -50,  -100, 400, 400 }, {100, 100, -100, 0});
    GetRootNode()->AddChild(testNodeParent);
    RegisterNode(testNodeParent);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Bound_Test022)
{
    auto testNodeParent = RSCanvasNode::Create();
    setNode(testNodeParent, { -50,  -100, -400, 400 }, {100, 100, -100, 0});
    GetRootNode()->AddChild(testNodeParent);
    RegisterNode(testNodeParent);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Bound_Test023)
{
    auto testNodeParent = RSCanvasNode::Create();
    setNode(testNodeParent, { -50,  -100, -400, -400 }, {100, 100, -100, 0});
    GetRootNode()->AddChild(testNodeParent);
    RegisterNode(testNodeParent);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Bound_Test024)
{
    auto testNodeParent = RSCanvasNode::Create();
    setNode(testNodeParent, { 100,  -100, -400, -400 }, {100, 100, -100, 0});
    GetRootNode()->AddChild(testNodeParent);
    RegisterNode(testNodeParent);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Bound_Test025)
{
    auto testNodeParent = RSCanvasNode::Create();
    setNode(testNodeParent, { 100,  100, -400, -400 }, {100, 100, -100, 0});
    GetRootNode()->AddChild(testNodeParent);
    RegisterNode(testNodeParent);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Bound_Test026)
{
    auto testNodeParent = RSCanvasNode::Create();
    setNode(testNodeParent, { 100,  100, 400, -400 }, {100, 100, -100, 0});
    GetRootNode()->AddChild(testNodeParent);
    RegisterNode(testNodeParent);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Bound_Test027)
{
    auto testNodeParent = RSCanvasNode::Create();
    setNode(testNodeParent, { 100,  100, 400, 400 }, {100, 100, -100, 0});
    GetRootNode()->AddChild(testNodeParent);
    RegisterNode(testNodeParent);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Bound_Test028)
{
    auto testNodeParent = RSCanvasNode::Create();
    setNode(testNodeParent, { 0, 0, 400, 400 }, {-100, -100, -100, 0});
    GetRootNode()->AddChild(testNodeParent);
    RegisterNode(testNodeParent);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Bound_Test029)
{
    auto testNodeParent = RSCanvasNode::Create();
    setNode(testNodeParent, { -50, 0, 400, 400 }, {-100, -100, -100, 0});
    GetRootNode()->AddChild(testNodeParent);
    RegisterNode(testNodeParent);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Bound_Test030)
{
    auto testNodeParent = RSCanvasNode::Create();
    setNode(testNodeParent, { -50,  -100, 400, 400 }, {-100, -100, -100, 0});
    GetRootNode()->AddChild(testNodeParent);
    RegisterNode(testNodeParent);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Bound_Test031)
{
    auto testNodeParent = RSCanvasNode::Create();
    setNode(testNodeParent, { -50,  -100, -400, 400 }, {-100, -100, -100, 0});
    GetRootNode()->AddChild(testNodeParent);
    RegisterNode(testNodeParent);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Bound_Test032)
{
    auto testNodeParent = RSCanvasNode::Create();
    setNode(testNodeParent, { -50,  -100, -400, -400 }, {-100, -100, -100, 0});
    GetRootNode()->AddChild(testNodeParent);
    RegisterNode(testNodeParent);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Bound_Test033)
{
    auto testNodeParent = RSCanvasNode::Create();
    setNode(testNodeParent, { 100,  -100, -400, -400 }, {-100, -100, -100, 0});
    GetRootNode()->AddChild(testNodeParent);
    RegisterNode(testNodeParent);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Bound_Test034)
{
    auto testNodeParent = RSCanvasNode::Create();
    setNode(testNodeParent, { 100,  100, -400, -400 }, {-100, -100, -100, 0});
    GetRootNode()->AddChild(testNodeParent);
    RegisterNode(testNodeParent);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Bound_Test035)
{
    auto testNodeParent = RSCanvasNode::Create();
    setNode(testNodeParent, { 100,  100, 400, -400 }, {-100, -100, -100, 0});
    GetRootNode()->AddChild(testNodeParent);
    RegisterNode(testNodeParent);
}

GRAPHIC_TEST(AppearanceTest02, CONTENT_DISPLAY_TEST, Appearance_Bound_Test036)
{
    auto testNodeParent = RSCanvasNode::Create();
    setNode(testNodeParent, { 100,  100, 400, 400 }, {-100, -100, -100, 0});
    GetRootNode()->AddChild(testNodeParent);
    RegisterNode(testNodeParent);
}
} // namespace OHOS::Rosen