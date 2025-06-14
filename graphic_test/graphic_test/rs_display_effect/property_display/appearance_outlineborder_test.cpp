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

#include "parameters_defination.h"
#include "rs_graphic_test.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class AppearanceTest04 : public RSGraphicTest {
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
        node->SetOutlineColor(outLineColor);
    };
};

// color
GRAPHIC_TEST(AppearanceTest04, CONTENT_DISPLAY_TEST, Appearance_OutlineBorder_Color_Test_1)
{
    Color colorList[] = { Color(0, 0, 0), Color(255, 0, 0), Color(0, 255, 0), Color(0, 0, 255) };

    for (int i = 0; i < FOUR_; i++) {
        int x = (i % TWO_) * FOUR_HUNDRED_TWENTY_;
        int y = (i / TWO_) * FOUR_HUNDRED_TWENTY_;
        auto testNodeColor = RSCanvasNode::Create();
        setNode(testNodeColor, { x, y, FOUR_HUNDRED_, FOUR_HUNDRED_ }, Vector4<Color>(colorList[i]));
        GetRootNode()->AddChild(testNodeColor);
        RegisterNode(testNodeColor);
    }

    // parent black, child red color, white border
    auto testNodeParent = RSCanvasNode::Create();
    setNode(
        testNodeParent, { 0, FOUR_HUNDRED_TWENTY_ * 2, FOUR_HUNDRED_, FOUR_HUNDRED_ }, Vector4<Color>(colorList[0]));
    testNodeParent->SetBackgroundColor(0xff000000);
    GetRootNode()->AddChild(testNodeParent);
    RegisterNode(testNodeParent);

    auto testNodeChild = RSCanvasNode::Create();
    setNode(testNodeChild, { FIFTY_, FIFTY_, TWO_HUNDRED_, TWO_HUNDRED_ }, Vector4<Color>(Color(0xffffffff)));
    testNodeChild->SetForegroundColor(0xffff0000);
    testNodeParent->AddChild(testNodeChild);
    RegisterNode(testNodeChild);

    // alpha border
    auto testNodeAlphaColor = RSCanvasNode::Create();
    setNode(testNodeAlphaColor, { FOUR_HUNDRED_TWENTY_, FOUR_HUNDRED_TWENTY_ * 2, FOUR_HUNDRED_, FOUR_HUNDRED_ },
        Vector4<Color>(Color(0x7dffffff)));
    GetRootNode()->AddChild(testNodeAlphaColor);
    RegisterNode(testNodeAlphaColor);

    // four different color
    auto testNodeFourColor = RSCanvasNode::Create();
    setNode(testNodeFourColor, { 0, FOUR_HUNDRED_TWENTY_ * 3, FOUR_HUNDRED_, FOUR_HUNDRED_ },
        Vector4<Color>(colorList[0], colorList[1], colorList[2], colorList[3]));
    GetRootNode()->AddChild(testNodeFourColor);
    RegisterNode(testNodeFourColor);
}

// width
GRAPHIC_TEST(AppearanceTest04, CONTENT_DISPLAY_TEST, Appearance_OutlineBorder_Width_Test_1)
{
    uint32_t widthList[] = { 0, 5, 120 };
    Vector4<BorderStyle> style = Vector4<BorderStyle>(BorderStyle::SOLID);
    Color color(0, 0, 0);
    Vector4<Color> outLineColor = { color, color, color, color };

    for (int i = 0; i < THREE_; i++) {
        int x = (i % TWO_) * FOUR_HUNDRED_TWENTY_;
        int y = (i / TWO_) * FOUR_HUNDRED_TWENTY_;
        auto testNodeWidth = RSCanvasNode::Create();
        testNodeWidth->SetBounds({ x, y, FOUR_HUNDRED_, FOUR_HUNDRED_ });
        testNodeWidth->SetTranslate(widthList[i] + TWENTY_, widthList[i] + FIFTY_, 0);
        testNodeWidth->SetOutlineStyle(style);
        testNodeWidth->SetOutlineWidth({ widthList[i], widthList[i], widthList[i], widthList[i] });
        testNodeWidth->SetOutlineColor(outLineColor);
        GetRootNode()->AddChild(testNodeWidth);
        RegisterNode(testNodeWidth);
    }

    // four different width
    auto testNodeFourWidth = RSCanvasNode::Create();
    testNodeFourWidth->SetBounds({ 0, FOUR_HUNDRED_TWENTY_ * 2, FOUR_HUNDRED_, FOUR_HUNDRED_ });
    testNodeFourWidth->SetTranslate(TWENTY_, FOUR_HUNDRED_, 0);
    testNodeFourWidth->SetOutlineStyle(style);
    testNodeFourWidth->SetOutlineWidth({ widthList[1] * 0, widthList[1] * 2, widthList[1] * 4, widthList[1] * 8 });
    testNodeFourWidth->SetOutlineColor(outLineColor);
    GetRootNode()->AddChild(testNodeFourWidth);
    RegisterNode(testNodeFourWidth);
}

// Style
// 0 -- Solid
// 1 -- Dash
// 2 -- Dot
// 3 -- None
GRAPHIC_TEST(AppearanceTest04, CONTENT_DISPLAY_TEST, Appearance_OutlineBorder_Style_Test_1)
{
    Color color(0, 0, 0);
    Vector4<Color> outLineColor = { color, color, color, color };

    for (int i = 0; i < FOUR_; i++) {
        int x = (i % TWO_) * FIVE_HUNDRED_TWENTY_;
        int y = (i / TWO_) * FIVE_HUNDRED_TWENTY_;
        auto testNodeStyle = RSCanvasNode::Create();
        testNodeStyle->SetBounds({ x, y, FIVE_HUNDRED_, FIVE_HUNDRED_ });
        testNodeStyle->SetTranslate(TWENTY_, TWENTY_, 0);
        Vector4<BorderStyle> style = Vector4<BorderStyle>((BorderStyle)i);
        testNodeStyle->SetOutlineStyle(style);
        testNodeStyle->SetOutlineWidth({ FIVE_, FIVE_, FIVE_, FIVE_ });
        testNodeStyle->SetOutlineColor(outLineColor);
        GetRootNode()->AddChild(testNodeStyle);
        RegisterNode(testNodeStyle);
    }

    // four different style
    auto testNodeFourStyle = RSCanvasNode::Create();
    testNodeFourStyle->SetBounds({ 0, FIVE_HUNDRED_TWENTY_ * 2, FIVE_HUNDRED_, FIVE_HUNDRED_ });
    testNodeFourStyle->SetTranslate(TWENTY_, TWENTY_, 0);
    Vector4<BorderStyle> style2 = Vector4<BorderStyle>((BorderStyle)0, (BorderStyle)1, (BorderStyle)2, (BorderStyle)3);
    testNodeFourStyle->SetOutlineStyle(style2);
    testNodeFourStyle->SetOutlineWidth({ FIVE_, FIVE_, FIVE_, FIVE_ });
    testNodeFourStyle->SetOutlineColor(outLineColor);
    GetRootNode()->AddChild(testNodeFourStyle);
    RegisterNode(testNodeFourStyle);
}

// Dash Width
GRAPHIC_TEST(AppearanceTest04, CONTENT_DISPLAY_TEST, Appearance_OutlineBorder_DashWidth_Test_1)
{
    uint32_t widthList[] = { 0, 20, 120, TWO_HUNDRED_FIFTY_ };
    Vector4<BorderStyle> style = Vector4<BorderStyle>((BorderStyle)1);
    Color color(0, 0, 0);
    Vector4<Color> outLineColor = { color, color, color, color };

    for (int i = 0; i < FOUR_; i++) {
        int x = (i % TWO_) * FIVE_HUNDRED_TWENTY_;
        int y = (i / TWO_) * FIVE_HUNDRED_TWENTY_;
        auto testNodeDashWidth = RSCanvasNode::Create();
        testNodeDashWidth->SetBounds({ x, y, FIVE_HUNDRED_, FIVE_HUNDRED_ });
        testNodeDashWidth->SetTranslate(TWENTY_, TWENTY_, 0);
        // dash style
        testNodeDashWidth->SetOutlineStyle(style);
        testNodeDashWidth->SetBorderDashWidth({ widthList[i], widthList[i], widthList[i], widthList[i] });
        testNodeDashWidth->SetOutlineWidth({ FIVE_, FIVE_, FIVE_, FIVE_ });
        testNodeDashWidth->SetOutlineColor(outLineColor);
        GetRootNode()->AddChild(testNodeDashWidth);
        RegisterNode(testNodeDashWidth);
    }

    // four different dash width
    auto testNodeFourDashWidth = RSCanvasNode::Create();
    testNodeFourDashWidth->SetBounds({ 0, FIVE_HUNDRED_TWENTY_ * 2, FIVE_HUNDRED_, FIVE_HUNDRED_ });
    testNodeFourDashWidth->SetTranslate(TWENTY_, TWENTY_, 0);
    testNodeFourDashWidth->SetOutlineStyle(style);
    testNodeFourDashWidth->SetBorderDashWidth(
        { widthList[1] * 0, widthList[1] * 2, widthList[1] * 4, widthList[1] * 8 });
    testNodeFourDashWidth->SetOutlineWidth({ FIVE_, FIVE_, FIVE_, FIVE_ });
    testNodeFourDashWidth->SetOutlineColor(outLineColor);
    GetRootNode()->AddChild(testNodeFourDashWidth);
    RegisterNode(testNodeFourDashWidth);

    // not dash style, set dash width
    auto testNodeSolid = RSCanvasNode::Create();
    testNodeSolid->SetBounds({ FIVE_HUNDRED_TWENTY_, FIVE_HUNDRED_TWENTY_ * 2, FIVE_HUNDRED_, FIVE_HUNDRED_ });
    testNodeSolid->SetTranslate(TWENTY_, TWENTY_, 0);
    // solid style
    testNodeSolid->SetOutlineStyle(style);
    testNodeSolid->SetBorderDashWidth({ widthList[1], widthList[1], widthList[1], widthList[1] });
    testNodeSolid->SetOutlineWidth({ FIVE_, FIVE_, FIVE_, FIVE_ });
    testNodeSolid->SetOutlineColor(outLineColor);
    GetRootNode()->AddChild(testNodeSolid);
    RegisterNode(testNodeSolid);
}

GRAPHIC_TEST(AppearanceTest04, CONTENT_DISPLAY_TEST, Appearance_OutlineBorder_OutlineDashWidth_Test_1)
{
    uint32_t widthList[] = { 0, 20, 120, TWO_HUNDRED_FIFTY_ };
    Vector4<BorderStyle> style = Vector4<BorderStyle>((BorderStyle)1);
    Color color(0, 0, 0);
    Vector4<Color> outLineColor = { color, color, color, color };

    for (int i = 0; i < FOUR_; i++) {
        int x = (i % TWO_) * FIVE_HUNDRED_TWENTY_;
        int y = (i / TWO_) * FIVE_HUNDRED_TWENTY_;
        auto testNodeDashWidth = RSCanvasNode::Create();
        testNodeDashWidth->SetBounds({ x, y, FIVE_HUNDRED_, FIVE_HUNDRED_ });
        testNodeDashWidth->SetTranslate(TWENTY_, TWENTY_, 0);
        // dash style
        testNodeDashWidth->SetOutlineStyle(style);
        testNodeDashWidth->SetOutlineDashWidth({ widthList[i], widthList[i], widthList[i], widthList[i] });
        testNodeDashWidth->SetOutlineWidth({ FIVE_, FIVE_, FIVE_, FIVE_ });
        testNodeDashWidth->SetOutlineColor(outLineColor);
        GetRootNode()->AddChild(testNodeDashWidth);
        RegisterNode(testNodeDashWidth);
    }

    // four different dash width
    auto testNodeFourDashWidth = RSCanvasNode::Create();
    testNodeFourDashWidth->SetBounds({ 0, FIVE_HUNDRED_TWENTY_ * 2, FIVE_HUNDRED_, FIVE_HUNDRED_ });
    testNodeFourDashWidth->SetTranslate(TWENTY_, TWENTY_, 0);
    testNodeFourDashWidth->SetOutlineStyle(style);
    testNodeFourDashWidth->SetOutlineDashWidth(
        { widthList[1] * 0, widthList[1] * 2, widthList[1] * 4, widthList[1] * 8 });
    testNodeFourDashWidth->SetOutlineWidth({ FIVE_, FIVE_, FIVE_, FIVE_ });
    testNodeFourDashWidth->SetOutlineColor(outLineColor);
    GetRootNode()->AddChild(testNodeFourDashWidth);
    RegisterNode(testNodeFourDashWidth);

    // not dash style, set dash width
    auto testNodeSolid = RSCanvasNode::Create();
    testNodeSolid->SetBounds({ FIVE_HUNDRED_TWENTY_, FIVE_HUNDRED_TWENTY_ * 2, FIVE_HUNDRED_, FIVE_HUNDRED_ });
    testNodeSolid->SetTranslate(TWENTY_, TWENTY_, 0);
    // solid style
    testNodeSolid->SetOutlineStyle(style);
    testNodeSolid->SetOutlineDashWidth({ widthList[1], widthList[1], widthList[1], widthList[1] });
    testNodeSolid->SetOutlineWidth({ FIVE_, FIVE_, FIVE_, FIVE_ });
    testNodeSolid->SetOutlineColor(outLineColor);
    GetRootNode()->AddChild(testNodeSolid);
    RegisterNode(testNodeSolid);
}

// Dash Gap
GRAPHIC_TEST(AppearanceTest04, CONTENT_DISPLAY_TEST, Appearance_OutlineBorder_DashGap_Test_1)
{
    uint32_t gapList[] = { 0, 20, 120, TWO_HUNDRED_FIFTY_ };
    Vector4<BorderStyle> style = Vector4<BorderStyle>((BorderStyle)1);
    Color color(0, 0, 0);
    Vector4<Color> outLineColor = { color, color, color, color };

    for (int i = 0; i < FOUR_; i++) {
        int x = (i % TWO_) * FIVE_HUNDRED_TWENTY_;
        int y = (i / TWO_) * FIVE_HUNDRED_TWENTY_;
        auto testNodeDashGap = RSCanvasNode::Create();
        testNodeDashGap->SetBounds({ x, y, FIVE_HUNDRED_, FIVE_HUNDRED_ });
        testNodeDashGap->SetTranslate(TWENTY_, TWENTY_, 0);
        // dash style
        testNodeDashGap->SetOutlineStyle(style);
        testNodeDashGap->SetBorderDashGap({ gapList[i], gapList[i], gapList[i], gapList[i] });
        testNodeDashGap->SetOutlineWidth({ FIVE_, FIVE_, FIVE_, FIVE_ });
        testNodeDashGap->SetOutlineColor(outLineColor);
        GetRootNode()->AddChild(testNodeDashGap);
        RegisterNode(testNodeDashGap);
    }

    // four different dash width
    auto testNodeFourDashGap = RSCanvasNode::Create();
    testNodeFourDashGap->SetBounds({ 0, FIVE_HUNDRED_TWENTY_ * 2, FIVE_HUNDRED_, FIVE_HUNDRED_ });
    testNodeFourDashGap->SetTranslate(TWENTY_, TWENTY_, 0);
    testNodeFourDashGap->SetOutlineStyle(style);
    testNodeFourDashGap->SetBorderDashGap({ gapList[1] * 0, gapList[1] * 2, gapList[1] * 4, gapList[1] * 8 });
    testNodeFourDashGap->SetOutlineWidth({ FIVE_, FIVE_, FIVE_, FIVE_ });
    testNodeFourDashGap->SetOutlineColor(outLineColor);
    GetRootNode()->AddChild(testNodeFourDashGap);
    RegisterNode(testNodeFourDashGap);

    // not dash style, set dash width
    auto testNodeSolid = RSCanvasNode::Create();
    testNodeSolid->SetBounds({ FIVE_HUNDRED_TWENTY_, FIVE_HUNDRED_TWENTY_ * 2, FIVE_HUNDRED_, FIVE_HUNDRED_ });
    testNodeSolid->SetTranslate(TWENTY_, TWENTY_, 0);
    // solid style
    testNodeSolid->SetOutlineStyle(style);
    testNodeSolid->SetBorderDashGap({ gapList[1], gapList[1], gapList[1], gapList[1] });
    testNodeSolid->SetOutlineWidth({ FIVE_, FIVE_, FIVE_, FIVE_ });
    testNodeSolid->SetOutlineColor(outLineColor);
    GetRootNode()->AddChild(testNodeSolid);
    RegisterNode(testNodeSolid);
}

GRAPHIC_TEST(AppearanceTest04, CONTENT_DISPLAY_TEST, Appearance_OutlineBorder_OutlineDashGap_Test_1)
{
    uint32_t gapList[] = { 0, 20, 120, TWO_HUNDRED_FIFTY_ };
    Vector4<BorderStyle> style = Vector4<BorderStyle>((BorderStyle)1);
    Color color(0, 0, 0);
    Vector4<Color> outLineColor = { color, color, color, color };

    for (int i = 0; i < FOUR_; i++) {
        int x = (i % TWO_) * FIVE_HUNDRED_TWENTY_;
        int y = (i / TWO_) * FIVE_HUNDRED_TWENTY_;
        auto testNodeDashGap = RSCanvasNode::Create();
        testNodeDashGap->SetBounds({ x, y, FIVE_HUNDRED_, FIVE_HUNDRED_ });
        testNodeDashGap->SetTranslate(TWENTY_, TWENTY_, 0);
        // dash style
        testNodeDashGap->SetOutlineStyle(style);
        testNodeDashGap->SetOutlineDashGap({ gapList[i], gapList[i], gapList[i], gapList[i] });
        testNodeDashGap->SetOutlineWidth({ FIVE_, FIVE_, FIVE_, FIVE_ });
        testNodeDashGap->SetOutlineColor(outLineColor);
        GetRootNode()->AddChild(testNodeDashGap);
        RegisterNode(testNodeDashGap);
    }

    // four different dash width
    auto testNodeFourDashGap = RSCanvasNode::Create();
    testNodeFourDashGap->SetBounds({ 0, FIVE_HUNDRED_TWENTY_ * 2, FIVE_HUNDRED_, FIVE_HUNDRED_ });
    testNodeFourDashGap->SetTranslate(TWENTY_, TWENTY_, 0);
    testNodeFourDashGap->SetOutlineStyle(style);
    testNodeFourDashGap->SetOutlineDashGap({ gapList[1] * 0, gapList[1] * 2, gapList[1] * 4, gapList[1] * 8 });
    testNodeFourDashGap->SetOutlineWidth({ FIVE_, FIVE_, FIVE_, FIVE_ });
    testNodeFourDashGap->SetOutlineColor(outLineColor);
    GetRootNode()->AddChild(testNodeFourDashGap);
    RegisterNode(testNodeFourDashGap);

    // not dash style, set dash width
    auto testNodeSolid = RSCanvasNode::Create();
    testNodeSolid->SetBounds({ FIVE_HUNDRED_TWENTY_, FIVE_HUNDRED_TWENTY_ * 2, FIVE_HUNDRED_, FIVE_HUNDRED_ });
    testNodeSolid->SetTranslate(TWENTY_, TWENTY_, 0);
    // solid style
    testNodeSolid->SetOutlineStyle(style);
    testNodeSolid->SetOutlineDashGap({ gapList[1], gapList[1], gapList[1], gapList[1] });
    testNodeSolid->SetOutlineWidth({ FIVE_, FIVE_, FIVE_, FIVE_ });
    testNodeSolid->SetOutlineColor(outLineColor);
    GetRootNode()->AddChild(testNodeSolid);
    RegisterNode(testNodeSolid);
}

// two node eoverlay & touch
GRAPHIC_TEST(AppearanceTest04, CONTENT_DISPLAY_TEST, Appearance_OutlineBorder_Touch_Test_1)
{
    Color color1(255, 0, 0);
    Vector4<Color> outLineColor1 = { color1, color1, color1, color1 };
    Color color2(0, 0, 255);
    Vector4<Color> outLineColor2 = { color2, color2, color2, color2 };

    uint32_t styleList[] = { 0, 1, 0, 1 };
    float xList[] = { 50, 50, 525, 525 };
    float yList[] = { 50, 50, 20, 20 };
    // overlay
    for (int i = 0; i < FOUR_; i++) {
        int x = (i % TWO_) * FIVE_HUNDRED_SIXTY_;
        int y = (i / TWO_) * FIVE_HUNDRED_TEN_;
        if (i == THREE_) {
            x -= FIVE_HUNDRED_SIXTY_;
            y += FIVE_HUNDRED_TEN_;
        }
        auto testNode1 = RSCanvasNode::Create();
        testNode1->SetBounds({ x, y, FIVE_HUNDRED_, FIVE_HUNDRED_ });
        testNode1->SetTranslate(TWENTY_, TWENTY_, 0);
        testNode1->SetBackgroundColor(0xff000000);
        Vector4<BorderStyle> style = Vector4<BorderStyle>((BorderStyle)styleList[i]);
        testNode1->SetOutlineStyle(style);
        testNode1->SetBorderDashGap({ TWENTY_FIVE_, TWENTY_FIVE_, TWENTY_FIVE_, TWENTY_FIVE_ });
        testNode1->SetOutlineWidth({ TEN_, TEN_, TEN_, TEN_ });
        testNode1->SetOutlineColor(outLineColor1);
        GetRootNode()->AddChild(testNode1);
        RegisterNode(testNode1);

        auto testNode2 = RSCanvasNode::Create();
        testNode2->SetBounds({ x, y, FIVE_HUNDRED_, FIVE_HUNDRED_ });
        testNode2->SetTranslate(xList[i], yList[i], 0);
        testNode2->SetBackgroundColor(0xff000000);
        testNode2->SetOutlineStyle(style);
        testNode2->SetBorderDashGap({ TWENTY_FIVE_, TWENTY_FIVE_, TWENTY_FIVE_, TWENTY_FIVE_ });
        testNode2->SetOutlineWidth({ TEN_, TEN_, TEN_, TEN_ });
        testNode2->SetOutlineColor(outLineColor2);
        GetRootNode()->AddChild(testNode2);
        RegisterNode(testNode2);
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
    TRANSLATE = 7,
};

static RSCanvasNode::SharedPtr OutlineCreate(vector<vector<float>> &vecs)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds(vecs[BOUNDS][0], vecs[BOUNDS][1], vecs[BOUNDS][2], vecs[BOUNDS][3]);
    testNode->SetBackgroundColor(0x8FFF00FF);
    Vector4<Color> outLineColor = {
        {vecs[COLOR][0], vecs[COLOR][1], vecs[COLOR][2]},
        {vecs[COLOR][1], vecs[COLOR][2], vecs[COLOR][3]},
        {vecs[COLOR][2], vecs[COLOR][3], vecs[COLOR][4]},
        {vecs[COLOR][4], vecs[COLOR][3], vecs[COLOR][1]}
    };
    testNode->SetOutlineColor(outLineColor);
    testNode->SetOutlineWidth({vecs[WIDTH][0], vecs[WIDTH][1], vecs[WIDTH][2], vecs[WIDTH][3]});
    testNode->SetOutlineRadius({vecs[RADIUS][0], vecs[RADIUS][1], vecs[RADIUS][2], vecs[RADIUS][3]});
    Vector4<BorderStyle> style = {
        (BorderStyle)vecs[STYLE][0],
        (BorderStyle)vecs[STYLE][1],
        (BorderStyle)vecs[STYLE][2],
        (BorderStyle)vecs[STYLE][3],
    };
    testNode->SetOutlineStyle(style);
    testNode->SetOutlineDashGap({vecs[DASHGAP][0], vecs[DASHGAP][1], vecs[DASHGAP][2], vecs[DASHGAP][3]});
    testNode->SetOutlineDashWidth({vecs[DASHWIDTH][0], vecs[DASHWIDTH][1], vecs[DASHWIDTH][2], vecs[DASHWIDTH][3]});
    testNode->SetTranslate(vecs[TRANSLATE][0], vecs[TRANSLATE][1], vecs[TRANSLATE][2]);
    return testNode;
}

GRAPHIC_TEST(AppearanceTest04, CONTENT_DISPLAY_TEST, Appearance_Outline_Test01)
{
    vector<vector<float>> vecs = {
        {0, 0, 500, 500}, // rect of bound
        {0x8F00001F, 0x8F00002F, 0x8F00003F, 0x8F00004F}, // color of border
        {20, 20, 10, 30}, // width of border
        {20, 30, 40, 50}, // radius of border
        {0, 2, 1, 1}, // style of border
        {50, 40, 30, 10}, // dashGap of border
        {10, 20, 30, 40}, // dashWidth of border
        {TWENTY_, TWENTY_, 0}
    };
    auto testNode = OutlineCreate(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest04, CONTENT_DISPLAY_TEST, Appearance_Outline_Test02)
{
    vector<vector<float>> vecs = {
        {0, 0, 500, 500}, // rect of bound
        {0x8F00001F, 0x8F00002F, 0x8F00003F, 0x8F00004F}, // color of border
        {10, 20, 30, 40}, // width of border
        {10, 10, 10, 10}, // radius of border
        {0, 0, 0, 0}, // style of border
        {50, 40, 30, 10}, // dashGap of border
        {10, 20, 30, 40}, // dashWidth of border
        {TWENTY_, TWENTY_, 0}
    };
    auto testNode = OutlineCreate(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest04, CONTENT_DISPLAY_TEST, Appearance_Outline_Test03)
{
    vector<vector<float>> vecs = {
        {0, 0, 500, 500}, // rect of bound
        {0x8F00001F, 0x8F00002F, 0x8F00003F, 0x8F00004F}, // color of border
        {10, 20, 30, 40}, // width of border
        {10, 10, 10, 10}, // radius of border
        {1, 1, 1, 1}, // style of border
        {50, 40, 30, 10}, // dashGap of border
        {10, 20, 30, 40}, // dashWidth of border
        {TWENTY_, TWENTY_, 0}
    };
    auto testNode = OutlineCreate(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest04, CONTENT_DISPLAY_TEST, Appearance_Outline_Test04)
{
    vector<vector<float>> vecs = {
        {0, 0, 500, 500}, // rect of bound
        {0x8F00001F, 0x8F00002F, 0x8F00003F, 0x8F00004F}, // color of border
        {10, 20, 30, 40}, // width of border
        {10, 10, 10, 10}, // radius of border
        {2, 2, 2, 2}, // style of border
        {50, 40, 30, 10}, // dashGap of border
        {10, 20, 30, 40}, // dashWidth of border
        {TWENTY_, TWENTY_, 0}
    };
    auto testNode = OutlineCreate(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest04, CONTENT_DISPLAY_TEST, Appearance_Outline_Test05)
{
    vector<vector<float>> vecs = {
        {0, 0, 500, 500}, // rect of bound
        {0x8F00001F, 0x8F00002F, 0x8F00003F, 0x8F00004F}, // color of border
        {10, 20, 30, 40}, // width of border
        {10, 10, 10, 10}, // radius of border
        {0, 0, 1, 1}, // style of border
        {50, 40, 30, 10}, // dashGap of border
        {10, 20, 30, 40}, // dashWidth of border
        {TWENTY_, TWENTY_, 0}
    };
    auto testNode = OutlineCreate(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest04, CONTENT_DISPLAY_TEST, Appearance_Outline_Test06)
{
    vector<vector<float>> vecs = {
        {0, 0, 500, 500}, // rect of bound
        {0x8F00001F, 0x8F00002F, 0x8F00003F, 0x8F00004F}, // color of border
        {10, 20, 30, 40}, // width of border
        {10, 10, 10, 10}, // radius of border
        {1, 1, 2, 2}, // style of border
        {100, 100, 30, 10}, // dashGap of border
        {100, 100, 30, 40}, // dashWidth of border
        {TWENTY_, TWENTY_, 0}
    };
    auto testNode = OutlineCreate(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest04, CONTENT_DISPLAY_TEST, Appearance_Outline_Test07)
{
    vector<vector<float>> vecs = {
        {0, 0, 500, 500}, // rect of bound
        {0x8F97001F, 0x8F98002F, 0x8F99003F, 0x8Fa0004F}, // color of border
        {10, 20, 30, 40}, // width of border
        {10, 10, 10, 10}, // radius of border
        {0, 0, 2, 2}, // style of border
        {0, 0, 0, 0}, // dashGap of border
        {0, 0, 0, 0}, // dashWidth of border
        {TWENTY_, TWENTY_, 0}
    };
    auto testNode = OutlineCreate(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest04, CONTENT_DISPLAY_TEST, Appearance_Outline_Test08)
{
    vector<vector<float>> vecs = {
        {0, 0, 500, 500}, // rect of bound
        {0x8F93001F, 0x8F94002F, 0x8F95003F, 0x8F96004F}, // color of border
        {100, 20, 30, 40}, // width of border
        {100, 20, 10, 10}, // radius of border
        {1, 1, 0, 2}, // style of border
        {50, 40, 0, 0}, // dashGap of border
        {10, 20, 0, 0}, // dashWidth of border
        {TWENTY_, TWENTY_, 0}
    };
    auto testNode = OutlineCreate(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest04, CONTENT_DISPLAY_TEST, Appearance_Outline_Test09)
{
    vector<vector<float>> vecs = {
        {0, 0, 500, 500}, // rect of bound
        {0x8F89001F, 0x8F90002F, 0x8F91003F, 0x8F92004F}, // color of border
        {10, 20, 30, 40}, // width of border
        {10, 10, 10, 10}, // radius of border
        {1, 1, 1, 2}, // style of border
        {100, 50, 30, 0}, // dashGap of border
        {100, 50, 30, 0}, // dashWidth of border
        {TWENTY_, TWENTY_, 0}
    };
    auto testNode = OutlineCreate(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest04, CONTENT_DISPLAY_TEST, Appearance_Outline_Test010)
{
    vector<vector<float>> vecs = {
        {0, 0, 500, 500}, // rect of bound
        {0x8F85001F, 0x8F86002F, 0x8F87003F, 0x8F88004F}, // color of border
        {10, 20, 30, 40}, // width of border
        {10, 10, 10, 10}, // radius of border
        {1, 1, 1, 1}, // style of border
        {50, 40, 100, 10}, // dashGap of border
        {50, 20, 100, 40}, // dashWidth of border
        {TWENTY_, TWENTY_, 0}
    };
    auto testNode = OutlineCreate(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest04, CONTENT_DISPLAY_TEST, Appearance_Outline_Test011)
{
    vector<vector<float>> vecs = {
        {0, 0, 500, 500}, // rect of bound
        {0x8F81001F, 0x8F82002F, 0x8F83003F, 0x8F84004F}, // color of border
        {10, 20, 30, 40}, // width of border
        {10, 10, 10, 10}, // radius of border
        {1, 2, 1, 2}, // style of border
        {100, 0, 10, 0}, // dashGap of border
        {100, 0, 10, 0}, // dashWidth of border
        {TWENTY_, TWENTY_, 0}
    };
    auto testNode = OutlineCreate(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest04, CONTENT_DISPLAY_TEST, Appearance_Outline_Test012)
{
    vector<vector<float>> vecs = {
        {0, 500, 400, 400}, // rect of bound
        {0x8F77001F, 0x8F78002F, 0x8F79003F, 0x8F80004F}, // color of border
        {20, 20, 10, 30}, // width of border
        {20, 30, 40, 50}, // radius of border
        {0, 2, 1, 1}, // style of border
        {50, 40, 30, 10}, // dashGap of border
        {10, 20, 30, 40}, // dashWidth of border
        {TWENTY_, TWENTY_, 0}
    };
    auto testNode = OutlineCreate(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest04, CONTENT_DISPLAY_TEST, Appearance_Outline_Test013)
{
    vector<vector<float>> vecs = {
        {0, 500, 400, 400}, // rect of bound
        {0x8F73001F, 0x8F74002F, 0x8F75003F, 0x8F76004F}, // color of border
        {10, 20, 30, 40}, // width of border
        {10, 10, 10, 10}, // radius of border
        {0, 0, 0, 0}, // style of border
        {50, 40, 30, 10}, // dashGap of border
        {10, 20, 30, 40}, // dashWidth of border
        {TWENTY_, TWENTY_, 0}
    };
    auto testNode = OutlineCreate(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest04, CONTENT_DISPLAY_TEST, Appearance_Outline_Test014)
{
    vector<vector<float>> vecs = {
        {0, 500, 400, 400}, // rect of bound
        {0x8F69001F, 0x8F70002F, 0x8F71003F, 0x8F72004F}, // color of border
        {10, 20, 30, 40}, // width of border
        {10, 10, 10, 10}, // radius of border
        {2, 2, 2, 2}, // style of border
        {50, 40, 30, 10}, // dashGap of border
        {10, 20, 30, 40}, // dashWidth of border
        {TWENTY_, TWENTY_, 0}
    };
    auto testNode = OutlineCreate(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest04, CONTENT_DISPLAY_TEST, Appearance_Outline_Test015)
{
    vector<vector<float>> vecs = {
        {0, 500, 400, 400}, // rect of bound
        {0x8F65001F, 0x8F66002F, 0x8F67003F, 0x8F68004F}, // color of border
        {10, 20, 30, 40}, // width of border
        {10, 10, 10, 10}, // radius of border
        {0, 0, 1, 1}, // style of border
        {50, 40, 30, 10}, // dashGap of border
        {10, 20, 30, 40}, // dashWidth of border
        {TWENTY_, TWENTY_, 0}
    };
    auto testNode = OutlineCreate(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest04, CONTENT_DISPLAY_TEST, Appearance_Outline_Test016)
{
    vector<vector<float>> vecs = {
        {0, 500, 400, 400}, // rect of bound
        {0x8F61001F, 0x8F62002F, 0x8F63003F, 0x8F64004F}, // color of border
        {10, 20, 30, 40}, // width of border
        {10, 10, 10, 10}, // radius of border
        {1, 1, 2, 2}, // style of border
        {100, 100, 30, 10}, // dashGap of border
        {100, 100, 30, 40}, // dashWidth of border
        {TWENTY_, TWENTY_, 0}
    };
    auto testNode = OutlineCreate(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest04, CONTENT_DISPLAY_TEST, Appearance_Outline_Test017)
{
    vector<vector<float>> vecs = {
        {0, 500, 400, 400}, // rect of bound
        {0x8F57001F, 0x8F58002F, 0x8F59003F, 0x8F60004F}, // color of border
        {10, 20, 30, 40}, // width of border
        {10, 10, 10, 10}, // radius of border
        {0, 0, 2, 2}, // style of border
        {0, 0, 0, 0}, // dashGap of border
        {0, 0, 0, 0}, // dashWidth of border
        {TWENTY_, TWENTY_, 0}
    };
    auto testNode = OutlineCreate(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest04, CONTENT_DISPLAY_TEST, Appearance_Outline_Test018)
{
    vector<vector<float>> vecs = {
        {0, 500, 400, 400}, // rect of bound
        {0x8F53001F, 0x8F54002F, 0x8F55003F, 0x8F56004F}, // color of border
        {100, 20, 30, 40}, // width of border
        {100, 20, 10, 10}, // radius of border
        {1, 1, 0, 2}, // style of border
        {50, 40, 0, 0}, // dashGap of border
        {10, 20, 0, 0}, // dashWidth of border
        {TWENTY_, TWENTY_, 0}
    };
    auto testNode = OutlineCreate(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest04, CONTENT_DISPLAY_TEST, Appearance_Outline_Test019)
{
    vector<vector<float>> vecs = {
        {0, 500, 400, 400}, // rect of bound
        {0x8F49001F, 0x8F50002F, 0x8F51003F, 0x8F52004F}, // color of border
        {10, 20, 30, 40}, // width of border
        {10, 10, 10, 10}, // radius of border
        {1, 1, 1, 2}, // style of border
        {100, 50, 30, 0}, // dashGap of border
        {100, 50, 30, 0}, // dashWidth of border
        {TWENTY_, TWENTY_, 0}
    };
    auto testNode = OutlineCreate(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest04, CONTENT_DISPLAY_TEST, Appearance_Outline_Test020)
{
    vector<vector<float>> vecs = {
        {0, 500, 400, 400}, // rect of bound
        {0x8F45001F, 0x8F46002F, 0x8F47003F, 0x8F48004F}, // color of border
        {10, 20, 30, 40}, // width of border
        {10, 10, 10, 10}, // radius of border
        {1, 1, 1, 1}, // style of border
        {50, 40, 100, 10}, // dashGap of border
        {50, 20, 100, 40}, // dashWidth of border
        {TWENTY_, TWENTY_, 0}
    };
    auto testNode = OutlineCreate(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest04, CONTENT_DISPLAY_TEST, Appearance_Outline_Test021)
{
    vector<vector<float>> vecs = {
        {0, 500, 400, 400}, // rect of bound
        {0x8F41001F, 0x8F42002F, 0x8F43003F, 0x8F44004F}, // color of border
        {10, 20, 30, 40}, // width of border
        {10, 10, 10, 10}, // radius of border
        {1, 2, 1, 2}, // style of border
        {100, 0, 10, 0}, // dashGap of border
        {100, 0, 10, 0}, // dashWidth of border
        {TWENTY_, TWENTY_, 0}
    };
    auto testNode = OutlineCreate(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest04, CONTENT_DISPLAY_TEST, Appearance_Outline_Test022)
{
    vector<vector<float>> vecs = {
        {0, 500, 400, 400}, // rect of bound
        {0x8F37001F, 0x8F38002F, 0x8F39003F, 0x8F40004F}, // color of border
        {50, 50, 50, 50}, // width of border
        {0, 0, 10, 80}, // radius of border
        {0, 0, 0, 0}, // style of border
        {50, 40, 30, 10}, // dashGap of border
        {10, 20, 30, 40}, // dashWidth of border
        {TWENTY_, TWENTY_, 0}
    };
    auto testNode = OutlineCreate(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest04, CONTENT_DISPLAY_TEST, Appearance_Outline_Test023)
{
    vector<vector<float>> vecs = {
        {0, 500, 400, 400}, // rect of bound
        {0x8F33001F, 0x8F34002F, 0x8F35003F, 0x8F36004F}, // color of border
        {100, 100, 100, 100}, // width of border
        {80, 0, 10, 80}, // radius of border
        {0, 0, 0, 0}, // style of border
        {50, 40, 30, 10}, // dashGap of border
        {10, 20, 30, 40}, // dashWidth of border
        {TWENTY_, TWENTY_, 0}
    };
    auto testNode = OutlineCreate(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest04, CONTENT_DISPLAY_TEST, Appearance_Outline_Test024)
{
    vector<vector<float>> vecs = {
        {0, 500, 400, 400}, // rect of bound
        {0x8F29001F, 0x8F30002F, 0x8F31003F, 0x8F30004F}, // color of border
        {100, 100, 100, 100}, // width of border
        {80, 0, 10, 80}, // radius of border
        {2, 2, 2, 2}, // style of border
        {50, 40, 30, 10}, // dashGap of border
        {10, 20, 30, 40}, // dashWidth of border
        {TWENTY_, TWENTY_, 0}
    };
    auto testNode = OutlineCreate(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest04, CONTENT_DISPLAY_TEST, Appearance_Outline_Test025)
{
    vector<vector<float>> vecs = {
        {0, 500, 400, 400}, // rect of bound
        {0x8F25001F, 0x8F26002F, 0x8F27003F, 0x8F28004F}, // color of border
        {100, 100, 100, 100}, // width of border
        {80, 0, 10, 80}, // radius of border
        {0, 0, 2, 2}, // style of border
        {50, 40, 30, 10}, // dashGap of border
        {10, 20, 30, 40}, // dashWidth of border
        {TWENTY_, TWENTY_, 0}
    };
    auto testNode = OutlineCreate(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest04, CONTENT_DISPLAY_TEST, Appearance_Outline_Test026)
{
    vector<vector<float>> vecs = {
        {0, 500, 400, 400}, // rect of bound
        {0x8F21001F, 0x8F22002F, 0x8F23003F, 0x8F24004F}, // color of border
        {100, 100, 100, 100}, // width of border
        {80, 0, 10, 80}, // radius of border
        {0, 1, 1, 2}, // style of border
        {50, 0, 0, 10}, // dashGap of border
        {10, 0, 0, 40}, // dashWidth of border
        {TWENTY_, TWENTY_, 0}
    };
    auto testNode = OutlineCreate(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest04, CONTENT_DISPLAY_TEST, Appearance_Outline_Test027)
{
    vector<vector<float>> vecs = {
        {0, 500, 400, 400}, // rect of bound
        {0x8F17001F, 0x8F18002F, 0x8F19003F, 0x8F20004F}, // color of border
        {100, 100, 100, 100}, // width of border
        {80, 0, 10, 80}, // radius of border
        {1, 1, 1, 2}, // style of border
        {0, 0, 0, 0}, // dashGap of border
        {0, 0, 0, 0}, // dashWidth of border
        {TWENTY_, TWENTY_, 0}
    };
    auto testNode = OutlineCreate(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest04, CONTENT_DISPLAY_TEST, Appearance_Outline_Test028)
{
    vector<vector<float>> vecs = {
        {0, 500, 400, 400}, // rect of bound
        {0x8F13001F, 0x8F14002F, 0x8F15003F, 0x8F16004F}, // color of border
        {100, 100, 100, 100}, // width of border
        {80, 0, 10, 80}, // radius of border
        {1, 1, 1, 1}, // style of border
        {0, 0, 0, 0}, // dashGap of border
        {0, 0, 0, 0}, // dashWidth of border
        {TWENTY_, TWENTY_, 0}
    };
    auto testNode = OutlineCreate(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest04, CONTENT_DISPLAY_TEST, Appearance_Outline_Test029)
{
    vector<vector<float>> vecs = {
        {0, 500, 400, 400}, // rect of bound
        {0x8F09001F, 0x8F10002F, 0x8F11003F, 0x8F12004F}, // color of border
        {100, 100, 100, 100}, // width of border
        {80, 0, 10, 80}, // radius of border
        {1, 1, 1, 1}, // style of border
        {80, 80, 80, 80}, // dashGap of border
        {80, 80, 80, 80}, // dashWidth of border
        {TWENTY_, TWENTY_, 0}
    };
    auto testNode = OutlineCreate(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest04, CONTENT_DISPLAY_TEST, Appearance_Outline_Test030)
{
    vector<vector<float>> vecs = {
        {0, 500, 400, 400}, // rect of bound
        {0x8F05001F, 0x8F06002F, 0x8F07003F, 0x8F08004F}, // color of border
        {-100, -100, -100, -100}, // width of border
        {80, 0, 10, 80}, // radius of border
        {1, 2, 0, 1}, // style of border
        {80, 80, 80, 80}, // dashGap of border
        {80, 80, 80, 80}, // dashWidth of border
        {TWENTY_, TWENTY_, 0}
    };
    auto testNode = OutlineCreate(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(AppearanceTest04, CONTENT_DISPLAY_TEST, Appearance_Outline_Test031)
{
    vector<vector<float>> vecs = {
        {0, 500, 400, 400}, // rect of bound
        {0x8F01001F, 0x8F02002F, 0x8F03003F, 0x8F04004F}, // color of border
        {600, 600, 600, 600}, // width of border
        {100, 100, 100, 100}, // radius of border
        {1, 2, 0, 1}, // style of border
        {100, 100, 100, 100}, // dashGap of border
        {100, 100, 100, 100}, // dashWidth of border
        {TWENTY_, TWENTY_, 0}
    };
    auto testNode = OutlineCreate(vecs);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

} // namespace OHOS::Rosen