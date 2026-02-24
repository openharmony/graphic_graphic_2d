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

void InitFrostedGlassEffect(std::shared_ptr<RSNGFrostedGlassEffect>& frostedGlassEffect)
{
    if (!frostedGlassEffect) {
        return;
    }
    // WeightsEmboss
    frostedGlassEffect->Setter<FrostedGlassEffectWeightsEmbossTag>(Vector2f{1.0f, 0.5f});
    // WeightsEdl
    frostedGlassEffect->Setter<FrostedGlassEffectWeightsEdlTag>(Vector2f{1.0f, 1.0f});
    // BgRates
    frostedGlassEffect->Setter<FrostedGlassEffectBgRatesTag>(Vector2f{-1.8792225f, 2.7626955f});
    // BgKBS
    frostedGlassEffect->Setter<FrostedGlassEffectBgKBSTag>(Vector3f{0.0073494f, 0.0998859f, 1.2f});
    // BgPos
    frostedGlassEffect->Setter<FrostedGlassEffectBgPosTag>(Vector3f{0.3f, 0.5f, 0.5f});
    // BgNeg
    frostedGlassEffect->Setter<FrostedGlassEffectBgNegTag>(Vector3f{0.5f, 1.0f, 1.0f});
    // RefractParams
    frostedGlassEffect->Setter<FrostedGlassEffectRefractParamsTag>(Vector3f{1.0f, 0.3f, 0.3f});
    // SdParams
    frostedGlassEffect->Setter<FrostedGlassEffectSdParamsTag>(Vector3f{-50.0f, 6.0f, 6.62f});
    // SdRates
    frostedGlassEffect->Setter<FrostedGlassEffectSdRatesTag>(Vector2f{0.0f, 0.0f});
    // SdKBS
    frostedGlassEffect->Setter<FrostedGlassEffectSdKBSTag>(Vector3f{0.9f, 0.0f, 1.0f});
    // SdPos
    frostedGlassEffect->Setter<FrostedGlassEffectSdPosTag>(Vector3f{1.0f, 1.7f, 1.5f});
    // SdNeg
    frostedGlassEffect->Setter<FrostedGlassEffectSdNegTag>(Vector3f{3.0f, 2.0f, 1.0f});
    // EnvLightParams
    frostedGlassEffect->Setter<FrostedGlassEffectEnvLightParamsTag>(Vector2f{20.0f, 5.0f});
    // EnvLightRates
    frostedGlassEffect->Setter<FrostedGlassEffectEnvLightRatesTag>(Vector2f{0.0f, 0.0f});
    // EnvLightKBS
    frostedGlassEffect->Setter<FrostedGlassEffectEnvLightKBSTag>(Vector3f{0.8f, 0.27451f, 2.0f});
    // EnvLightPos
    frostedGlassEffect->Setter<FrostedGlassEffectEnvLightPosTag>(Vector3f{1.0f, 1.7f, 1.5f});
    // EnvLightNeg
    frostedGlassEffect->Setter<FrostedGlassEffectEnvLightNegTag>(Vector3f{3.0f, 2.0f, 1.0f});
    // EdLightParams
    frostedGlassEffect->Setter<FrostedGlassEffectEdLightParamsTag>(Vector2f{2.0f, 2.0f});
    // EdLightAngles
    frostedGlassEffect->Setter<FrostedGlassEffectEdLightAnglesTag>(Vector2f{40.0f, 20.0f});
    // EdLightDir
    frostedGlassEffect->Setter<FrostedGlassEffectEdLightDirTag>(Vector2f{2.5f, 2.5f});
    // EdLightRates
    frostedGlassEffect->Setter<FrostedGlassEffectEdLightRatesTag>(Vector2f{0.0f, 0.0f});
    // EdLightKBS
    frostedGlassEffect->Setter<FrostedGlassEffectEdLightKBSTag>(Vector3f{0.6027f, 0.627451f, 2.0f});
    // EdLightPos
    frostedGlassEffect->Setter<FrostedGlassEffectEdLightPosTag>(Vector3f{1.0f, 1.7f, 1.5f});
    // EdLightNeg
    frostedGlassEffect->Setter<FrostedGlassEffectEdLightNegTag>(Vector3f{3.2f, 2.0f, 1.0f});
    // MaterialColor
    frostedGlassEffect->Setter<FrostedGlassEffectMaterialColorTag>(Vector4f{0.0f, 0.0f, 0.0f, 0.0f});
}

namespace {
const std::string TEST_IMAGE_PATH = "/data/local/tmp/Images/backGroundImage.jpg";
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 2000;

// Material color variations
const std::vector<Vector4f> materialColors = {
    Vector4f{1.0f, 1.0f, 1.0f, 1.0f},  // White
    Vector4f{0.8f, 0.8f, 0.8f, 1.0f},  // Light gray
    Vector4f{0.5f, 0.5f, 0.5f, 1.0f},  // Medium gray
    Vector4f{0.2f, 0.2f, 0.2f, 1.0f}   // Dark gray
};

// Shape values (need to create proper shape objects)
const std::vector<std::shared_ptr<RSNGRenderShapeBase>> shapeValues = {
    nullptr,  // Placeholder - should be actual shape objects
    nullptr,  // Placeholder - should be actual shape objects
    nullptr   // Placeholder - should be actual shape objects
};
}

class NGShaderFrostedGlassEffectTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    }

private:
    void SetUpTestNode(const size_t i, const size_t columnCount, const size_t rowCount,
        std::shared_ptr<RSNGFrostedGlassEffect>& frostedGlass)
    {
        if (columnCount == 0 || rowCount == 0) {
            return;  // Invalid test configuration
        }
        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t x = (i % columnCount) * sizeX;
        const size_t y = (i / columnCount) * sizeY;

        auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {x, y, sizeX, sizeY});
        testNode->SetBackgroundNGShader(frostedGlass);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
};

GRAPHIC_TEST(NGShaderFrostedGlassEffectTest, EFFECT_TEST, Set_Frosted_Glass_Effect_Material_Color_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < materialColors.size(); i++) {
        auto frostedGlass = std::make_shared<RSNGFrostedGlassEffect>();
        InitFrostedGlassEffect(frostedGlass);
        frostedGlass->Setter<FrostedGlassEffectMaterialColorTag>(materialColors[i]);

        SetUpTestNode(i, columnCount, rowCount, frostedGlass);
    }
}

GRAPHIC_TEST(NGShaderFrostedGlassEffectTest, EFFECT_TEST, Set_Frosted_Glass_Effect_Shape_Test)
{
    const size_t columnCount = 3;
    const size_t rowCount = 1;

    for (size_t i = 0; i < shapeValues.size(); i++) {
        auto frostedGlass = std::make_shared<RSNGFrostedGlassEffect>();
        InitFrostedGlassEffect(frostedGlass);
        frostedGlass->Setter<FrostedGlassEffectMaterialColorTag>(Vector4f{0.8f, 0.8f, 0.8f, 1.0f});
        frostedGlass->Setter<FrostedGlassEffectShapeTag>(shapeValues[i]);

        SetUpTestNode(i, columnCount, rowCount, frostedGlass);
    }
}

/*
 * Test frosted glass effect with extreme material color values
 * Tests malicious inputs: negative values, extremely large values
 */
GRAPHIC_TEST(NGShaderFrostedGlassEffectTest, EFFECT_TEST, Set_Frosted_Glass_Effect_Extreme_Values_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;
    const std::vector<Vector4f> extremeColors = {
        Vector4f{-1.0f, -1.0f, -1.0f, -1.0f},   // All negative
        Vector4f{10.0f, 10.0f, 10.0f, 10.0f},    // Above max (1.0)
        Vector4f{9999.0f, 9999.0f, 9999.0f, 1.0f}, // Extremely large
        Vector4f{0.5f, 1e10f, 0.5f, 0.5f}        // Mixed extreme
    };
    for (size_t i = 0; i < extremeColors.size(); i++) {
        auto frostedGlass = std::make_shared<RSNGFrostedGlassEffect>();
        InitFrostedGlassEffect(frostedGlass);
        frostedGlass->Setter<FrostedGlassEffectMaterialColorTag>(extremeColors[i]);
        SetUpTestNode(i, columnCount, rowCount, frostedGlass);
    }
}

} // namespace OHOS::Rosen