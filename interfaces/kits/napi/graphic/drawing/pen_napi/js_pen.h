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

#ifndef OHOS_ROSEN_JS_PEN_H
#define OHOS_ROSEN_JS_PEN_H

#include <native_engine/native_engine.h>
#include <native_engine/native_value.h>

#include "draw/pen.h"

namespace OHOS::Rosen {
namespace Drawing {
class JsPen final {
public:
    JsPen();
    ~JsPen();

    static napi_value Init(napi_env env, napi_value exportObj);
    static napi_value Constructor(napi_env env, napi_callback_info info);
    static void Destructor(napi_env env, void* nativeObject, void* finalize);

    static napi_value SetColor(napi_env env, napi_callback_info info);
    static napi_value SetStrokeWidth(napi_env env, napi_callback_info info);
    static napi_value SetAntiAlias(napi_env env, napi_callback_info info);
    static napi_value SetAlpha(napi_env env, napi_callback_info info);
    static napi_value SetColorFilter(napi_env env, napi_callback_info info);
    static napi_value SetMaskFilter(napi_env env, napi_callback_info info);
    static napi_value SetBlendMode(napi_env env, napi_callback_info info);
    static napi_value SetDither(napi_env env, napi_callback_info info);
    static napi_value SetJoinStyle(napi_env env, napi_callback_info info);
    static napi_value GetJoinStyle(napi_env env, napi_callback_info info);
    static napi_value SetCapStyle(napi_env env, napi_callback_info info);
    static napi_value GetCapStyle(napi_env env, napi_callback_info info);
    static napi_value SetPathEffect(napi_env env, napi_callback_info info);
    static napi_value SetShadowLayer(napi_env env, napi_callback_info info);

    Pen* GetPen();

private:
    static thread_local napi_ref constructor_;

    Pen* pen_;
};
} // namespace Drawing
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_JS_PEN_H