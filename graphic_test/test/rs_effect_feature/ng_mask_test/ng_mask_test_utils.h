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

#ifndef NG_MASK_TEST_UTILS_H
#define NG_MASK_TEST_UTILS_H

#include <memory>
#include <string>
#include <vector>
#include "ui_effect/property/include/rs_ui_shader_base.h"

namespace OHOS::Rosen {
class RSCanvasNode;
class RSEffectNode;

// Constants for Harmonium effect tests (prefixed to avoid conflicts with test file constants)
const int HARMONIUM_SCREEN_WIDTH = 1200;
const int HARMONIUM_SCREEN_HEIGHT = 2000;
const float HARMONIUM_BLUR_RADIUS = 10.f;

const std::string HARMONIUM_BACKGROUND_IMAGE_PATH = "/data/local/tmp/Images/backGroundImage.jpg";
const std::string HARMONIUM_DISTOR_IMAGE_PATH = "/data/local/tmp/Images/perlin_worley_noise_3d_64.png";
const std::string HARMONIUM_CLOCK_IMAGE_PATH = "/data/local/tmp/Images/clockEffect.png";

const std::vector<bool> useEffectVec = {true, false, false, true};
const std::vector<float> refractionVec = {-1.f, 1.f, 10.f, 100.f};
const std::vector<float> reflectionVec = {-1.f, 1.f, 10.f, 100.f};
const std::vector<float> materialVec = {-1.f, 0.f, 1.f, 10.f};
const std::vector<Vector4f> tintColorVec = {
    Vector4f{1.f, -1.f, -1.f, 1.f},
    Vector4f{10.f, 1.f, -1.f, 1.f},
    Vector4f{0.2f, -1.f, 10.f, 1.f},
    Vector4f{1.f, 1.f, 1.f, 1.f}
};
const std::vector<float> rateVec = {-2.f, -1.f, 1.f, 2.f};
const std::vector<float> lightUpDegreeVec = {-2.f, -1.f, 1.f, 2.f};
const std::vector<float> cubicCoeffVec = {-2.f, -1.f, 1.f, 2.f};
const std::vector<float> quadCoeffVec = {-2.f, -1.f, 1.f, 2.f};
const std::vector<float> saturationVec = {-2.f, -1.f, 1.f, 2.f};
const std::vector<float> fractionVec = {-2.f, -1.f, 1.f, 2.f};
const std::vector<Vector3f> posRgbVec = {
    Vector3f{1.f, -1.f, -1.f},
    Vector3f{10.f, 1.f, -1.f},
    Vector3f{0.2f, -1.f, 10.f},
    Vector3f{1.f, 1.f, 1.f}
};
const std::vector<Vector3f> negRgbVec = {
    Vector3f{1.f, -1.f, -1.f},
    Vector3f{10.f, 1.f, -1.f},
    Vector3f{0.2f, -1.f, 10.f},
    Vector3f{1.f, 1.f, 1.f}
};
const std::vector<float> rippleProgressVec = {0.5f, 2.5f, 4.5f, 6.5f};
const std::vector<std::vector<Vector2f>> ripplePositionVec = {
    {{-1.f, -1.f}, {0.f, 0.f}, {0.5f, 0.5f}, {0.5f, 0.f}, {0.5f, 0.75f}, {1.f, 0.f}},
    {{0.25f, 0.f}, {0.f, 0.f}, {0.25f, 0.25f}, {0.5f, 0.f}, {0.25f, 0.5f}, {1.f, 0.f}},
    {{0.f, 0.f}, {0.f, 0.f}, {0.5f, 0.25f}, {0.5f, 0.f}, {0.75f, 0.25f}, {1.f, 0.f}},
    {{0.25f, 0.25f}, {0.f, 0.f}, {1.f, 1.f}, {0.5f, 0.f}, {2.f, 2.f}, {1.f, 0.f}},
};
const std::vector<float> distortProgressVec = {0.f, 0.35f, 0.75f, 1.f};
const std::vector<float> distortFactorVec = {-1.f, 1.f, 10.f, 100.f};
const std::vector<float> cornerRadiusVec = {-100.f, 100.f, 1000.f, 10000.f};

inline void InitHarmoniumEffect(std::shared_ptr<RSNGHarmoniumEffect>& harmoniumEffect)
{
    auto useEffectMask = std::make_shared<RSNGUseEffectMask>();
    useEffectMask->Setter<UseEffectMaskUseEffectTag>(true);
    harmoniumEffect->Setter<HarmoniumEffectUseEffectMaskTag>(std::static_pointer_cast<RSNGMaskBase>(useEffectMask));
    harmoniumEffect->Setter<HarmoniumEffectRippleProgressTag>(0.f);
    harmoniumEffect->Setter<HarmoniumEffectTintColorTag>(Vector4f{1.f, 1.f, 1.0f, 1.0f});
    harmoniumEffect->Setter<HarmoniumEffectDistortProgressTag>(0.f);
    harmoniumEffect->Setter<HarmoniumEffectDistortFactorTag>(0.f);
    harmoniumEffect->Setter<HarmoniumEffectReflectionFactorTag>(1.f);
    harmoniumEffect->Setter<HarmoniumEffectRefractionFactorTag>(0.4f);
    harmoniumEffect->Setter<HarmoniumEffectMaterialFactorTag>(1.f);
    harmoniumEffect->Setter<HarmoniumEffectRateTag>(1.f);
    harmoniumEffect->Setter<HarmoniumEffectLightUpDegreeTag>(2.f);
    harmoniumEffect->Setter<HarmoniumEffectCubicCoeffTag>(0.f);
    harmoniumEffect->Setter<HarmoniumEffectQuadCoeffTag>(0.f);
    harmoniumEffect->Setter<HarmoniumEffectSaturationTag>(1.5f);
    harmoniumEffect->Setter<HarmoniumEffectPosRGBTag>(Vector3f{0.4f, 1.f, 0.2f});
    harmoniumEffect->Setter<HarmoniumEffectNegRGBTag>(Vector3f{1.f, 1.f, 0.5f});
    harmoniumEffect->Setter<HarmoniumEffectFractionTag>(1.f);
}

// 1. InitPixelMapMask - RSNGPixelMapMask (4 parameters)
inline void InitPixelMapMask(std::shared_ptr<RSNGPixelMapMask>& pixelMapMask)
{
    if (!pixelMapMask) {
        return;
    }
    // Src: source rectangle (left, top, right, bottom)
    pixelMapMask->Setter<PixelMapMaskSrcTag>(Vector4f{0.0f, 0.0f, 1.0f, 1.0f});
    // Dst: destination rectangle (left, top, right, bottom)
    pixelMapMask->Setter<PixelMapMaskDstTag>(Vector4f{0.0f, 0.0f, 1.0f, 1.0f});
    // FillColor: white opaque
    pixelMapMask->Setter<PixelMapMaskFillColorTag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
    // Image: PixelMap (non-animatable, typically set by test)
}

// 2. InitRadialGradientMask - RSNGRadialGradientMask (6 parameters)
inline void InitRadialGradientMask(std::shared_ptr<RSNGRadialGradientMask>& radialGradientMask)
{
    if (!radialGradientMask) {
        return;
    }
    // Center: center position
    radialGradientMask->Setter<RadialGradientMaskCenterTag>(Vector2f{0.5f, 0.5f});
    // RadiusX: horizontal radius
    radialGradientMask->Setter<RadialGradientMaskRadiusXTag>(0.5f);
    // RadiusY: vertical radius
    radialGradientMask->Setter<RadialGradientMaskRadiusYTag>(0.5f);
    // Colors: gradient colors (white to black)
    std::vector<float> colors = {1.0f, 0.5f, 0.0f};
    radialGradientMask->Setter<RadialGradientMaskColorsTag>(colors);
    // Positions: color stop positions
    std::vector<float> positions = {0.0f, 0.5f, 1.0f};
    radialGradientMask->Setter<RadialGradientMaskPositionsTag>(positions);
}

// 3. InitRippleMask - RSNGRippleMask (4 parameters)
inline void InitRippleMask(std::shared_ptr<RSNGRippleMask>& rippleMask)
{
    if (!rippleMask) {
        return;
    }
    // Radius: ripple radius
    rippleMask->Setter<RippleMaskRadiusTag>(5.0f);
    // Width: ripple width
    rippleMask->Setter<RippleMaskWidthTag>(5.0f);
    // Center: center position
    rippleMask->Setter<RippleMaskCenterTag>(Vector2f{0.5f, 0.5f});
    // Offset: width center offset
    rippleMask->Setter<RippleMaskOffsetTag>(0.0f);
}

// 4. InitWaveGradientMask - RSNGWaveGradientMask (5 parameters)
inline void InitWaveGradientMask(std::shared_ptr<RSNGWaveGradientMask>& waveGradientMask)
{
    if (!waveGradientMask) {
        return;
    }
    // WaveCenter: wave center position
    waveGradientMask->Setter<WaveGradientMaskWaveCenterTag>(Vector2f{0.5f, 0.5f});
    // WaveWidth: wave width
    waveGradientMask->Setter<WaveGradientMaskWaveWidthTag>(2.5f);
    // PropagationRadius: propagation radius
    waveGradientMask->Setter<WaveGradientMaskPropagationRadiusTag>(5.0f);
    // BlurRadius: blur radius
    waveGradientMask->Setter<WaveGradientMaskBlurRadiusTag>(2.5f);
    // TurbulenceStrength: turbulence strength
    waveGradientMask->Setter<WaveGradientMaskTurbulenceStrengthTag>(0.5f);
}

inline std::shared_ptr<RSCanvasNode> CreateEffectChildNode(const int i, const int columnCount, const int rowCount,
    std::shared_ptr<RSEffectNode>& effectNode, std::shared_ptr<RSNGHarmoniumEffect>& harmoniumEffect)
{
    auto sizeX = (columnCount != 0) ? (HARMONIUM_SCREEN_WIDTH / columnCount) : HARMONIUM_SCREEN_WIDTH;
    auto sizeY = (rowCount != 0) ? (HARMONIUM_SCREEN_HEIGHT * columnCount / rowCount) : HARMONIUM_SCREEN_HEIGHT;

    int x = (columnCount != 0) ? (i % columnCount) * sizeX : 0;
    int y = (columnCount != 0) ? (i / columnCount) * sizeY : 0;

    // set effect child node
    auto effectChildNode = RSCanvasNode::Create();
    effectChildNode->SetBounds(x, y, sizeX, sizeY);
    effectChildNode->SetFrame(x, y, sizeX, sizeY);
    effectChildNode->SetCornerRadius(cornerRadiusVec[i]);
    effectChildNode->SetBackgroundNGShader(harmoniumEffect);
    effectNode->AddChild(effectChildNode);
    return effectChildNode;
}

}  // namespace OHOS::Rosen
#endif // NG_MASK_TEST_UTILS_H
