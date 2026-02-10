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

#include "modifier_ng/appearance/rs_color_picker_modifier.h"
#include "modifier_ng/custom/rs_content_style_modifier.h"
#include "property/rs_color_picker_def.h"
#include "rs_graphic_test.h"
#include "rs_graphic_test_img.h"
#include "transaction/rs_interfaces.h"
#include "ui/rs_canvas_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
constexpr int SCREEN_WIDTH = 1200;
constexpr int SCREEN_HEIGHT = 2000;
constexpr float CONTENT_NODE_WIDTH = 500.0f;
constexpr float CONTENT_NODE_HEIGHT = 500.0f;
const std::string TEST_IMAGE_PATH = "/data/local/tmp/Images/backGroundImage.jpg";
} // namespace

class ModifierNGNativeTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    }
};

class NativeContentModifier : public ModifierNG::RSContentStyleModifier {
public:
    ~NativeContentModifier() override = default;

    int16_t CallBaseGetterForCoverage() const
    {
        return this->ModifierNG::RSCustomModifier::Getter(
            ModifierNG::RSPropertyType::CUSTOM_INDEX, static_cast<int16_t>(0));
    }

    void Draw(ModifierNG::RSDrawingContext& context) const override
    {
        if (!context.canvas) {
            return;
        }
        Drawing::Brush brush;
        brush.SetColor(Drawing::Color::COLOR_CYAN);
        context.canvas->AttachBrush(brush);
        context.canvas->DrawRect(Drawing::Rect(0, 0, context.width, context.height));
        context.canvas->DetachBrush();
    }
};

/*
 * @tc.name: ColorPickerModifier_NativeCoverage_Test
 * @tc.desc: Cover RSColorPickerModifier getter/setter interfaces in native graphic test style.
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ModifierNGNativeTest, CONTENT_DISPLAY_TEST, ColorPickerModifier_NativeCoverage_Test)
{
    auto backgroundNode = SetUpNodeBgImage(TEST_IMAGE_PATH, { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT });
    ASSERT_NE(backgroundNode, nullptr);

    auto colorPickerModifier = std::make_shared<ModifierNG::RSColorPickerModifier>();
    ASSERT_NE(colorPickerModifier, nullptr);
    EXPECT_EQ(colorPickerModifier->GetType(), ModifierNG::RSModifierType::COLOR_PICKER);

    colorPickerModifier->SetColorPickerPlaceholder(ColorPlaceholder::SURFACE_CONTRAST);
    EXPECT_EQ(colorPickerModifier->GetColorPickerPlaceholder(), ColorPlaceholder::SURFACE_CONTRAST);

    colorPickerModifier->SetColorPickerStrategy(ColorPickStrategyType::CONTRAST);
    EXPECT_EQ(colorPickerModifier->GetColorPickerStrategy(), ColorPickStrategyType::CONTRAST);

    constexpr uint64_t interval = 360;
    colorPickerModifier->SetColorPickerInterval(interval);
    EXPECT_EQ(colorPickerModifier->GetColorPickerInterval(), interval);

    constexpr uint32_t notifyThreshold = (210u << 16) | 100u;
    colorPickerModifier->SetColorPickerNotifyThreshold(notifyThreshold);
    EXPECT_EQ(colorPickerModifier->GetColorPickerNotifyThreshold(), notifyThreshold);

    backgroundNode->AddModifier(colorPickerModifier);
    GetRootNode()->AddChild(backgroundNode);
    RegisterNode(backgroundNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
}

/*
 * @tc.name: CustomModifier_NativeCoverage_Test
 * @tc.desc: Cover RSCustomModifier interfaces with native modifier drawing case.
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ModifierNGNativeTest, CONTENT_DISPLAY_TEST, CustomModifier_NativeCoverage_Test)
{
    auto backgroundNode = SetUpNodeBgImage(TEST_IMAGE_PATH, { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT });
    ASSERT_NE(backgroundNode, nullptr);
    RegisterNode(backgroundNode);

    auto contentNode = RSCanvasNode::Create(true);
    ASSERT_NE(contentNode, nullptr);
    contentNode->SetBounds({ 0, 0, CONTENT_NODE_WIDTH, CONTENT_NODE_HEIGHT });
    contentNode->SetFrame({ 0, 0, CONTENT_NODE_WIDTH, CONTENT_NODE_HEIGHT });

    auto contentModifier = std::make_shared<NativeContentModifier>();
    ASSERT_NE(contentModifier, nullptr);
    EXPECT_TRUE(contentModifier->IsCustom());

    contentModifier->SetNoNeedUICaptured(true);
    contentModifier->SetIndex(7);
    EXPECT_EQ(contentModifier->GetIndex(), 7);
    EXPECT_EQ(contentModifier->CallBaseGetterForCoverage(), 7);

    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(300);
    contentModifier->SetContentTransitionParam(
        ContentTransitionType::OPACITY, timingProtocol, RSAnimationTimingCurve::EASE_IN_OUT);

    auto drawContext = ModifierNG::RSCustomModifierHelper::CreateDrawingContext(contentNode);
    std::shared_ptr<ModifierNG::RSCustomModifier> baseModifier = contentModifier;
    baseModifier->Draw(drawContext);
    auto drawCmdList = ModifierNG::RSCustomModifierHelper::FinishDrawing(drawContext);
    ASSERT_NE(drawCmdList, nullptr);

    contentNode->AddModifier(contentModifier);
    backgroundNode->AddChild(contentNode);

    GetRootNode()->AddChild(backgroundNode);
    RegisterNode(contentNode);

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
}
} // namespace OHOS::Rosen
