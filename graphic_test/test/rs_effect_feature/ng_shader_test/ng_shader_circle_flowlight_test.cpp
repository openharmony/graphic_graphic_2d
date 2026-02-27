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

void InitCircleFlowlight(std::shared_ptr<RSNGCircleFlowlight>& circleFlowlight)
{
    if (!circleFlowlight) {
        return;
    }
    // Colors
    circleFlowlight->Setter<CircleFlowlightColor0Tag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
    circleFlowlight->Setter<CircleFlowlightColor1Tag>(Vector4f{0.8f, 0.8f, 1.0f, 1.0f});
    circleFlowlight->Setter<CircleFlowlightColor2Tag>(Vector4f{0.6f, 0.6f, 1.0f, 1.0f});
    circleFlowlight->Setter<CircleFlowlightColor3Tag>(Vector4f{0.4f, 0.4f, 1.0f, 1.0f});
    // RotationFrequency
    circleFlowlight->Setter<CircleFlowlightRotationFrequencyTag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
    // RotationAmplitude
    circleFlowlight->Setter<CircleFlowlightRotationAmplitudeTag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
    // RotationSeed
    circleFlowlight->Setter<CircleFlowlightRotationSeedTag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
    // GradientX
    circleFlowlight->Setter<CircleFlowlightGradientXTag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
    // GradientY
    circleFlowlight->Setter<CircleFlowlightGradientYTag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
    // Progress
    circleFlowlight->Setter<CircleFlowlightProgressTag>(0.5f);
    // Strength
    circleFlowlight->Setter<CircleFlowlightStrengthTag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
    // DistortStrength
    circleFlowlight->Setter<CircleFlowlightDistortStrengthTag>(1.0f);
    // BlendGradient
    circleFlowlight->Setter<CircleFlowlightBlendGradientTag>(1.0f);
}

namespace {
const std::string TEST_IMAGE_PATH = "/data/local/tmp/Images/backGroundImage.jpg";
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 2000;

// Color variations
const std::vector<Vector4f> circleFlowColors = {
    Vector4f{1.0f, 0.0f, 0.0f, 1.0f},  // Red
    Vector4f{0.0f, 1.0f, 0.0f, 1.0f},  // Green
    Vector4f{0.0f, 0.0f, 1.0f, 1.0f},  // Blue
    Vector4f{1.0f, 1.0f, 1.0f, 1.0f}   // White
};

// Progress boundary values
const std::vector<float> circleFlowProgress = {0.0f, 0.3f, 0.5f, 1.0f};

// Strength variations
const std::vector<Vector4f> circleFlowStrength = {
    Vector4f{0.0f, 0.0f, 0.0f, 0.0f},
    Vector4f{0.5f, 0.5f, 0.5f, 0.5f},
    Vector4f{1.0f, 1.0f, 1.0f, 1.0f},
    Vector4f{2.0f, 2.0f, 2.0f, 2.0f}
};

// Rotation frequency
const std::vector<float> circleFlowRotationFrequency = {0.5f, 1.0f, 2.0f};

// Rotation amplitude
const std::vector<float> circleFlowRotationAmplitude = {0.1f, 0.5f, 1.0f};
}

class NGShaderCircleFlowlightTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    }

private:
    void SetUpTestNode(const size_t i, const size_t columnCount, const size_t rowCount,
        std::shared_ptr<RSNGCircleFlowlight>& circleFlowlight)
    {
        if (columnCount == 0 || rowCount == 0) {
            return;  // Invalid test configuration
        }
        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t x = (i % columnCount) * sizeX;
        const size_t y = (i / columnCount) * sizeY;

        auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {x, y, sizeX, sizeY});
        testNode->SetBackgroundNGShader(circleFlowlight);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
};

/*
 * Test circle flowlight with different colors
 */
GRAPHIC_TEST(NGShaderCircleFlowlightTest, EFFECT_TEST, Set_Circle_Flowlight_Color_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < circleFlowColors.size(); i++) {
        auto circleFlowlight = std::make_shared<RSNGCircleFlowlight>();
        InitCircleFlowlight(circleFlowlight);
        circleFlowlight->Setter<CircleFlowlightColor0Tag>(circleFlowColors[i]);
        circleFlowlight->Setter<CircleFlowlightColor1Tag>(circleFlowColors[(i + 1) % circleFlowColors.size()]);
        circleFlowlight->Setter<CircleFlowlightColor2Tag>(circleFlowColors[(i + 2) % circleFlowColors.size()]);
        circleFlowlight->Setter<CircleFlowlightColor3Tag>(circleFlowColors[(i + 3) % circleFlowColors.size()]);
        circleFlowlight->Setter<CircleFlowlightProgressTag>(0.5f);
        circleFlowlight->Setter<CircleFlowlightStrengthTag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});

        SetUpTestNode(i, columnCount, rowCount, circleFlowlight);
    }
}

/*
 * Test circle flowlight with boundary progress values
 */
GRAPHIC_TEST(NGShaderCircleFlowlightTest, EFFECT_TEST, Set_Circle_Flowlight_Progress_Boundary_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < circleFlowProgress.size(); i++) {
        auto circleFlowlight = std::make_shared<RSNGCircleFlowlight>();
        InitCircleFlowlight(circleFlowlight);
        circleFlowlight->Setter<CircleFlowlightColor0Tag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
        circleFlowlight->Setter<CircleFlowlightColor1Tag>(Vector4f{1.0f, 0.0f, 0.0f, 1.0f});
        circleFlowlight->Setter<CircleFlowlightColor2Tag>(Vector4f{0.0f, 1.0f, 0.0f, 1.0f});
        circleFlowlight->Setter<CircleFlowlightColor3Tag>(Vector4f{0.0f, 0.0f, 1.0f, 1.0f});
        circleFlowlight->Setter<CircleFlowlightProgressTag>(circleFlowProgress[i]);
        circleFlowlight->Setter<CircleFlowlightStrengthTag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});

        SetUpTestNode(i, columnCount, rowCount, circleFlowlight);
    }
}

/*
 * Test circle flowlight with strength and progress combinations
 */
GRAPHIC_TEST(NGShaderCircleFlowlightTest, EFFECT_TEST, Set_Circle_Flowlight_Strength_Progress_Combination_Test)
{
    const size_t columnCount = 3;
    const size_t rowCount = 1;

    for (size_t i = 0; i < circleFlowStrength.size(); i++) {
        auto circleFlowlight = std::make_shared<RSNGCircleFlowlight>();
        InitCircleFlowlight(circleFlowlight);
        circleFlowlight->Setter<CircleFlowlightColor0Tag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
        circleFlowlight->Setter<CircleFlowlightColor1Tag>(Vector4f{1.0f, 0.0f, 0.0f, 1.0f});
        circleFlowlight->Setter<CircleFlowlightColor2Tag>(Vector4f{0.0f, 1.0f, 0.0f, 1.0f});
        circleFlowlight->Setter<CircleFlowlightColor3Tag>(Vector4f{0.0f, 0.0f, 1.0f, 1.0f});
        circleFlowlight->Setter<CircleFlowlightProgressTag>(circleFlowProgress[i]);
        circleFlowlight->Setter<CircleFlowlightStrengthTag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});

        SetUpTestNode(i, columnCount, rowCount, circleFlowlight);
    }
}

/*
 * Test circle flowlight with extreme and invalid progress values
 * Tests malicious inputs: negative, out-of-range values
 */
GRAPHIC_TEST(NGShaderCircleFlowlightTest, EFFECT_TEST, Set_Circle_Flowlight_Progress_Extreme_Values_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    const std::vector<float> extremeProgress = {-1.0f, -0.5f, 1.5f, 2.0f, 9999.0f};

    for (size_t i = 0; i < extremeProgress.size(); i++) {
        auto circleFlowlight = std::make_shared<RSNGCircleFlowlight>();
        InitCircleFlowlight(circleFlowlight);
        circleFlowlight->Setter<CircleFlowlightColor0Tag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
        circleFlowlight->Setter<CircleFlowlightColor1Tag>(Vector4f{1.0f, 0.0f, 0.0f, 1.0f});
        circleFlowlight->Setter<CircleFlowlightColor2Tag>(Vector4f{0.0f, 1.0f, 0.0f, 1.0f});
        circleFlowlight->Setter<CircleFlowlightColor3Tag>(Vector4f{0.0f, 0.0f, 1.0f, 1.0f});
        circleFlowlight->Setter<CircleFlowlightProgressTag>(extremeProgress[i]);
        circleFlowlight->Setter<CircleFlowlightStrengthTag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});

        SetUpTestNode(i, columnCount, rowCount, circleFlowlight);
    }
}

} // namespace OHOS::Rosen

