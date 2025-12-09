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

#include "ani_cache_manager.h"

#include "utils/text_log.h"

namespace OHOS::Text::ANI {
ani_namespace AniFindNamespace(ani_env* env, const char* descriptor)
{
    ani_namespace ns = nullptr;
    ani_status status = env->FindNamespace(descriptor, &ns);
    if (status != ANI_OK) {
        TEXT_LOGE("Failed to find namespace: %{public}s, status %{public}d", descriptor, status);
        return nullptr;
    }
    ani_ref ref = nullptr;
    status = env->GlobalReference_Create(ns, &ref);
    if (status != ANI_OK) {
        TEXT_LOGE("Failed to create global reference for namespace: %{public}s, status %{public}d", descriptor, status);
        return nullptr;
    }
    return static_cast<ani_namespace>(ref);
}

ani_class AniFindClass(ani_env* env, const char* descriptor)
{
    ani_class cls = nullptr;
    ani_status status = env->FindClass(descriptor, &cls);
    if (status != ANI_OK) {
        TEXT_LOGE("Failed to find class: %{public}s, status %{public}d", descriptor, status);
        return nullptr;
    }
    ani_ref ref = nullptr;
    status = env->GlobalReference_Create(cls, &ref);
    if (status != ANI_OK) {
        TEXT_LOGE("Failed to create global reference for class: %{public}s, status %{public}d", descriptor, status);
        return nullptr;
    }
    return static_cast<ani_class>(ref);
}

ani_enum AniFindEnum(ani_env* env, const char* descriptor)
{
    ani_enum enumObj = nullptr;
    ani_status status = env->FindEnum(descriptor, &enumObj);
    if (status != ANI_OK) {
        TEXT_LOGE("Failed to find enum: %{public}s, status %{public}d", descriptor, status);
        return nullptr;
    }
    ani_ref ref = nullptr;
    status = env->GlobalReference_Create(enumObj, &ref);
    if (status != ANI_OK) {
        TEXT_LOGE("Failed to create global reference for enum: %{public}s, status %{public}d", descriptor, status);
        return nullptr;
    }
    return static_cast<ani_enum>(ref);
}

ani_method AniClassFindMethod(ani_env* env, const ani_class cls, const CacheKey& key)
{
    ani_method method = nullptr;
    ani_status status = env->Class_FindMethod(cls, std::string(key.n).c_str(), std::string(key.s).c_str(), &method);
    if (status != ANI_OK) {
        TEXT_LOGE("Failed to find method: %{public}s %{public}s %{public}s, status %{public}d",
            std::string(key.d).c_str(), std::string(key.n).c_str(), std::string(key.s).c_str(), status);
        return nullptr;
    }
    return method;
}

ani_function AniNamespaceFindFunction(ani_env* env, const ani_namespace ns, const CacheKey& key)
{
    ani_function function = nullptr;
    ani_status status =
        env->Namespace_FindFunction(ns, std::string(key.n).c_str(), std::string(key.s).c_str(), &function);
    if (status != ANI_OK) {
        TEXT_LOGE("Failed to find function: %{public}s %{public}s %{public}s, status %{public}d",
            std::string(key.d).c_str(), std::string(key.n).c_str(), std::string(key.s).c_str(), status);
        return nullptr;
    }
    return function;
}
} // namespace OHOS::Text::ANI