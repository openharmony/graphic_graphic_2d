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

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    ModifierId id = GetData<ModifierId>();
    std::shared_ptr<ModifierNG::RSAlphaRenderModifier> modifier =
        std::make_shared<ModifierNG::RSAlphaRenderModifier>(id);
    modifier->GetType();
    RSProperties properties;
    modifier->ResetProperties(properties);
    modifier->GetLegacyPropertyApplierMap();
    return true;
}   

bool RSBackgroundFilterRenderModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<ModifierNG::RSBackgroundFilterRenderModifier> modifier =
        std::make_shared<ModifierNG::RSBackgroundFilterRenderModifier>();
    modifier->GetType();
    RSProperties properties;
    modifier->ResetProperties(properties);
    modifier->GetLegacyPropertyApplierMap();
    return true;
}

bool RSBehindWindowFilterRenderModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<ModifierNG::RSBehindWindowFilterRenderModifier> modifier =
        std::make_shared<ModifierNG::RSBehindWindowFilterRenderModifier>();
    modifier->GetType();
    modifier->OnSetDirty();
    return true;
}

bool RSBlendRenderModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<ModifierNG::RSBlendRenderModifier> modifier =
        std::make_shared<ModifierNG::RSBlendRenderModifier>();
    modifier->GetType();
    RSProperties properties;
    modifier->ResetProperties(properties);
    modifier->GetLegacyPropertyApplierMap();
    return true;
}

bool RSBorderRenderModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<ModifierNG::RSBorderRenderModifier> modifier =
        std::make_shared<ModifierNG::RSBorderRenderModifier>();
    modifier->GetType();
    RSProperties properties;
    modifier->ResetProperties(properties);
    modifier->GetLegacyPropertyApplierMap();

    return true;
}

bool RSCompositingFilterRenderModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<ModifierNG::RSCompositingFilterRenderModifier> modifier =
        std::make_shared<ModifierNG::RSCompositingFilterRenderModifier>();
    modifier->GetType();
    RSProperties properties;
    modifier->ResetProperties(properties);
    modifier->GetLegacyPropertyApplierMap();

    return true;
}

bool RSDynamicLightUpRenderModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<ModifierNG::RSDynamicLightUpRenderModifier> modifier =
        std::make_shared<ModifierNG::RSDynamicLightUpRenderModifier>();
    modifier->GetType();
    RSProperties properties;
    modifier->ResetProperties(properties);
    modifier->GetLegacyPropertyApplierMap();
    
    return true;
}

bool RSForegroundFilterRenderModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<ModifierNG::RSForegroundFilterRenderModifier> modifier =
        std::make_shared<ModifierNG::RSForegroundFilterRenderModifier>();
    modifier->GetType();
    RSProperties properties;
    modifier->ResetProperties(properties);
    modifier->GetLegacyPropertyApplierMap();

    return true;
}

bool RSHDRBrightnessRenderModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<ModifierNG::RSHDRBrightnessRenderModifier> modifier =
        std::make_shared<ModifierNG::RSHDRBrightnessRenderModifier>();
    modifier->GetType();
    modifier->OnSetDirty();

    return true;
}

bool RSMaskRenderModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<ModifierNG::RSMaskRenderModifier> modifier = std::make_shared<ModifierNG::RSMaskRenderModifier>();
    modifier->GetType();
    RSProperties properties;
    modifier->ResetProperties(properties);
    modifier->GetLegacyPropertyApplierMap();

    return true;
}

bool RSOutlineRenderModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<ModifierNG::RSOutlineRenderModifier> modifier =
        std::make_shared<ModifierNG::RSOutlineRenderModifier>();
    modifier->GetType();
    RSProperties properties;
    modifier->ResetProperties(properties);
    modifier->GetLegacyPropertyApplierMap();

    return true;
}

bool RSParticleEffectRenderModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<ModifierNG::RSParticleEffectRenderModifier> modifier =
        std::make_shared<ModifierNG::RSParticleEffectRenderModifier>();
    modifier->GetType();
    RSProperties properties;
    modifier->ResetProperties(properties);
    modifier->GetLegacyPropertyApplierMap();
    
    return true;
}

bool RSPixelStretchRenderModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<ModifierNG::RSPixelStretchRenderModifier> modifier =
        std::make_shared<ModifierNG::RSPixelStretchRenderModifier>();
    modifier->GetType();
    RSProperties properties;
    modifier->ResetProperties(properties);
    modifier->GetLegacyPropertyApplierMap();
    return true;
}

bool RSPointLightRenderModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<ModifierNG::RSPointLightRenderModifier> modifier =
        std::make_shared<ModifierNG::RSPointLightRenderModifier>();
    modifier->GetType();
    RSProperties properties;
    modifier->ResetProperties(properties);
    modifier->GetLegacyPropertyApplierMap();
    return true;
}

bool RSShadowRenderModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<ModifierNG::RSShadowRenderModifier> modifier =
        std::make_shared<ModifierNG::RSShadowRenderModifier>();
    modifier->GetType();
    RSProperties properties;
    modifier->ResetProperties(properties);
    modifier->GetLegacyPropertyApplierMap();
    return true;
}

bool RSUseEffectRenderModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<ModifierNG::RSUseEffectRenderModifier> modifier =
        std::make_shared<ModifierNG::RSUseEffectRenderModifier>();
    modifier->GetType();
    RSProperties properties;
    modifier->ResetProperties(properties);
    modifier->GetLegacyPropertyApplierMap();

    return true;
}

bool RSVisibilityRenderModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<ModifierNG::RSVisibilityRenderModifier> modifier =
        std::make_shared<ModifierNG::RSVisibilityRenderModifier>();
    modifier->GetType();
    RSProperties properties;
    modifier->ResetProperties(properties);
    modifier->GetLegacyPropertyApplierMap();
    
    return true;
}

} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::RSAlphaRenderModifierFuzzTest(data, size);
    OHOS::Rosen::RSBackgroundFilterRenderModifierFuzzTest(data, size);
    OHOS::Rosen::RSBehindWindowFilterRenderModifierFuzzTest(data, size);
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

