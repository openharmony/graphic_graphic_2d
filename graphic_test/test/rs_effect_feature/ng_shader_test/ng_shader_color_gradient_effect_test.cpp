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
#include "ui/rs_effect_node.h"
#include "ui_effect/property/include/rs_ui_shader_base.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

void InitColorGradientEffect(std::shared_ptr<RSNGColorGradientEffect>& colorGradientEffect)
{
    if (!colorGradientEffect) {
        return;
    }
    // Colors (12 colors)
    colorGradientEffect->Setter<ColorGradientEffectColor0Tag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
    colorGradientEffect->Setter<ColorGradientEffectColor1Tag>(Vector4f{0.9f, 0.9f, 1.0f, 1.0f});
    colorGradientEffect->Setter<ColorGradientEffectColor2Tag>(Vector4f{0.8f, 0.8f, 1.0f, 1.0f});
    colorGradientEffect->Setter<ColorGradientEffectColor3Tag>(Vector4f{0.7f, 0.7f, 1.0f, 1.0f});
    colorGradientEffect->Setter<ColorGradientEffectColor4Tag>(Vector4f{0.6f, 0.6f, 1.0f, 1.0f});
    colorGradientEffect->Setter<ColorGradientEffectColor5Tag>(Vector4f{0.5f, 0.5f, 1.0f, 1.0f});
    colorGradientEffect->Setter<ColorGradientEffectColor6Tag>(Vector4f{0.4f, 0.4f, 1.0f, 1.0f});
    colorGradientEffect->Setter<ColorGradientEffectColor7Tag>(Vector4f{0.3f, 0.3f, 1.0f, 1.0f});
    colorGradientEffect->Setter<ColorGradientEffectColor8Tag>(Vector4f{0.2f, 0.2f, 1.0f, 1.0f});
    colorGradientEffect->Setter<ColorGradientEffectColor9Tag>(Vector4f{0.1f, 0.1f, 1.0f, 1.0f});
    colorGradientEffect->Setter<ColorGradientEffectColor10Tag>(Vector4f{0.0f, 0.0f, 1.0f, 1.0f});
    colorGradientEffect->Setter<ColorGradientEffectColor11Tag>(Vector4f{1.0f, 0.0f, 1.0f, 1.0f});
    // Positions (12 positions, distributed)
    colorGradientEffect->Setter<ColorGradientEffectPosition0Tag>(Vector2f{0.0f, 0.0f});
    colorGradientEffect->Setter<ColorGradientEffectPosition1Tag>(Vector2f{0.1f, 0.1f});
    colorGradientEffect->Setter<ColorGradientEffectPosition2Tag>(Vector2f{0.2f, 0.2f});
    colorGradientEffect->Setter<ColorGradientEffectPosition3Tag>(Vector2f{0.3f, 0.3f});
    colorGradientEffect->Setter<ColorGradientEffectPosition4Tag>(Vector2f{0.4f, 0.4f});
    colorGradientEffect->Setter<ColorGradientEffectPosition5Tag>(Vector2f{0.5f, 0.5f});
    colorGradientEffect->Setter<ColorGradientEffectPosition6Tag>(Vector2f{0.6f, 0.6f});
    colorGradientEffect->Setter<ColorGradientEffectPosition7Tag>(Vector2f{0.7f, 0.7f});
    colorGradientEffect->Setter<ColorGradientEffectPosition8Tag>(Vector2f{0.8f, 0.8f});
    colorGradientEffect->Setter<ColorGradientEffectPosition9Tag>(Vector2f{0.9f, 0.9f});
    colorGradientEffect->Setter<ColorGradientEffectPosition10Tag>(Vector2f{1.0f, 1.0f});
    colorGradientEffect->Setter<ColorGradientEffectPosition11Tag>(Vector2f{0.5f, 1.0f});
    // Strengths (12 strengths)
    colorGradientEffect->Setter<ColorGradientEffectStrength0Tag>(1.0f);
    colorGradientEffect->Setter<ColorGradientEffectStrength1Tag>(1.0f);
    colorGradientEffect->Setter<ColorGradientEffectStrength2Tag>(1.0f);
    colorGradientEffect->Setter<ColorGradientEffectStrength3Tag>(1.0f);
    colorGradientEffect->Setter<ColorGradientEffectStrength4Tag>(1.0f);
    colorGradientEffect->Setter<ColorGradientEffectStrength5Tag>(1.0f);
    colorGradientEffect->Setter<ColorGradientEffectStrength6Tag>(1.0f);
    colorGradientEffect->Setter<ColorGradientEffectStrength7Tag>(1.0f);
    colorGradientEffect->Setter<ColorGradientEffectStrength8Tag>(1.0f);
    colorGradientEffect->Setter<ColorGradientEffectStrength9Tag>(1.0f);
    colorGradientEffect->Setter<ColorGradientEffectStrength10Tag>(1.0f);
    colorGradientEffect->Setter<ColorGradientEffectStrength11Tag>(1.0f);
    // ColorNumber
    colorGradientEffect->Setter<ColorGradientEffectColorNumberTag>(4.0f);
    // Blend
    colorGradientEffect->Setter<ColorGradientEffectBlendTag>(1.0f);
    // BlendK
    colorGradientEffect->Setter<ColorGradientEffectBlendKTag>(1.0f);
    // Brightness
    colorGradientEffect->Setter<ColorGradientEffectBrightnessTag>(1.0f);
}

namespace {
const std::string TEST_IMAGE_PATH = "/data/local/tmp/Images/backGroundImage.jpg";
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 2000;

// Brightness values
const std::vector<float> brightnessValues = {0.0f, 0.5f, 1.0f, 2.0f};

// Blend values
const std::vector<float> blendValues = {0.0f, 0.5f, 1.0f};

// Color number values
const std::vector<float> colorNumberValues = {2.0f, 4.0f, 6.0f, 8.0f};

// Extreme values
const std::vector<float> extremeValues = {-1.0f, -10.0f, 9999.0f, 1e10f};

std::shared_ptr<RSCanvasNode> CreateEffectChildNode(const size_t i, const size_t columnCount, const size_t rowCount,
    std::shared_ptr<RSEffectNode>& effectNode, std::shared_ptr<RSNGColorGradientEffect>& colorGradient)
{
    auto sizeX = (columnCount != 0) ? (SCREEN_WIDTH / columnCount) : SCREEN_WIDTH;
    auto sizeY = (rowCount != 0) ? (SCREEN_HEIGHT * columnCount / rowCount) : SCREEN_HEIGHT;

    int x = (columnCount != 0) ? (i % columnCount) * sizeX : 0;
    int y = (columnCount != 0) ? (i / columnCount) * sizeY : 0;

    auto effectChildNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
    if (!effectChildNode || !effectNode) {
        return nullptr;
    }
    effectChildNode->SetBounds(x, y, sizeX, sizeY);
    effectChildNode->SetFrame(x, y, sizeX, sizeY);
    effectChildNode->SetCoverageNGShader(colorGradient);
    effectNode->AddChild(effectChildNode);
    return effectChildNode;
}
}

class NGShaderColorGradientEffectTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    }

    void SetEffectChildNode(const size_t i, const size_t columnCount, const size_t rowCount,
        std::shared_ptr<RSEffectNode>& effectNode, std::shared_ptr<RSNGColorGradientEffect>& colorGradient)
    {
        auto effectChildNode = CreateEffectChildNode(i, columnCount, rowCount, effectNode, colorGradient);
        RegisterNode(effectChildNode);
    }
 
    std::shared_ptr<RSEffectNode> SetUpEffectNode()
    {
        auto backgroundTestNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
        auto effectNode = RSEffectNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        if (!backgroundTestNode || !effectNode) {
            return nullptr;
        }
        effectNode->SetBounds({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
        effectNode->SetFrame({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
        std::shared_ptr<Rosen::RSFilter> backFilter = Rosen::RSFilter::CreateMaterialFilter(10.f, 1, 1, 0,
            BLUR_COLOR_MODE::DEFAULT, true);
        effectNode->SetBackgroundFilter(backFilter);
        effectNode->SetClipToBounds(false);
        GetRootNode()->AddChild(backgroundTestNode);
        backgroundTestNode->AddChild(effectNode);
        RegisterNode(effectNode);
        RegisterNode(backgroundTestNode);
        return effectNode;
    }
};

GRAPHIC_TEST(NGShaderColorGradientEffectTest, EFFECT_TEST, Set_Color_Gradient_Effect_Brightness_Test)
{
    const size_t columnCount = 1;
    const size_t rowCount = static_cast<size_t>(brightnessValues.size());
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < brightnessValues.size(); i++) {
        auto colorGradient = std::make_shared<RSNGColorGradientEffect>();
        InitColorGradientEffect(colorGradient);
        colorGradient->Setter<ColorGradientEffectBrightnessTag>(brightnessValues[i]);
        colorGradient->Setter<ColorGradientEffectColorNumberTag>(4.0f);
        colorGradient->Setter<ColorGradientEffectColor0Tag>(Vector4f{1.0f, 0.0f, 0.0f, 1.0f});
        colorGradient->Setter<ColorGradientEffectColor1Tag>(Vector4f{0.0f, 1.0f, 0.0f, 1.0f});
        colorGradient->Setter<ColorGradientEffectColor2Tag>(Vector4f{0.0f, 0.0f, 1.0f, 1.0f});
        colorGradient->Setter<ColorGradientEffectColor3Tag>(Vector4f{1.0f, 1.0f, 0.0f, 1.0f});

        SetEffectChildNode(static_cast<size_t>(i), columnCount, rowCount, effectNode, colorGradient);
    }
}

GRAPHIC_TEST(NGShaderColorGradientEffectTest, EFFECT_TEST, Set_Color_Gradient_Effect_Blend_Brightness_Combination_Test)
{
    const size_t columnCount = 1;
    const size_t rowCount = static_cast<size_t>(blendValues.size());
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < blendValues.size(); i++) {
        auto colorGradient = std::make_shared<RSNGColorGradientEffect>();
        InitColorGradientEffect(colorGradient);
        colorGradient->Setter<ColorGradientEffectBrightnessTag>(brightnessValues[i]);
        colorGradient->Setter<ColorGradientEffectBlendTag>(blendValues[i]);
        colorGradient->Setter<ColorGradientEffectColorNumberTag>(4.0f);
        colorGradient->Setter<ColorGradientEffectColor0Tag>(Vector4f{1.0f, 0.0f, 0.0f, 1.0f});
        colorGradient->Setter<ColorGradientEffectColor1Tag>(Vector4f{0.0f, 1.0f, 0.0f, 1.0f});
        colorGradient->Setter<ColorGradientEffectColor2Tag>(Vector4f{0.0f, 0.0f, 1.0f, 1.0f});
        colorGradient->Setter<ColorGradientEffectColor3Tag>(Vector4f{1.0f, 1.0f, 0.0f, 1.0f});

        SetEffectChildNode(static_cast<size_t>(i), columnCount, rowCount, effectNode, colorGradient);
    }
}

GRAPHIC_TEST(NGShaderColorGradientEffectTest, EFFECT_TEST, Set_Color_Gradient_Effect_Extreme_Values_Test)
{
    const size_t columnCount = 1;
    const size_t rowCount = static_cast<size_t>(extremeValues.size());
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < extremeValues.size(); i++) {
        auto colorGradient = std::make_shared<RSNGColorGradientEffect>();
        InitColorGradientEffect(colorGradient);
        colorGradient->Setter<ColorGradientEffectBrightnessTag>(extremeValues[i]);
        colorGradient->Setter<ColorGradientEffectColorNumberTag>(4.0f);
        
        SetEffectChildNode(static_cast<size_t>(i), columnCount, rowCount, effectNode, colorGradient);
    }
}

} // namespace OHOS::Rosen
