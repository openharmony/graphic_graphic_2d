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

void InitGradientFlowColors(std::shared_ptr<RSNGGradientFlowColors>& gradientFlowColors)
{
    if (!gradientFlowColors) {
        return;
    }
    // Colors
    gradientFlowColors->Setter<GradientFlowColorsColor0Tag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
    gradientFlowColors->Setter<GradientFlowColorsColor1Tag>(Vector4f{0.8f, 0.8f, 1.0f, 1.0f});
    gradientFlowColors->Setter<GradientFlowColorsColor2Tag>(Vector4f{0.6f, 0.6f, 1.0f, 1.0f});
    gradientFlowColors->Setter<GradientFlowColorsColor3Tag>(Vector4f{0.4f, 0.4f, 1.0f, 1.0f});
    // GradientBegin
    gradientFlowColors->Setter<GradientFlowColorsGradientBeginTag>(0.0f);
    // GradientEnd
    gradientFlowColors->Setter<GradientFlowColorsGradientEndTag>(1.0f);
    // EffectAlpha
    gradientFlowColors->Setter<GradientFlowColorsEffectAlphaTag>(1.0f);
    // Progress
    gradientFlowColors->Setter<GradientFlowColorsProgressTag>(0.5f);
}

namespace {
const std::string TEST_IMAGE_PATH = "/data/local/tmp/Images/backGroundImage.jpg";
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 2000;

// Progress values
const std::vector<float> progressValues = {0.0f, 0.3f, 0.5f, 1.0f};

// Effect alpha values
const std::vector<float> alphaValues = {0.0f, 0.5f, 1.0f};

// Gradient begin/end
const std::vector<Vector2f> gradientPositions = {
    Vector2f{0.0f, 0.0f},
    Vector2f{1.0f, 1.0f}
};
}

class NGShaderGradientFlowColorsTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    }

private:
    void SetUpTestNode(const size_t i, const size_t columnCount, const size_t rowCount,
        std::shared_ptr<RSNGGradientFlowColors>& gradientFlow)
    {
        if (columnCount == 0 || rowCount == 0) {
            return;  // Invalid test configuration
        }
        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t x = (i % columnCount) * sizeX;
        const size_t y = (i / columnCount) * sizeY;

        auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {x, y, sizeX, sizeY});
        testNode->SetBackgroundNGShader(gradientFlow);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
};

GRAPHIC_TEST(NGShaderGradientFlowColorsTest, EFFECT_TEST, Set_Gradient_Flow_Colors_Progress_Boundary_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < progressValues.size(); i++) {
        auto gradientFlow = std::make_shared<RSNGGradientFlowColors>();
        InitGradientFlowColors(gradientFlow);
        gradientFlow->Setter<GradientFlowColorsColor0Tag>(Vector4f{1.0f, 0.0f, 0.0f, 1.0f});
        gradientFlow->Setter<GradientFlowColorsColor1Tag>(Vector4f{0.0f, 1.0f, 0.0f, 1.0f});
        gradientFlow->Setter<GradientFlowColorsColor2Tag>(Vector4f{0.0f, 0.0f, 1.0f, 1.0f});
        gradientFlow->Setter<GradientFlowColorsColor3Tag>(Vector4f{1.0f, 1.0f, 0.0f, 1.0f});
        gradientFlow->Setter<GradientFlowColorsProgressTag>(progressValues[i]);

        SetUpTestNode(i, columnCount, rowCount, gradientFlow);
    }
}

GRAPHIC_TEST(NGShaderGradientFlowColorsTest, EFFECT_TEST, Set_Gradient_Flow_Colors_Alpha_Progress_Combination_Test)
{
    const size_t columnCount = 3;
    const size_t rowCount = 1;

    for (size_t i = 0; i < alphaValues.size(); i++) {
        auto gradientFlow = std::make_shared<RSNGGradientFlowColors>();
        InitGradientFlowColors(gradientFlow);
        gradientFlow->Setter<GradientFlowColorsColor0Tag>(Vector4f{1.0f, 0.0f, 0.0f, 1.0f});
        gradientFlow->Setter<GradientFlowColorsColor1Tag>(Vector4f{0.0f, 1.0f, 0.0f, 1.0f});
        gradientFlow->Setter<GradientFlowColorsColor2Tag>(Vector4f{0.0f, 0.0f, 1.0f, 1.0f});
        gradientFlow->Setter<GradientFlowColorsColor3Tag>(Vector4f{1.0f, 1.0f, 0.0f, 1.0f});
        gradientFlow->Setter<GradientFlowColorsProgressTag>(progressValues[i]);
        gradientFlow->Setter<GradientFlowColorsEffectAlphaTag>(alphaValues[i]);

        SetUpTestNode(i, columnCount, rowCount, gradientFlow);
    }
}

GRAPHIC_TEST(NGShaderGradientFlowColorsTest, EFFECT_TEST, Set_Gradient_Flow_Colors_Extreme_Values_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;
    const std::vector<float> extremeValues = {-1.0f, -10.0f, 9999.0f, 1e10f};
    for (size_t i = 0; i < extremeValues.size(); i++) {
        auto gradientFlow = std::make_shared<RSNGGradientFlowColors>();
        InitGradientFlowColors(gradientFlow);
        gradientFlow->Setter<GradientFlowColorsColor0Tag>(Vector4f{1.0f, 0.0f, 0.0f, 1.0f});
        gradientFlow->Setter<GradientFlowColorsColor1Tag>(Vector4f{0.0f, 1.0f, 0.0f, 1.0f});
        gradientFlow->Setter<GradientFlowColorsColor2Tag>(Vector4f{0.0f, 0.0f, 1.0f, 1.0f});
        gradientFlow->Setter<GradientFlowColorsColor3Tag>(Vector4f{1.0f, 1.0f, 0.0f, 1.0f});
        gradientFlow->Setter<GradientFlowColorsProgressTag>(extremeValues[i]);
        SetUpTestNode(i, columnCount, rowCount, gradientFlow);
    }
}

} // namespace OHOS::Rosen
