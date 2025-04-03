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
#include "rs_graphic_test_img.h"
#include "rs_graphic_test_text.h"

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

GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, ClipRRect_Test_1)
{
    const Vector4f rect[] = {
        { 0.0, 0.0, 250.0, 250.0 },
        { 100.0, 100.0, 450.0, 450.0 },
        { 100.0, 100.0, 200.0, 200.0 },
    };
    const Vector4f radius[] = {
        { 100.0, 100.0, 100.0, 100.0 },
        { 250.0, 250.0, 250.0, 250.0 },
        { 500.0, 500.0, 500.0, 500.0 },
    };
    const int counts = 3;
    for (int i = 0; i < counts; i++) {
        for (int j = 0; j < counts; j++) {
            Vector4f bounds = { i * 410.0, j * 500.0, 400.0, 400.0 };
            const int nodeSize = 400;
            const int fontSize = 50;

            auto testFaNode = RSCanvasNode::Create();
            testFaNode->SetBounds(bounds);
            testFaNode->SetFrame(bounds);
            auto imageModifier = std::make_shared<ImageCustomModifier>();
            imageModifier->SetWidth(nodeSize);
            imageModifier->SetHeight(nodeSize);
            imageModifier->SetPixelMapPath("/data/local/tmp/fg_test.jpg");
            testFaNode->AddModifier(imageModifier);
            auto textModifier = std::make_shared<TextCustomModifier>();
            textModifier->SetText("TestText");
            textModifier->SetFontSize(fontSize);
            testFaNode->AddModifier(textModifier);
            testFaNode->SetClipToBounds(true);
            auto testNode = RSCanvasNode::Create();
            testNode->SetBounds({ 0.0, fontSize, nodeSize, nodeSize });
            testNode->SetBackgroundColor(0xff00ff00);
            testFaNode->AddChild(testNode);
            testFaNode->SetClipRRect(rect[i], radius[j]);
            GetRootNode()->AddChild(testFaNode);
            RegisterNode(testNode);
            RegisterNode(testFaNode);
        }
    }
}

GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, ClipRRect_Test_2)
{
    const Vector4f rect[] = {
        { 0.0, 0.0, 250.0, 250.0 },
        { 100.0, 100.0, 450.0, 450.0 },
        { 100.0, 100.0, 200.0, 200.0 },
    };
    const Vector4f radius[] = {
        { 100.0, 100.0, 100.0, 100.0 },
        { 250.0, 250.0, 250.0, 250.0 },
        { 500.0, 500.0, 500.0, 500.0 },
    };
    const int counts = 3;
    for (int i = 0; i < counts; i++) {
        for (int j = 0; j < counts; j++) {
            Vector4f bounds = { i * 410.0, j * 500.0, 400.0, 400.0 };
            const int nodeSize = 400;
            const int nodePosY = 50;

            auto testFaNode = RSCanvasNode::Create();
            testFaNode->SetBounds(bounds);
            auto imageModifier = std::make_shared<ImageCustomModifier>();
            imageModifier->SetWidth(nodeSize);
            imageModifier->SetHeight(nodeSize);
            imageModifier->SetPixelMapPath("/data/local/tmp/fg_test.jpg");
            testFaNode->AddModifier(imageModifier);
            testFaNode->SetClipToBounds(false);
            auto testNode = RSCanvasNode::Create();
            testNode->SetBounds({ 0.0, nodePosY, nodeSize, nodeSize });
            testNode->SetBackgroundColor(0xff00ff00);
            testFaNode->AddChild(testNode);
            testFaNode->SetClipRRect(rect[i], radius[j]);
            GetRootNode()->AddChild(testFaNode);
            RegisterNode(testNode);
            RegisterNode(testFaNode);
        }
    }
}

GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, CustomClipToFrame_Test_1)
{
    const Vector4f rect[] = { { 0.1, 0.2, 0.3, 0.4 }, { 0.5, 1.0, 0.5, 1.0 }, { 1, 1, 1, 1 },
        { -0.1, 1.1, -0.2, 2.1 } };

    const int columnCount = 2;
    const int rowCount = 2;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    const int counts = 4;
    for (int i = 0; i < counts; i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNode =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { x, y, sizeX - 10, sizeY - 10 });
        testNode->SetBorderStyle(0, 0, 0, 0);
        testNode->SetBorderWidth(5, 5, 5, 5);
        testNode->SetBorderColor(Vector4<Color>(RgbPalette::Green()));
        testNode->SetClipToFrame(true);
        testNode->SetCustomClipToFrame(rect[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, SetClipBounds_RSPath_Test_1)
{
    int columnCount = 2;
    int rowCount = 4;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;

    std::vector<Drawing::Path> drawingPath(4, Drawing::Path());
    drawingPath[0].AddRect(0, 0, 200, 450, Drawing::PathDirection::CW_DIRECTION);
    drawingPath[1].AddCircle(160, 260, 245);
    std::vector<Drawing::Point> triangle = { { 50, 50 }, { 250, 50 }, { 150, 250 } };
    drawingPath[2].AddPoly(triangle, 3, true);
    std::vector<Drawing::Point> star = { { 50, 15 }, { 61, 39 }, { 88, 39 }, { 66, 57 }, { 74, 84 }, { 50, 69 },
        { 26, 84 }, { 34, 57 }, { 12, 39 }, { 39, 39 } };
    drawingPath[3].AddPoly(star, 10, true);
    drawingPath[3].AddCircle(160, 260, 145);

    for (int i = 0; i < rowCount; i++) {
        auto clipPath = RSPath::CreateRSPath(drawingPath[i]);
        int y = i * sizeY;
        auto testNode1 = RSCanvasNode::Create();
        testNode1->SetBounds({ 0, y, sizeX - 10, sizeY - 10 });
        testNode1->SetBackgroundColor(0xff00ff00);
        testNode1->SetBorderStyle(0, 0, 0, 0);
        testNode1->SetBorderWidth(5, 5, 5, 5);
        testNode1->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        testNode1->SetClipToBounds(true);
        testNode1->SetClipBounds(clipPath);
        GetRootNode()->AddChild(testNode1);
        RegisterNode(testNode1);

        auto testNode2 =
            SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", { sizeX, y, sizeX - 10, sizeY - 10 });
        testNode2->SetBorderStyle(0, 0, 0, 0);
        testNode2->SetBorderWidth(5, 5, 5, 5);
        testNode2->SetBorderColor(Vector4<Color>(RgbPalette::Green()));
        testNode2->SetClipToBounds(true);
        testNode2->SetClipBounds(clipPath);
        GetRootNode()->AddChild(testNode2);
        RegisterNode(testNode2);
    }
}
} // namespace OHOS::Rosen