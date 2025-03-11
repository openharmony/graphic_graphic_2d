/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
class TestTemplateFiveTest : public RSGraphicTest {
public:
    // called before each tests
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }

    // called after each tests
    void AfterEach() override {}

private:
    const int screenWidth = 600;
    const int screenHeight = 1000;
};

GRAPHIC_TEST(TestTemplateFiveTest, CONTENT_DISPLAY_TEST, test1)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({0, 0, screenWidth, screenHeight});
    testNode->SetFrame({0, 0, screenWidth, screenHeight});
    testNode->SetBackgroundColor(0xffffff00);
    GetRootNode()->AddChild(testNode);

    auto testNode2 = RSCanvasNode::Create();
    testNode2->SetBounds({0, 0, screenWidth/2, screenHeight/2});
    testNode2->SetFrame({0, 0, screenWidth/2, screenHeight/2});
    testNode2->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(testNode2);

    // created node should be registered to preserve ref_count
    RegisterNode(testNode);
    RegisterNode(testNode2);
}

GRAPHIC_TEST(TestTemplateFiveTest, CONTENT_DISPLAY_TEST, test2)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({0, 0, screenWidth, screenHeight});
    testNode->SetFrame({0, 0, screenWidth, screenHeight});
    testNode->SetBackgroundColor(0xffffff00);
    GetRootNode()->AddChild(testNode);

    auto testNode2 = RSCanvasNode::Create();
    testNode2->SetBounds({0, 0, screenWidth/2, screenHeight/2});
    testNode2->SetFrame({0, 0, screenWidth/2, screenHeight/2});
    testNode2->SetBackgroundColor(0xff00ff00);
    GetRootNode()->AddChild(testNode2);

    // created node should be registered to preserve ref_count
    RegisterNode(testNode);
    RegisterNode(testNode2);
}

GRAPHIC_TEST(TestTemplateFiveTest, CONTENT_DISPLAY_TEST, test3)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({0, 0, screenWidth, screenHeight});
    testNode->SetFrame({0, 0, screenWidth, screenHeight});
    testNode->SetBackgroundColor(0xffffff00);
    GetRootNode()->AddChild(testNode);

    auto testNode2 = RSCanvasNode::Create();
    testNode2->SetBounds({0, 0, screenWidth/2, screenHeight/2});
    testNode2->SetFrame({0, 0, screenWidth/2, screenHeight/2});
    testNode2->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(testNode2);

    // created node should be registered to preserve ref_count
    RegisterNode(testNode);
    RegisterNode(testNode2);
}

GRAPHIC_TEST(TestTemplateFiveTest, CONTENT_DISPLAY_TEST, test4)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({0, 0, screenWidth, screenHeight});
    testNode->SetFrame({0, 0, screenWidth, screenHeight});
    testNode->SetBackgroundColor(0xffffff00);
    GetRootNode()->AddChild(testNode);

    auto testNode2 = RSCanvasNode::Create();
    testNode2->SetBounds({0, 0, screenWidth/2, screenHeight/2});
    testNode2->SetFrame({0, 0, screenWidth/2, screenHeight/2});
    testNode2->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(testNode2);

    // created node should be registered to preserve ref_count
    RegisterNode(testNode);
    RegisterNode(testNode2);
}

GRAPHIC_TEST(TestTemplateFiveTest, CONTENT_DISPLAY_TEST, test5)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({0, 0, screenWidth, screenHeight});
    testNode->SetFrame({0, 0, screenWidth, screenHeight});
    testNode->SetBackgroundColor(0xffffff00);
    GetRootNode()->AddChild(testNode);

    auto testNode2 = RSCanvasNode::Create();
    testNode2->SetBounds({0, 0, screenWidth/2, screenHeight/2});
    testNode2->SetFrame({0, 0, screenWidth/2, screenHeight/2});
    testNode2->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(testNode2);

    // created node should be registered to preserve ref_count
    RegisterNode(testNode);
    RegisterNode(testNode2);
}

}