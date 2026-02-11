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

void InitDotMatrixShader(std::shared_ptr<RSNGDotMatrixShader>& dotMatrixShader)
{
    if (!dotMatrixShader) {
        return;
    }
    // PathDirection
    dotMatrixShader->Setter<DotMatrixShaderPathDirectionTag>(0);
    // EffectColors (vector of Vector4f)
    std::vector<Vector4f> effectColors = {
        Vector4f{1.0f, 1.0f, 1.0f, 1.0f},
        Vector4f{0.8f, 0.8f, 1.0f, 1.0f}
    };
    dotMatrixShader->Setter<DotMatrixShaderEffectColorsTag>(effectColors);
    // ColorFractions
    dotMatrixShader->Setter<DotMatrixShaderColorFractionsTag>(Vector2f{0.0f, 1.0f});
    // StartPoints (vector of Vector2f)
    std::vector<Vector2f> startPoints = {
        Vector2f{0.2f, 0.2f},
        Vector2f{0.8f, 0.8f}
    };
    dotMatrixShader->Setter<DotMatrixShaderStartPointsTag>(startPoints);
    // PathWidth
    dotMatrixShader->Setter<DotMatrixShaderPathWidthTag>(10.0f);
    // InverseEffect
    dotMatrixShader->Setter<DotMatrixShaderInverseEffectTag>(false);
    // DotColor
    dotMatrixShader->Setter<DotMatrixShaderDotColorTag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
    // DotSpacing
    dotMatrixShader->Setter<DotMatrixShaderDotSpacingTag>(5.0f);
    // DotRadius
    dotMatrixShader->Setter<DotMatrixShaderDotRadiusTag>(2.0f);
    // BgColor
    dotMatrixShader->Setter<DotMatrixShaderBgColorTag>(Vector4f{0.0f, 0.0f, 0.0f, 1.0f});
    // EffectType
    dotMatrixShader->Setter<DotMatrixShaderEffectTypeTag>(0);
    // Progress
    dotMatrixShader->Setter<DotMatrixShaderProgressTag>(0.5f);
}

namespace {
const std::string TEST_IMAGE_PATH = "/data/local/tmp/Images/backGroundImage.jpg";
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 2000;

// Dot spacing values
const std::vector<float> dotSpacingValues = {1.0f, 5.0f, 10.0f, 20.0f};

// Dot radius values
const std::vector<float> dotRadiusValues = {0.5f, 2.0f, 5.0f};

// Progress values
const std::vector<float> progressValues = {0.0f, 0.3f, 0.5f, 1.0f};
}

class NGShaderDotMatrixTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    }

private:
    void SetUpTestNode(const size_t i, const size_t columnCount, const size_t rowCount,
        std::shared_ptr<RSNGDotMatrixShader>& dotMatrixShader)
    {
        if (columnCount == 0 || rowCount == 0) {
            return;  // Invalid test configuration
        }
        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t x = (i % columnCount) * sizeX;
        const size_t y = (i / columnCount) * sizeY;

        auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {x, y, sizeX, sizeY});
        testNode->SetBackgroundNGShader(dotMatrixShader);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
};

/*
 * Test dot matrix with different dot spacing values
 */
GRAPHIC_TEST(NGShaderDotMatrixTest, EFFECT_TEST, Set_Dot_Matrix_Dot_Spacing_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < dotSpacingValues.size(); i++) {
        auto dotMatrixShader = std::make_shared<RSNGDotMatrixShader>();
        InitDotMatrixShader(dotMatrixShader);
        dotMatrixShader->Setter<DotMatrixShaderDotSpacingTag>(dotSpacingValues[i]);
        dotMatrixShader->Setter<DotMatrixShaderDotRadiusTag>(2.0f);
        dotMatrixShader->Setter<DotMatrixShaderProgressTag>(0.5f);

        SetUpTestNode(i, columnCount, rowCount, dotMatrixShader);
    }
}

/*
 * Test dot matrix with dot radius values
 */
GRAPHIC_TEST(NGShaderDotMatrixTest, EFFECT_TEST, Set_Dot_Matrix_Dot_Radius_Test)
{
    const size_t columnCount = 3;
    const size_t rowCount = 1;

    for (size_t i = 0; i < dotRadiusValues.size(); i++) {
        auto dotMatrixShader = std::make_shared<RSNGDotMatrixShader>();
        InitDotMatrixShader(dotMatrixShader);
        dotMatrixShader->Setter<DotMatrixShaderDotSpacingTag>(5.0f);
        dotMatrixShader->Setter<DotMatrixShaderDotRadiusTag>(dotRadiusValues[i]);
        dotMatrixShader->Setter<DotMatrixShaderProgressTag>(0.5f);

        SetUpTestNode(i, columnCount, rowCount, dotMatrixShader);
    }
}

/*
 * Test dot matrix with progress boundary values
 */
GRAPHIC_TEST(NGShaderDotMatrixTest, EFFECT_TEST, Set_Dot_Matrix_Progress_Boundary_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < progressValues.size(); i++) {
        auto dotMatrixShader = std::make_shared<RSNGDotMatrixShader>();
        InitDotMatrixShader(dotMatrixShader);
        dotMatrixShader->Setter<DotMatrixShaderDotSpacingTag>(5.0f);
        dotMatrixShader->Setter<DotMatrixShaderDotRadiusTag>(2.0f);
        dotMatrixShader->Setter<DotMatrixShaderProgressTag>(progressValues[i]);

        SetUpTestNode(i, columnCount, rowCount, dotMatrixShader);
    }
}

/*
 * Test dot matrix with extreme and invalid values
 * Tests malicious inputs: negative values, extremely large values
 */
GRAPHIC_TEST(NGShaderDotMatrixTest, EFFECT_TEST, Set_Dot_Matrix_Extreme_Values_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;
    const std::vector<float> extremeValues = {-1.0f, -10.0f, 9999.0f, 1e10f};
    for (size_t i = 0; i < extremeValues.size(); i++) {
        auto dotMatrixShader = std::make_shared<RSNGDotMatrixShader>();
        InitDotMatrixShader(dotMatrixShader);
        dotMatrixShader->Setter<DotMatrixShaderDotSpacingTag>(extremeValues[i]);
        dotMatrixShader->Setter<DotMatrixShaderDotRadiusTag>(2.0f);
        dotMatrixShader->Setter<DotMatrixShaderProgressTag>(0.5f);
        SetUpTestNode(i, columnCount, rowCount, dotMatrixShader);
    }
}

} // namespace OHOS::Rosen
