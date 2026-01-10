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

#include "modifier_ng/custom/rs_content_style_modifier.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_root_node.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_ui_director.h"

using namespace testing;
using namespace testing::ext;

constexpr uint32_t SLEEP_TIME_IN_US = 10000;      // 10ms
constexpr uint32_t SLEEP_TIME_FOR_PROXY = 100000; // 100ms

namespace OHOS::Rosen {
class MultiInstanceTest : public RSGraphicTest {
public:
    // called before each tests
    void BeforeEach() override
    {
        auto size = GetScreenSize();
        SetSurfaceBounds({ 0, 0, size.x_ / 2.0f, size.y_ / 2.0f });
        SetSurfaceColor(RSColor(0xffff0000));
    }

    // called after each tests
    void AfterEach() override {}
};

class CustomModifier : public ModifierNG::RSContentStyleModifier {
public:
    CustomModifier() = default;
    ~CustomModifier() = default;

    void Draw(ModifierNG::RSDrawingContext& context) const override
    {
        Drawing::Rect rect(0, 0, width_, height_);
        Drawing::Brush brush;
        brush.SetColor(backgroundColor_);
        context.canvas->AttachBrush(brush);
        context.canvas->DrawRect(rect);
        context.canvas->DetachBrush();

        Drawing::Pen pen;
        pen.SetAntiAlias(true);
        pen.SetColor(Drawing::Color::COLOR_BLACK);
        Drawing::scalar penWidth = 5;
        pen.SetWidth(penWidth);
        context.canvas->AttachPen(pen);
        Drawing::Point startPt(10, 10);
        Drawing::Point endPt(width_, height_);
        context.canvas->DrawLine(startPt, endPt);
        context.canvas->DetachPen();
    }

    void SetWidth(float width)
    {
        width_ = width;
    }

    void SetHeight(float height)
    {
        height_ = height;
    }

    void SetBackgroundColor(Drawing::Color color)
    {
        backgroundColor_ = color;
    }

private:
    float width_;
    float height_;
    Drawing::Color backgroundColor_;
};

/*
 * @tc.name: GRAPHIC_TEST_MOVE_MODIFIER_TEST_001
 * @tc.desc: Test on the Move Modifier function when the instance of a Multi-Instance node id changed
 * @tc.type: FUNC
 */
GRAPHIC_TEST(MultiInstanceTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_MOVE_MODIFIER_TEST_001)
{
    auto uiDirector = RSUIDirector::Create();
    uiDirector->Init(true, true);
    auto uiContext = uiDirector->GetRSUIContext();

    auto rootNode = RSRootNode::Create(false, false, uiContext);
    rootNode->SetBounds({ 0, 0, 1000, 1000 });
    rootNode->SetFrame({ 0, 0, 1000, 1000 });
    rootNode->SetBackgroundColor(SK_ColorGREEN);
    GetRootNode()->AddChild(rootNode);

    auto canvasNode = RSCanvasNode::Create(false, false, uiContext);
    canvasNode->SetBounds({ 0, 0, 500, 500 });
    canvasNode->SetFrame({ 0, 0, 500, 500 });
    canvasNode->SetBackgroundColor(SK_ColorWHITE);
    rootNode->AddChild(canvasNode, -1);
    uiDirector->SendMessages();

    auto customModifier = std::make_shared<CustomModifier>();
    customModifier->SetWidth(360);
    customModifier->SetHeight(480);
    customModifier->SetBackgroundColor(Drawing::Color::COLOR_BLUE);
    canvasNode->AddModifier(customModifier);

    auto newUIDirector = RSUIDirector::Create();
    newUIDirector->Init(true, true);
    auto newUIContext = newUIDirector->GetRSUIContext();
    rootNode->SetRSUIContext(newUIContext);

    newUIDirector->FlushModifier();
    usleep(SLEEP_TIME_IN_US);
    newUIDirector->SendMessages();
    usleep(SLEEP_TIME_FOR_PROXY);

    // created node should be registered to preserve ref_count
    RegisterNode(rootNode);
    RegisterNode(canvasNode);
}
} // namespace OHOS::Rosen