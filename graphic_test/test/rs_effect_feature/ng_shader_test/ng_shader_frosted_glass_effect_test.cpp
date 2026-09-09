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
#include "ui_effect/property/include/rs_ui_mask_base.h"
#include "ui_effect/property/include/rs_ui_shader_base.h"
#include "../ng_shape_test/ng_sdf_test_utils.h"

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
    // AntiAlias
    frostedGlassEffect->Setter<FrostedGlassEffectAntiAliasTag>(Vector2f{-1.0f, 1.0f});
    // BgRates
    frostedGlassEffect->Setter<FrostedGlassEffectBgRatesTag>(Vector2f{-1.8792225f, 2.7626955f});
    // BgKBS
    frostedGlassEffect->Setter<FrostedGlassEffectBgKBSTag>(Vector3f{0.0073494f, 0.0998859f, 1.2f});
    // BgPos
    frostedGlassEffect->Setter<FrostedGlassEffectBgPosTag>(Vector3f{0.3f, 0.5f, 0.5f});
    // BgNeg
    frostedGlassEffect->Setter<FrostedGlassEffectBgNegTag>(Vector3f{0.5f, 1.0f, 1.0f});
    // BgAlpha
    frostedGlassEffect->Setter<FrostedGlassEffectBgAlphaTag>(0.8f);
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
    // ColorBlendMode
    frostedGlassEffect->Setter<FrostedGlassEffectColorBlendModeTag>(0); // LINEAR_MIX
    // LumaParams
    frostedGlassEffect->Setter<FrostedGlassEffectLumaParamsTag>(Vector3f{0.6628f, 0.3137f, 0.1538f});
    // MaterialColorFraction
    frostedGlassEffect->Setter<FrostedGlassEffectMaterialColorFractionTag>(0.0f);
}

namespace {
const std::string TEST_IMAGE_PATH = "/data/local/tmp/Images/backGroundImage.jpg";
const std::string TEST_TINTEDGLASS_IMAGE_PATH = "/data/local/tmp/Images/colorGrid.jpg";
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

// Extreme material colors
const std::vector<Vector4f> extremeColors = {
    Vector4f{-1.0f, -1.0f, -1.0f, -1.0f},   // All negative
    Vector4f{10.0f, 10.0f, 10.0f, 10.0f},    // Above max (1.0)
    Vector4f{9999.0f, 9999.0f, 9999.0f, 1.0f}, // Extremely large
    Vector4f{0.5f, 1e10f, 0.5f, 0.5f}        // Mixed extreme
};

// Color blend mode variations (MaterialColorBlendMode: LINEAR_MIX=0, TINTED_GLASS=1)
const std::vector<int> colorBlendModes = {0, 1, -1, 2};

// Luma params variations [-1.0, 1.0] per component
const std::vector<Vector3f> lumaParamsList = {
    Vector3f{0.6628f, 0.3137f, 0.1538f},
    Vector3f{0.0f, 0.0f, 0.0f},
    Vector3f{1.0f, 1.0f, 1.0f},
    Vector3f{-1.0f, -1.0f, -1.0f},
    Vector3f{0.5f, 0.5f, 0.5f},
    Vector3f{1.0f, 0.0f, -1.0f}
};

// Material color fraction variations [0.0, 1.0]
const std::vector<float> materialColorFractions = {0.0f, 0.2f, 0.5f, 0.8f, 1.0f, -1.0f};

// Tint color for color-related tests
const Vector4f TEST_TINT_COLOR = Vector4f{0.6f, 0.6f, 0.9f, 1.0f};
}

class NGShaderFrostedGlassEffectTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    }

    std::shared_ptr<Rosen::RSCanvasNode> SetCommonBackgroundNode()
    {
        // set background node
        auto backgroundTestNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
        return backgroundTestNode;
    }

    std::shared_ptr<RSEffectNode> SetDefaultFrostedGlassBlurEffectNode()
    {
        auto effectNode = RSEffectNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        if (!effectNode) {
            return nullptr;
        }
        effectNode->SetBounds({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
        effectNode->SetFrame({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
 
        auto frostedGlassBlurFilter = std::make_shared<RSNGFrostedGlassBlurFilter>();
        frostedGlassBlurFilter->Setter<FrostedGlassBlurRadiusTag>(40.0f);
        frostedGlassBlurFilter->Setter<FrostedGlassBlurRefractOutPxTag>(20.0f);
        frostedGlassBlurFilter->Setter<FrostedGlassBlurRadiusScaleKTag>(1.0f);
        effectNode->SetBackgroundNGFilter(frostedGlassBlurFilter);

        effectNode->SetClipToBounds(true);
        GetRootNode()->AddChild(effectNode);
        RegisterNode(effectNode);
 
        return effectNode;
    }

    void SetEffectChildNode(const size_t i, const size_t columnCount, const size_t rowCount,
        std::shared_ptr<RSEffectNode>& effectNode, std::shared_ptr<RSNGFrostedGlassEffect>& frostedGlass)
    {
        auto sizeX = (columnCount != 0) ? (SCREEN_WIDTH / columnCount) : SCREEN_WIDTH;
        auto sizeY = (rowCount != 0) ? (SCREEN_HEIGHT * columnCount / rowCount) : SCREEN_HEIGHT;

        int x = (columnCount != 0) ? (i % columnCount) * sizeX : 0;
        int y = (columnCount != 0) ? (i / columnCount) * sizeY : 0;

        // set effect child node
        auto effectChildNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        if (!effectChildNode || !effectNode) {
            return;
        }
        effectChildNode->SetBounds(x, y, sizeX, sizeY);
        effectChildNode->SetFrame(x, y, sizeX, sizeY);

        // apply frostedGlassEffect on child node
        effectChildNode->SetBackgroundNGShader(frostedGlass);

        //  apply sdf on effect effect child node
        const RRect defaultRectParam = {
            RectT<float>{sizeX / 4, sizeY / 4, sizeX / 2, sizeY / 2}, sizeX / 16, sizeX / 16
        };
        std::shared_ptr<RSNGShapeBase> sdfShape;
        InitSmoothUnionShapes(sdfShape, defaultRectParam, defaultRectParam, 0.0);
        if (!sdfShape) {
            return;
        }
        effectChildNode->SetSDFShape(sdfShape);

        effectNode->AddChild(effectChildNode);
        RegisterNode(effectChildNode);
    }

    // Per-cell background + per-cell blur effectNode + shader child, so each cell shares the same
    // full background image and only the shader parameter differs across cells.
    void SetBgBlurAndSdfEffectChildNode(const size_t i, const size_t columnCount, const size_t rowCount,
        std::shared_ptr<RSNGFrostedGlassEffect>& frostedGlass)
    {
        auto sizeX = (columnCount != 0) ? (SCREEN_WIDTH / columnCount) : SCREEN_WIDTH;
        auto sizeY = (rowCount != 0) ? (SCREEN_HEIGHT * columnCount / rowCount) : SCREEN_HEIGHT;
        int x = (columnCount != 0) ? (i % columnCount) * sizeX : 0;
        int y = (columnCount != 0) ? (i / columnCount) * sizeY : 0;

        // 1. per-cell background: the same image scaled to fill the cell
        auto bgNode = SetUpNodeBgImage(TEST_TINTEDGLASS_IMAGE_PATH, {x, y, sizeX, sizeY});
        GetRootNode()->AddChild(bgNode);
        RegisterNode(bgNode);

        // 2. per-cell blur effectNode (child of bgNode): blurs the cell background into a texture
        auto effectNode = RSEffectNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        if (!effectNode) {
            return;
        }
        effectNode->SetBounds({0, 0, sizeX, sizeY});
        effectNode->SetFrame({0, 0, sizeX, sizeY});
        auto frostedGlassBlurFilter = std::make_shared<RSNGFrostedGlassBlurFilter>();
        frostedGlassBlurFilter->Setter<FrostedGlassBlurRadiusTag>(15.0f);
        frostedGlassBlurFilter->Setter<FrostedGlassBlurRefractOutPxTag>(3.0f);
        frostedGlassBlurFilter->Setter<FrostedGlassBlurRadiusScaleKTag>(1.0f);
        effectNode->SetBackgroundNGFilter(frostedGlassBlurFilter);
        effectNode->SetClipToBounds(true);
        bgNode->AddChild(effectNode);
        RegisterNode(effectNode);


        // 3. shader child (child of effectNode): reads the blurred texture and applies the shader + SDF
        auto effectChildNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        if (!effectChildNode) {
            return;
        }
        effectChildNode->SetBounds({0, 0, sizeX, sizeY});
        effectChildNode->SetFrame({0, 0, sizeX, sizeY});
        effectChildNode->SetBackgroundNGShader(frostedGlass);

        const RRect defaultRectParam = {
            RectT<float>{20.0, 20.0, sizeX / 1.1, sizeY / 1.1}, sizeX / 16, sizeX / 16
        };
        std::shared_ptr<RSNGShapeBase> sdfShape;
        InitSmoothUnionShapes(sdfShape, defaultRectParam, defaultRectParam, 0.0);
        if (!sdfShape) {
            return;
        }
        effectChildNode->SetSDFShape(sdfShape);

        effectNode->AddChild(effectChildNode);
        RegisterNode(effectChildNode);
    }
};

GRAPHIC_TEST(NGShaderFrostedGlassEffectTest, EFFECT_TEST, Set_Frosted_Glass_Effect_Material_Color_Test)
{
    const size_t columnCount = 1;
    const size_t rowCount = static_cast<size_t>(materialColors.size());
    auto backgroundTestNode = SetCommonBackgroundNode();
    auto effectNode = SetDefaultFrostedGlassBlurEffectNode();
    if (!backgroundTestNode || !effectNode) {
        return;
    }

    for (size_t i = 0; i < materialColors.size(); i++) {
        auto frostedGlass = std::make_shared<RSNGFrostedGlassEffect>();
        InitFrostedGlassEffect(frostedGlass);
        frostedGlass->Setter<FrostedGlassEffectMaterialColorTag>(materialColors[i]);

        SetEffectChildNode(static_cast<size_t>(i), columnCount, rowCount, effectNode, frostedGlass);
    }
}

GRAPHIC_TEST(NGShaderFrostedGlassEffectTest, EFFECT_TEST, Set_Frosted_Glass_Effect_Shape_Test)
{
    const size_t columnCount = 1;
    const size_t rowCount = static_cast<size_t>(shapeValues.size());
    auto backgroundTestNode = SetCommonBackgroundNode();
    auto effectNode = SetDefaultFrostedGlassBlurEffectNode();
    if (!backgroundTestNode || !effectNode) {
        return;
    }

    for (size_t i = 0; i < shapeValues.size(); i++) {
        auto frostedGlass = std::make_shared<RSNGFrostedGlassEffect>();
        InitFrostedGlassEffect(frostedGlass);
        frostedGlass->Setter<FrostedGlassEffectMaterialColorTag>(Vector4f{0.8f, 0.8f, 0.8f, 1.0f});
        frostedGlass->Setter<FrostedGlassEffectShapeTag>(shapeValues[i]);

        SetEffectChildNode(static_cast<size_t>(i), columnCount, rowCount, effectNode, frostedGlass);
    }
}

/*
 * Test frosted glass effect with extreme material color values
 * Tests malicious inputs: negative values, extremely large values
 */
GRAPHIC_TEST(NGShaderFrostedGlassEffectTest, EFFECT_TEST, Set_Frosted_Glass_Effect_Extreme_Values_Test)
{
    const size_t columnCount = 1;
    const size_t rowCount = static_cast<size_t>(extremeColors.size());
    auto backgroundTestNode = SetCommonBackgroundNode();
    auto effectNode = SetDefaultFrostedGlassBlurEffectNode();
    if (!backgroundTestNode || !effectNode) {
        return;
    }

    for (size_t i = 0; i < extremeColors.size(); i++) {
        auto frostedGlass = std::make_shared<RSNGFrostedGlassEffect>();
        InitFrostedGlassEffect(frostedGlass);
        frostedGlass->Setter<FrostedGlassEffectMaterialColorTag>(extremeColors[i]);

        SetEffectChildNode(static_cast<size_t>(i), columnCount, rowCount, effectNode, frostedGlass);
    }
}

GRAPHIC_TEST(NGShaderFrostedGlassEffectTest, EFFECT_TEST, Set_Frosted_Glass_Effect_Color_Blend_Mode_Test)
{
    const size_t columnCount = 1;
    const size_t rowCount = static_cast<size_t>(colorBlendModes.size());
    for (size_t i = 0; i < colorBlendModes.size(); i++) {
        auto frostedGlass = std::make_shared<RSNGFrostedGlassEffect>();
        InitFrostedGlassEffect(frostedGlass);
        frostedGlass->Setter<FrostedGlassEffectMaterialColorTag>(TEST_TINT_COLOR);
        frostedGlass->Setter<FrostedGlassEffectColorBlendModeTag>(colorBlendModes[i]);

        SetBgBlurAndSdfEffectChildNode(i, columnCount, rowCount, frostedGlass);
    }
}

GRAPHIC_TEST(NGShaderFrostedGlassEffectTest, EFFECT_TEST, Set_Frosted_Glass_Effect_Luma_Params_Test)
{
    const size_t columnCount = 1;
    const size_t rowCount = static_cast<size_t>(lumaParamsList.size());
    for (size_t i = 0; i < lumaParamsList.size(); i++) {
        auto frostedGlass = std::make_shared<RSNGFrostedGlassEffect>();
        InitFrostedGlassEffect(frostedGlass);
        frostedGlass->Setter<FrostedGlassEffectMaterialColorTag>(TEST_TINT_COLOR);
        frostedGlass->Setter<FrostedGlassEffectColorBlendModeTag>(1); // TINTED_GLASS
        frostedGlass->Setter<FrostedGlassEffectMaterialColorFractionTag>(0.5f);
        frostedGlass->Setter<FrostedGlassEffectLumaParamsTag>(lumaParamsList[i]);

        SetBgBlurAndSdfEffectChildNode(i, columnCount, rowCount, frostedGlass);
    }
}

GRAPHIC_TEST(NGShaderFrostedGlassEffectTest, EFFECT_TEST, Set_Frosted_Glass_Effect_Material_Color_Fraction_Test)
{
    const size_t columnCount = 1;
    const size_t rowCount = static_cast<size_t>(materialColorFractions.size());
    for (size_t i = 0; i < materialColorFractions.size(); i++) {
        auto frostedGlass = std::make_shared<RSNGFrostedGlassEffect>();
        InitFrostedGlassEffect(frostedGlass);
        frostedGlass->Setter<FrostedGlassEffectMaterialColorTag>(TEST_TINT_COLOR);
        frostedGlass->Setter<FrostedGlassEffectColorBlendModeTag>(1); // TINTED_GLASS
        frostedGlass->Setter<FrostedGlassEffectMaterialColorFractionTag>(materialColorFractions[i]);

        SetBgBlurAndSdfEffectChildNode(i, columnCount, rowCount, frostedGlass);
    }
}

} // namespace OHOS::Rosen