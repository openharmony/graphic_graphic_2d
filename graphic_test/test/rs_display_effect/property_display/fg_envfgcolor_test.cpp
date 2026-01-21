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

GRAPHIC_TEST(ForegroundTest, CONTENT_DISPLAY_TEST, EnvFgColor_Test_1)
{
    auto testFaNode = RSCanvasNode::Create();
    testFaNode->SetBounds({ 0, 0, 1200, 2000 });
    testFaNode->SetBackgroundColor(0xff000000);

    uint32_t colorList[] = { 0xffff0000, 0xff00ff00, 0xff0000ff, 0xff000000, 0x7d000000, 0x00000000 };
    for (int i = 0; i < 6; i++) {
        int x = (i % 2) * 510;
        int y = (i / 2) * 510;
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ x, y, 500, 500 });
        testNode->SetBackgroundColor(0x7dffffff);
        testNode->SetEnvForegroundColor(colorList[i]);
        testFaNode->AddChild(testNode);
        RegisterNode(testNode);
    }
    GetRootNode()->AddChild(testFaNode);
    RegisterNode(testFaNode);
}

GRAPHIC_TEST(ForegroundTest, CONTENT_DISPLAY_TEST, EnvFgColor_Test_2)
{
    auto testFaNode = RSCanvasNode::Create();
    testFaNode->SetBounds({ 0, 0, 1200, 2000 });
    testFaNode->SetBackgroundColor(0xff000000);

    auto testNode1 = RSCanvasNode::Create();
    testNode1->SetBounds({ 0, 0, 500, 500 });
    testNode1->SetEnvForegroundColor(0xff00ff00);
    auto testNode2 = RSCanvasNode::Create();
    testNode2->SetBounds({ 250, 250, 500, 500 });
    testNode2->SetEnvForegroundColor(0xff0000ff);

    // addNode
    testFaNode->AddChild(testNode1);
    testFaNode->AddChild(testNode2);
    GetRootNode()->AddChild(testFaNode);
    RegisterNode(testFaNode);
    RegisterNode(testNode1);
    RegisterNode(testNode2);
}

GRAPHIC_TEST(ForegroundTest, CONTENT_DISPLAY_TEST, EnvFgColor_StrategyTypeTest_1)
{
    auto testFaNode = RSCanvasNode::Create();
    testFaNode->SetBounds({ 0, 0, 1200, 2000 });
    testFaNode->SetBackgroundColor(0xff000000);

    auto testNode1 = RSCanvasNode::Create();
    testNode1->SetBounds({ 0, 0, 500, 500 });
    testNode1->SetEnvForegroundColorStrategy(ForegroundColorStrategyType::INVALID);
    auto testNode2 = RSCanvasNode::Create();
    testNode2->SetBounds({ 510, 510, 500, 500 });
    testNode2->SetBackgroundColor(0xff000000);
    auto testNode3 = RSCanvasNode::Create();
    testNode3->SetEnvForegroundColorStrategy(ForegroundColorStrategyType::INVERT_BACKGROUNDCOLOR);

    // addNode
    testNode2->AddChild(testNode3);
    testFaNode->AddChild(testNode1);
    testFaNode->AddChild(testNode2);
    GetRootNode()->AddChild(testFaNode);
    RegisterNode(testFaNode);
    RegisterNode(testNode1);
    RegisterNode(testNode2);
}

} // namespace OHOS::Rosen