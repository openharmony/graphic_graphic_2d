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

#include <codecvt>
#include <cstdint>

#include "ani_common.h"
#include "ani_line_metrics_converter.h"
#include "ani_paragraph.h"
#include "ani_text_line.h"
#include "ani_text_utils.h"
#include "canvas_ani/ani_canvas.h"
#include "font_collection.h"
#include "text/font_metrics.h"
#include "typography.h"
#include "typography_create.h"
#include "utils/text_log.h"

namespace OHOS::Text::ANI {
using namespace OHOS::Rosen;
ani_object AniParagraph::SetTypography(ani_env* env, OHOS::Rosen::Typography* typography)
{
    if (typography == nullptr) {
        TEXT_LOGE("Failed to set paragraph, emtpy ptr");
        return AniTextUtils::CreateAniUndefined(env);
    }
    ani_object pargraphObj = AniTextUtils::CreateAniObject(env, ANI_CLASS_PARAGRAPH, ":V");
    ani_status ret =
        env->Object_SetFieldByName_Long(pargraphObj, "nativeObj", reinterpret_cast<ani_long>(typography));
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to create ani Paragraph obj");
        return AniTextUtils::CreateAniUndefined(env);
    }
    return pargraphObj;
}

ani_status AniParagraph::AniInit(ani_vm* vm, uint32_t* result)
{
    ani_env* env = nullptr;
    ani_status ret = vm->GetEnv(ANI_VERSION_1, &env);
    if (ret != ANI_OK || env == nullptr) {
        TEXT_LOGE("Failed to get env, ret %{public}d", ret);
        return ANI_NOT_FOUND;
    }

    ani_class cls = nullptr;
    ret = env->FindClass(ANI_CLASS_PARAGRAPH, &cls);
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to find class, ret %{public}d", ret);
        return ANI_NOT_FOUND;
    }

    std::string paintSignature = std::string(ANI_CLASS_CANVAS) + "DD:V";
    std::string paintOnPathSignature = std::string(ANI_CLASS_CANVAS) + std::string(ANI_CLASS_PATH) + "DD:V";
    std::array methods = {
        ani_native_function{"layoutSync", "D:V", reinterpret_cast<void*>(LayoutSync)},
        ani_native_function{"paint", paintSignature.c_str(), reinterpret_cast<void*>(Paint)},
        ani_native_function{"paintOnPath", paintOnPathSignature.c_str(), reinterpret_cast<void*>(PaintOnPath)},
        ani_native_function{"getLongestLine", ":D", reinterpret_cast<void*>(GetLongestLine)},
        ani_native_function{"getTextLines", getTextLinesSignature.c_str(), reinterpret_cast<void*>(GetTextLines)},
        ani_native_function{"getLineMetrics", ":Lescompat/Array;", reinterpret_cast<void*>(GetLineMetrics)},
        ani_native_function{"nativeGetLineMetricsAt", "D:L@ohos/graphics/text/text/LineMetrics;",
            reinterpret_cast<void*>(GetLineMetricsAt)},
    };

    ret = env->Class_BindNativeMethods(cls, methods.data(), methods.size());
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to bind methods, ret %{public}d", ret);
        return ANI_ERROR;
    }
    return ANI_OK;
}

void AniParagraph::LayoutSync(ani_env* env, ani_object object, ani_double width)
{
    Typography* typography = AniTextUtils::GetNativeFromObj<Typography>(env, object);
    if (typography == nullptr) {
        TEXT_LOGE("Paragraph is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return;
    }
    typography->Layout(width);
}

void AniParagraph::Paint(ani_env* env, ani_object object, ani_object canvas, ani_double x, ani_double y)
{
    Typography* typography = AniTextUtils::GetNativeFromObj<Typography>(env, object);
    if (typography == nullptr) {
        TEXT_LOGE("Paragraph is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return;
    }
    Drawing::AniCanvas* aniCanvas =  AniTextUtils::GetNativeFromObj<Drawing::AniCanvas>(env, canvas);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        TEXT_LOGE("Canvas is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "canvas unavailable.");
        return;
    }
    typography->Paint(aniCanvas->GetCanvas(), x, y);
}

void AniParagraph::PaintOnPath(
    ani_env* env, ani_object object, ani_object canvas, ani_object path, ani_double hOffset, ani_double vOffset)
{
    Typography* typography = AniTextUtils::GetNativeFromObj<Typography>(env, object);
    if (typography == nullptr) {
        TEXT_LOGE("Paragraph is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return;
    }
    Drawing::AniCanvas* aniCanvas =  AniTextUtils::GetNativeFromObj<Drawing::AniCanvas>(env, canvas);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        TEXT_LOGE("Canvas is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Canvas unavailable.");
        return;
    }
    Drawing::Path* pathInternal = AniTextUtils::GetNativeFromObj<Drawing::Path>(env, path);
    if (pathInternal == nullptr) {
        TEXT_LOGE("Path is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Path unavailable.");
        return;
    }
    typography->Paint(aniCanvas->GetCanvas(), pathInternal, hOffset, vOffset);
}

ani_double AniParagraph::GetLongestLine(ani_env* env, ani_object object)
{
    Typography* typography = AniTextUtils::GetNativeFromObj<Typography>(env, object);
    if (typography == nullptr) {
        TEXT_LOGE("Paragraph is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return 0;
    }
    return typography->GetActualWidth();
}

ani_ref AniParagraph::GetTextLines(ani_env* env, ani_object object)
{
    ani_object arrayObj = AniTextUtils::CreateAniUndefined(env);
    Typography* typography = AniTextUtils::GetNativeFromObj<Typography>(env, object);
    if (typography == nullptr) {
        TEXT_LOGE("Paragraph is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return arrayObj;
    }
    std::vector<std::unique_ptr<TextLineBase>> textlines = typography->GetTextLines();
    arrayObj = AniTextUtils::CreateAniArray(env, textlines.size());
    ani_boolean isUndefined;
    env->Reference_IsUndefined(arrayObj, &isUndefined);
    if (isUndefined) {
        TEXT_LOGE("Failed to create arrayObject");
        return arrayObj;
    }
    ani_size index = 0;
    for (auto& textline : textlines) {
        if (textline == nullptr) {
            continue;
        }
        TextLineBase* textLineBasePtr = textline.release();
        ani_object aniObj = AniTextLine::CreateTextLine(env, textLineBasePtr);
        if (AniTextUtils::IsUndefined(env, aniObj)) {
            TEXT_LOGE("Failed to create text line");
            delete textLineBasePtr;
            textLineBasePtr = nullptr;
            continue;
        }
        ani_status ret = env->Object_CallMethodByName_Void(arrayObj, "$_set", "ILstd/core/Object;:V", index, aniObj);
        if (ret != ANI_OK) {
            TEXT_LOGE("Failed to set textline item %{public}zu", index);
            delete textLineBasePtr;
            textLineBasePtr = nullptr;
            continue;
        }
        index++;
    }
    return arrayObj;
}

ani_ref AniParagraph::GetLineMetrics(ani_env* env, ani_object object)
{
    ani_object arrayObj = AniTextUtils::CreateAniUndefined(env);
    Typography* typography = AniTextUtils::GetNativeFromObj<Typography>(env, object);
    if (typography == nullptr) {
        TEXT_LOGE("Paragraph is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return arrayObj;
    }
    std::vector<LineMetrics> vectorLineMetrics = typography->GetLineMetrics();
    arrayObj = AniTextUtils::CreateAniArray(env, vectorLineMetrics.size());
    ani_boolean isUndefined;
    env->Reference_IsUndefined(arrayObj, &isUndefined);
    if (isUndefined) {
        TEXT_LOGE("Failed to create arrayObject");
        return arrayObj;
    }
    ani_size index = 0;
    for (const auto& lineMetrics : vectorLineMetrics) {
        ani_object aniObj = AniLineMetricsConverter::ParseLineMetricsToAni(env, lineMetrics);
        ani_status ret = env->Object_CallMethodByName_Void(arrayObj, "$_set", "ILstd/core/Object;:V", index, aniObj);
        if (ret != ANI_OK) {
            TEXT_LOGE("Failed to set lineMetrics item");
            continue;
        }
        index++;
    }
    return arrayObj;
}

ani_object AniParagraph::GetLineMetricsAt(ani_env* env, ani_object object, ani_double lineNumber)
{
    Typography* typography = AniTextUtils::GetNativeFromObj<Typography>(env, object);
    if (typography == nullptr) {
        TEXT_LOGE("Paragraph is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return AniTextUtils::CreateAniUndefined(env);
    }
    LineMetrics lineMetrics;
    if (!typography->GetLineMetricsAt(lineNumber, &lineMetrics)) {
        TEXT_LOGE("Failed to get line metrics");
        return AniTextUtils::CreateAniUndefined(env);
    }
    return AniLineMetricsConverter::ParseLineMetricsToAni(env, lineMetrics);
}
} // namespace OHOS::Text::ANI