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

#include "ani_path_effect.h"
#include "effect/path_effect.h"
#include "path_ani/ani_path.h"
#include "ani_drawing_utils.h"

namespace OHOS::Rosen {
namespace Drawing {

const char* ANI_CLASS_PATHEFFECT_NAME = "@ohos.graphics.drawing.drawing.PathEffect";

ani_status AniPathEffect::AniInit(ani_env *env)
{
    ani_class cls = nullptr;
    ani_status ret = env->FindClass(ANI_CLASS_PATHEFFECT_NAME, &cls);
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] can't find class: %{public}s", ANI_CLASS_PATHEFFECT_NAME);
        return ANI_NOT_FOUND;
    }

    std::array methods = {
        ani_native_function { "createDiscretePathEffect", nullptr, reinterpret_cast<void*>(CreateDiscretePathEffect) },
        ani_native_function { "createSumPathEffect", nullptr, reinterpret_cast<void*>(CreateSumPathEffect) },
        ani_native_function { "createPathDashEffect", nullptr, reinterpret_cast<void*>(CreatePathDashEffect) },
        ani_native_function { "createComposePathEffect", nullptr, reinterpret_cast<void*>(CreateComposePathEffect) },
        ani_native_function { "createCornerPathEffect", nullptr, reinterpret_cast<void*>(CreateCornerPathEffect) },
        ani_native_function { "createDashPathEffect", nullptr, reinterpret_cast<void*>(CreateDashPathEffect) },
    };

    ret = env->Class_BindStaticNativeMethods(cls, methods.data(), methods.size());
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] bind methods fail: %{public}s", ANI_CLASS_PATHEFFECT_NAME);
        return ANI_NOT_FOUND;
    }

    return ANI_OK;
}

ani_object AniPathEffect::CreateDiscretePathEffect(ani_env* env, [[maybe_unused]]ani_object obj,
    ani_double aniSegLength, ani_double aniDev, ani_object aniSeedAssistObj)
{
    uint32_t seedAssist = 0;
    ani_boolean isUndefined;
    env->Reference_IsUndefined(aniSeedAssistObj, &isUndefined);
    if (!isUndefined) {
        ani_double aniSeedAssist = 0;
        ani_status ret =  env->Object_CallMethodByName_Double(aniSeedAssistObj, "toDouble", ":d", &aniSeedAssist);
        if (ret != ANI_OK) {
            ROSEN_LOGE("AniPathEffect::CreateDiscretePathEffect invalid param seedAssist: %{public}d", ret);
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param seedAssist.");
            return CreateAniUndefined(env);
        }
        seedAssist = static_cast<uint32_t>(aniSeedAssist);
    }

    AniPathEffect* pathEffect = new AniPathEffect(
        PathEffect::CreateDiscretePathEffect(aniSegLength, aniDev, seedAssist));
    ani_object aniObj = CreateAniObjectStatic(env, ANI_CLASS_PATHEFFECT_NAME, pathEffect);
    env->Reference_IsUndefined(aniObj, &isUndefined);
    if (isUndefined) {
        delete pathEffect;
        ROSEN_LOGE("AniPathEffect::CreateDiscretePathEffect failed cause aniObj is undefined");
    }
    return aniObj;
}

ani_object AniPathEffect::CreateSumPathEffect(ani_env* env, [[maybe_unused]]ani_object obj,
    ani_object aniFirstPathEffectObj, ani_object aniSecondPathEffectObj)
{
    auto aniFirstPathEffect = GetNativeFromObj<AniPathEffect>(env, aniFirstPathEffectObj);
    if (aniFirstPathEffect == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param firstPathEffect.");
        return CreateAniUndefined(env);
    }
    auto aniSecondPathEffect = GetNativeFromObj<AniPathEffect>(env, aniSecondPathEffectObj);
    if (aniSecondPathEffect == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param secondPathEffect.");
        return CreateAniUndefined(env);
    }
    AniPathEffect* pathEffect = new AniPathEffect(PathEffect::CreateSumPathEffect(
        *(aniFirstPathEffect->GetPathEffect()), *(aniSecondPathEffect->GetPathEffect())));
    ani_object aniObj = CreateAniObjectStatic(env, ANI_CLASS_PATHEFFECT_NAME, pathEffect);
    ani_boolean isUndefined;
    env->Reference_IsUndefined(aniObj, &isUndefined);
    if (isUndefined) {
        delete pathEffect;
        ROSEN_LOGE("AniPathEffect::CreateSumPathEffect failed cause aniObj is undefined");
    }
    return aniObj;
}

ani_object AniPathEffect::CreatePathDashEffect(ani_env* env, [[maybe_unused]]ani_object obj,
    ani_object aniPathObj, ani_double aniAdvance, ani_double aniPhase, ani_enum_item aniStyleEnum)
{
    auto aniPath = GetNativeFromObj<AniPath>(env, aniPathObj);
    if (aniPath == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param path.");
        return CreateAniUndefined(env);
    }
    ani_int aniStyle;
    ani_status ret = env->EnumItem_GetValue_Int(aniStyleEnum, &aniStyle);
    if (ret != ANI_OK) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param style.");
        return CreateAniUndefined(env);
    }
    AniPathEffect* pathEffect = new AniPathEffect(PathEffect::CreatePathDashEffect(
        aniPath->GetPath(), aniAdvance, aniPhase, static_cast<PathDashStyle>(aniStyle)));
    ani_object aniObj = CreateAniObjectStatic(env, ANI_CLASS_PATHEFFECT_NAME, pathEffect);
    ani_boolean isUndefined;
    env->Reference_IsUndefined(aniObj, &isUndefined);
    if (isUndefined) {
        delete pathEffect;
        ROSEN_LOGE("AniPathEffect::CreatePathDashEffect failed cause aniObj is undefined");
    }
    return aniObj;
}

ani_object AniPathEffect::CreateComposePathEffect(ani_env* env, [[maybe_unused]]ani_object obj,
    ani_object aniOuterPathEffectObj, ani_object aniInnerPathEffectObj)
{
    auto aniOuterPathEffect = GetNativeFromObj<AniPathEffect>(env, aniOuterPathEffectObj);
    if (aniOuterPathEffect == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param outer.");
        return CreateAniUndefined(env);
    }
    auto aniInnerPathEffect = GetNativeFromObj<AniPathEffect>(env, aniInnerPathEffectObj);
    if (aniInnerPathEffect == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param outer.");
        return CreateAniUndefined(env);
    }
    AniPathEffect* pathEffect = new AniPathEffect(PathEffect::CreateComposePathEffect(
        *(aniOuterPathEffect->GetPathEffect()), *(aniInnerPathEffect->GetPathEffect())));
    ani_object aniObj = CreateAniObjectStatic(env, ANI_CLASS_PATHEFFECT_NAME, pathEffect);
    ani_boolean isUndefined;
    env->Reference_IsUndefined(aniObj, &isUndefined);
    if (isUndefined) {
        delete pathEffect;
        ROSEN_LOGE("AniPathEffect::CreateComposePathEffect failed cause aniObj is undefined");
    }
    return aniObj;
}

ani_object AniPathEffect::CreateCornerPathEffect(
    ani_env* env, [[maybe_unused]]ani_object obj, ani_double aniRadius)
{
    AniPathEffect* pathEffect = new AniPathEffect(PathEffect::CreateCornerPathEffect(aniRadius));
    ani_object aniObj = CreateAniObjectStatic(env, ANI_CLASS_PATHEFFECT_NAME, pathEffect);
    ani_boolean isUndefined;
    env->Reference_IsUndefined(aniObj, &isUndefined);
    if (isUndefined) {
        delete pathEffect;
        ROSEN_LOGE("AniPathEffect::CreateCornerPathEffect failed cause aniObj is undefined");
    }
    return aniObj;
}

ani_object AniPathEffect::CreateDashPathEffect(
    ani_env* env, [[maybe_unused]]ani_object obj, ani_object aniIntervalsArray, ani_double aniPhase)
{
    ani_int aniLength;
    ani_status ret = env->Object_GetPropertyByName_Int(aniIntervalsArray, "length", &aniLength);
    if (ret != ANI_OK) {
        ROSEN_LOGE("AniPathEffect::CreateDashPathEffect aniIntervalsArray invalid %{public}d", ret);
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param intervals.");
        return CreateAniUndefined(env);
    }
    uint32_t intervalSize = static_cast<uint32_t>(aniLength);
    if ((intervalSize & 1) || (intervalSize <= 0)) { // intervalSize must be even and greater than 0;
        ROSEN_LOGD("AniPathEffect::CreateDashPathEffect intervalSize invalid %{public}u.", intervalSize);
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid intervals array length.");
        return CreateAniUndefined(env);
    }
    std::unique_ptr<float[]> intervals = std::make_unique<float[]>(intervalSize);
    for (uint32_t i = 0; i < intervalSize; i++) {
        ani_ref intervalRef;
        ani_double interval;
        ret = env->Object_CallMethodByName_Ref(
            aniIntervalsArray, "$_get", "i:Y", &intervalRef, static_cast<ani_int>(i));
        if (ret != ANI_OK) {
            ROSEN_LOGE("AniPathEffect::CreateDashPathEffect get intervalRef failed: %{public}d", ret);
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid intervals array length.");
            return CreateAniUndefined(env);
        }
        ret = env->Object_CallMethodByName_Double(static_cast<ani_object>(intervalRef), "toDouble", ":d", &interval);
        if (ret != ANI_OK) {
            ROSEN_LOGE("AniPathEffect::CreateDashPathEffect intervalRef get interval failed: %d", ret);
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid intervals array length.");
            return CreateAniUndefined(env);
        }
        intervals[i] = interval;
    }
    AniPathEffect* pathEffect = new AniPathEffect(PathEffect::CreateDashPathEffect(intervals.get(), intervalSize,
        aniPhase));
    ani_object aniObj = CreateAniObjectStatic(env, ANI_CLASS_PATHEFFECT_NAME, pathEffect);
    ani_boolean isUndefined;
    env->Reference_IsUndefined(aniObj, &isUndefined);
    if (isUndefined) {
        delete pathEffect;
        ROSEN_LOGE("AniPathEffect::CreateDashPathEffect failed cause aniObj is undefined");
    }
    return aniObj;
}

AniPathEffect::~AniPathEffect()
{
    pathEffect_ = nullptr;
}

std::shared_ptr<PathEffect> AniPathEffect::GetPathEffect()
{
    return pathEffect_;
}
} // namespace Drawing
} // namespace OHOS::Rosen
