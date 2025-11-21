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
ani_ref ToGlobalRef(ani_env* env, ani_ref local)
{
    ani_ref global = nullptr;
    ani_status status = env->GlobalReference_Create(local, &global);
    if (status != ANI_OK) {
        TEXT_LOGE("Failed to create global reference");
    }
    return global;
}
ani_namespace AniFindNamespace(ani_env* env, const char* descriptor)
{
    {
        std::shared_lock<std::shared_mutex> lock(g_nsCache.mtx);
        auto it = g_nsCache.store.find(descriptor);
        if (it != g_nsCache.store.end()) {
            return static_cast<ani_namespace>(it->second);
        }
    }

    ani_namespace ns = nullptr;
    ani_status status = env->FindNamespace(descriptor, &ns);
    if (status != ANI_OK) {
        TEXT_LOGE("Failed to find namespace: %{public}s, status: %{public}d", descriptor, status);
        return nullptr;
    }

    ani_ref global = ToGlobalRef(env, ns);
    {
        std::unique_lock<std::shared_mutex> lock(g_nsCache.mtx);
        g_nsCache.store.emplace(descriptor, global);
    }
    return ns;
}

ani_class AniFindClass(ani_env* env, const char* descriptor)
{
    {
        std::shared_lock<std::shared_mutex> lock(g_classCache.mtx);
        auto it = g_classCache.store.find(descriptor);
        if (it != g_classCache.store.end()) {
            return static_cast<ani_class>(it->second);
        }
    }

    ani_class cls = nullptr;
    ani_status status = env->FindClass(descriptor, &cls);
    if (status != ANI_OK) {
        TEXT_LOGE("Failed to find class: %{public}s, status: %{public}d", descriptor, status);
        return nullptr;
    }

    ani_ref global = ToGlobalRef(env, cls);
    {
        std::unique_lock<std::shared_mutex> lock(g_classCache.mtx);
        g_classCache.store.emplace(descriptor, global);
    }
    return cls;
}

ani_enum AniFindEnum(ani_env* env, const char* descriptor)
{
    {
        std::shared_lock<std::shared_mutex> lock(g_enumCache.mtx);
        auto it = g_enumCache.store.find(descriptor);
        if (it != g_enumCache.store.end()) {
            return static_cast<ani_enum>(it->second);
        }
    }

    ani_enum en = nullptr;
    ani_status status = env->FindEnum(descriptor, &en);
    if (status != ANI_OK) {
        TEXT_LOGE("Failed to find enum: %{public}s, status: %{public}d", descriptor, status);
        return nullptr;
    }

    ani_ref global = ToGlobalRef(env, en);
    {
        std::unique_lock<std::shared_mutex> lock(g_enumCache.mtx);
        g_enumCache.store.emplace(descriptor, global);
    }
    return en;
}

ani_method AniClassFindMethod(ani_env* env, const CacheKey& key)
{
    {
        std::shared_lock<std::shared_mutex> lock(g_methodMutex);
        auto it = g_methodCache.find(key);
        if (it != g_methodCache.end()) {
            return it->second;
        }
    }

    ani_class cls = AniFindClass(env, std::string(key.d).c_str());
    if (cls == nullptr) {
        return nullptr;
    }

    ani_method method = nullptr;
    ani_status status = env->Class_FindMethod(cls, std::string(key.n).c_str(), std::string(key.s).c_str(), &method);
    if (status != ANI_OK) {
        TEXT_LOGE("Failed to find method: %{public}s %{public}s %{public}s, status: %{public}d",
            std::string(key.d).c_str(), std::string(key.n).c_str(), std::string(key.s).c_str(), status);
        return nullptr;
    }

    {
        std::unique_lock<std::shared_mutex> lock(g_methodMutex);
        g_methodCache.emplace(key, method);
    }
    return method;
}

ani_function AniNamespaceFindFunction(ani_env* env, const CacheKey& key)
{
    {
        std::shared_lock<std::shared_mutex> lock(g_functionMutex);
        auto it = g_functionCache.find(key);
        if (it != g_functionCache.end()) {
            return it->second;
        }
    }

    ani_namespace ns = AniFindNamespace(env, std::string(key.d).c_str());
    if (ns == nullptr) {
        return nullptr;
    }

    ani_function function = nullptr;
    ani_status status = env->Namespace_FindFunction(
        ns, std::string(key.n).c_str(), std::string(key.s).c_str(), &function);
    if (status != ANI_OK) {
        TEXT_LOGE("Failed to find function: %{public}s %{public}s %{public}s, status: %{public}d",
            std::string(key.d).c_str(), std::string(key.n).c_str(), std::string(key.s).c_str(), status);
        return nullptr;
    }

    {
        std::unique_lock<std::shared_mutex> lock(g_functionMutex);
        g_functionCache.emplace(key, function);
    }
    return function;
}
} // namespace OHOS::Text::ANI