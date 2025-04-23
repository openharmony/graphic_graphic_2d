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
#include <cstdint>
#include <vector>
#include "ani.h"
#include "ani_common.h"
#include "ani_text_utils.h"
#include "typography_types.h"
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

    double maxLines;
    if (AniTextUtils::ReadOptionalDoubleField(env, obj, "maxLines", maxLines) == ANI_OK) {
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

    ani_ref colorRef = nullptr;
    if (AniTextUtils::ReadOptionalField(env, obj, "color", colorRef) == ANI_OK && colorRef != nullptr) {
        ParseDrawingColor(env, static_cast<ani_object>(colorRef), textStyle->color);
    }

    AniTextUtils::ReadOptionalEnumField(env, obj, "fontWeight", textStyle->fontWeight);
    AniTextUtils::ReadOptionalEnumField(env, obj, "fontStyle", textStyle->fontStyle);
    if (textStyle->fontStyle == FontStyle::OBLIQUE) {
        textStyle->fontStyle = FontStyle::ITALIC;
    }
    AniTextUtils::ReadOptionalEnumField(env, obj, "baseline", textStyle->baseline);

    AniTextUtils::ReadOptionalArrayField<std::string>(
        env, obj, "fontFamilies", textStyle->fontFamilies,
        [](ani_env* env, ani_ref ref) { return AniTextUtils::AniToStdStringUtf8(env, static_cast<ani_string>(ref)); });

    AniTextUtils::ReadOptionalDoubleField(env, obj, "fontSize", textStyle->fontSize);
    AniTextUtils::ReadOptionalDoubleField(env, obj, "letterSpacing", textStyle->letterSpacing);
    AniTextUtils::ReadOptionalDoubleField(env, obj, "wordSpacing", textStyle->wordSpacing);
    AniTextUtils::ReadOptionalDoubleField(env, obj, "heightScale", textStyle->heightScale);
    AniTextUtils::ReadOptionalBoolField(env, obj, "halfLeading", textStyle->halfLeading);
    AniTextUtils::ReadOptionalBoolField(env, obj, "heightOnly", textStyle->heightOnly);
    AniTextUtils::ReadOptionalU16StringField(env, obj, "ellipsis", textStyle->ellipsis);
    AniTextUtils::ReadOptionalEnumField(env, obj, "ellipsisMode", textStyle->ellipsisModal);
    AniTextUtils::ReadOptionalStringField(env, obj, "locale", textStyle->locale);
    AniTextUtils::ReadOptionalDoubleField(env, obj, "baselineShift", textStyle->baseLineShift);
    ParseFontFeature(env, obj, textStyle->fontFeatures);

    return textStyle;
}

void AniCommon::ParseDrawingColor(ani_env* env, ani_object obj, Drawing::Color& color)
{
    ani_class cls;
    ani_status ret;
    ret = env->FindClass(ANI_CLASS_TEXT_STYLE, &cls);
    if (ret != ANI_OK) {
        TEXT_LOGE("[ANI] can't find class:%{public}d", ret);
        return;
    }
    ani_boolean isObj = false;
    ret = env->Object_InstanceOf(obj, cls, &isObj);
    if (!isObj) {
        TEXT_LOGE("[ANI] Object mismatch:%{public}d", ret);
        return;
    }
}

void AniCommon::ParseFontFeature(ani_env* env, ani_object obj, FontFeatures& fontFeatures)
{
    std::vector<std::string> array;
    AniTextUtils::ReadOptionalArrayField<std::string>(
        env, obj, "fontFeatures", array, [&fontFeatures](ani_env* env, ani_ref ref) {
            ani_object obj = static_cast<ani_object>(ref);
            ani_class cls;
            ani_status ret;
            ret = env->FindClass(ANI_CLASS_FONTFEATURE, &cls);
            if (ret != ANI_OK) {
                TEXT_LOGE("[ANI] can't find class:%{public}d", ret);
                return "";
            }
            ani_boolean isObj = false;
            ret = env->Object_InstanceOf(static_cast<ani_object>(ref), cls, &isObj);
            if (!isObj) {
                TEXT_LOGE("[ANI] Object mismatch:%{public}d", ret);
                return "";
            }
            ani_ref nameRef = nullptr;
            ret = env->Object_GetPropertyByName_Ref(obj, "name", &nameRef);
            if (ret != ANI_OK) {
                TEXT_LOGE("[ANI] get filed name failed:%{public}d", ret);
                return "";
            }
            std::string name = AniTextUtils::AniToStdStringUtf8(env, static_cast<ani_string>(nameRef));
            ani_double valueDouble;
            ret = env->Object_GetFieldByName_Double(obj, "value", &valueDouble);
            if (ret != ANI_OK) {
                TEXT_LOGE("[ANI] get filed name failed:%{public}d", ret);
                return "";
            }
            fontFeatures.SetFeature(name, static_cast<int>(valueDouble));
            return "";
        });
}
} // namespace OHOS::Rosen