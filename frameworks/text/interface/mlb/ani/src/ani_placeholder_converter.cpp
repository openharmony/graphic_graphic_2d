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
ani_status AniPlaceholderConverter::ParsePlaceholderSpanToNative(
    ani_env* env, ani_object obj, OHOS::Rosen::PlaceholderSpan& placeholderSpan)
{
    ani_status ret = env->Object_CallMethod_Double(
        obj, ANIClassFindMethod(env, ANI_INTERFACE_PLACEHOLDER_SPAN, "<get>width", ":d"), &placeholderSpan.width);
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to parse width, ret %{public}d", ret);
        return ret;
    }
    ret = env->Object_CallMethod_Double(
        obj, ANIClassFindMethod(env, ANI_INTERFACE_PLACEHOLDER_SPAN, "<get>height", ":d"), &placeholderSpan.height);
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to parse height, ret %{public}d", ret);
        return ret;
    }
    ret = AniTextUtils::ReadEnumField(env, obj, ANI_ENUM_PLACEHOLDER_ALIGNMENT,
        ANIClassFindMethod(
            env, ANI_INTERFACE_PLACEHOLDER_SPAN, "<get>align", ANI_WRAP_RETURN_E(ANI_ENUM_PLACEHOLDER_ALIGNMENT)),
        placeholderSpan.alignment);
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to parse align, ret %{public}d", ret);
        return ret;
    }
    ret = AniTextUtils::ReadEnumField(env, obj, ANI_ENUM_TEXT_BASELINE,
        ANIClassFindMethod(
            env, ANI_INTERFACE_PLACEHOLDER_SPAN, "<get>baseline", ANI_WRAP_RETURN_E(ANI_ENUM_TEXT_BASELINE)),
        placeholderSpan.baseline);
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to parse baseline, ret %{public}d", ret);
        return ret;
    }
    ret = env->Object_CallMethod_Double(obj,
        ANIClassFindMethod(env, ANI_INTERFACE_PLACEHOLDER_SPAN, "<get>baselineOffset", ":d"),
        &placeholderSpan.baselineOffset);
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to parse baselineOffset, ret %{public}d", ret);
        return ret;
    }
    return ANI_OK;
}
} // namespace OHOS::Text::ANI