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
#include "ng_harmonium_effect_test_utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class NGShaderHarmoniumEffectTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    }

    void SetEffectChildNode(const int i, const int columnCount, const int rowCount,
        std::shared_ptr<RSEffectNode>& effectNode, std::shared_ptr<RSNGHarmoniumEffect>& harmoniumEffect)
    {
        auto effectChildNode = CreateEffectChildNode(i, columnCount, rowCount, effectNode, harmoniumEffect);
        RegisterNode(effectChildNode);
    }

    std::shared_ptr<RSEffectNode> SetUpEffectNode()
    {
        auto backgroundTestNode = SetUpNodeBgImage(BACKGROUND_IMAGE_PATH, {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
        auto effectNode = RSEffectNode::Create();
        effectNode->SetBounds({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
        effectNode->SetFrame({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
        std::shared_ptr<Rosen::RSFilter> backFilter = Rosen::RSFilter::CreateMaterialFilter(BLUR_RADIUS, 1, 1, 0,
            BLUR_COLOR_MODE::DEFAULT, true);
        effectNode->SetBackgroundFilter(backFilter);
        effectNode->SetClipToBounds(true);
        GetRootNode()->AddChild(backgroundTestNode);
        backgroundTestNode->AddChild(effectNode);
        RegisterNode(effectNode);
        RegisterNode(backgroundTestNode);
        return effectNode;
    }

    template<class T>
    void SetUpHarmoniumEffect(std::shared_ptr<RSEffectNode>& effectNode,
        std::function<void(std::shared_ptr<RSNGHarmoniumEffect>&, T)> func, std::vector<T> progressVec)
    {
        const int columnCount = 2;
        const int rowCount = 4;
        for (int i = 0; i < rowCount; i++) {
            auto harmoniumEffect = std::make_shared<RSNGHarmoniumEffect>();
            InitHarmoniumEffect(harmoniumEffect);
            if (func) {
                func(harmoniumEffect, progressVec[i]);
            }
            SetEffectChildNode(i, columnCount, rowCount, effectNode, harmoniumEffect);
        }
    }
};

void SetRippleProgress(std::shared_ptr<RSNGHarmoniumEffect>& harmoniumEffect, float rippleProgress)
{
    harmoniumEffect->Setter<HarmoniumEffectRippleProgressTag>(rippleProgress);
    std::vector<Vector2f> ripplePosition = {{0.5f, 0.5f}};
    harmoniumEffect->Setter<HarmoniumEffectRipplePositionTag>(ripplePosition);
}

void SetRipplePosition(std::shared_ptr<RSNGHarmoniumEffect>& harmoniumEffect, std::vector<Vector2f> ripplePosition)
{
    harmoniumEffect->Setter<HarmoniumEffectRippleProgressTag>(4.5f);
    harmoniumEffect->Setter<HarmoniumEffectRipplePositionTag>(ripplePosition);
}

void SetImageMask(std::shared_ptr<RSNGHarmoniumEffect>& harmoniumEffect, std::string imagePath)
{
    auto imageMask = std::make_shared<RSNGImageMask>();
    std::shared_ptr<Media::PixelMap> pixelmap = DecodePixelMap(imagePath, Media::AllocatorType::SHARE_MEM_ALLOC);
    imageMask->Setter<ImageMaskImageTag>(pixelmap);
    harmoniumEffect->Setter<HarmoniumEffectMaskTag>(std::static_pointer_cast<RSNGMaskBase>(imageMask));
}

void SetDistortProgress(std::shared_ptr<RSNGHarmoniumEffect>& harmoniumEffect, float distortProgress)
{
    harmoniumEffect->Setter<HarmoniumEffectDistortProgressTag>(distortProgress);
    harmoniumEffect->Setter<HarmoniumEffectDistortFactorTag>(1.f);
    SetImageMask(harmoniumEffect, DISTOR_IMAGE_PATH);
}

void SetDistortFactor(std::shared_ptr<RSNGHarmoniumEffect>& harmoniumEffect, float distortFactor)
{
    harmoniumEffect->Setter<HarmoniumEffectDistortProgressTag>(0.75f);
    harmoniumEffect->Setter<HarmoniumEffectDistortFactorTag>(distortFactor);
    SetImageMask(harmoniumEffect, DISTOR_IMAGE_PATH);
}

GRAPHIC_TEST(NGShaderHarmoniumEffectTest, EFFECT_TEST, Set_NG_Shader_Harmonium_Effect_Test)
{
    auto effectNode = SetUpEffectNode();
    SetUpHarmoniumEffect<nullptr_t>(effectNode, nullptr, {});
}

GRAPHIC_TEST(NGShaderHarmoniumEffectTest, EFFECT_TEST, Set_NG_Shader_Harmonium_Effect_Refraction_Test)
{
    auto effectNode = SetUpEffectNode();
    SetUpHarmoniumEffect<float>(effectNode, [](std::shared_ptr<RSNGHarmoniumEffect>& harmoniumEffect, float refraction)
        {harmoniumEffect->Setter<HarmoniumEffectRefractionFactorTag>(refraction);}, refractionVec);
}

GRAPHIC_TEST(NGShaderHarmoniumEffectTest, EFFECT_TEST, Set_NG_Shader_Harmonium_Effect_Reflection_Test)
{
    auto effectNode = SetUpEffectNode();
    SetUpHarmoniumEffect<float>(effectNode, [](std::shared_ptr<RSNGHarmoniumEffect>& harmoniumEffect, float reflection)
        {harmoniumEffect->Setter<HarmoniumEffectReflectionFactorTag>(reflection);}, reflectionVec);
}

GRAPHIC_TEST(NGShaderHarmoniumEffectTest, EFFECT_TEST, Set_NG_Shader_Harmonium_Effect_Material_Test)
{
    auto effectNode = SetUpEffectNode();
    SetUpHarmoniumEffect<float>(effectNode, [](std::shared_ptr<RSNGHarmoniumEffect>& harmoniumEffect, float material)
        {harmoniumEffect->Setter<HarmoniumEffectMaterialFactorTag>(material);}, materialVec);
}

GRAPHIC_TEST(NGShaderHarmoniumEffectTest, EFFECT_TEST, Set_NG_Shader_Harmonium_Effect_Tintcolor_Test)
{
    auto effectNode = SetUpEffectNode();
    SetUpHarmoniumEffect<Vector4f>(effectNode, [](std::shared_ptr<RSNGHarmoniumEffect>& harmoniumEffect,
        Vector4f tintcolor) {harmoniumEffect->Setter<HarmoniumEffectTintColorTag>(tintcolor);}, tintColorVec);
}

GRAPHIC_TEST(NGShaderHarmoniumEffectTest, EFFECT_TEST, Set_NG_Shader_Harmonium_Effect_Rate_Test)
{
    auto effectNode = SetUpEffectNode();
    SetUpHarmoniumEffect<float>(effectNode, [](std::shared_ptr<RSNGHarmoniumEffect>& harmoniumEffect, float rate)
        {harmoniumEffect->Setter<HarmoniumEffectRateTag>(rate);}, rateVec);
}

GRAPHIC_TEST(NGShaderHarmoniumEffectTest, EFFECT_TEST, Set_NG_Shader_Harmonium_Effect_Light_Up_Degree_Test)
{
    auto effectNode = SetUpEffectNode();
    SetUpHarmoniumEffect<float>(effectNode, [](std::shared_ptr<RSNGHarmoniumEffect>& harmoniumEffect,
        float lightUpDegree) {harmoniumEffect->Setter<HarmoniumEffectLightUpDegreeTag>(lightUpDegree);},
        lightUpDegreeVec);
}

GRAPHIC_TEST(NGShaderHarmoniumEffectTest, EFFECT_TEST, Set_NG_Shader_Harmonium_Effect_Cubic_Coeff_Test)
{
    auto effectNode = SetUpEffectNode();
    SetUpHarmoniumEffect<float>(effectNode, [](std::shared_ptr<RSNGHarmoniumEffect>& harmoniumEffect, float cubicCoeff)
        {harmoniumEffect->Setter<HarmoniumEffectCubicCoeffTag>(cubicCoeff);}, cubicCoeffVec);
}

GRAPHIC_TEST(NGShaderHarmoniumEffectTest, EFFECT_TEST, Set_NG_Shader_Harmonium_Effect_Quad_Coeff_Test)
{
    auto effectNode = SetUpEffectNode();
    SetUpHarmoniumEffect<float>(effectNode, [](std::shared_ptr<RSNGHarmoniumEffect>& harmoniumEffect, float quadCoeff)
        {harmoniumEffect->Setter<HarmoniumEffectQuadCoeffTag>(quadCoeff);}, quadCoeffVec);
}

GRAPHIC_TEST(NGShaderHarmoniumEffectTest, EFFECT_TEST, Set_NG_Shader_Harmonium_Effect_Saturation_Test)
{
    auto effectNode = SetUpEffectNode();
    SetUpHarmoniumEffect<float>(effectNode, [](std::shared_ptr<RSNGHarmoniumEffect>& harmoniumEffect, float saturation)
        {harmoniumEffect->Setter<HarmoniumEffectSaturationTag>(saturation);}, saturationVec);
}

GRAPHIC_TEST(NGShaderHarmoniumEffectTest, EFFECT_TEST, Set_NG_Shader_Harmonium_Effect_Posrgb_Test)
{
    auto effectNode = SetUpEffectNode();
    SetUpHarmoniumEffect<Vector3f>(effectNode, [](std::shared_ptr<RSNGHarmoniumEffect>& harmoniumEffect,
        Vector3f posRgb) {harmoniumEffect->Setter<HarmoniumEffectPosRGBTag>(posRgb);}, posRgbVec);
}

GRAPHIC_TEST(NGShaderHarmoniumEffectTest, EFFECT_TEST, Set_NG_Shader_Harmonium_Effect_Negrgb_Test)
{
    auto effectNode = SetUpEffectNode();
    SetUpHarmoniumEffect<Vector3f>(effectNode, [](std::shared_ptr<RSNGHarmoniumEffect>& harmoniumEffect,
        Vector3f negRgb) {harmoniumEffect->Setter<HarmoniumEffectNegRGBTag>(negRgb);}, negRgbVec);
}

GRAPHIC_TEST(NGShaderHarmoniumEffectTest, EFFECT_TEST, Set_NG_Shader_Harmonium_Effect_Fraction_Test)
{
    auto effectNode = SetUpEffectNode();
    SetUpHarmoniumEffect<float>(effectNode, [](std::shared_ptr<RSNGHarmoniumEffect>& harmoniumEffect, float fraction)
        {harmoniumEffect->Setter<HarmoniumEffectFractionTag>(fraction);}, fractionVec);
}

GRAPHIC_TEST(NGShaderHarmoniumEffectTest, EFFECT_TEST, Set_NG_Shader_Harmonium_Effect_Distort_Progress_Test)
{
    auto effectNode = SetUpEffectNode();
    SetUpHarmoniumEffect<float>(effectNode, SetDistortProgress, distortProgressVec);
}

GRAPHIC_TEST(NGShaderHarmoniumEffectTest, EFFECT_TEST, Set_NG_Shader_Harmonium_Effect_Distort_Factor_Test)
{
    auto effectNode = SetUpEffectNode();
    SetUpHarmoniumEffect<float>(effectNode, SetDistortFactor, distortFactorVec);
}

GRAPHIC_TEST(NGShaderHarmoniumEffectTest, EFFECT_TEST, Set_NG_Shader_Harmonium_Effect_Ripple_Progress_Test)
{
    auto effectNode = SetUpEffectNode();
    SetUpHarmoniumEffect<float>(effectNode, SetRippleProgress, rippleProgressVec);
}

GRAPHIC_TEST(NGShaderHarmoniumEffectTest, EFFECT_TEST, Set_NG_Shader_Harmonium_Effect_Ripple_Position_Test)
{
    auto effectNode = SetUpEffectNode();
    SetUpHarmoniumEffect<std::vector<Vector2f>>(effectNode, SetRipplePosition, ripplePositionVec);
}

}
