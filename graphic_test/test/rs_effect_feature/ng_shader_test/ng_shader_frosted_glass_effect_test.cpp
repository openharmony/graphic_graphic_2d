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