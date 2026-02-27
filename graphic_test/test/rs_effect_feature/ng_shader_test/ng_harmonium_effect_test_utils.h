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

#ifndef NG_HARMONIUM_EFFECT_TEST_UTILS_H
#define NG_HARMONIUM_EFFECT_TEST_UTILS_H

namespace OHOS::Rosen {
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 2000;
const float BLUR_RADIUS = 10.f;

const std::string BACKGROUND_IMAGE_PATH = "/data/local/tmp/Images/backGroundImage.jpg";
const std::string DISTOR_IMAGE_PATH = "/data/local/tmp/Images/perlin_worley_noise_3d_64.png";
const std::string CLOCK_IMAGE_PATH = "/data/local/tmp/Images/clockEffect.png";

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

void InitHarmoniumEffect(std::shared_ptr<RSNGHarmoniumEffect>& harmoniumEffect)
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
};

std::shared_ptr<RSCanvasNode> CreateEffectChildNode(const int i, const int columnCount, const int rowCount,
    std::shared_ptr<RSEffectNode>& effectNode, std::shared_ptr<RSNGHarmoniumEffect>& harmoniumEffect)
{
    auto sizeX = (columnCount != 0) ? (SCREEN_WIDTH / columnCount) : SCREEN_WIDTH;
    auto sizeY = (rowCount != 0) ? (SCREEN_HEIGHT * columnCount / rowCount) : SCREEN_HEIGHT;

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
};

}  // namespace OHOS::Rosen
#endif // NG_HARMONIUM_EFFECT_TEST_UTILS_H
