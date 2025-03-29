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

class AppearanceTest01 : public RSGraphicTest {
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
        float radius = 50.0;
        node->SetCornerRadius(radius);
        node->SetBorderColor(outLineColor);
    };
};

// color
GRAPHIC_TEST(AppearanceTest01, CONTENT_DISPLAY_TEST, Appearance_Border_Radius_Color_Test_1)
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
GRAPHIC_TEST(AppearanceTest01, CONTENT_DISPLAY_TEST, Appearance_Border_Radius_Width_Test_1)
{
    uint32_t widthList[] = { 0, 5, 250, 500 };

    const int nodeCount = 4;
    const int columnCount = 2;
    const int nodeSize = 500;
    const int nodePos = 510;
    const float radiusVal = 50.0f;

    for (int i = 0; i < nodeCount; i++) {
        int x = (i % columnCount) * nodePos;
        int y = (i / columnCount) * nodePos;
        auto testNodeWidth = RSCanvasNode::Create();
        testNodeWidth->SetBounds({ x, y, nodeSize, nodeSize });
        testNodeWidth->SetBorderStyle(0, 0, 0, 0);
        testNodeWidth->SetBorderWidth(widthList[i], widthList[i], widthList[i], widthList[i]);
        testNodeWidth->SetCornerRadius(radiusVal);
        testNodeWidth->SetBorderColor(0xff000000, 0xff000000, 0xff000000, 0xff000000);
        GetRootNode()->AddChild(testNodeWidth);
        RegisterNode(testNodeWidth);
    }

    // four different width
    auto testNodeFourWidth = RSCanvasNode::Create();
    testNodeFourWidth->SetBounds({ 0, nodePos * 2, nodeSize, nodeSize });
    testNodeFourWidth->SetBorderStyle(0, 0, 0, 0);
    testNodeFourWidth->SetBorderWidth(widthList[1] * 0, widthList[1] * 2, widthList[1] * 4, widthList[1] * 8);
    testNodeFourWidth->SetCornerRadius(radiusVal);
    testNodeFourWidth->SetBorderColor(0xff000000, 0xff000000, 0xff000000, 0xff000000);
    GetRootNode()->AddChild(testNodeFourWidth);
    RegisterNode(testNodeFourWidth);
}

// Style
// 0 -- Solid
// 1 -- Dash
// 2 -- Dot
// 3 -- None
GRAPHIC_TEST(AppearanceTest01, CONTENT_DISPLAY_TEST, Appearance_Border_Radius_Style_Test_1)
{
    const int nodeCount = 4;
    const int columnCount = 2;
    const int nodeSize = 500;
    const int nodePos = 510;
    const float radiusVal = 50.0f;
    const int borderWidth = 5;

    for (int i = 0; i < nodeCount; i++) {
        int x = (i % columnCount) * nodePos;
        int y = (i / columnCount) * nodePos;
        auto testNodeStyle = RSCanvasNode::Create();
        testNodeStyle->SetBounds({ x, y, nodeSize, nodeSize });
        testNodeStyle->SetBorderStyle(i, i, i, i);
        testNodeStyle->SetBorderWidth(borderWidth, borderWidth, borderWidth, borderWidth);
        testNodeStyle->SetCornerRadius(radiusVal);
        testNodeStyle->SetBorderColor(0xff000000, 0xff000000, 0xff000000, 0xff000000);
        GetRootNode()->AddChild(testNodeStyle);
        RegisterNode(testNodeStyle);
    }

    // four different style
    auto testNodeFourStyle = RSCanvasNode::Create();
    testNodeFourStyle->SetBounds({ 0, nodePos * 2, nodeSize, nodeSize });
    testNodeFourStyle->SetBorderStyle(0, 1, 2, 3);
    testNodeFourStyle->SetBorderWidth(borderWidth, borderWidth, borderWidth, borderWidth);
    testNodeFourStyle->SetCornerRadius(radiusVal);
    testNodeFourStyle->SetBorderColor(0xff000000, 0xff000000, 0xff000000, 0xff000000);
    GetRootNode()->AddChild(testNodeFourStyle);
    RegisterNode(testNodeFourStyle);
}

// Dash Width
GRAPHIC_TEST(AppearanceTest01, CONTENT_DISPLAY_TEST, Appearance_Border_Radius_DashWidth_Test_1)
{
    uint32_t widthList[] = { 0, 20, 120, 250 };

    const int nodeCount = 4;
    const int columnCount = 2;
    const int nodeSize = 500;
    const int nodePos = 510;
    const float radiusVal = 50.0f;
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
        testNodeDashWidth->SetCornerRadius(radiusVal);
        testNodeDashWidth->SetBorderColor(0xff000000, 0xff000000, 0xff000000, 0xff000000);
        GetRootNode()->AddChild(testNodeDashWidth);
        RegisterNode(testNodeDashWidth);
    }

    // four different dash width
    auto testNodeFourDashWidth = RSCanvasNode::Create();
    testNodeFourDashWidth->SetBounds({ 0, nodePos * 2, nodeSize, nodeSize });
    testNodeFourDashWidth->SetBorderStyle(1, 1, 1, 1);
    testNodeFourDashWidth->SetBorderDashWidth(
        { widthList[1] * 0, widthList[1] * 2, widthList[1] * 4, widthList[1] * 8 });
    testNodeFourDashWidth->SetBorderWidth(borderWidth, borderWidth, borderWidth, borderWidth);
    testNodeFourDashWidth->SetCornerRadius(radiusVal);
    testNodeFourDashWidth->SetBorderColor(0xff000000, 0xff000000, 0xff000000, 0xff000000);
    GetRootNode()->AddChild(testNodeFourDashWidth);
    RegisterNode(testNodeFourDashWidth);

    // not dash style, set dash width
    auto testNodeSolid = RSCanvasNode::Create();
    testNodeSolid->SetBounds({ nodePos, nodePos * 2, nodeSize, nodeSize });
    // solid style
    testNodeSolid->SetBorderStyle(0, 0, 0, 0);
    testNodeSolid->SetBorderDashWidth({ widthList[1], widthList[1], widthList[1], widthList[1] });
    testNodeSolid->SetBorderWidth(borderWidth, borderWidth, borderWidth, borderWidth);
    testNodeSolid->SetCornerRadius(radiusVal);
    testNodeSolid->SetBorderColor(0xff000000, 0xff000000, 0xff000000, 0xff000000);
    GetRootNode()->AddChild(testNodeSolid);
    RegisterNode(testNodeSolid);
}

// Dash Gap
GRAPHIC_TEST(AppearanceTest01, CONTENT_DISPLAY_TEST, Appearance_Border_Radius_DashGap_Test_1)
{
    uint32_t gapList[] = { 0, 20, 120, 250 };

    const int nodeCount = 4;
    const int columnCount = 2;
    const int nodeSize = 500;
    const int nodePos = 510;
    const float radiusVal = 50.0f;
    const int borderWidth = 5;

    for (int i = 0; i < nodeCount; i++) {
        int x = (i % columnCount) * 510;
        int y = (i / columnCount) * 510;
        auto testNodeDashGap = RSCanvasNode::Create();
        testNodeDashGap->SetBounds({ x, y, nodeSize, nodeSize });
        // dash style
        testNodeDashGap->SetBorderStyle(1, 1, 1, 1);
        testNodeDashGap->SetBorderDashGap({ gapList[i], gapList[i], gapList[i], gapList[i] });
        testNodeDashGap->SetBorderWidth(borderWidth, borderWidth, borderWidth, borderWidth);
        testNodeDashGap->SetCornerRadius(radiusVal);
        testNodeDashGap->SetBorderColor(0xff000000, 0xff000000, 0xff000000, 0xff000000);
        GetRootNode()->AddChild(testNodeDashGap);
        RegisterNode(testNodeDashGap);
    }

    // four different dash width
    auto testNodeFourDashGap = RSCanvasNode::Create();
    testNodeFourDashGap->SetBounds({ 0, nodePos * 2, nodeSize, nodeSize });
    testNodeFourDashGap->SetBorderStyle(1, 1, 1, 1);
    testNodeFourDashGap->SetBorderDashGap({ gapList[1] * 0, gapList[1] * 2, gapList[1] * 4, gapList[1] * 8 });
    testNodeFourDashGap->SetBorderWidth(borderWidth, borderWidth, borderWidth, borderWidth);
    testNodeFourDashGap->SetCornerRadius(radiusVal);
    testNodeFourDashGap->SetBorderColor(0xff000000, 0xff000000, 0xff000000, 0xff000000);
    GetRootNode()->AddChild(testNodeFourDashGap);
    RegisterNode(testNodeFourDashGap);

    // not dash style, set dash width
    auto testNodeSolid = RSCanvasNode::Create();
    testNodeSolid->SetBounds({ nodePos, nodePos * 2, nodeSize, nodeSize });
    // solid style
    testNodeSolid->SetBorderStyle(0, 0, 0, 0);
    testNodeSolid->SetBorderDashGap({ gapList[1], gapList[1], gapList[1], gapList[1] });
    testNodeSolid->SetBorderWidth(borderWidth, borderWidth, borderWidth, borderWidth);
    testNodeSolid->SetCornerRadius(radiusVal);
    testNodeSolid->SetBorderColor(0xff000000, 0xff000000, 0xff000000, 0xff000000);
    GetRootNode()->AddChild(testNodeSolid);
    RegisterNode(testNodeSolid);
}

// two node eoverlay & touch
GRAPHIC_TEST(AppearanceTest01, CONTENT_DISPLAY_TEST, Appearance_Border_Radius_Touch_Test_1)
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
    const float radiusVal = 50.0f;
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
        testNode1->SetBorderStyle(styleList[i], styleList[i], styleList[i], styleList[i]);
        testNode1->SetBorderDashGap({ dashGap, dashGap, dashGap, dashGap });
        testNode1->SetBorderWidth(borderWidth, borderWidth, borderWidth, borderWidth);
        testNode1->SetCornerRadius(radiusVal);
        testNode1->SetBorderColor(0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000);
        GetRootNode()->AddChild(testNode1);
        RegisterNode(testNode1);

        auto testNode2 = RSCanvasNode::Create();
        testNode2->SetBounds({ x, y, nodeSize, nodeSize });
        testNode2->SetTranslate(xList[i], yList[i], 0);
        testNode2->SetBackgroundColor(0xff000000);
        testNode2->SetBorderStyle(styleList[i], styleList[i], styleList[i], styleList[i]);
        testNode2->SetBorderDashGap({ dashGap, dashGap, dashGap, dashGap });
        testNode2->SetBorderWidth(borderWidth, borderWidth, borderWidth, borderWidth);
        testNode2->SetCornerRadius(radiusVal);
        testNode2->SetBorderColor(0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff);
        GetRootNode()->AddChild(testNode2);
        RegisterNode(testNode2);
    }
}

} // namespace OHOS::Rosen