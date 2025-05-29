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

#ifndef OHOS_ROSEN_JS_SHADER_EFFECT_H
#define OHOS_ROSEN_JS_SHADER_EFFECT_H

#include <native_engine/native_engine.h>
#include <native_engine/native_value.h>

#include "effect/shader_effect.h"

namespace OHOS::Rosen {
namespace Drawing {
class JsShaderEffect final {
public:
    explicit JsShaderEffect(std::shared_ptr<ShaderEffect> shaderEffect = nullptr) : m_shaderEffect(shaderEffect) {}
    ~JsShaderEffect();

    static napi_value Init(napi_env env, napi_value exportObj);
    static napi_value Constructor(napi_env env, napi_callback_info info);
    static void Destructor(napi_env env, void* nativeObject, void* finalize);
    static void Finalizer(napi_env env, void* data, void* hint);

    static napi_value CreateColorShader(napi_env env, napi_callback_info info);
    static napi_value CreateLinearGradient(napi_env env, napi_callback_info info);
    static napi_value CreateRadialGradient(napi_env env, napi_callback_info info);
    static napi_value CreateSweepGradient(napi_env env, napi_callback_info info);
    static napi_value CreateComposeShader(napi_env env, napi_callback_info info);
    static napi_value CreateConicalGradient(napi_env env, napi_callback_info info);
    static napi_value CreateImageShader(napi_env env, napi_callback_info info);
    std::shared_ptr<ShaderEffect> GetShaderEffect();

private:
    static napi_value Create(napi_env env, const std::shared_ptr<ShaderEffect> gradient);
    static thread_local napi_ref constructor_;
    std::shared_ptr<ShaderEffect> m_shaderEffect = nullptr;
};
} // namespace Drawing
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_JS_SHADER_EFFECT_H