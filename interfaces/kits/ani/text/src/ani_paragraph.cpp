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

#include "ani_text_utils.h"
#include "font_collection.h"
#include "typography_create.h"

#include "utils/text_log.h"

namespace OHOS::Rosen {
namespace {
const char* ANI_CLASS_PARAGRAPH = "L@ohos/graphics/text/text/Paragraph;";
const char* ANI_CLASS_LINEMETRICS_I = "L@ohos/graphics/text/text/LineMetricsInternal;";
} // namespace

AniParagraph::AniParagraph() {}

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
        TEXT_LOGE("[ANI] null env");
        return ANI_NOT_FOUND;
    }

    ani_class cls = nullptr;
    ret = env->FindClass(ANI_CLASS_PARAGRAPH, &cls);
    if (ret != ANI_OK) {
        TEXT_LOGE("[ANI] can't find class: %{public}s", ANI_CLASS_PARAGRAPH);
        return ANI_NOT_FOUND;
    }

    std::array methods = {
        ani_native_function { "nativeConstructor", nullptr, reinterpret_cast<void*>(Constructor) },
        ani_native_function { "nativeLayoutSync", "D:V", reinterpret_cast<void*>(LayoutSync) },
        ani_native_function { "nativeGetLongestLine", ":D", reinterpret_cast<void*>(GetLongestLine) },
        ani_native_function { "nativeGetLineMetrics", ":Lescompat/Array;", reinterpret_cast<void*>(GetLineMetrics) },
    };

    ret = env->Class_BindNativeMethods(cls, methods.data(), methods.size());
    if (ret != ANI_OK) {
        TEXT_LOGE("[ANI] bind methods fail: %{public}s", ANI_CLASS_PARAGRAPH);
        return ANI_NOT_FOUND;
    }
    return ANI_OK;
}

void AniParagraph::LayoutSync(ani_env* env, ani_object object, ani_double width)
{
    AniParagraph* aniParagraph = AniTextUtils::GetNativeFromObj<AniParagraph>(env, object);
    if (aniParagraph == nullptr || aniParagraph->paragraph_ == nullptr) {
        TEXT_LOGE("paragraph is null");
        return;
    }
    aniParagraph->paragraph_->Layout(width);
}

ani_double AniParagraph::GetLongestLine(ani_env* env, ani_object object)
{
    AniParagraph* aniParagraph = AniTextUtils::GetNativeFromObj<AniParagraph>(env, object);
    if (aniParagraph == nullptr || aniParagraph->paragraph_ == nullptr) {
        TEXT_LOGE("paragraph is null");
        return 0;
    }
    return aniParagraph->paragraph_->GetActualWidth();
}

ani_ref AniParagraph::GetLineMetrics(ani_env* env, ani_object object)
{
    ani_object arrayObj = AniTextUtils::CreateAniUndefined(env);
    AniParagraph* aniParagraph = AniTextUtils::GetNativeFromObj<AniParagraph>(env, object);
    if (aniParagraph == nullptr || aniParagraph->paragraph_ == nullptr) {
        TEXT_LOGE("paragraph is null");
        return arrayObj;
    }
    std::vector<LineMetrics> vectorLineMetrics = aniParagraph->paragraph_->GetLineMetrics();
    arrayObj = AniTextUtils::CreateAniArray(env, vectorLineMetrics.size());

    ani_size index = 0;
    for (auto lineMetrics : vectorLineMetrics) {
        ani_object aniObj = AniTextUtils::CreateAniObject(env, ANI_CLASS_LINEMETRICS_I, ":V");
        env->Object_SetPropertyByName_Int(aniObj, "StartIndex", ani_int(lineMetrics.startIndex));
        env->Object_SetPropertyByName_Int(aniObj, "endIndex", ani_int(lineMetrics.endIndex));
        env->Object_SetPropertyByName_Double(aniObj, "ascent", ani_int(lineMetrics.ascender));
        env->Object_SetPropertyByName_Double(aniObj, "descent", ani_int(lineMetrics.descender));
        env->Object_SetPropertyByName_Double(aniObj, "height", ani_int(lineMetrics.height));
        env->Object_SetPropertyByName_Double(aniObj, "width", ani_int(lineMetrics.width));
        env->Object_SetPropertyByName_Double(aniObj, "left", ani_int(lineMetrics.x));
        env->Object_SetPropertyByName_Double(aniObj, "baseline", ani_int(lineMetrics.baseline));
        env->Object_SetPropertyByName_Int(aniObj, "lineNumber", ani_int(lineMetrics.lineNumber));
        env->Object_SetPropertyByName_Int(aniObj, "topHeight", ani_int(lineMetrics.y));
        // env->Object_SetPropertyByName_Int(aniObj, "runMetrics", ani_int(lineMetrics.runMetrics));
        if (ANI_OK != env->Object_CallMethodByName_Void(arrayObj, "$_set", "ILstd/core/Object;:V", index, aniObj)) {
            TEXT_LOGE("Object_CallMethodByName_Void $_set failed");
            break;
        }
        index++;
    }
    return arrayObj;
}
} // namespace OHOS::Rosen