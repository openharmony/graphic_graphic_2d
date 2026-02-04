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
#include "ui_effect/property/include/rs_ui_mask_base.h"
#include "ui_effect/property/include/rs_ui_shader_base.h"
#include "ui/rs_effect_node.h"
#include "ng_colorgradient_effect_test_utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class NGColorGradientEffectTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    }

    RSCanvasNode::SharedPtr SetUpEffectNode()
    {
        auto effectNode = RSCanvasNode::Create();
        effectNode->SetBounds({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
        effectNode->SetFrame({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
        effectNode->SetBackgroundColor(0xFF333333);
        return effectNode;
    }

    void CreateAndSetColorGradientNode(RSCanvasNode::SharedPtr& effectNode,
        const ColorGradientEffectTestParams& params)
    {
        auto colorGradientEffect = std::make_shared<RSNGColorGradientEffect>();
        InitColorGradientEffect(colorGradientEffect, params);
        effectNode->SetBackgroundNGShader(colorGradientEffect);
        GetRootNode()->AddChild(effectNode);
        RegisterNode(effectNode);
    }
};

// Test with single color (dimension = 1)
GRAPHIC_TEST(NGColorGradientEffectTest, EFFECT_TEST, Set_NG_ColorGradient_Effect_Single_Color_Test)
{
    auto effectNode = SetUpEffectNode();
    CreateAndSetColorGradientNode(effectNode, SINGLECOLORPARAMS);
}

// Test with two colors (dimension = 2)
GRAPHIC_TEST(NGColorGradientEffectTest, EFFECT_TEST, Set_NG_ColorGradient_Effect_Two_Colors_Test)
{
    auto effectNode = SetUpEffectNode();
    CreateAndSetColorGradientNode(effectNode, TWOCOLORSPARAMS);
}

// Test with three colors (dimension = 3)
GRAPHIC_TEST(NGColorGradientEffectTest, EFFECT_TEST, Set_NG_ColorGradient_Effect_Three_Colors_Test)
{
    auto effectNode = SetUpEffectNode();
    CreateAndSetColorGradientNode(effectNode, THREECOLORSPARAMS);
}

// Test with four colors (dimension = 4)
GRAPHIC_TEST(NGColorGradientEffectTest, EFFECT_TEST, Set_NG_ColorGradient_Effect_Four_Colors_Test)
{
    auto effectNode = SetUpEffectNode();
    CreateAndSetColorGradientNode(effectNode, FOURCOLORSPARAMS);
}

// Test with five colors (dimension = 5)
GRAPHIC_TEST(NGColorGradientEffectTest, EFFECT_TEST, Set_NG_ColorGradient_Effect_Five_Colors_Test)
{
    auto effectNode = SetUpEffectNode();
    CreateAndSetColorGradientNode(effectNode, FIVECOLORSPARAMS);
}

// Test with six colors (dimension = 6)
GRAPHIC_TEST(NGColorGradientEffectTest, EFFECT_TEST, Set_NG_ColorGradient_Effect_Six_Colors_Test)
{
    auto effectNode = SetUpEffectNode();
    CreateAndSetColorGradientNode(effectNode, SIXCOLORSPARAMS);
}

// Test with eight colors (dimension = 8)
GRAPHIC_TEST(NGColorGradientEffectTest, EFFECT_TEST, Set_NG_ColorGradient_Effect_Eight_Colors_Test)
{
    auto effectNode = SetUpEffectNode();
    CreateAndSetColorGradientNode(effectNode, EIGHTCOLORSPARAMS);
}

// Test with eleven colors (dimension = 11, max < 12)
GRAPHIC_TEST(NGColorGradientEffectTest, EFFECT_TEST, Set_NG_ColorGradient_Effect_Eleven_Colors_Test)
{
    auto effectNode = SetUpEffectNode();
    CreateAndSetColorGradientNode(effectNode, ELEVENCOLORSPARAMS);
}

// Test with invalid color values
GRAPHIC_TEST(NGColorGradientEffectTest, EFFECT_TEST, Set_NG_ColorGradient_Effect_Invalid_Color_Test)
{
    auto effectNode = SetUpEffectNode();
    CreateAndSetColorGradientNode(effectNode, INVALIDCOLORPARAMS);
}

// Test with extreme position values
GRAPHIC_TEST(NGColorGradientEffectTest, EFFECT_TEST, Set_NG_ColorGradient_Effect_Extreme_Position_Test)
{
    auto effectNode = SetUpEffectNode();
    CreateAndSetColorGradientNode(effectNode, EXTREMEPOSITIONPARAMS);
}

// Test with varying strengths
GRAPHIC_TEST(NGColorGradientEffectTest, EFFECT_TEST, Set_NG_ColorGradient_Effect_Varying_Strength_Test)
{
    auto effectNode = SetUpEffectNode();
    CreateAndSetColorGradientNode(effectNode, VARYINGSTRENGTHPARAMS);
}

// Test with different blend modes
GRAPHIC_TEST(NGColorGradientEffectTest, EFFECT_TEST, Set_NG_ColorGradient_Effect_Blend_Mode_Test)
{
    auto effectNode = SetUpEffectNode();
    CreateAndSetColorGradientNode(effectNode, BLENDMODEPARAMS);
}

// Test Blend parameter specifically
GRAPHIC_TEST(NGColorGradientEffectTest, EFFECT_TEST, Set_NG_ColorGradient_Effect_Blend_Test)
{
    auto effectNode = SetUpEffectNode();
    auto colorGradientEffect = std::make_shared<RSNGColorGradientEffect>();
    InitColorGradientEffect(colorGradientEffect, THREECOLORSPARAMS);
    colorGradientEffect->Setter<ColorGradientEffectBlendTag>(0.8f);

    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
    canvasNode->SetFrame({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
    canvasNode->SetBackgroundNGShader(colorGradientEffect);
    effectNode->AddChild(canvasNode);
    RegisterNode(canvasNode);
}

// Test BlendK parameter specifically
GRAPHIC_TEST(NGColorGradientEffectTest, EFFECT_TEST, Set_NG_ColorGradient_Effect_BlendK_Test)
{
    auto effectNode = SetUpEffectNode();
    auto colorGradientEffect = std::make_shared<RSNGColorGradientEffect>();
    InitColorGradientEffect(colorGradientEffect, THREECOLORSPARAMS);
    colorGradientEffect->Setter<ColorGradientEffectBlendKTag>(1.5f);

    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
    canvasNode->SetFrame({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
    canvasNode->SetBackgroundNGShader(colorGradientEffect);
    effectNode->AddChild(canvasNode);
    RegisterNode(canvasNode);
}

// Test Brightness parameter specifically
GRAPHIC_TEST(NGColorGradientEffectTest, EFFECT_TEST, Set_NG_ColorGradient_Effect_Brightness_Test)
{
    auto effectNode = SetUpEffectNode();
    auto colorGradientEffect = std::make_shared<RSNGColorGradientEffect>();
    InitColorGradientEffect(colorGradientEffect, THREECOLORSPARAMS);
    colorGradientEffect->Setter<ColorGradientEffectBrightnessTag>(1.2f);

    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
    canvasNode->SetFrame({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
    canvasNode->SetBackgroundNGShader(colorGradientEffect);
    effectNode->AddChild(canvasNode);
    RegisterNode(canvasNode);
}

// Test ColorNumber parameter specifically
GRAPHIC_TEST(NGColorGradientEffectTest, EFFECT_TEST, Set_NG_ColorGradient_Effect_ColorNumber_Test)
{
    auto effectNode = SetUpEffectNode();
    auto colorGradientEffect = std::make_shared<RSNGColorGradientEffect>();
    InitColorGradientEffect(colorGradientEffect, FIVECOLORSPARAMS);
    colorGradientEffect->Setter<ColorGradientEffectColorNumberTag>(5.0f);

    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
    canvasNode->SetFrame({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
    canvasNode->SetBackgroundNGShader(colorGradientEffect);
    effectNode->AddChild(canvasNode);
    RegisterNode(canvasNode);
}

// Test with Mask
GRAPHIC_TEST(NGColorGradientEffectTest, EFFECT_TEST, Set_NG_ColorGradient_Effect_With_Mask_Test)
{
    auto effectNode = SetUpEffectNode();
    auto colorGradientEffect = std::make_shared<RSNGColorGradientEffect>();
    InitColorGradientEffect(colorGradientEffect, FOURCOLORSPARAMS);

    auto imageMask = std::make_shared<RSNGImageMask>();
    colorGradientEffect->Setter<ColorGradientEffectMaskTag>(
        std::static_pointer_cast<RSNGMaskBase>(imageMask));

    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
    canvasNode->SetFrame({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
    canvasNode->SetBackgroundNGShader(colorGradientEffect);
    effectNode->AddChild(canvasNode);
    RegisterNode(canvasNode);
}

} // namespace OHOS::Rosen
