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
#include "rs_graphic_test_img.h"
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
}

class NGShaderColorGradientEffectTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    }

private:
    void SetUpTestNode(const size_t i, const size_t columnCount, const size_t rowCount,
        std::shared_ptr<RSNGColorGradientEffect>& colorGradient)
    {
        if (columnCount == 0 || rowCount == 0) {
            return;  // Invalid test configuration
        }
        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t x = (i % columnCount) * sizeX;
        const size_t y = (i / columnCount) * sizeY;

        auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {x, y, sizeX, sizeY});
        testNode->SetBackgroundNGShader(colorGradient);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
};

GRAPHIC_TEST(NGShaderColorGradientEffectTest, EFFECT_TEST, Set_Color_Gradient_Effect_Brightness_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < brightnessValues.size(); i++) {
        auto colorGradient = std::make_shared<RSNGColorGradientEffect>();
        InitColorGradientEffect(colorGradient);
        colorGradient->Setter<ColorGradientEffectBrightnessTag>(brightnessValues[i]);
        colorGradient->Setter<ColorGradientEffectColorNumberTag>(4.0f);
        colorGradient->Setter<ColorGradientEffectColor0Tag>(Vector4f{1.0f, 0.0f, 0.0f, 1.0f});
        colorGradient->Setter<ColorGradientEffectColor1Tag>(Vector4f{0.0f, 1.0f, 0.0f, 1.0f});
        colorGradient->Setter<ColorGradientEffectColor2Tag>(Vector4f{0.0f, 0.0f, 1.0f, 1.0f});
        colorGradient->Setter<ColorGradientEffectColor3Tag>(Vector4f{1.0f, 1.0f, 0.0f, 1.0f});

        SetUpTestNode(i, columnCount, rowCount, colorGradient);
    }
}

GRAPHIC_TEST(NGShaderColorGradientEffectTest, EFFECT_TEST, Set_Color_Gradient_Effect_Blend_Brightness_Combination_Test)
{
    const size_t columnCount = 3;
    const size_t rowCount = 1;

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

        SetUpTestNode(i, columnCount, rowCount, colorGradient);
    }
}

GRAPHIC_TEST(NGShaderColorGradientEffectTest, EFFECT_TEST, Set_Color_Gradient_Effect_Extreme_Values_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;
    const std::vector<float> extremeValues = {-1.0f, -10.0f, 9999.0f, 1e10f};
    for (size_t i = 0; i < extremeValues.size(); i++) {
        auto colorGradient = std::make_shared<RSNGColorGradientEffect>();
        InitColorGradientEffect(colorGradient);
        colorGradient->Setter<ColorGradientEffectBrightnessTag>(extremeValues[i]);
        colorGradient->Setter<ColorGradientEffectColorNumberTag>(4.0f);
        SetUpTestNode(i, columnCount, rowCount, colorGradient);
    }
}

} // namespace OHOS::Rosen
