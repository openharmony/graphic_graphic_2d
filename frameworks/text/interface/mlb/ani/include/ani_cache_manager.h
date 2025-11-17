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

#ifndef OHOS_TEXT_ANI_CACHE_MANAGER_H
#define OHOS_TEXT_ANI_CACHE_MANAGER_H

#include <ani.h>
#include <memory>
#include <shared_mutex>
#include <string>
#include <string_view>
#include <unordered_map>

#include "utils/text_log.h"

namespace OHOS::Text::ANI {
class calc {
protected:
    ani_ref ref;

public:
    calc(ani_env* env, ani_ref val){env->GlobalReference_Create(val, &ref)};
    ani_ref get_ref() { return ref; }
}

#define ANI_FIND_NAMESPACE(env, descriptor)                                                                            \
    ([&] {                                                                                                             \
        static calc __calc(env, []() -> ani_namespace {                                                                \
            ani_namespace ns;                                                                                          \
            ani_status status = env->FindNamespace(descriptor, &ns);                                                   \
            if (status != ANI_OK) {                                                                                    \
                TEXT_LOGE("Failed to find namespace: %{public}s, status %{public}d", descriptor, status);              \
                return nullptr;                                                                                        \
            }                                                                                                          \
            return ns;                                                                                                 \
        }());                                                                                                          \
        return static_cast<ani_namespace>(__calc.get_ref());                                                           \
    }())

#define ANI_CLASS(env, descriptor)                                                                                     \
    ([&] {                                                                                                             \
        static calc __calc(env, []() -> ani_class {                                                                    \
            ani_class cls;                                                                                             \
            ani_status status = env->FindClass(descriptor, &cls);                                                      \
            if (status != ANI_OK) {                                                                                    \
                TEXT_LOGE("Failed to find class: %{public}s, status %{public}d", descriptor, status);                  \
                return nullptr;                                                                                        \
            }                                                                                                          \
            return cls;                                                                                                \
        }());                                                                                                          \
        return static_cast<ani_class>(__calc.get_ref());                                                               \
    }())

#define ANI_CLASS_FIND_METHOD(env, descriptor, name, signature)                                                        \
    ([&] {                                                                                                             \
        static ani_method __method = [&]() -> ani_method {                                                             \
            ani_class cls = ANI_FIND_CLASS(env, descriptor);                                                           \
            if (cls == nullptr) {                                                                                      \
                return nullptr;                                                                                        \
            }                                                                                                          \
            ani_method method;                                                                                         \
            ani_status status = env->CLASS_FindMethod(cls, name, signature, &method);                                  \
            if (status != ANI_OK) {                                                                                    \
                TEXT_LOGE("Failed to find method: %{public}s::%{public}s::%{public}s, status %{public}d", descriptor,  \
                    name, signature, status);                                                                          \
                return nullptr;                                                                                        \
            }                                                                                                          \
            return method;                                                                                             \
        }();                                                                                                           \
        return __method;                                                                                               \
    }())

#define ANI_NAMESPACE_FIND_FUNCTION(env, descriptor, name, signature)                                                  \
    ([&] {                                                                                                             \
        static ani_function __function = [&]() -> ani_function {                                                       \
            ani_namespace ns = ANI_FIND_NAMESPACE(env, descriptor);                                                    \
            if (ns == nullptr) {                                                                                       \
                return nullptr;                                                                                        \
            }                                                                                                          \
            ani_function function;                                                                                     \
            ani_status status = env->Namespace_FindFunction(ns, name, signature, &function);                           \
            if (status != ANI_OK) {                                                                                    \
                TEXT_LOGE("Failed to find function: %{public}s::%{public}s::%{public}s, status %{public}d",            \
                    descriptor, name, signature, status);                                                              \
                return nullptr;                                                                                        \
            }                                                                                                          \
            return function;                                                                                           \
        }();                                                                                                           \
        return __function;                                                                                             \
    }())

struct AniCacheParam {
    std::string cacheKey{""};
    const char* clsName{nullptr};
    const char* methodName{nullptr};
    const char* signature{nullptr};

    AniCacheParam(const char* cls, const char* method, const char* sig)
        : clsName(cls), methodName(method), signature(sig)
    {
        if (IsValid()) {
            BuildCacheKey();
        }
    }

    bool IsValid() const
    {
        return clsName != nullptr && methodName != nullptr && signature != nullptr;
    }

    const char* GetCacheKey() const
    {
        return cacheKey.c_str();
    }

private:
    void BuildCacheKey()
    {
        cacheKey.clear();
        size_t clsLen = std::strlen(clsName);
        size_t methodLen = std::strlen(methodName);
        size_t sigLen = std::strlen(signature);
        cacheKey.reserve(clsLen + methodLen + sigLen);
        cacheKey.append(clsName, clsLen);
        cacheKey.append(methodName, methodLen);
        cacheKey.append(signature, sigLen);
    }
};

class AniCacheManager {
public:
    static AniCacheManager& Instance()
    {
        static AniCacheManager instance;
        return instance;
    }

    void InsertNamespace(ani_env* env, const std::string& key, ani_namespace ns);
    bool FindNamespace(const std::string& key, ani_namespace& ns);
    void InsertClass(ani_env* env, const std::string& key, ani_class cls);
    bool FindClass(const std::string& key, ani_class& cls);
    void InsertMethod(const std::string& key, ani_method method);
    bool FindMethod(const std::string& key, ani_method& method);
    void InsertFunction(const std::string& key, ani_function method);
    bool FindFunction(const std::string& key, ani_function& method);
    void InsertEnum(ani_env* env, const std::string& key, ani_enum enumType);
    bool FindEnum(const std::string& key, ani_enum& enumType);
    void Clear(ani_env* env);

private:
    AniCacheManager() = default;
    ~AniCacheManager() = default;
    AniCacheManager(AniCacheManager&&) = delete;
    AniCacheManager& operator=(AniCacheManager&&) = delete;
    AniCacheManager(const AniCacheManager&) = delete;
    AniCacheManager& operator=(const AniCacheManager&) = delete;

    std::unordered_map<std::string, ani_ref> nsCache_;
    std::unordered_map<std::string, ani_ref> clsCache_;
    std::unordered_map<std::string, ani_method> methodCache_;
    std::unordered_map<std::string, ani_function> functionCache_;
    std::unordered_map<std::string, ani_ref> enumCache_;
    mutable std::shared_mutex nsMutex_;
    mutable std::shared_mutex clsMutex_;
    mutable std::shared_mutex methodMutex_;
    mutable std::shared_mutex functionMutex_;
    mutable std::shared_mutex enumMutex_;
};
} // namespace OHOS::Text::ANI
#endif // OHOS_TEXT_ANI_CACHE_MANAGER_H