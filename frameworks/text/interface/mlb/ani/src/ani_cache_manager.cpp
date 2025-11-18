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
class AniCacher {
protected:
    ani_ref ref;

public:
    AniCacher(ani_env* env, ani_ref val) { env->GlobalReference_Create(val, &ref); }
    ani_ref get_ref() { return ref; }
};

ani_namespace ANIFindNamespace(ani_env* env, const char* descriptor)
{
    static AniCacher aniCacher(env, [env, descriptor]() -> ani_namespace {
        ani_namespace ns;
        ani_status status = env->FindNamespace(descriptor, &ns);
        if (status != ANI_OK) {
            TEXT_LOGE("Failed to find namespace: %{public}s, status %{public}d", descriptor, status);
            return nullptr;
        }
        return ns;
    }());
    return static_cast<ani_namespace>(aniCacher.get_ref());
}

ani_class ANIFindClass(ani_env* env, const char* descriptor)
{
    static AniCacher aniCacher(env, [env, descriptor]() -> ani_class {
        ani_class cls;
        ani_status status = env->FindClass(descriptor, &cls);
        if (status != ANI_OK) {
            TEXT_LOGE("Failed to find class: %{public}s, status %{public}d", descriptor, status);
            return nullptr;
        }
        return cls;
    }());
    return static_cast<ani_class>(aniCacher.get_ref());
}

ani_enum ANIFindEnum(ani_env* env, const char* descriptor)
{
    static AniCacher aniCacher(env, [env, descriptor]() -> ani_enum {
        ani_enum enumObj;
        ani_status status = env->FindEnum(descriptor, &enumObj);
        if (status != ANI_OK) {
            TEXT_LOGE("Failed to find enum: %{public}s, status %{public}d", descriptor, status);
            return nullptr;
        }
        return enumObj;
    }());
    return static_cast<ani_enum>(aniCacher.get_ref());
}

ani_method ANIClassFindMethod(ani_env* env, const char* descriptor, const char* name, const char* signature)
{
    static ani_method method = [env, descriptor, name, signature]() -> ani_method {
        ani_class cls = ANIFindClass(env, descriptor);
        if (cls == nullptr) {
            return nullptr;
        }
        ani_method method;
        ani_status status = env->Class_FindMethod(cls, name, signature, &method);
        if (status != ANI_OK) {
            TEXT_LOGE("Failed to find method: %{public}s::%{public}s::%{public}s, status %{public}d",
                      descriptor, name, signature, status);
            return nullptr;
        }
        return method;
    }();
    return method;
}

ani_function ANINamespaceFindFunction(ani_env* env, const char* descriptor, const char* name, const char* signature)
{
    static ani_function function = [env, descriptor, name, signature]() -> ani_function {
        ani_namespace ns = ANIFindNamespace(env, descriptor);
        if (ns == nullptr) {
            return nullptr;
        }
        ani_function function;
        ani_status status = env->Namespace_FindFunction(ns, name, signature, &function);
        if (status != ANI_OK) {
            TEXT_LOGE("Failed to find function: %{public}s::%{public}s::%{public}s, status %{public}d",
                      descriptor, name, signature, status);
            return nullptr;
        }
        return function;
    }();
    return function;
}
} // namespace OHOS::Text::ANI