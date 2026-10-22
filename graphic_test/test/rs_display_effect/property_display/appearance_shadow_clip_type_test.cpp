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
 
#include "parameters_defination.h"
#include "rs_graphic_test.h"
#include "rs_graphic_test_director.h"
#include "rs_graphic_test_img.h"
 
using namespace testing;
using namespace testing::ext;
 
namespace OHOS::Rosen {
 
class ShadowClipTypeTest : public RSGraphicTest {
private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;
 
public:
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }
};
 
// Shadow clip type with radius/elevation/filled/path shadow, BOUNDS_SHAPE vs DEFAULT_SHAPE
GRAPHIC_TEST(ShadowClipTypeTest, CONTENT_DISPLAY_TEST, Shadow_ClipType_Normal_Test_1)
{
    int nodePos = 350;
    int nodeOffset = 50;
    int nodeSize = 250;

    // 0: DEFAULT_SHAPE (clip by path), 1: BOUNDS_SHAPE (clip by bounds rrect)
    int clipTypeList[] = { 0, 1 };

    // Irregular star shadow path with tips extending beyond node bounds
    Drawing::Path drawingPath;
    std::vector<Drawing::Point> star = { { 125, -30 }, { 160, 80 }, { 280, 80 },
        { 185, 150 }, { 220, 260 }, { 125, 190 }, { 30, 260 }, { 65, 150 },
        { -30, 80 }, { 90, 80 } };
    drawingPath.AddPoly(star, 10, true);
    auto shadowPath = RSPath::CreateRSPath(drawingPath);

    // Row 0: radius (blur) shadow with star path, DEFAULT_SHAPE vs BOUNDS_SHAPE
    for (int i = 0; i < TWO_; i++) {
        int x = i * nodePos;
        int y = 0;
        auto testNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        testNode->SetBounds({ x, y, nodeSize, nodeSize });
        testNode->SetTranslate(nodeOffset, nodeOffset, 0);
        testNode->SetBorderStyle(0, 0, 0, 0);
        testNode->SetBorderWidth(5, 5, 5, 5);
        testNode->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        testNode->SetShadowColor(0xff000000);
        testNode->SetShadowAlpha(1.0f);
        testNode->SetShadowRadius(50);
        testNode->SetShadowOffset(60, 60);
        testNode->SetShadowPath(shadowPath);
        testNode->SetShadowClipType(clipTypeList[i]);
        testNode->SetCornerRadius(50);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }

    // Row 1: elevation shadow with star path, DEFAULT_SHAPE vs BOUNDS_SHAPE
    for (int i = 0; i < TWO_; i++) {
        int x = i * nodePos;
        int y = nodePos;
        auto testNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        testNode->SetBounds({ x, y, nodeSize, nodeSize });
        testNode->SetTranslate(nodeOffset, nodeOffset, 0);
        testNode->SetBorderStyle(0, 0, 0, 0);
        testNode->SetBorderWidth(5, 5, 5, 5);
        testNode->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        testNode->SetShadowColor(0xff000000);
        testNode->SetShadowAlpha(1.0f);
        testNode->SetShadowElevation(30);
        testNode->SetShadowOffset(60, 60);
        testNode->SetShadowPath(shadowPath);
        testNode->SetShadowClipType(clipTypeList[i]);
        testNode->SetCornerRadius(50);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }

    // Row 2: isFilled=true (clip should not be applied), DEFAULT_SHAPE vs BOUNDS_SHAPE
    for (int i = 0; i < TWO_; i++) {
        int x = i * nodePos;
        int y = 2 * nodePos;
        auto testNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        testNode->SetBounds({ x, y, nodeSize, nodeSize });
        testNode->SetTranslate(nodeOffset, nodeOffset, 0);
        testNode->SetBorderStyle(0, 0, 0, 0);
        testNode->SetBorderWidth(5, 5, 5, 5);
        testNode->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        testNode->SetShadowColor(0xff000000);
        testNode->SetShadowAlpha(1.0f);
        testNode->SetShadowRadius(50);
        testNode->SetShadowOffset(60, 60);
        testNode->SetShadowIsFilled(true);
        testNode->SetShadowClipType(clipTypeList[i]);
        testNode->SetCornerRadius(50);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }

    // Row 3: shadow path with offset, DEFAULT_SHAPE vs BOUNDS_SHAPE
    for (int i = 0; i < TWO_; i++) {
        int x = i * nodePos;
        int y = 3 * nodePos;
        auto testNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        testNode->SetBounds({ x, y, nodeSize, nodeSize });
        testNode->SetTranslate(nodeOffset, nodeOffset, 0);
        testNode->SetBorderStyle(0, 0, 0, 0);
        testNode->SetBorderWidth(5, 5, 5, 5);
        testNode->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        testNode->SetShadowColor(0xff000000);
        testNode->SetShadowAlpha(1.0f);
        testNode->SetShadowRadius(30);
        testNode->SetShadowOffset(40, 40);
        testNode->SetShadowPath(shadowPath);
        testNode->SetShadowClipType(clipTypeList[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}
 
// Shadow clip type BOUNDS_SHAPE with irregular shadow path and different corner radius values
GRAPHIC_TEST(ShadowClipTypeTest, CONTENT_DISPLAY_TEST, Shadow_ClipType_CornerRadius_Test_1)
{
    int nodePos = 500;
    int nodeOffset = 100;
    int nodeSize = 400;

    // Irregular shadow path (star shape) with tips extending beyond node bounds
    Drawing::Path shadowDrawingPath;
    std::vector<Drawing::Point> star = { { 200, -60 }, { 259, 119 }, { 447, 120 },
        { 295, 231 }, { 353, 410 }, { 200, 300 }, { 47, 410 }, { 105, 231 },
        { -47, 120 }, { 141, 119 } };
    shadowDrawingPath.AddPoly(star, 10, true);
    auto shadowPath = RSPath::CreateRSPath(shadowDrawingPath);

    std::vector<Vector4f> radiusList = {
        { 0, 0, 0, 0 },
        { 50, 50, 50, 50 },
        { 100, 100, 100, 100 },
        { 200, 100, 50, 200 }
    };

    // Row 0-1: BOUNDS_SHAPE (clip by bounds rrect), clipType = 1
    for (int i = 0; i < FOUR_; i++) {
        int x = (i % TWO_) * nodePos;
        int y = (i / TWO_) * nodePos;
        auto testNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        testNode->SetBounds({ x, y, nodeSize, nodeSize });
        testNode->SetTranslate(nodeOffset, nodeOffset, 0);
        testNode->SetBorderStyle(0, 0, 0, 0);
        testNode->SetBorderWidth(5, 5, 5, 5);
        testNode->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        testNode->SetShadowColor(0xff000000);
        testNode->SetShadowAlpha(1.0f);
        testNode->SetShadowRadius(80);
        testNode->SetShadowPath(shadowPath);
        testNode->SetShadowClipType(1); // BOUNDS_SHAPE
        testNode->SetCornerRadius(radiusList[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }

    // Row 2-3: DEFAULT_SHAPE (clip by shadow path), clipType = 0, for comparison
    for (int i = 0; i < FOUR_; i++) {
        int x = (i % TWO_) * nodePos;
        int y = (i / TWO_ + TWO_) * nodePos;
        auto testNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        testNode->SetBounds({ x, y, nodeSize, nodeSize });
        testNode->SetTranslate(nodeOffset, nodeOffset, 0);
        testNode->SetBorderStyle(0, 0, 0, 0);
        testNode->SetBorderWidth(5, 5, 5, 5);
        testNode->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        testNode->SetShadowColor(0xff000000);
        testNode->SetShadowAlpha(1.0f);
        testNode->SetShadowRadius(80);
        testNode->SetShadowPath(shadowPath);
        testNode->SetShadowClipType(0); // DEFAULT_SHAPE
        testNode->SetCornerRadius(radiusList[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}
} // namespace OHOS::Rosen
