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
#include "rs_graphic_test_img.h"
 
#include "ui/rs_canvas_node.h"
#include "ui/rs_node.h"
 
using namespace testing;
using namespace testing::ext;
 
namespace OHOS::Rosen {
class CustomDrawTest : public RSGraphicTest {
public:
    // called before each tests
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }
 
    void setNode(std::shared_ptr<RSNode>& node, const Vector4f& bounds, Vector4<Color> borderColor)
    {
        node->SetBounds(bounds);
        int transXY = 20;
        int transZ = 0;
        node->SetTranslate(transXY, transXY, transZ);
        Vector4<BorderStyle> style = Vector4<BorderStyle>(BorderStyle::SOLID);
        node->SetBorderStyle(style);
        Vector4f borderWidth = { 5, 5, 5, 5 };
        node->SetBorderWidth(borderWidth);
        node->SetBorderColor(borderColor);
    }
 
private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;
};
 
GRAPHIC_TEST(CustomDrawTest, CONTENT_DISPLAY_TEST, DrawOnNode_Test)
{
    int width = 1000;
    int height = 1000;
    auto testNode1 = RSCanvasNode::Create(true);
    auto node = std::static_pointer_cast<RSNode>(testNode1);
    setNode(node, { 0, 0, width, height }, Vector4<Color>(Color(0, 255, 0)));
    testNode1->DrawOnNode(RSModifierType::CONTENT_STYLE, [testNode1](std::shared_ptr<Drawing::Canvas> canvas) {
        Drawing::Brush brush;
        brush.SetColor(Drawing::Color::COLOR_CYAN);
        canvas->AttachBrush(brush);
        canvas->DrawRect(Drawing::Rect(0, 0, 500, 500));
        if (testNode1->IsRecording()) {
            brush.SetColor(Drawing::Color::COLOR_YELLOW);
            canvas->DrawRect(Drawing::Rect(0, 0, 10, 10));
        }
        canvas->DetachBrush();
    });
    GetRootNode()->AddChild(testNode1);
    RegisterNode(testNode1);
 
    auto testNode2 = std::make_shared<RSNode>(true, true);
    setNode(testNode2, { 500, 500, width, height }, Vector4<Color>(Color(0, 255, 0)));
    testNode2->SetPaintOrder(true);
    testNode2->DrawOnNode(RSModifierType::CONTENT_STYLE, [](std::shared_ptr<Drawing::Canvas> canvas) {});
    GetRootNode()->AddChild(testNode2);
    RegisterNode(testNode2);
}
} // namespace OHOS::Rosen