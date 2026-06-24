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
 
#include "modifier_ng/custom/rs_content_style_modifier.h"

#include "pixel_map.h"
#include "rs_graphic_test.h"
#include "rs_graphic_test_img.h"
 
#include "ui/rs_canvas_drawing_node.h"
#include "ui/rs_canvas_node.h"
 
using namespace testing;
using namespace testing::ext;
 
namespace OHOS::Rosen {
class CanvasDrawingNodeTest : public RSGraphicTest {
public:
    // called before each tests
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }
 
    void setNode(std::shared_ptr<RSCanvasNode>& node, const Vector4f& bounds, Vector4<Color> borderColor)
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
 
class TestContentStyleModifier : public ModifierNG::RSContentStyleModifier {
public:
    ~TestContentStyleModifier() = default;
 
    void Draw(ModifierNG::RSDrawingContext& context) const override
    {
        auto canvas = context.canvas;
        if (canvas == nullptr) {
            return;
        }
        Drawing::Brush brush;
        brush.SetColor(Drawing::Color::COLOR_CYAN);
        canvas->AttachBrush(brush);
        canvas->DrawRect(Drawing::Rect(0, 0, 500, 500)); // Width and height is 500
        canvas->DetachBrush();
    }
};
 
class TestBitmapModifier : public ModifierNG::RSContentStyleModifier {
public:
    explicit TestBitmapModifier(Drawing::Bitmap& bitmap) : bitmap_(bitmap) {}
    ~TestBitmapModifier() = default;
 
    void Draw(ModifierNG::RSDrawingContext& context) const override
    {
        auto canvas = context.canvas;
        if (canvas == nullptr) {
            return;
        }
        Drawing::Brush brush;
        brush.SetColor(Drawing::Color::COLOR_CYAN);
        canvas->AttachBrush(brush);
        canvas->DrawBitmap(bitmap_, 500, 500); // Width and height is 500
        canvas->DetachBrush();
    }
 
private:
    Drawing::Bitmap& bitmap_;
};
 
GRAPHIC_TEST(CanvasDrawingNodeTest, CONTENT_DISPLAY_TEST, CanvasDrawingNode_DrawContent_Test)
{
    auto testNode1 = RSCanvasDrawingNode::Create(true);
    int width = 1000;
    int height = 1000;
    auto node = std::static_pointer_cast<RSCanvasNode>(testNode1);
    setNode(node, { 0, 0, width, height }, Vector4<Color>(Color(0, 0, 255)));
    testNode1->ResetSurface(width, height);
    auto modifier1 = std::make_shared<TestContentStyleModifier>();
    testNode1->AddModifier(modifier1);
    GetRootNode()->AddChild(testNode1);
    RegisterNode(testNode1);
 
    Media::InitializationOptions opts;
    opts.size.width = 500;
    opts.size.height = 500;
    opts.alphaType = Media::AlphaType::IMAGE_ALPHA_TYPE_OPAQUE;
    opts.pixelFormat = Media::PixelFormat::ARGB_8888;
    std::shared_ptr<Media::PixelMap> pixelmap = Media::PixelMap::Create(opts);
    Drawing::Rect pixelMapRect(0, 0, 500, 500);
    testNode1->GetPixelmap(pixelmap, nullptr, &pixelMapRect);
    Drawing::Bitmap bitmap;
    testNode1->GetBitmap(bitmap, nullptr, &pixelMapRect);
    auto modifier2 = std::make_shared<TestBitmapModifier>(bitmap);
    testNode1->AddModifier(modifier2);
 
    auto testNode2 = RSCanvasNode::Create(true);
    setNode(testNode2, { 500, 500, 500, 500 }, Vector4<Color>(Color(0, 255, 0)));
    testNode2->SetBgImageWidth(500);
    testNode2->SetBgImageHeight(500);
    auto image = std::make_shared<Rosen::RSImage>();
    image->SetPixelMap(pixelmap);
    image->SetImageFit((int)ImageFit::FILL);
    testNode2->SetBgImage(image);
    GetRootNode()->AddChild(testNode2);
    RegisterNode(testNode2);
}
} // namespace OHOS::Rosen