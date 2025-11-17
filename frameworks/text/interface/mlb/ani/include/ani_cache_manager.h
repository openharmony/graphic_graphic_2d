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
    calc(ani_env* env, ani_ref val) { env->GlobalReference_Create(val, &ref); }
    ani_ref get_ref() { return ref; }
};

#define ANI_FIND_NAMESPACE(env, descriptor)                                                                            \
    ([&] {                                                                                                             \
        static calc __calc(env, [&]() -> ani_namespace {                                                               \
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

#define ANI_FIND_CLASS(env, descriptor)                                                                                \
    ([&] {                                                                                                             \
        static calc __calc(env, [&]() -> ani_class {                                                                   \
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

#define ANI_FIND_ENUM(env, descriptor)                                                                                \
    ([&] {                                                                                                             \
        static calc __calc(env, [&]() -> ani_enum {                                                                   \
            ani_enum enumObj;                                                                                             \
            ani_status status = env->FindEnum(descriptor, &enumObj);                                                      \
            if (status != ANI_OK) {                                                                                    \
                TEXT_LOGE("Failed to find enum: %{public}s, status %{public}d", descriptor, status);                  \
                return nullptr;                                                                                        \
            }                                                                                                          \
            return enumObj;                                                                                            \
        }());                                                                                                          \
        return static_cast<ani_enum>(__calc.get_ref());                                                               \
    }())

#define ANI_CLASS_FIND_METHOD(env, descriptor, name, signature)                                                        \
    ([&] {                                                                                                             \
        static ani_method __method = [&]() -> ani_method {                                                             \
            ani_class cls = ANI_FIND_CLASS(env, descriptor);                                                           \
            if (cls == nullptr) {                                                                                      \
                return nullptr;                                                                                        \
            }                                                                                                          \
            ani_method method;                                                                                         \
            ani_status status = env->Class_FindMethod(cls, name, signature, &method);                                  \
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
} // namespace OHOS::Text::ANI
#endif // OHOS_TEXT_ANI_CACHE_MANAGER_H