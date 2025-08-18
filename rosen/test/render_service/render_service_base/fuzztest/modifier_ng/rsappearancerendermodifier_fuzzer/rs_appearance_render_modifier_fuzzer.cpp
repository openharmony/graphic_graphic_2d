/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "rs_appearance_render_modifier_fuzzer.h"

#include "modifier_ng/appearance/rs_alpha_render_modifier.h"
#include "modifier_ng/appearance/rs_background_filter_render_modifier.h"
#include "modifier_ng/appearance/rs_behind_window_filter_render_modifier.h"
#include "modifier_ng/appearance/rs_blend_render_modifier.h"
#include "modifier_ng/appearance/rs_border_render_modifier.h"
#include "modifier_ng/appearance/rs_compositing_filter_render_modifier.h"
#include "modifier_ng/appearance/rs_dynamic_light_up_render_modifier.h"
#include "modifier_ng/appearance/rs_foreground_filter_render_modifier.h"
#include "modifier_ng/appearance/rs_hdr_brightness_render_modifier.h"
#include "modifier_ng/appearance/rs_mask_render_modifier.h"
#include "modifier_ng/appearance/rs_outline_render_modifier.h"
#include "modifier_ng/appearance/rs_particle_effect_render_modifier.h"
#include "modifier_ng/appearance/rs_pixel_stretch_render_modifier.h"
#include "modifier_ng/appearance/rs_point_light_render_modifier.h"
#include "modifier_ng/appearance/rs_shadow_render_modifier.h"
#include "modifier_ng/appearance/rs_use_effect_render_modifier.h"
#include "modifier_ng/appearance/rs_visibility_render_modifier.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos;
} // namespace

/*
 * describe: get data from outside untrusted data(g_data) which size is according to sizeof(T)
 * tips: only support basic type
 */
template<class T>
T GetData()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (g_data == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, g_data + g_pos, objectSize);
    if (ret != EOK) {
        return {};
    }
    g_pos += objectSize;
    return object;
}

bool RSAlphaRenderModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // test
    ModifierId id = GetData<ModifierId>();
    auto modifier = std::make_shared<ModifierNG::RSAlphaRenderModifier>(id);
    RSProperties properties;
    properties.SetAlpha(GetData<float>());
    properties.SetAlphaOffscreen(GetData<bool>());
    modifier->ResetProperties(properties);
    return true;
}

bool RSBackgroundFilterRenderModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // test
    auto modifier = std::make_shared<ModifierNG::RSBackgroundFilterRenderModifier>();
    RSProperties properties;
    properties.SetSystemBarEffect(GetData<bool>());
    properties.SetWaterRippleProgress(GetData<float>());
    modifier->ResetProperties(properties);
    return true;
}

bool RSBlendRenderModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // test
    auto modifier = std::make_shared<ModifierNG::RSBlendRenderModifier>();
    RSProperties properties;
    properties.SetFgBrightnessSaturation(GetData<float>());
    modifier->ResetProperties(properties);

    return true;
}

bool RSBorderRenderModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // test
    auto modifier = std::make_shared<ModifierNG::RSBorderRenderModifier>();
    RSProperties properties;
    properties.ResetBorder(GetData<bool>());
    modifier->ResetProperties(properties);
    return true;
}

bool RSCompositingFilterRenderModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // test
    auto modifier = std::make_shared<ModifierNG::RSCompositingFilterRenderModifier>();
    RSProperties properties;
    properties.SetLightUpEffect(GetData<float>());
    modifier->ResetProperties(properties);
    return true;
}

bool RSDynamicLightUpRenderModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // test
    auto modifier = std::make_shared<ModifierNG::RSDynamicLightUpRenderModifier>();
    RSProperties properties;
    std::optional<float> tmp = std::make_optional(GetData<float>());
    properties.SetDynamicLightUpRate(tmp);
    modifier->ResetProperties(properties);

    return true;
}

bool RSForegroundFilterRenderModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // test
    auto modifier = std::make_shared<ModifierNG::RSForegroundFilterRenderModifier>();
    RSProperties properties;
    properties.SetSpherize(GetData<float>());
    modifier->ResetProperties(properties);

    return true;
}

bool RSHDRBrightnessRenderModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // test
    auto modifier = std::make_shared<ModifierNG::RSHDRBrightnessRenderModifier>();
    RSProperties properties;
    properties.SetHDRUIBrightness(GetData<float>());
    properties.SetHDRBrightnessFactor(GetData<float>());
    modifier->ResetProperties(properties);

    return true;
}

bool RSMaskRenderModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // test
    auto modifier = std::make_shared<ModifierNG::RSMaskRenderModifier>();
    RSProperties properties;
    double xyScale = GetData<double>();
    auto mask = RSMask::CreateSVGMask(xyScale, xyScale, xyScale, xyScale, nullptr);
    properties.SetMask(mask);
    modifier->ResetProperties(properties);

    return true;
}

bool RSOutlineRenderModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // test
    auto modifier = std::make_shared<ModifierNG::RSOutlineRenderModifier>();
    RSProperties properties;
    properties.ResetBorder(GetData<bool>());
    modifier->ResetProperties(properties);

    return true;
}

bool RSParticleEffectRenderModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // test
    auto modifier = std::make_shared<ModifierNG::RSParticleEffectRenderModifier>();
    RSProperties properties;
    properties.SetRotation(GetData<float>());
    modifier->ResetProperties(properties);

    return true;
}

bool RSPixelStretchRenderModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // test
    auto modifier = std::make_shared<ModifierNG::RSPixelStretchRenderModifier>();
    RSProperties properties;
    properties.SetPixelStretchTileMode(GetData<int>());
    modifier->ResetProperties(properties);
    return true;
}

bool RSPointLightRenderModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // test
    auto modifier = std::make_shared<ModifierNG::RSPointLightRenderModifier>();
    RSProperties properties;
    properties.SetLightIntensity(GetData<float>());
    modifier->ResetProperties(properties);
    return true;
}

bool RSShadowRenderModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // test
    auto modifier = std::make_shared<ModifierNG::RSShadowRenderModifier>();
    RSProperties properties;
    properties.SetShadowAlpha(GetData<float>());
    modifier->ResetProperties(properties);
    return true;
}

bool RSUseEffectRenderModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // test
    auto modifier = std::make_shared<ModifierNG::RSUseEffectRenderModifier>();
    RSProperties properties;
    properties.SetUseEffect(GetData<bool>());
    properties.SetUseEffectType(GetData<int>());
    modifier->ResetProperties(properties);

    return true;
}

bool RSVisibilityRenderModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // test
    auto modifier = std::make_shared<ModifierNG::RSVisibilityRenderModifier>();
    RSProperties properties;
    properties.SetVisible(GetData<bool>());
    modifier->ResetProperties(properties);

    return true;
}

} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    // initialize
    OHOS::Rosen::g_data = data;
    OHOS::Rosen::g_size = size;
    OHOS::Rosen::g_pos = 0;

    /* Run your code on data */
    OHOS::Rosen::RSAlphaRenderModifierFuzzTest(data, size);
    OHOS::Rosen::RSBackgroundFilterRenderModifierFuzzTest(data, size);
    OHOS::Rosen::RSBlendRenderModifierFuzzTest(data, size);
    OHOS::Rosen::RSBorderRenderModifierFuzzTest(data, size);
    OHOS::Rosen::RSCompositingFilterRenderModifierFuzzTest(data, size);
    OHOS::Rosen::RSDynamicLightUpRenderModifierFuzzTest(data, size);
    OHOS::Rosen::RSForegroundFilterRenderModifierFuzzTest(data, size);
    OHOS::Rosen::RSHDRBrightnessRenderModifierFuzzTest(data, size);
    OHOS::Rosen::RSMaskRenderModifierFuzzTest(data, size);
    OHOS::Rosen::RSOutlineRenderModifierFuzzTest(data, size);
    OHOS::Rosen::RSParticleEffectRenderModifierFuzzTest(data, size);
    OHOS::Rosen::RSPixelStretchRenderModifierFuzzTest(data, size);
    OHOS::Rosen::RSPointLightRenderModifierFuzzTest(data, size);
    OHOS::Rosen::RSShadowRenderModifierFuzzTest(data, size);
    OHOS::Rosen::RSUseEffectRenderModifierFuzzTest(data, size);
    OHOS::Rosen::RSVisibilityRenderModifierFuzzTest(data, size);
    return 0;
}