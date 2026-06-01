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

void InitBorderLight(std::shared_ptr<RSNGBorderLight>& borderLight)
{
    if (!borderLight) {
        return;
    }
    // Position (x, y, z)
    borderLight->Setter<BorderLightPositionTag>(Vector3f{0.5f, 0.5f, 0.5f});
    // Color
    borderLight->Setter<BorderLightColorTag>(Vector4f{1.0f, 0.0f, 0.0f, 1.0f});
    // Intensity
    borderLight->Setter<BorderLightIntensityTag>(1.0f);
    // Width
    borderLight->Setter<BorderLightWidthTag>(50.0f);
    // RotationAngle (x, y, z)
    borderLight->Setter<BorderLightRotationAngleTag>(Vector3f{45.0f, 45.0f, 45.0f});
    // CornerRadius
    borderLight->Setter<BorderLightCornerRadiusTag>(5.0f);
}

namespace {
const std::string TEST_IMAGE_PATH = "/data/local/tmp/Images/backGroundImage.jpg";
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 2000;

// Position variations (Vector3f: x, y, z)
const std::vector<Vector3f> borderLightPositions = {
    Vector3f{0.0f, 0.0f, 0.0f},       // Origin
    Vector3f{0.5f, 0.5f, 0.5f},       // Center
    Vector3f{1.0f, 1.0f, 1.0f}        // Max
};

// Color variations (RGBA)
const std::vector<Vector4f> borderLightColors = {
    Vector4f{1.0f, 0.0f, 0.0f, 1.0f},  // Red
    Vector4f{0.0f, 1.0f, 0.0f, 1.0f},  // Green
    Vector4f{0.0f, 0.0f, 1.0f, 1.0f},  // Blue
    Vector4f{1.0f, 1.0f, 1.0f, 1.0f}   // White
};

// Intensity boundary values
const std::vector<float> borderLightIntensities = {0.0f, 0.5f, 1.0f, 1.5f};

// Width boundary values
const std::vector<float> borderLightWidths = {0.0f, 10.0f, 50.0f, 100.0f};

// Rotation angle variations
const std::vector<Vector3f> borderLightRotationAngles = {
    Vector3f{0.0f, 0.0f, 0.0f},
    Vector3f{45.0f, 45.0f, 45.0f},
    Vector3f{90.0f, 90.0f, 90.0f},
    Vector3f{180.0f, 180.0f, 180.0f}
};

// Corner radius variations
const std::vector<float> borderLightCornerRadii = {0.0f, 10.0f, 25.0f, 50.0f};

// Parameter combinations for interaction testing
const std::vector<std::tuple<Vector4f, Vector3f, float, float>> borderLightColorIntensityWidthCombinations = {
    {Vector4f{1.0f, 0.0f, 0.0f, 1.0f}, Vector3f{0.5f, 0.5f, 0.5f}, 0.5f, 10.0f},   // Red, low intensity, thin
    {Vector4f{0.0f, 1.0f, 0.0f, 1.0f}, Vector3f{0.5f, 0.5f, 0.5f}, 1.0f, 50.0f},   // Green, normal intensity, medium
    {Vector4f{0.0f, 0.0f, 1.0f, 1.0f}, Vector3f{0.5f, 0.5f, 0.5f}, 1.5f, 100.0f}   // Blue, high intensity, thick
};

// Extreme and invalid values for robustness testing
const std::vector<float> borderLightExtremeIntensities = {
    -1.0f,         // Negative
    -10.0f,        // Large negative
    100.0f,        // Extremely high
    9999.0f        // astronomical value
};

const std::vector<float> borderLightExtremeWidths = {
    -50.0f,        // Negative width
    -1.0f,         // Small negative
    1000.0f,       // Extremely large
    std::numeric_limits<float>::quiet_NaN()  // NaN
};

std::shared_ptr<RSCanvasNode> CreateEffectChildNode(const size_t i, const size_t columnCount, const size_t rowCount,
    std::shared_ptr<RSEffectNode>& effectNode, std::shared_ptr<RSNGBorderLight>& borderLight)
{
    auto sizeX = (columnCount != 0) ? (SCREEN_WIDTH / columnCount) : SCREEN_WIDTH;
    auto sizeY = (rowCount != 0) ? (SCREEN_HEIGHT * columnCount / rowCount) : SCREEN_HEIGHT;

    int x = (columnCount != 0) ? (i % columnCount) * sizeX : 0;
    int y = (columnCount != 0) ? (i / columnCount) * sizeY : 0;

    auto effectChildNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
    effectChildNode->SetBounds(x, y, sizeX, sizeY);
    effectChildNode->SetFrame(x, y, sizeX, sizeY);
    effectChildNode->SetOverlayNGShader(borderLight);
    effectNode->AddChild(effectChildNode);
    return effectChildNode;
}
}

class NGShaderBorderLightTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    }

    void SetEffectChildNode(const size_t i, const size_t columnCount, const size_t rowCount,
        std::shared_ptr<RSEffectNode>& effectNode, std::shared_ptr<RSNGBorderLight>& borderLight)
    {
        auto effectChildNode = CreateEffectChildNode(i, columnCount, rowCount, effectNode, borderLight);
        RegisterNode(effectChildNode);
    }
 
    std::shared_ptr<RSEffectNode> SetUpEffectNode()
    {
        auto backgroundTestNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
        auto effectNode = RSEffectNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
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

/*
 * Test border light with different colors
 * Tests 4 different color combinations including RGB and white
 */
GRAPHIC_TEST(NGShaderBorderLightTest, EFFECT_TEST, Set_Border_Light_Color_Test)
{
    const size_t columnCount = 1;
    const size_t rowCount = static_cast<size_t>(borderLightColors.size());
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < borderLightColors.size(); i++) {
        auto borderLight = std::make_shared<RSNGBorderLight>();
        InitBorderLight(borderLight);
        borderLight->Setter<BorderLightColorTag>(borderLightColors[i]);
        borderLight->Setter<BorderLightPositionTag>(Vector3f{0.5f, 0.5f, 0.5f});
        borderLight->Setter<BorderLightIntensityTag>(1.0f);
        borderLight->Setter<BorderLightWidthTag>(50.0f);

        SetEffectChildNode(static_cast<size_t>(i), columnCount, rowCount, effectNode, borderLight);
    }
}

/*
 * Test border light with boundary intensity values
 * Tests 4 intensity levels: 0 (no effect), low, normal, high
 */
GRAPHIC_TEST(NGShaderBorderLightTest, EFFECT_TEST, Set_Border_Light_Intensity_Boundary_Test)
{
    const size_t columnCount = 1;
    const size_t rowCount = static_cast<size_t>(borderLightIntensities.size());
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < borderLightIntensities.size(); i++) {
        auto borderLight = std::make_shared<RSNGBorderLight>();
        InitBorderLight(borderLight);
        borderLight->Setter<BorderLightColorTag>(Vector4f{1.0f, 0.0f, 0.0f, 1.0f});
        borderLight->Setter<BorderLightPositionTag>(Vector3f{0.5f, 0.5f, 0.5f});
        borderLight->Setter<BorderLightIntensityTag>(borderLightIntensities[i]);
        borderLight->Setter<BorderLightWidthTag>(50.0f);

        SetEffectChildNode(static_cast<size_t>(i), columnCount, rowCount, effectNode, borderLight);
    }
}

/*
 * Test border light with boundary width values
 * Tests 4 width values: 0 (no effect), small, medium, large
 */
GRAPHIC_TEST(NGShaderBorderLightTest, EFFECT_TEST, Set_Border_Light_Width_Boundary_Test)
{
    const size_t columnCount = 1;
    const size_t rowCount = static_cast<size_t>(borderLightWidths.size());
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < borderLightWidths.size(); i++) {
        auto borderLight = std::make_shared<RSNGBorderLight>();
        InitBorderLight(borderLight);
        borderLight->Setter<BorderLightColorTag>(Vector4f{1.0f, 0.0f, 0.0f, 1.0f});
        borderLight->Setter<BorderLightPositionTag>(Vector3f{0.5f, 0.5f, 0.5f});
        borderLight->Setter<BorderLightIntensityTag>(1.0f);
        borderLight->Setter<BorderLightWidthTag>(borderLightWidths[i]);

        SetEffectChildNode(static_cast<size_t>(i), columnCount, rowCount, effectNode, borderLight);
    }
}

/*
 * Test border light with different rotation angles
 * Tests 4 rotation angles: 0, 45, 90, 180 degrees
 */
GRAPHIC_TEST(NGShaderBorderLightTest, EFFECT_TEST, Set_Border_Light_Rotation_Angle_Test)
{
    const size_t columnCount = 1;
    const size_t rowCount = static_cast<size_t>(borderLightRotationAngles.size());
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < borderLightRotationAngles.size(); i++) {
        auto borderLight = std::make_shared<RSNGBorderLight>();
        InitBorderLight(borderLight);
        borderLight->Setter<BorderLightColorTag>(Vector4f{1.0f, 0.0f, 0.0f, 1.0f});
        borderLight->Setter<BorderLightPositionTag>(Vector3f{0.5f, 0.5f, 0.5f});
        borderLight->Setter<BorderLightIntensityTag>(1.0f);
        borderLight->Setter<BorderLightWidthTag>(50.0f);
        borderLight->Setter<BorderLightRotationAngleTag>(borderLightRotationAngles[i]);

        SetEffectChildNode(static_cast<size_t>(i), columnCount, rowCount, effectNode, borderLight);
    }
}

/*
 * Test border light with corner radius variations
 * Tests 4 corner radius values
 */
GRAPHIC_TEST(NGShaderBorderLightTest, EFFECT_TEST, Set_Border_Light_Corner_Radius_Test)
{
    const size_t columnCount = 1;
    const size_t rowCount = static_cast<size_t>(borderLightCornerRadii.size());
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < borderLightCornerRadii.size(); i++) {
        auto borderLight = std::make_shared<RSNGBorderLight>();
        InitBorderLight(borderLight);
        borderLight->Setter<BorderLightColorTag>(Vector4f{1.0f, 0.0f, 0.0f, 1.0f});
        borderLight->Setter<BorderLightPositionTag>(Vector3f{0.5f, 0.5f, 0.5f});
        borderLight->Setter<BorderLightIntensityTag>(1.0f);
        borderLight->Setter<BorderLightWidthTag>(50.0f);
        borderLight->Setter<BorderLightCornerRadiusTag>(borderLightCornerRadii[i]);

        SetEffectChildNode(static_cast<size_t>(i), columnCount, rowCount, effectNode, borderLight);
    }
}

/*
 * Test border light with color, intensity, and width combinations
 * Tests interaction between color, intensity, and width parameters
 */
GRAPHIC_TEST(NGShaderBorderLightTest, EFFECT_TEST, Set_Border_Light_Color_Intensity_Width_Combination_Test)
{
    const size_t columnCount = 1;
    const size_t rowCount = static_cast<size_t>(borderLightColorIntensityWidthCombinations.size());
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < borderLightColorIntensityWidthCombinations.size(); i++) {
        auto borderLight = std::make_shared<RSNGBorderLight>();
        InitBorderLight(borderLight);
        borderLight->Setter<BorderLightColorTag>(std::get<0>(borderLightColorIntensityWidthCombinations[i]));
        borderLight->Setter<BorderLightPositionTag>(std::get<1>(borderLightColorIntensityWidthCombinations[i]));
        borderLight->Setter<BorderLightIntensityTag>(std::get<2>(borderLightColorIntensityWidthCombinations[i]));
        borderLight->Setter<BorderLightWidthTag>(std::get<3>(borderLightColorIntensityWidthCombinations[i]));

        SetEffectChildNode(static_cast<size_t>(i), columnCount, rowCount, effectNode, borderLight);
    }
}

/*
 * Test border light with extreme and invalid intensity values
 * Tests malicious inputs: negative values, extremely large values
 * Ensures system robustness against invalid parameters
 */
GRAPHIC_TEST(NGShaderBorderLightTest, EFFECT_TEST, Set_Border_Light_Intensity_Extreme_Values_Test)
{
    const size_t columnCount = 1;
    const size_t rowCount = static_cast<size_t>(borderLightExtremeIntensities.size());
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < borderLightExtremeIntensities.size(); i++) {
        auto borderLight = std::make_shared<RSNGBorderLight>();
        InitBorderLight(borderLight);
        borderLight->Setter<BorderLightColorTag>(Vector4f{1.0f, 0.0f, 0.0f, 1.0f});
        borderLight->Setter<BorderLightPositionTag>(Vector3f{0.5f, 0.5f, 0.5f});
        borderLight->Setter<BorderLightIntensityTag>(borderLightExtremeIntensities[i]);
        borderLight->Setter<BorderLightWidthTag>(50.0f);

        SetEffectChildNode(static_cast<size_t>(i), columnCount, rowCount, effectNode, borderLight);
    }
}

/*
 * Test border light with extreme and invalid width values
 * Tests malicious inputs: negative values, extremely large values, NaN
 * Ensures system robustness against invalid parameters
 */
GRAPHIC_TEST(NGShaderBorderLightTest, EFFECT_TEST, Set_Border_Light_Width_Extreme_Values_Test)
{
    const size_t columnCount = 1;
    const size_t rowCount = static_cast<size_t>(borderLightExtremeWidths.size());
    auto effectNode = SetUpEffectNode();
    if (!effectNode) {
        return;
    }

    for (size_t i = 0; i < borderLightExtremeWidths.size(); i++) {
        auto borderLight = std::make_shared<RSNGBorderLight>();
        InitBorderLight(borderLight);
        borderLight->Setter<BorderLightColorTag>(Vector4f{1.0f, 0.0f, 0.0f, 1.0f});
        borderLight->Setter<BorderLightPositionTag>(Vector3f{0.5f, 0.5f, 0.5f});
        borderLight->Setter<BorderLightIntensityTag>(1.0f);
        borderLight->Setter<BorderLightWidthTag>(borderLightExtremeWidths[i]);

        SetEffectChildNode(static_cast<size_t>(i), columnCount, rowCount, effectNode, borderLight);
    }
}

} // namespace OHOS::Rosen

