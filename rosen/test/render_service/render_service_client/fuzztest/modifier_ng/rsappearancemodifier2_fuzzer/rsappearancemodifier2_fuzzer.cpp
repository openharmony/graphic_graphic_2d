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

#include "rsappearancemodifier2_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>

#include "modifier_ng/appearance/rs_mask_modifier.h"
#include "modifier_ng/appearance/rs_outline_modifier.h"
#include "modifier_ng/appearance/rs_particle_effect_modifier.h"
#include "modifier_ng/appearance/rs_pixel_stretch_modifier.h"
#include "modifier_ng/appearance/rs_shadow_modifier.h"
#include "modifier_ng/appearance/rs_use_effect_modifier.h"
#include "modifier_ng/appearance/rs_visibility_modifier.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t USE_EFFECT_TYPE_MAX = 3;
const uint8_t DO_MASK = 0;
const uint8_t DO_OUTLINE = 1;
const uint8_t DO_PARTICLE_EFFECT = 2;
const uint8_t DO_PIXEL_STRETCH = 3;
const uint8_t DO_SHADOW = 4;
const uint8_t DO_USE_EFFECT = 5;
const uint8_t DO_VISIBILITY = 6;
const uint8_t TARGET_SIZE = 7;
}

void DoMaskModifier(FuzzedDataProvider& fdp)
{
    auto modifier = std::make_shared<ModifierNG::RSMaskModifier>();
    double xyScale = fdp.ConsumeFloatingPoint<double>();
    auto mask = RSMask::CreateSVGMask(xyScale, xyScale, xyScale, xyScale, nullptr);
    modifier->SetMask(mask);
}

void DoOutlineModifier(FuzzedDataProvider& fdp)
{
    auto modifier = std::make_shared<ModifierNG::RSOutlineModifier>();
    Color color{fdp.ConsumeIntegral<uint32_t>(), fdp.ConsumeIntegral<uint32_t>(),
        fdp.ConsumeIntegral<uint32_t>(), fdp.ConsumeIntegral<uint32_t>()};
    Vector4<Color> outlineColor{color, color, color, color};
    modifier->SetOutlineColor(outlineColor);
    modifier->GetOutlineColor();
    Vector4f randomVec4F{fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeFloatingPoint<float>(),
        fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeFloatingPoint<float>()};
    modifier->SetOutlineWidth(randomVec4F);
    modifier->GetOutlineWidth();
    uint32_t style = fdp.ConsumeIntegral<uint32_t>();
    Vector4<uint32_t> outlineStyle{style, style, style, style};
    modifier->SetOutlineStyle(outlineStyle);
    modifier->GetOutlineStyle();
    modifier->SetOutlineDashWidth(randomVec4F);
    modifier->GetOutlineDashWidth();
    modifier->SetOutlineDashGap(randomVec4F);
    modifier->GetOutlineDashGap();
    modifier->SetOutlineRadius(randomVec4F);
    modifier->GetOutlineRadius();
}

void DoParticleEffectModifier(FuzzedDataProvider& fdp)
{
    auto modifier = std::make_shared<ModifierNG::RSParticleEffectModifier>();
    uint32_t emitterIndex = fdp.ConsumeIntegral<uint32_t>();
    std::optional<Vector2f> position = std::make_optional<Vector2f>();
    std::optional<Vector2f> emitSize = std::make_optional<Vector2f>();
    std::optional<int> emitRate = std::make_optional<int>(fdp.ConsumeIntegral<int>());
    std::vector<std::shared_ptr<EmitterUpdater>> para = {
        std::make_shared<EmitterUpdater>(emitterIndex, position, emitSize, emitRate)};
    modifier->SetEmitterUpdater(para);
    modifier->GetEmitterUpdater();
    auto param = std::make_shared<ParticleNoiseFields>();
    modifier->SetParticleNoiseFields(param);
    modifier->GetParticleNoiseFields();
}

void DoPixelStretchModifier(FuzzedDataProvider& fdp)
{
    auto modifier = std::make_shared<ModifierNG::RSPixelStretchModifier>();
    Vector4f randomVec4F(fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeFloatingPoint<float>(),
        fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeFloatingPoint<float>());
    modifier->SetPixelStretchSize(randomVec4F);
    modifier->GetPixelStretchSize();
    modifier->SetPixelStretchTileMode(fdp.ConsumeIntegral<int>());
    modifier->GetPixelStretchTileMode();
    modifier->SetPixelStretchPercent(randomVec4F);
    modifier->GetPixelStretchPercent();
}

void DoShadowModifier(FuzzedDataProvider& fdp)
{
    auto modifier = std::make_shared<ModifierNG::RSShadowModifier>();
    Color color{fdp.ConsumeIntegral<uint32_t>(), fdp.ConsumeIntegral<uint32_t>(),
        fdp.ConsumeIntegral<uint32_t>(), fdp.ConsumeIntegral<uint32_t>()};
    modifier->SetShadowColor(color);
    modifier->GetShadowColor();
    float randomFloat = fdp.ConsumeFloatingPoint<float>();
    modifier->SetShadowOffsetX(randomFloat);
    modifier->GetShadowOffsetX();
    modifier->SetShadowOffsetY(randomFloat);
    modifier->GetShadowOffsetY();
    modifier->SetShadowAlpha(randomFloat);
    modifier->GetShadowAlpha();
    modifier->SetShadowElevation(randomFloat);
    modifier->GetShadowElevation();
    modifier->SetShadowRadius(randomFloat);
    modifier->GetShadowRadius();
    auto path = std::make_shared<RSPath>();
    modifier->SetShadowPath(path);
    modifier->GetShadowPath();
    bool randomBool = fdp.ConsumeBool();
    modifier->SetShadowMask(randomBool);
    modifier->GetShadowMask();
    modifier->SetShadowColorStrategy(fdp.ConsumeIntegral<int>());
    modifier->GetShadowColorStrategy();
    modifier->SetShadowIsFilled(randomBool);
    modifier->GetShadowIsFilled();
    modifier->SetShadowDisableSDFBlur(randomBool);
    modifier->GetShadowDisableSDFBlur();
    modifier->SetUseShadowBatching(randomBool);
    modifier->GetUseShadowBatching();
}

void DoUseEffectModifier(FuzzedDataProvider& fdp)
{
    auto modifier = std::make_shared<ModifierNG::RSUseEffectModifier>();
    bool randomBool = fdp.ConsumeBool();
    modifier->SetUseEffect(randomBool);
    modifier->GetUseEffect();
    UseEffectType type = static_cast<UseEffectType>(fdp.ConsumeIntegral<uint32_t>() % USE_EFFECT_TYPE_MAX);
    modifier->SetUseEffectType(type);
    modifier->GetUseEffectType();
}

void DoVisibilityModifier(FuzzedDataProvider& fdp)
{
    auto modifier = std::make_shared<ModifierNG::RSVisibilityModifier>();
    modifier->SetVisible(fdp.ConsumeBool());
    modifier->GetVisible();
}

} // namespace Rosen
} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return -1;
    }

    FuzzedDataProvider fdp(data, size);
    uint8_t tarPos = fdp.ConsumeIntegral<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    switch (tarPos) {
        case OHOS::Rosen::DO_MASK:
            OHOS::Rosen::DoMaskModifier(fdp);
            break;
        case OHOS::Rosen::DO_OUTLINE:
            OHOS::Rosen::DoOutlineModifier(fdp);
            break;
        case OHOS::Rosen::DO_PARTICLE_EFFECT:
            OHOS::Rosen::DoParticleEffectModifier(fdp);
            break;
        case OHOS::Rosen::DO_PIXEL_STRETCH:
            OHOS::Rosen::DoPixelStretchModifier(fdp);
            break;
        case OHOS::Rosen::DO_SHADOW:
            OHOS::Rosen::DoShadowModifier(fdp);
            break;
        case OHOS::Rosen::DO_USE_EFFECT:
            OHOS::Rosen::DoUseEffectModifier(fdp);
            break;
        case OHOS::Rosen::DO_VISIBILITY:
            OHOS::Rosen::DoVisibilityModifier(fdp);
            break;
        default:
            break;
    }
    return 0;
}
