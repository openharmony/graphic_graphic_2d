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
 
static void ClipShapeNode(std::shared_ptr<RSCanvasNode>& node, const RectShape& shape)
{
    node->SetBounds({ 0, 0, 500, 500 });
    node->SetBackgroundColor(0x000ff000);
    node->SetFrame({ 0, 0, 400, 400 });
    auto imageModifier = std::make_shared<ImageCustomModifier>();
    imageModifier->SetWidth(400);
    imageModifier->SetHeight(400);
    imageModifier->SetPixelMapPath("/data/local/tmp/fg_test.jpg");
    node->AddModifier(imageModifier);
    auto textModifier = std::make_shared<TextCustomModifier>();
    textModifier->SetText("ClipShape");
    textModifier->SetFontSize(50.0);
    node->AddModifier(textModifier);
    node->SetClipToFrame(true);
    node->SetClipShape(shape);
}
 
// Matrix test: 3x3 combinations of size and cornerRadius
GRAPHIC_TEST(GeometryClipShapeTest, CONTENT_DISPLAY_TEST, Geometry_ClipShape_Test_1)
{
    float widths[] = { 200.0f, 400.0f, 500.0f };
    Vector4f radii[] = {
        { 0.0f, 0.0f, 0.0f, 0.0f },
        { 50.0f, 50.0f, 50.0f, 50.0f },
        { 100.0f, 100.0f, 100.0f, 100.0f },
    };
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            auto testNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
            testNode->SetBounds({ i * 410.0, j * 500.0, 400.0, 400.0 });
            testNode->SetFrame({ i * 410.0, j * 500.0, 400.0, 400.0 });
            testNode->SetBackgroundColor(0xff00ff00);
            RectShape shape(widths[i], widths[i], radii[j]);
            testNode->SetClipShape(shape);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}
 
// Matrix test: 3x3 combinations with parent-child hierarchy and image
GRAPHIC_TEST(GeometryClipShapeTest, CONTENT_DISPLAY_TEST, Geometry_ClipShape_Test_2)
{
    float widths[] = { 200.0f, 400.0f, 500.0f };
    Vector4f radii[] = {
        { 50.0f, 50.0f, 50.0f, 50.0f },
        { 100.0f, 100.0f, 100.0f, 100.0f },
        { 200.0f, 200.0f, 200.0f, 200.0f },
    };
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            auto testFaNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
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
            auto testChildNode =
                RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
            testChildNode->SetFrame({ 0.0, 50.0, 400.0, 400.0 });
            testChildNode->SetBackgroundColor(0xff00ff00);
            testFaNode->AddChild(testChildNode);
            RectShape shape(widths[i], widths[i], radii[j]);
            testFaNode->SetClipShape(shape);
            GetRootNode()->AddChild(testFaNode);
            RegisterNode(testChildNode);
            RegisterNode(testFaNode);
        }
    }
}
 
// Normal values: full-size, smaller, non-uniform radius, with image and text
GRAPHIC_TEST(GeometryClipShapeTest, CONTENT_DISPLAY_TEST, ClipShape_Normal_Test)
{
    RectShape shapes[] = {
        RectShape(400.0f, 400.0f, Vector4f(50.0f, 50.0f, 50.0f, 50.0f)),
        RectShape(200.0f, 200.0f, Vector4f(30.0f, 30.0f, 30.0f, 30.0f)),
        RectShape(400.0f, 400.0f, Vector4f(80.0f, 20.0f, 80.0f, 20.0f)),
    };
    for (int i = 0; i < 3; i++) {
        auto testNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        testNode->SetBounds({ i * 410.0, 0.0, 400.0, 400.0 });
        ClipShapeNode(testNode, shapes[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}
 
// Boundary and extreme values: zero radius, oversized radius, oversized shape, tiny shape, zero size
GRAPHIC_TEST(GeometryClipShapeTest, CONTENT_DISPLAY_TEST, ClipShape_Boundary_Test)
{
    RectShape shapes[] = {
        RectShape(400.0f, 400.0f, Vector4f(0.0f, 0.0f, 0.0f, 0.0f)),
        RectShape(400.0f, 400.0f, Vector4f(300.0f, 300.0f, 300.0f, 300.0f)),
        RectShape(600.0f, 600.0f, Vector4f(50.0f, 50.0f, 50.0f, 50.0f)),
        RectShape(100.0f, 100.0f, Vector4f(20.0f, 20.0f, 20.0f, 20.0f)),
        RectShape(0.0f, 0.0f, Vector4f(50.0f, 50.0f, 50.0f, 50.0f)),
    };
    for (int i = 0; i < 5; i++) {
        auto testNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        testNode->SetBounds({ i * 410.0, 500.0, 400.0, 400.0 });
        ClipShapeNode(testNode, shapes[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}
 
// Combined: ClipShape with ClipToFrame false, and parent-child with child overflow
GRAPHIC_TEST(GeometryClipShapeTest, CONTENT_DISPLAY_TEST, ClipShape_Combined_Test)
{
    // ClipToFrame false
    auto testNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
    testNode->SetBounds({ 0, 0, 500, 500 });
    testNode->SetBackgroundColor(0x000ff000);
    testNode->SetFrame({ 0, 0, 400, 400 });
    auto imageModifier = std::make_shared<ImageCustomModifier>();
    imageModifier->SetWidth(400);
    imageModifier->SetHeight(400);
    imageModifier->SetPixelMapPath("/data/local/tmp/fg_test.jpg");
    testNode->AddModifier(imageModifier);
    auto textModifier = std::make_shared<TextCustomModifier>();
    textModifier->SetText("ClipShape");
    textModifier->SetFontSize(50.0);
    testNode->AddModifier(textModifier);
    testNode->SetClipToFrame(false);
    RectShape shape(400.0f, 400.0f, Vector4f(50.0f, 50.0f, 50.0f, 50.0f));
    testNode->SetClipShape(shape);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
 
    // Parent-child with child overflow
    auto testFaNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
    testFaNode->SetBounds({ 410.0, 0.0, 400.0, 400.0 });
    testFaNode->SetFrame({ 410.0, 0.0, 400.0, 400.0 });
    testFaNode->SetClipToFrame(true);
    auto testChildNode =
        RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
    testChildNode->SetFrame({ 0.0, 50.0, 400.0, 400.0 });
    testChildNode->SetBackgroundColor(0xff00ff00);
    testFaNode->AddChild(testChildNode);
    RectShape childShape(300.0f, 300.0f, Vector4f(60.0f, 60.0f, 60.0f, 60.0f));
    testFaNode->SetClipShape(childShape);
    GetRootNode()->AddChild(testFaNode);
    RegisterNode(testChildNode);
    RegisterNode(testFaNode);
}
} // namespace OHOS::Rosen