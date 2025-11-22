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

#include "ani_placeholder_converter.h"

#include "ani_common.h"
#include "ani_text_utils.h"

namespace OHOS::Text::ANI {
using namespace OHOS::Rosen;
namespace {
constexpr CacheKey PLACEHOLDER_SPAN_WIDTH_KEY{ANI_INTERFACE_PLACEHOLDER_SPAN, "<get>width", ":d"};
constexpr CacheKey PLACEHOLDER_SPAN_HEIGHT_KEY{ANI_INTERFACE_PLACEHOLDER_SPAN, "<get>height", ":d"};
constexpr CacheKey PLACEHOLDER_SPAN_ALIGN_KEY{
    ANI_INTERFACE_PLACEHOLDER_SPAN, "<get>align", ANI_WRAP_RETURN_E(ANI_ENUM_PLACEHOLDER_ALIGNMENT)};
constexpr CacheKey PLACEHOLDER_SPAN_BASELINE_KEY{
    ANI_INTERFACE_PLACEHOLDER_SPAN, "<get>baseline", ANI_WRAP_RETURN_E(ANI_ENUM_TEXT_BASELINE)};
constexpr CacheKey PLACEHOLDER_SPAN_BASELINE_OFFSET_KEY{ANI_INTERFACE_PLACEHOLDER_SPAN, "<get>baselineOffset", ":d"};
} // namespace

ani_status AniPlaceholderConverter::ParsePlaceholderSpanToNative(
    ani_env* env, ani_object obj, OHOS::Rosen::PlaceholderSpan& placeholderSpan)
{
    ani_status ret =
        env->Object_CallMethod_Double(obj, AniClassFindMethod(env, PLACEHOLDER_SPAN_WIDTH_KEY), &placeholderSpan.width);
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to parse width, ret %{public}d", ret);
        return ret;
    }
    ret = env->Object_CallMethod_Double(
        obj, AniClassFindMethod(env, PLACEHOLDER_SPAN_HEIGHT_KEY), &placeholderSpan.height);
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to parse height, ret %{public}d", ret);
        return ret;
    }
    ret = AniTextUtils::ReadEnumField(env, obj, AniTextEnum::placeHolderAlignment,
        AniClassFindMethod(env, PLACEHOLDER_SPAN_ALIGN_KEY), placeholderSpan.alignment);
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to parse align, ret %{public}d", ret);
        return ret;
    }
    ret = AniTextUtils::ReadEnumField(env, obj, AniTextEnum::textBaseLine,
        AniClassFindMethod(env, PLACEHOLDER_SPAN_BASELINE_KEY), placeholderSpan.baseline);
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to parse baseline, ret %{public}d", ret);
        return ret;
    }
    ret = env->Object_CallMethod_Double(
        obj, AniClassFindMethod(env, PLACEHOLDER_SPAN_BASELINE_OFFSET_KEY), &placeholderSpan.baselineOffset);
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to parse baselineOffset, ret %{public}d", ret);
        return ret;
    }
    return ANI_OK;
}
} // namespace OHOS::Text::ANI