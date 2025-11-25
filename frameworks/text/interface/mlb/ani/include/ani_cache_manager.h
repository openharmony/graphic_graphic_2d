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
#include <string_view>

#include "utils/text_log.h"

namespace OHOS::Text::ANI {
class AniCacher {
protected:
    ani_ref ref;

public:
    AniCacher(ani_env* env, ani_ref val) { env->GlobalReference_Create(val, &ref); }
    ani_ref get_ref() { return ref; }
};

struct CacheKey {
    std::string_view d;
    std::string_view n;
    std::string_view s;
    bool operator==(const CacheKey& other) const { return d == other.d && n == other.n && s == other.s; }
};

#define ANI_FIND_NAMESPACE(env, descriptor)                                                                            \
    ([] (ani_env* env, const char* desc) -> ani_namespace {                                                            \
        static AniCacher aniCacher(env, [](ani_env* e, const char* d) -> ani_namespace {                               \
            ani_namespace ns;                                                                                          \
            ani_status status = e->FindNamespace(d, &ns);                                                              \
            if (status != ANI_OK) {                                                                                    \
                TEXT_LOGE("Failed to find namespace: %{public}s, status %{public}d", d, status);                       \
                return nullptr;                                                                                        \
            }                                                                                                          \
            return ns;                                                                                                 \
        }(env, desc));                                                                                                 \
        return static_cast<ani_namespace>(aniCacher.get_ref());                                                        \
    }(env, descriptor))

#define ANI_FIND_CLASS(env, descriptor)                                                                                \
    ([] (ani_env* env, const char* desc) -> ani_class {                                                                \
        static AniCacher aniCacher(env, [](ani_env* e, const char* d) -> ani_class {                                   \
            ani_class cls;                                                                                             \
            ani_status status = e->FindClass(d, &cls);                                                                 \
            if (status != ANI_OK) {                                                                                    \
                TEXT_LOGE("Failed to find class: %{public}s, status %{public}d", d, status);                           \
                return nullptr;                                                                                        \
            }                                                                                                          \
            return cls;                                                                                                \
        }(env, desc));                                                                                                 \
        return static_cast<ani_class>(aniCacher.get_ref());                                                            \
    }(env, descriptor))

#define ANI_FIND_ENUM(env, descriptor)                                                                                 \
    ([] (ani_env* env, const char* desc) -> ani_enum {                                                                 \
        static AniCacher aniCacher(env, [](ani_env* e, const char* d) -> ani_enum {                                    \
            ani_enum enumObj;                                                                                          \
            ani_status status = e->FindEnum(d, &enumObj);                                                              \
            if (status != ANI_OK) {                                                                                    \
                TEXT_LOGE("Failed to find enum: %{public}s, status %{public}d", d, status);                            \
                return nullptr;                                                                                        \
            }                                                                                                          \
            return enumObj;                                                                                            \
        }(env, desc));                                                                                                 \
        return static_cast<ani_enum>(aniCacher.get_ref());                                                             \
    }(env, descriptor))

#define ANI_CLASS_FIND_METHOD(env, key)                                                                                \
    ([] (ani_env* env, const CacheKey& key) -> ani_method {                                                            \
        static ani_method __method = [](ani_env* env, const CacheKey& key) -> ani_method {                             \
            ani_class cls = ANI_FIND_CLASS(env, std::string(key.d).c_str());                                           \
            if (cls == nullptr) {                                                                                      \
                return nullptr;                                                                                        \
            }                                                                                                          \
            ani_method method;                                                                                         \
            ani_status status =                                                                                        \
                env->Class_FindMethod(cls, std::string(key.n).c_str(), std::string(key.s).c_str(), &method);           \
            if (status != ANI_OK) {                                                                                    \
                TEXT_LOGE("Failed to find method: %{public}s::%{public}s::%{public}s, status %{public}d",              \
                    std::string(key.d).c_str(), std::string(key.n).c_str(), std::string(key.s).c_str(), status);       \
                return nullptr;                                                                                        \
            }                                                                                                          \
            return method;                                                                                             \
        }(env, key);                                                                                                   \
        return __method;                                                                                               \
    }(env, key))

#define ANI_NAMESPACE_FIND_FUNCTION(env, key)                                                                          \
    ([] (ani_env* env, const CacheKey& key) -> ani_function {                                                          \
        static ani_function __function = [](ani_env* env, const CacheKey& key) -> ani_function {                       \
            ani_namespace ns = ANI_FIND_NAMESPACE(env, std::string(key.d).c_str());                                    \
            if (ns == nullptr) {                                                                                       \
                return nullptr;                                                                                        \
            }                                                                                                          \
            ani_function function;                                                                                     \
            ani_status status =                                                                                        \
                env->Namespace_FindFunction(ns, std::string(key.n).c_str(), std::string(key.s).c_str(), &function);    \
            if (status != ANI_OK) {                                                                                    \
                TEXT_LOGE("Failed to find function: %{public}s::%{public}s::%{public}s, status %{public}d",            \
                    std::string(key.d).c_str(), std::string(key.n).c_str(), std::string(key.s).c_str(), status);       \
                return nullptr;                                                                                        \
            }                                                                                                          \
            return function;                                                                                           \
        }(env, key);                                                                                                   \
        return __function;                                                                                             \
    }(env, key))
} // namespace OHOS::Text::ANI
#endif // OHOS_TEXT_ANI_CACHE_MANAGER_H