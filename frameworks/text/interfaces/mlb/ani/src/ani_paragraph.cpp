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

#include "ani_paragraph.h"

#include <codecvt>
#include <cstdint>

#include "ani.h"
#include "ani_common.h"
#include "ani_text_utils.h"
#include "draw/canvas.h"
#include "font_collection.h"
#include "typography_create.h"
#include "text/font_metrics.h"
#include "utils/text_log.h"

namespace OHOS::Text::NAI {

AniParagraph::AniParagraph()
{
}

void AniParagraph::setTypography(std::unique_ptr<OHOS::Rosen::Typography>& typography)
{
    paragraph_ = std::move(typography);
}

void AniParagraph::Constructor(ani_env* env, ani_object object)
{
    AniParagraph* aniParagraph = new AniParagraph();
    if (ANI_OK != env->Object_SetFieldByName_Long(object, "nativeObj", reinterpret_cast<ani_long>(aniParagraph))) {
        TEXT_LOGE("Failed to create ani Paragraph obj");
        return;
    }
}

ani_status AniParagraph::AniInit(ani_vm* vm, uint32_t* result)
{
    ani_env* env;
    ani_status ret;
    if ((ret = vm->GetEnv(ANI_VERSION_1, &env)) != ANI_OK) {
        TEXT_LOGE("[ANI] AniParagraph null env:%{public}d", ret);
        return ANI_NOT_FOUND;
    }

    ani_class cls = nullptr;
    ret = env->FindClass(ANI_CLASS_PARAGRAPH, &cls);
    if (ret != ANI_OK) {
        TEXT_LOGE("[ANI] AniParagraph can't find class:%{public}d", ret);
        return ANI_NOT_FOUND;
    }

    std::string paintSignature = std::string(ANI_CLASS_CANVAS) + "DD:V";
    std::string paintOnPathSignature = std::string(ANI_CLASS_CANVAS) + std::string(ANI_CLASS_PATH) + "DD:V";
    std::array methods = {
        ani_native_function{"constructorNative", ":V", reinterpret_cast<void*>(Constructor)},
        ani_native_function{"layoutSync", "D:V", reinterpret_cast<void*>(LayoutSync)},
        ani_native_function{"paint", paintSignature.c_str(), reinterpret_cast<void*>(Paint)},
        ani_native_function{"paintOnPath", paintOnPathSignature.c_str(), reinterpret_cast<void*>(PaintOnPath)},
        ani_native_function{"getLongestLine", ":D", reinterpret_cast<void*>(GetLongestLine)},
        ani_native_function{"getLineMetrics", ":Lescompat/Array;", reinterpret_cast<void*>(GetLineMetrics)},
        ani_native_function{"nativeGetLineMetricsAt", "D:L@ohos/graphics/text/text/LineMetrics;",
                            reinterpret_cast<void*>(GetLineMetricsAt)},
    };

    ret = env->Class_BindNativeMethods(cls, methods.data(), methods.size());
    if (ret != ANI_OK) {
        TEXT_LOGE("[ANI] AniParagraph bind methods fail::%{public}d", ret);
        return ANI_NOT_FOUND;
    }
    return ANI_OK;
}

void AniParagraph::LayoutSync(ani_env* env, ani_object object, ani_double width)
{
    AniParagraph* aniParagraph = AniTextUtils::GetNativeFromObj<AniParagraph>(env, object);
    if (aniParagraph == nullptr || aniParagraph->paragraph_ == nullptr) {
        TEXT_LOGE("paragraph is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return;
    }
    aniParagraph->paragraph_->Layout(width);
}

void AniParagraph::Paint(ani_env* env, ani_object object, ani_object canvas, ani_double x, ani_double y)
{
    AniParagraph* aniParagraph = AniTextUtils::GetNativeFromObj<AniParagraph>(env, object);
    if (aniParagraph == nullptr || aniParagraph->paragraph_ == nullptr) {
        TEXT_LOGE("paragraph is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return;
    }
    Drawing::Canvas* canvasI = AniTextUtils::GetNativeFromObj<Drawing::Canvas>(env, canvas);
    if (canvasI == nullptr) {
        TEXT_LOGE("canvas is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "canvas unavailable.");
        return;
    }
    aniParagraph->paragraph_->Paint(canvasI, x, y);
}

void AniParagraph::PaintOnPath(ani_env* env, ani_object object, ani_object canvas, ani_object path, ani_double hOffset,
                               ani_double vOffset)
{
    AniParagraph* aniParagraph = AniTextUtils::GetNativeFromObj<AniParagraph>(env, object);
    if (aniParagraph == nullptr || aniParagraph->paragraph_ == nullptr) {
        TEXT_LOGE("paragraph is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return;
    }
    Drawing::Canvas* canvasI = AniTextUtils::GetNativeFromObj<Drawing::Canvas>(env, canvas);
    if (canvasI == nullptr) {
        TEXT_LOGE("canvas is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Canvas unavailable.");
        return;
    }
    Drawing::Path* pathI = AniTextUtils::GetNativeFromObj<Drawing::Path>(env, canvas);
    if (pathI == nullptr) {
        TEXT_LOGE("Path is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Path unavailable.");
        return;
    }
    aniParagraph->paragraph_->Paint(canvasI, pathI, hOffset, vOffset);
}

ani_double AniParagraph::GetLongestLine(ani_env* env, ani_object object)
{
    AniParagraph* aniParagraph = AniTextUtils::GetNativeFromObj<AniParagraph>(env, object);
    if (aniParagraph == nullptr || aniParagraph->paragraph_ == nullptr) {
        TEXT_LOGE("paragraph is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return 0;
    }
    return aniParagraph->paragraph_->GetActualWidth();
}

ani_ref AniParagraph::GetLineMetrics(ani_env* env, ani_object object)
{
    ani_object arrayObj = AniTextUtils::CreateAniUndefined(env);
    AniParagraph* aniParagraph = AniTextUtils::GetNativeFromObj<AniParagraph>(env, object);
    if (aniParagraph == nullptr || aniParagraph->paragraph_ == nullptr) {
        TEXT_LOGE("Paragraph is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return arrayObj;
    }
    std::vector<LineMetrics> vectorLineMetrics = aniParagraph->paragraph_->GetLineMetrics();
    arrayObj = AniTextUtils::CreateAniArray(env, vectorLineMetrics.size());
    ani_boolean isUndefined;
    env->Reference_IsUndefined(arrayObj, &isUndefined);
    if (isUndefined) {
        TEXT_LOGE("Create arrayObject failed");
        return arrayObj;
    }
    ani_size index = 0;
    for (const auto& lineMetrics : vectorLineMetrics) {
        ani_object aniObj = AniCommon::ParseLineMetricsToAni(env, lineMetrics);
        if (ANI_OK != env->Object_CallMethodByName_Void(arrayObj, "$_set", "ILstd/core/Object;:V", index, aniObj)) {
            TEXT_LOGE("Object_CallMethodByName_Void $_set failed");
            break;
        }
        index++;
    }
    return arrayObj;
}

ani_object AniParagraph::GetLineMetricsAt(ani_env* env, ani_object object, ani_double lineNumber)
{
    AniParagraph* aniParagraph = AniTextUtils::GetNativeFromObj<AniParagraph>(env, object);
    if (aniParagraph == nullptr || aniParagraph->paragraph_ == nullptr) {
        TEXT_LOGE("Paragraph is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return AniTextUtils::CreateAniUndefined(env);
    }
    LineMetrics lineMetrics;
    if (!aniParagraph->paragraph_->GetLineMetricsAt(lineNumber, &lineMetrics)) {
        TEXT_LOGE("Failed to get line metrics");
        return AniTextUtils::CreateAniUndefined(env);
    }
    return AniCommon::ParseLineMetricsToAni(env, lineMetrics);
}
} // namespace OHOS::Text::NAI