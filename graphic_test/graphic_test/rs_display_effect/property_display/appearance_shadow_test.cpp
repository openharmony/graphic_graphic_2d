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

// shadow color
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Shadow_Color_Test_1)
{
    uint32_t colorList[] = { 0xff000000, 0xffff0000, 0xff00ff00, 0xff0000ff };

    int nodePos = 500;
    int nodeOffset = 100;
    int nodeSize = 400;

    for (int i = 0; i < FOUR_; i++) {
        int x = (i % TWO_) * nodePos;
        int y = (i / TWO_) * nodePos;
        auto testNodeColor = RSCanvasNode::Create();
        testNodeColor->SetBounds({ x, y, nodeSize, nodeSize });
        testNodeColor->SetTranslate(nodeOffset, nodeOffset, 0);
        testNodeColor->SetShadowColor(colorList[i]);
        testNodeColor->SetShadowRadius(FIFTY_);
        GetRootNode()->AddChild(testNodeColor);
        RegisterNode(testNodeColor);
    }
}

// shadow offset
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Shadow_Offset_Test_1)
{
    int nodePos = 500;
    int nodeOffset = 100;
    int nodeSize = 400;

    for (int i = 0; i < FOUR_; i++) {
        int x = (i % TWO_) * nodePos;
        int y = (i / TWO_) * nodePos;
        auto testNodeOffset = RSCanvasNode::Create();
        testNodeOffset->SetBounds({ x, y, nodeSize, nodeSize });
        testNodeOffset->SetTranslate(nodeOffset, nodeOffset, 0);
        testNodeOffset->SetShadowColor(0xff000000);
        switch (i) {
            case 0: {
                testNodeOffset->SetShadowOffset(-50, -50);
                break;
            }
            case 1: {
                testNodeOffset->SetShadowOffset(50, -50);
                break;
            }
            case 2: {
                testNodeOffset->SetShadowOffset(-50, 50);
                break;
            }
            case 3: {
                testNodeOffset->SetShadowOffset(50, 50);
                break;
            }
            default:
                break;
        }
        testNodeOffset->SetShadowRadius(FIFTY_);
        GetRootNode()->AddChild(testNodeOffset);
        RegisterNode(testNodeOffset);
    }
}

GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Shadow_Offset_Test_2)
{
    int nodePos = 500;
    int nodeOffset = 100;
    int nodeSize = 400;

    for (int i = 0; i < FIVE_; i++) {
        int x = (i % TWO_) * nodePos;
        int y = (i / TWO_) * nodePos;
        auto testNodeOffset = RSCanvasNode::Create();
        testNodeOffset->SetBounds({ x, y, nodeSize, nodeSize });
        testNodeOffset->SetTranslate(nodeOffset, nodeOffset, 0);
        testNodeOffset->SetBackgroundColor(0xffc0c0c0);
        testNodeOffset->SetShadowColor(0xff000000);
        testNodeOffset->SetShadowRadius(50);

        switch (i) {
            case 0: {
                testNodeOffset->SetShadowOffset(-250, -250);
                break;
            }
            case 1: {
                testNodeOffset->SetShadowOffset(250, 0);
                break;
            }
            case 2: {
                testNodeOffset->SetShadowOffset(0, 250);
                break;
            }
            case 3: {
                testNodeOffset->SetShadowOffset(250, 250);
                break;
            }
            case 4: {
                testNodeOffset->SetTranslate(0, nodeOffset, 0);
                testNodeOffset->SetShadowOffset(nodeSize, nodeSize);
                break;
            }
            default:
                break;
        }
        GetRootNode()->AddChild(testNodeOffset);
        RegisterNode(testNodeOffset);
    }
}

GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Shadow_Offset_Test_3)
{
    int nodePos = 500;
    int nodeOffset = 100;
    int nodeSize = 400;

    std::vector<Vector4f> radiusList = { { 200, 200, 200, 200 }, { 100, 100, 100, 100 }, { 100, 100, 150, 150 },
        { 50, 50, 50, 50 }, { 200, 1000, 250, 50 } };

    for (int i = 0; i < FIVE_; i++) {
        int x = (i % TWO_) * nodePos;
        int y = (i / TWO_) * nodePos;
        auto testNodeOffset = RSCanvasNode::Create();
        testNodeOffset->SetBounds({ x, y, nodeSize, nodeSize });
        testNodeOffset->SetTranslate(nodeOffset, nodeOffset, 0);
        testNodeOffset->SetBackgroundColor(0xffc0c0c0);
        testNodeOffset->SetCornerRadius(radiusList[i]);
        testNodeOffset->SetShadowColor(0xff000000);
        testNodeOffset->SetShadowRadius(50);

        switch (i) {
            case 0: {
                testNodeOffset->SetShadowOffset(-100, -100);
                break;
            }
            case 1: {
                testNodeOffset->SetShadowOffset(100, 0);
                break;
            }
            case 2: {
                testNodeOffset->SetShadowOffset(0, 100);
                break;
            }
            case 3: {
                testNodeOffset->SetShadowOffset(100, 100);
                break;
            }
            case 4: {
                testNodeOffset->SetTranslate(0, nodeOffset, 0);
                testNodeOffset->SetShadowOffset(nodeSize, nodeSize);
                break;
            }
            default:
                break;
        }
        GetRootNode()->AddChild(testNodeOffset);
        RegisterNode(testNodeOffset);
    }
}

// shadow alpha
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Shadow_Alpha_Test_1)
{
    int nodePos = 500;
    int nodeOffset = 100;
    int nodeSize = 400;

    float alphaList[] = { 0, 0.2, 0.8, 1 };

    for (int i = 0; i < FOUR_; i++) {
        int x = (i % TWO_) * nodePos;
        int y = (i / TWO_) * nodePos;
        auto testNodeAlpha = RSCanvasNode::Create();
        testNodeAlpha->SetBounds({ x, y, nodeSize, nodeSize });
        testNodeAlpha->SetTranslate(nodeOffset, nodeOffset, 0);
        testNodeAlpha->SetBackgroundColor(0xffc0c0c0);
        testNodeAlpha->SetShadowColor(0xff000000);
        testNodeAlpha->SetShadowAlpha(alphaList[i]);
        testNodeAlpha->SetShadowRadius(FIFTY_);
        GetRootNode()->AddChild(testNodeAlpha);
        RegisterNode(testNodeAlpha);
    }
}

// shadow radius
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Shadow_Radius_Test_1)
{
    int nodePos = 500;
    int nodeOffset = 100;
    int nodeSize = 400;

    float radiusList[] = { 0, 50, 250 };

    for (int i = 0; i < THREE_; i++) {
        int x = (i % TWO_) * nodePos;
        int y = (i / TWO_) * nodePos;
        auto testNodeRadius = RSCanvasNode::Create();
        testNodeRadius->SetBounds({ x, y, nodeSize, nodeSize });
        testNodeRadius->SetTranslate(nodeOffset, nodeOffset, 0);
        testNodeRadius->SetBackgroundColor(0xffc0c0c0);
        testNodeRadius->SetShadowColor(0xff000000);
        testNodeRadius->SetShadowRadius(radiusList[i]);
        GetRootNode()->AddChild(testNodeRadius);
        RegisterNode(testNodeRadius);
    }

    // ClipToBounds
    const int yPos = 1250;
    for (int i = 0; i < 2; i++) {
        auto testNodeParent = RSCanvasNode::Create();
        testNodeParent->SetBounds({ i * nodePos, yPos, nodeSize, nodeSize });
        testNodeParent->SetTranslate(nodeOffset, nodeOffset, 0);
        testNodeParent->SetClipToBounds(i);
        GetRootNode()->AddChild(testNodeParent);
        RegisterNode(testNodeParent);

        auto testNodeChild = RSCanvasNode::Create();
        testNodeChild->SetBounds({ 0, 0, nodeSize, nodeSize });
        testNodeChild->SetTranslate(nodeOffset, nodeOffset, 0);
        testNodeChild->SetBackgroundColor(0xffc0c0c0);
        testNodeChild->SetShadowColor(0xff000000);
        testNodeChild->SetShadowRadius(FIFTY_);
        testNodeParent->AddChild(testNodeChild);
        RegisterNode(testNodeChild);
    }
}

GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Shadow_Radius_Test_2)
{
    int nodePos = 500;
    int nodeOffset = 100;
    int nodeSize = 400;

    std::vector<float> radiusList = { 10, 50, 100, 200 };

    // First column draw normal shadow radius
    for (int i = 0; i < radiusList.size(); i++) {
        int x = 0; // first column
        int y = i * nodePos;
        auto testNodeRadius = RSCanvasNode::Create();
        testNodeRadius->SetBounds({ x, y, nodeSize, nodeSize });
        testNodeRadius->SetTranslate(nodeOffset, nodeOffset, 0);
        testNodeRadius->SetBackgroundColor(0xffc0c0c0);
        testNodeRadius->SetShadowColor(0xff000000);
        testNodeRadius->SetShadowRadius(radiusList[i]);
        GetRootNode()->AddChild(testNodeRadius);
        RegisterNode(testNodeRadius);
    }

    // Second column draw shadow radius with elevation
    for (int i = 0; i < radiusList.size(); i++) {
        int x = nodePos; // second column's x position
        int y = i * nodePos;
        auto testNodeRadius = RSCanvasNode::Create();
        testNodeRadius->SetBounds({ x, y, nodeSize, nodeSize });
        testNodeRadius->SetTranslate(nodeOffset, nodeOffset, 0);
        testNodeRadius->SetBackgroundColor(0xffc0c0c0);
        testNodeRadius->SetShadowColor(0xff000000);
        testNodeRadius->SetShadowRadius(radiusList[i]);
        testNodeRadius->SetShadowElevation(radiusList[radiusList.size() - i]); // reverse index
        GetRootNode()->AddChild(testNodeRadius);
        RegisterNode(testNodeRadius);
    }
}

// shadow color strategy
// 0 -- None
// 1 -- Average
// 2 -- Primary
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Shadow_Strategy_Test_1)
{
    float alphaList[] = { 1, 0.2 };
    float radiusList[] = { 50, 100 };
    float offsetList[] = { 0, 50 };

    for (int i = 0; i < SIX_; i++) {
        int x = (i % TWO_) * FIVE_HUNDRED_TEN_;
        int y = (i / TWO_) * FIVE_HUNDRED_TEN_;
        auto testNodeParent = RSCanvasNode::Create();
        testNodeParent->SetBounds({ x, y, FIVE_HUNDRED_, FIVE_HUNDRED_ });
        auto imageModifier = std::make_shared<ImageCustomModifier>();
        imageModifier->SetWidth(FIVE_HUNDRED_);
        imageModifier->SetHeight(FIVE_HUNDRED_);
        imageModifier->SetPixelMapPath("/data/local/tmp/appearance_test.jpg");
        testNodeParent->AddModifier(imageModifier);
        GetRootNode()->AddChild(testNodeParent);
        RegisterNode(testNodeParent);

        auto testNodeChild = RSCanvasNode::Create();
        testNodeChild->SetBounds({ 0, 0, TWO_HUNDRED_FIFTY_, TWO_HUNDRED_FIFTY_ });
        testNodeChild->SetTranslate(ONE_HUNDRED_, ONE_HUNDRED_, 0);
        testNodeChild->SetBackgroundColor(0xffffffff);
        testNodeChild->SetShadowAlpha(alphaList[i % TWO_]);
        testNodeChild->SetShadowOffset(offsetList[i % TWO_], offsetList[i % TWO_]);
        // 1st, 2nd row, Average and Primary, radius
        if (i < 4) {
            testNodeChild->SetShadowColorStrategy((i / TWO_) + 1);
            testNodeChild->SetShadowRadius(radiusList[i % TWO_]);
        }
        // third row, Shadow Elevation, normal shadow
        else {
            testNodeChild->SetShadowColorStrategy(i / TWO_);
            testNodeChild->SetShadowElevation(radiusList[i % TWO_]);
        }
        testNodeParent->AddChild(testNodeChild);
        RegisterNode(testNodeChild);
    }
}

// shadow filled
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Shadow_Filled_Test_1)
{
    int nodePos = 500;
    int nodeOffset = 100;
    int nodeSize = 400;

    float alphaList[] = { 1, 0.5 };

    for (int i = 0; i < FOUR_; i++) {
        int x = (i % TWO_) * nodePos;
        int y = (i / TWO_) * nodePos;
        auto testNodeFilled = RSCanvasNode::Create();
        testNodeFilled->SetBounds({ x, y, nodeSize, nodeSize });
        testNodeFilled->SetTranslate(nodeOffset, nodeOffset, 0);
        auto imageModifier = std::make_shared<ImageCustomModifier>();
        imageModifier->SetWidth(nodeSize);
        imageModifier->SetHeight(nodeSize);
        imageModifier->SetPixelMapPath("/data/local/tmp/appearance_test.jpg");
        testNodeFilled->AddModifier(imageModifier);
        testNodeFilled->SetAlpha(0.5);
        testNodeFilled->SetShadowColor(0xff00ff00);
        testNodeFilled->SetShadowOffset(FIFTY_, FIFTY_);
        testNodeFilled->SetShadowRadius(FIFTY_);
        testNodeFilled->SetShadowIsFilled(i / TWO_);
        testNodeFilled->SetShadowAlpha(alphaList[i % TWO_]);
        GetRootNode()->AddChild(testNodeFilled);
        RegisterNode(testNodeFilled);
    }
}

// shadow mask
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Shadow_mask_Test_1)
{
    const int nodePos = 500;
    const int nodeSize = 400;
    const int nodeOffset = 100;

    const int columnCount = TWO_;

    for (int i = 0; i < columnCount; i++) {
        int x = (i % columnCount) * nodePos;
        int y = (i / columnCount) * nodePos;
        auto testNodeMask = SetUpNodeBgImage("/data/local/tmp/appearance_test.jpg", { x, y, nodeSize, nodeSize });
        testNodeMask->SetTranslate(nodeOffset, nodeOffset, 0);
        testNodeMask->SetShadowMask(i);
        testNodeMask->SetShadowRadius(ONE_HUNDRED_);
        testNodeMask->SetShadowAlpha(0.5);
        GetRootNode()->AddChild(testNodeMask);
        RegisterNode(testNodeMask);
    }

    // With shadowMask, the shadow color will not be shown correctly
    std::vector<uint32_t> colorList = { 0xffff0000, 0xff00ff00, 0xff0000ff };
    for (int i = 0; i < SIX_; i++) {
        int x = (i % columnCount) * nodePos;
        int y = (i / columnCount) * nodePos + nodePos; // start from the second row
        auto testNodeMask = SetUpNodeBgImage("/data/local/tmp/appearance_test.jpg", { x, y, nodeSize, nodeSize });
        testNodeMask->SetTranslate(nodeOffset, nodeOffset, 0);
        testNodeMask->SetShadowColor(colorList[i / columnCount]);
        if (i % columnCount == 0) {
            testNodeMask->SetShadowMask(false);
        } else {
            testNodeMask->SetShadowMask(true);
        }
        testNodeMask->SetShadowRadius(ONE_HUNDRED_);
        GetRootNode()->AddChild(testNodeMask);
        RegisterNode(testNodeMask);
    }
}

// shadow touch
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Shadow_Touch_Test_1)
{
    int nodePos = 500;
    int nodeSize = 400;

    float alphaList[] = { 1, 0.5 };
    uint32_t colorList[] = { 0xff000000, 0xffff0000 };

    for (int i = 0; i < FOUR_; i++) {
        int x = (i % TWO_) * nodePos;
        int y = (i / TWO_) * nodePos;
        auto testNodeTouch = RSCanvasNode::Create();
        testNodeTouch->SetBounds({ x, y, nodeSize, nodeSize });
        testNodeTouch->SetShadowColor(colorList[i]);
        testNodeTouch->SetShadowRadius(ONE_HUNDRED_);
        testNodeTouch->SetShadowAlpha(alphaList[i / TWO_]);
        GetRootNode()->AddChild(testNodeTouch);
        RegisterNode(testNodeTouch);
    }
}

GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Shadow_Filter_Test_1)
{
    int nodePos = 500;
    int nodeSize = 20;

    uint32_t colorList[] = { 0xff000000, 0xffff0000, 0xff00ff00, 0xff0000ff };
    std::vector<Drawing::Path> drawingPath(4, Drawing::Path());
    drawingPath[0].AddRect(0, 0, 200, 450, Drawing::PathDirection::CW_DIRECTION);
    drawingPath[1].AddCircle(160, 260, 245);
    std::vector<Drawing::Point> triangle = { { 50, 50 }, { 250, 50 }, { 150, 250 } };
    drawingPath[2].AddPoly(triangle, 3, true);
    std::vector<Drawing::Point> star = { { 50, 15 }, { 61, 39 }, { 88, 39 }, { 66, 57 }, { 74, 84 }, { 50, 69 },
        { 26, 84 }, { 34, 57 }, { 12, 39 }, { 39, 39 } };
    drawingPath[3].AddPoly(star, 10, true);
    drawingPath[3].AddCircle(160, 260, 145);

    for (int i = 0; i < 4; i++) {
        auto shadowPath = RSPath::CreateRSPath(drawingPath[i]);
        int x = (i % 2) * nodePos;
        int y = (i / 2) * nodePos;
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ x, y, nodeSize, nodeSize });
        testNode->SetBackgroundColor(0xffc0c0c0);
        testNode->SetShadowColor(colorList[i]);
        testNode->SetShadowRadius(30);
        testNode->SetShadowPath(shadowPath);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_Shadow_Filter_Test_2)
{
    int nodePos = 500;
    int nodeSize = 20;

    uint32_t colorList[] = { 0xff000000, 0xffff0000, 0xff00ff00, 0xff0000ff };
    std::vector<Drawing::Path> drawingPath(4, Drawing::Path());
    drawingPath[0].AddRect(0, 0, 200, 450, Drawing::PathDirection::CW_DIRECTION);
    drawingPath[1].AddCircle(160, 260, 245);
    std::vector<Drawing::Point> triangle = { { 50, 50 }, { 250, 50 }, { 150, 250 } };
    drawingPath[2].AddPoly(triangle, 3, true);
    std::vector<Drawing::Point> star = { { 50, 15 }, { 61, 39 }, { 88, 39 }, { 66, 57 }, { 74, 84 }, { 50, 69 },
        { 26, 84 }, { 34, 57 }, { 12, 39 }, { 39, 39 } };
    drawingPath[3].AddPoly(star, 10, true);
    drawingPath[3].AddCircle(160, 260, 145);
    std::vector<bool> filledList = { false, true, false, true };

    for (int i = 0; i < 4; i++) {
        auto shadowPath = RSPath::CreateRSPath(drawingPath[i]);
        int x = (i % 2) * nodePos;
        int y = (i / 2) * nodePos;
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ x, y, nodeSize, nodeSize });
        testNode->SetBackgroundColor(0xffc0c0c0);
        testNode->SetShadowColor(colorList[i]);
        testNode->SetShadowElevation(30);
        testNode->SetShadowOffset(50, 50);
        testNode->SetShadowIsFilled(filledList[i]);
        testNode->SetShadowPath(shadowPath);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

} // namespace OHOS::Rosen