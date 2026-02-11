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

void InitDistortChroma(std::shared_ptr<RSNGDistortChroma>& distortChroma)
{
    if (!distortChroma) {
        return;
    }
    // Progress
    distortChroma->Setter<DistortChromaProgressTag>(0.5f);
    // Granularity
    distortChroma->Setter<DistortChromaGranularityTag>(Vector2f{1.0f, 1.0f});
    // Frequency
    distortChroma->Setter<DistortChromaFrequencyTag>(1.0f);
    // Sharpness
    distortChroma->Setter<DistortChromaSharpnessTag>(1.0f);
    // Brightness
    distortChroma->Setter<DistortChromaBrightnessTag>(1.0f);
    // Dispersion
    distortChroma->Setter<DistortChromaDispersionTag>(0.5f);
    // DistortFactor
    distortChroma->Setter<DistortChromaDistortFactorTag>(Vector2f{1.0f, 1.0f});
    // Saturation
    distortChroma->Setter<DistortChromaSaturationTag>(1.0f);
    // ColorStrength
    distortChroma->Setter<DistortChromaColorStrengthTag>(Vector3f{1.0f, 1.0f, 1.0f});
    // VerticalMoveDistance
    distortChroma->Setter<DistortChromaVerticalMoveDistanceTag>(0.0f);
}

namespace {
const std::string TEST_IMAGE_PATH = "/data/local/tmp/Images/backGroundImage.jpg";
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 2000;

// Progress values
const std::vector<float> progressValues = {0.0f, 0.3f, 0.5f, 1.0f};

// Distort factor values (Vector2f for X and Y components)
const std::vector<Vector2f> distortFactors = {
    Vector2f{0.0f, 0.0f},
    Vector2f{0.5f, 0.5f},
    Vector2f{1.0f, 1.0f},
    Vector2f{2.0f, 2.0f}
};

// Brightness values
const std::vector<float> brightnessValues = {0.0f, 0.5f, 1.0f, 2.0f};
}

class NGShaderDistortChromaTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    }

private:
    void SetUpTestNode(const size_t i, const size_t columnCount, const size_t rowCount,
        std::shared_ptr<RSNGDistortChroma>& distortChroma)
    {
        if (columnCount == 0 || rowCount == 0) {
            return;  // Invalid test configuration
        }
        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t x = (i % columnCount) * sizeX;
        const size_t y = (i / columnCount) * sizeY;

        auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {x, y, sizeX, sizeY});
        testNode->SetBackgroundNGShader(distortChroma);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
};

GRAPHIC_TEST(NGShaderDistortChromaTest, EFFECT_TEST, Set_Distort_Chroma_Progress_Boundary_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < progressValues.size(); i++) {
        auto distortChroma = std::make_shared<RSNGDistortChroma>();
        InitDistortChroma(distortChroma);
        distortChroma->Setter<DistortChromaProgressTag>(progressValues[i]);

        SetUpTestNode(i, columnCount, rowCount, distortChroma);
    }
}

GRAPHIC_TEST(NGShaderDistortChromaTest, EFFECT_TEST, Set_Distort_Chroma_Distort_Factor_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < distortFactors.size(); i++) {
        auto distortChroma = std::make_shared<RSNGDistortChroma>();
        InitDistortChroma(distortChroma);
        distortChroma->Setter<DistortChromaProgressTag>(0.5f);
        distortChroma->Setter<DistortChromaDistortFactorTag>(distortFactors[i]);

        SetUpTestNode(i, columnCount, rowCount, distortChroma);
    }
}

GRAPHIC_TEST(NGShaderDistortChromaTest, EFFECT_TEST, Set_Distort_Chroma_Brightness_Progress_Combination_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < brightnessValues.size(); i++) {
        auto distortChroma = std::make_shared<RSNGDistortChroma>();
        InitDistortChroma(distortChroma);
        distortChroma->Setter<DistortChromaProgressTag>(progressValues[i]);
        distortChroma->Setter<DistortChromaBrightnessTag>(brightnessValues[i]);

        SetUpTestNode(i, columnCount, rowCount, distortChroma);
    }
}

GRAPHIC_TEST(NGShaderDistortChromaTest, EFFECT_TEST, Set_Distort_Chroma_Extreme_Values_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;
    const std::vector<float> extremeValues = {-1.0f, -10.0f, 9999.0f, 1e10f};
    for (size_t i = 0; i < extremeValues.size(); i++) {
        auto distortChroma = std::make_shared<RSNGDistortChroma>();
        InitDistortChroma(distortChroma);
        distortChroma->Setter<DistortChromaProgressTag>(extremeValues[i]);
        SetUpTestNode(i, columnCount, rowCount, distortChroma);
    }
}

} // namespace OHOS::Rosen