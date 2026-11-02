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
 
#include "rs_graphic_test.h"
#include "rs_graphic_test_director.h"
#include "rs_graphic_test_img.h"
#include "rs_graphic_test_text.h"
 
#include "common/rs_rect.h"
 
using namespace testing;
using namespace testing::ext;
 
namespace OHOS::Rosen {

class GeometryClipShapeTest : public RSGraphicTest {
private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;

public:
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }
};

static void ClipShapeNode(std::shared_ptr<RSCanvasNode>& node, const RRect& shape, float imgSize)
{
    node->SetBackgroundColor(0xFF0ff000);
    auto imageModifier = std::make_shared<ImageCustomModifier>();
    imageModifier->SetWidth(imgSize);
    imageModifier->SetHeight(imgSize);
    imageModifier->SetPixelMapPath("/data/local/tmp/fg_test.jpg");
    node->AddModifier(imageModifier);
    auto textModifier = std::make_shared<TextCustomModifier>();
    textModifier->SetText("ClipShape");
    textModifier->SetFontSize(50.0);
    node->AddModifier(textModifier);
    node->SetClipToBounds(true);
    node->SetClipShape(shape);
}

// Matrix test: 3x3 combinations of size and cornerRadius (plain + parent-child with image)
// Layout: 3 cols x 6 rows, max X=920, max Y=1780
GRAPHIC_TEST(GeometryClipShapeTest, CONTENT_DISPLAY_TEST, Geometry_ClipShape_Test_1)
{
    constexpr int MATRIX_SIZE = 3;
    constexpr float NODE_SIZE = 280.0f;
    constexpr float STEP_X = 320.0f;
    constexpr float STEP_Y = 300.0f;
    constexpr float IMG_SIZE = 280.0f;
    float widths[] = { 140.0f, 280.0f, 350.0f };
    Vector4f radii[] = {
        { 0.0f, 0.0f, 0.0f, 0.0f },
        { 50.0f, 50.0f, 50.0f, 50.0f },
        { 100.0f, 100.0f, 100.0f, 100.0f },
    };
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            float posX = i * STEP_X;
            float plainY = j * 2 * STEP_Y;       // 0, 600, 1200
            float parentY = j * 2 * STEP_Y + STEP_Y; // 300, 900, 1500

            // Plain node with background color
            auto testNode =
                RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
            testNode->SetBounds({ posX, plainY, NODE_SIZE, NODE_SIZE });
            testNode->SetFrame({ posX, plainY, NODE_SIZE, NODE_SIZE });
            testNode->SetBackgroundColor(0xff00ff00);
            RRect shape(RectT<float>(0.0f, 0.0f, widths[i], widths[i]), radii[j]);
            testNode->SetClipShape(shape);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);

            // Parent-child with image and text
            auto testFaNode =
                RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
            testFaNode->SetBounds({ posX, parentY, NODE_SIZE, NODE_SIZE });
            testFaNode->SetFrame({ posX, parentY, NODE_SIZE, NODE_SIZE });
            auto imageModifier = std::make_shared<ImageCustomModifier>();
            imageModifier->SetWidth(IMG_SIZE);
            imageModifier->SetHeight(IMG_SIZE);
            imageModifier->SetPixelMapPath("/data/local/tmp/fg_test.jpg");
            testFaNode->AddModifier(imageModifier);
            auto textModifier = std::make_shared<TextCustomModifier>();
            textModifier->SetText("TestText");
            textModifier->SetFontSize(50.0);
            testFaNode->AddModifier(textModifier);
            testFaNode->SetClipToBounds(true);
            auto testChildNode =
                RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
            testChildNode->SetFrame({ 0.0, 50.0, NODE_SIZE, NODE_SIZE });
            testChildNode->SetBackgroundColor(0xff00ff00);
            testFaNode->AddChild(testChildNode);
            testFaNode->SetClipShape(shape);
            GetRootNode()->AddChild(testFaNode);
            RegisterNode(testChildNode);
            RegisterNode(testFaNode);
        }
    }
}
 
// Combined: normal, boundary/extreme, ClipToBounds toggle, parent-child overflow
// Layout: Row1 Y=0(h=300), Row2 Y=350(h=200), Row3 Y=600(h=350), max X=1120, max Y=950
GRAPHIC_TEST(GeometryClipShapeTest, CONTENT_DISPLAY_TEST, Geometry_ClipShape_Test_2)
{
    constexpr float NODE_L = 300.0f;
    constexpr float STEP_X_L = 330.0f;
    constexpr float NODE_S = 200.0f;
    constexpr float STEP_X_S = 230.0f;
    constexpr float IMG_L = 300.0f;
    constexpr float IMG_S = 200.0f;
    constexpr float ROW1_Y = 0.0f;
    constexpr float ROW2_Y = 350.0f;
    constexpr float ROW3_Y = 600.0f;

    // --- Row 1: normal values (full-size uniform, small, non-uniform radius) ---
    RRect normalShapes[] = {
        RRect(RectT<float>(0.0f, 0.0f, NODE_L, NODE_L), Vector4f(50.0f, 50.0f, 50.0f, 50.0f)),
        RRect(RectT<float>(0.0f, 0.0f, 200.0f, 200.0f), Vector4f(30.0f, 30.0f, 30.0f, 30.0f)),
        RRect(RectT<float>(0.0f, 0.0f, NODE_L, NODE_L), Vector4f(80.0f, 20.0f, 80.0f, 20.0f)),
    };
    for (int i = 0; i < 3; i++) {
        auto testNode =
            RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        testNode->SetBounds({ i * STEP_X_L, ROW1_Y, NODE_L, NODE_L });
        testNode->SetFrame({ i * STEP_X_L, ROW1_Y, NODE_L, NODE_L });
        ClipShapeNode(testNode, normalShapes[i], IMG_L);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }

    // --- Row 2: boundary and extreme values ---
    RRect boundaryShapes[] = {
        RRect(RectT<float>(0.0f, 0.0f, NODE_S, NODE_S), Vector4f(0.0f, 0.0f, 0.0f, 0.0f)),
        RRect(RectT<float>(0.0f, 0.0f, NODE_S, NODE_S), Vector4f(300.0f, 300.0f, 300.0f, 300.0f)),
        RRect(RectT<float>(0.0f, 0.0f, 600.0f, 600.0f), Vector4f(50.0f, 50.0f, 50.0f, 50.0f)),
        RRect(RectT<float>(0.0f, 0.0f, 100.0f, 100.0f), Vector4f(20.0f, 20.0f, 20.0f, 20.0f)),
        RRect(RectT<float>(0.0f, 0.0f, 0.0f, 0.0f), Vector4f(50.0f, 50.0f, 50.0f, 50.0f)),
    };
    for (int i = 0; i < 5; i++) {
        auto testNode =
            RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        testNode->SetBounds({ i * STEP_X_S, ROW2_Y, NODE_S, NODE_S });
        testNode->SetFrame({ i * STEP_X_S, ROW2_Y, NODE_S, NODE_S });
        ClipShapeNode(testNode, boundaryShapes[i], IMG_S);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }

    // --- Row 3 left: ClipToBounds false (ClipShape should not take effect) ---
    auto clipFalseNode =
        RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
    clipFalseNode->SetBounds({ 0, ROW3_Y, NODE_L, NODE_L });
    clipFalseNode->SetFrame({ 0, ROW3_Y, NODE_L, NODE_L });
    clipFalseNode->SetBackgroundColor(0xFF0ff000);
    auto imageModifier = std::make_shared<ImageCustomModifier>();
    imageModifier->SetWidth(IMG_L);
    imageModifier->SetHeight(IMG_L);
    imageModifier->SetPixelMapPath("/data/local/tmp/fg_test.jpg");
    clipFalseNode->AddModifier(imageModifier);
    auto textModifier = std::make_shared<TextCustomModifier>();
    textModifier->SetText("ClipShape");
    textModifier->SetFontSize(50.0);
    clipFalseNode->AddModifier(textModifier);
    clipFalseNode->SetClipToBounds(false);
    RRect falseShape(RectT<float>(0.0f, 0.0f, NODE_L, NODE_L), Vector4f(50.0f, 50.0f, 50.0f, 50.0f));
    clipFalseNode->SetClipShape(falseShape);
    GetRootNode()->AddChild(clipFalseNode);
    RegisterNode(clipFalseNode);

    // --- Row 3 right: parent-child with child overflow ---
    auto testFaNode =
        RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
    testFaNode->SetBounds({ STEP_X_L, ROW3_Y, NODE_L, NODE_L });
    testFaNode->SetFrame({ STEP_X_L, ROW3_Y, NODE_L, NODE_L });
    testFaNode->SetClipToBounds(true);
    auto testChildNode =
        RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
    testChildNode->SetFrame({ 0.0, 50.0, NODE_L, NODE_L });
    testChildNode->SetBackgroundColor(0xff00ff00);
    testFaNode->AddChild(testChildNode);
    RRect childShape(RectT<float>(0.0f, 0.0f, 300.0f, 300.0f), Vector4f(60.0f, 60.0f, 60.0f, 60.0f));
    testFaNode->SetClipShape(childShape);
    GetRootNode()->AddChild(testFaNode);
    RegisterNode(testChildNode);
    RegisterNode(testFaNode);
}
} // namespace OHOS::Rosen