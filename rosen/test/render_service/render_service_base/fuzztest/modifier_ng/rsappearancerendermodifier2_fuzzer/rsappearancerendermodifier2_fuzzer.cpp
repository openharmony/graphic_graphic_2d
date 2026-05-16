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

#include "rsappearancerendermodifier2_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>

#include "pipeline/rs_render_node.h"
#include "modifier_ng/appearance/rs_mask_render_modifier.h"
#include "modifier_ng/appearance/rs_outline_render_modifier.h"
#include "modifier_ng/appearance/rs_particle_effect_render_modifier.h"
#include "modifier_ng/appearance/rs_pixel_stretch_render_modifier.h"
#include "modifier_ng/appearance/rs_shadow_render_modifier.h"
#include "modifier_ng/appearance/rs_use_effect_render_modifier.h"
#include "modifier_ng/appearance/rs_visibility_render_modifier.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint8_t DO_MASK = 0;
constexpr uint8_t DO_OUTLINE = 1;
constexpr uint8_t DO_PARTICLE_EFFECT = 2;
constexpr uint8_t DO_PIXEL_STRETCH = 3;
constexpr uint8_t DO_SHADOW = 4;
constexpr uint8_t DO_USE_EFFECT = 5;
constexpr uint8_t DO_VISIBILITY = 6;
constexpr uint8_t TARGET_SIZE = 7;
} // namespace

void DoMask(FuzzedDataProvider& fdp)
{
    auto modifier = std::make_shared<ModifierNG::RSMaskRenderModifier>();
    RSProperties properties;
    double xyScale = fdp.ConsumeFloatingPoint<double>();
    auto mask = RSMask::CreateSVGMask(xyScale, xyScale, xyScale, xyScale, nullptr);
    properties.SetMask(mask);
    modifier->ResetProperties(properties);
}

void DoOutline(FuzzedDataProvider& fdp)
{
    auto modifier = std::make_shared<ModifierNG::RSOutlineRenderModifier>();
    RSProperties properties;
    properties.ResetBorder(fdp.ConsumeBool());
    modifier->ResetProperties(properties);
}

void DoParticleEffect(FuzzedDataProvider& fdp)
{
    auto modifier = std::make_shared<ModifierNG::RSParticleEffectRenderModifier>();
    RSProperties properties;
    properties.SetRotation(fdp.ConsumeFloatingPoint<float>());
    modifier->ResetProperties(properties);
}

void DoPixelStretch(FuzzedDataProvider& fdp)
{
    auto modifier = std::make_shared<ModifierNG::RSPixelStretchRenderModifier>();
    RSProperties properties;
    properties.SetPixelStretchTileMode(fdp.ConsumeIntegral<int>());
    modifier->ResetProperties(properties);
}

void DoShadow(FuzzedDataProvider& fdp)
{
    (void)fdp;
    auto modifier = std::make_shared<ModifierNG::RSShadowRenderModifier>();
    RSProperties properties;
    modifier->ResetProperties(properties);
}

void DoUseEffect(FuzzedDataProvider& fdp)
{
    auto modifier = std::make_shared<ModifierNG::RSUseEffectRenderModifier>();
    RSProperties properties;
    properties.SetUseEffect(fdp.ConsumeBool());
    properties.SetUseEffectType(fdp.ConsumeIntegral<int>());
    modifier->ResetProperties(properties);
}

void DoVisibility(FuzzedDataProvider& fdp)
{
    auto modifier = std::make_shared<ModifierNG::RSVisibilityRenderModifier>();
    RSProperties properties;
    properties.SetVisible(fdp.ConsumeBool());
    modifier->ResetProperties(properties);
}

void FuzzBody(FuzzedDataProvider& fdp)
{
    uint8_t tarPos = fdp.ConsumeIntegral<uint8_t>() % TARGET_SIZE;
    switch (tarPos) {
        case DO_MASK:
            DoMask(fdp);
            break;
        case DO_OUTLINE:
            DoOutline(fdp);
            break;
        case DO_PARTICLE_EFFECT:
            DoParticleEffect(fdp);
            break;
        case DO_PIXEL_STRETCH:
            DoPixelStretch(fdp);
            break;
        case DO_SHADOW:
            DoShadow(fdp);
            break;
        case DO_USE_EFFECT:
            DoUseEffect(fdp);
            break;
        case DO_VISIBILITY:
            DoVisibility(fdp);
            break;
        default:
            break;
    }
}
} // namespace Rosen
} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (data == nullptr || size == 0) {
        return -1;
    }
    FuzzedDataProvider fdp(data, size);
    OHOS::Rosen::FuzzBody(fdp);
    return 0;
}
