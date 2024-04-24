/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef OHOS_ROSEN_JS_PATH_EFFECT_H
#define OHOS_ROSEN_JS_PATH_EFFECT_H

#include <native_engine/native_engine.h>
#include <native_engine/native_value.h>
#include "effect/path_effect.h"

namespace OHOS::Rosen {
namespace Drawing {
class JsPathEffect final {
public:
    explicit JsPathEffect(std::shared_ptr<PathEffect> pathEffect = nullptr) : m_pathEffect(pathEffect) {}
    ~JsPathEffect();

    static napi_value Init(napi_env env, napi_value exportObj);
    static void Finalizer(napi_env env, void* data, void* hint);
    static napi_value Constructor(napi_env env, napi_callback_info info);
    static void Destructor(napi_env env, void *nativeObject, void *finalize);

    static napi_value CreateDashPathEffect(napi_env env, napi_callback_info info);
    std::shared_ptr<PathEffect> GetPathEffect();

private:
    static napi_value Create(napi_env env, std::shared_ptr<PathEffect> pathEffect);
    std::shared_ptr<PathEffect> m_pathEffect = nullptr;
    static thread_local napi_ref constructor_;
};
} // namespace Drawing
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_JS_PATH_EFFECT_H
