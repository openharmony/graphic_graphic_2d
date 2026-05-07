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

#include "rsappearancerendermodifier_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>

#include "pipeline/rs_render_node.h"
#include "modifier_ng/appearance/rs_alpha_render_modifier.h"
#include "modifier_ng/appearance/rs_background_filter_render_modifier.h"
#include "modifier_ng/appearance/rs_blend_render_modifier.h"
#include "modifier_ng/appearance/rs_border_render_modifier.h"
#include "modifier_ng/appearance/rs_compositing_filter_render_modifier.h"
#include "modifier_ng/appearance/rs_dynamic_light_up_render_modifier.h"
#include "modifier_ng/appearance/rs_foreground_filter_render_modifier.h"
#include "modifier_ng/appearance/rs_hdr_brightness_render_modifier.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint8_t DO_ALPHA = 0;
constexpr uint8_t DO_BACKGROUND_FILTER = 1;
constexpr uint8_t DO_BLEND = 2;
constexpr uint8_t DO_BORDER = 3;
constexpr uint8_t DO_COMPOSITING_FILTER = 4;
constexpr uint8_t DO_DYNAMIC_LIGHT_UP = 5;
constexpr uint8_t DO_FOREGROUND_FILTER = 6;
constexpr uint8_t DO_HDR_BRIGHTNESS = 7;
constexpr uint8_t TARGET_SIZE = 8;
} // namespace

void DoAlpha(FuzzedDataProvider& fdp)
{
    ModifierId id = fdp.ConsumeIntegral<uint64_t>();
    auto modifier = std::make_shared<ModifierNG::RSAlphaRenderModifier>(id);
    RSProperties properties;
    properties.SetAlpha(fdp.ConsumeFloatingPoint<float>());
    properties.SetAlphaOffscreen(fdp.ConsumeBool());
    modifier->ResetProperties(properties);
}

void DoBackgroundFilter(FuzzedDataProvider& fdp)
{
    auto modifier = std::make_shared<ModifierNG::RSBackgroundFilterRenderModifier>();
    RSProperties properties;
    properties.SetSystemBarEffect(fdp.ConsumeBool());
    properties.SetWaterRippleProgress(fdp.ConsumeFloatingPoint<float>());
    modifier->ResetProperties(properties);
}

void DoBlend(FuzzedDataProvider& fdp)
{
    auto modifier = std::make_shared<ModifierNG::RSBlendRenderModifier>();
    RSProperties properties;
    properties.SetFgBrightnessSaturation(fdp.ConsumeFloatingPoint<float>());
    modifier->ResetProperties(properties);
}

void DoBorder(FuzzedDataProvider& fdp)
{
    auto modifier = std::make_shared<ModifierNG::RSBorderRenderModifier>();
    RSProperties properties;
    properties.ResetBorder(fdp.ConsumeBool());
    modifier->ResetProperties(properties);
}

void DoCompositingFilter(FuzzedDataProvider& fdp)
{
    auto modifier = std::make_shared<ModifierNG::RSCompositingFilterRenderModifier>();
    RSProperties properties;
    properties.SetLightUpEffect(fdp.ConsumeFloatingPoint<float>());
    modifier->ResetProperties(properties);
}

void DoDynamicLightUp(FuzzedDataProvider& fdp)
{
    auto modifier = std::make_shared<ModifierNG::RSDynamicLightUpRenderModifier>();
    RSProperties properties;
    std::optional<float> tmp = std::make_optional(fdp.ConsumeFloatingPoint<float>());
    properties.SetDynamicLightUpRate(tmp);
    modifier->ResetProperties(properties);
}

void DoForegroundFilter(FuzzedDataProvider& fdp)
{
    auto modifier = std::make_shared<ModifierNG::RSForegroundFilterRenderModifier>();
    RSProperties properties;
    properties.SetSpherize(fdp.ConsumeFloatingPoint<float>());
    modifier->ResetProperties(properties);
}

void DoHDRBrightness(FuzzedDataProvider& fdp)
{
    auto modifier = std::make_shared<ModifierNG::RSHDRBrightnessRenderModifier>();
    RSProperties properties;
    properties.SetHDRUIBrightness(fdp.ConsumeFloatingPoint<float>());
    properties.SetHDRBrightnessFactor(fdp.ConsumeFloatingPoint<float>());
    properties.SetHDRColorHeadroom(fdp.ConsumeFloatingPoint<float>());
    modifier->ResetProperties(properties);
}

void FuzzBody(FuzzedDataProvider& fdp)
{
    uint8_t tarPos = fdp.ConsumeIntegral<uint8_t>() % TARGET_SIZE;
    switch (tarPos) {
        case DO_ALPHA:
            DoAlpha(fdp);
            break;
        case DO_BACKGROUND_FILTER:
            DoBackgroundFilter(fdp);
            break;
        case DO_BLEND:
            DoBlend(fdp);
            break;
        case DO_BORDER:
            DoBorder(fdp);
            break;
        case DO_COMPOSITING_FILTER:
            DoCompositingFilter(fdp);
            break;
        case DO_DYNAMIC_LIGHT_UP:
            DoDynamicLightUp(fdp);
            break;
        case DO_FOREGROUND_FILTER:
            DoForegroundFilter(fdp);
            break;
        case DO_HDR_BRIGHTNESS:
            DoHDRBrightness(fdp);
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
