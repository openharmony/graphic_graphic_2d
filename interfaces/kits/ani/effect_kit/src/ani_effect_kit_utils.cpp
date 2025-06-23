/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#include "ani_effect_kit_utils.h"
#include "effect_utils.h"

namespace OHOS {
namespace Rosen {

ani_object AniEffectKitUtils::CreateAniObject(
    ani_env* env, const char* className, const char* methodSig, ani_long object)
{
    EFFECT_LOG_E("[EFFECT_KIT_DEBUG] CreateAniObject: Start");
    ani_class cls;
    if (env->FindClass(className, &cls) != ANI_OK) {
        EFFECT_LOG_E("Not found class");
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    ani_method ctor;
    if (env->Class_FindMethod(cls, "<ctor>", methodSig, &ctor) != ANI_OK) {
        EFFECT_LOG_E("Not found ani_method");
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    ani_object aniValue;
    if (env->Object_New(cls, ctor, &aniValue, object) != ANI_OK) {
        EFFECT_LOG_E("New Context Failed");
        return AniEffectKitUtils::CreateAniUndefined(env);
    }
    EFFECT_LOG_E("[EFFECT_KIT_DEBUG] CreateAniObject: End");
    return aniValue;
}

ani_object AniEffectKitUtils::CreateAniUndefined(ani_env* env)
{
    ani_ref aniRef;
    env->GetUndefined(&aniRef);
    return static_cast<ani_object>(aniRef);
}

AniFilter* AniEffectKitUtils::GetFilterFromEnv([[maybe_unused]] ani_env* env, [[maybe_unused]] ani_object obj)
{
    ani_status ret;
    ani_long nativeObj {};
    if ((ret = env->Object_GetFieldByName_Long(obj, "nativeObj", &nativeObj)) != ANI_OK) {
        EFFECT_LOG_E("Object_GetField_Long fetch failed, %{public}d", ret);
        return nullptr;
    }
    AniFilter* aniFilter = reinterpret_cast<AniFilter*>(nativeObj);
    if (!aniFilter) {
        EFFECT_LOG_E("filter is null");
        return nullptr;
    }
    return aniFilter;
}

AniColorPicker* AniEffectKitUtils::GetColorPickerFromEnv([[maybe_unused]] ani_env* env, [[maybe_unused]] ani_object obj)
{
    EFFECT_LOG_E("[EFFECT_KIT_DEBUG] GetColorPickerFromEnv Start");
    ani_status ret;
    ani_long nativeObj {};
    // 从 JS 对象中获取 nativeObj 字段（存储 C++ 对象指针）
    if ((ret = env->Object_GetFieldByName_Long(obj, "nativeObj", &nativeObj)) != ANI_OK) {
        EFFECT_LOG_E("Object_GetField_Long fetch failed, %{public}d", ret);
        return nullptr;
    }
    EFFECT_LOG_E("[EFFECT_KIT_DEBUG] GetColorPickerFromEnv: nativePtr value");
    // 将指针转换为 AniColorPicker 类型
    AniColorPicker* colorPicker = reinterpret_cast<AniColorPicker*>(nativeObj);
    if (!colorPicker) {
        EFFECT_LOG_E("AniColorPicker instance is null");
        return nullptr;
    }
    return colorPicker;
}
} // namespace Rosen
} // namespace OHOS