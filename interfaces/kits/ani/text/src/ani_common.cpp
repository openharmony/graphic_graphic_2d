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

#include <cstddef>
#include "ani.h"
#include "ani_common.h"
#include "ani_text_utils.h"
#include "utils/text_log.h"

namespace OHOS::Rosen {
std::unique_ptr<TypographyStyle> AniCommon::ParseParagraphStyle(ani_env* env, ani_object obj)
{
    ani_class cls;
    ani_status ret;
    ret = env->FindClass(ANI_CLASS_PARAGRAPH_STYLE, &cls);
    if (ret != ANI_OK) {
        TEXT_LOGE("[ANI] can't find class:%{public}d", ret);
        return nullptr;
    }
    ani_boolean isObj = false;
    ret = env->Object_InstanceOf(obj, cls, &isObj);
    if (!isObj) {
        TEXT_LOGE("[ANI] Object mismatch:%{public}d", ret);
        return nullptr;
    }
    std::unique_ptr<TypographyStyle> paragraphStyle = std::make_unique<TypographyStyle>();

    ani_ref maxLinesRef = nullptr;
    if (AniTextUtils::ReadOptionalField(env, obj, "maxLines", maxLinesRef) == ANI_OK && maxLinesRef != nullptr) {
        ani_double maxLines{0};
        env->Object_CallMethodByName_Double(static_cast<ani_object>(maxLinesRef), "doubleValue", nullptr, &maxLines);
        paragraphStyle->maxLines = static_cast<size_t>(maxLines);
    }

    ani_ref textStyleRef = nullptr;
    if (AniTextUtils::ReadOptionalField(env, obj, "textStyle", textStyleRef) == ANI_OK && textStyleRef != nullptr) {
        std::unique_ptr<TextStyle> textStyle = ParseTextStyle(env, static_cast<ani_object>(textStyleRef));
        if (textStyle != nullptr) {
            paragraphStyle->SetTextStyle(*textStyle);
        }
    }

    return paragraphStyle;
}

std::unique_ptr<TextStyle> AniCommon::ParseTextStyle(ani_env* env, ani_object obj)
{
    ani_class cls;
    ani_status ret;
    ret = env->FindClass(ANI_CLASS_TEXT_STYLE, &cls);
    if (ret != ANI_OK) {
        TEXT_LOGE("[ANI] can't find class:%{public}d", ret);
        return nullptr;
    }
    ani_boolean isObj = false;
    ret = env->Object_InstanceOf(obj, cls, &isObj);
    if (!isObj) {
        TEXT_LOGE("[ANI] Object mismatch:%{public}d", ret);
        return nullptr;
    }
    std::unique_ptr<TextStyle> textStyle = std::make_unique<TextStyle>();

    ani_double fontSize{0};
    if (AniTextUtils::ReadOptionalDoubleField(env, obj, "fontSize", fontSize) == ANI_OK) {
        textStyle->fontSize = static_cast<size_t>(fontSize);
    }
    ani_double letterSpacing{0};
    if (AniTextUtils::ReadOptionalDoubleField(env, obj, "letterSpacing", letterSpacing) == ANI_OK) {
        textStyle->letterSpacing = static_cast<size_t>(letterSpacing);
    }
    ani_double wordSpacing{0};
    if (AniTextUtils::ReadOptionalDoubleField(env, obj, "wordSpacing", wordSpacing) == ANI_OK) {
        textStyle->wordSpacing = static_cast<size_t>(wordSpacing);
    }
    ani_double heightScale{0};
    if (AniTextUtils::ReadOptionalDoubleField(env, obj, "heightScale", heightScale) == ANI_OK) {
        textStyle->heightScale = static_cast<size_t>(heightScale);
    }
    ani_boolean halfLeading{false};
    if (AniTextUtils::ReadOptionalBoolField(env, obj, "halfLeading", halfLeading) == ANI_OK) {
        textStyle->halfLeading = static_cast<bool>(halfLeading);
    }
    ani_boolean heightOnly{false};
    if (AniTextUtils::ReadOptionalBoolField(env, obj, "heightOnly", heightOnly) == ANI_OK) {
        textStyle->heightOnly = static_cast<bool>(heightOnly);
    }
    ani_boolean baselineShift{false};
    if (AniTextUtils::ReadOptionalBoolField(env, obj, "baselineShift", baselineShift) == ANI_OK) {
        textStyle->baseLineShift = static_cast<bool>(baselineShift);
    }
    std::string locale;
    if (AniTextUtils::ReadOptionalStringField(env, obj, "locale", locale) == ANI_OK) {
        textStyle->locale = locale;
    }

    return textStyle;
}
} // namespace OHOS::Rosen