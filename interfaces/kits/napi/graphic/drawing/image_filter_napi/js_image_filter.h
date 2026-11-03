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

#ifndef OHOS_ROSEN_JS_IMAGE_FILTER_H
#define OHOS_ROSEN_JS_IMAGE_FILTER_H

#include <native_engine/native_engine.h>
#include <native_engine/native_value.h>

#include "color_filter_napi/js_color_filter.h"
#include "effect/image_filter.h"
#include "effect/shader_effect.h"
#include "shader_effect_napi/js_shader_effect.h"

namespace OHOS::Rosen {
namespace Drawing {
class JsImageFilter final {
public:
    explicit JsImageFilter(std::shared_ptr<ImageFilter> imageFilter = nullptr) : m_ImageFilter(imageFilter) {}
    ~JsImageFilter();

    static napi_value Init(napi_env env, napi_value exportObj);
    static void Finalizer(napi_env env, void* data, void* hint);
    static napi_value Constructor(napi_env env, napi_callback_info info);
    static void Destructor(napi_env env, void *nativeObject, void *finalize);
    static napi_value CreateBlurImageFilter(napi_env env, napi_callback_info info);
    static napi_value CreateBlendImageFilter(napi_env env, napi_callback_info info);
    static napi_value CreateComposeImageFilter(napi_env env, napi_callback_info info);
    static napi_value CreateFromColorFilter(napi_env env, napi_callback_info info);
    static napi_value CreateFromImage(napi_env env, napi_callback_info info);
    static napi_value CreateFromShaderEffect(napi_env env, napi_callback_info info);
    static napi_value CreateOffsetImageFilter(napi_env env, napi_callback_info info);
    static napi_value Create(napi_env env, const std::shared_ptr<ImageFilter> imageFilter);
    DRAWING_API std::shared_ptr<ImageFilter> GetImageFilter();

private:
    std::shared_ptr<ImageFilter> m_ImageFilter = nullptr;
    static thread_local napi_ref constructor_;
};
} // namespace Drawing
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_JS_IMAGE_FILTER_H
