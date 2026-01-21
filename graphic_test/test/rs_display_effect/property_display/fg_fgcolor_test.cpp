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

class ForegroundTest : public RSGraphicTest {
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

GRAPHIC_TEST(ForegroundTest, CONTENT_DISPLAY_TEST, FgColor_Test_1)
{
    uint32_t colorList[] = { 0xffff0000, 0xff00ff00, 0xff0000ff, 0xff000000, 0x7d000000, 0x00000000 };
    for (int i = 0; i < 6; i++) {
        int x = (i % 2) * 510;
        int y = (i / 2) * 510;
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ x, y, 500, 500 });
        testNode->SetForegroundColor(colorList[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(ForegroundTest, CONTENT_DISPLAY_TEST, FgColor_Test_2)
{
    auto testNode1 = RSCanvasNode::Create();
    testNode1->SetBounds({ 0, 0, 500, 500 });
    testNode1->SetForegroundColor(0xff00ff00);
    auto testNode2 = RSCanvasNode::Create();
    // set offset
    testNode2->SetBounds({ 250, 250, 500, 500 });
    testNode2->SetForegroundColor(0xff0000ff);

    // addNode
    GetRootNode()->AddChild(testNode1);
    GetRootNode()->AddChild(testNode2);
    RegisterNode(testNode1);
    RegisterNode(testNode2);
}

GRAPHIC_TEST(ForegroundTest, CONTENT_DISPLAY_TEST, FgColor_Test_4)
{
    auto testNode1 = RSCanvasNode::Create();
    testNode1->SetBounds({ 0, 0, 500, 500 });
    testNode1->SetForegroundColor(0xff000000);
    GetRootNode()->AddChild(testNode1);
    RegisterNode(testNode1);
}

GRAPHIC_TEST(ForegroundTest, CONTENT_DISPLAY_TEST, FgColor_Test_5)
{
    auto testNode1 = RSCanvasNode::Create();
    testNode1->SetBounds({ -100, 0, 500, 500 });
    testNode1->SetForegroundColor(0xf0f00000);
    GetRootNode()->AddChild(testNode1);
    RegisterNode(testNode1);
}

GRAPHIC_TEST(ForegroundTest, CONTENT_DISPLAY_TEST, FgColor_Test_6)
{
    auto testNode1 = RSCanvasNode::Create();
    testNode1->SetBounds({ 0, -100, 500, 500 });
    testNode1->SetForegroundColor(0xf00f0000);
    GetRootNode()->AddChild(testNode1);
    RegisterNode(testNode1);
}

GRAPHIC_TEST(ForegroundTest, CONTENT_DISPLAY_TEST, FgColor_Test_7)
{
    auto testNode1 = RSCanvasNode::Create();
    testNode1->SetBounds({ 0, 0, -500, 500 });
    testNode1->SetForegroundColor(0xf00f0000);
    GetRootNode()->AddChild(testNode1);
    RegisterNode(testNode1);
}

GRAPHIC_TEST(ForegroundTest, CONTENT_DISPLAY_TEST, FgColor_Test_8)
{
    auto testNode1 = RSCanvasNode::Create();
    testNode1->SetBounds({ 0, 0, 500, -500 });
    testNode1->SetForegroundColor(0xf000f000);
    GetRootNode()->AddChild(testNode1);
    RegisterNode(testNode1);
}

GRAPHIC_TEST(ForegroundTest, CONTENT_DISPLAY_TEST, FgColor_Test_9)
{
    auto testNode1 = RSCanvasNode::Create();
    testNode1->SetBounds({ 0, 0, -500, -500 });
    testNode1->SetForegroundColor(0xf0000f00);
    GetRootNode()->AddChild(testNode1);
    RegisterNode(testNode1);
}

GRAPHIC_TEST(ForegroundTest, CONTENT_DISPLAY_TEST, FgColor_Test_10)
{
    auto testNode1 = RSCanvasNode::Create();
    testNode1->SetBounds({ -100, -100, 500, 500 });
    testNode1->SetForegroundColor(0xf00000f0);
    GetRootNode()->AddChild(testNode1);
    RegisterNode(testNode1);
}

GRAPHIC_TEST(ForegroundTest, CONTENT_DISPLAY_TEST, FgColor_Test_11)
{
    auto testNode1 = RSCanvasNode::Create();
    testNode1->SetBounds({ -100, -100, -500, 500 });
    testNode1->SetForegroundColor(0xf000000f);
    GetRootNode()->AddChild(testNode1);
    RegisterNode(testNode1);
}

GRAPHIC_TEST(ForegroundTest, CONTENT_DISPLAY_TEST, FgColor_Test_12)
{
    auto testNode1 = RSCanvasNode::Create();
    testNode1->SetBounds({ -100, -100, -500, -500 });
    testNode1->SetForegroundColor(0x000ff000);
    GetRootNode()->AddChild(testNode1);
    RegisterNode(testNode1);
}

GRAPHIC_TEST(ForegroundTest, CONTENT_DISPLAY_TEST, FgColor_Test_13)
{
    auto testNode1 = RSCanvasNode::Create();
    testNode1->SetBounds({ 100, 100, 500, 500 });
    testNode1->SetForegroundColor(0x000000ff);
    GetRootNode()->AddChild(testNode1);
    RegisterNode(testNode1);
}
} // namespace OHOS::Rosen