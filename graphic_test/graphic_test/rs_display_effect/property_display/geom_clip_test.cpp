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

class GeometryTest : public RSGraphicTest {
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

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_ClipFlame_Test_1)
{
    Vector4f rect[] = {
        { 0.0, 0.0, 250.0, 250.0 },
        { 100.0, 100.0, 450.0, 450.0 },
        { 100.0, 100.0, 200.0, 200.0 },
    };
    Vector4f radius[] = {
        { 100.0, 100.0, 100.0, 100.0 },
        { 250.0, 250.0, 250.0, 250.0 },
        { 500.0, 500.0, 500.0, 500.0 },
    };
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            auto testFaNode = RSCanvasNode::Create();
            testFaNode->SetBounds({ i * 410.0, j * 500.0, 400.0, 400.0 });
            testFaNode->SetFrame({ i * 410.0, j * 500.0, 400.0, 400.0 });
            auto imageModifier = std::make_shared<ImageCustomModifier>();
            imageModifier->SetWidth(400);
            imageModifier->SetHeight(400);
            imageModifier->SetPixelMapPath("/data/local/tmp/fg_test.jpg");
            testFaNode->AddModifier(imageModifier);
            auto textModifier = std::make_shared<TextCustomModifier>();
            textModifier->SetText("TestText");
            textModifier->SetFontSize(50.0);
            testFaNode->AddModifier(textModifier);
            testFaNode->SetClipToFrame(true);
            auto testNode = RSCanvasNode::Create();
            testNode->SetFrame({ 0.0, 50.0, 400.0, 400.0 });
            testNode->SetBackgroundColor(0xff00ff00);
            testFaNode->AddChild(testNode);
            testFaNode->SetClipRRect(rect[i], radius[j]);
            GetRootNode()->AddChild(testFaNode);
            RegisterNode(testNode);
            RegisterNode(testFaNode);
        }
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_ClipFlame_Test_2)
{
    Vector4f rect[] = {
        { 0.0, 0.0, 250.0, 250.0 },
        { 100.0, 100.0, 450.0, 450.0 },
        { 100.0, 100.0, 200.0, 200.0 },
    };
    Vector4f radius[] = {
        { 100.0, 100.0, 100.0, 100.0 },
        { 250.0, 250.0, 250.0, 250.0 },
        { 500.0, 500.0, 500.0, 500.0 },
    };
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            auto testFaNode = RSCanvasNode::Create();
            testFaNode->SetFrame(i * 410.0, j * 500.0, 400.0, 400.0);
            auto imageModifier = std::make_shared<ImageCustomModifier>();
            imageModifier->SetWidth(400);
            imageModifier->SetHeight(400);
            imageModifier->SetPixelMapPath("/data/local/tmp/fg_test.jpg");
            testFaNode->AddModifier(imageModifier);
            testFaNode->SetClipToFrame(false);
            auto testNode = RSCanvasNode::Create();

            // testNode->SetFrame({ 0.0, 50.0, 400.0, 400.0 }) convert to SetFramePositionXY
            testNode->SetFrame(0.0, 0.0, 400.0, 400.0);
            testNode->SetFramePositionX(0.0);
            testNode->SetFramePositionY(50.0);
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