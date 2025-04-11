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

} // namespace OHOS::Rosen