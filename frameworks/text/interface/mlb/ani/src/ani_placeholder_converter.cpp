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
    static AniCacheParam paramWidth = { ANI_INTERFACE_PLACEHOLDER_SPAN, "<get>width", ":d" };
    ani_status ret = AniTextUtils::GetPropertyByCache_Double(env, obj, paramWidth, placeholderSpan.width);
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to parse width, ret %{public}d", ret);
        return ret;
    }
    static AniCacheParam paramHeight = { ANI_INTERFACE_PLACEHOLDER_SPAN, "<get>height", ":d" };
    ret = AniTextUtils::GetPropertyByCache_Double(env, obj, paramHeight, placeholderSpan.height);
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to parse height, ret %{public}d", ret);
        return ret;
    }

    static AniCacheParam paramAlign =
        { ANI_INTERFACE_PLACEHOLDER_SPAN, "<get>align", ANI_WRAP_RETURN_E(ANI_ENUM_PLACEHOLDER_ALIGNMENT) };
    ret = AniTextUtils::ReadEnumField(env, obj, paramAlign, placeholderSpan.alignment);
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to parse align, ret %{public}d", ret);
        return ret;
    }
    static AniCacheParam paramBaseline =
        { ANI_INTERFACE_PLACEHOLDER_SPAN, "<get>baseline", ANI_WRAP_RETURN_E(ANI_ENUM_TEXT_BASELINE) };
    ret = AniTextUtils::ReadEnumField(env, obj, paramBaseline, placeholderSpan.baseline);
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to parse baseline, ret %{public}d", ret);
        return ret;
    }
    static AniCacheParam paramBaselineOffset = { ANI_INTERFACE_PLACEHOLDER_SPAN, "<get>baselineOffset", ":d" };
    ret = AniTextUtils::GetPropertyByCache_Double(env, obj, paramBaselineOffset, placeholderSpan.baselineOffset);
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to parse baselineOffset, ret %{public}d", ret);
        return ret;
    }
    return ANI_OK;
}
} // namespace OHOS::Text::ANI