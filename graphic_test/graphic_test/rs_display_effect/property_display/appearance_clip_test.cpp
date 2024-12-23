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
        SetScreenSurfaceBounds({ 0, 0, screenWidth, screenHeight });
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
} // namespace OHOS::Rosen