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

#ifndef RENDER_SERVICE_BASE_EFFECT_RS_RENDER_ADAPTIVE_FILTER_HELPER_H
#define RENDER_SERVICE_BASE_EFFECT_RS_RENDER_ADAPTIVE_FILTER_HELPER_H

#include <type_traits>

#include "effect/rs_render_effect_template.h"
#include "effect/rs_render_property_tag.h"

namespace OHOS {
namespace Rosen {
namespace RSAdaptiveFilterHelper {

template<typename FilterType, typename PropTag>
void UpdateAdaptiveParam(
    FilterType* filter, std::shared_ptr<Drawing::GEVisualEffect>& geFilter, const PropTag& tag, float darkScale)
{
    using TagType = std::decay_t<PropTag>;
    // Compile-time no-op for dark-mode-only tags: they are selected indirectly
    // by the base adaptive tag mapping and should not be written directly.
    if constexpr (std::is_same_v<TagType, FrostedGlassDarkModeBlurParamsRenderTag> ||
                  std::is_same_v<TagType, FrostedGlassDarkModeWeightsEmbossRenderTag> ||
                  std::is_same_v<TagType, FrostedGlassDarkModeBgRatesRenderTag> ||
                  std::is_same_v<TagType, FrostedGlassDarkModeBgKBSRenderTag> ||
                  std::is_same_v<TagType, FrostedGlassDarkModeBgPosRenderTag> ||
                  std::is_same_v<TagType, FrostedGlassDarkModeBgNegRenderTag> ||
                  std::is_same_v<TagType, FrostedGlassDarkScaleRenderTag>) {
        return;
    }
    if (!geFilter) {
        return;
    }
    if constexpr (std::is_same_v<TagType, FrostedGlassBlurParamsRenderTag>) {
        const auto& darkVal = filter->template Getter<FrostedGlassDarkModeBlurParamsRenderTag>()->GetRef();
        RSNGRenderEffectHelper::UpdateAdaptiveParam(geFilter, tag, darkVal, darkScale);
    } else if constexpr (std::is_same_v<TagType, FrostedGlassWeightsEmbossRenderTag>) {
        const auto& darkVal = filter->template Getter<FrostedGlassDarkModeWeightsEmbossRenderTag>()->GetRef();
        RSNGRenderEffectHelper::UpdateAdaptiveParam(geFilter, tag, darkVal, darkScale);
    } else if constexpr (std::is_same_v<TagType, FrostedGlassBgRatesRenderTag>) {
        const auto& darkVal = filter->template Getter<FrostedGlassDarkModeBgRatesRenderTag>()->GetRef();
        RSNGRenderEffectHelper::UpdateAdaptiveParam(geFilter, tag, darkVal, darkScale);
    } else if constexpr (std::is_same_v<TagType, FrostedGlassBgKBSRenderTag>) {
        const auto& darkVal = filter->template Getter<FrostedGlassDarkModeBgKBSRenderTag>()->GetRef();
        RSNGRenderEffectHelper::UpdateAdaptiveParam(geFilter, tag, darkVal, darkScale);
    } else if constexpr (std::is_same_v<TagType, FrostedGlassBgPosRenderTag>) {
        const auto& darkVal = filter->template Getter<FrostedGlassDarkModeBgPosRenderTag>()->GetRef();
        RSNGRenderEffectHelper::UpdateAdaptiveParam(geFilter, tag, darkVal, darkScale);
    } else if constexpr (std::is_same_v<TagType, FrostedGlassBgNegRenderTag>) {
        const auto& darkVal = filter->template Getter<FrostedGlassDarkModeBgNegRenderTag>()->GetRef();
        RSNGRenderEffectHelper::UpdateAdaptiveParam(geFilter, tag, darkVal, darkScale);
    } else {
        RSNGRenderEffectHelper::UpdateVisualEffectParam(geFilter, tag);
    }
}
} // namespace RSAdaptiveFilterHelper
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_EFFECT_RS_RENDER_ADAPTIVE_FILTER_HELPER_H
