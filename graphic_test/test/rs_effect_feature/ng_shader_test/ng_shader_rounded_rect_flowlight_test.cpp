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

void InitRoundedRectFlowlight(std::shared_ptr<RSNGRoundedRectFlowlight>& roundedRectFlowlight)
{
    if (!roundedRectFlowlight) {
        return;
    }
    // StartEndPosition
    roundedRectFlowlight->Setter<RoundedRectFlowlightStartEndPositionTag>(Vector2f{0.0f, 1.0f});
    // WaveLength
    roundedRectFlowlight->Setter<RoundedRectFlowlightWaveLengthTag>(0.2f);
    // WaveTop
    roundedRectFlowlight->Setter<RoundedRectFlowlightWaveTopTag>(0.2f);
    // CornerRadius
    roundedRectFlowlight->Setter<RoundedRectFlowlightCornerRadiusTag>(0.1f);
    // Brightness
    roundedRectFlowlight->Setter<RoundedRectFlowlightBrightnessTag>(1.0f);
    // Scale
    roundedRectFlowlight->Setter<RoundedRectFlowlightScaleTag>(1.0f);
    // Sharping
    roundedRectFlowlight->Setter<RoundedRectFlowlightSharpingTag>(1.0f);
    // Feathering
    roundedRectFlowlight->Setter<RoundedRectFlowlightFeatheringTag>(1.0f);
    // FeatheringBezierControlPoints
    Vector4f featheringPoints{0.0f, 0.0f, 1.0f, 1.0f};
    roundedRectFlowlight->Setter<RoundedRectFlowlightFeatheringBezierControlPointsTag>(featheringPoints);
    // GradientBezierControlPoints
    Vector4f gradientPoints{0.0f, 0.0f, 1.0f, 1.0f};
    roundedRectFlowlight->Setter<RoundedRectFlowlightGradientBezierControlPointsTag>(gradientPoints);
    // Color
    roundedRectFlowlight->Setter<RoundedRectFlowlightColorTag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
    // Progress
    roundedRectFlowlight->Setter<RoundedRectFlowlightProgressTag>(0.5f);
}

namespace {
const std::string TEST_IMAGE_PATH = "/data/local/tmp/Images/backGroundImage.jpg";
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 2000;

// Progress values
const std::vector<float> progressValues = {0.0f, 0.3f, 0.5f, 1.0f};

// Corner radius values
const std::vector<float> cornerRadii = {0.0f, 10.0f, 25.0f, 50.0f};

// Brightness values
const std::vector<float> brightnessValues = {0.0f, 0.5f, 1.0f, 2.0f};
}

class NGShaderRoundedRectFlowlightTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    }

private:
    void SetUpTestNode(const size_t i, const size_t columnCount, const size_t rowCount,
        std::shared_ptr<RSNGRoundedRectFlowlight>& roundedRectFlow)
    {
        if (columnCount == 0 || rowCount == 0) {
            return;  // Invalid test configuration
        }
        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t x = (i % columnCount) * sizeX;
        const size_t y = (i / columnCount) * sizeY;

        auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {x, y, sizeX, sizeY});
        testNode->SetBackgroundNGShader(roundedRectFlow);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
};

GRAPHIC_TEST(NGShaderRoundedRectFlowlightTest, EFFECT_TEST, Set_Rounded_Rect_Flowlight_Progress_Boundary_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < progressValues.size(); i++) {
        auto roundedRectFlow = std::make_shared<RSNGRoundedRectFlowlight>();
        InitRoundedRectFlowlight(roundedRectFlow);
        roundedRectFlow->Setter<RoundedRectFlowlightProgressTag>(progressValues[i]);
        roundedRectFlow->Setter<RoundedRectFlowlightCornerRadiusTag>(25.0f);

        SetUpTestNode(i, columnCount, rowCount, roundedRectFlow);
    }
}

GRAPHIC_TEST(NGShaderRoundedRectFlowlightTest, EFFECT_TEST, Set_Rounded_Rect_Flowlight_Corner_Radius_Boundary_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < cornerRadii.size(); i++) {
        auto roundedRectFlow = std::make_shared<RSNGRoundedRectFlowlight>();
        InitRoundedRectFlowlight(roundedRectFlow);
        roundedRectFlow->Setter<RoundedRectFlowlightProgressTag>(0.5f);
        roundedRectFlow->Setter<RoundedRectFlowlightCornerRadiusTag>(cornerRadii[i]);

        SetUpTestNode(i, columnCount, rowCount, roundedRectFlow);
    }
}

GRAPHIC_TEST(NGShaderRoundedRectFlowlightTest, EFFECT_TEST,
    Set_Rounded_Rect_Flowlight_Brightness_Progress_Combination_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < brightnessValues.size(); i++) {
        auto roundedRectFlow = std::make_shared<RSNGRoundedRectFlowlight>();
        InitRoundedRectFlowlight(roundedRectFlow);
        roundedRectFlow->Setter<RoundedRectFlowlightProgressTag>(progressValues[i]);
        roundedRectFlow->Setter<RoundedRectFlowlightBrightnessTag>(brightnessValues[i]);

        SetUpTestNode(i, columnCount, rowCount, roundedRectFlow);
    }
}


/*
 * Test rounded rect flowlight with extreme and invalid values
 * Tests malicious inputs: negative values, extremely large values
 */
GRAPHIC_TEST(NGShaderRoundedRectFlowlightTest, EFFECT_TEST, Set_Rounded_Rect_Flowlight_Extreme_Values_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;
    const std::vector<float> extremeValues = {-1.0f, -10.0f, 9999.0f, 1e10f};
    for (size_t i = 0; i < extremeValues.size(); i++) {
        auto roundedRectFlow = std::make_shared<RSNGRoundedRectFlowlight>();
        InitRoundedRectFlowlight(roundedRectFlow);
        roundedRectFlow->Setter<RoundedRectFlowlightProgressTag>(extremeValues[i]);
        SetUpTestNode(i, columnCount, rowCount, roundedRectFlow);
    }
}

} // namespace OHOS::Rosen
