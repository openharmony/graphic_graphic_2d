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
#include "ng_shader_test_utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
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
